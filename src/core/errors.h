#pragma once
/*
    A basic error handling system.

    Note C style error handling with return values should be used when errors can be recovered from.
*/

#include "log.h"

/*
    runtimeAssert - checks assertion even when not debugging at runtime
    ONLY USE FOR NONE RECOVERABLE ERRORS

    ERROR: errorMessage File: __FILE__ Function: __func__ Line: __LINE__
*/
#define releaseAssert(condition, errorMessage) if(!condition) \
    logging::print( std::string("ERROR: ") + errorMessage + " File: " + __FILE__ + " Function: " + __func__ + " Line: " + std::to_string(__LINE__) + "\n", {logging::outputAffects::red, logging::outputAffects::bold, logging::outputAffects::underline})

/*
    debugAssert - checks assertion when debugging else does nothing
*/
//Check for debugging enviorment
#ifdef NDEBUG
    #define debugAssert(condition, errorMessage) ((void)0)
#else
    #define debugAssert(condition, errorMessage) releaseAssert(condition, errorMessage) 
#endif
