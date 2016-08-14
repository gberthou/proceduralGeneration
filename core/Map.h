#ifndef MAP_H
#define MAP_H

#include <vector>

#include "../random/NumberGenerator.h"

namespace pg
{
    template <typename T>
    struct MapPoint
    {
        T x;
        T y;
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

            const std::vector<MapPoint<T>> GetVertices() const
            {
                return vertices;
            }

            const std::vector<MapEdge> GetEdges() const
            {
                return edges;
            }

        protected:
            std::vector<MapPoint<T>> vertices;
            std::vector<MapEdge> edges;
    };

    template<typename T>
    void CreateRandomizedGrid(NumberGenerator &rngenerator,
                              std::vector<MapPoint<T>> &points,
                              T minX, T maxX, T minY, T maxY,
                              size_t tileCountX, size_t tileCountY)
    {
        points.clear();

        T paceX = (maxX - minY) / tileCountX;
        T paceY = (maxY - minY) / tileCountY;
        auto distribution = pg::CreateDistributionUniformFloat(0, 1);
        for(size_t y = 0; y < tileCountY; ++y)
            for(size_t x = 0; x < tileCountX; ++x)
            {
                MapPoint<T> point = {
                    minX + paceX * (x + distribution(rngenerator)),
                    minY + paceY * (y + distribution(rngenerator))};
                points.push_back(point);
            }
    }
}

#endif

