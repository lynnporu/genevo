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
