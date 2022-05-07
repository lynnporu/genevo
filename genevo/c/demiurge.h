#pragma once

#include <stdint.h>
#include <stdio.h>

#include "pool.h"
#include "rand.h"
#include "string.h"
#include "stdbool.h"
#include "pickler.h"
#include "bit_manipulations.h"

/* @enum generator_mode
 * @type uint8
 * @member GENERATE_RANDOMNESS (1 << 0)
 * @member GENERATE_ZEROS      (1 << 1)
 */
typedef enum generator_mode_e {
    GENERATE_RANDOMNESS = (uint8_t)(1 << 0),
    GENERATE_ZEROS      = (uint8_t)(1 << 1)
} generator_mode_t;

void generate_random_genome_data(
    genome_t * const, const uint8_t gene_byte_size,
    const generator_mode_t
);

/* @function allocate_genome
 * @return genome*
 * @argument bool
 * @argument uint32
 * @argument uint8
 * @argument uint32
 */
genome_t * allocate_genome(
    const bool allocate_data,
    const genome_length_t, const uint8_t gene_bytes_size,
    const uint32_t genome_bit_size
);

/* @function allocate_genome_vector
 * @return genome**
 * @argument uint64
 * @argument bool
 * @argument uint32
 * @argument uint8
 * @argument uint32
 */
genome_t ** allocate_genome_vector (
    const pool_organisms_num_t, const bool allocate_data,
    const genome_length_t, const pool_gene_byte_size_t,
    const uint32_t genome_bit_size
);

/* @function duplicate_genome_vector
 * @return genome**
 * @argument uint64
 * @argument genome_duplicating_mode
 * @argument uint32
 * @argument uint8
 * @argument uint32
 * @argument genome**
 */
genome_t ** duplicate_genome_vector (
    const pool_organisms_num_t size, duplicating_mode_t mode,
    const genome_length_t genes_number, const uint8_t gene_bytes_size,
    const uint32_t genome_bit_size,
    const genome_t * const * const src
);

/* @function copy_genome_vector
 * @return void
 * @argument uint64
 * @argument genome_duplicating_mode
 * @argument uint32
 * @argument genome**
 * @argument genome**
 */
void copy_genome_vector (
    const pool_organisms_num_t size,
    const genome_t * const * const src,
    genome_t * const * const dst,
    const pool_gene_byte_size_t gene_bytes_size,
    duplicating_mode_t mode
);

/* @function copy_genome
 * @return void
 * @argument genome*
 * @argument genome*
 * @argument duplicating_mode
 * @argument uint8
 */
void copy_genome(
    const genome_t * const src, genome_t * const dst,
    duplicating_mode_t mode,
    pool_gene_byte_size_t gene_byte_size
);

/* @function destroy_genomes_vector
 * @return void
 * @argument uint64
 * @argument bool
 * @argument bool
 * @argument genome**
 */
void destroy_genomes_vector(
    const pool_organisms_num_t,
    const bool deallocate_data, const bool destroy_each_genome,
    genome_t ** const genomes
);

/* @function destroy_genome
 * @return void
 * @argument genome*
 * @argument bool
 */
void destroy_genome(genome_t * const genome, const bool deallocate_data);

/* @function assign_genome_metadata
 * @return void
 * @argument genome*
 * @argument uint16
 * @argument char*
 */
void assign_genome_metadata(
    genome_t * const, const genome_metadata_size_t, const char *metadata
);

/* @function delete_genome_metadata
 * @return void
 * @argument genome*
 */
void delete_genome_metadata(genome_t * const genome);

/* @function allocate_pool
 * @return pool*
 */
pool_t * allocate_pool();

/* @function destroy_pool
 * @return void
 * @argument pool*
 * @argument bool
 */
void destroy_pool(pool_t * const pool, const bool close_file);

/* @function assign_pool_metadata
 * @return void
 * @argument pool*
 * @argument uint16
 * @argument char*
 */
void assign_pool_metadata(
    pool_t * const pool, const pool_metadata_size_t, const char* metadata
);

/* @function delete_pool_metadata
 * @return void
 * @argument pool*
 */
void delete_pool_metadata(pool_t * const pool);

/* @function fill_pool
 * @return void
 * @argument char*
 * @argument population*
 * @argument uint64
 * @argument generator_mode
 */
void fill_pool(
    const char *address, population_t * const,
    const generator_mode_t
);

/* @function create_pool_in_file
 * @return population*
 * @argument uint64
 * @argument uint8
 * @argument uint8
 * @argument uint64
 * @argument uint64
 * @argument uint64
 * @argument generator_mode
 */
population_t * create_pool_in_file(
    const pool_organisms_num_t,
    const pool_gene_node_id_part_t, const pool_gene_weight_part_t,
    const pool_neurons_num_t input_neurons_number,
    const pool_neurons_num_t output_neurons_number,
    const uint64_t genome_bit_size,
    const generator_mode_t
);

char * alloc_name_for_pool(pool_t *);

/* @function destroy_population
 * @return void
 * @argument population*
 * @argument bool
 * @argument bool
 * @argument bool
 */
void destroy_population(
    population_t * const population,
    const bool destroy_genomes, const bool deallocate_genomes_data,
    const bool close_pool_file
);

/* @function destroy_population_object
 * @return void
 * @argument population*
 */
void destroy_population_object(population_t * const population);
