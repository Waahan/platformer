/*
    Log - a very simple and fast logging library
*/
#pragma once

#include <string>
#include <initializer_list>

namespace logging
{
    enum class outputAffects : unsigned char {none = 0, bold = 1, underline = 4, red = 31, green = 32, yellow = 33, blue = 34, purple = 35};

    /*
        print - outputs string to terminal
    */
    void print(const std::string& message, std::initializer_list<outputAffects> affects = {});
} 
