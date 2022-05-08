#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <time.h>

#include "mutations.h"

int main(int argc, char *argv[]) {

    int size = 0;
    float probability = 0;
    char silent = 0;

    int opt;
    while ((opt = getopt(argc, argv, "s:p:mh")) != -1){
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
            case 'h':
                printf("usage: flip_bits_test s [int] -p [float] =p -h\n");
                printf("\t-s sets size in bytes\n");
                printf("\t-p sets probability of the flip\n");
                printf("\t-m do not print the bytes\n");
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

    clock_t start_time = clock();
    flip_bits_with_probability(bytes, size * 8, probability);
    double elapsed_time = (double)(clock() - start_time) / CLOCKS_PER_SEC;
    printf("\nbenchmark took %f sec.\n", elapsed_time);

    if (!silent) {
        for (int byte_i = 0; byte_i < size; byte_i++)
            for (int bit_i = 7; bit_i > 0; bit_i--)
                printf("%d", bytes[byte_i] & (1 << bit_i));
        printf("\n");
    }


    free(bytes);

	return 0;

}
