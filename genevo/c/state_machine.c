#include "state_machine.h"

state_machine_t * generate_state_machine(const uint32_t states_number) {

	DECLARE_MALLOC_OBJECT(state_machine_t, machine, RETURN_NULL_ON_ERR);

	machine->states_number = 0;
	machine->current_state = 0;
	machine->prev_state = 0;

	ASSIGN_MALLOC_LINKS_ARRAY(machine->transitions,     state_probability_t, states_number);
	ASSIGN_MALLOC_LINKS_ARRAY(machine->cdf_transitions, cdf_item_t,          states_number);

	if (
		machine->transitions == NULL ||
		machine->cdf_transitions == NULL
	) DESTROY_AND_EXIT(destroy_state_machine, machine, RETURN_NULL_ON_ERR);

	for (uint32_t state_i = 0; state_i < states_number; state_i++) {

		ASSIGN_CALLOC_ARRAY(
			machine->transitions[state_i], state_probability_t, states_number);
		ASSIGN_MALLOC_ARRAY(
			machine->cdf_transitions[state_i], cdf_item_t, states_number);

		if (
			machine->transitions[state_i] == NULL ||
			machine->cdf_transitions[state_i] == NULL
		) DESTROY_AND_EXIT(destroy_state_machine, machine, RETURN_NULL_ON_ERR);

		machine->states_number++;

	}

	return machine;

}

void destroy_state_machine(state_machine_t *machine) {

	// machine->states_number will always contain number of items that were
	// actually allocated
	for (uint32_t state_i = 0; state_i < machine->states_number; state_i++) {
		FREE_NOT_NULL(machine->transitions[state_i]);
		FREE_NOT_NULL(machine->cdf_transitions[state_i]);
	}

	FREE_NOT_NULL(machine->transitions);
	FREE_NOT_NULL(machine->cdf_transitions);
	FREE_NOT_NULL(machine);

}

int cdf_items_comparator(const void *item1_void, const void *item2_void) {

	const cdf_item_t* item1 = (cdf_item_t *) item1_void;
	const cdf_item_t* item2 = (cdf_item_t *) item2_void;

	if (item1->value < item2->value) return -1;
	if (item1->value > item2->value) return 1;
	else                             return 0;

}

void init_state_machine(state_machine_t *machine, const uint32_t initial_state) {

	#ifdef STATE_MACHINE_CHECK_DISTRIBUTION_ON_INIT
	for (uint32_t row_i = 0; row_i < machine->states_number; row_i++) {
		double row_sum = 0;
		for (uint32_t column_i = 0; column_i < machine->states_number; column_i++)
			row_sum += machine->transitions[row_i][column_i];
		if (!FLOAT_IS_NEAR(row_sum, 1)) {
			ERROR_LEVEL = ERR_SM_WRONG_DISTRIBUTION;
			return;
		}
	}
	#endif

	machine->current_state = initial_state;
	machine->prev_state = initial_state;

	uint32_t state_i, state_j;
	for (state_i = 0; state_i < machine->states_number; state_i++) {

		// fill probabilities
		for (state_j = 0; state_j < machine->states_number; state_j++) {
			machine->cdf_transitions[state_i][state_j].x = state_j;
			machine->cdf_transitions[state_i][state_j].value =
				machine->transitions[state_i][state_j];
		}

		// sort them
		qsort(
			machine->cdf_transitions[state_i], // what to sort
			machine->states_number,            // how many elements in the array
			sizeof(cdf_item_t),                // size of each of them
			cdf_items_comparator);

		// sum up values
		for (state_j = 1; state_j < machine->states_number; state_j++) {
			machine->cdf_transitions[state_i][state_j].value +=
				machine->cdf_transitions[state_i][state_j - 1].value;
		}

	}

}

/*

Set matrix of distribution by given diagonal and non-diagonal elements.
E.g. if diag_probs=A, non_diag_probs=B, then matrix would be
	[ A B B
	  B A B
	  B B A ]

*/
void state_machine_diag_distribution(
	state_machine_t *machine,
	state_probability_t diag_probs, state_probability_t non_diag_probs
) {

	#ifdef STATE_MACHINE_CHECK_DISTRIBUTION
	if (!FLOAT_IS_NEAR(
		diag_probs + (machine->states_number - 1) * non_diag_probs,
		1
	)) {
		ERROR_LEVEL = ERR_SM_WRONG_DISTRIBUTION;
		return;
	}
	#endif

	for (uint32_t i = 0; i < machine->states_number; i++)
		for (uint32_t j = 0; j < machine->states_number; j++)

			if (i == j)
				machine->transitions[i][j] = diag_probs;
			else
				machine->transitions[i][j] = non_diag_probs;

}

/*

Set each cell of the matrix to equal value.

*/
void state_machine_uniform_distribution(state_machine_t *machine) {

	for (uint32_t i = 0; i < machine->states_number; i++)
		for (uint32_t j = 0; j < machine->states_number; j++)
				machine->transitions[i][j] =
					(state_probability_t)1 /
					(state_probability_t)machine->states_number;

}

void machine_next_state(state_machine_t * const machine) {

	#if   STATE_MACHINE_RANDOMNESS_MODE == STATE_MACHINE_XORSHIFT_RANDOM
	state_probability_t random_value = next_double_urandom64_in_range(0, 1);
	#elif STATE_MACHINE_RANDOMNESS_MODE == STATE_MACHINE_FAST_RANDOM
	state_probability_t random_value = next_double_fast_random_in_range(0, 1);
	#elif STATE_MACHINE_RANDOMNESS_MODE == STATE_MACHINE_MERSENNE_RANDOM
	state_probability_t random_value = mersenne_genrand64_real2();
	#endif

	const uint32_t state_i = machine->current_state;
	for(uint32_t state_j = 0; state_j < machine->states_number; state_j++) {

		if (random_value <= machine->cdf_transitions[state_i][state_j].value) {
			machine->prev_state = machine->current_state;
			machine->current_state = machine->cdf_transitions[state_i][state_j].x;
			return;
		}

	}

}
