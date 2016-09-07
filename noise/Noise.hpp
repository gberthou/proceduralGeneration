#ifndef NOISE_HPP
#define NOISE_HPP

#include <vector>
#include <array>
#include <cmath>

namespace pg
{
    template<typename T, size_t DIM>
    class Noise
    {
        public:
            typedef std::array<T, DIM> Tuple;

            Noise() = default;

            virtual ~Noise() = default;

            /* input must be in range [0, dimension.i[
             * output in range [0, 1]
             */
            virtual T operator()(const Tuple &point) const = 0;
    };
}
        
#endif

