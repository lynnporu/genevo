#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <time.h>

#include "mutations.h"
#include "rand.h"

int main(int argc, char *argv[]) {

    int size = 0;
    float probability = 0;
    char silent = 0;
    int output_width = 10;

    int opt;
    while ((opt = getopt(argc, argv, "s:p:o:mh")) != -1){
        switch (opt) {
            case 's':
                size = atoi(optarg);
                break;
            case 'p':
                probability = atof(optarg);
                break;
            case 'm':
                silent = 1;
                break;
            case 'o':
                output_width = atoi(optarg);
                break;
            case 'h':
                printf("usage: flip_bits_test s [int] -p [float] - p[int] -p -h\n");
                printf("\t-s sets size in bytes\n");
                printf("\t-p sets probability of the flip\n");
                printf("\t-m do not print the bytes\n");
                printf("\t-o sets output width\n");
        }
    }

    if (size < 1) {
        printf("-s should be bigger than 0\n");
        return 0;
    }

    if (probability < 0 || probability > 1) {
        printf("-p should be in ranges [0; 1]\n");
        return 0;
    }

    printf("bytes size:\t\t%d\n", size);
    printf("flip probability:\t%.10f\n", probability);

    unsigned char* const bytes = calloc(size, sizeof(char));

    #if   MUTATIONS_RANDOMNESS_MODE == MUTATIONS_XORSHIFT_FOR_RANDOM64
        ENSURE_XORSHIFT128P_RND_SEED_IS_SET;
    #elif MUTATIONS_RANDOMNESS_MODE == MUTATIONS_MERSENNE_FOR_RANDOM64
        ENSURE_MERSENNE_RND_SEED_IS_SET;
    #endif

    clock_t start_time = clock();
    flip_bits_with_probability(bytes, size * 8, probability);
    double elapsed_time = (double)(clock() - start_time) / CLOCKS_PER_SEC;
    printf("\nbenchmark took %f sec.\n", elapsed_time);

    int ones = 0;

    for (int byte_i = 0; byte_i < size; byte_i++) {
        if (byte_i % output_width == 0)
            if (!silent) printf("\n%d\t:", byte_i);
        for (int bit_i = 7; bit_i > 0; bit_i--) {
            int bit = bytes[byte_i] & (1 << bit_i) ? 1 : 0;
            ones += bit;
            if (!silent) printf("%d", bit);
        }
        if (!silent) printf(" | ");
    }
    if (!silent) printf("\n");

    printf(
        "%d ones out of %d bits, ratio: %.5f\n",
        ones, size * 8, (float)ones / (float)(size * 8));


    free(bytes);

	return 0;

}
