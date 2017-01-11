#include <iostream>
#include <random>
#include <cstdlib>
#include <vector>
#include <string>

#include <SFML/Graphics.hpp>

#include "../random/StdNumberGenerator.hpp"
#include "../noise/PerlinNoise2.hpp"
    
const unsigned int WIDTH = 640;
const unsigned int HEIGHT = 640;
const size_t NOISE_DETAIL = 32;

void refreshImage(sf::Image &image, pg::PerlinNoiseUniformFloat<3> &noise,
                  float z)
{
    for(unsigned int y = 0; y < HEIGHT; ++y)
    {
        float fy = y * NOISE_DETAIL / static_cast<float>(HEIGHT);

        for(unsigned int x = 0; x < WIDTH; ++x)
        {
            float fx = x * NOISE_DETAIL / static_cast<float>(WIDTH);
            sf::Uint8 value = noise({fx, fy, z}) * 255.f;

            image.setPixel(x, y, sf::Color(value, value, value));
        }
    }
}

void TestPerlinNoise(pg::StdNumberGenerator &rngenerator)
{
    auto noise = pg::PerlinNoiseUniformFloat<3>
            (rngenerator);

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
        else if(z >= NOISE_DETAIL)
        {
            z = NOISE_DETAIL - velocity;
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

int main()
{
    pg::StdNumberGenerator rngenerator;
    
    TestPerlinNoise(rngenerator);

    return EXIT_SUCCESS;
}

