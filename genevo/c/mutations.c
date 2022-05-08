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
    LOG_ARBITRARY_BASE(1 - (1 / (float)_COLLECTION_SIZE), -_PROBABILITY + 1)

/*

Flip every bit in given bytes sequention with probability `probability`.

*/
void flip_bits_with_probability(
    gene_byte_t * const bytes, uint64_t bits_number,
    mutation_probability_t probability
) {

    if (probability < 0 || probability >= 1)
        ERR_AND_RETURN(ERR_WRONG_PARAMS, RETURN_VOID);

    for (
        uint64_t trial = 0;
        trial < TRIALS_TO_MAKE_PROBABILITY(bits_number, probability);
        trial++
    ) {
        #if   MUTATIONS_RANDOMNESS_MODE == MUTATIONS_XORSHIFT_FOR_RANDOM64
        uint64_t position = next_urandom64_in_range(0, bits_number);
        #elif MUTATIONS_RANDOMNESS_MODE == MUTATIONS_MERSENNE_FOR_RANDOM64
        uint64_t position = next_mersenne_random64_in_range(0, bits_number);
        #endif
        uint64_t byte_i     = position / 8;
        uint8_t  bit_i      = position % 8;
        ((uint8_t * const)bytes)[byte_i] ^= 1 << (8 - bit_i);
    }
}

void flip_bits_in_genome_with_probability(
    const genome_t *genome, const pool_t *pool,
    mutation_probability_t probability
) {
    flip_bits_with_probability(
        genome->genes,
        genome->length * pool->gene_bytes_size,
        probability);
}

void change_genes_with_probability(
    gene_byte_t * const genes,
    pool_gene_byte_size_t gene_bytes_size, genome_length_t genes_number,
    gene_mutation_mode_t mode, mutation_probability_t probability
) {

    #ifndef SKIP_LCG_RND_SEED_CHECK
        ENSURE_LCG_RND_SEED_IS_SET;
    #endif

    for (
        genome_length_t trial = 0;
        trial < TRIALS_TO_MAKE_PROBABILITY(genes_number, probability);
        trial++
    ) {

        #if   MUTATIONS_RANDOMNESS_MODE == MUTATIONS_XORSHIFT_FOR_RANDOM64
        uint64_t position = next_urandom64_in_range(0, genes_number);
        #elif MUTATIONS_RANDOMNESS_MODE == MUTATIONS_MERSENNE_FOR_RANDOM64
        uint64_t position = next_mersenne_random64_in_range(0, genes_number);
        #endif

        // This variable used if mode == REPEAT_NEIGHBOR_GENES
        // If neighbor_gene is `-1`, then previous gene will be repeated.
        // In case it is equal to `1`, the next one is the candidate.
        int8_t neighbor_gene = 0;

        if (mode == COMBINE_GENES_MUTATION)
            // maps [0; 2^32-1] -> {0; 1} -> {0; 2}
            mode = 1 << ((next_fast_random() % 1) * 2);

        switch (mode) {

            case RANDOMIZE_GENES:
                fill_bytes_with_randomness(
                    genes + position * gene_bytes_size,
                    gene_bytes_size);
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
                    genes + position * gene_bytes_size,
                    genes + (position * gene_bytes_size) + (gene_bytes_size * neighbor_gene),
                    gene_bytes_size);
            break;

            default:
            case ZERO_GENES:
                memset(genes + position * gene_bytes_size, 0, gene_bytes_size);
            break;

        }

    }

}

void change_genes_in_genome_with_probability(
    const genome_t *genome, const pool_t *pool,
    gene_mutation_mode_t mode, mutation_probability_t probability
) {
    change_genes_with_probability(
        genome->genes,
        pool->gene_bytes_size, genome->length,
        mode, probability);
}

void crossover_genomes(
    const genome_t *child, const genome_t * const * const parents,
    const pool_gene_byte_size_t gene_bytes_size,
    state_machine_t * const blender
) {

    gene_byte_t *writer_position = child->genes;
    for (genome_length_t gene_i = 0; gene_i < child->length; gene_i++) {

        const genome_t *parent = parents[blender->current_state];

        // TODO: prevent running out of genes to copy
        if (parent->length < gene_i) break;

        memcpy(
            writer_position,
            parent->genes + gene_bytes_size * gene_i,
            gene_bytes_size);

        writer_position += gene_bytes_size * gene_i;
        machine_next_state(blender);

    }

}

/*

Simple brute-force O(n^2 - n) algorithm. It could be effective to use
sorting on big arrays, but since combination_length usually equals
to at most 10, sorting becomes a little overkill.

*/
bool combination_has_duplicates(
    pool_organisms_num_t *combination, uint8_t combination_length
) {

    for (uint8_t genome_i = 0; genome_i < combination_length; genome_i++)
        for (uint8_t genome_j = 0; genome_j < combination_length; genome_j++)
            if (combination[genome_i] == combination[genome_j])
                return true;

    return false;
}

void crossover_genomes_combinations(
    pool_organisms_num_t parents_number, pool_organisms_num_t children_number,
    replication_type_t combination_length, double blend_coefficient,
    const genome_t * const * const genomes_parents,
    genome_t * const * const genomes_children,
    const pool_gene_byte_size_t gene_bytes_size
) {

    if (blend_coefficient <= 0 || blend_coefficient >= 1) {
        ERROR_LEVEL = ERR_WRONG_PARAMS;
        return;
    }

    state_machine_t *blender = generate_state_machine(combination_length);

    state_machine_diag_distribution(
        blender,
        1 - blend_coefficient,
        blend_coefficient / (combination_length - 1));

    init_state_machine(blender, next_fast_random_in_range(0, combination_length));
    if (ERROR_LEVEL != ERR_OK) {
        destroy_state_machine(blender);
        return;
    }

    DECLARE_MALLOC_ARRAY(
        pool_organisms_num_t, combination, combination_length,
        DESTROY_AND_EXIT(destroy_state_machine, blender, RETURN_VOID_ON_ERR));

    DECLARE_MALLOC_LINKS_ARRAY(
        const genome_t, genomes_combination, combination_length,
        DESTROY_AND_EXIT(destroy_state_machine, blender, RETURN_VOID_ON_ERR));

    for (
        pool_organisms_num_t combination_counter = 0;
        combination_counter < children_number;
        combination_counter++
    ) {

        do for (replication_type_t i = 0; i < combination_length; i++) 
            #if   MUTATIONS_RANDOMNESS_MODE == MUTATIONS_XORSHIFT_FOR_RANDOM64
            combination[i] = next_urandom64_in_range(0, parents_number);
            #elif MUTATIONS_RANDOMNESS_MODE == MUTATIONS_MERSENNE_FOR_RANDOM64
            combination[i] = next_mersenne_random64_in_range(0, parents_number);
            #endif
        while (combination_has_duplicates(combination, combination_length));

        for (replication_type_t i = 0; i < combination_length; i++)
            genomes_combination[i] = genomes_parents[combination[i]];

        crossover_genomes(
            genomes_children[combination_counter], genomes_combination,
            gene_bytes_size, blender);

    }

    FREE_NOT_NULL(combination);

    destroy_state_machine(blender);

}

void bottleneck_population(
    pool_organisms_num_t src_number, pool_organisms_num_t dst_number,   
    const genome_t * const * const src,
    const genome_t ** const dst
) {

    state_machine_t *picker = generate_state_machine(src_number);
    state_machine_uniform_distribution(picker);
    #if   MUTATIONS_RANDOMNESS_MODE == MUTATIONS_XORSHIFT_FOR_RANDOM64
    init_state_machine(picker, next_urandom64_in_range(0, src_number));
    #elif MUTATIONS_RANDOMNESS_MODE == MUTATIONS_MERSENNE_FOR_RANDOM64    
    init_state_machine(picker, next_mersenne_random64_in_range(0, src_number));
    #endif

    for (pool_organisms_num_t dst_i = 0; dst_i < dst_number; dst_i++) {
        pool_organisms_num_t src_i = picker->current_state;
        dst[dst_i] = src[src_i];
        machine_next_state(picker);
    }
}

void pairing_season(
    const pool_organisms_num_t parents_number,
    const pool_organisms_num_t children_number,
    const replication_type_t replication_type, const blend_coefficient_t blend_coefficient,
    const mutation_probability_t change_genes_prob, const gene_mutation_mode_t mutation_mode,
    const mutation_probability_t flip_bits_prob,
    const genome_t * const * const genomes_parents,
    genome_t * const * const genomes_children,
    const pool_gene_byte_size_t gene_bytes_size
) {

    const genome_t ** bottleneck_source;

    if (parents_number != children_number)
        bottleneck_population(
            parents_number, children_number, genomes_parents, bottleneck_source);

    crossover_genomes_combinations(
        parents_number, children_number,
        replication_type, blend_coefficient,
        (parents_number == children_number)
            ? genomes_parents : bottleneck_source,
        genomes_children,
        gene_bytes_size);

    if (!change_genes_prob && !flip_bits_prob) return;

    for (
        pool_organisms_num_t genome_i = 0;
        genome_i < children_number;
        genome_i++
    ) {

        if (change_genes_prob)
            change_genes_with_probability(
                genomes_children[genome_i]->genes,
                gene_bytes_size, genomes_children[genome_i]->length,
                mutation_mode, change_genes_prob);

        if (flip_bits_prob)
            flip_bits_with_probability(
                genomes_children[genome_i]->genes,
                gene_bytes_size * genomes_children[genome_i]->length,
                flip_bits_prob);

    }

}
