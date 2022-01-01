/*

This header contains declarations for gene, genome and gene pool. It does not
contain any methods.

 */

#pragma once

#include <stdint.h>

#include "files.h"

/* @flags gene_connection_flag
 * @type uint8
 * @flag GENE_OUTCOME_IS_INPUT           (1 << 7)
 * @flag GENE_OUTCOME_IS_INTERMEDIATE    (1 << 6)
 * @flag GENE_OUTCOME_IS_OUTPUT          (1 << 5)
 * @flag GENE_INCOME_IS_INPUT            (1 << 4)
 * @flag GENE_INCOME_IS_INTERMEDIATE     (1 << 3)
 * @flag GENE_INCOME_IS_OUTPUT           (1 << 2)
 */
#define GENE_OUTCOME_IS_INPUT        0b10000000
#define GENE_OUTCOME_IS_INTERMEDIATE 0b01000000
#define GENE_OUTCOME_IS_OUTPUT       0b00100000
#define GENE_INCOME_IS_INPUT         0b00010000
#define GENE_INCOME_IS_INTERMEDIATE  0b00001000
#define GENE_INCOME_IS_OUTPUT        0b00000100
typedef uint8_t gene_connection_flag_t;

/*

Genome of the organism basically defines a weighted oriented graph, which forms
the brain.

Let's use the following notation:
    P        : gene pool
    N        : genome
    G        : gene
    OG       : part of the genome where the number of outcome node is written
    IG       : part for the income node
    WG       : part for the edge's weight
    M<code>  : metadata of the <code> (i.e. MP - metadata of the gene pool)
    <code>Sb : size of the part encoded with <code> in bits
               (i.e. OGS - size of the OG)
    <code>SB : size of the <code> in bytes (suppose byte = 8 bits)

Gene is a number with the following bit structure:
    G=[OG][IG][WG]

If we want our model to contain at least 15 input, 2 output and 40 intermediate
neurons, it'll be enough to give OG and IG just 6 bits. Maximum
number we can encode with 6 bit is 63. That is:
    [ 0; 14] encodes input neurons,
    [15; 61] encodes intermediate neurons,
    [62; 63] encodes output neurons.
Selected model will be capable of containing just 239 intermediate neurons.
This number can be increased with maximizing the number of bits.

! Be sure that size of the `weight` (WGSb) is at least 16 bits.

! Be sure that GSb % 8 == 0.

 */

typedef uint8_t byte_t;

/* @typedef gene_byte
 * @from_type uint8
 */
/* @typedef gene_byte_p
 * @from_type gene_byte*
 */
typedef uint8_t gene_byte_t;

typedef uint64_t gene_node_id_t;
typedef int64_t  gene_edge_weight_unnormalized_t;
typedef double   gene_edge_weight;

/* @struct gene
 * @member uint64 outcome_node_id
 * @member uint64 income_node_id
 * @member gene_connection_flag connection_type
 * @member int64 weight_unnormalized
 * @member double weight
 */
/* @typedef gene_p
 * @from_type gene*
 */
typedef struct gene_s {
    gene_node_id_t                  outcome_node_id;
    gene_node_id_t                  income_node_id;
    gene_connection_flag_t          connection_type;
    gene_edge_weight_unnormalized_t weight_unnormalized;
    // weight is normalized to [-1; 1]
    gene_edge_weight                 weight;
} gene_t;

typedef uint32_t genome_length_t;
typedef uint16_t genome_metadata_size_t;
typedef uint16_t genome_residue_size_t;

/* @typedef genome_p
 * @from_type genome*
 */
/* @typedef genome_p_p
 * @from_type genome**
 */
/* @struct genome
 * @member uint32 length
 * @member uint8* metadata
 * @member uint8* genes
 * @member uint16 residue_size_bits
 * @member uint8* residue
 */
typedef struct genome_s {
    genome_length_t          length;
    byte_t                  *metadata;
    genome_metadata_size_t   metadata_byte_size;
    gene_byte_t             *genes;
    genome_residue_size_t    residue_size_bits;
    byte_t                  *residue;
} genome_t;

// 'pl' means 'pool'
typedef uint64_t   pool_organisms_num_t;
typedef uint64_t   pool_neurons_num_t;
typedef uint16_t   pool_metadata_size_t;
typedef uint8_t    pool_gene_node_id_part_t;
typedef uint8_t    pool_gene_weight_part_t;
typedef uint8_t    pool_gene_byte_size_t;

/* @typedef pool_p
 * @from_type pool*
 */
/* @struct pool
 * @member uint64 organisms_number
 * @member uint64 input_neurons_number
 * @member uint64 output_neurons_number
 * @member uint16 metadata_byte_size
 * @member uint8* metadata
 * @member uint8 node_id_part_bit_size
 * @member uint8 weight_part_bit_size
 * @member uint8 gene_bytes_size
 * @member file_map file_mapping
 * @member uint8* first_genome_start_position
 * @member uint8* cursor
 */
typedef struct pool_s {
    pool_organisms_num_t      organisms_number;
    pool_neurons_num_t        input_neurons_number;
    pool_neurons_num_t        output_neurons_number;
    pool_metadata_size_t      metadata_byte_size;
    byte_t                   *metadata;
    pool_gene_node_id_part_t  node_id_part_bit_size;
    pool_gene_weight_part_t   weight_part_bit_size;
    pool_gene_byte_size_t     gene_bytes_size;
    file_map_t               *file_mapping;
    void                     *first_genome_start_position;
    // Position of the byte after POOL_META_TERMINAL_BYTE
    void                     *cursor;
} pool_t;

/* @typedef population_p
 * @from_type population*
 */
/* @struct population
 * @member pool* pool
 * @member genome** genomes
 */
typedef struct population_s {
    pool_t     *pool;
    genome_t  **genomes;
} population_t;
