#ifndef DDRAIGOS_DEBUG_H
#define DDRAIGOD_DEBUG_H

#include "ddraig.h"
#include "printf.h"

//#define DEBUG_PRINT

#ifdef DEBUG_PRINT
    #define debug_printf(fmt, ...) printf(fmt, __VA_ARGS__)
    #define debug_print(str) printf(str)
#else
    #define debug_printf(fmt, ...)
    #define debug_print(str)
#endif

#endif
