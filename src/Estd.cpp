#include <iostream>

#include "Estd.h"

namespace Estd
{
    randomNumberGenerator& randomNumberGenerator::get()
    {
        static randomNumberGenerator instance;

        return instance;
    }

    timer::~timer()
    {
        stop();
    }

    void timer::stop() const
    {
        std::chrono::time_point<std::chrono::steady_clock> end = std::chrono::steady_clock::now();
        
        std::cout << "Timer " << name << " milliseconds: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;
    }
}
