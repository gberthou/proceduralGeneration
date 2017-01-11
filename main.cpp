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

#include "TileType.h"
#include "MeshSpriteGroup.h"
#include "IslandGenerator.h"

struct Direction
{
    sf::Keyboard::Key keycode;
    bool enabled;
};

int main()
{
    pg::StdNumberGenerator rngenerator;

    const unsigned int WIDTH = 640;
    const unsigned int HEIGHT = 640;

    IslandGenerator islandGenerator(WIDTH, HEIGHT);
    pg::VoronoiMesh<float, TileType> map(rngenerator,
                                         islandGenerator, 8, 8, 120, 120);

    MeshSpriteGroup meshSpriteGroup(map, WIDTH, HEIGHT);
    
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Voronoi");
    window.setFramerateLimit(60);

    float x = 0;
    float y = 0;
    const float MOVE_VELOCITY = 4;

    const size_t DIRECTION_COUNT = 4;
    Direction directions[DIRECTION_COUNT] = {{sf::Keyboard::Left, false},
                                             {sf::Keyboard::Right, false},
                                             {sf::Keyboard::Up, false},
                                             {sf::Keyboard::Down, false}};

    while(window.isOpen())
    {
        sf::Event event;
        while(window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed)
                window.close();
            else if(event.type == sf::Event::KeyPressed)
            {
                for(size_t i = 0; i < DIRECTION_COUNT; ++i)
                    if(event.key.code == directions[i].keycode)
                    {
                        directions[i].enabled = true;
                        break;
                    }
            }
            else if(event.type == sf::Event::KeyReleased)
            {
                for(size_t i = 0; i < DIRECTION_COUNT; ++i)
                    if(event.key.code == directions[i].keycode)
                    {
                        directions[i].enabled = false;
                        break;
                    }
            }
        }

        if(directions[0].enabled)
            x -= MOVE_VELOCITY;
        else if(directions[1].enabled)
            x += MOVE_VELOCITY;
        if(directions[2].enabled)
            y -= MOVE_VELOCITY;
        else if(directions[3].enabled)
            y += MOVE_VELOCITY;

        meshSpriteGroup.setPosition(x, y);
        meshSpriteGroup.Refresh();

        window.clear();
        window.draw(meshSpriteGroup);
        window.display();
    }

    return EXIT_SUCCESS;
}

