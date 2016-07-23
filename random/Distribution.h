#ifndef DISTRIBUTION_H
#define DISTRIBUTION_H

namespace pg
{

    template<typename result_type, template<typename> class Dist>
    class Distribution
    {
        public:
            Distribution(const Dist<result_type> &dist):
                internalDistribution(dist)
            {
            }
            
            virtual ~Distribution() = default;

            template<class Generator>
            result_type operator()(Generator &g)
            {
                return internalDistribution(g);
            }

            result_type min() const
            {
                return internalDistribution.min();
            }

            result_type max() const
            {
                return internalDistribution.max();
            }

        protected:
            Dist<result_type> internalDistribution;
    };

    pg::Distribution<unsigned int, std::uniform_int_distribution>
        CreateDistributionUniformUint(unsigned int min, unsigned int max)
    {
        std::uniform_int_distribution<unsigned int> dist(min, max);
        return pg::Distribution<unsigned int, std::uniform_int_distribution>
               (dist);
    }
}


#endif

