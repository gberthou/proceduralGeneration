#ifndef NAME_GENERATOR_H
#define NAME_GENERATOR_H

#include <vector>
#include <string>
#include <random>
#include <stdexcept>

#include "RandomEngine.h"

namespace pg
{

template <template<typename> class DistC, template<typename> class DistI>
class NameGenerator
{
    public:
        NameGenerator(NumberGenerator &generator,
                      const std::vector<std::string> &syl,
                      pg::Distribution<unsigned int, DistC> &sylCntDist,
                      pg::Distribution<unsigned int, DistI> &sylIndDist):
            syllableCountEngine(generator, sylCntDist),
            syllableIndexEngine(generator, sylIndDist),
            syllables(syl)
        {
        }

        virtual ~NameGenerator() = default;

        std::string operator()()
        {
            std::string ret;
            unsigned int syllableCount = syllableCountEngine();

            for(unsigned int i = 0; i < syllableCount; ++i)
            {
                unsigned int index = syllableIndexEngine();
                if(index >= syllables.size())
                {
                    throw std::runtime_error("NameGenerator::operator()  Index "
                                             "out of bounds");
                }
                ret += syllables[index];
            }

            return ret;
        }

    protected:
        pg::RandomEngine<unsigned int, DistC> syllableCountEngine;
        pg::RandomEngine<unsigned int, DistI> syllableIndexEngine;
        const std::vector<std::string> &syllables;
};

template <template<typename> class DistC, template<typename> class DistI>
pg::NameGenerator<DistC, DistI> CreateNameGenerator(
        NumberGenerator &generator, const std::vector<std::string> &syl,
        pg::Distribution<unsigned int, DistC> &sylCntDist,
        pg::Distribution<unsigned int, DistI> &sylIndDist)
{
    return pg::NameGenerator<DistC, DistI>(generator, syl, sylCntDist, sylIndDist);
}

pg::NameGenerator<std::uniform_int_distribution, std::uniform_int_distribution>
    CreateNameGenerator(NumberGenerator &generator,
                        const std::vector<std::string> &syl,
                        unsigned int sylCountMin,
                        unsigned int sylCountMax)
{
    auto sylCntDist = pg::CreateDistributionUniformUint(sylCountMin,
                                                        sylCountMax);
    auto sylIndDist = pg::CreateDistributionUniformUint(0, syl.size() - 1);
    return pg::NameGenerator<std::uniform_int_distribution,
                             std::uniform_int_distribution>
        (generator, syl, sylCntDist, sylIndDist);
}

}

#endif

