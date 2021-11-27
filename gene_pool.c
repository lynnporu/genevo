#include "gene_pool.h"

#define MAPPING_FAIL_CONDITION(condition, err_const) \
    if(condition) {ERROR_LEVEL = err_const; close_file(mapping); return NULL;}

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

pool_t * read_pool(const char *address) {

    file_map_t *mapping = open_file(address, OPEN_MODE_READ);
    if (ERROR_LEVEL != ERR_OK) return NULL;

    MAPPING_FAIL_CONDITION(
        mapping->size < (POOL_FILE_MIN_SAFE_BIT_SIZE / 8),
        ERR_POOL_CORRUPT_TOO_SMALL);

    pool_file_preamble_t *preamble = mapping->data;

    MAPPING_FAIL_CONDITION(
        preamble->initial_byte != POOL_INITIAL_BYTE,
        ERR_POOL_CORRUPT_INITIAL);

    MAPPING_FAIL_CONDITION(
        preamble->metadata_initial_byte != POOL_META_INITIAL_BYTE,
        ERR_POOL_CORRUPT_METADATA_START);

    MAPPING_FAIL_CONDITION(
        preamble->node_id_part_bit_size > 64,
        ERR_GENE_OGSB_TOO_LARGE);

    MAPPING_FAIL_CONDITION(
        preamble->weight_part_bit_size > 64,
        ERR_GENE_WEIGHT_TOO_LARGE);

    MAPPING_FAIL_CONDITION(
        (preamble->weight_part_bit_size +
         preamble->node_id_part_bit_size) % 8 != 0,
        ERR_GENE_NOT_ALIGNED);


    pool_t *pool = malloc(sizeof(pool_t));
    pool->file_mapping = mapping;

    pool->metadata_byte_size = preamble->metadata_byte_size;
    pool->metadata = (uint8_t *)preamble->metadata;

    pool->node_id_part_bit_size = preamble->node_id_part_bit_size;
    pool->weight_part_bit_size = preamble->weight_part_bit_size;
    pool->gene_bytes_size = (
        pool->node_id_part_bit_size * 2 +
        pool->weight_part_bit_size);

    pool->first_genome_start_position =
        preamble->metadata + preamble->metadata_byte_size;

    pool->cursor = pool->first_genome_start_position;

    free(preamble);
    return pool;

}

void close_pool(pool_t *pool) {
    close_file(pool->file_mapping);
    free(pool);
}

genome_t * read_next_genome(pool_t *pool) {

    genome_file_preamble_t *preamble = pool->cursor;

    if (preamble->initial_byte != GENOME_INITIAL_BYTE) {
        ERROR_LEVEL = ERR_GENM_CORRUPT_START;
        return NULL;
    }

    if (preamble->metadata_initial_byte != GENOME_META_INITIAL_BYTE) {
        ERROR_LEVEL = ERR_GENM_CORRUPT_METADATA_START;
        return NULL;
    }

    genome_t* genome = malloc(sizeof(genome_t));

    genome->length = preamble->genes_number;
    genome->metadata = preamble->metadata;
    genome->metadata_byte_size = preamble->metadata_byte_size;
    genome->genes = preamble->metadata + preamble->metadata_byte_size + 1;

    void * residue_byte = (
        genome->genes +
        pool->gene_bytes_size * genome->length +
        1);

    if (*(uint8_t *)residue_byte != GENOME_RESIDUE_BYTE) {
        ERROR_LEVEL = ERR_GENM_CORRUPT_RESIDUE;
        free(genome);
        return NULL;
    }

    genome->residue_size_bits = *(uint8_t *)(residue_byte + 1);
    genome->residue = residue_byte + 1;

    void * terminal_byte = residue_byte + 1 + sizeof(genome->residue_size_bits);

    if (*(uint8_t *)residue_byte != GENOME_TERMINAL_BYTE) {
        ERROR_LEVEL = ERR_GENM_CORRUPT_END;
        free(genome);
        return NULL;
    }

    pool->cursor = terminal_byte + 1;

    free(preamble);
    return genome;

}

gene_t * get_gene_by_index(
    genome_t *genome, uint32_t index, pool_t *pool
) {

    gene_t *gene = calloc(1, sizeof(gene_t));

    uint8_t gene_byte_size =
        (pool->node_id_part_bit_size + pool->weight_part_bit_size) / 8;

    uint8_t *gene_start_byte = genome->genes + (gene_byte_size * index);

    // TODO: avoid copying bits
    // TODO: fill connection_type
    // copy outcome node id
    for(
        uint8_t gene_cursor = 1,
                number_cursor = 0;
        gene_cursor <= pool->node_id_part_bit_size;
        gene_cursor--, number_cursor++
    )
        gene->outcome_node_id |= BIT_TEST(
            gene_start_byte,
            pool->node_id_part_bit_size - gene_cursor) << number_cursor;

    // copy income node id
    for(
        uint8_t gene_cursor = 1,
                number_cursor = 0;
        gene_cursor >= pool->node_id_part_bit_size;
        gene_cursor--, number_cursor++
    )
        gene->income_node_id |= BIT_TEST(
            gene_start_byte + pool->node_id_part_bit_size,
            pool->node_id_part_bit_size - gene_cursor) << number_cursor;

    // copy weight
    for(
        uint8_t gene_cursor = 1,
                number_cursor = 0;
        gene_cursor >= pool->weight_part_bit_size;
        gene_cursor--, number_cursor++
    )
        gene->weight_unnormalized |= BIT_TEST(
            gene_start_byte + (pool->node_id_part_bit_size * 2),
            pool->weight_part_bit_size - gene_cursor) << number_cursor;

    uint64_t normalization =
        pool->weight_part_bit_size == 64
        ? 0xffffffffffff
        : (1 << pool->weight_part_bit_size) - 1;

    gene->weight = gene->weight_unnormalized / normalization;

    return gene;

}
