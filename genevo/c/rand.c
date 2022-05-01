#include "rand.h"

/*

xorshift128p random generator
===============================
This generator is being used for big amounts of data. It provides great
periodicity though being robust.

*/

struct xorshift128p_state {
    union {
        uint64_t x[2];
        uint32_t numbers[4];
    } seed;
};

bool xorshift128p_seed_initialized = false;

struct xorshift128p_state xorshift128p_rand_state = {
    .seed.numbers = {0xcaffee, 0xcaffee, 0xcaffee, 0xcaffee} };

void set_xorshift128p_seed(uint32_t new_seed) {

    xorshift128p_seed_initialized = true;
    srand(new_seed);

    xorshift128p_rand_state.seed.numbers[0] = rand();
    xorshift128p_rand_state.seed.numbers[1] = rand();
    xorshift128p_rand_state.seed.numbers[2] = rand();
    xorshift128p_rand_state.seed.numbers[3] = rand();

}

uint64_t xorshift128p() {
    uint64_t t = xorshift128p_rand_state.seed.x[0];
    uint64_t const s = xorshift128p_rand_state.seed.x[1];
    xorshift128p_rand_state.seed.x[0] = s;
    t ^= t << 23;       // a
    t ^= t >> 18;       // b
    t ^= s ^ (s >> 5);  // c
    xorshift128p_rand_state.seed.x[1] = t;
    return t + s;
}

/*
    
Linear Congruential Generator
===============================
This generator is faster then built-in rand() but lacks of quality. It can be
used for making non-important random decisions.
    
*/

struct lcg_state {
    uint32_t seed;
    uint32_t multiplier, increment;
};

bool lcg_seed_initialized = false;

struct lcg_state lcg_rand_state = {
    .seed = 0xcaffee,
    .multiplier = 214013,
    .increment = 2531011
};

void set_lcg_seed(uint32_t new_seed) {

    lcg_seed_initialized = true;
    srand(new_seed);

    lcg_rand_state.seed = rand();

}

inline uint32_t lcg_rand() {
    lcg_rand_state.seed = 214013 * lcg_rand_state.seed + 2531011;
    return (lcg_rand_state.seed >> 16) & 0x7FFF;
}

/*

Use random generator to fill given bits number with randomness. *destination
should point to allocated memory that is equal to or bigger than
    (bits_size // 8) + 1.
In case bits_size % 8 != 0, left bits will be assigned to zero.

Example:

uint8_t *mem = malloc(2);
place_random_bits(mem, 9);

Now destination has memory dump `???????? ?0000000`, where `?` is some random
bit.

 */
void fill_with_randomness(
    uint8_t * destination, uint32_t bytes, const uint8_t bits) {

    #ifndef SKIP_XORSHIFT128P_RND_SEED_CHECK
        ENSURE_XORSHIFT128P_RND_SEED_IS_SET;
    #endif

    // fill whole bytes first

    #define FILL_NEXT(_SIZE, _TYPE)                                            \
        if (bytes >= _SIZE) {                                                  \
            *(_TYPE *)destination = next_urandom64();                          \
            destination += _SIZE;                                              \
            bytes -= _SIZE;                                                    \
            continue;                                                          \
        }

    while (bytes > 0) {
        // this will work faster than just filling all the bytes
        // with next_urandom64()
        FILL_NEXT(8, uint64_t);
        FILL_NEXT(4, uint32_t);
        FILL_NEXT(2, uint16_t);
        FILL_NEXT(1, uint8_t );
    }

    #undef FILL_NEXT

    // partially fill one left byte
    if (!bits) return;
    *(uint8_t *)destination = (uint8_t)next_urandom64() << (8 - bits);

}
