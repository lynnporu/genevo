/*

The gene itself us just a bunch of bytes. We can use it to store any number of
numbers.

This header contains declartions for custom gene structure.

 */

#pragma once

#include <cstdint>

typedef std::uint8_t gene_number_width_t;

struct gene_structure_s {
	const gene_number_width_t bit_width;
	const bool                is_signed;
	const bool                is_denormalized;
};

typedef std::uint8_t  gene_capacity_t;
typedef std::uint32_t gene_structure_id_t;

template <gene_capacity_t N>
class Gene {

public:
	gene_structure_id_t id;
	struct gene_structure_s numbers[N];

};
