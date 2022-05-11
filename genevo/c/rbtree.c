#include "rbtree.h"

rbtree_t * allocate_rbtree() {

	DECLARE_MALLOC_OBJECT(rbtree_t, tree, RETURN_NULL_ON_ERR);
	tree->head = NULL;
	return tree;

};

void free_rbtree_node(rbtree_node_t *node) {

	FREE_NOT_NULL(node);

}

void destroy_rbtree_traversal(rbtree_node_t *head) {

	rbtree_node_t *tmp = NULL;

	while (head->left || head->right)
		head = head->left ? head->left : head->right;

	tmp = head->parent;
	free_rbtree_node(head);

	if (!tmp) return;

	if (head == tmp->left) tmp->left  = NULL;
	else                   tmp->right = NULL;

	destroy_rbtree_traversal(tmp);

}

void destroy_rbtree(rbtree_t *tree) {
	destroy_rbtree_traversal(tree->head);
	FREE_NOT_NULL(tree);
}

rbtree_node_t *allocate_empty_rbtree_node() {

	DECLARE_MALLOC_OBJECT(rbtree_node_t, node, RETURN_NULL_ON_ERR);
	node->id = 0;
	node->stored_object = NULL;
	node->parent = NULL;
	node->right = NULL;
	node->left = NULL;
	return node;

}

static inline bool rbtree_uncle_is_red(rbtree_node_t *z) {

	rbtree_node_t *y;

	y = z->parent->parent;
	y = z->parent == y->left
		? y->right
		: y->left;

	if (y && y->color == RBTREE_NODE_RED) {
		z->parent->parent->color = RBTREE_NODE_RED;
		z        ->parent->color = RBTREE_NODE_BLACK;
		y                ->color = RBTREE_NODE_BLACK;
		z = z->parent->parent;
		return true;
	}
	else
		return false;

}


void rbtree_left_rotate(rbtree_node_t **head, rbtree_node_t *x) {

	rbtree_node_t *y;

	if (!x || !x->right) return;

	y = x->right;
	x->right = y->left;

	if (x->right != NULL) x->right->parent = x;

	y->parent = x->parent;

	if (x->parent == NULL)          (*head) = y;
	else if (x == x->parent->left)  x->parent->left = y;
	else                            x->parent->right = y;

	y->left = x;
	x->parent = y;

}

void rbtree_right_rotate(rbtree_node_t **head, rbtree_node_t *y) {

	rbtree_node_t *x;

	if (!y || !y->left) return;

	x = y->left;
	y->left = x->right;

	if (x->right != NULL) x->right->parent = y;

	x->parent =y->parent;

	if (x->parent == NULL)          (*head) = x;
	else if (y == y->parent->left)  y->parent->left = x;
	else                            y->parent->right = x;

	x->right = y;
	y->parent = x;

}

#define SWAP_COLORS(_C1, C2) {                                                 \
	enum rbtree_color_e tmp;                                                   \
	tmp = _C1; _C1 = C2; C2 = tmp;                                             \
}

static inline void rbtree_case_left(rbtree_node_t **head, rbtree_node_t *z) {

	if (z == z->parent->left) {
		SWAP_COLORS(z->parent->color, z->parent->parent->color);
		rbtree_right_rotate(head, z->parent->parent);
	}
	else {
		SWAP_COLORS(z->color, z->parent->parent->color);
		rbtree_left_rotate (head, z->parent);
		rbtree_right_rotate(head, z->parent->parent);
	}

}

static inline void rbtree_case_right(rbtree_node_t **head, rbtree_node_t *z) {

	if (z == z->parent->right) {
		SWAP_COLORS(z->parent->color, z->parent->parent->color);
		rbtree_left_rotate(head, z->parent->parent);
	}
	else {
		SWAP_COLORS(z->color, z->parent->parent->color);
		rbtree_right_rotate(head, z->parent);
		rbtree_left_rotate (head, z->parent->parent);
	}
}

#undef SWAP_COLORS

void rbtree_fixup(rbtree_node_t **head, rbtree_node_t *z) {

	while (
		   z         != *head
		&& z->parent != *head
		&& z->parent->color == RBTREE_NODE_RED
	) {

		if (rbtree_uncle_is_red(z))
			continue;
		else if (z->parent == z->parent->parent->left)
			rbtree_case_left(head, z);
		else if (z->parent == z->parent->parent->right)
			rbtree_case_right(head, z);

	}

	(*head)->color = RBTREE_NODE_BLACK;

}

void rbtree_bst_insert(rbtree_node_t **head, rbtree_node_t *node) {

	rbtree_node_t *y = NULL,
	              *x = *head;

	while (x) {
		y = x;
		x = (node->id < x->id)
			? x->left
			: x->right;
	}

	node->parent = y;

	if (node->id > y->id) y->right = node;
	else                  y->left  = node;

	node->color = RBTREE_NODE_RED;

	rbtree_fixup(head, node);

}

rbtree_node_t *rbtree_insert(
	rbtree_t *tree, rbtree_node_id_t id, void *object
) {

	rbtree_node_t *node = allocate_empty_rbtree_node();
	node->stored_object = object;
	node->id = id;

	if (tree->head == NULL) {
		node->color = RBTREE_NODE_BLACK;
		tree->head = node;
	}

	else rbtree_bst_insert(&(tree->head), node);

	return node;

}

struct rbtree_node_s* rbtree_find_in_subtree(
	rbtree_node_id_t id, struct rbtree_node_s* subtree
) {

	    if (id == subtree->id)
	    	return subtree;
	else
		if (!(subtree->left) && !(subtree->right))
			return NULL;
	else
		if (id  < subtree->id)
			return rbtree_find_in_subtree(id, subtree->left);
	else
		if (id  > subtree->id)
			return rbtree_find_in_subtree(id, subtree->right);

	return NULL;
}

struct rbtree_node_s *rbtree_find(rbtree_t *tree, rbtree_node_id_t id) {
	return rbtree_find_in_subtree(id, tree->head);
}
