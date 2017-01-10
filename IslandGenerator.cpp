#include "IslandGenerator.h"

IslandGenerator::IslandGenerator(float uX, float uY):
    noise(rngenerator, NOISE_DENSITY),
    unitX(uX),
    unitY(uY)
{
}

TileType IslandGenerator::operator()(const pg::VPoint<float> & point)
{
    float x = point.x / unitX;
    float y = point.y / unitY;
    return {noise({x, y}) > .6f};
}

