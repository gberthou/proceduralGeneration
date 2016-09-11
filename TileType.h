#ifndef TILE_TYPE_H
#define TILE_TYPE_H

#include "core/Serializable.hpp"

struct TileType : public pg::Serializable
{
    public:
        TileType():
            island(false)
        {
        }

        TileType(bool i):
            island(i)
        {
        }

        pg::InputStream &Deserialize(pg::InputStream &stream)
        {
            stream >> island;
            return stream;
        }
        
        pg::OutputStream &Serialize(pg::OutputStream &stream) const
        {
            stream << island;
            return stream;
        }

        bool island;
};

#endif

