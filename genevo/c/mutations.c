#include "mutations.h"

#ifndef LOG_ARBITRARY_BASE
#   define LOG_ARBITRARY_BASE(_BASE, _NUMBER) (log(_NUMBER) / log(_BASE))
#endif

/*

When we choose an object from the collection of the size N, there's always
probability `1/N` for each object that it will be chosen. But when we're making
repetitive trials and the object selected are not being deleted from the
collection, there's `1-(1-1/N)^n` probability it'll be selected. Here `n` means
the number of trials. Let's say, we want to make such a number if trial, that
every object will have `p` probability to be selected. We can derive the number
of trials from the equation:
    1 - (1 - 1/N)^n = p   =>   n = log_{1 - (1/N)} (-p + 1)

*/
#define TRIALS_TO_MAKE_PROBABILITY(_COLLECTION_SIZE, _PROBABILITY) \
    LOG_ARBITRARY_BASE(1 - (1 / _COLLECTION_SIZE), -_PROBABILITY + 1)

/*

Flip every bit in given bytes sequention with probability `probability`.

*/
void flip_bits_with_probability(
    gene_byte_t * const bytes, uint64_t bytes_number, double probability
) {
    for (
        uint64_t trial = 0;
        trial < TRIALS_TO_MAKE_PROBABILITY(bytes_number * 8, probability);
        trial++
    ) {
        uint64_t position = next_urandom64_in_range(0, bytes_number * 8),
                 byte     = position / 8,
                 bit      = position % 8;
        ((uint8_t * const)bytes)[byte] ^= 1 << bit;
    }
}

void flip_bits_in_genome_with_probability(
    const genome_t *genome, const pool_t *pool, double probability
) {
    flip_bits_with_probability(
        genome->genes,
        genome->length * pool->gene_bytes_size,
        probability);
}
