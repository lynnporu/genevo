#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <time.h>

#include "state_machine.h"

int main(int argc, char *argv[]) {

	float blend_coefficient = 0;
    int states_number = 0;
    int iterations_number = 0;
    int first_state = 0;
    int output_width = 10;
    char silent = 0;

    int opt;
    while ((opt = getopt(argc, argv, "b:s:i:f:o:lh")) != -1){
        switch (opt) {
            case 'b':
                blend_coefficient = atof(optarg);
                break;
            case 's':
                states_number = atoi(optarg);
                break;
            case 'i':
                iterations_number = atoi(optarg);
                break;
            case 'f':
                first_state = atoi(optarg);
                break;
            case 'o':
                output_width = atoi(optarg);
                break;
            case 'l':
                silent = 1;
                break;
            case 'h':
                printf("usage: diag_state_machine_test -b [float] -s [int] -i [int] -f [int] -o [int] -l -h\n");
                printf("\t-b sets blend coefficient\n");
                printf("\t-s sets number of states\n");
                printf("\t-i sets iterations number\n");
                printf("\t-f sets initial state\n");
                printf("\t-s turn on silent states generations\n");
                printf("\t-o sets output width\n");
        }
    }

    if (blend_coefficient <= 0 || blend_coefficient >= 1) {
        printf("'-b' should be in range (0; 1)\n");
        return 0;
    }

    if (states_number <= 0) {
        printf("-s should be bigger than 0\n");
        return 0;
    }

    if (first_state < 0 || first_state >= states_number) {
        printf("-f should be in range [0; states number)\n");
        return 0;
    }

    if (output_width < 0) {
        printf("-o bigger than 0\n");
        return 0;
    }

    printf("blend coefficient:\t%f\n", blend_coefficient);
    printf("states number:\t\t%d\n", states_number);
    printf("iterations number:\t%d\n", iterations_number);
    printf("initial state:\t\t%d\n", first_state);

    state_machine_t *blender = generate_state_machine(states_number);
    state_machine_diag_distribution(
        blender,
        1 - blend_coefficient,
        blend_coefficient / (states_number - 1));
    init_state_machine(blender, first_state);

    printf("\ndistribution matrix: [\n");
    for (int i = 0; i < states_number; i++) {
        printf("\t");
        for (int j = 0; j < states_number; j++) {
            printf("%.5f ; ", blender->transitions[i][j]);
        }
        printf("\n");
    }
    printf("]\n");

    printf("\ncummulative distribution matrix: [\n");
    for (int i = 0; i < states_number; i++) {
        printf("\t");
        for (int j = 0; j < states_number; j++) {
            printf(
                "{%d: %.5f} ; ",
                blender->cdf_transitions[i][j].x,
                blender->cdf_transitions[i][j].value
            );
        }
        printf("\n");
    }
    printf("]\n");

    ENSURE_MERSENNE_RND_SEED_IS_SET;
    ENSURE_XORSHIFT128P_RND_SEED_IS_SET;

    if (!silent) {
        int iterations_counter = 0;
        do {
            if (iterations_counter % output_width == 0)
                printf("\n%d\t: ", iterations_counter);
            printf("%d ", blender->current_state);
            machine_next_state(blender);
        } while (++iterations_counter < iterations_number);

        printf("\n");
    }

    clock_t start_time = clock();
    for (int i = 0; i < iterations_number; i++) {
        machine_next_state(blender);
    }
    double elapsed_time = (double)(clock() - start_time) / CLOCKS_PER_SEC;

    printf("\nbenchmark on %d times took %f sec.\n", iterations_number, elapsed_time);

    destroy_state_machine(blender);

	return 0;

}
