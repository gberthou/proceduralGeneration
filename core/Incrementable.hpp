#ifndef INCREMENTABLE_HPP
#define INCREMENTABLE_HPP

#include <map>

#include "../random/NumberGenerator.hpp"

namespace pg
{
    struct TileCoord
    {
        int x;
        int y;

        TileCoord():
            x(0),
            y(0)
        {
        }

        TileCoord(int a, int b):
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
    
        bool operator<(const TileCoord &b) const
        {
            return x < b.x
                || (x == b.x && y < b.y);
        }
    };

    template<typename T>
    class Incrementable
    {
        public:
            Incrementable(pg::NumberGenerator &generator):
                rngenerator(generator)
            {
            }

            virtual ~Incrementable() = default;

            T &At(int x, int y)
            {
                auto it = tiles.find({x, y});
                if(it != tiles.end())
                    return it->second;

                // Does not exist yet
                return increment(x, y);
            }

        protected:
            virtual T &increment(int x, int y) = 0;

            std::map<TileCoord, T> tiles;
            pg::NumberGenerator &rngenerator;
    };
}

#endif

