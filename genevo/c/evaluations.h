#pragma once

#include <stdint.h>

#include "memory.h"
#include "error.h"

#include "pool.h"
#include "pickler.h"
#include "rbtree.h"

typedef struct organism_io_vec_s {
	pool_neurons_num_t  size;
	double             *values;
} organism_io_vec_t;

organism_io_vec_t * const run_organism(
	const organism_io_vec_t * const,
	const gene_t * const * const, const genome_length_t,
	const pool_t * const);

organism_io_vec_t * allocate_organism_io_vec(pool_neurons_num_t);

void destroy_organism_io_vec(organism_io_vec_t *);
