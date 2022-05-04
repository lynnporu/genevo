#pragma once

#include <stdlib.h>
#include <stdarg.h>

#define _CHECK_NULL_DESTINATION(_DESTINATION, _EXIT_CODE)                      \
    if (_DESTINATION == NULL) { ERROR_LEVEL = ERR_CANNOT_MALLOC; _EXIT_CODE; }

#define DECLARE_MALLOC_OBJECT(_TYPE, _DESTINATION, _EXIT_CODE)                 \
    _TYPE *_DESTINATION = malloc(sizeof(_TYPE));                               \
    _CHECK_NULL_DESTINATION(_DESTINATION, _EXIT_CODE);

#define DECLARE_CONST_MALLOC_OBJECT(_TYPE, _DESTINATION, _EXIT_CODE)           \
    _TYPE * const _DESTINATION = malloc(sizeof(_TYPE));                        \
    _CHECK_NULL_DESTINATION(_DESTINATION, _EXIT_CODE);

#define DECLARE_MALLOC_ARRAY(_TYPE, _DESTINATION, _SIZE, _EXIT_CODE)           \
    _TYPE *_DESTINATION = malloc(sizeof(_TYPE) * _SIZE);                       \
    _CHECK_NULL_DESTINATION(_DESTINATION, _EXIT_CODE);

#define DECLARE_MALLOC_LINKS_ARRAY(_TYPE, _DESTINATION, _SIZE, _EXIT_CODE)     \
    _TYPE **_DESTINATION = malloc(sizeof(_TYPE *) * _SIZE);                    \
    _CHECK_NULL_DESTINATION(_DESTINATION, _EXIT_CODE);

#define ASSIGN_MALLOC_ARRAY(_DESTINATION, _TYPE, _SIZE)                        \
    _DESTINATION = malloc(sizeof(_TYPE) * _SIZE);

#define ASSIGN_MALLOC_LINKS_ARRAY(_DESTINATION, _TYPE, _SIZE)                  \
    _DESTINATION = malloc(sizeof(_TYPE *) * _SIZE);

#define ASSIGN_CALLOC_ARRAY(_DESTINATION, _TYPE, _SIZE)                        \
    _DESTINATION = calloc(_SIZE, sizeof(_TYPE));

#define RETURN_NULL_ON_ERR {return NULL;}

#define RETURN_VOID_ON_ERR {return;}

#define DO_NOTHING_ON_ERR ;

#define FREE_NOT_NULL(_OBJECT)                                                 \
    if (_OBJECT != NULL) free(_OBJECT);

#define DESTROY_AND_EXIT(_DESTRUCTOR, _OBJECT, _EXIT_CODE) {                   \
    _DESTRUCTOR(_OBJECT);                                                      \
    ERROR_LEVEL = ERR_CANNOT_MALLOC;                                           \
    _EXIT_CODE; }
