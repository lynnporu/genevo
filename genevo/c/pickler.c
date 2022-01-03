/*

This module contains methods for dumping gene pool into file and vice versa.

 */


// New year 2022 commit

#include "pickler.h"

#define sizeof_member(type, member) sizeof(((type *)0)->member)

#define COPY_MEMBER(_MEMB_NAME, _STRUCT_SRC, _STRUCT_DIST)                     \
    { _STRUCT_DIST -> _MEMB_NAME = _STRUCT_SRC -> _MEMB_NAME; }

#define COPY_MEMBER_WITH_SWAP(_MEMB_NAME, _STRUCT_SRC, _STRUCT_DIST, _DIRECTION)\
    {                                                                          \
        _STRUCT_DIST -> _MEMB_NAME = _DIRECTION(_STRUCT_SRC -> _MEMB_NAME);    \
    }

#define COPY_MEMBER_HTON(_MEMB_NAME, _STRUCT_SRC, _STRUCT_DIST)                \
    COPY_MEMBER_WITH_SWAP(_MEMB_NAME, _STRUCT_SRC, _STRUCT_DIST, HTON)

#define COPY_MEMBER_NTOH(_MEMB_NAME, _STRUCT_SRC, _STRUCT_DIST)                \
    COPY_MEMBER_WITH_SWAP(_MEMB_NAME, _STRUCT_SRC, _STRUCT_DIST, NTOH)

#define MAPPING_FAIL_CONDITION(_CONDITION, _ERR_CONST) \
    if(_CONDITION) {ERROR_LEVEL = (_ERR_CONST); close_file(mapping); return NULL;}

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

    COPY_MEMBER_NTOH(organisms_number,      preamble, pool);
    COPY_MEMBER_NTOH(metadata_byte_size,    preamble, pool);
    COPY_MEMBER_NTOH(input_neurons_number,  preamble, pool);
    COPY_MEMBER_NTOH(output_neurons_number, preamble, pool);

    pool->metadata = (byte_t *)(&preamble->metadata_initial_byte + 1);

    pool->node_id_part_bit_size = preamble->node_id_part_bit_size;
    pool->weight_part_bit_size = preamble->weight_part_bit_size;
    pool->gene_bytes_size = BITS_TO_BYTES(
        pool->node_id_part_bit_size * 2 +
        pool->weight_part_bit_size);

    pool->first_genome_start_position =
        &preamble->metadata_initial_byte + 1 + pool->metadata_byte_size + 1;

    pool->cursor = pool->first_genome_start_position;

    return pool;

}

void open_file_for_pool(const char *address, pool_t *pool, genome_t **genomes) {

    ERROR_LEVEL = 0;

    size_t file_size =
        sizeof(pool_file_preamble_t) +
        pool->metadata_byte_size +
        sizeof(POOL_META_TERMINAL_BYTE) +
        sizeof(POOL_TERMINAL_BYTE);

    for (
        pool_organisms_num_t genome_i = 0;
        genome_i < pool->organisms_number;
        genome_i++
    )
        file_size +=
            sizeof(genome_file_preamble_t) +
            genomes[genome_i]->metadata_byte_size +
            sizeof(GENOME_META_TERMINAL_BYTE) +
            genomes[genome_i]->length * pool->gene_bytes_size +
            sizeof(GENOME_RESIDUE_BYTE) +
            sizeof_member(genome_t, residue_size_bits) +
            (genome_residue_size_t)BITS_TO_BYTES(
                genomes[genome_i]->residue_size_bits) +
            sizeof(GENOME_TERMINAL_BYTE);

    file_map_t *mapping = open_file(address, OPEN_MODE_WRITE, file_size - 1);
    if (ERROR_LEVEL != ERR_OK) return;

    pool->file_mapping = mapping;

}

void close_file_for_pool(pool_t * const pool) {
    close_file(pool->file_mapping);
}

void save_pool(
    pool_t * const pool, genome_t ** const genomes, save_pool_flag_t flags
) {

    ERROR_LEVEL = ERR_OK;

    if (
        (flags & POOL_COPY_DATA && flags & POOL_ASSIGN_GENOME_POINTERS) ||
        (flags & POOL_COPY_METADATA && flags & POOL_ASSIGN_METADATA_POINTERS)
    ) {
        ERROR_LEVEL = ERR_INCOMPATIBLE_FLAGS;
        return;
    }

    pool_file_preamble_t *pool_preamble = pool->file_mapping->data;
    if (flags & POOL_REWRITE_DESCRIPTION) {
        pool_preamble->initial_byte         = POOL_INITIAL_BYTE;
        COPY_MEMBER_HTON(organisms_number,        pool, pool_preamble);
        COPY_MEMBER_HTON(input_neurons_number,    pool, pool_preamble);
        COPY_MEMBER_HTON(output_neurons_number,   pool, pool_preamble);
        COPY_MEMBER     (node_id_part_bit_size,   pool, pool_preamble);
        COPY_MEMBER     (weight_part_bit_size,    pool, pool_preamble);
        COPY_MEMBER_HTON(metadata_byte_size,      pool, pool_preamble);
        pool_preamble->metadata_initial_byte = POOL_META_INITIAL_BYTE;
    }

    byte_t *pool_metadata = &pool_preamble->metadata_initial_byte + 1;

    // copy pool meta bytes
    if (flags & POOL_COPY_METADATA)
        memcpy(pool_metadata, pool->metadata, pool->metadata_byte_size);

    if (flags & POOL_ASSIGN_METADATA_POINTERS)
        pool->metadata = pool_metadata;

    void *pool_meta_terminal_byte = pool_metadata + pool->metadata_byte_size;
    if (flags & POOL_REWRITE_DESCRIPTION)
        *(file_control_byte_t *)pool_meta_terminal_byte = POOL_META_TERMINAL_BYTE;

    // genome pointer
    genome_file_preamble_t *genome_preamble = pool_meta_terminal_byte + 1;

    for(
        // genome iterator
        pool_organisms_num_t genome_itr = 0;
        genome_itr < pool->organisms_number;
        genome_itr++
    ) {

        genome_t *current_genome = genomes[genome_itr];

        if (flags & POOL_REWRITE_DESCRIPTION) {
            genome_preamble->initial_byte = GENOME_INITIAL_BYTE;
            COPY_MEMBER_HTON(length,             current_genome, genome_preamble);
            COPY_MEMBER_HTON(metadata_byte_size, current_genome, genome_preamble);
            genome_preamble->metadata_initial_byte = GENOME_META_INITIAL_BYTE;
        }

        byte_t *genome_metadata = &genome_preamble->metadata_initial_byte + 1;

        // copy genome meta bytes
        if (flags & POOL_COPY_METADATA)
            memcpy(
                genome_metadata, current_genome->metadata,
                current_genome->metadata_byte_size);

        if (flags & POOL_ASSIGN_METADATA_POINTERS)
            current_genome->metadata = genome_metadata;

        uint8_t residue_size_bytes = BITS_TO_BYTES(
            current_genome->residue_size_bits);

        #define GENES_BYTES_SIZE (current_genome->length * pool->gene_bytes_size)

        void *genome_meta_terminal_byte =
            genome_metadata + current_genome->metadata_byte_size;
        void *residue_byte = genome_meta_terminal_byte + 1 + GENES_BYTES_SIZE;
        void *residue_start = residue_byte + 1 + sizeof(current_genome->residue_size_bits);
        void *terminal_byte = residue_start + residue_size_bytes;

        if (flags & POOL_REWRITE_DESCRIPTION) {
            *(uint8_t *)genome_meta_terminal_byte = GENOME_META_TERMINAL_BYTE;
            *(uint8_t *)residue_byte = GENOME_RESIDUE_BYTE;
            *(uint16_t *)(residue_byte + 1) =
                HTON(current_genome->residue_size_bits);
            *(uint8_t *)terminal_byte = GENOME_TERMINAL_BYTE;
        }

        if (flags & POOL_COPY_DATA) {
            // copy genes
            memcpy(
                genome_meta_terminal_byte + 1, current_genome->genes,
                GENES_BYTES_SIZE);

            memcpy(
                residue_start, current_genome->residue, residue_size_bytes);
        }

        if (flags & POOL_ASSIGN_GENOME_POINTERS) {
            current_genome->genes = genome_meta_terminal_byte + 1;
            current_genome->residue = residue_start;
        }

        #undef GENES_BYTES_SIZE

        genome_preamble = terminal_byte + 1;

    }

    if (flags & POOL_REWRITE_DESCRIPTION)
        genome_preamble->initial_byte = POOL_TERMINAL_BYTE;

}

void write_pool(
    const char *address, pool_t * const pool, genome_t ** const genomes
) {

    if (pool->file_mapping != NULL) close_file_for_pool(pool);
    open_file_for_pool(address, pool, genomes);
    save_pool(
        pool, genomes,
        POOL_COPY_DATA | POOL_REWRITE_DESCRIPTION | POOL_COPY_METADATA);
    close_file_for_pool(pool);

}

void close_pool(pool_t * const pool) {
    if (pool->file_mapping != NULL) close_file_for_pool(pool);
    free(pool);
}

void reset_genome_cursor(pool_t * const pool) {
    pool->cursor = pool->first_genome_start_position;
}


genome_t * read_next_genome(pool_t * const pool) {

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

    COPY_MEMBER_NTOH(length,             preamble, genome);
    COPY_MEMBER_NTOH(metadata_byte_size, preamble, genome);

    genome->metadata = &preamble->metadata_initial_byte + 1;

    void *genome_meta_terminal_byte =
        genome->metadata + genome->metadata_byte_size;

    if (*(uint8_t *)genome_meta_terminal_byte != GENOME_META_TERMINAL_BYTE) {
        ERROR_LEVEL = ERR_GENM_CORRUPT_METADATA_END;
        return NULL;
    }

    genome->genes = genome_meta_terminal_byte + 1;

    void * residue_byte = (
        genome->genes +
        pool->gene_bytes_size * genome->length);

    if (*(uint8_t *)residue_byte != GENOME_RESIDUE_BYTE) {
        ERROR_LEVEL = ERR_GENM_CORRUPT_RESIDUE;
        free(genome);
        return NULL;
    }

    genome->residue_size_bits = NTOH(
        *(uint16_t *)(residue_byte + sizeof(uint8_t)));

    genome->residue =
        residue_byte + sizeof(uint8_t) + sizeof_member(genome_t, residue_size_bits);

    void * terminal_byte =
        genome->residue + BITS_TO_BYTES(genome->residue_size_bits);

    if (*(uint8_t *)terminal_byte != GENOME_TERMINAL_BYTE) {
        ERROR_LEVEL = ERR_GENM_CORRUPT_END;
        free(genome);
        return NULL;
    }

    pool->cursor = terminal_byte + 1;

    return genome;

}

genome_t ** read_genomes(pool_t * const pool) {

    genome_t ** genomes = malloc(sizeof(genome_t) * pool->organisms_number);

    for(uint64_t cursor = 0; cursor <= pool->organisms_number; cursor++)
        genomes[cursor] = read_next_genome(pool);

    reset_genome_cursor(pool);
    return genomes;

}

/*

Destroys array of pointers to genomes.

 */
void free_genomes_ptrs(genome_t ** const genomes) {
    free(genomes);
}

/*

Set `number` to 0 and copy bits within given range [start, end] into number.
`slots` is array of uint8_t, for example: [0b11111010, 0b11111111]
Result of copy_bitslots_to_uint64(slots, number, 5, 13) will be (13-5=8)
copied bits into number, so now:
number == 0b00000000...0000000010101111, sizeof(number) == 64

 */
void copy_bitslots_to_uint64(
    const gene_byte_t* slots, uint64_t * const number, uint8_t start, uint8_t end
) {

    const uint8_t byte_offset = start / 8,
                  bit_offset = start % 8;
    
    uint64_t copy_number = *(uint64_t *)(slots + byte_offset);
    
    // Delete left unwanted bits with left shifting
    copy_number <<= bit_offset;

    // Delete right unwanted bits should be done with considering byte
    // alignment.
    // For example: 1111 1111 1000 0000 -> 0000 1111 1111 1000. Here also 3
    // last bits should be manually set to 0
    const uint8_t number_size = end - start + 1;
    const uint8_t ceil_bit_size = ceil(number_size / 8) * 8;

    // Number represented with bytes [0xa, 0xb, 0xc] >> 8 on litle endian
    // platform will give us number [0xb, 0xc] and vice versa. So in order to
    // erase right bits we should do left shifting.

    #ifdef IS_BIG_ENDIAN
    copy_number >>= 64 - number_size;
    #endif

    #ifdef IS_LITTLE_ENDIAN
    copy_number <<= 64 - number_size;
    #endif

    const uint8_t unwanted_right_bits_num = ceil_bit_size - number_size;
    copy_number &= ~((1 << unwanted_right_bits_num) - 1);

    // On big endian platform number has already properly placed bytes

    #ifdef IS_LITTLE_ENDIAN
    copy_number = bswap_64(copy_number);
    #endif

    *number = copy_number;

}

/*

Does the opposite to copy_bitslots_to_uint64.

 */
void copy_uint64_to_bitslots(
    const uint64_t *number, gene_byte_t * const slots, uint8_t start, uint8_t number_size
) {

    uint8_t byte_offset = start / 8,
            bit_offset = start % 8;
    
    uint64_t copy_number = *number << (64 - bit_offset - number_size);
    
    *(uint64_t *)(slots + byte_offset) |= HTON(copy_number);

}

uint8_t * point_gene_in_genome_by_index(
    genome_t * const genome, uint32_t index, pool_t * const pool
) {
    return genome->genes + (pool->gene_bytes_size * index);
}

uint8_t * point_gene_by_index(
    gene_byte_t * const genes, uint32_t index, pool_t * const pool
) {
    return genes + (pool->gene_bytes_size * index);
}

/*

Guess node type (input, output or intermediate) by its ID and sizes of input
and output ranges. New ID to `_ID` and type to `_CONNECTION_TYPE_VAR`.
`_DIRECTION` should be one of "INPUT" or "OUTPUT".

! Potential bug: _ID will be calculated several times when passed as the
  expression.

 */
#define ASSIGN_TYPE_BY_ID(_ID,                                                 \
                          _INPUT_RANGE_SIZE, _OUTPUT_RANGE_SIZE,               \
                          _NODES_CAPACITY,                                     \
                          _CONNECTION_TYPE_VAR, _DIRECTION)                    \
{                                                                              \
    if (_ID < _INPUT_RANGE_SIZE) {                                             \
        _CONNECTION_TYPE_VAR |= GENE_  ## _DIRECTION ## _IS_INPUT; }           \
    else                                                                       \
    if (_ID > _NODES_CAPACITY - _OUTPUT_RANGE_SIZE) {                          \
        _CONNECTION_TYPE_VAR |= GENE_ ## _DIRECTION ## _IS_OUTPUT;             \
        _ID -= _NODES_CAPACITY - _OUTPUT_RANGE_SIZE + 1; }                     \
    else {                                                                     \
        _CONNECTION_TYPE_VAR |= GENE_ ## _DIRECTION ## _IS_INTERMEDIATE;       \
        _ID -= _INPUT_RANGE_SIZE; }                                            \
}

#define ASSIGN_ID_BY_TYPE(_ID, _TYPE,                                          \
                           _INPUT_RANGE_SIZE, _OUTPUT_RANGE_SIZE,              \
                           _NODES_CAPACITY)                                    \
{                                                                              \
    if (_TYPE & (GENE_INCOME_IS_INTERMEDIATE | GENE_OUTCOME_IS_INTERMEDIATE))  \
        _ID += _INPUT_RANGE_SIZE;                                              \
    if (_TYPE & (GENE_INCOME_IS_OUTPUT | GENE_OUTCOME_IS_OUTPUT))              \
        _ID += _NODES_CAPACITY - _OUTPUT_RANGE_SIZE;                           \
}


gene_t * get_gene_by_pointer(gene_byte_t * const gene_start_byte, pool_t * const pool) {

    gene_t *gene = malloc(sizeof(gene_t));

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

    gene->weight = NORMALIZE_FROM_BIT_WIDTH(
        gene->weight_unnormalized,
        pool->weight_part_bit_size);

    uint64_t nodes_capacity = MAX_FOR_BIT_WIDTH(pool->node_id_part_bit_size);

    gene->connection_type = 0;

    // outcome node
    ASSIGN_TYPE_BY_ID(
        gene->outcome_node_id,                                   // id
        pool->input_neurons_number, pool->output_neurons_number, // range sizes
        nodes_capacity,
        gene->connection_type,
        OUTCOME);                                                // direction

    // outcome node
    ASSIGN_TYPE_BY_ID(
        gene->income_node_id,                                    // id
        pool->input_neurons_number, pool->output_neurons_number, // range sizes
        nodes_capacity,
        gene->connection_type,
        INCOME);                                                 // direction

    return gene;

}

gene_t * get_gene_in_genome_by_index(
    genome_t * const genome, uint32_t index, pool_t * const pool
) {

    #ifndef SKIP_CHECK_BOUNDS
    if (index >= genome->length) {
        ERROR_LEVEL = ERR_OUT_OF_BOUNDS;
        return NULL;
    }
    #endif

    return get_gene_by_pointer(
        point_gene_in_genome_by_index(genome, index, pool),
        pool 
    );

}

gene_t * get_gene_by_index(gene_byte_t * const genes, uint32_t index, pool_t * const pool) {

    return get_gene_by_pointer(
        point_gene_by_index(genes, index, pool),
        pool
    );

}

gene_byte_t * genes_to_byte_array(
    gene_t ** const genes, pool_t * const pool, uint64_t length
) {

    #define GENES_ARRAY_SIZE \
        (sizeof(gene_byte_t) * length * pool->gene_bytes_size)

    // 8 auxiliary bytes is malloc'ed here, because
    // copy_uint64_to_bitslots can cause writing out of bounds.
    gene_byte_t *array = malloc(GENES_ARRAY_SIZE + 8);
    memset(array, 0, GENES_ARRAY_SIZE);

    #undef GENES_ARRAY_SIZE

    uint64_t nodes_capacity = MAX_FOR_BIT_WIDTH(pool->gene_bytes_size);

    gene_byte_t* start_byte;
    gene_t* gene;
    for (uint64_t counter = 0; counter < length; counter++) {

        start_byte = array + counter * pool->gene_bytes_size;
        gene = genes[counter];

        uint64_t outcome_node_id = gene->outcome_node_id,
                 income_node_id = gene->income_node_id;

        ASSIGN_ID_BY_TYPE(
            outcome_node_id,
            gene->connection_type,
            pool->input_neurons_number, pool->output_neurons_number,
            nodes_capacity);

        ASSIGN_ID_BY_TYPE(
            income_node_id,
            gene->connection_type,
            pool->input_neurons_number, pool->output_neurons_number,
            nodes_capacity);

        copy_uint64_to_bitslots(
            &outcome_node_id,
            start_byte,
            0,
            pool->node_id_part_bit_size);

        copy_uint64_to_bitslots(
            &income_node_id,
            start_byte,
            pool->node_id_part_bit_size,
            pool->node_id_part_bit_size);

        copy_uint64_to_bitslots(
            (uint64_t *)&(gene->weight_unnormalized),
            start_byte,
            pool->node_id_part_bit_size * 2,
            pool->weight_part_bit_size);

    }

    return array;

}

void free_genes_byte_array(gene_byte_t * const array) {

    free(array);

}
