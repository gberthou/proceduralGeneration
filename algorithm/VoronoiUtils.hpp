#ifndef VORONOI_UTILS_HPP
#define VORONOI_UTILS_HPP

#include <array>
#include <set>
#include <vector>
#include <map>
#include <algorithm>

#include "../core/Map.hpp"
#include "../core/Serializable.hpp"

namespace pg
{
    template<typename T>
    using VPoint = MapPoint<T>;
    
    template<typename T>
    class DistanceModifier
    {
        public:
            DistanceModifier() = default;
            virtual ~DistanceModifier() = default;

            virtual T operator()(const VPoint<T> &,
                                 const VPoint<T> &) const
            {
                return 0;
            }
    
            static DistanceModifier<T> defaultDistanceModifier;
    };
    
    template<typename T>
    DistanceModifier<T> DistanceModifier<T>::defaultDistanceModifier;

    template<typename T, typename P>
    struct VoronoiSite : public pg::Serializable
    {
        VPoint<T> point;
        P properties;

        pg::InputStream &Deserialize(pg::InputStream &stream)
        {
            stream >> point.x >> point.y >> properties;
            return stream;
        }
        
        pg::OutputStream &Serialize(pg::OutputStream &stream) const
        {
            stream << point.x << point.y << properties;
            return stream;
        }
    };

    struct VoronoiTileCoord : public pg::Serializable
    {
        int x;
        int y;

        VoronoiTileCoord():
            x(0),
            y(0)
        {
        }

        VoronoiTileCoord(int a, int b):
            x(a),
            y(b)
        {
        }
        
        pg::InputStream &Deserialize(pg::InputStream &stream)
        {
            stream >> x >> y;
            return stream;
        }
        
        pg::OutputStream &Serialize(pg::OutputStream &stream) const
        {
            stream << x << y;
            return stream;
        }
    
        bool operator<(const VoronoiTileCoord &b) const
        {
            return x < b.x
                || (x == b.x && y < b.y);
        }
    };
    
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
    class VoronoiTile : public pg::Serializable
    {
        public:
            VoronoiTile(const DistanceModifier<T> &dModifier =
                            DistanceModifier<T>::defaultDistanceModifier):
                distanceModifier(dModifier)
            {
            }

            VoronoiTile(const std::vector<VoronoiSite<T, P>> &s,
                        const DistanceModifier<T> &dModifier =
                            DistanceModifier<T>::defaultDistanceModifier):
                sites(s),
                distanceModifier(dModifier)
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
                T minDistance2 = dist2(point, sites[0].point);
                T minDistance = minDistance2
                              + distanceModifier(point, sites[0].point);

                for(size_t i = 1; i < sites.size(); ++i)
                {
                    T tmp = dist2(point, sites[i].point)
                          + distanceModifier(point, sites[i].point);
                    if(tmp < minDistance)
                    {
                        minDistance = tmp;
                        index = i;
                    }
                }
                distance = minDistance;

                return sites[index];
            }
            
            pg::InputStream &Deserialize(pg::InputStream &stream)
            {
                size_t size;
                stream >> size;
                
                sites.resize(size);
                for(size_t i = 0; i < size; ++i)
                    stream >> sites[i];
                return stream;
            }
            
            pg::OutputStream &Serialize(pg::OutputStream &stream) const
            {
                stream << sites.size();
                for(auto site : sites)
                    stream << site;
                return stream;
            }

        protected:
            std::vector<VoronoiSite<T, P>> sites;
            const DistanceModifier<T> &distanceModifier;
    };
}

#endif

