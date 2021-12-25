#pragma once

#include <time.h>
#include <stdint.h>
#include <stdlib.h>

#include "stdbool.h"

#define ENSURE_RND_SEED_IS_SET { if (!seed_initialized) set_seed(time(NULL)); }
extern bool seed_initialized;

/* @function set_seed
*  @return void
*  @argument uint32
*/
void set_seed(uint32_t);

uint64_t xorshift128p();

#define RANDOM_WORD_BYTE_SIZE 8
#define next_random64 xorshift128p
