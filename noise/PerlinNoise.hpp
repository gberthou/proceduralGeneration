#ifndef PERLIN_NOISE_HPP
#define PERLIN_NOISE_HPP

#include <vector>
#include <array>
#include <cmath>

#include "../random/RandomEngine.hpp"
#include "../core/Incrementable.hpp"

namespace pg
{
    template<typename T, template<typename> class Dist, size_t DIM>
    class PerlinNoiseTile
    {
        public:
            typedef std::array<T, DIM> Tuple;

            PerlinNoiseTile(pg::NumberGenerator &generator,
                            const pg::Distribution<T, Dist> &distribution,
                            const std::array<size_t, DIM> &dim,
                            const pg::Incrementable<PerlinNoiseTile<T, Dist, DIM>, DIM> &noise,
                            const std::array<int, DIM> &coord):
                engine(generator, distribution),
                dimensions(increaseArray(dim)),
                grid(gridSize())
            {
                for(size_t i = 0; i < grid.size(); ++i)
                    grid[i] = generateVector();

                // Import vectors for neighbors
                for(size_t i = 0; i < DIM; ++i)
                {
                    for(size_t j = 0; j < 2; ++j)
                    {
                        const PerlinNoiseTile<T, Dist, DIM> *tile;
                        std::array<int, DIM> c = coord;
                        c[i] += (j == 0 ? 1 : -1);
                        if(noise.HasTile(c, tile))
                        {
                            for(size_t k = 0; k < dimensions[i]; ++k)
                            {

                            }
                        }
                    }
                }
            }

            virtual ~PerlinNoiseTile() = default;

            /* input must be in range [0, dimension.i[
             * output in range [0, 1]
             */
            T operator()(const Tuple &point) const
            {
                std::array<uint8_t, DIM> base;
                // No need to initialize the array since
                // computeLocalContribution already does it
                return (computeLocalContribution(point, base, 0) + 1) / 2.;
            }

        protected:

            size_t gridSize() const
            {
                size_t ret = 1;
                for(size_t x : dimensions)
                    ret *= x;
                return ret;
            }

            /* Generates a scalar in range [0, 1] */
            inline T generateScalar()
            {
                return (engine() - engine.min())/(engine.max() - engine.min());
            }

            Tuple generateVector()
            {
                Tuple ret;
                T sum2 = 0;
                for(size_t i = 0; i + 1 < DIM; ++i)
                {
                    // Generate number in range [-1,1]
                    ret[i] = 2 * generateScalar() - 1;
                    sum2 += ret[i] * ret[i];
                }

                // Now compute last vector so that length cannot be zero
                const T EPSILON2 = 0.0001;
                if(sum2 < EPSILON2)
                {
                    ret[DIM - 1] = std::exp(engine()); // Always strictly
                                                       // positive
                    if(engine() < 0.5) // 50% numbers will be negative
                        ret[DIM - 1] *= -1;
                }
                else
                    ret[DIM - 1] = 2 * generateScalar() - 1;
                sum2 += ret[DIM - 1] * ret[DIM - 1];

                T length = std::sqrt(sum2); // Should be always strictly
                                            // positive
                for(size_t i = 0; i < DIM; ++i)
                    ret[i] /= length;

                return ret;
            }

            size_t indexAt(const std::array<size_t, DIM> &indices) const
            {
                size_t index = indices[DIM - 1];
                for(size_t i = DIM - 1; --i + 1; )
                    index = index * dimensions[i] + indices[i];
                return index;
            }

            const Tuple &at(const std::array<size_t, DIM> &indices) const
            {
                return grid[indexAt(indices)];
            }
            
            Tuple &at(const std::array<size_t, DIM> &indices)
            {
                return grid[indexAt(indices)];
            }

            T contribution(const Tuple &point,
                           const std::array<uint8_t, DIM> &relativeRef) const
            {
                std::array<size_t, DIM> absoluteRef;
                for(size_t i = 0; i < DIM; ++i)
                    absoluteRef[i] = std::floor(point[i] * dimensions[i]) + relativeRef[i];

                const Tuple &tuple = at(absoluteRef);
                T product = 0;
                for(size_t i = 0; i < DIM; ++i)
                {
                    product += (fractionalPart(point[i] * dimensions[i]) - relativeRef[i])
                             * tuple[i];
                }
                return product;
            }

            /* x must be non-negative */
            static inline T fractionalPart(T x)
            {
                return x - std::floor(x);
            }

            static inline T lerp(T a, T b, T w)
            {
                return (1 - w) * a + w * b;
            }

            static inline T smooth(T x)
            {
                T xsq = x * x;
                return xsq * x * (6 * xsq - 15 * x + 10);
                // 6x^5 - 15x^4 + 10x^3
            }

            pg::RandomEngine<T, Dist> engine;

            std::array<size_t, DIM> dimensions;
            std::vector<Tuple> grid;

        private:
            static std::array<size_t, DIM> increaseArray
                    (const std::array<size_t, DIM> &array)
            {
                std::array<size_t, DIM> ret;
                for(size_t i = 0; i < DIM; ++i)
                    ret[i] = array[i] + 1;
                return ret;
            }

            /* Recursive method, splits the exploration space in binary
             * subtrees */
            T computeLocalContribution(const Tuple &point,
                                       const std::array<uint8_t, DIM> &base,
                                       size_t dimIndex) const
            {
                if(dimIndex == DIM) // Last dimension (leaf nodes)
                    return contribution(point, base);

                // Else, non-leaf nodes
                std::array<uint8_t, DIM> tmp = base;

                // Compute left subtree
                tmp[dimIndex] = 0;
                T leftResult = computeLocalContribution(point, tmp, dimIndex + 1);

                // Compute right subtree
                tmp[dimIndex] = 1;
                T rightResult = computeLocalContribution(point, tmp, dimIndex + 1);

                T factor = smooth(fractionalPart(point[dimIndex]));

                return lerp(leftResult, rightResult, factor);
            }
    };
    
    template<typename T, template<typename> class Dist, size_t DIM>
    class PerlinNoise : public pg::Incrementable<pg::PerlinNoiseTile<T, Dist, DIM>, DIM>
    {
        public:
            typedef std::array<T, DIM> Tuple;

            PerlinNoise(NumberGenerator &generator,
                        const Distribution<T, Dist> &distribution,
                        size_t tileDetail):
                pg::Incrementable<pg::PerlinNoiseTile<T, Dist, DIM>, DIM>(generator),
                distribution(distribution),
                tileDetail(tileDetail)
            {
            }

            virtual ~PerlinNoise() = default;

            T operator()(const Tuple &tuple)
            {
                std::array<int, DIM> coord;
                Tuple localCoord;
                for(size_t i = 0; i < DIM; ++i)
                {
                    float tmp;
                    localCoord[i] = std::modf(tuple[i], &tmp);
                    coord[i]      = tmp;
                }
                auto tile = this->At(coord);
                return tile(localCoord);
            }

        protected:
            pg::PerlinNoiseTile<T, Dist, DIM> &increment(const std::array<int, DIM> &coord)
            {
                PerlinNoiseTile<T, Dist, DIM> tile(
                        this->rngenerator, distribution, {tileDetail, tileDetail}, *this, coord);
                auto itInsert =this->tiles.insert({coord, tile});

                if(!itInsert.second){} // TODO
                return itInsert.first->second;
            }

            pg::Distribution<T, Dist> distribution;
            size_t tileDetail;
    };

    template <size_t DIM>
    class PerlinNoiseUniformFloat :
        public PerlinNoise<float, std::uniform_real_distribution, DIM>
    {
        public:
            PerlinNoiseUniformFloat(pg::NumberGenerator &generator,
                                    size_t tileDetail):
                PerlinNoise<float, std::uniform_real_distribution, DIM>
                (generator, std::uniform_real_distribution<float>{}, tileDetail)
            {
            }
    };
}

#endif

