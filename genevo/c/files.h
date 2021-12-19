/*

This header contains declarations for file mappings.

TODO: add Windows support.

*/

#pragma once

#include <stdlib.h>
#include <stdint.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "error.h"

typedef struct file_map_s {
    int    descriptor;
    size_t size;
    void  *data;
} file_map_t;

typedef enum map_mode_e { OPEN_MODE_READ, OPEN_MODE_WRITE } map_mode_t;

file_map_t * open_file(
    const char *address, map_mode_t mode, size_t trunc_to_size);
void close_file(file_map_t *mapping);
