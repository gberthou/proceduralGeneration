#ifndef MESH_SPRITE_GROUP_H
#define MESH_SPRITE_GROUP_H

#include <memory>

#include <SFML/Graphics.hpp>

#include "MeshSprite.h"

class MeshSpriteGroup : public sf::Drawable, public sf::Transformable
{
    public:
        MeshSpriteGroup(pg::NumberGenerator &generator,
                        pg::VoronoiMesh<float, TileType> &mesh,
                        size_t texWidth, size_t texHeight);
        virtual ~MeshSpriteGroup();

        void Refresh();

        virtual void draw(sf::RenderTarget &target, sf::RenderStates states)
            const;

    protected:
        static const size_t SPRITE_DIM = 3;
        static const size_t SPRITE_COUNT = SPRITE_DIM * SPRITE_DIM;

        size_t texWidth;
        size_t texHeight;
        sf::View view;
        std::unique_ptr<MeshSprite> sprites[SPRITE_COUNT];
};

#endif

