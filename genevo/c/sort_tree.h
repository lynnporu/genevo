#pragma once

/*

This module contains implementation of red black tree.

*/

#include <stdint.h>

#include "memory.h"

typedef uint64_t sort_tree_node_id_t;

enum sort_tree_node_color {
	SORT_TREE_NODE_RED, SORT_TREE_NODE_BLACK
};

typedef struct sort_tree_node_s {
	sort_tree_node_id_t       id;
	void                     *stored_object;
	enum sort_tree_node_color color;
	struct sort_tree_node_t  *parent;
	struct sort_tree_node_t  *right;
	struct sort_tree_node_t  *left;
} sort_tree_node_t;

typedef struct sort_tree_s {
	struct sort_tree_node_t *head;
} sort_tree_t;

sort_tree_t * allocate_sort_tree();

void destroy_sort_tree(sort_tree_t *);

sort_tree_node_t *sort_tree_insert(
	sort_tree_node_id_t id, const void *);
