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

/* @function fill_with_randomness
*  @return void
*  @argument uint8*
*  @argument uint32
*  @argument uint8
*/
void fill_with_randomness(uint8_t *destination, uint32_t bytes, uint8_t bits);

/* @enum generator_mode
*  @type uint8
*  @member GENERATE_RANDOMNESS (1 << 0)
*  @member GENERATE_ZEROS      (1 << 1)
*/
typedef enum generator_mode_e {
    GENERATE_RANDOMNESS = (uint8_t)(1 << 0),
    GENERATE_ZEROS      = (uint8_t)(1 << 1)
} generator_mode_t;

/* @function generate_random_genome_data
*  @return void
*  @argument genome*
*  @argument uint64
*  @argument uint8
*  @argument generator_mode
*/
void generate_random_genome_data(
    genome_t * const, uint64_t bits_number, uint8_t gene_byte_size,
    generator_mode_t
);

/* @function allocate_genome
*  @return genome*
*  @argument bool
*  @argument uint32
*  @argument uint8
*  @argument uint32
*/
genome_t * allocate_genome(
    bool allocate_data,
    uint32_t length, uint8_t gene_bytes_size,
    uint32_t genome_bit_size
);

/* @function allocate_genome_vector
*  @return genome**
*  @argument uint64
*  @argument bool
*  @argument uint32
*  @argument uint8
*  @argument uint32
*/
genome_t ** allocate_genome_vector (
    uint64_t size, bool allocate_data,
    uint32_t genes_number, uint8_t gene_bytes_size,
    uint32_t genome_bit_size
);

/* @function destroy_genomes_vector
*  @return void
*  @argument uint64
*  @argument bool
*  @argument bool
*  @argument genome**
*/
void destroy_genomes_vector(
    uint64_t size, bool deallocate_data, bool destroy_each_genome,
    genome_t ** const genomes
);

/* @function destroy_genome
*  @return void
*  @argument genome*
*  @argument bool
*/
void destroy_genome(genome_t * const genome, bool deallocate_data);

/* @function assign_genome_metadata
*  @return void
*  @argument genome*
*  @argument uint16
*  @argument char*
*/
void assign_genome_metadata(
    genome_t * const, uint16_t metadata_byte_size, const char *metadata
);

/* @function delete_genome_metadata
*  @return void
*  @argument genome*
*/
void delete_genome_metadata(genome_t * const genome);

/* @function allocate_pool
*  @return pool*
*/
pool_t * allocate_pool();

/* @function destroy_pool
*  @return void
*  @argument pool*
*  @argument bool
*/
void destroy_pool(pool_t * const pool, bool close_file);

/* @function assign_pool_metadata
*  @return void
*  @argument pool*
*  @argument uint16
*  @argument char*
*/
void assign_pool_metadata(
    pool_t * const pool, uint16_t metadata_byte_size, const char* metadata
);

/* @function delete_pool_metadata
*  @return void
*  @argument pool*
*/
void delete_pool_metadata(pool_t * const pool);

/* @function fill_pool
*  @return void
*  @argument char*
*  @argument population*
*  @argument uint64
*  @argument generator_mode
*/
void fill_pool(
    const char *address, population_t * const,
    uint64_t genome_bit_size,
    generator_mode_t
);

/* @function create_pool_in_file
*  @return population*
*  @argument uint64
*  @argument uint8
*  @argument uint8
*  @argument uint64
*  @argument uint64
*  @argument uint64
*  @argument generator_mode
*/
population_t * create_pool_in_file(
    uint64_t organisms_number,
    uint8_t node_id_bit_size, uint8_t weight_bit_size,
    uint64_t input_neurons_number, uint64_t output_neurons_number,
    uint64_t genome_bit_size,
    generator_mode_t
);

/* @function destroy_population
*  @return void
*  @argument population*
*  @argument bool
*  @argument bool
*  @argument bool
*/
void destroy_population(
    population_t * const population,
    bool destroy_genomes, bool deallocate_genomes_data,
    bool close_pool_file
);
