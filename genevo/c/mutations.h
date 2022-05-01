#pragma once

#include <string.h>
#include <math.h>
#include <stdbool.h>

#include "pool.h"
#include "rand.h"

/* @function flip_bits_with_probability
 * @return void
 * @argument uint8*
 * @argument uint64
 * @argument double
 */
void flip_bits_with_probability(
    gene_byte_t * const, uint64_t bits_number, double probability);

/* @function flip_bits_in_genome_with_probability
 * @return void
 * @argument genome*
 * @argument pool*
 * @argument double
 */
void flip_bits_in_genome_with_probability(
    const genome_t *genome, const pool_t *pool, double probability
);

/* @enum gene_mutation_mode
 * @type uint8
 * @member RANDOMIZE_GENES       (1 << 0)
 * @member ZERO_GENES            (1 << 1)
 * @member REPEAT_NEIGHBOR_GENES (1 << 2)
 */
typedef enum gene_mutation_mode_e {
    RANDOMIZE_GENES       = (uint8_t)(1 << 0),
    ZERO_GENES            = (uint8_t)(1 << 1),
    REPEAT_NEIGHBOR_GENES = (uint8_t)(1 << 2)
} gene_mutation_mode_t;

/* @function change_genes_with_probability
 * @return void
 * @argument gene_byte_p
 * @argument uint8
 * @argument uint32
 * @argument gene_mutation_mode
 * @argument double
 */
void change_genes_with_probability(
    gene_byte_t * const, pool_gene_byte_size_t, genome_length_t
    gene_mutation_mode_t, double probability
);

/* @function change_genes_in_genome_with_probability
 * @return void
 * @argument genome*
 * @argument pool*
 * @argument gene_mutation_mode
 * @argument double
 */
void change_genes_in_genome_with_probability(
    const genome_t *genome, const pool_t *pool,
    gene_mutation_mode_t mode, double probability
);
