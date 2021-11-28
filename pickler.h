#pragma once

#include <stdlib.h>
#include <stdint.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "error.h"

#define BYTE_SIZE sizeof(uint8_t)

typedef struct file_map_s {
    int    descriptor;
    size_t size;
    void  *data;
} file_map_t;

typedef enum map_mode_e { OPEN_MODE_READ, OPEN_MODE_WRITE } map_mode_t;

file_map_t * open_file(const char *address, map_mode_t mode);
void close_file(file_map_t *mapping);

void copy_bitslots_to_uint64(
    uint8_t *slots, uint64_t *number,
    uint8_t start, uint8_t end);
