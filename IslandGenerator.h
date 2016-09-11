#ifndef ISLAND_GENERATOR_H
#define ISLAND_GENERATOR_H

#include "algorithm/VoronoiUtils.hpp"
#include "random/StdNumberGenerator.hpp"
#include "noise/PerlinNoise.hpp"

#include "TileType.h"

class IslandGenerator : public pg::PropertyGenerator<float, TileType>
{
    public:
        IslandGenerator(float unitX, float unitY);
        virtual ~IslandGenerator() = default;

        virtual TileType operator()(const pg::VPoint<float> & point);

    protected:
        static const size_t NOISE_DENSITY = 8;

        pg::StdNumberGenerator rngenerator;
        pg::PerlinNoiseUniformFloat<2> noise;
        float unitX;
        float unitY;
};

#endif

