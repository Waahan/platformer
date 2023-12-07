#include "Estd.h"

namespace Estd
{
    std::string generateUUID(int length)
    {
    /*
        Generate a uuid as long as length
    */
        //What can be in the UUID
        const std::string options = "01234567890 qwertyuiopasdfghjklzxcvbnm";
        const int stringMax = options.size() - 1;

        randomNumberGenerator& randomSource = randomNumberGenerator.get();

        std::string uuid{};

        for(int iterations = 0; interations < length; iterations++)
        {
            int randomNumber = randomSource.generate(0, stringMax);

            uuid += options[randomNumber];
        }

        return uuid;
    }
} //Estd
