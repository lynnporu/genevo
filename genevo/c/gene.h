/*

The declarations below are not currently used in the production. They are
desired for creating custom gene structure in the future.

The gene itself is just a bunch of numbers with different bit width. We can use
it to store any amount of information in the gene.

 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef uint8_t gene_number_width_t;

typedef struct gene_number_s {
    const gene_number_width_t   bit_width;
    const bool                  is_signed;
    const bool                  is_denormalized;
} gene_number_t;

typedef uint8_t gene_capacity_t;

typedef struct gene_structure_s {
    const gene_capacity_t  capacity; // number of numbers
    const gene_number_t    numbers[];
} gene_structure_t;

#define UNSIGNED_NORMALIZED(_BIT_WIDTH) \
    { .bit_width = _BIT_WIDTH, .is_signed = false, .is_denormalized = false }

#define SIGNED_DENORMALIZED(_BIT_WIDTH) \
    { .bit_width = _BIT_WIDTH, .is_signed = true, .is_denormalized = true }

#define DECLARE_GENE_SIMP_NETWORK(_HUMAN_NAME, _NODE_ID_WIDTH, _WEIGHT_WIDTH)  \
    const gene_structure_t gene_Sn_ ## _NODE_ID_WIDTH ## _ ## _WEIGHT_WIDTH = {\
        .capacity = 3,                                                         \
        .numbers = {                                                           \
            UNSIGNED_NORMALIZED(_NODE_ID_WIDTH),                               \
            UNSIGNED_NORMALIZED(_NODE_ID_WIDTH),                               \
            SIGNED_DENORMALIZED(_WEIGHT_WIDTH)   }};                           \
    const gene_structure_t                                                     \
    gene_Sn_ ## _HUMAN_NAME = gene_Sn_ ## _NODE_ID_WIDTH ## _ ## _WEIGHT_WIDTH;

// Caenorhabditis elegans, 302 neurons
// Size = 4 bytes, capacity = 512 nodes, width precision = 6.106e-5
DECLARE_GENE_SIMP_NETWORK(roundworm, 9, 14);

// Hirudinea (Leech), ~10e3 neurons
// Size = 6 bytes, capacity = 16_384 nodes, width precision = 9.536e-7
DECLARE_GENE_SIMP_NETWORK(leech, 14, 20)

// Nephropidae (Lobster), ~100e3 neurons
// Size = 7 bytes, capacity = 1_310_72 nodes, width precision = 2.384e-7
DECLARE_GENE_SIMP_NETWORK(lobster, 17, 22);

// Poecilia reticulata (Guppy fish), ~4.3e6 neurons
// Size = 8 bytes, capacity = 4_194_304 nodes, width precision = 9.536e-7
DECLARE_GENE_SIMP_NETWORK(guppy, 22, 20)

// Anura (Frog), ~16e6 neurons
// Size = 9 bytes, capacity = 16_777_216 nodes, width precision = 5.960e-8
DECLARE_GENE_SIMP_NETWORK(frog, 24, 24);

// Felis catus (Cat), ~760e6
// Size = 11 bytes, capacity = 1_073_741_824 nodes, width precision = 3.725e-9
DECLARE_GENE_SIMP_NETWORK(cat, 30, 28)

