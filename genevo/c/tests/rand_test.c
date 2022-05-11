#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "rand.h"

int main(int argc, char *argv[]) {

    int iterations_number = 0;
    int upper = 10, lower = 0;

    int opt;
    while ((opt = getopt(argc, argv, "i:ha:b:")) != -1){
        switch (opt) {

            case 'i':
                iterations_number = atoi(optarg);
                break;
            case 'a':
                lower = atoi(optarg);
                break;
            case 'b':
                upper = atoi(optarg);
                break;
            case 'h':
                printf("usage: rand_test -i [int] -a [int] -b [int]\n");
                printf("\t-i sets iterations number\n");
                printf("\t-a and -b sets ranges\n");
        }
    }

    ENSURE_XORSHIFT128P_RND_SEED_IS_SET;
    ENSURE_LCG_RND_SEED_IS_SET;
    ENSURE_MERSENNE_RND_SEED_IS_SET;

    for (int i = 0; i < iterations_number; ++i){
        double r = 0xffffffffffff;
        printf("xorshift       : %lu\n",   next_urandom64_in_range(lower, upper));
        printf("xorshift double: %.10f\n", next_double_urandom64_in_range(lower, upper));
        printf("lcg            : %d\n",    next_fast_random_in_range(lower, upper));
        printf("lcg double     : %.10f\n", next_double_fast_random_in_range(lower, upper));
        printf("mersenne       : %lu\n",   next_mersenne_random64_in_range(lower, upper));
        printf("mersenne double: %.10f\n", next_double_mersenne_random64_in_range(lower, upper));
        printf("\n");
    }

	return 0;

}
