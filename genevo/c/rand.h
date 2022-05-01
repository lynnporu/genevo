#pragma once

#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "bit_manipulations.h"

#define ENSURE_RND_SEED_IS_SET { if (!seed_initialized) set_seed(time(NULL)); }
extern bool seed_initialized;

/* @function set_seed
 * @return void
 * @argument uint32
  */
void set_seed(uint32_t);

uint64_t xorshift128p() __attribute__((pure));

#define next_urandom64 xorshift128p

#define MAX_FOR_64 0xffffffffffffUL

#define next_urandom64_in_range(_A, _B) ({                                     \
    double _A_ = (_A);                                                         \
    double _B_ = (_B);                                                         \
    (uint64_t)roundl(                                                          \
        _A_ + ((_B_ - _A_) / MAX_FOR_64) * (double)next_urandom64());          \
})

#define fill_bytes_with_randomness(_DESTINATION, _BYTES)                       \
    fill_with_randomness(_DESTINATION, _BYTES, 0)

#define fill_bits_with_randomness(_DESTINATION, _BITS)                         \
    fill_with_randomness(_DESTINATION, (uint32_t)(_BITS / 8), _BITS % 8)

/* @function fill_with_randomness
 * @return void
 * @argument uint8*
 * @argument uint32
 * @argument uint8
 */
void fill_with_randomness(
    uint8_t *destination, uint32_t bytes, const uint8_t bits);

