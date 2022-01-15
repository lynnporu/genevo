/*

The declarations below are not currently used in the production. They are
desired for creating custom gene structure in the future.

The gene itself is just a bunch of numbers with different bit width. We can use
it to store any amount of information in the gene.

 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "bit_manipulations.h"

#define MAXIMUM_GENE_CAPACITY 3

typedef uint8_t gene_number_width_t;

typedef struct gene_number_s {
    const gene_number_width_t   bit_width;
    const bool                  is_signed;
    const bool                  is_denormalized;
} gene_number_t;

typedef uint8_t  gene_capacity_t;
typedef uint8_t  gene_structure_class_t;
typedef uint32_t gene_structure_id_t;

#define GENE_STRUCTURE_ID_INSTANCE_WIDTH \
    (( sizeof(gene_structure_id_t) - sizeof(gene_structure_class_t) ) * 8)

typedef struct gene_structure_s {
    const gene_structure_id_t id;
    const gene_capacity_t     capacity; // number of numbers
    const gene_number_t       numbers[MAXIMUM_GENE_CAPACITY];
} gene_structure_t;

#define GENE_STRUCTURE_ID(_CLASS_ID, _INSTANCE_ID)                             \
    (   (                                                                      \
            (gene_structure_class_t)(_CLASS_ID) <<                             \
            (GENE_STRUCTURE_ID_INSTANCE_WIDTH)                                 \
        ) | (_INSTANCE_ID)                                                     \
    )

#define UNSIGNED_NORMALIZED(_BIT_WIDTH) \
    { .bit_width = _BIT_WIDTH, .is_signed = false, .is_denormalized = false }

#define SIGNED_DENORMALIZED(_BIT_WIDTH) \
    { .bit_width = _BIT_WIDTH, .is_signed = true, .is_denormalized = true }
