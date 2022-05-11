#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <time.h>

#include "rbtree.h"

void rbtree_print(rbtree_node_t *root)
{
    if (root == NULL) {
        printf("<NIL> ");
        return;
    }
    rbtree_print(root->left);
    printf("%ld ", root->id);
    rbtree_print(root->right);
}


int main(int argc, char *argv[]) {

    int nodes_number = 0;
    int indexing_number = 0;
    char silent = 0;

    clock_t start_time;
    double elapsed_time;

    int opt;
    while ((opt = getopt(argc, argv, "s:i:lh")) != -1){
        switch (opt) {
            case 's':
                nodes_number = atoi(optarg);
                break;
            case 'i':
                indexing_number = atoi(optarg);
                break;
            case 'l':
                silent = 1;
                break;
            case 'h':
                printf("usage: rbtree_test -s [int] -i [inet] -l -h\n");
                printf("\t-s sets number of generated nodes\n");
                printf("\t-i sets number indexing\n");
                printf("\t-l do not print result tree\n");
        }
    }

    if (nodes_number < 1) {
        printf("-s should be > 0\n");
        return 1;
    }

    if (indexing_number < 0) {
        printf("-i should be > 0\n");
        return 1;
    }

    rbtree_t *tree = allocate_rbtree();

    start_time = clock();
    for (unsigned int i = nodes_number; i > 0; i--)
        rbtree_insert(tree, i, NULL);
    elapsed_time = (double)(clock() - start_time) / CLOCKS_PER_SEC;
    printf(
        "stuffing with %d numbers took %.10f sec.\n",
        nodes_number, elapsed_time);

    printf("root node is %lu\n", tree->head->id);

    if (!silent) {
        printf("Tree traversal:\n");
        rbtree_print(tree->head);
        printf("\n");
    }

    volatile struct rbtree_node_s *found;
    srand(time(NULL));
    start_time = clock();
    for (int i = 0; i < indexing_number; ++i)
        found = rbtree_find(tree, (unsigned int)(rand() % nodes_number));
    elapsed_time = (double)(clock() - start_time) / CLOCKS_PER_SEC;
    printf(
        "tree was indexed %d times, it took %.10f sec.\n",
        indexing_number, elapsed_time);


    start_time = clock();
    destroy_rbtree(tree);
    elapsed_time = (double)(clock() - start_time) / CLOCKS_PER_SEC;
    printf("destroying took %.10f sec.\n", elapsed_time);

	return 0;

}
