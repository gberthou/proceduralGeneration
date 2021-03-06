#ifndef VORONOI_MESH_HPP
#define VORONOI_MESH_HPP

#include <array>
#include <set>
#include <vector>
#include <map>
#include <algorithm>

#include "VoronoiUtils.hpp"
#include "../core/Incrementable.hpp"

namespace pg
{
    template<typename T, typename P>
    class VoronoiMesh : public pg::Serializable,
                        public pg::Incrementable<pg::VoronoiTile<T, P>, 2>
    {
        public:
            VoronoiMesh(pg::NumberGenerator &ngenerator,
                        PropertyGenerator<T, P> &pgenerator):
                pg::Incrementable<pg::VoronoiTile<T, P>, 2>(ngenerator),
                propertyGenerator(pgenerator)
            {
            }

            VoronoiMesh(pg::NumberGenerator &ngenerator,
                        PropertyGenerator<T, P> &pgenerator,
                        size_t tDensityX, size_t tDensityY, T uX, T uY):
                pg::Incrementable<pg::VoronoiTile<T, P>, 2>(ngenerator),
                propertyGenerator(pgenerator),
                tileDensityX(tDensityX),
                tileDensityY(tDensityY),
                unitX(uX),
                unitY(uY)
            {
            }

            virtual ~VoronoiMesh() = default;

            
            VoronoiSite<T, P> &SiteAt(const VPoint<T> &point)
            {
                struct Candidate
                {
                    VoronoiSite<T, P> *site;
                    float distance;

                    bool operator<(const Candidate &a)
                    {
                        return distance < a.distance;
                    }
                };

                int tileX = std::floor(point.x / unitX);
                int tileY = std::floor(point.y / unitY);
                VoronoiTile<T, P> &tile = this->At({tileX, tileY});

                size_t subtileIndex;
                float distance;
                VoronoiSite<T, P> &site = tile.SiteAt(point, subtileIndex,
                                                      distance);

                size_t subtileX = subtileIndex % tileDensityX;
                size_t subtileY = subtileIndex / tileDensityX;

                std::vector<VoronoiTile<T, P>*> borderTiles;
                std::vector<Candidate> candidates = {{&site, distance}};

                if(subtileX == 0) // Left border
                {
                    // Lookup tile on the left
                    borderTiles.push_back(&this->At({tileX-1, tileY}));
                }
                else if(subtileX + 1 == tileDensityX) // Right border
                {
                    // Lookup tile on the right
                    borderTiles.push_back(&this->At({tileX+1, tileY}));
                }

                if(subtileY == 0) // Upper border
                {
                    // Lookup tile on the top
                    borderTiles.push_back(&this->At({tileX, tileY-1}));
                }
                else if(subtileY + 1 == tileDensityY) // Lower border
                {
                    // Lookup tile on the bottom
                    borderTiles.push_back(&this->At({tileX, tileY+1}));
                }

                for(auto borderTile : borderTiles)
                {
                    size_t subtileIndex;
                    float distance;
                    VoronoiSite<T, P> &site =
                        borderTile->SiteAt(point, subtileIndex, distance);
                    candidates.push_back({&site, distance});
                }

                // Take the closest candidate to the point
                std::partial_sort(candidates.begin(), candidates.begin() + 1,
                                  candidates.end()); 

                return *candidates[0].site;
            }
            
            pg::InputStream &Deserialize(pg::InputStream &stream)
            {
                size_t size;
                stream >> tileDensityX >> tileDensityY >> unitX >> unitY
                       >> size;

                this->tiles.clear();
                for(size_t i = 0; i < size; ++i)
                {
                    pg::TileCoord<2> key;
                    VoronoiTile<T, P> value;

                    stream >> key >> value;
                    this->tiles.insert({key, value});
                }
                return stream;
            }
            
            pg::OutputStream &Serialize(pg::OutputStream &stream) const
            {
                stream << tileDensityX << tileDensityY << unitX << unitY
                       << this->tiles.size();
                
                for(auto tile : this->tiles)
                    stream << tile.first << tile.second;
                return stream;
            }

        protected:
            VoronoiTile<T, P> &increment(const std::array<int, 2> &coord)
            {
                int x = coord[0];
                int y = coord[1];

                std::vector<pg::MapPoint<T>> points;
                pg::CreateRandomizedGrid(this->rngenerator, points, x*unitX,
                                         (x+1)*unitX, y*unitY, (y+1)*unitY,
                                         tileDensityX, tileDensityY);
                std::vector<VoronoiSite<T, P>> sites(points.size());
                for(size_t i = 0; i < sites.size(); ++i)
                {
                    sites[i].point = points[i];
                    sites[i].properties = propertyGenerator(points[i]);
                }

                auto itInsert = this->tiles.insert({coord, VoronoiTile<T, P>(sites)});
                if(!itInsert.second){} // TODO
                return itInsert.first->second;
            }

            PropertyGenerator<T, P> &propertyGenerator;
            size_t tileDensityX;
            size_t tileDensityY;
            T unitX;
            T unitY;
    };
}

#endif

