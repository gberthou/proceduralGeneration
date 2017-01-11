#include "MeshSpriteGroup.h"

MeshSpriteGroup::MeshSpriteGroup(pg::VoronoiMesh<float, TileType> &mesh,
                                 size_t tWidth, size_t tHeight):
    texWidth(tWidth),
    texHeight(tHeight),
    view(sf::Vector2f(tWidth, tHeight), sf::Vector2f(tWidth, tHeight))
{
    for(size_t y = 0; y < SPRITE_DIM; ++y)
        for(size_t x = 0; x < SPRITE_DIM; ++x)
        {
            MeshSprite *sprite =
                new MeshSprite(mesh, texWidth, texHeight,
                               x * texWidth, y * texHeight);
            sprite->setPosition(x * texWidth, y * texHeight);
            sprites[x + y * SPRITE_DIM].reset(sprite);
        }
}

MeshSpriteGroup::~MeshSpriteGroup()
{
}

void MeshSpriteGroup::Refresh()
{
    sf::Vector2f center = getTransform().transformPoint(texWidth*1.5f,
                                                        texHeight*1.5f);
    view.setCenter(center);
}

void MeshSpriteGroup::draw(sf::RenderTarget &target, sf::RenderStates states)
            const
{
    target.setView(view);
    for(size_t i = 0; i < SPRITE_COUNT; ++i)
        target.draw(*(sprites[i]), states);
}

