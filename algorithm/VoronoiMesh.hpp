#ifndef VORONOI_MESH
#define VORONOI_MESH

#include <array>
#include <set>
#include <vector>

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
    
    template <typename T>
    static T dist2(const VPoint<T> &a, const VPoint<T> &b)
    {
        T dx = a.x - b.x;
        T dy = a.y - b.y;

        return dx*dx + dy*dy;
    }

    template<typename T, typename P>
    class VoronoiTile
    {
        public:
            VoronoiTile(const std::vector<VoronoiSite<T, P>> &s):
                sites(s)
            {
            }

            ~VoronoiTile() = default;

            const VoronoiSite<T, P> &GetSiteAt(const VPoint<T> &point) const
            {
                if(sites.size() == 0)
                {
                    throw std::runtime_error("VoronoiTile should have at least "
                                             "one site");
                }

                const VoronoiSite<T, P> *ret = &sites[0];
                T minDistance = dist2(ret->point, point);

                for(size_t i = 1; i < sites.size(); ++i)
                {
                    T tmp = dist2(sites[i].point, point);
                    if(tmp < minDistance)
                    {
                        minDistance = tmp;
                        ret = &sites[i];
                    }
                }
                return *ret;
            }
        
        protected:
            std::vector<VoronoiSite<T, P>> sites;
    };
}

#endif

