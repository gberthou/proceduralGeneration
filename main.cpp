#include <iostream>
#include <random>
#include <cstdlib>
#include <vector>
#include <string>

#include <SFML/Graphics.hpp>

#include "random/StdNumberGenerator.h"
#include "random/NameGenerator.h"
#include "noise/PerlinNoise.h"

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

    const unsigned int NOISE_DIM = 16;

    pg::Distribution<float, std::uniform_real_distribution>
        distribution(std::uniform_real_distribution<float>{0,2.f*std::acos(-1)});
    pg::PerlinNoise2D<float, std::uniform_real_distribution>
        noise(rngenerator, distribution, NOISE_DIM, NOISE_DIM);
    
    sf::Image image;
    image.create(640, 640);
    
    for(unsigned int y = 0; y < 640; ++y)
    {
        for(unsigned int x = 0; x < 640; ++x)
        {
            float fx = x * NOISE_DIM / 640.f;
            float fy = y * NOISE_DIM / 640.f;
            sf::Uint8 value = noise(fx, fy) * 255.f;

            image.setPixel(x, y, sf::Color(value, value, value));
        }
    }

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

