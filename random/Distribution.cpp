#include "Distribution.hpp"

namespace pg
{
    DistributionUniformUint
        CreateDistributionUniformUint(unsigned int min, unsigned int max)
    {
        std::uniform_int_distribution<unsigned int> dist(min, max);
        return DistributionUniformUint(dist);
    }

    DistributionUniformFloat
        CreateDistributionUniformFloat(float min, float max)
    {
        std::uniform_real_distribution<float> dist(min, max);
        return DistributionUniformFloat(dist);
    }
}

