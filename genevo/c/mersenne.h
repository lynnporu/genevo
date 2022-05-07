#pragma once

#include <stdbool.h>
#include <time.h>
#include <stdlib.h>

void mersenne_init_genrand64(unsigned long long seed);

/* generates a random number on [0, 2^64-1]-interval */
unsigned long long mersenne_genrand64_int64(void);

/* generates a random number on [0, 2^63-1]-interval */
long long mersenne_genrand64_int63(void);

/* generates a random number on [0,1]-real-interval */
double mersenne_genrand64_real1(void);

/* generates a random number on [0,1)-real-interval */
double mersenne_genrand64_real2(void);

/* generates a random number on (0,1)-real-interval */
double mersenne_genrand64_real3(void);
