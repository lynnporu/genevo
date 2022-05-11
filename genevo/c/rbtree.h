#pragma once

/*

This module contains implementation of red black tree.

*/

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "memory.h"

typedef uint64_t rbtree_node_id_t;

enum rbtree_color_e {
	RBTREE_NODE_RED, RBTREE_NODE_BLACK
};

typedef struct rbtree_node_s {
	rbtree_node_id_t        id;
	const void             *stored_object;
	enum rbtree_color_e     color;
	struct rbtree_node_t   *parent;
	struct rbtree_node_t   *right;
	struct rbtree_node_t   *left;
} rbtree_node_t;

typedef struct rbtree_s {
	struct rbtree_node_t *head;
} rbtree_t;

rbtree_t * allocate_rbtree();

void destroy_rbtree(rbtree_t *);

rbtree_node_t *rbtree_insert(rbtree_t *, rbtree_node_id_t, void *object);
