/* @file: rbtree.c
 * #desc:
 *    The implementations of Red-Black self-balancing binary tree.
 *
 * #copy:
 *    Copyright (C) 1970 Public Free Software
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License as published by the Free Software Foundation; either
 *    version 2.1 of the License, or (at your option) any later version.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not,
 *    see <https://www.gnu.org/licenses/>.
 */

#include <demoz/config.h>
#include <demoz/c/stddef.h>
#include <demoz/c/stdint.h>
#include <demoz/ds/rbtree.h>


/* @func: _rb_change_child (static)
 * #desc:
 *    change the child of the parent node.
 *
 * #1: root node
 * #2: parent node
 * #3: old node
 * #4: new node
 */
static void _rb_change_child(struct rb_root *root, struct rb_node *parent,
		struct rb_node *old, struct rb_node *new)
{
	if (!parent) {
		root->node = new;
	} else if (parent->left == old) {
		parent->left = new;
	} else {
		parent->right = new;
	}
}

/* @func: _rb_left_rotate (static)
 * #desc:
 *    rb-tree left rotate.
 *
 *        g           g
 *       / \         / \
 *      p   u  -->  n   u
 *     / \         / \
 *    s   n       p   r
 *       / \     / \
 *      l   r   s   l
 *
 * #1: root node
 * #2: parent of the rotate node
 */
static void _rb_left_rotate(struct rb_root *root, struct rb_node *parent)
{
	struct rb_node *gparent, *node, *child;
	gparent = parent->parent;
	node = parent->right;
	child = node->left;

	/* 'left node' becomes 'parent right' */
	parent->right = child;
	if (child)
		child->parent = parent;

	/* 'parent' becomes 'left node' */
	node->left = parent;
	parent->parent = node;

	/* 'node' becomes 'gparent child' */
	_rb_change_child(root, gparent, parent, node);
	node->parent = gparent;
}

/* @func: _rb_right_rotate (static)
 * #desc:
 *    rb-tree right rotate.
 *
 *          g         g
 *         / \       / \
 *        p   u --> n   u
 *       / \       / \
 *      n   s     l   p
 *     / \           / \
 *    l   r         r   s
 *
 * #1: root node
 * #2: parent of the rotate node
 */
static void _rb_right_rotate(struct rb_root *root, struct rb_node *parent)
{
	struct rb_node *gparent, *node, *child;
	gparent = parent->parent;
	node = parent->left;
	child = node->right;

	/* 'right node' becomes 'parent left' */
	parent->left = child;
	if (child)
		child->parent = parent;

	/* 'parent' becomes 'right node' */
	node->right = parent;
	parent->parent = node;

	/* 'node' becomes 'gparent child' */
	_rb_change_child(root, gparent, parent, node);
	node->parent = gparent;
}

/* @func: rb_insert_fix
 * #desc:
 *    fix the inserted node and balance rb-tree.
 *
 * #1: root node
 * #2: node to be fix
 */
void F_SYMBOL(rb_insert_fix)(struct rb_root *root, struct rb_node *node)
{
	struct rb_node *parent, *gparent, *uncle;

	while (1) {
		parent = node->parent;
		if (!parent) {
			node->color = RB_BLACK;
			break;
		}
		if (parent->color == RB_BLACK)
			break;
		/* gparent and parent are Black and Red */

		gparent = parent->parent;
		uncle = gparent->right;
		if (parent != uncle) { /* parent is left */
			/* uncle is Red
			 *     G         g
			 *    / \       / \
			 *   p   u --> P   U
			 *  /         /
			 * n         n
			 */
			if (uncle && uncle->color == RB_RED) {
				parent->color = RB_BLACK;
				uncle->color = RB_BLACK;
				gparent->color = RB_RED;
				node = gparent;
				continue;
			}

			/* LR
			 *   G         G
			 *  / \       / \
			 * p   U --> p   U
			 *  \       /
			 *   n     n
			 */
			if (node == parent->right) {
				_rb_left_rotate(root, parent);
				parent = node;
			}

			/* LL
			 *     G         P
			 *    / \       / \
			 *   p   U --> n   g
			 *  /               \
			 * n                 U
			 */
			_rb_right_rotate(root, gparent);
			parent->color = RB_BLACK;
			gparent->color = RB_RED;

			break;
		} else { /* parent is right */
			uncle = gparent->left;
			/* uncle is Red
			 *     G         g
			 *    / \       / \
			 *   u   p --> U   P
			 *        \         \
			 *         n         n
			 */
			if (uncle && uncle->color == RB_RED) {
				parent->color = RB_BLACK;
				uncle->color = RB_BLACK;
				gparent->color = RB_RED;
				node = gparent;
				continue;
			}

			/* RL
			 *   G         G
			 *  / \       / \
			 * U   p --> U   p
			 *    /           \
			 *   n             n
			 */
			if (node == parent->left) {
				_rb_right_rotate(root, parent);
				parent = node;
			}

			/* RR
			 *   G          P
			 *  / \        / \
			 * U   p -->  g   n
			 *      \    /
			 *       n  U
			 */
			_rb_left_rotate(root, gparent);
			parent->color = RB_BLACK;
			gparent->color = RB_RED;

			break;
		}
	}
}

/* @func: _rb_erase (static)
 * #desc:
 *    erase the node of the rb-tree.
 *
 * #1: root node
 * #2: erase node
 * #r: returns the parent node or NULL
 */
static struct rb_node *_rb_erase(struct rb_root *root, struct rb_node *node)
{
	struct rb_node *parent = node->parent, *left = node->left,
		*right = node->right, *successor, *child;

	/*
	 *     (p)          (p)
	 *     / \          /  \
	 *   (n)  (s) --> (r)  (s)
	 *   / \
	 * nil  (r)
	 *
	 *     (p)           (p)
	 *     / \          /  \
	 *   (n)  (s) --> (r)  (s)
	 *   / \
	 * nil  nil
	 */
	if (!left) { /* no left child */
		_rb_change_child(root, parent, node, right);
		if (right) { /* only right child */
			/* 'node right' becomes 'node' */
			right->parent = parent;
			right->color = node->color;
			return NULL;
		}
		/* no left and right child */

		return (node->color == RB_BLACK) ? parent : NULL;
	}
	/*
	 *     (p)          (p)
	 *     / \          / \
	 *   (n)  (s) --> (l)  (s)
	 *   / \
	 * (l)  nil
	 */
	if (!right) { /* only left child */
		/* 'node left' becomes 'node' */
		_rb_change_child(root, parent, node, left);
		left->parent = parent;
		left->color = node->color;
		return NULL;
	}

	/*
	 *    (n)           (r)         (r)
	 *    / \           / \         / \
	 * (l)  (r)  --> (l)  (n) --> (l)  (r2)
	 *      /  \          / \
	 *   nil  (r2)     nil  (r2)
	 *
	 *    (n)           (l2)           (l2)
	 *    / \           /  \           /  \
	 *  (l) (r)  --> (l)   (r)  -->  (l)  (r)
	 *      /  \           / \            / \
	 *   (l2)  (r2)      (n)  (r2)    (r3)  (r2)
	 *   /  \            / \
	 * nil  (r3)      nil  (r3)
	 */
	parent = successor = right;
	child = successor->right;
	if (successor->left) {
		do {
			parent = successor;
			successor = successor->left;
		} while (successor->left);

		/* 'successor right' becomes 'parent left' */
		child = parent->left = successor->right;

		/* 'node right' becomes 'successor right' */
		successor->right = right;
		right->parent = successor;
	}

	/* 'node left' becomes 'successor left' */
	successor->left = left;
	left->parent = successor;

	/* 'successor' becomes 'node parent child' */
	_rb_change_child(root, node->parent, node, successor);
	successor->parent = node->parent;

	if (child) {
		/* '^successor right' becomes Black */
		child->parent = parent;
		child->color = RB_BLACK;
		parent = NULL;
	} else {
		parent = (successor->color == RB_BLACK) ? parent : NULL;
	}
	/* 'successor' acquires 'node' color */
	successor->color = node->color;

	return parent;
}

/* @func: _rb_erase_color (static)
 * #desc:
 *    fix the colors of erased node and balance rb-tree.
 *
 * #1: root node
 * #2: parent node
 */
static void _rb_erase_color(struct rb_root *root, struct rb_node *parent)
{
	struct rb_node *left, *right, *sibling, *node = NULL;

	while (1) {
		sibling = parent->right;
		if (node != sibling) { /* node is left */
			/* sibling is Red
			 *    P            S
			 *   / \          / \
			 * nil  s  -->   p   R
			 *     / \      / \
			 *    L   R   nil  L
			 */
			if (sibling && sibling->color == RB_RED) {
				sibling->color = RB_BLACK;
				parent->color = RB_RED;
				_rb_left_rotate(root, parent);
				sibling = parent->right; /* sibling left */
			}

			right = sibling->right;
			if (!right || right->color == RB_BLACK) {
				left = sibling->left;
				if (!left || left->color == RB_BLACK) {
					/* left and right is Black
					 *   (p)         (p)
					 *   / \         / \
					 * nil  S  --> nil  s
					 *     / \         / \
					 *    L   R       L   R
					 */
					sibling->color = RB_RED;
					if (parent->color == RB_RED) {
						parent->color = RB_BLACK;
					} else {
						node = parent;
						parent = node->parent;
						if (parent)
							continue;
					}

					break;
				}

				/* RL
				 *   (p)         (p)
				 *   / \         / \
				 * nil  S  --> nil  l
				 *     / \           \
				 *    l   R           S
				 *                     \
				 *                      R
				 */
				_rb_right_rotate(root, sibling);
				sibling = parent->right;
			}

			/* RR
			 *    (p)            (l)
			 *    / \            / \
			 * nil   l          P   S
			 *        \  -->   /     \
			 *         S     nil      R
			 *          \
			 *           R
			 *
			 *   (p)          (s)
			 *   / \          / \
			 * nil  S  -->   P   R
			 *     / \      / \
			 *   (l)  r   nil  (l)
			 */
			sibling->color = parent->color;
			parent->color = RB_BLACK;
			if (sibling->right)
				sibling->right->color = RB_BLACK;
			_rb_left_rotate(root, parent);

			break;
		} else { /* node is right */
			sibling = parent->left;
			/* sibling is Red
			 *     P           S
			 *    / \         / \
			 *   s   nil --> L   p
			 *  / \             / \
			 * L   R           R  nil
			 */
			if (sibling && sibling->color == RB_RED) {
				sibling->color = RB_BLACK;
				parent->color = RB_RED;
				_rb_right_rotate(root, parent);
				sibling = parent->left; /* sibling right */
			}

			left = sibling->left;
			if (!left || left->color == RB_BLACK) {
				right = sibling->right;
				if (!right || right->color == RB_BLACK) {
					/* left and right is Black
					 *    (p)         (p)
					 *    / \         / \
					 *   S  nil -->  s   nil
					 *  / \         / \
					 * L   R       L   R
					 */
					sibling->color = RB_RED;
					if (parent->color == RB_RED) {
						parent->color = RB_BLACK;
					} else {
						node = parent;
						parent = node->parent;
						if (parent)
							continue;
					}

					break;
				}

				/* LR
				 *    (p)         (p)
				 *    / \         / \
				 *   S  nil -->  r  nil
				 *  / \         /
				 * L   r       S
				 *            /
				 *           L
				 */
				_rb_left_rotate(root, sibling);
				sibling = parent->left;
			}

			/* LL
			 *      (p)          (r)
			 *      / \          / \
			 *     r   nil -->  S   P
			 *    /            /     \
			 *   S            L      nil
			 *  /
			 * L
			 *
			 *    (p)        (s)
			 *    / \        / \
			 *   S  nil --> L   P
			 *  / \            / \
			 * l  (r)        (r)  nil
			 */
			sibling->color = parent->color;
			parent->color = RB_BLACK;
			if (sibling->left)
				sibling->left->color = RB_BLACK;
			_rb_right_rotate(root, parent);

			break;
		}
	}
}

/* @func: rb_erase_fix
 * #desc:
 *    erase and fix node in the rb-tree.
 *
 * #1: root node
 * #2: node to be erased
 */
void F_SYMBOL(rb_erase_fix)(struct rb_root *root, struct rb_node *node)
{
	node = _rb_erase(root, node);
	if (node)
		_rb_erase_color(root, node);
}

/* @func: rb_wrap_insert
 * #desc:
 *    insert a node in an rb-tree.
 *
 * #1: root node
 * #2: new node
 * #3: cmp callback
 * #r: new node / NULL
 */
struct rb_node *F_SYMBOL(rb_wrap_insert)(struct rb_root *root,
		struct rb_node *new, int32_t (*cmp)(void *, void *))
{
	struct rb_node **link = &root->node, *parent = NULL;

	while (*link) {
		parent = *link;
		int32_t m = cmp(parent, new);
		if (m < 0) {
			link = &parent->left;
		} else if (m > 0) {
			link = &parent->right;
		} else {
			return NULL;
		}
	}

	new->left = new->right = NULL;
	new->parent = parent;
	new->color = RB_RED;
	*link = new;
	F_SYMBOL(rb_insert_fix)(root, new);

	return new;
}

/* @func: rb_wrap_search
 * #desc:
 *    search a node in an rb-tree.
 *
 * #1: root node
 * #2: callback arg
 * #3: cmp callback
 * #r: search node / NULL
 */
struct rb_node *F_SYMBOL(rb_wrap_search)(struct rb_root *root,
		void *arg, int32_t (*cmp)(void *, void *))
{
	struct rb_node **link = &root->node, *parent = NULL;

	while (*link) {
		parent = *link;
		int32_t m = cmp(parent, arg);
		if (m < 0) {
			link = &parent->left;
		} else if (m > 0) {
			link = &parent->right;
		} else {
			return parent;
		}
	}

	return NULL;
}

/* @func: rb_wrap_erase
 * #desc:
 *    erase a node in an rb-tree.
 *
 * #1: root node
 * #2: old node
 * #r: erase node / NULL
 */
struct rb_node *F_SYMBOL(rb_wrap_erase)(struct rb_root *root,
		struct rb_node *old)
{
	if (old)
		F_SYMBOL(rb_erase_fix)(root, old);

	return old;
}

/* @func: rb_wrap_erase2
 * #desc:
 *    search and erase a node in an rb-tree.
 *
 * #1: root node
 * #2: callback arg
 * #3: cmp callback
 * #r: erase node / NULL
 */
struct rb_node *F_SYMBOL(rb_wrap_erase2)(struct rb_root *root,
		void *arg, int32_t (*cmp)(void *, void *))
{
	struct rb_node *old = F_SYMBOL(rb_wrap_search)(root, arg, cmp);
	if (old)
		F_SYMBOL(rb_erase_fix)(root, old);

	return old;
}

/* @func: rb_wrap_first
 * #desc:
 *    returns the first node in the rb-tree.
 *
 * #1: root node
 * #r: first node / NULL
 */
struct rb_node *F_SYMBOL(rb_wrap_first)(struct rb_root *root)
{
	struct rb_node *node = root->node;
	if (!node)
		return NULL;

	while (node->left)
		node = node->left;

	return node;
}

/* @func: rb_wrap_last
 * #desc:
 *    returns the last node in the rb-tree.
 *
 * #1: root node
 * #r: last node / NULL
 */
struct rb_node *F_SYMBOL(rb_wrap_last)(struct rb_root *root)
{
	struct rb_node *node = root->node;
	if (!node)
		return NULL;

	while (node->right)
		node = node->right;

	return node;
}
