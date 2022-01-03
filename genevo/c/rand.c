#include "rand.h"

struct xorshift128p_state {
    union {
        uint64_t x[2];
        uint32_t numbers[4];
    } seed;
};

bool seed_initialized = false;

struct xorshift128p_state rand_state = {
    .seed.numbers = {0xcaffee, 0xcaffee, 0xcaffee, 0xcaffee} };

void set_seed(uint32_t new_seed) {

    seed_initialized = true;
    srand(new_seed);

    rand_state.seed.numbers[0] = rand();
    rand_state.seed.numbers[1] = rand();
    rand_state.seed.numbers[2] = rand();
    rand_state.seed.numbers[3] = rand();

}

uint64_t xorshift128p() {
    uint64_t t = rand_state.seed.x[0];
    uint64_t const s = rand_state.seed.x[1];
    rand_state.seed.x[0] = s;
    t ^= t << 23;       // a
    t ^= t >> 18;       // b
    t ^= s ^ (s >> 5);  // c
    rand_state.seed.x[1] = t;
    return t + s;
}
