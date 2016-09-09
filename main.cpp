#include <iostream>
#include <fstream>
#include <random>
#include <cstdlib>
#include <vector>
#include <string>

#include <SFML/Graphics.hpp>

#include "random/StdNumberGenerator.hpp"
#include "random/Distribution.hpp"
#include "algorithm/VoronoiMesh.hpp"
#include "core/Map.hpp"
#include "noise/PerlinNoise.hpp"

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

class IslandGenerator : public pg::PropertyGenerator<float, TileType>
{
    public:
        IslandGenerator(float uX, float uY):
            noise(rngenerator, {NOISE_DENSITY, NOISE_DENSITY}),
            unitX(uX),
            unitY(uY)
        {
        }
        virtual ~IslandGenerator() = default;

        virtual TileType operator()(const pg::VPoint<float> & point)
        {
            float x = point.x * NOISE_DENSITY / unitX;
            float y = point.y * NOISE_DENSITY / unitY;
            return noise({x, y}) > .65f;
        }

    protected:
        static const size_t NOISE_DENSITY = 8;

        pg::StdNumberGenerator rngenerator;
        pg::PerlinNoiseUniformFloat<2> noise;
        float unitX;
        float unitY;
};
    
void drawMesh(pg::StdNumberGenerator &rngenerator, sf::RenderTexture &texture,
              pg::VoronoiMesh<float, TileType> &mesh,
              size_t width, size_t height)
{
    sf::Uint8 *pixels = new sf::Uint8[width*height*4];
    for(size_t y = 0; y < height; ++y)
        for(size_t x = 0; x < width; ++x)
        {
            pg::VPoint<float> vpoint;
            vpoint.x = x;
            vpoint.y = y;

            TileType isIsland = mesh.SiteAt(rngenerator, vpoint).properties;
            size_t i = (x+y*width)*4;

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
    image.create(width, height, pixels);

    sf::Texture tex;
    tex.loadFromImage(image);

    sf::Sprite sprite(tex);
    texture.draw(sprite);
}

void TestVoronoi(pg::StdNumberGenerator &rngenerator)
{
    const std::string DATA_FILENAME = "map.bin";

    const unsigned int WIDTH = 640;
    const unsigned int HEIGHT = 640;

    IslandGenerator islandGenerator(WIDTH, HEIGHT);
    pg::VoronoiMesh<float, TileType> map(islandGenerator, 8, 8, 120, 120);
    
    sf::RenderTexture texture;
    texture.create(WIDTH, HEIGHT);
   
    drawMesh(rngenerator, texture, map, WIDTH, HEIGHT);

    sf::Sprite sprite;
    sprite.setTexture(texture.getTexture());

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Voronoi");
    window.setFramerateLimit(60);
    
    while(window.isOpen())
    {
        sf::Event event;
        while(window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed)
                window.close();
        }
        
        window.clear();
        window.draw(sprite);
        window.display();
    }
}

int main()
{
    pg::StdNumberGenerator rngenerator;

    TestVoronoi(rngenerator);

    return EXIT_SUCCESS;
}

