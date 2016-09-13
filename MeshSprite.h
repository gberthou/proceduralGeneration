#ifndef MESH_SPRITE_H
#define MESH_SPRITE_H

#include <SFML/Graphics.hpp>

#include "algorithm/VoronoiMesh.hpp"

#include "TileType.h"

class MeshSprite : public sf::Sprite
{
    public:
        MeshSprite(pg::VoronoiMesh<float, TileType> &mesh,
                   size_t texWidth, size_t texHeight);
        virtual ~MeshSprite() = default;

    protected:
        sf::Texture texture;
};

#endif

