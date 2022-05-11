#pragma once

#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "bit_manipulations.h"
#include "mersenne.h"

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
uint32_t xorshift128p32() __attribute__((pure));

#define next_urandom64 xorshift128p
#define next_urandom32 xorshift128p32

#define MAP_RANGE_TO_RANGE(_X, _A1, _B1, _A2, _B2) \
    (((_X) - (_A1)) * ((_B2) - (_A2)) / ((_B1) - (_A1)) + (_A2))

#define next_urandom64_in_range(_A, _B) ({                                     \
    double _R  = next_urandom32();                                             \
    (uint64_t)roundl(MAP_RANGE_TO_RANGE(_R, 0, (double)MAX_FOR_32, _A, _B));   \
})

#define next_double_urandom64_in_range(_A, _B) ({                              \
    double _R  = next_urandom32();                                             \
    MAP_RANGE_TO_RANGE(_R, 0, (double)MAX_FOR_32, _A, _B);                     \
})

// linear congruent generator

#define ENSURE_LCG_RND_SEED_IS_SET \
    { if (!lcg_seed_initialized) set_lcg_seed(time(NULL)); }
extern bool lcg_seed_initialized;

void set_lcg_seed(uint32_t);

uint32_t lcg_rand() __attribute__((pure));

#define next_fast_random lcg_rand

#define next_fast_random_in_range(_A, _B) ({                                   \
    double _R  = next_fast_random();                                           \
    (uint32_t)roundl(MAP_RANGE_TO_RANGE(_R, 0, (double)MAX_FOR_32, _A, _B));   \
})

#define next_double_fast_random_in_range(_A, _B) ({                            \
    double _R  = next_fast_random();                                           \
    MAP_RANGE_TO_RANGE(_R, 0, (double)MAX_FOR_32, _A, _B);                     \
})

// Mersenne twister

#define ENSURE_MERSENNE_RND_SEED_IS_SET \
    { if (!mersenne_seed_initialized) mersenne_init_genrand64(time(NULL)); }
extern bool mersenne_seed_initialized;

#define next_mersenne_random64_in_range(_A, _B) ({                             \
    double _R = mersenne_genrand64_int64() % MAX_FOR_32;                       \
    (uint64_t)roundl(MAP_RANGE_TO_RANGE(_R, 0, (double)MAX_FOR_32, _A, _B));   \
})

#define next_double_mersenne_random64_in_range(_A, _B) ({                      \
    double _R = mersenne_genrand64_int64() % MAX_FOR_32;                       \
    MAP_RANGE_TO_RANGE(_R, 0, (double)MAX_FOR_32, _A, _B);                     \
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
