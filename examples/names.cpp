#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>

#include "../random/StdNumberGenerator.hpp"
#include "../random/NameGenerator.hpp"

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

int main()
{
    pg::StdNumberGenerator rngenerator;
    TestNameGenerator(rngenerator);

    return EXIT_SUCCESS;
}

