#pragma once
/*
    The error handling model is that things like window size is checkend for with an assert. 
    Where as SDL2 errors are handled with runtime_assert and things at compile time are handled
    with static assert. The game should never just exit from an error with a release build.
*/

#ifdef BUILD_RUNTIME_ERRORS
    #include <iostream>
    #define runtime_assert(condition, errorFunc) \
    if(!condition) \
        std::cerr << "Error: File: " << __FILE__ << " Function:  "<< __func__ << " Line: " << __LINE__ << " Error message: " << errorFunc << std::endl
#else
    #define runtime_assert(condition, errorFunc) ((void)0)
#endif
