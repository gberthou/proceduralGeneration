#ifndef DISTRIBUTION_HPP
#define DISTRIBUTION_HPP

#include <random>

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

    using DistributionUniformUint =
        Distribution<unsigned int, std::uniform_int_distribution>;
    using DistributionUniformFloat =
        Distribution<float, std::uniform_real_distribution>;

    DistributionUniformUint
        CreateDistributionUniformUint(unsigned int min, unsigned int max);
    
    DistributionUniformFloat
        CreateDistributionUniformFloat(float min, float max);
}


#endif

