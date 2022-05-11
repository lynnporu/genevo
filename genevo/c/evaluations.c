#include "evaluations.h"

organism_io_vec_t * const run_organism(
	const organism_io_vec_t * const input,
	const gene_t * const * const genes, const genome_length_t genes_num,
	const pool_t * const pool
) {

	organism_io_vec_t *output =
		allocate_organism_io_vec(pool->output_neurons_number);

	rbtree_t *nodes_heap = allocate_rbtree();

	for (genome_length_t gene_i = 0; gene_i < genes_num; gene_i++) {
		rbtree_insert(nodes_heap, gene_i, (void *)(genes[gene_i]));
		SWALLOW_ERR(ERR_INSERT_RBTREE_DUPLICATE);
	}

	return output;

}

organism_io_vec_t * allocate_organism_io_vec(pool_neurons_num_t size) {

	DECLARE_MALLOC_OBJECT(organism_io_vec_t, vec, RETURN_NULL_ON_ERR);

	ASSIGN_MALLOC_ARRAY(
		vec->values, double, size,
		DESTROY_AND_EXIT(destroy_organism_io_vec, vec, RETURN_NULL_ON_ERR));

	vec->size = size;

	return vec;

}

void destroy_organism_io_vec(organism_io_vec_t *vec) {

	FREE_NOT_NULL(vec->values);
	FREE_NOT_NULL(vec);

};
