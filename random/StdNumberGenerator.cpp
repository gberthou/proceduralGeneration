#include "StdNumberGenerator.h"

unsigned int pg::StdNumberGenerator::operator()()
{
    return randomDevice();
}

unsigned int pg::StdNumberGenerator::min()
{
    return randomDevice.min();
}

unsigned int pg::StdNumberGenerator::max()
{
    return randomDevice.max();
}

