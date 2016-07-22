#ifndef PERLIN_NOISE_2D_H
#define PERLIN_NOISE_2D_H

#include <vector>
#include <cmath>

#include "../random/RandomEngine.h"

namespace pg
{

template<typename T, template<typename> class Dist>
class PerlinNoise2D
{
    public:
        PerlinNoise2D(pg::NumberGenerator &generator,
                      const pg::Distribution<T, Dist> &distribution,
                      unsigned int w, unsigned int h):
            engine(generator, distribution),
            width(w),
            height(h),
            grid(w * h)
        {
            for(unsigned int i = 0; i < w * h; ++i)
                grid[i] = generate2DVector();
        }

        virtual ~PerlinNoise2D() = default;

        
        /* x must be in range [0, width[
         * y must be in range [0, height[
         * output in range [0, 1]
         */
        T operator()(T x, T y) const
        {
            unsigned int x0 = floor(x);
            unsigned int y0 = floor(y);
            unsigned int x1 = x0 + 1;
            unsigned int y1 = y0 + 1;
            T factorX = smooth(x - x0);
            T factorY = smooth(y - y0);

            T contribution00 = contribution(x, y, x0, y0);
            T contribution10 = contribution(x, y, x1, y0);
            T contribution01 = contribution(x, y, x0, y1);
            T contribution11 = contribution(x, y, x1, y1);

            T intermediate0 = lerp(contribution00, contribution10, factorX);
            T intermediate1 = lerp(contribution01, contribution11, factorX);
            
            return (lerp(intermediate0, intermediate1, factorY) + 1) / 2.;
        }

    protected:
        struct Tuple
        {
            T x;
            T y;
        };

        virtual Tuple generate2DVector()
        {
            T number = engine();
            Tuple ret;
            ret.x = std::cos(number);
            ret.y = std::sin(number);
            return ret;
        }

        inline const Tuple &at(unsigned int x, unsigned int y) const
        {
            return grid[x + y * width];
        }

        T contribution(T x, T y, unsigned int xRef, unsigned int yRef) const
        {
            T dx = x - xRef;
            T dy = y - yRef;
            const Tuple &tuple = at(xRef, yRef);
            return dx * tuple.x + dy * tuple.y;
        }

        static inline T lerp(T a, T b, T w)
        {
            return (1 - w) * a + w * b;
        }

        static inline T smooth(T x)
        {
            T xsq = x * x;
            return xsq * x * (6 * xsq - 15 * x + 10); // 6x^5 - 15x^4 + 10x^3
        }

        pg::RandomEngine<T, Dist> engine;

        unsigned int width;
        unsigned int height;
        std::vector<Tuple> grid;
};
}

#endif

