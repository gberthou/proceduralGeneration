#ifndef VORONOI_MESH
#define VORONOI_MESH

#include <array>
#include <set>
#include <vector>
#include <map>
#include <algorithm>

#include "../core/Map.hpp"
#include "../core/Serializable.h"

namespace pg
{
    template<typename T>
    using VPoint = MapPoint<T>;

    template<typename T, typename P>
    struct VoronoiSite
    {
        VPoint<T> point;
        P properties;
    };

    struct VoronoiTileCoord
    {
        int x;
        int y;
    };
    bool operator<(const VoronoiTileCoord &a, const VoronoiTileCoord &b)
    {
        return a.x < b.x
            || (a.x == b.x && a.y < b.y);
    }
    
    template <typename T>
    static T dist2(const VPoint<T> &a, const VPoint<T> &b)
    {
        T dx = a.x - b.x;
        T dy = a.y - b.y;

        return dx*dx + dy*dy;
    }

    template<typename T, typename P>
    class PropertyGenerator
    {
        public:
            PropertyGenerator() = default;
            virtual ~PropertyGenerator() = default;

            virtual P operator()(const VPoint<T> &point) = 0;
    };

    template<typename T, typename P>
    class VoronoiTile
    {
        public:
            VoronoiTile(const std::vector<VoronoiSite<T, P>> &s):
                sites(s)
            {
            }

            virtual ~VoronoiTile() = default;

            VoronoiSite<T, P> &SiteAt(const VPoint<T> &point, size_t &index,
                                      float &distance)
            {
                if(sites.size() == 0)
                {
                    throw std::runtime_error("VoronoiTile should have at least "
                                             "one site");
                }

                index = 0;
                T minDistance = dist2(sites[0].point, point);

                for(size_t i = 1; i < sites.size(); ++i)
                {
                    T tmp = dist2(sites[i].point, point);
                    if(tmp < minDistance)
                    {
                        minDistance = tmp;
                        index = i;
                    }
                }
                distance = minDistance;

                return sites[index];
            }
        
        protected:
            std::vector<VoronoiSite<T, P>> sites;
    };

    template<typename T, typename P>
    class VoronoiMesh
    {
        public:
            VoronoiMesh(PropertyGenerator<T, P> &pgenerator,
                        size_t tDensityX, size_t tDensityY, T uX, T uY):
                propertyGenerator(pgenerator),
                tileDensityX(tDensityX),
                tileDensityY(tDensityY),
                unitX(uX),
                unitY(uY)
            {
            }

            virtual ~VoronoiMesh() = default;

            VoronoiTile<T, P> &TileAt(pg::NumberGenerator &rngenerator, int x,
                                      int y)
            {
                auto it = tiles.find({x, y});
                if(it != tiles.end())
                    return it->second;

                // Does not exist yet
                std::vector<pg::MapPoint<T>> points;
                pg::CreateRandomizedGrid(rngenerator, points, x*unitX,
                                         (x+1)*unitX, y*unitY, (y+1)*unitY,
                                         tileDensityX, tileDensityY);
                std::vector<VoronoiSite<T, P>> sites(points.size());
                for(size_t i = 0; i < sites.size(); ++i)
                {
                    sites[i].point = points[i];
                    sites[i].properties = propertyGenerator(points[i]);
                }

                auto itInsert= tiles.insert({{x, y}, VoronoiTile<T, P>(sites)});
                if(!itInsert.second){} // TODO
                return itInsert.first->second;
            }
            
            VoronoiSite<T, P> &SiteAt(pg::NumberGenerator &rngenerator,
                                      const VPoint<T> &point)
            {
                struct Candidate
                {
                    VoronoiSite<T, P> site;
                    float distance;

                    bool operator<(const Candidate &a)
                    {
                        return distance < a.distance;
                    }
                };

                int tileX = std::floor(point.x / unitX);
                int tileY = std::floor(point.y / unitY);
                VoronoiTile<T, P> &tile = TileAt(rngenerator, tileX, tileY);

                size_t subtileIndex;
                float distance;
                VoronoiSite<T, P> &site = tile.SiteAt(point, subtileIndex,
                                                      distance);

                size_t subtileX = subtileIndex % tileDensityX;
                size_t subtileY = subtileIndex / tileDensityX;

                std::vector<VoronoiTile<T, P>*> borderTiles;
                std::vector<Candidate> candidates = {{site, distance}};

                if(subtileX == 0) // Left border
                {
                    // Lookup tile on the left
                    borderTiles.push_back(&TileAt(rngenerator, tileX-1, tileY));
                }
                else if(subtileX + 1 == tileDensityX) // Right border
                {
                    // Lookup tile on the right
                    borderTiles.push_back(&TileAt(rngenerator, tileX+1, tileY));
                }

                if(subtileY == 0) // Upper border
                {
                    // Lookup tile on the top
                    borderTiles.push_back(&TileAt(rngenerator, tileX, tileY-1));
                }
                else if(subtileY + 1 == tileDensityY) // Lower border
                {
                    // Lookup tile on the bottom
                    borderTiles.push_back(&TileAt(rngenerator, tileX, tileY+1));
                }

                for(auto borderTile : borderTiles)
                {
                    size_t subtileIndex;
                    float distance;
                    VoronoiSite<T, P> &site =
                        borderTile->SiteAt(point, subtileIndex, distance);
                    candidates.push_back({site, distance});
                }

                // Take the closest candidate to the point
                std::partial_sort(candidates.begin(), candidates.begin() + 1,
                                  candidates.end()); 

                return candidates[0].site;
            }

        protected:
            PropertyGenerator<T, P> &propertyGenerator;
            size_t tileDensityX;
            size_t tileDensityY;
            T unitX;
            T unitY;
            std::map<VoronoiTileCoord, VoronoiTile<T, P>> tiles;
    };
}

#endif

