#ifndef RANDOM_ENGINE_HPP
#define RANDOM_ENGINE_HPP

#include "NumberGenerator.hpp"
#include "Distribution.hpp"

namespace pg
{
    template <typename ResultType, template<typename> class Dist>
    class RandomEngine
    {
        public:
            RandomEngine(pg::NumberGenerator &gen,
                         const pg::Distribution<ResultType, Dist> &dis):
                generator(gen),
                distribution(dis)
            {
            }

            virtual ~RandomEngine() = default;

            ResultType operator()()
            {
                return distribution(generator);
            }

            ResultType min() const
            {
                return distribution.min();
            }

            ResultType max() const
            {
                return distribution.max();
            }

        protected:
            pg::NumberGenerator &generator;
            pg::Distribution<ResultType, Dist> distribution;
    };

    template <typename ResultType, template<typename> class Dist>
    RandomEngine<ResultType, Dist>
        CreateRandomEngine(pg::NumberGenerator &generator,
                           pg::Distribution<ResultType, Dist> &distribution)
    {
        return RandomEngine<ResultType, Dist> (generator, distribution);
    }
}

#endif

