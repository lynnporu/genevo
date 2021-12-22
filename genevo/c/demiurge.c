#include "demiurge.h"

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
void fill_with_randomness(uint8_t *destination, uint32_t bytes, uint8_t bits) {

	#ifndef SKIP_RND_SEED_CHECK
		ENSURE_RND_SEED_IS_SET;
	#endif

	// fill whole bytes first

	#define FILL_NEXT(_SIZE, _TYPE)                                            \
		if (left_bytes >= _SIZE) {                                             \
			*(_TYPE *)destination = next_random64();                           \
			destination += _SIZE;                                              \
			left_bytes -= _SIZE;                                               \
		}

	for (uint8_t left_bytes = bytes; left_bytes > 0; ) {
		// this will work faster than just filling all the bytes
		// with next_random64()
		FILL_NEXT(8, uint64_t);
		FILL_NEXT(4, uint32_t);
		FILL_NEXT(2, uint16_t);
		FILL_NEXT(1, uint8_t );
	}

	#undef FILL_NEXT

	// partially fill one left byte

	*(uint8_t *)destination = (uint8_t)next_random64() << (8 - bits);

}

void generate_genome_data(
	genome_t *genome, uint64_t bits_number, uint8_t gene_byte_size,
	generator_mode_t generator_mode
) {

	// float will round down
	genome->length = bits_number / gene_byte_size;
	genome->residue_size_bits = bits_number - genome->length * gene_byte_size;

	if (generator_mode == GENERATE_RANDOMNESS) {
		fill_bytes_with_randomness(genome->genes, genome->length * gene_byte_size);
		fill_bits_with_randomness(genome->residue, genome->residue_size_bits);
	}
	else
	if (generator_mode == GENERATE_ZEROS) {
		memset(genome->genes, 0, genome->length * gene_byte_size);
		memset(genome->residue, 0, (genome->residue_size_bits / 8) + 1);
	}
	else {
		ERROR_LEVEL = ERR_WRONG_FLAG;
		return;
	}

}

/*

Allocate genome_t.
genes_bytes_size and residue_size_bits will be ignored in case allocate_data
is `false`.

*/
genome_t *allocate_genome(
	bool allocate_data,
	uint64_t genes_bytes_size, uint16_t residue_size_bits
) {

	genome_t *genome = malloc(sizeof(genome_t));

	if (allocate_data) {
		genome->genes = malloc(genes_bytes_size);
		genome->residue = malloc(residue_size_bits / 8);
		genome->residue_size_bits = residue_size_bits;
	} else {
		genome->genes = NULL;
		genome->residue = NULL;
		genome->residue_size_bits = 0;
	}

	genome->metadata = NULL;
	genome->metadata_byte_size = 0;

	return genome;

}

genome_t ** const allocate_genome_vector (
	uint64_t size, bool allocate_data,
	uint32_t genes_number, uint8_t gene_bytes_size,
	uint32_t genome_bit_size
) {

	// allocate each genome and genomes vector
	genome_t ** const genomes = malloc(sizeof(genome_t *) * size);

	for(uint64_t genome_itr = 0; genome_itr < size; genome_itr++)
		genomes[genome_itr] = allocate_genome(
			allocate_data, genes_number, gene_bytes_size, genome_bit_size);

	return genomes;

}

	if (deallocate_data) {
		if (genome->genes != NULL) free(genome->genes);
		if (genome->residue != NULL) free(genome->residue);
	}

	delete_genome_metadata(genome);

	free(genome);

}

void assign_genome_metadata(
	genome_t *genome, uint16_t metadata_byte_size, const char *metadata
) {

	delete_genome_metadata(genome);
	genome->metadata = malloc(metadata_byte_size);
	memcpy(genome->metadata, metadata, metadata_byte_size);

}

void delete_genome_metadata(genome_t *genome) {

	if (genome->metadata != NULL) free(genome->metadata);

}

pool_t *allocate_pool() {

	pool_t *pool = malloc(sizeof(pool_t));

	pool->metadata = NULL;
	pool->metadata_byte_size = 0;

	return pool;

}

void destroy_pool(pool_t *pool, bool close_file) {

	if (close_file) close_file_for_pool(pool);
	delete_pool_metadata(pool);

	free(pool);

}

void assign_pool_metadata(
	pool_t *pool, uint16_t metadata_byte_size, const char* metadata
) {

	delete_pool_metadata(pool);
	pool->metadata = malloc(metadata_byte_size);
	memcpy(pool->metadata, metadata, metadata_byte_size);

}

void delete_pool_metadata(pool_t *pool) {

	if (pool->metadata != NULL) free(pool->metadata);

}

/*

Generate genomes for pool. For this function the following members of
pool_t must be set:
	* organisms_number
	* node_id_part_bit_size
	* weight_part_bit_size

*/
void fill_pool(
	const char *address,
	pool_and_genomes_t *pool_and_genomes,
	uint64_t genome_bit_size,
	generator_mode_t generator_mode
) {

	open_file_for_pool(
		address,
		pool_and_genomes->pool,
		pool_and_genomes->genomes);
	save_pool(
		pool_and_genomes->pool,
		pool_and_genomes->genomes,
		POOL_ASSIGN_GENOME_POINTERS);

	// fill each genome with values
	for(
		uint64_t genome_itr = 0;
		genome_itr < pool_and_genomes->pool->organisms_number;
		genome_itr++
	) {
		generate_genome_data(
			pool_and_genomes->genomes[genome_itr],
			genome_bit_size,
			pool_and_genomes->pool->gene_bytes_size,
			generator_mode);
	}

}

pool_and_genomes_t * const create_pool_in_file(
	uint64_t organisms_number,
	uint8_t node_id_bit_size, uint8_t weight_bit_size,
	uint64_t input_neurons_number, uint64_t output_neurons_number,
	uint64_t genome_bit_size,
	generator_mode_t generator_mode
) {

	pool_t * const pool = allocate_pool();

	pool->organisms_number = organisms_number;
	pool->input_neurons_number = input_neurons_number;
	pool->output_neurons_number = output_neurons_number;
	pool->node_id_part_bit_size = node_id_bit_size;
	pool->weight_part_bit_size = weight_bit_size;
	pool->gene_bytes_size =
		pool->node_id_part_bit_size * 2 + pool->weight_part_bit_size;

	uint32_t genes_number = genome_bit_size / pool->gene_bytes_size,
		     genome_residue_size = genome_bit_size % pool->gene_bytes_size;

	genome_t ** const genomes = allocate_genome_vector(
		organisms_number, false /* allocate data */,
		genes_number, pool->gene_bytes_size, genome_bit_size
	);

	pool_and_genomes_t *pool_and_genomes = malloc(sizeof(pool_and_genomes_t));

	pool_and_genomes->pool = pool;
	pool_and_genomes->genomes = genomes;

	// TODO: come up with a name
	fill_pool("????", pool, genome_bit_size, generator_mode);

	return pool_and_genomes;

}

/*

Destroy pool_and_genomes_t struct and its member `genomes`.
If destroy_genomes is true, then destroy_genome will be called on every genome.
deallocate_genomes_data will be passed into destroy_genome.

*/
void destroy_pool_and_genomes(
	pool_and_genomes_t *pool_and_genomes,
	bool destroy_genomes, bool deallocate_genomes_data
) {

	if (destroy_genomes)
		for (
			uint64_t genome_itr = 0;
			genome_itr < pool_and_genomes->pool->organisms_number;
			genome_itr++
		)
			destroy_genome(
				pool_and_genomes->genomes[genome_itr],
				deallocate_genomes_data);

	free(pool_and_genomes->genomes);
	free(pool_and_genomes);

}
