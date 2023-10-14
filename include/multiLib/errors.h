#pragma once
/*
    The error handling model is that things like window size is checkend for with an assert. 
    Where as SDL2 errors are handled with runtimeAssert and things at compile time are handled
    with static assert.
*/

#ifdef BUILD_RUNTIME_ERRORS 
    #include <iostream>
    #define runtimeAssert(condition, errorFunc) if(!conditon) std::cerr << "Error: File: " << __FILE__ << " Function:  "<< __func__ << " Line: " << __LINE__ << " Error message: " << errorFunc << std::endl; exit(1);
#else
    #define runtimeAssert(condition, errorFunc) ((void)0)
#endif
