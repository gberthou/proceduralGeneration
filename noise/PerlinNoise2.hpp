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
    class PerlinNodes : public pg::Incrementable<std::array<T, DIM>, DIM>
    {
        public:
            typedef std::array<T, DIM> Tuple;

            PerlinNodes(pg::NumberGenerator &generator,
                        const Distribution<T, Dist> &distribution):
                pg::Incrementable<std::array<T, DIM>, DIM>(generator),
                engine(generator, distribution)
            {
            }

            virtual ~PerlinNodes() = default;

        protected:
            virtual Tuple &increment(const std::array<int, DIM> &coord)
            {
                auto it = this->tiles.find(coord);
                if(it == this->tiles.end())
                {
                    Tuple vec = generateVector();
                    return this->tiles.insert(std::make_pair(coord, vec)).first->second;
                }
                return it->second;
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

            pg::RandomEngine<T, Dist> engine;
    };

    template<typename T, template<typename> class Dist, size_t DIM>
    class PerlinNoise
    {
        public:
            typedef std::array<T, DIM> Tuple;

            PerlinNoise(NumberGenerator &generator,
                        const Distribution<T, Dist> &distribution):
                nodes(generator, distribution),
                distribution(distribution)
            {
            }

            virtual ~PerlinNoise() = default;

            T operator()(const Tuple &tuple)
            {
                /*
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
                */

                std::array<uint8_t, DIM> base;
                // No need to initialize the array since
                // computeLocalContribution already does it
                return (computeLocalContribution(tuple, base, 0) + 1) / 2.;
            }

        protected:
            T contribution(const Tuple &point,
                           const std::array<uint8_t, DIM> &relativeRef)
            {
                std::array<int, DIM> absoluteRef;
                for(size_t i = 0; i < DIM; ++i)
                    absoluteRef[i] = std::floor(point[i]) + relativeRef[i];

                const Tuple &tuple = nodes.At(absoluteRef);
                T product = 0;
                for(size_t i = 0; i < DIM; ++i)
                {
                    product += (fractionalPart(point[i]) - relativeRef[i])
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

            /* Recursive method, splits the exploration space in binary
             * subtrees */
            T computeLocalContribution(const Tuple &point,
                                       const std::array<uint8_t, DIM> &base,
                                       size_t dimIndex)
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

            PerlinNodes<T, Dist, DIM> nodes;
            pg::Distribution<T, Dist> distribution;
    };

    template <size_t DIM>
    class PerlinNoiseUniformFloat :
        public PerlinNoise<float, std::uniform_real_distribution, DIM>
    {
        public:
            PerlinNoiseUniformFloat(pg::NumberGenerator &generator):
                PerlinNoise<float, std::uniform_real_distribution, DIM>
                (generator, std::uniform_real_distribution<float>{})
            {
            }
    };
}

#endif

