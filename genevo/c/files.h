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
    int     descriptor;
    size_t  size;
    void   *data;
} file_map_t;

typedef enum map_mode_e {
    OPEN_MODE_READ  = (uint8_t)(1 << 0),
    OPEN_MODE_WRITE = (uint8_t)(1 << 1)
} map_mode_t;

void set_file_size(int descriptor, size_t new_size);
file_map_t * open_file(
    const char *address, map_mode_t mode, size_t trunc_to_size);
void close_file(file_map_t * const mapping);
