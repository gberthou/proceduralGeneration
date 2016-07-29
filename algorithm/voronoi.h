#ifndef VORONOI_h
#define VORONOI_H

#include <vector>
#include <array>
#include <algorithm>

namespace pg
{
    /*
    template<typename T>
    struct Distance
    {
        const std::array<T, 2> &point;
        T distance;
    };

    template<typename T>
    bool operator<(const Distance<T> &a, const Distance<T> &b)
    {
        return a.distance < b.distance;
    }

    template <typename T, size_t N>
    static T dist2(const std::array<T, N> &a, const std::array<T, N> &b)
    {
        T ret = 0;
        for(size_t i = 0; i < N; ++i)
        {
            T tmp = a[i] - b[i];
            ret += tmp * tmp;
        }
        return ret;
    }
    */

    template<typename T>
    using Point = std::array<T, 2>;

    template<typename T>
    using Triangle = std::array<Point<T>, 3>;

    template<typename T>
    Triangle<T> boundingTriangle(const std::vector<Point<T>> &points)
    {
        /* Compute center */
        Point<T> center = {0, 0};
        for(auto point : points)
            for(size_t i = 0; i < point.size(); ++i)
                center[i] += point[i];
        for(size_t i = 0; i < center.size(); ++i)
            center[i] /= points.size();

        /* Compute largest distance to center */
        T maxDistance = 0;
        for(auto point : points)
        {
            T distance = 0;
            for(size_t i = 0; i < point.size(); ++i)
                distance += (point[i] - center[i]) * (point[i] - center[i]);
            if(distance > maxDistance)
                maxDistance = distance;
        }
        T radius = 1.1 * std::sqrt(maxDistance);

        /* Take an arbitrary triangle which inscribed circle is the one 
         * computed above */
        Triangle<T> ret;
        ret[0] = {center[0] - 2*radius, center[1] + radius};
        ret[1] = {center[0] + 2*radius, center[1] + radius};

        /*
        const T sqrt2 = std::sqrt(2.);
        T k = 2. / (2 - sqrt2 / (radius + 1));
        ret[2] = {(1-k) * ret[0] - k * sqrt2/2.,
                  (1-k) * ret[1] - k * sqrt2/2.};
        */
        ret[2] = {center[0] * 3 - ret[0][0] - ret[1][0],
                  center[1] * 3 - ret[0][1] - ret[1][0]};
        return ret;
    }

    template<typename T>
    void Voronoi(const std::vector<Point<T>> &points,
                 std::vector<Triangle<T>> &triangles)
    {
        /*
        for(size_t i = 0; i < points.size(); ++i)
        {
            std::vector<Distance<T>> distances;
            for(size_t j = 0; j < points.size(); ++j)
            {
                if(i != j)
                    distances.push_back({points[j], dist2(pints[i],points[j])});
            }

            // Take the closest 3 points
            std::partial_sort(distances.begin(), distances.begin() + 3,
                              distances.end());
        }
        */

        triangles.clear();
        Triangle<T> superTriangle = boundingTriangle(points);
        triangles.push_back(superTriangle);
    }
}

#endif

