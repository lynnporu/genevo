#pragma once

#include <stdint.h>

#include "pool.h"
#include "rand.h"
#include "string.h"
#include "stdbool.h"
#include "pickler.h"

#define fill_bytes_with_randomness(_DESTINATION, _BYTES)                       \
    fill_with_randomness(_DESTINATION, _BYTES, 0)

#define fill_bits_with_randomness(_DESTINATION, _BITS)                         \
    fill_with_randomness(_DESTINATION, (uint32_t)(_BITS / 8), _BITS % 8)

void fill_with_randomness(uint8_t *destination, uint32_t bytes, uint8_t bits);

#define GENERATE_RANDOMNESS (uint8_t)(0x1 << 0)
#define GENERATE_ZEROS      (uint8_t)(0x1 << 1)
typedef uint8_t generator_mode_t;

void generate_random_genome_data(
    genome_t *, uint64_t bits_number, uint8_t gene_byte_size,
    generator_mode_t
);

genome_t *allocate_genome(
    bool allocate_data,
    uint64_t genes_bytes_size, uint16_t residue_size_bits
);
void destroy_genome(genome_t *genome, bool deallocate_data);

void assign_genome_metadata(
    genome_t *, uint16_t metadata_byte_size, const char *metadata
);
void delete_genome_metadata(genome_t *genome);

pool_t *allocate_pool();
void destroy_pool(pool_t *pool, bool close_file);

void assign_pool_metadata(
    pool_t *pool, uint16_t metadata_byte_size, const char* metadata
);
void delete_pool_metadata(pool_t *pool);

pool_and_genomes_t *fill_pool(
    const char *address, pool_t *pool, uint64_t genome_bit_size,
    generator_mode_t
);

void destroy_pool_and_genomes(
    pool_and_genomes_t *pool_and_genomes,
    bool destroy_genomes, bool deallocate_genomes_data
);
