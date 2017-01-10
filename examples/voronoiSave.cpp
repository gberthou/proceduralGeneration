#include <iostream>
#include <fstream>
#include <random>
#include <cstdlib>
#include <vector>
#include <string>

#include <SFML/Graphics.hpp>

#include "../random/StdNumberGenerator.hpp"
#include "../random/Distribution.hpp"
#include "../algorithm/VoronoiMesh.hpp"
#include "../core/Map.hpp"

struct Color : public pg::Serializable
{
    public:
        Color():
            r(0),
            g(0),
            b(0)
        {
        }

        Color(sf::Uint8 red, sf::Uint8 green, sf::Uint8 blue):
            r(red),
            g(green),
            b(blue)
        {
        }

        pg::InputStream &Deserialize(pg::InputStream &stream)
        {
            stream >> r >> g >> b;
            return stream;
        }
        
        pg::OutputStream &Serialize(pg::OutputStream &stream) const
        {
            stream << r << g << b;
            return stream;
        }

        sf::Uint8 r;
        sf::Uint8 g;
        sf::Uint8 b;
};

class ColorGenerator : public pg::PropertyGenerator<float, Color>
{
    public:
        ColorGenerator():
            distribution(pg::CreateDistributionUniformUint(0x00, 0xff))
        {
        }
        virtual ~ColorGenerator() = default;

        virtual Color operator()(const pg::VPoint<float> &)
        {
            sf::Uint8 colors[3];
            for(size_t i = 0; i < 3; ++i)
                colors[i] = distribution(rngenerator);
            return Color(colors[0], colors[1], colors[2]);
        }

    protected:
        pg::StdNumberGenerator rngenerator;
        pg::DistributionUniformUint distribution;
};
    
void drawMesh(sf::RenderTexture &texture, pg::VoronoiMesh<float, Color> &mesh,
              size_t width, size_t height)
{
    sf::Uint8 *pixels = new sf::Uint8[width*height*4];
    for(size_t y = 0; y < height; ++y)
        for(size_t x = 0; x < width; ++x)
        {
            pg::VPoint<float> vpoint;
            vpoint.x = x;
            vpoint.y = y;

            Color color = mesh.SiteAt(vpoint).properties;
            size_t i = (x+y*width)*4;
            pixels[i  ] = color.r;
            pixels[i+1] = color.g;
            pixels[i+2] = color.b;
            pixels[i+3] = 0xff;
        }
    
    sf::Image image;
    image.create(width, height, pixels);

    sf::Texture tex;
    tex.loadFromImage(image);

    sf::Sprite sprite(tex);
    texture.draw(sprite);
}

void TestVoronoi(pg::StdNumberGenerator &rngenerator, bool in)
{
    const std::string DATA_FILENAME = "map.bin";

    const unsigned int WIDTH = 640;
    const unsigned int HEIGHT = 640;

    ColorGenerator colorGenerator;
    pg::VoronoiMesh<float, Color> *map;
    
    if(in)
    {
        map = new pg::VoronoiMesh<float, Color>(rngenerator, colorGenerator);
        
        std::ifstream s(DATA_FILENAME.c_str(),
                        std::ios_base::in | std::ios_base::binary);
        pg::InputStream stream(s);
        stream >> *map;
    }
    else
        map = new pg::VoronoiMesh<float, Color>(rngenerator, colorGenerator, 8,
                                                8, 240, 240);

    sf::RenderTexture texture;
    texture.create(WIDTH, HEIGHT);
   
    drawMesh(texture, *map, WIDTH, HEIGHT);

    if(!in)
    {
        std::ofstream s(DATA_FILENAME.c_str(),
                        std::ios_base::out | std::ios_base::binary);
        pg::OutputStream stream(s);
        stream << *map;
    }

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

    delete map;
}

int main(int argc, char *argv[])
{
    const std::string S_IN  = "in";
    const std::string S_OUT = "out";
    pg::StdNumberGenerator rngenerator;

    if(argc < 2
    || (std::string(argv[1]) != S_IN && std::string(argv[1]) != S_OUT))
    {
        std::cout << "Usage: ./a.out [in|out]" << std::endl;
        return EXIT_SUCCESS;
    }

    TestVoronoi(rngenerator, std::string(argv[1]) == S_IN);

    return EXIT_SUCCESS;
}

