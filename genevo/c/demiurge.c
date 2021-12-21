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

void generate_random_genome_data(
	genome_t *genome, uint64_t bits_number, uint8_t gene_byte_size
) {

	uint8_t bytes_number = bits_number / (gene_byte_size * 8);
	genome->residue_size_bits = bits_number - bytes_number * 8;

	fill_bytes_with_randomness(genome->genes, bytes_number);
	fill_bits_with_randomness(genome->residue, genome->residue_size_bits);

}

genome_t * create_random_genome(
	const uint8_t *metadata, uint16_t metadata_byte_size,
	uint64_t bits_number, uint8_t gene_byte_size
) {

	genome_t *genome = malloc(sizeof(genome_t));

	genome->length = bits_number / gene_byte_size;
	genome->metadata_byte_size = metadata_byte_size;
	genome->metadata = malloc(metadata_byte_size);

	memcpy(genome->metadata, metadata, metadata_byte_size);

	generate_random_genome_data(genome, bits_number, gene_byte_size);

	return genome;

}
