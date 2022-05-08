#include "demiurge.h"

/*

For this function genome->length and genome->residue_size_bits should be set.

 */
void generate_genome_data(
	genome_t * const genome, const pool_gene_byte_size_t gene_bytes_size,
	const generator_mode_t generator_mode
) {

	if (generator_mode == GENERATE_RANDOMNESS) {
		fill_bytes_with_randomness(genome->genes, genome->length * gene_bytes_size);
		fill_bits_with_randomness(genome->residue, genome->residue_size_bits);
	}
	else
	if (generator_mode == GENERATE_ZEROS) {
		memset(genome->genes, 0, genome->length * gene_bytes_size);
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
	const bool allocate_data,
	const genome_length_t length, const pool_gene_byte_size_t gene_bytes_size,
	const uint32_t genome_bit_size
) {

	DECLARE_MALLOC_OBJECT(genome_t, genome, RETURN_NULL_ON_ERR);

	const uint64_t genome_byte_size = gene_bytes_size * length;
	const uint16_t residue_size_bits =
		genome_bit_size - BYTES_TO_BITS(genome_byte_size);

	if (allocate_data) {

		genome->genes = malloc(genome_byte_size);
		genome->residue = malloc(BITS_TO_BYTES(residue_size_bits));

		if (genome->genes == NULL || genome->residue == NULL)
			RAISE_MALLOC_ERR(RETURN_NULL_ON_ERR);

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
	const pool_organisms_num_t size, const bool allocate_data,
	const genome_length_t genes_number,
	const pool_gene_byte_size_t gene_bytes_size,
	const uint32_t genome_bit_size
) {

	DECLARE_MALLOC_LINKS_ARRAY(genome_t, genomes, size, RETURN_NULL_ON_ERR);

	for(pool_organisms_num_t genome_itr = 0; genome_itr < size; genome_itr++)
		genomes[genome_itr] = allocate_genome(
			allocate_data, genes_number, gene_bytes_size, genome_bit_size);

	return genomes;

}

genome_t ** duplicate_genome_vector (
	const pool_organisms_num_t size, duplicating_mode_t mode,
	const genome_length_t genes_number,
	const pool_gene_byte_size_t gene_bytes_size,
	const uint32_t genome_bit_size,
	const genome_t * const * const src
) {

	genome_t ** dst = allocate_genome_vector(
		size,
		mode == DUPLICATION_COPY_DATA,
		genes_number, gene_bytes_size, genome_bit_size);

	for (pool_organisms_num_t i = 0; i < size; i++) {
		copy_genome(src[i], dst[i], mode, gene_bytes_size);
	}

	return dst;

}

void copy_genome_vector (
	const pool_organisms_num_t size,
	const genome_t * const * const src,
	genome_t * const * const dst,
	const pool_gene_byte_size_t gene_bytes_size,
	duplicating_mode_t mode
) {

	for (pool_organisms_num_t i = 0; i < size; i++) {
		copy_genome(src[i], dst[i], mode, gene_bytes_size);
	}

}

void copy_genome(
	const genome_t * const src, genome_t * const dst,
	duplicating_mode_t mode,
	pool_gene_byte_size_t gene_bytes_size
) {
	dst->length = src->length;
	dst->metadata_byte_size = src->metadata_byte_size;
	dst->residue_size_bits = src->residue_size_bits;

	switch (mode) {

		case DUPLICATION_COPY_LINKS:
			dst->metadata = src->metadata;
			dst->genes = src->genes;
			dst->residue = src->residue;
			break;

		case DUPLICATION_COPY_DATA:
			memcpy(dst->metadata, src->metadata, dst->metadata_byte_size);
			memcpy(dst->genes, src->genes, dst->length * gene_bytes_size);
			memcpy(dst->residue, src->residue, BYTES_TO_BITS(dst->residue_size_bits));
			break;

		default:
		case DUPLICATION_LEAVE_NULL:
			dst->metadata = NULL;
			dst->genes = NULL;
			dst->residue = NULL;
			break;


	}

}

void destroy_genomes_vector(
	const pool_organisms_num_t size,
	const bool deallocate_data, const bool destroy_each_genome,
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

void destroy_genome(genome_t * const genome, const bool deallocate_data) {

	if (deallocate_data) {
		FREE_NOT_NULL(genome->genes);
		FREE_NOT_NULL(genome->residue);
	}

	delete_genome_metadata(genome);

	free(genome);

}

void assign_genome_metadata(
	genome_t * const genome, const genome_metadata_size_t metadata_byte_size,
	const char *metadata
) {

	delete_genome_metadata(genome);
	genome->metadata = malloc(metadata_byte_size);
	if (genome->metadata == NULL) RAISE_MALLOC_ERR(RETURN_VOID_ON_ERR);
	memcpy(genome->metadata, metadata, metadata_byte_size);

}

void delete_genome_metadata(genome_t * const genome) {

	FREE_NOT_NULL(genome->metadata);

}

pool_t * allocate_pool() {

	DECLARE_CONST_MALLOC_OBJECT(pool_t, pool, RETURN_NULL_ON_ERR);

	pool->metadata = NULL;
	pool->metadata_byte_size = 0;

	return pool;

}

void destroy_pool(pool_t * const pool, const bool close_file) {

	if (close_file) close_file_for_pool(pool);
	delete_pool_metadata(pool);

	free(pool);

}

void assign_pool_metadata(
	pool_t * const pool, const pool_metadata_size_t metadata_byte_size,
	const char *metadata
) {

	delete_pool_metadata(pool);
	pool->metadata = malloc(metadata_byte_size);
	if (pool->metadata == NULL) RAISE_MALLOC_ERR(RETURN_VOID_ON_ERR);
	memcpy(pool->metadata, metadata, metadata_byte_size);

}

void delete_pool_metadata(pool_t * const pool) {

	FREE_NOT_NULL(pool->metadata);

}


/*

Create name for a file in which the pool will be allocated. New name will be
created based on current timestamp and a pointer to the pool.
! This function allocates new memory, which should be freed after use.

 */
char * alloc_name_for_pool(pool_t * const pool) {

	const uint64_t number = time(NULL) + (uint64_t)pool;
	// maximum size of uint64 in hex is 9 symbols + ".pool"
	DECLARE_CONST_CALLOC_ARRAY(char, address, 9 + 5, RETURN_NULL_ON_ERR);

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
	const generator_mode_t generator_mode
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

	if (ERROR_LEVEL != ERR_OK) return;

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
	const pool_organisms_num_t organisms_number,
	const pool_gene_node_id_part_t node_id_bit_size,
	const pool_gene_weight_part_t weight_bit_size,
	const pool_neurons_num_t input_neurons_number,
	const pool_neurons_num_t output_neurons_number,
	const uint64_t genome_bit_size,
	const generator_mode_t generator_mode
) {

	pool_t * const pool = allocate_pool();

	pool->organisms_number = organisms_number;
	pool->input_neurons_number = input_neurons_number;
	pool->output_neurons_number = output_neurons_number;
	pool->node_id_part_bit_size = node_id_bit_size;
	pool->weight_part_bit_size = weight_bit_size;

	const uint8_t gene_bit_size =
		pool->node_id_part_bit_size * 2 + pool->weight_part_bit_size;

	if (gene_bit_size % 8 /*!= 0*/) {
		ERROR_LEVEL = ERR_GENE_NOT_ALIGNED;
		destroy_pool(pool, false /*close file*/);
		return NULL;
	}

	pool->gene_bytes_size = BITS_TO_BYTES(gene_bit_size);

	const genome_length_t genes_number = genome_bit_size / gene_bit_size;

	genome_t ** const genomes = allocate_genome_vector(
		organisms_number, false /* allocate data  */,
		genes_number, pool->gene_bytes_size, genome_bit_size
	);

	DECLARE_CONST_MALLOC_OBJECT(population_t, population, RETURN_NULL_ON_ERR);

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
	const bool destroy_genomes, const bool deallocate_genomes_data,
	const bool close_pool_file
) {

	destroy_genomes_vector(
		population->pool->organisms_number,
		deallocate_genomes_data, destroy_genomes,
		population->genomes);
	destroy_pool(population->pool, close_pool_file);
	free(population);

}

/*

Dellocate genomes vector and population struct, but don't any of data.

*/
void destroy_population_object(population_t * const population) {
	free(population->genomes);
	free(population);
}
