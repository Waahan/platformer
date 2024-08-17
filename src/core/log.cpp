#include "log.h"

#include <cstdio>

#ifdef _WIN32
    #include <windows.h>
#endif

namespace logging
{
    void print(const std::string& message, std::initializer_list<outputAffects> affects)
    {
    /*
        print - outputs string to terminal with affects
    */
        /*
            \033[XXXm

            XXX is ; seperated list of numbers for formating
        */
        constexpr const char* const beginEscapeSequence = "\033[";
        constexpr const char* const endSequence = "m";
        constexpr const char* const resetOutput = "\033[0m"; 

        /*
            Windows ANSI support code.
        */
#ifdef _WIN32
        static bool enabledAnsi = false;

        if(!enabledAnsi)
        {
            SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), ENABLE_VIRTUAL_TERMINAL_PROCESSING);
            enabledAnsi = true;
        }
#endif

        std::string formattedMessage{beginEscapeSequence};

        formattedMessage.reserve(affects.size() * 2);

        for(auto current = affects.begin(); current != affects.end(); current++)
        {
            formattedMessage += std::to_string(static_cast<unsigned char>(*current));

            if(current + 1 != affects.end())
                formattedMessage += ";";
        }

        formattedMessage += endSequence;

        formattedMessage += message;

        formattedMessage += resetOutput;

        //buffer size count stream
        std::fwrite(formattedMessage.c_str(), sizeof(std::string::value_type), formattedMessage.size(), stdout);
    }
}
