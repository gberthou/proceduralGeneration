#ifndef MAP_HPP
#define MAP_HPP

#include <vector>

#include "../random/NumberGenerator.hpp"
#include "../random/Distribution.hpp"

namespace pg
{
    template <typename T>
    struct MapPoint
    {
        T x;
        T y;
        
        MapPoint():
            x(0),
            y(0)
        {
        }

        MapPoint(T ax, T ay):
            x(ax),
            y(ay)
        {
        }

        MapPoint(const MapPoint &other):
            x(other.x),
            y(other.y)
        {
        }
        
        bool operator!=(const MapPoint &other) const
        {
            return x != other.x || y != other.y;
        }
    };

    struct MapEdge
    {
        // Stores points indices
        size_t p0;
        size_t p1;
    };

    template <typename T>
    class Map
    {
        public:
            Map<T>() = default;
            virtual ~Map<T>() = default;
    };

    template<typename T>
    void CreateRandomizedGrid(NumberGenerator &rngenerator,
                              std::vector<MapPoint<T>> &points,
                              T minX, T maxX, T minY, T maxY,
                              size_t tileCountX, size_t tileCountY)
    {
        points.resize(tileCountX * tileCountY);

        T paceX = (maxX - minX) / tileCountX;
        T paceY = (maxY - minY) / tileCountY;
        auto distribution = pg::CreateDistributionUniformFloat(0, 1);
        for(size_t y = 0; y < tileCountY; ++y)
            for(size_t x = 0; x < tileCountX; ++x)
            {
                MapPoint<T> point = {
                    minX + paceX * (x + distribution(rngenerator)),
                    minY + paceY * (y + distribution(rngenerator))};
                points[x + y * tileCountX] = point;
            }
    }
}

#endif

