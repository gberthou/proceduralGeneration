#include "IslandGenerator.h"

IslandGenerator::IslandGenerator(float uX, float uY):
    noise(rngenerator),
    unitX(uX),
    unitY(uY)
{
}

TileType IslandGenerator::operator()(const pg::VPoint<float> & point)
{
    float x = point.x * NOISE_DENSITY / unitX;
    float y = point.y * NOISE_DENSITY / unitY;
    return {noise({x, y}) > .6f};
}

