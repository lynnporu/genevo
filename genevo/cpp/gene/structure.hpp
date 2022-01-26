#pragma once

#include <cstdint>

typedef std::uint8_t  gene_structure_class_t;
typedef std::uint32_t gene_structure_id_t;

#define GENE_STRUCTURE_ID_INSTANCE_WIDTH \
    (( sizeof(gene_structure_id_t) - sizeof(gene_structure_class_t) ) * 8)

#define GENE_STRUCTURE_ID(_CLASS_ID, _INSTANCE_ID)                             \
    (   (                                                                      \
            (gene_structure_class_t)(_CLASS_ID) <<                             \
            (GENE_STRUCTURE_ID_INSTANCE_WIDTH)                                 \
        ) | (_INSTANCE_ID)                                                     \
    )

typedef std::uint8_t gene_number_width_t;

struct GeneNumber {
	gene_number_width_t bit_width;
	bool                is_signed;
	bool                is_denormalized;
};

typedef std::uint8_t gene_capacity_t;

struct GeneStructure {
	gene_structure_id_t id;
	gene_capacity_t     capacity;	
};

template<gene_capacity_t structure_size = 3>
struct DefinedGeneStructure: public GeneStructure {
	GeneNumber          numbers[structure_size];
};

#define UNSIGNED_NORMALIZED(_BIT_WIDTH) \
    { .bit_width = _BIT_WIDTH, .is_signed = false, .is_denormalized = false }

#define SIGNED_DENORMALIZED(_BIT_WIDTH) \
    { .bit_width = _BIT_WIDTH, .is_signed = true, .is_denormalized = true }

