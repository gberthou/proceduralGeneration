#include <stdexcept>

#include "MeshSprite.h"

MeshSprite::MeshSprite(pg::VoronoiMesh<float, TileType> &mesh,
                       size_t texWidth, size_t texHeight,
                       float offsetX, float offsetY)
{
    sf::Uint8 *pixels = new sf::Uint8[texWidth * texHeight * 4];

    for(size_t y = 0; y < texHeight; ++y)
        for(size_t x = 0; x < texWidth; ++x)
        {
            pg::VPoint<float> vpoint;
            vpoint.x = x + offsetX;
            vpoint.y = y + offsetY;

            TileType isIsland = mesh.SiteAt(vpoint).properties;
            size_t i = (x + y * texWidth) * 4;

            if(isIsland.island)
            {
                pixels[i  ] = 192;
                pixels[i+1] = 192;
                pixels[i+2] = 64;
            }
            else
            {
                pixels[i  ] = 64;
                pixels[i+1] = 64;
                pixels[i+2] = 255;
            }

            pixels[i+3] = 0xff;
        }
    
    sf::Image image;
    image.create(texWidth, texHeight, pixels);

    if(!texture.create(texWidth, texHeight))
        throw std::runtime_error("Cannot create texture");
    if(!texture.loadFromImage(image))
        throw std::runtime_error("Cannot load texture");
    delete [] pixels;
   
    setTexture(texture);
}

