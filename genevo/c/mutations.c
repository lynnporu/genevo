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

void change_genes_with_probability(
    gene_byte_t * const genes,
    pool_gene_byte_size_t gene_size, genome_length_t genes_number,
    gene_mutation_mode_t mode, double probability
) {

    #ifndef SKIP_LCG_RND_SEED_CHECK
        ENSURE_LCG_RND_SEED_IS_SET;
    #endif

    for (
        genome_length_t trial = 0;
        trial < TRIALS_TO_MAKE_PROBABILITY(genes_number, probability);
        trial++
    ) {

        uint64_t position = next_urandom64_in_range(0, genes_number);

        // This variable used if mode == REPEAT_NEIGHBOR_GENES
        // If neighbor_gene is `-1`, then previous gene will be repeated.
        // In case it is equal to `1`, the next one is the candidate.
        int8_t neighbor_gene = 0;

        if (mode == COMBINE_GENES_MUTATION)
            mode = 1 << next_fast_random_in_range(0, 2);

        switch (mode) {

            case RANDOMIZE_GENES:
                fill_bytes_with_randomness(
                    genes + position * gene_size,
                    gene_size);
                break;

            case REPEAT_NEIGHBOR_GENES:
                if (position == 0)
                    neighbor_gene = 1;
                else
                if (position == genes_number - 1)
                    neighbor_gene = -1;
                else
                    // maps {0; 1} -> {-1; 1}
                    neighbor_gene = ((uint8_t)next_fast_random() % 1) * 2 - 1;

                memcpy(
                    genes + position * gene_size,
                    genes + (position * gene_size) + (gene_size * neighbor_gene),
                    gene_size);
            break;

            default:
            case ZERO_GENES:
                memset(genes + position * gene_size, 0, gene_size);
            break;

        }

    }

}

void change_genes_in_genome_with_probability(
    const genome_t *genome, const pool_t *pool,
    gene_mutation_mode_t mode, double probability
) {
    change_genes_with_probability(
        genome->genes,
        pool->gene_bytes_size, genome->length,
        mode, probability);
}

void crossover_genomes(
    const genome_t *child,
    const genome_t * const * const parents, const uint32_t parents_num,
    const pool_t *pool,
    double blend_coefficient
) {

    if (blend_coefficient <= 0 || blend_coefficient >= 1) {
        ERROR_LEVEL = ERR_WRONG_PARAMS;
        return;
    }

    state_machine_t *blender = generate_state_machine(parents_num);

    for (uint32_t i = 0; i < parents_num; i++) {
        for (uint32_t j = 0; j < parents_num; j++) {

            if (i == j)
                blender->transitions[i][j] = 1 - blend_coefficient;
            else
                blender->transitions[i][j] = blend_coefficient / (parents_num - 1);

        }
    }

    init_state_machine(blender, next_fast_random_in_range(0, parents_num));

    gene_byte_t *writer_position = child->genes;
    for (genome_length_t gene_i = 0; gene_i < child->length; gene_i++) {

        memcpy(
            writer_position,
            parents[blender->current_state]->genes,
            pool->gene_bytes_size);

        writer_position += pool->gene_bytes_size * gene_i;
        machine_next_state(blender);

    }

    destroy_state_machine(blender);

}
