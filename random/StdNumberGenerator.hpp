#ifndef STD_NUMBER_GENERATOR_HPP
#define STD_NUMBER_GENERATOR_HPP

#include <random>

#include "NumberGenerator.hpp"

namespace pg
{
    class StdNumberGenerator : public pg::NumberGenerator
    {
        public:
            StdNumberGenerator() = default;
            virtual ~StdNumberGenerator() = default;

            virtual unsigned int operator()()
            {
                return randomDevice();
            }

            virtual unsigned int min()
            {
                return randomDevice.min();
            }

            virtual unsigned int max()
            {
                return randomDevice.max();
            }

        protected:
            std::random_device randomDevice;
    };
}

#endif

