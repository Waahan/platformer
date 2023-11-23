#pragma once
/*
    The error handling model is that things like window size is checkend for with an debug_assert. 
    Where as SDL2 errors are handled with runtime_assert and things at compile time are handled
    with static assert. The game should never just exit from an error with a release build.

    I replaced assert with debug assert because its more clear and explicit.
*/

//Temp fix
#define BUILD_RUNTIME_ERRORS 1 

#ifdef BUILD_RUNTIME_ERRORS
    #include <iostream>
    #define runtime_assert(condition, errorMessage) \
    if(!condition) \
        std::cerr << "Error: File: " << __FILE__ << " Function:  "<< __func__ << " Line: " << __LINE__ << " Error message: " << errorMessage << std::endl

    #define runtime_error(errorMessage) \
    std::cerr << "Error: File: " << __FILE__ << " Function:  "<< __func__ << " Line: " << __LINE__ << " Error message: " << errorMessage << std::endl
#else
    #define runtime_assert(condition, errorMessage) ((void)0)
    #define runtime_error(errorMessage) ((void)0)
#endif

#ifdef NDEBUG
    #define debug_assert(condition, errorMessage) ((void)0)
#else
/*
    I made my own assert because I don't like the c librarys assert
*/
    #include <iostream>
    #define debug_assert(condition, errorMessage) \
    if(!condition) \
        std::cerr << "Error: File: " << __FILE__ << " Function:  "<< __func__ << " Line: " << __LINE__ << " Error message: " << errorMessage << std::endl

    #define debug_error(errorMessage) \
    std::cerr << "Error: File: " << __FILE__ << " Function:  "<< __func__ << " Line: " << __LINE__ << " Error message: " << errorMessage << std::endl
#endif
