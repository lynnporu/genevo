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
			continue;                                                          \
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

/*

For this function genome->length and genome->residue_size_bits should be set.

 */
void generate_genome_data(
	genome_t * const genome, uint8_t gene_byte_size,
	generator_mode_t generator_mode
) {

	if (generator_mode == GENERATE_RANDOMNESS) {
		fill_bytes_with_randomness(genome->genes, genome->length * gene_byte_size);
		fill_bits_with_randomness(genome->residue, genome->residue_size_bits);
	}
	else
	if (generator_mode == GENERATE_ZEROS) {
		memset(genome->genes, 0, genome->length * gene_byte_size);
		memset(genome->residue, 0, BITS_TO_BYTES(genome->residue_size_bits));
	}
	else {
		ERROR_LEVEL = ERR_WRONG_FLAG;
		return;
	}

}

/*

Allocate genome_t. If allocate_data is true, then genome->genes array of size
`genes_bytes_size * length` will be allocated.
Residue size will be calcualted and assigned to genome->residue_size_bits,
`length` will be assigned to genome->length.

 */
genome_t * allocate_genome(
	bool allocate_data,
	genome_length_t length, uint8_t gene_bytes_size,
	uint32_t genome_bit_size
) {

	genome_t * const genome = malloc(sizeof(genome_t));

	uint64_t genome_byte_size = gene_bytes_size * length;
	uint16_t residue_size_bits = genome_bit_size - BYTES_TO_BITS(genome_byte_size);

	if (allocate_data) {
		genome->genes = malloc(genome_byte_size);
		genome->residue = malloc(BITS_TO_BYTES(residue_size_bits));
	} else {
		genome->genes = NULL;
		genome->residue = NULL;
	}

	genome->length = length;
	genome->residue_size_bits = residue_size_bits;
	genome->metadata = NULL;
	genome->metadata_byte_size = 0;

	return genome;

}

genome_t ** allocate_genome_vector(
	pool_organisms_num_t size, bool allocate_data,
	genome_length_t genes_number, uint8_t gene_bytes_size,
	uint32_t genome_bit_size
) {

	// allocate each genome and genomes vector
	genome_t ** const genomes = malloc(sizeof(genome_t *) * size);

	for(pool_organisms_num_t genome_itr = 0; genome_itr < size; genome_itr++)
		genomes[genome_itr] = allocate_genome(
			allocate_data, genes_number, gene_bytes_size, genome_bit_size);

	return genomes;

}

void destroy_genomes_vector(
	pool_organisms_num_t size, bool deallocate_data, bool destroy_each_genome,
	genome_t ** const genomes
) {
	if (destroy_each_genome)
		for (
			pool_organisms_num_t genome_itr = 0;
			genome_itr < size; genome_itr++
		)
			destroy_genome(genomes[genome_itr], deallocate_data);

	free(genomes);
}

void destroy_genome(genome_t * const genome, bool deallocate_data) {

	if (deallocate_data) {
		if (genome->genes != NULL) free(genome->genes);
		if (genome->residue != NULL) free(genome->residue);
	}

	delete_genome_metadata(genome);

	free(genome);

}

void assign_genome_metadata(
	genome_t * const genome, genome_metadata_size_t metadata_byte_size,
	const char *metadata
) {

	delete_genome_metadata(genome);
	genome->metadata = malloc(metadata_byte_size);
	memcpy(genome->metadata, metadata, metadata_byte_size);

}

void delete_genome_metadata(genome_t * const genome) {

	if (genome->metadata != NULL) free(genome->metadata);

}

pool_t * allocate_pool() {

	pool_t * const pool = malloc(sizeof(pool_t));

	pool->metadata = NULL;
	pool->metadata_byte_size = 0;

	return pool;

}

void destroy_pool(pool_t * const pool, bool close_file) {

	if (close_file) close_file_for_pool(pool);
	delete_pool_metadata(pool);

	free(pool);

}

void assign_pool_metadata(
	pool_t * const pool, pool_metadata_size_t metadata_byte_size,
	const char *metadata
) {

	delete_pool_metadata(pool);
	pool->metadata = malloc(metadata_byte_size);
	memcpy(pool->metadata, metadata, metadata_byte_size);

}

void delete_pool_metadata(pool_t * const pool) {

	if (pool->metadata != NULL) free(pool->metadata);

}


/*

Create name for a file in which the pool will be allocated. New name will be
created based on current timestamp and a pointer to the pool.
! This function allocates new memory, which should be freed after use.

 */
char * alloc_name_for_pool(pool_t *pool) {

	uint64_t number = time(NULL) + (uint64_t)pool;
	// maximum size of uint64 in hex is 9 symbols + ".pool"
	char *address = calloc(sizeof(char), 9 + 5);

	// in case printed string is less than (9 + 5), symbols, the last bit is
	// \0 anyway, so it will suit well for functions taking (const char *)
	sprintf(address, "%lX.pool", number);

	return address;

}


/*

Generate genomes for pool. For this function the following members of
pool_t must be set:
	* organisms_number
	* node_id_part_bit_size
	* weight_part_bit_size

 */
void fill_pool(
	const char *address, population_t * const population,
	generator_mode_t generator_mode
) {

	#ifndef ERROR_ON_EMPTY_FILENAME_FOR_POOL
	bool address_is_allocated = false;
	char *allocated_address;
	#endif

	if (address == NULL) {
	#ifndef ERROR_ON_EMPTY_FILENAME_FOR_POOL
		allocated_address = alloc_name_for_pool(population->pool);
		address = allocated_address;
		address_is_allocated = true;
	#else
		ERROR_LEVEL = ERR_NOT_ENOUGH_PARAMS;
		return;
	#endif
	}

	open_file_for_pool(
		address, population->pool, population->genomes);
	save_pool(
		population->pool, population->genomes, POOL_ASSIGN_GENOME_POINTERS);

	#ifndef ERROR_ON_EMPTY_FILENAME_FOR_POOL
	if (address_is_allocated) free(allocated_address);
	#endif

	// fill each genome with values
	for(
		uint64_t genome_itr = 0;
		genome_itr < population->pool->organisms_number;
		genome_itr++
	) {
		generate_genome_data(
			population->genomes[genome_itr],
			population->pool->gene_bytes_size, generator_mode);
	}

}

population_t * create_pool_in_file(
	pool_organisms_num_t organisms_number,
	pool_gene_node_id_part_t node_id_bit_size,
	pool_gene_weight_part_t weight_bit_size,
	pool_neurons_num_t input_neurons_number,
	pool_neurons_num_t output_neurons_number,
	uint64_t genome_bit_size,
	generator_mode_t generator_mode
) {

	pool_t * const pool = allocate_pool();

	pool->organisms_number = organisms_number;
	pool->input_neurons_number = input_neurons_number;
	pool->output_neurons_number = output_neurons_number;
	pool->node_id_part_bit_size = node_id_bit_size;
	pool->weight_part_bit_size = weight_bit_size;

	uint8_t gene_bit_size =
		pool->node_id_part_bit_size * 2 + pool->weight_part_bit_size;

	if (gene_bit_size % 8 /*!= 0*/) {
		ERROR_LEVEL = ERR_GENE_NOT_ALIGNED;
		destroy_pool(pool, false /*close file*/);
		return NULL;
	}

	pool->gene_bytes_size = BITS_TO_BYTES(gene_bit_size);

	genome_length_t genes_number = genome_bit_size / gene_bit_size;

	genome_t ** const genomes = allocate_genome_vector(
		organisms_number, false /* allocate data  */,
		genes_number, pool->gene_bytes_size, genome_bit_size
	);

	population_t * const population = malloc(sizeof(population_t));

	population->pool = pool;
	population->genomes = genomes;

	fill_pool(
		NULL, // file address
		population, generator_mode);

	return population;

}

/*

Destroy population_t struct and its member `genomes`.
If destroy_genomes is true, then destroy_genome will be called on every genome.
deallocate_genomes_data will be passed into destroy_genome.

 */
void destroy_population(
	population_t * const population,
	bool destroy_genomes, bool deallocate_genomes_data,
	bool close_pool_file
) {

	destroy_genomes_vector(
		population->pool->organisms_number,
		deallocate_genomes_data, destroy_genomes,
		population->genomes);
	destroy_pool(population->pool, close_pool_file);
	free(population);

}
