#pragma once

#include <stdint.h>

#include "pool.h"
#include "rand.h"
#include "bits.h"

#define place_random_bytes(_DESTINATION, _BYTES)                               \
    fill_with_randomness(_DESTINATION, _BYTES, 0)

#define place_random_bits(_DESTINATION, _BITS)                                 \
    fill_with_randomness(_DESTINATION, (uint32_t)(_BITS / 8), _BITS % 8)

void fill_with_randomness(uint8_t *destination, uint32_t bytes, uint8_t bits);
