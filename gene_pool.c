#include "gene_pool.h"

file_map_t * open_file(const char *address, map_mode_t mode) {

    int descriptor;
    if ((descriptor = open(address, O_RDONLY, 0)) < 0) {
        ERROR_LEVEL = ERR_FILE_CANNOT_OPEN;
        return NULL;
    }

    struct stat st;
    if (fstat(descriptor, &st) < 0) {
        ERROR_LEVEL = ERR_FILE_CANNOT_FSTAT;
        close(descriptor);
        return NULL;
    }
    size_t size = st.st_size;

    void *data = mmap(
        NULL, size,
        PROT_READ, MAP_PRIVATE,    // read options, flags
        descriptor,
        0);                        // offset
    if (data == MAP_FAILED) {
        ERROR_LEVEL = ERR_FILE_CANNOT_MMAP;
        close(descriptor);
        return NULL;
    }

    file_map_t *mapping = malloc(sizeof(file_map_t));
    if (mapping == NULL) {
        ERROR_LEVEL = ERR_CANNOT_MALLOC;
        close(descriptor);
        munmap(data, size);
        return NULL;
    }

    mapping->descriptor = descriptor;
    mapping->size       = size;
    mapping->data       = data;

    return mapping;

}

void close_file(file_map_t *mapping) {

    munmap(mapping->data, mapping->size);
    close(mapping->descriptor);
    free(mapping);

}

pool_t * open_pool(const char *address) {

    file_map_t *mapping = open_file(address, OPEN_MODE_READ);
    if (ERROR_LEVEL != ERR_OK) return NULL;

    if (mapping->size < (POOL_FILE_MIN_SAFE_BIT_SIZE / 8)) {
        ERROR_LEVEL = ERR_POOL_CORRUPT_TOO_SMALL;
        close_file(mapping);
        return NULL;
    }

    pool_file_preamble_t *preamble = mapping->data;

    if (preamble->initial_byte != POOL_INITIAL_BYTE) {
        ERROR_LEVEL = ERR_POOL_CORRUPT_INITIAL;
        close_file(mapping);
        return NULL;
    }

    if (preamble->initial_byte != POOL_META_INITIAL_BYTE) {
        ERROR_LEVEL = ERR_POOL_CORRUPT_METADATA_START;
        close_file(mapping);
        return NULL;
    }

    pool_t *pool = malloc(sizeof(pool_t));
    pool->file_mapping = mapping;

    pool->metadata_byte_size = preamble->metadata_byte_size;
    pool->metadata = (uint8_t *)preamble->metadata;

    pool->node_id_part_bit_size = preamble->node_id_part_bit_size;
    pool->weight_part_bit_size = preamble->weight_part_bit_size;

    return pool;

}
