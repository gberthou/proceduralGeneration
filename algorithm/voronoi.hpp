#ifndef VORONOI_HPP
#define VORONOI_HPP

#include <iostream>
#include <vector>
#include <map>
#include <boost/polygon/voronoi.hpp>

#include "../core/Map.hpp"

template<typename T>
struct VVector
{
    T x;
    T y;

    VVector<T>(T ax, T ay):
        x(ax),
        y(ay)
    {
    }

    T crossProduct(const VVector<T> &other)
    {
        return x * other.y - y * other.x;
    }
};

template<typename T>
struct VSegment
{
    pg::MapPoint<T> p0;
    pg::MapPoint<T> p1;

    VSegment<T>()
    {
    }
     
    VSegment<T>(T x1, T y1, T x2, T y2):
        p0(x1, y1),
        p1(x2, y2)
    {
    }

    VSegment<T>(const pg::MapPoint<T> point0, const pg::MapPoint<T> &point1):
        p0(point0),
        p1(point1)
    {
    }
    
    bool operator!=(const VSegment<T> &b) const
    {
        return p0 != b.p0 || p1 != b.p1;
    }

    T length2() const
    {
        T dx = p0.x - p1.x;
        T dy = p0.y - p1.y;
        return dx*dx + dy*dy;
    }
};

template<typename T>
struct Line
{
    pg::MapPoint<T> point;
    T dx;
    T dy;

    Line<T>(const pg::MapPoint<T> &p, T adx, T ady):
        point(p),
        dx(adx),
        dy(ady)
    {
    }

    Line<T>(const VSegment<T> &edge):
        point((edge.p0.x+edge.p1.x)/static_cast<T>(2.),
              (edge.p0.y+edge.p1.y)/static_cast<T>(2.))
    {
        T length = std::sqrt(edge.length2());
        dx = (edge.p1.x - edge.p0.x) / length;
        dy = (edge.p1.y - edge.p0.y) / length;
    }

    Line<T> orthogonal() const
    {
        return Line<T>(point, dy, -dx);
    }

    bool intersect(const VSegment<T> &segment, pg::MapPoint<T> &intersection)
        const
    {
        const float EPSILON = 1e-3;

        VVector<T> ab(segment.p1.x - segment.p0.x,
                      segment.p1.y - segment.p0.y);
        VVector<T> ac(point.x - segment.p0.x,
                      point.y - segment.p0.y);
        VVector<T> u(dx, dy);

        T uXab = u.crossProduct(ab);
        if(uXab >= -EPSILON && uXab <= EPSILON) // Parallel vectors
            return false;
        
        T abXac = ab.crossProduct(ac);

        // CP = k * u
        T k = abXac / uXab;

        intersection.x = point.x + k * dx;
        intersection.y = point.y + k * dy;

        // Check that intersection belongs to segment
        return (segment.p0.x-intersection.x)*(segment.p1.x-intersection.x)
             + (segment.p0.y-intersection.y)*(segment.p1.y-intersection.y) < 0;
    }
    
    bool intersect(const Line<T> &line, pg::MapPoint<T> &intersection)
        const
    {
        const float EPSILON = 1e-3;

        VVector<T> ab(line.dx, line.dy);
        VVector<T> ac(point.x - line.point.x,
                      point.y - line.point.y);
        VVector<T> u(dx, dy);

        T uXab = u.crossProduct(ab);
        if(uXab >= -EPSILON && uXab <= EPSILON) // Parallel vectors
            return false;
        
        T abXac = ab.crossProduct(ac);

        // CP = k * u
        T k = abXac / uXab;
        if(k < 0) // Outside semi-line
            return false;

        intersection.x = point.x + k * dx;
        intersection.y = point.y + k * dy;

        return true;
    }
};

namespace boost
{
    namespace polygon
    {
        template <typename T>
        struct geometry_concept<pg::MapPoint<T>>
        {
            typedef point_concept type;
        };

        template <typename T>
        struct point_traits<pg::MapPoint<T>>
        {
            typedef T coordinate_type;

            static inline coordinate_type get(const pg::MapPoint<T>& point,
                                              orientation_2d orient)
            {
                    return (orient == HORIZONTAL) ? point.x : point.y;
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
            typedef pg::MapPoint<T> point_type;

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
    struct Rect2
    {
        T minX;
        T maxX;
        T minY;
        T maxY;

        Rect2(T miX, T maX, T miY, T maY):
            minX(miX),
            maxX(maX),
            minY(miY),
            maxY(maY)
        {
        }

        bool HasPoint(const pg::MapPoint<T> &point) const
        {
            return point.x >= minX && point.x <= maxX
                && point.y >= minY && point.y <= maxY;
        }
    };

    struct Tile2
    {
        int x;
        int y;
    };
        
    bool operator<(const Tile2 &a, const Tile2 &b)
    {
        if(a.x != b.x)
            return a.x < b.x;
        return a.y < b.y;

    }

    template<typename T>
    class VoronoiTile
    {
        public:
            VoronoiTile<T>(T minX, T maxX, T minY, T maxY,
                           const std::vector<MapPoint<T>> &p):
                rect(minX, maxX, minY, maxY),
                points(p)
            {
                boost::polygon::construct_voronoi(
                    p.begin(), p.end(),
                    &diagram);

                // Populate parent-class attributes
                
                std::map<const boost::polygon::voronoi_vertex<double>*, size_t>
                    m;
                for(auto it = diagram.vertices().cbegin();
                    it != diagram.vertices().cend();
                    ++it)
                {
                    auto vertex = *it;
                    if(vertex.incident_edge()->is_primary())
                    {
                        T x = static_cast<T>(vertex.x());
                        T y = static_cast<T>(vertex.y());
            
                        pg::MapPoint<T> point = {x, y};
                        if(rect.HasPoint(point))
                        {
                            m.insert(std::pair<const
                            boost::polygon::voronoi_vertex<double>*, size_t>(
                                &*it, vertices.size()));

                            vertices.push_back(point);
                        }
                        else
                            removedVertices.push_back(point);
                    }
                }

                for(auto edge : diagram.edges())
                {
                    if(edge.is_primary()
                    && edge.vertex0() != nullptr
                    && edge.vertex1() != nullptr)
                    {
                        auto it0 = m.find(edge.vertex0());
                        auto it1 = m.find(edge.vertex1());

                        if(it0 != m.end() && it1 != m.end())
                        {
                            pg::MapEdge mEdge = {it0->second, it1->second};
                            edges.push_back(mEdge);
                        }
                        //else
                        //    removedEdges.push_back(edge);
                    }
                }
            }

            virtual ~VoronoiTile<T>() = default;

            const std::vector<pg::MapPoint<T>> &GetVertices() const
            {
                return vertices;
            }

            const std::vector<pg::MapEdge> &GetEdges() const
            {
                return edges;
            }

            /* 'this' must be directly on the left, and 'other' on the right */
            void MergeHorizontal(const VoronoiTile<T> &other, size_t width,
                                 size_t height)
            {
                VSegment<T> upperHalfLine(points[width-1], other.points[0]);
                VSegment<T> lowerHalfLine(points[width-1 + (height-1)*width],
                                             other.points[(height-1)*width]);

                Line<T> ortho = Line<T>(upperHalfLine).orthogonal();
                ortho.dy = std::abs(ortho.dy);

                T originFactor = (ortho.dy < 0 ? 1 : -1)*(rect.maxY-rect.minY);
                pg::MapPoint<T> origin = ortho.point;
                origin.x += ortho.dx * originFactor;
                origin.y += ortho.dy * originFactor;

                std::cout << origin.x << ", " << origin.y << std::endl;

                VSegment<T> currentLine = upperHalfLine;
                const pg::MapPoint<T> *pointLocal = &points[width-1];
                const pg::MapPoint<T> *pointOther = &other.points[0];
                while(currentLine != lowerHalfLine)
                {
                    // Find highest intersections with edges involving hull
                    // vertices from left and right graphs
                    pg::MapPoint<T> closestLocal;
                    pg::MapPoint<T> closestOther;
                    if(getClosestIntersection(ortho, width-1, closestLocal)
                    && other.getClosestIntersection(ortho, 0, closestOther))
                    {
                        std::cout << closestLocal.x << ", " << closestLocal.y
                                  << " / " << closestOther.x << ", "
                                  << closestOther.y << std::endl;

                        if(closestLocal.y <= closestOther.y)
                        {
                            currentLine.p0 = *pointLocal;
                            pointLocal += width;
                            std::cout << "left" << std::endl;
                        }
                        else
                        {
                            currentLine.p1 = *pointOther;
                            pointOther += width;
                            std::cout << "right" << std::endl;
                        }
                    }
                    else
                        std::cout << "merde" << std::endl;

                    ortho = Line<T>(currentLine).orthogonal();
                }
            }

        protected:
            bool getClosestIntersection(const Line<T> &line,
                                        size_t sourceIndex,
                                        pg::MapPoint<T> &intersection) const
            {
                std::cout << "{{" << line.point.x << ", " << line.point.y
                          << "}, {" << line.dx << ", " << line.dy << "}}"
                          << std::endl;
                for(auto cell : diagram.cells())
                {
                    if(cell.source_index() != sourceIndex)
                        continue;

                    auto edge = cell.incident_edge();
                    T highest = 1e16;
                    do
                    {
                        if(edge->is_primary())
                        {
                            pg::MapPoint<T> tmp;
                            VSegment<T> segment;

                            if(edge->is_finite())
                            {
                                pg::MapPoint<T> p0(edge->vertex0()->x(),
                                                   edge->vertex0()->y());
                                pg::MapPoint<T> p1(edge->vertex1()->x(),
                                                   edge->vertex1()->y());

                                segment.p0 = p0;
                                segment.p1 = p1;
                            }
                            else
                            {
                                std::cout << "merde1" << std::endl;
                            }
                            
                            if(line.intersect(segment, tmp))
                            {
                                if(tmp.y < highest)
                                {
                                    std::cout << "super!" << std::endl;
                                    intersection = tmp;
                                    highest = intersection.y;
                                }
                            }
                        }

                        edge = edge->rot_next();
                    } while(edge != cell.incident_edge());
                    return true;
                }
                return false;
            }

            Rect2<T> rect;
            boost::polygon::voronoi_diagram<double> diagram;
            std::vector<pg::MapPoint<T>> points;
            std::vector<pg::MapPoint<T>> vertices;
            std::vector<pg::MapPoint<T>> removedVertices;
            std::vector<pg::MapEdge> edges;
            std::vector<pg::MapEdge> removedEdges;
    };

    template<typename T>
    class VoronoiMap : public pg::Map<T>
    {
        public:
            VoronoiMap<T>() = default;
            virtual ~VoronoiMap<T>()
            {
                for(auto it : tiles)
                {
                    delete it.second;
                }
            }
            
            bool GenerateTileAt(int tileX, int tileY, T minX, T maxX,
                                T minY, T maxY,
                                const std::vector<pg::MapPoint<T>> &p)
            {
                if(tiles.find({tileX, tileY}) != tiles.end())
                    return false;

                VoronoiTile<T> *tile = new VoronoiTile<T>(minX, maxX,
                                                          minY, maxY, p);

                std::pair<Tile2, VoronoiTile<T>*> pair({tileX, tileY}, tile);
                tiles.insert(pair);

                auto it = tiles.find({tileX-1, tileY});
                if(it != tiles.end())
                {
                    it->second->MergeHorizontal(*tile, 8, 8);
                }

                return true;
            }
            
            std::vector<MapPoint<T>> GetVertices(int tileX, int tileY) const
            {
                auto it = tiles.find({tileX, tileY});
                if(it == tiles.end())
                    return std::vector<MapPoint<T>>();
                return it->second->GetVertices();
            }

            std::vector<MapEdge> GetEdges(int tileX, int tileY) const
            {
                auto it = tiles.find({tileX, tileY});
                if(it == tiles.end())
                    return std::vector<MapEdge>();
                return it->second->GetEdges();
            }
            
            /*
            void filterOutVerticesAndEdges()
            {
                std::vector<pg::MapPoint<T>> &vertices = pg::Map<T>::vertices;
                std::vector<pg::MapEdge> &edges = pg::Map<T>::edges;

                for(size_t i = 0; i < vertices.size();)
                {
                    if(!pointBelongsToMap(vertices[i]))
                    {
                        for(size_t j = 0; j != edges.size();)
                        {
                            pg::MapEdge &edge = edges[j];
                            if(edge.p0 == i || edge.p1 == i)
                            {
                                edges.erase(edges.begin() + j);
                            }
                            else
                            {
                                if(edge.p0 > i)
                                    --edge.p0;
                                if(edge.p1 > i)
                                    --edge.p1;
                                ++j;
                            }
                        }
                        vertices.erase(vertices.begin() + i);
                    }
                    else
                        ++i;
                }
            }
            */

        protected:
            std::map<Tile2, VoronoiTile<T>*> tiles; 
    };
}

#endif

