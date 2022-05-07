#pragma once

#include <string.h>
#include <math.h>
#include <stdbool.h>

#include "demiurge.h"
#include "pool.h"
#include "rand.h"
#include "state_machine.h"

#define MUTATIONS_XORSHIFT_FOR_RANDOM64  0
#define MUTATIONS_MERSENNE_FOR_RANDOM64  1
#define MUTATIONS_RANDOMNESS_MODE MUTATIONS_MERSENNE_FOR_RANDOM64

typedef double mutation_probability_t;

/* @function flip_bits_with_probability
 * @return void
 * @argument uint8*
 * @argument uint64
 * @argument double
 */
void flip_bits_with_probability(
    gene_byte_t * const, uint64_t bits_number, mutation_probability_t);

/* @function flip_bits_in_genome_with_probability
 * @return void
 * @argument genome*
 * @argument pool*
 * @argument double
 */
void flip_bits_in_genome_with_probability(
    const genome_t *genome, const pool_t *pool, mutation_probability_t
);

/* @enum gene_mutation_mode
 * @type uint8
 * @member RANDOMIZE_GENES       (1 << 0)
 * @member ZERO_GENES            (1 << 1)
 * @member REPEAT_NEIGHBOR_GENES (1 << 2)
 */
typedef enum gene_mutation_mode_e {
    RANDOMIZE_GENES       =  (uint8_t)(1 << 0),
    ZERO_GENES            =  (uint8_t)(1 << 1),
    REPEAT_NEIGHBOR_GENES =  (uint8_t)(1 << 2),
    COMBINE_GENES_MUTATION = (uint8_t)(1 << 7)
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
    gene_byte_t * const,
    pool_gene_byte_size_t, genome_length_t,
    gene_mutation_mode_t, mutation_probability_t probability
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
    gene_mutation_mode_t mode, mutation_probability_t probability
);

typedef uint8_t replication_type_t;
typedef double blend_coefficient_t;

/* @function pairing_season
 * @return void
 * @argument uint64_t
 * @argument uint64_t
 * @argument double
 * @argument double
 * @argument double
 * @argument gene_mutation_mode
 * @argument double
 * @argument genome**
 * @argument genome**
 * @argument uint8_t
 */
void pairing_season(
    const pool_organisms_num_t parents_number,
    const pool_organisms_num_t children_number,
    const replication_type_t, const blend_coefficient_t,
    const mutation_probability_t change_genes_prob, const gene_mutation_mode_t,
    const mutation_probability_t flip_bits_prob,
    const genome_t * const * const genomes_parents,
    genome_t * const * const genomes_children,
    const pool_gene_byte_size_t
);
