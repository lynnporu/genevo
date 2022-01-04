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
