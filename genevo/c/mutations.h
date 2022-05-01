#pragma once

#include <string.h>
#include <math.h>

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
