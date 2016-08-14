#include <iostream>
#include <random>
#include <cstdlib>
#include <vector>
#include <string>

#include <SFML/Graphics.hpp>

#include "../random/StdNumberGenerator.hpp"
#include "../random/Distribution.hpp"
#include "../algorithm/voronoi.hpp"
    
const unsigned int WIDTH = 640;
const unsigned int HEIGHT = 640;

void TestVoronoi(pg::StdNumberGenerator &rngenerator)
{
    std::vector<pg::MapPoint<float>> points;
    pg::CreateRandomizedGrid<float>(rngenerator, points,
                                    WIDTH / 4, WIDTH * 3 / 4,
                                    HEIGHT / 4, HEIGHT * 3 / 4,
                                    16, 16);
    pg::RectVoronoiMap<float> map(WIDTH/4, WIDTH*3/4, HEIGHT/4, HEIGHT*3/4,
                                  points);

    sf::RenderTexture texture;
    texture.create(WIDTH, HEIGHT);

    for(auto point : points)
    {
        const float RADIUS = 2.5;
        sf::CircleShape circle(RADIUS);
        circle.setFillColor(sf::Color(255, 0, 0));
        circle.setPosition(point.x - RADIUS, point.y - RADIUS);
        texture.draw(circle);
    }

    auto vertices = map.GetVertices();
    for(auto edge : map.GetEdges())
    {
        sf::Vertex line[] =
        {
            sf::Vertex(sf::Vector2f(vertices[edge.p0].x, vertices[edge.p0].y)),
            sf::Vertex(sf::Vector2f(vertices[edge.p1].x, vertices[edge.p1].y))
        };

        texture.draw(line, 2, sf::Lines);
    }
    
    for(auto point : map.GetVertices())
    {
        const float RADIUS = 1;
        sf::CircleShape circle(RADIUS);
        circle.setFillColor(sf::Color(0, 255, 0));
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

