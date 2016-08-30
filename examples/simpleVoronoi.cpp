#include <iostream>
#include <random>
#include <cstdlib>
#include <vector>
#include <string>

#include <SFML/Graphics.hpp>

#include "../random/StdNumberGenerator.hpp"
#include "../random/Distribution.hpp"
#include "../algorithm/VoronoiMesh.hpp"
#include "../core/Map.hpp"
    
void drawTile(sf::RenderTexture &texture, const pg::VoronoiTile<float, sf::Color> &tile,
              size_t width, size_t height)
{
    sf::Uint8 *pixels = new sf::Uint8[width*height*4];
    for(size_t y = 0; y < height; ++y)
        for(size_t x = 0; x < width; ++x)
        {
            pg::VPoint<float> vpoint;
            vpoint.x = x;
            vpoint.y = y;

            sf::Color color = tile.GetSiteAt(vpoint).properties;
            size_t i = (x+y*width)*4;
            pixels[i  ] = color.r;
            pixels[i+1] = color.g;
            pixels[i+2] = color.b;
            pixels[i+3] = color.a;
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
    const unsigned int WIDTH = 640;
    const unsigned int HEIGHT = 640;

    std::vector<pg::VPoint<float>> points;
    pg::CreateRandomizedGrid<float>(rngenerator, points,
                                    16, WIDTH / 2,
                                    16, WIDTH / 2,
                                    8, 8);

    std::vector<pg::VoronoiSite<float, sf::Color>> sites(points.size());
    for(size_t i = 0; i < points.size(); ++i)
        sites[i] = {points[i], sf::Color(10*i, 10*i, 10*i)};

    pg::VoronoiTile<float, sf::Color> map(sites);

    sf::RenderTexture texture;
    texture.create(WIDTH, HEIGHT);
   
    drawTile(texture, map, WIDTH, HEIGHT);

    for(auto point : points)
    {
        const float RADIUS = 2.5;
        sf::CircleShape circle(RADIUS);
        circle.setFillColor(sf::Color(255, 0, 0));
        circle.setPosition(point.x - RADIUS, point.y - RADIUS);
        texture.draw(circle);
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
}

int main()
{
    pg::StdNumberGenerator rngenerator;
    
    TestVoronoi(rngenerator);

    return EXIT_SUCCESS;
}

