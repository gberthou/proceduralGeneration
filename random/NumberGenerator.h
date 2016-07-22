#ifndef NUMBER_GENERATOR_H
#define NUMBER_GENERATOR_H

namespace pg
{

class NumberGenerator
{
    public:
        typedef unsigned int result_type;

        NumberGenerator() = default;
        virtual ~NumberGenerator() = default;

        virtual result_type operator()() = 0;
        virtual result_type min() = 0;
        virtual result_type max() = 0;
        
};
}

#endif

