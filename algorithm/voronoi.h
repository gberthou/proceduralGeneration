#ifndef VORONOI_H
#define VORONOI_H

#include <iostream>
#include <vector>
#include <map>
#include <boost/polygon/voronoi.hpp>

#include "../core/Map.h"

template<typename T>
struct VPoint
{
    T a;
    T b;

    VPoint(T x, T y):
        a(x), b(y)
    {
    }
};

template<typename T>
struct VSegment
{
    VPoint<T> p0;
    VPoint<T> p1;

    VSegment(T x1, T y1, T x2, T y2):
        p0(x1, y1), p1(x2, y2)
    {
    }
};

namespace boost
{
    namespace polygon
    {
        template <typename T>
        struct geometry_concept<VPoint<T>>
        {
            typedef point_concept type;
        };

        template <typename T>
        struct point_traits<VPoint<T>>
        {
            typedef T coordinate_type;

            static inline coordinate_type get(const VPoint<T>& point,
                                              orientation_2d orient)
            {
                    return (orient == HORIZONTAL) ? point.a : point.b;
            }
        };

        template <typename T>
        struct geometry_concept<VSegment<T>>
        {
              typedef segment_concept type;
        };

        template <typename T>
        struct segment_traits<VSegment<T>>
        {
            typedef T coordinate_type;
            typedef VPoint<T> point_type;

            static inline point_type get(const VSegment<T>& segment,
                                         direction_1d dir)
            {
                return dir.to_int() ? segment.p1 : segment.p0;
            }
        };
    }
}

namespace pg
{
    template<typename T>
    class VoronoiMap : public pg::Map<T>
    {
        public:
            explicit VoronoiMap<T>(const std::vector<VPoint<T>> &points)
            {
                std::vector<VSegment<T>> segments;
                boost::polygon::construct_voronoi(
                    points.begin(), points.end(),
                    segments.begin(), segments.end(),
                    &diagram);

                // Populate parent-class attributes
                
                std::map<const boost::polygon::voronoi_vertex<double>*, size_t> m;
                for(auto it = diagram.vertices().cbegin();
                    it != diagram.vertices().cend();
                    ++it)
                {
                    auto vertex = *it;
                    if(vertex.incident_edge()->is_primary())
                    {
                        T x = static_cast<T>(vertex.x());
                        T y = static_cast<T>(vertex.y());
            
                        //if(pointBelongsToMap({x, y}))
                        {
                            m.insert(std::pair<const boost::polygon::voronoi_vertex<double>*, size_t>(&*it, pg::Map<T>::vertices.size()));

                            pg::Map<T>::vertices.push_back({x, y});
                        }
                    }
                }

                for(auto edge : diagram.edges())
                {
                    if(edge.is_primary())
                    {
                        // Should never happen
                        if(edge.vertex0() == nullptr
                        && edge.vertex1() == nullptr)
                        {
                            continue;
                        }
                        
                        if(edge.vertex0() == nullptr)
                        {
                        }
                        else if(edge.vertex1() == nullptr)
                        {
                        }
                        else
                        {
                            auto it0 = m.find(edge.vertex0());
                            auto it1 = m.find(edge.vertex1());

                            if(it0 != m.end() && it1 != m.end())
                            {
                                pg::MapEdge edge = {it0->second, it1->second};
                                pg::Map<T>::edges.push_back(edge);
                            }
                        }
                    }
                }
            }

            virtual ~VoronoiMap<T>() = default;

        protected:
            virtual bool pointBelongsToMap(const MapPoint<T> &point) const = 0;

            boost::polygon::voronoi_diagram<double> diagram;

    };
    
    template<typename T>
    class RectVoronoiMap : public pg::VoronoiMap<T>
    {
        public:
            RectVoronoiMap<T>(T miX, T maX, T miY, T maY,
                              const std::vector<VPoint<T>> &points):
                VoronoiMap<T>(points),
                minX(miX),
                maxX(maX),
                minY(miY),
                maxY(maY)
            {
            }

            virtual ~RectVoronoiMap<T>() = default;

        protected:
            bool pointBelongsToMap(const MapPoint<T> &point) const
            {
                return point.x >= minX && point.x <= maxX
                    && point.y >= minY && point.y <= maxY;
            }
            
            T minX;
            T maxX;
            T minY;
            T maxY;
    };
}

#endif

