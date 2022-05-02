#include "state_machine.h"

state_machine_t * generate_state_machine(const uint32_t states_number) {

	state_machine_t *machine = malloc(sizeof(state_machine_t));
	machine->transitions = malloc(sizeof(state_probability_t *) * states_number);
	machine->cdf_transitions = malloc(sizeof(cdf_item_t *) * states_number);


	for (uint32_t state_i = 0; state_i < machine->states_number; state_i++) {

		machine->transitions[state_i] = calloc(
			states_number, sizeof(state_probability_t));
		machine->cdf_transitions[state_i] = malloc(sizeof(cdf_item_t) * states_number);

	}

	return machine;

}

void destroy_state_machine(state_machine_t *machine) {

	for (uint32_t state_i = 0; state_i < machine->states_number; state_i++) {
		free(machine->transitions[state_i]);
		free(machine->cdf_transitions[state_i]);
	}

	free(machine->transitions);
	free(machine->cdf_transitions);
	free(machine);

}

int cdf_items_comparator(const void *item1_void, const void *item2_void) {

	const cdf_item_t* item1 = (cdf_item_t *) item1_void;
	const cdf_item_t* item2 = (cdf_item_t *) item2_void;

	if (item1->value < item2->value) return -1;
	if (item1->value > item2->value) return 1;
	else                             return 0;

}

void init_state_machine(state_machine_t *machine, const uint32_t initial_state) {

	machine->current_state = initial_state;

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
		for (state_j = 1; state_j < machine->states_number; state_j++) {\
			machine->cdf_transitions[state_i][state_j].value +=
				machine->transitions[state_i][state_j - 1];
		}

	}

}

void machine_next_state(state_machine_t *machine) {

	#if   STATE_MACHINE_RANDOMNESS_MODE == STATE_MACHINE_XORSHIFT_RANDOM
	state_probability_t random_value = next_urandom64() / MAX_FOR_64;
	#elif STATE_MACHINE_RANDOMNESS_MODE == STATE_MACHINE_FAST_RANDOM
	state_probability_t random_value = next_fast_random() / MAX_FOR_32;
	#endif

	const uint32_t state_i = machine->current_state;
	for(uint32_t state_j = 0; state_j < machine->states_number; state_j++) {
		if (random_value <= machine->cdf_transitions[state_i][state_j].value) {
			machine->current_state = machine->cdf_transitions[state_i][state_j].x;
			return;
		}
	}

}
