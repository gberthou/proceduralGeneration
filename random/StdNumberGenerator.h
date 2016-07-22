#ifndef STD_NUMBER_GENERATOR_H
#define STD_NUMBER_GENERATOR_H

#include <random>

#include "NumberGenerator.h"

namespace pg
{

class StdNumberGenerator : public pg::NumberGenerator
{
    public:
        StdNumberGenerator() = default;
        virtual ~StdNumberGenerator() = default;

        virtual unsigned int operator()();
        virtual unsigned int min();
        virtual unsigned int max();

    protected:
        std::random_device randomDevice;
};
}

#endif

