#include <iostream>
#include <random>
#include <cstdlib>
#include <vector>
#include <string>

#include <SFML/Graphics.hpp>

#include "random/StdNumberGenerator.h"
#include "random/NameGenerator.h"
#include "noise/PerlinNoise.h"
    
const unsigned int WIDTH = 640;
const unsigned int HEIGHT = 640;
const size_t NOISE_DIM = 16;

void refreshImage(sf::Image &image, const pg::PerlinNoiseUniformFloat<3> &noise,
                  float z)
{
    for(unsigned int y = 0; y < HEIGHT; ++y)
    {
        for(unsigned int x = 0; x < WIDTH; ++x)
        {
            float fx = x * NOISE_DIM / static_cast<float>(WIDTH);
            float fy = y * NOISE_DIM / static_cast<float>(HEIGHT);
            sf::Uint8 value = noise({fx, fy, z}) * 255.f;

            image.setPixel(x, y, sf::Color(value, value, value));
        }
    }
}

int main()
{
    pg::StdNumberGenerator rngenerator;
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

    auto noise = pg::PerlinNoiseUniformFloat<3>
            (rngenerator, {NOISE_DIM, NOISE_DIM, NOISE_DIM});
    
    sf::Image image;
    image.create(640, 640);

    float z = NOISE_DIM / 2.f;
    refreshImage(image, noise, z);
    
    sf::Texture texture;
    texture.loadFromImage(image);

    sf::Sprite sprite;
    sprite.setTexture(texture);

    sf::RenderWindow window(sf::VideoMode(640, 640), "Sample");
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
    return EXIT_SUCCESS;
}

