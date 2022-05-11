#include "heap.h"

rbtree_t * allocate_rbtree() {

	DECLARE_MALLOC_OBJECT(rbtree_t, tree, RETURN_NULL_ON_ERR);
	tree->head = NULL;
	return tree;

};

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

void free_rbtree_node(rbtree_node_t *node) {

	FREE_NOT_NULL(node);

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

static inline bool rbtree_uncle_is_red(rbtree_node_t *z) {

	rbtree_node_t *y;

	y = z->parent->parent;
	y = z->parent == y->left
		? y->right
		: y->left;

	if (y && y->color == RBTREE_NODE_RED) {
		y->color = RBTREE_NODE_BLACK;
		z->parent->color = RBTREE_NODE_BLACK;
		z->parent->parent->color = RBTREE_NODE_RED;
		z = z->parent->parent;
		return true;
	}
	else
		return false;

}

static inline void rbtree_case_left(rbtree_node_t **head, rbtree_node_t *z) {

	enum rbtree_color_e tmp_color;

	if (z == z->parent->left) {
		tmp_color = z->parent->color;
		z->parent->color = z->parent->parent->color;
		z->parent->parent->color = tmp_color;
		rbtree_right_rotate(head, z->parent->parent);
	}
	else {
		tmp_color = z->color ;
		z->color = z->parent->parent->color;
		z->parent->parent->color = tmp_color;
		rbtree_left_rotate (head, z->parent);
		rbtree_right_rotate(head, z->parent->parent);
	}

}

static inline void	rbtree_case_right(rbtree_node_t **head, rbtree_node_t *z) {

	enum rbtree_color_e tmp_color;

	if (z == z->parent->right) {
		tmp_color = z->parent->color ;
		z->parent->color = z->parent->parent->color;
		z->parent->parent->color = tmp_color;
		rbtree_left_rotate(head, z->parent->parent);
	}
	else {
		tmp_color = z->color;
		z->color = z->parent->parent->color;
		z->parent->parent->color = tmp_color;
		rbtree_right_rotate(head, z->parent);
		rbtree_left_rotate (head, z->parent->parent);
	}
}

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

	(*head)->color = BLACK;

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
