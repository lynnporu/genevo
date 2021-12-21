/*

This header contains declarations for gene, genome and gene pool. It does not
contain any methods.

*/

#pragma once

#include <stdint.h>

#include "files.h"

#define GENE_OUTCOME_IS_INPUT        0b10000000
#define GENE_OUTCOME_IS_INTERMEDIATE 0b01000000
#define GENE_OUTCOME_IS_OUTPUT       0b00100000
#define GENE_INCOME_IS_INPUT         0b00010000
#define GENE_INCOME_IS_INTERMEDIATE  0b00001000
#define GENE_INCOME_IS_OUTPUT        0b00000100

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

typedef uint8_t gene_byte_t;
#define GENE_BYTE_SIZE sizeof(gene_byte_t)

typedef struct gene_s {
    uint64_t outcome_node_id;
    uint64_t income_node_id;
    uint8_t  connection_type;  /* bitmask with following bits meaning
                                  [0] - 1= outcome node is input neurone
                                  [1] - 1= outcome node is intermediate neurone
                                  [2] - 1= outcome node is output neorune
                                  [3] - 1= income node is input neurone
                                  [4] - 1= income node is intermediate neurone
                                  [5] - 1= income node is output neorune
                               */
    int64_t  weight_unnormalized;
    double   weight;           // weight is normalized to [-1; 1]
} gene_t;

typedef struct genome_s {
    uint32_t      length;
    uint8_t      *metadata;
    uint16_t      metadata_byte_size;
    gene_byte_t  *genes;
    uint16_t      residue_size_bits;
    uint8_t      *residue;
} genome_t;

typedef struct pool_s {
    uint64_t    organisms_number;
    uint64_t    input_neurons_number;
    uint64_t    output_neurons_number;
    uint16_t    metadata_byte_size;
    uint8_t    *metadata;
    uint8_t     node_id_part_bit_size;
    uint8_t     weight_part_bit_size;
    uint8_t     gene_bytes_size;
    file_map_t *file_mapping;
    void       *first_genome_start_position;
                // Position of the byte after POOL_META_TERMINAL_BYTE
    void       *cursor;
} pool_t;
