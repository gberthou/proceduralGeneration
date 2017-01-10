#ifndef INCREMENTABLE_HPP
#define INCREMENTABLE_HPP

#include <map>

#include "../random/NumberGenerator.hpp"
#include "Serializable.hpp"

namespace pg
{
    template<size_t DIM>
    struct TileCoord
    {
        std::array<int, DIM> coord;

        TileCoord()
        {
        }

        TileCoord(const std::array<int, DIM> &coord):
            coord(coord)
        {
        }
        
        pg::InputStream &Deserialize(pg::InputStream &stream)
        {
            for(auto c : coord)
                stream >> c;
            return stream;
        }
        
        pg::OutputStream &Serialize(pg::OutputStream &stream) const
        {
            for(size_t i = 0; i < DIM; ++i)
                stream << coord[i];
            return stream;
        }
    
        bool operator<(const TileCoord<DIM> &b) const
        {
            for(size_t i = 0; i < DIM; ++i)
            {
                if(coord[i] < b.coord[i])
                    return true;
                else if(coord[i] > b.coord[i])
                    return false;
            }
            return false;
        }
    };

    template<typename T, size_t DIM>
    class Incrementable
    {
        public:
            Incrementable(pg::NumberGenerator &generator):
                rngenerator(generator)
            {
            }

            virtual ~Incrementable() = default;

            T &At(const std::array<int, DIM> &coord)
            {
                auto it = tiles.find(coord);
                if(it != tiles.end())
                    return it->second;

                // Does not exist yet
                return increment(coord);
            }
            
            bool HasTile(const std::array<int, DIM> &coord, const T *&tile) const
            {
                auto it = this->tiles.find(coord);
                if(it != this->tiles.end())
                {
                    tile = &it->second;
                    return true;
                }
                return false;
            }

        protected:
            virtual T &increment(const std::array<int, DIM> &coord) = 0;

            std::map<TileCoord<DIM>, T> tiles;
            pg::NumberGenerator &rngenerator;
    };
}

#endif

