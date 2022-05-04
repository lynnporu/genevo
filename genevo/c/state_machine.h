#pragma once
/*

This header implements state machine.

*/

#include <stdint.h>
#include <stdlib.h>

#include "error.h"
#include "rand.h"
#include "memory.h"
#include "bit_manipulations.h"

#define STATE_MACHINE_FAST_RANDOM      0
#define STATE_MACHINE_XORSHIFT_RANDOM  1
#define STATE_MACHINE_RANDOMNESS_MODE MACHINE_XORSHIFT_RANDOM

typedef double state_probability_t;

/* @struct cdf_item
 * @member uint32 x
 * @member double value
 */
// cummulative distribution function
typedef struct cdf_item_s {
    uint32_t            x;
    state_probability_t value;
} cdf_item_t;

/* @struct state_machine
 * @member double** transitions
 * @member double** cdf_transitions
 * @member uint32 states_number
 * @member uint32 current_state
 */
typedef struct state_machine_s {
    // upper triangle 2d matrix
    state_probability_t **transitions;
    cdf_item_t          **cdf_transitions;
    uint32_t              states_number;
    uint32_t              current_state;
    uint32_t              prev_state;
} state_machine_t;

/* @function generate_state_machine
 * @return state_machine*
 * @argument uint32
 */
state_machine_t * generate_state_machine(const uint32_t states_number);

/* @function destroy_state_machine
 * @return void
 * @argument state_machine*
 */
void destroy_state_machine(state_machine_t *machine);

/* @function init_state_machine
 * @return void
 * @argument state_machine*
 * @argument uint32
 */
void init_state_machine(state_machine_t *machine, const uint32_t initial_state);

/* @function generate_state_machine
 * @return void
 * @argument state_machine*
 */
void machine_next_state(state_machine_t *machine);
