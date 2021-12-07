/*

This module contains methods for dumping gene pool into file and vice versa.

*/


#include "pickler.h"

#define MAPPING_FAIL_CONDITION(_CONDITION, _ERR_CONST) \
    if(_CONDITION) {ERROR_LEVEL = _ERR_CONST; close_file(mapping); return NULL;}

pool_t * read_pool(const char *address) {

    file_map_t *mapping = open_file(address, OPEN_MODE_READ, 0);
    if (ERROR_LEVEL != ERR_OK) return NULL;

    MAPPING_FAIL_CONDITION(
        mapping->size < (POOL_FILE_MIN_SAFE_BIT_SIZE / 8),
        ERR_POOL_CORRUPT_TOO_SMALL);

    pool_file_preamble_t *preamble = mapping->data;

    MAPPING_FAIL_CONDITION(
        preamble->initial_byte != POOL_INITIAL_BYTE,
        ERR_POOL_CORRUPT_INITIAL);

    MAPPING_FAIL_CONDITION(
        preamble->metadata_initial_byte != POOL_META_INITIAL_BYTE,
        ERR_POOL_CORRUPT_METADATA_START);

    MAPPING_FAIL_CONDITION(
        preamble->node_id_part_bit_size > 64,
        ERR_GENE_OGSB_TOO_LARGE);

    MAPPING_FAIL_CONDITION(
        preamble->weight_part_bit_size > 64,
        ERR_GENE_WEIGHT_TOO_LARGE);

    MAPPING_FAIL_CONDITION(
        (preamble->weight_part_bit_size +
         preamble->node_id_part_bit_size) % 8 != 0,
        ERR_GENE_NOT_ALIGNED);


    pool_t *pool = malloc(sizeof(pool_t));
    pool->file_mapping = mapping;

    pool->organisms_number = ntoh64(preamble->organisms_number);

    pool->metadata_byte_size = preamble->metadata_byte_size;
    pool->metadata_byte_size = ntoh16(preamble->metadata_byte_size);
    pool->metadata = (uint8_t *)(&preamble->metadata_initial_byte + 1);

    pool->input_neurons_number = ntoh64(preamble->input_neurons_number);
    pool->output_neurons_number = ntoh64(preamble->output_neurons_number);

    pool->node_id_part_bit_size = preamble->node_id_part_bit_size;
    pool->weight_part_bit_size = preamble->weight_part_bit_size;
    pool->gene_bytes_size = (
        pool->node_id_part_bit_size * 2 +
        pool->weight_part_bit_size);

    pool->first_genome_start_position =
        &preamble->metadata_initial_byte + 1 + preamble->metadata_byte_size;

    pool->cursor = pool->first_genome_start_position;

    free(preamble);
    return pool;

}

void write_pool(const char *address, pool_t *pool, genome_t **genomes) {

    ERROR_LEVEL = 0;

    size_t file_size =
        1 +               // initial byte
        8 +               // number of organisms
        8 + 8 +           // number of input and output neurons
        1 + 1 +           // size of the OG and WG
        2 +               // size of the metadata
        1 +               // metadata initial byte
        pool->metadata_byte_size +
        1 +               // metadata terminal byte
        1;                // pool terminal byte

    for (uint64_t genome_i = 0; genome_i < pool->organisms_number; genome_i++)
        file_size +=
            1 +           // genome initial byte
            4 +           // number of genes
            2 +           // size of the metadata
            1 +           // metadata initial byte
            genomes[genome_i]->metadata_byte_size +
            1 +           // metadata terminal byte
            genomes[genome_i]->length * pool->gene_bytes_size +
            1 +           // genome residue byte
            (uint16_t)(genomes[genome_i]->residue_size_bits / 8) + 1 +
            1;            // genome terminal byte

    file_map_t *mapping = open_file(address, OPEN_MODE_WRITE, file_size - 1);
    if (ERROR_LEVEL != ERR_OK) return;

    pool_file_preamble_t *pool_preamble = mapping->data;
    pool_preamble->initial_byte = POOL_INITIAL_BYTE;
    pool_preamble->organisms_number = hton64(pool->organisms_number);
    pool_preamble->input_neurons_number = hton64(pool->input_neurons_number);
    pool_preamble->output_neurons_number = hton64(pool->output_neurons_number);
    pool_preamble->node_id_part_bit_size = pool->node_id_part_bit_size;
    pool_preamble->weight_part_bit_size = pool->weight_part_bit_size;
    pool_preamble->metadata_byte_size = pool->metadata_byte_size;
    pool_preamble->metadata_initial_byte = POOL_META_INITIAL_BYTE;

    uint8_t *pool_metadata = &pool_preamble->metadata_initial_byte + 1;

    // copy pool meta bytes
    memcpy(pool_metadata, pool->metadata, pool->metadata_byte_size);

    void *pool_meta_terminal_byte = pool_metadata + pool->metadata_byte_size;
    *(uint8_t *)pool_meta_terminal_byte = POOL_META_TERMINAL_BYTE;

    // genome pointer
    genome_file_preamble_t *genome_preamble = pool_meta_terminal_byte + 1;

    for(
        // genome iterator
        uint64_t genome_itr = 0;
        genome_itr < pool->organisms_number;
        genome_itr++
    ) {

        genome_t *current_genome = genomes[genome_itr];

        genome_preamble->initial_byte = GENOME_INITIAL_BYTE;
        genome_preamble->genes_number = hton32(current_genome->length);
        genome_preamble->metadata_byte_size = hton16(current_genome->metadata_byte_size);
        genome_preamble->metadata_initial_byte = GENOME_META_INITIAL_BYTE;

        uint8_t *genome_metadata = &genome_preamble->metadata_initial_byte + 1;

        // copy genome meta bytes
        memcpy(
            genome_metadata, current_genome->metadata,
            current_genome->metadata_byte_size);

        void *genome_meta_terminal_byte =
            genome_metadata + current_genome->metadata_byte_size;

        *(uint8_t *)genome_meta_terminal_byte = GENOME_META_TERMINAL_BYTE;

        #define GENES_BYTES_SIZE (current_genome->length * pool->gene_bytes_size)

        // copy genes
        memcpy(
            genome_meta_terminal_byte + 1, current_genome->genes,
            GENES_BYTES_SIZE);

        void *residue_byte = genome_meta_terminal_byte + 1 + GENES_BYTES_SIZE;
        *(uint8_t *)residue_byte = GENOME_RESIDUE_BYTE;

        #undef GENES_BYTES_SIZE

        uint8_t residue_size_bytes =
            (uint8_t)(current_genome->residue_size_bits / 8) + 1;

        memcpy(
            residue_byte + 1, current_genome->residue, residue_size_bytes);

        void *terminal_byte = residue_byte + residue_size_bytes + 1;
        *(uint8_t *)terminal_byte = GENOME_TERMINAL_BYTE;

        genome_preamble = terminal_byte + 1;

    }

    genome_preamble->initial_byte = POOL_TERMINAL_BYTE;

    close_file(mapping);

}

void close_pool(pool_t *pool) {
    close_file(pool->file_mapping);
    free(pool);
}

void reset_genome_cursor(pool_t *pool) {
    pool->cursor = pool->first_genome_start_position;
}


genome_t * read_next_genome(pool_t *pool) {

    ERROR_LEVEL = 0;

    if (*(uint8_t *)pool->cursor == POOL_TERMINAL_BYTE) {
        ERROR_LEVEL = ERR_GENM_END_ITERATION;
        return NULL;
    }

    genome_file_preamble_t *preamble = pool->cursor;

    if (preamble->initial_byte != GENOME_INITIAL_BYTE) {
        ERROR_LEVEL = ERR_GENM_CORRUPT_START;
        return NULL;
    }

    if (preamble->metadata_initial_byte != GENOME_META_INITIAL_BYTE) {
        ERROR_LEVEL = ERR_GENM_CORRUPT_METADATA_START;
        return NULL;
    }

    genome_t* genome = malloc(sizeof(genome_t));

    genome->length = ntoh32(preamble->genes_number);
    genome->metadata = &preamble->metadata_initial_byte + 1;
    genome->metadata_byte_size = ntoh16(preamble->metadata_byte_size);
    genome->genes = preamble->metadata + ntoh16(preamble->metadata_byte_size + 1);

    void * residue_byte = (
        genome->genes +
        pool->gene_bytes_size * genome->length +
        1);

    if (*(uint8_t *)residue_byte != GENOME_RESIDUE_BYTE) {
        ERROR_LEVEL = ERR_GENM_CORRUPT_RESIDUE;
        free(genome);
        return NULL;
    }

    genome->residue_size_bits = ntoh16(
        *(uint16_t *)(residue_byte + sizeof(uint8_t)));

    genome->residue =
        residue_byte + sizeof(uint8_t) + sizeof(genome->residue_size_bits);

    void * terminal_byte = residue_byte + 1 + sizeof(genome->residue_size_bits);

    if (*(uint8_t *)residue_byte != GENOME_TERMINAL_BYTE) {
        ERROR_LEVEL = ERR_GENM_CORRUPT_END;
        free(genome);
        return NULL;
    }

    pool->cursor = terminal_byte + 1;

    free(preamble);
    return genome;

}

genome_t ** read_genomes(pool_t *pool) {

    genome_t ** genomes = malloc(sizeof(genome_t) * pool->organisms_number);

    for(uint64_t cursor = 0; cursor <= pool->organisms_number; cursor++)
        genomes[cursor] = read_next_genome(pool);

    reset_genome_cursor(pool);
    return genomes;

}

/*

Destroys array of pointers to genomes.

*/
void free_genomes_ptrs(genome_t ** genomes) {
    free(genomes);
}

#define LEFT_ZERO_UINT8(_NUMBER, _OFFSET) \
    ((uint8_t)(_NUMBER << _OFFSET) >> _OFFSET)
    
#define RIGHT_ZERO_UINT8(_NUMBER, _OFFSET) \
    ((uint8_t)(_NUMBER >> _OFFSET) << _OFFSET)

/*

Set `number` to 0 and copy bits within given range [start, end] into number.
`slots` is array of uint8_t, for example: [0b11111010, 0b11111111]
Result of copy_bitslots_to_uint64(slots, number, 5, 13) will be (13-5=8)
copied bits into number, so now:
number == 0b00000000...0000000010101111, sizeof(number) == 64

*/
// void copy_bitslots_to_uint64__endianless(
//     uint8_t *slots, uint64_t *number, uint8_t start, uint8_t end
// ) {

//     *number = 0;

//     // right offset of the first bit in `number`
//     uint8_t number_offset = end - start + 1;

//     for(
//         uint8_t slot_n = start / 8;
//         slot_n <= end / 8;
//         slot_n++
//     ) {

//         uint8_t slot = slots[slot_n];
//         uint16_t offset;

//         // there's some left offset in the current slot present
//         if (slot_n * 8 < start) {
//             offset = start - slot_n * 8;
//             *number |=
//                 LEFT_ZERO_UINT8(slot, offset) << (number_offset - 8 + offset);
//         }

//         // there's right offset
//         else if ((end + 1) < (slot_n + 1) * 8) {
//             offset = (slot_n + 1) * 8 - end - 1;
//             *number |= RIGHT_ZERO_UINT8(slot, offset) >> offset;
//         }

//         // copy the whole byte
//         else {
//             offset = 0;
//             *number |= slot << (number_offset - 8);
//         }

//         number_offset -= 8 - offset;

//     }

// }

void copy_bitslots_to_uint64(
    uint8_t *slots, uint64_t *number, uint8_t start, uint8_t end
) {

    uint8_t byte_offset = start / 8,
            bit_offset = start % 8;
    
    uint64_t copy_number = *(uint64_t *)(slots + byte_offset);
    
    copy_number = hton64(copy_number) << bit_offset;
        
    *number = copy_number >> (64 - end + start - 1);

}

/*

Does the opposite to copy_bitslots_to_uint64.

*/
void copy_uint64_to_bitslots(
    uint64_t *number, uint8_t *slots, uint8_t start, uint8_t number_size
) {

    uint8_t byte_offset = start / 8,
            bit_offset = start % 8;
    
    uint64_t copy_number = *number << (64 - bit_offset - number_size);
    
    *(uint64_t *)(slots + byte_offset) |= hton64(copy_number);

}

#define MAX_FOR_BIT(_BIT_SIZE) \
    (_BIT_SIZE == 64 ? 0xffffffffffff : (1 << _BIT_SIZE) - 1)

uint8_t * point_gene_by_index(
    genome_t *genome, uint32_t index, pool_t *pool
) {
    return genome->genes + (pool->gene_bytes_size * index);
}

inline gene_t * get_gene_by_pointer(
    uint8_t *gene_start_byte, pool_t *pool
) {

    gene_t *gene = calloc(1, sizeof(gene_t));

    copy_bitslots_to_uint64(
        gene_start_byte,
        &(gene->outcome_node_id),
        0,
        pool->node_id_part_bit_size - 1);

    copy_bitslots_to_uint64(
        gene_start_byte,
        &(gene->income_node_id),
        pool->node_id_part_bit_size,
        pool->node_id_part_bit_size * 2 - 1);

    copy_bitslots_to_uint64(
        gene_start_byte,
        // sign of number is not important, just copy all the bits
        (uint64_t *)&(gene->weight_unnormalized),
        pool->node_id_part_bit_size * 2,
        pool->node_id_part_bit_size * 2 + pool->weight_part_bit_size - 1);

    gene->weight =
        gene->weight_unnormalized / MAX_FOR_BIT(pool->weight_part_bit_size);

    uint64_t first_output_neuron_id =
        MAX_FOR_BIT(pool->weight_part_bit_size) - pool->output_neurons_number + 1;

    // type of outcome node

    if (gene->outcome_node_id <= pool->input_neurons_number - 1)
        gene->connection_type |= GENE_OUTCOME_IS_INPUT;

    else
    if (gene->outcome_node_id >= first_output_neuron_id)
        gene->connection_type |= GENE_OUTCOME_IS_OUTPUT;

    else
        gene->connection_type |= GENE_OUTCOME_IS_INTERMEDIATE;

    // type of income node

    if (gene->income_node_id <= pool->input_neurons_number - 1)
        gene->connection_type |= GENE_INCOME_IS_INPUT;

    else
    if (gene->income_node_id >= first_output_neuron_id)
        gene->connection_type |= GENE_INCOME_IS_OUTPUT;

    else
        gene->connection_type |= GENE_INCOME_IS_INTERMEDIATE;

    return gene;

}

gene_t * get_gene_by_index(genome_t *genome, uint32_t index, pool_t *pool) {

    return get_gene_by_pointer(
        point_gene_by_index(genome, index, pool),
        pool
    );

}
