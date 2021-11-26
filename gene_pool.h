#pragma once

#include <stdint.h>

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

typedef uint8_t* gene_t;

/*

Structure of the genome in the memory is the following:

Content                               Size         Note
----------                            ----------   ----------
GENOME_INITIAL_BYTE                   8            Start byte of the genome
MNSB                                  16           Size of the metadata which
                                                   describes this genome.
GENOME_META_INITIAL_BYTE              8            Start byte of the metadata.
MN                                    MNSb         The metadata itself
0GENOME_META_TERMINAL_BYTE            8            End byte of the metadata.
[gene #1]                             2*IGSb + WGSb
[gene #2]                             2*IGSb + WGSb
...
GENOME_RESIDUE_BYTE                   8            Genome residue byte
[size of the residue in bits=RSb]     16           Size of the residual genome*.
...                                   RSb          Residual genome.
GENOME_TERMINAL_BYTE                  8            End byte of the genome

* Could be 10 bits, but that would be unconvenient to put into struct.

If the size of the genome cannot be aligned to size of the single gene (i.e.
genome contains some terminal bits that are not anough to form the new gene),
the residue can be placed after GENOME_RESIDUE_BYTE.

*/

typedef struct genome_preamble_s {
    uint8_t  initial_byte;
    uint16_t metadata_byte_size;
    uint8_t  metadata_initial_byte;
} genome_file_preamble_t;

typedef struct genome_s {
    uint8_t* metadata;
    gene_t*  genes;
    uint16_t residue_size_bits;
    uint8_t* residue;
} genome_t;

/*

Structure of the gene pool written in the file is the following:

Content                               Size (bits)  Note
----------                            ----------   ----------
POOL_INITIAL_BYTE                     8            Initial byte used to verify
                                                   integrity of the file
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
    uint8_t  node_id_part_bit_size;
    uint8_t  weight_part_bit_size;
    uint16_t metadata_byte_size;
    uint8_t  metadata_initial_byte;
} pool_file_preamble_t;

typedef struct pool_s {
    uint8_t*  metadata;
    genome_t* genomes;
} pool_t;