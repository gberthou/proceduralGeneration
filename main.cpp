#include <iostream>
#include <random>
#include <cstdlib>
#include <vector>
#include <string>

#include <SFML/Graphics.hpp>

#include "random/StdNumberGenerator.h"
#include "random/NameGenerator.h"
#include "noise/PerlinNoise.h"
#include "algorithm/voronoi.h"
    
const unsigned int WIDTH = 640;
const unsigned int HEIGHT = 640;
const size_t NOISE_DIM_X = 16;
const size_t NOISE_DIM_Y = 16;
const size_t NOISE_DIM_Z = 4;

void TestNameGenerator(pg::StdNumberGenerator &rngenerator)
{
    const std::vector<std::string> syllables = {
        "ar", "yn", "ith", "min", "zor", "um", "ul", "rog", "pyr", "ba", "pa",
        "chi", "ski", "lap", "vid", "por", "til", "hul", "jor", "den", "er",
        "mar", "ing", "hil", "kar", "ver", "pro", "plo", "pra", "pla", 
        "yck", "dag", "van", "lig", "kom", "tar", "tin", "lit", "ten", "tet",
        "eng", "syl", "var", "dys", "in", "myt", "tul", "sur", "fur", "na",
        "ung", "dom", "'"};
 
    auto nameGenerator = pg::CreateNameGenerator(rngenerator, syllables,
                                                 1, 7);

    for(unsigned int i = 0; i < 10; ++i)
        std::cout << nameGenerator() << std::endl;
}

void refreshImage(sf::Image &image,const pg::PerlinNoiseUniformFloat<3> &noise,
                  float z)
{
    for(unsigned int y = 0; y < HEIGHT; ++y)
    {
        float fy = y * NOISE_DIM_Y / static_cast<float>(HEIGHT);

        for(unsigned int x = 0; x < WIDTH; ++x)
        {
            float fx = x * NOISE_DIM_X / static_cast<float>(WIDTH);
            sf::Uint8 value = noise({fx, fy, z}) * 255.f;

            image.setPixel(x, y, sf::Color(value, value, value));
        }
    }
}

void TestPerlinNoise(pg::StdNumberGenerator &rngenerator)
{
    auto noise = pg::PerlinNoiseUniformFloat<3>
            (rngenerator, {NOISE_DIM_X, NOISE_DIM_Y, NOISE_DIM_Z});

    sf::Image image;
    image.create(WIDTH, HEIGHT);

    float z = 0;
    refreshImage(image, noise, z);
    
    sf::Texture texture;
    texture.loadFromImage(image);

    sf::Sprite sprite;
    sprite.setTexture(texture);

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Perlin noise");
    window.setFramerateLimit(60);
    
    float velocity = 0.025f;
    while(window.isOpen())
    {
        sf::Event event;
        while(window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed)
                window.close();
        }
        
        z += velocity;
        if(z < 0)
        {
            z = 0;
            velocity *= -1;
        }
        else if(z >= NOISE_DIM_Z)
        {
            z = NOISE_DIM_Z - velocity;
            velocity *= -1;
        }

        refreshImage(image, noise, z);
        texture.loadFromImage(image);
        sprite.setTexture(texture);

        window.clear();
        window.draw(sprite);
        window.display();
    }
}

void TestVoronoi(pg::StdNumberGenerator &rngenerator)
{
    std::vector<pg::Point<float>> points;

    auto distributionX = pg::CreateDistributionUniformUint(WIDTH/4, WIDTH*3/4);
    auto distributionY = pg::CreateDistributionUniformUint(HEIGHT/4,HEIGHT*3/4);
    const size_t N_POINTS = 16;
    for(size_t i = 0; i < N_POINTS; ++i)
    {
        pg::Point<float> tmp;
        tmp[0] = distributionX(rngenerator);
        tmp[1] = distributionY(rngenerator);
        points.push_back(tmp);
    }

    std::vector<pg::Triangle<float>> triangles;
    pg::Voronoi<float>(points, triangles);
    
    sf::RenderTexture texture;
    texture.create(WIDTH, HEIGHT);

    for(auto triangle : triangles)
    {
        sf::ConvexShape polygon(3);
        for(size_t i = 0; i < triangle.size(); ++i)
            polygon.setPoint(i, {triangle[i][0], triangle[i][1]});
        polygon.setOutlineColor(sf::Color(255, 255, 255));
        polygon.setOutlineThickness(2);
        polygon.setFillColor(sf::Color::Transparent);
        texture.draw(polygon);
    }

    for(auto point : points)
    {
        sf::CircleShape circle(5);
        circle.setFillColor(sf::Color(255, 0, 0));
        circle.setPosition(point[0], point[1]);
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
    
    TestNameGenerator(rngenerator);
    //TestPerlinNoise(rngenerator);
    TestVoronoi(rngenerator);

    return EXIT_SUCCESS;
}

