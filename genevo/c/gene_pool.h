#pragma once

#include <stdint.h>

#include "files.h"


// Even the empty pool file should be at least 64 bits long.
#define POOL_FILE_MIN_SAFE_BIT_SIZE 64

#define POOL_INITIAL_BYTE           (uint8_t) 0xAB
#define POOL_META_INITIAL_BYTE      (uint8_t) 0xBC
#define POOL_META_TERMINAL_BYTE     (uint8_t) 0xCD
#define POOL_TERMINAL_BYTE          (uint8_t) 0xFF

#define GENOME_INITIAL_BYTE         (uint8_t) 0xA0
#define GENOME_META_INITIAL_BYTE    (uint8_t) 0xDE
#define GENOME_META_TERMINAL_BYTE   (uint8_t) 0xEF
#define GENOME_RESIDUE_BYTE         (uint8_t) 0xA2
#define GENOME_TERMINAL_BYTE        (uint8_t) 0xA1

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

#define GENE_OUTCOME_IS_INPUT        0b10000000
#define GENE_OUTCOME_IS_INTERMEDIATE 0b01000000
#define GENE_OUTCOME_IS_OUTPUT       0b00100000
#define GENE_INCOME_IS_INPUT         0b00010000
#define GENE_INCOME_IS_INTERMEDIATE  0b00001000
#define GENE_INCOME_IS_OUTPUT        0b00000100

typedef uint8_t gene_byte_t;
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

/*

Structure of the genome in the memory is the following:

Content                               Size         Note
----------                            ----------   ----------
GENOME_INITIAL_BYTE                   8            Start byte of the genome
[number of genes]                     32
MNSB                                  16           Size of the metadata which
                                                   describes this genome.
GENOME_META_INITIAL_BYTE              8            Start byte of the metadata.
MN                                    MNSb         The metadata itself
GENOME_META_TERMINAL_BYTE             8            End byte of the metadata.
[gene #1]                             2*IGSb + WGSb
[gene #2]                             2*IGSb + WGSb
...
GENOME_RESIDUE_BYTE                   8            Genome residue byte
[size of the residue in bits=RSb]     16           Size of the residual genome*.
...                                   RSb          Residual genome.
GENOME_TERMINAL_BYTE                  8            End byte of the genome

* Maximum size of a gene in bits is 255*2 + 255 = 765. Just 10 bits is enough
  To encode this number, but that would be unconvenient to put into struct, so
  16 chosen instead.

If the size of the genome cannot be aligned to size of the single gene (i.e.
genome contains some terminal bits that are not anough to form the new gene),
the residue can be placed after GENOME_RESIDUE_BYTE.

*/

typedef struct genome_preamble_s {
    uint8_t   initial_byte;
    uint32_t  genes_number;
    uint16_t  metadata_byte_size;
    uint8_t   metadata_initial_byte;
} __attribute__((packed, aligned(1))) genome_file_preamble_t;

typedef struct genome_s {
    uint32_t      length;
    uint8_t      *metadata;
    uint16_t      metadata_byte_size;
    gene_byte_t  *genes;
    uint16_t      residue_size_bits;
    uint8_t      *residue;
} genome_t;

/*

Structure of the gene pool written in the file is the following:

Content                               Size (bits)  Note
----------                            ----------   ----------
POOL_INITIAL_BYTE                     8            Initial byte used to verify
                                                   integrity of the file
[number of organisms]                 64
[number of input neurons]             64
[number of output neurons]            64
[size of the OG and IG in bits]       8
[size of the WG in bits]              8
[size of metadata in bytes = MPSB]    16           Metadata contains desription
                                                   of this gene pool
POOL_META_INITIAL_BYTE                8            Start byte of the metadata
MP                                    MPSb         The metadata itself
POOL_META_TERMINAL_BYTE               8            End byte of the metadata
[genome of the organism #1]           -
[genome of the organism #2]           -
...
POOL_TERMINAL_BYTE                    8            End byte used to verify
                                                   integrity of the file.

! Note, that OG and IG has the same size.

! Note, that size of OG in bits (OGSb) can be encoded with 8-bit number. That
  means, maximum OGSb value can be 255. Maximum value for number, which consists
  of 255 bits is 5.789604e+76, which is pretty large.

*/

typedef struct pool_file_preamble_s {
    uint8_t  initial_byte;
    uint64_t organisms_number;
    uint64_t input_neurons_number;
    uint64_t output_neurons_number;
    uint8_t  node_id_part_bit_size;
    uint8_t  weight_part_bit_size;
    uint16_t metadata_byte_size;
    uint8_t  metadata_initial_byte;
} __attribute__((packed, aligned(1))) pool_file_preamble_t;

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