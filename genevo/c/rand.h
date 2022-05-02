#pragma once

#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "bit_manipulations.h"

// xorshift128p generator

#define ENSURE_XORSHIFT128P_RND_SEED_IS_SET \
    { if (!xorshift128p_seed_initialized) set_xorshift128p_seed(time(NULL)); }
extern bool xorshift128p_seed_initialized;

/* @function set_xorshift128p_seed
 * @return void
 * @argument uint32
  */
void set_xorshift128p_seed(uint32_t);

uint64_t xorshift128p() __attribute__((pure));

#define next_urandom64 xorshift128p

#define next_urandom64_in_range(_A, _B) ({                                     \
    double _A_ = (_A);                                                         \
    double _B_ = (_B);                                                         \
    (uint64_t)roundl(                                                          \
        _A_ + ((_B_ - _A_) / MAX_FOR_64) * (double)next_urandom64());          \
})

// linear congruent generator

#define ENSURE_LCG_RND_SEED_IS_SET \
    { if (!lcg_seed_initialized) set_lcg_seed(time(NULL)); }
extern bool lcg_seed_initialized;

void set_lcg_seed(uint32_t);

uint32_t lcg_rand() __attribute__((pure));

#define next_fast_random lcg_rand

#define next_fast_random_in_range(_A, _B) ({                                   \
    double _A_ = (_A);                                                         \
    double _B_ = (_B);                                                         \
    (uint32_t)roundl(                                                          \
        _A_ + ((_B_ - _A_) / MAX_FOR_32) * (double)next_fast_random());        \
})

// ...other functions

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

