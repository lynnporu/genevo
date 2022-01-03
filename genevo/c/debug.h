#pragma once

/*

This header was intended for debug purposes only. It provides convenient
functions for making breakpoint in functions and outputting the logs into a
file.

Do not include this header into any of production sources.

*/

#include <stdio.h>

#define DEBUG_MODE(_MODE)                                        \
    char _debug_file_name[100] = {0};                            \
    sprintf(_debug_file_name, "logs--%s.txt", __FUNCTION__);     \
    FILE *_debug_file_descr = fopen(_debug_file_name, _MODE);

#define DEBUG DEBUG_MODE("w+")

#define BREAKPOINT(_FORMAT, ...)                                 \
    fprintf(_debug_file_descr, _FORMAT __VA_OPT__(,) __VA_ARGS__);

#define CLOSE_DEBUG(var)                                         \
    fprintf(_debug_file_descr, "\n");                            \
    fflush(_debug_file_descr);                                   \
    fclose(_debug_file_descr);

#define END_RUNTIME_VOID                                         \
    CLOSE_DEBUG;                                                 \
    return;

#define END_RUNTIME_RETURN(var)                                  \
    CLOSE_DEBUG;                                                 \
    return var;
