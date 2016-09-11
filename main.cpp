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

#include "TileType.h"
#include "MeshSprite.h"
#include "IslandGenerator.h"

int main()
{
    pg::StdNumberGenerator rngenerator;

    const unsigned int WIDTH = 640;
    const unsigned int HEIGHT = 640;

    IslandGenerator islandGenerator(WIDTH, HEIGHT);
    pg::VoronoiMesh<float, TileType> map(islandGenerator, 8, 8, 120, 120);

    MeshSprite meshSprite(rngenerator, map, WIDTH, HEIGHT);
    
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
        window.draw(meshSprite);
        window.display();
    }

    return EXIT_SUCCESS;
}

