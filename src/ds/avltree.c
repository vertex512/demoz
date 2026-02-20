/* @file: avltree.c
 * #desc:
 *    The implementations of AVL (Adelson-Velsky and Landis) self-balancing
 *    binary tree.
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
#include <demoz/ds/avltree.h>


/* @func: _avl_change_child (static)
 * #desc:
 *    change the child of the parent node.
 *
 * #1: root   [in/out] root node
 * #2: parent [in/out] parent node
 * #3: old    [in]     old node
 * #4: new    [in]     new node
 */
static void _avl_change_child(struct avl_root *root, struct avl_node *parent,
		struct avl_node *old, struct avl_node *new)
{
	if (!parent) {
		root->node = new;
	} else if (parent->left == old) {
		parent->left = new;
	} else {
		parent->right = new;
	}
}

/* @func: _avl_update_height (static)
 * #desc:
 *    update the node height.
 *
 * #1: node [in/out] avl-tree node
 */
static void _avl_update_height(struct avl_node *node)
{
	int32_t lh = AVL_HEIGHT(node->left);
	int32_t rh = AVL_HEIGHT(node->right);

	node->bf = 1 + AVL_MAX(lh, rh);
}

/* @func: _avl_balance_factor (static)
 * #desc:
 *    get the balance factor of the node.
 *
 * #1: node [in] avl-tree node
 */
static int32_t _avl_balance_factor(struct avl_node *node)
{
	int32_t lh = AVL_HEIGHT(node->left);
	int32_t rh = AVL_HEIGHT(node->right);

	return lh - rh;
}

/* @func: _avl_left_rotate (static)
 * #desc:
 *    avl-tree left rotate.
 *
 *        g           g
 *       / \         / \
 *      p   u  -->  n   u
 *     / \         / \
 *    s   n       p   r
 *       / \     / \
 *      l   r   s   l
 *
 * #1: root   [in/out] root node
 * #2: parent [in/out] parent of the rotate node
 */
static void _avl_left_rotate(struct avl_root *root, struct avl_node *parent)
{
	struct avl_node *gparent, *node, *child;
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
	_avl_change_child(root, gparent, parent, node);
	node->parent = gparent;

	/* update height */
	_avl_update_height(parent);
	_avl_update_height(node);
}

/* @func: _avl_right_rotate (static)
 * #desc:
 *    avl-tree right rotate.
 *
 *          g         g
 *         / \       / \
 *        p   u --> n   u
 *       / \       / \
 *      n   s     l   p
 *     / \           / \
 *    l   r         r   s
 *
 * #1: root   [in/out] root node
 * #2: parent [in/out] parent of the rotate node
 */
static void _avl_right_rotate(struct avl_root *root, struct avl_node *parent)
{
	struct avl_node *gparent, *node, *child;
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
	_avl_change_child(root, gparent, parent, node);
	node->parent = gparent;

	/* update height */
	_avl_update_height(parent);
	_avl_update_height(node);
}

/* @func: avl_balance
 * #desc:
 *    fix the avl-tree balance.
 *
 * #1: root [in/out] root node
 * #2: node [in/out] node to be fix
 */
void F_SYMBOL(avl_balance)(struct avl_root *root, struct avl_node *node)
{
	for (struct avl_node *parent = NULL; node; node = parent) {
		_avl_update_height(node);

		parent = node->parent;
		if (!parent)
			break;

		int32_t bf = _avl_balance_factor(parent);
		if (bf > 1) { /* LL LR */
			/* LR */
			if (_avl_balance_factor(parent->left) < 0)
				_avl_left_rotate(root, parent->left);
			/* LL */
			_avl_right_rotate(root, parent);
		} else if (bf < -1) { /* RR RL */
			/* RL */
			if (_avl_balance_factor(parent->right) > 0)
				_avl_right_rotate(root, parent->right);
			/* RR */
			_avl_left_rotate(root, parent);
		}
	}
}

/* @func: avl_erase_fix
 * #desc:
 *    erase and fix the node balance in the avl-tree.
 *
 * #1: root [in/out] root node
 * #2: node [in/out] node to be erased
 */
void F_SYMBOL(avl_erase_fix)(struct avl_root *root, struct avl_node *node)
{
	struct avl_node *parent = node->parent, *left = node->left,
		*right = node->right, *successor;

	/*
	 *   n          r          r
	 *  / \        / \        / \
	 * l   r  --> l   n  --> l   r2
	 *    / \        / \
	 *  nil  r2    nil  r2
	 *
	 *    n          l2         l2
	 *   / \        / \        / \
	 *  l   r  --> l   r  --> l   r
	 *     / \        / \        / \
	 *   l2  r2      n  r2      r3  r2
	 *   / \        / \
	 * nil  r3    nil  r3
	 */
	if (left && right) {
		successor = right;
		if (successor->left) {
			do {
				parent = successor;
				successor = successor->left;
			} while (successor->left);

			/* 'successor right' becomes 'parent left' */
			parent->left = successor->right;
			if (parent->left)
				parent->left->parent = parent;

			/* 'node right' becomes 'successor right' */
			successor->right = right;
			right->parent = successor;
		}

		/* 'node left' becomes 'successor left' */
		successor->left = left;
		left->parent = successor;

		/* 'successor' becomes 'node parent child' */
		_avl_change_child(root, node->parent, node, successor);
		successor->parent = node->parent;

		if (successor == right) {
			F_SYMBOL(avl_balance)(root, left);
		} else {
			F_SYMBOL(avl_balance)(root, successor);
		}

		return;
	}
	/*
	 *     p         p
	 *    / \       / \
	 *   n   s --> l   s
	 *  / \
	 * l  nil
	 */
	if (left) {
		/* 'node left' becomes 'parent child' */
		_avl_change_child(root, parent, node, left);
		left->parent = parent;

		F_SYMBOL(avl_balance)(root, left);

		return;
	}
	/*
	 *      p         p
	 *     / \       / \
	 *    n   s --> r   s
	 *   / \
	 * nil  r
	 */
	if (right) {
		/* 'node right' becomes 'parent child' */
		_avl_change_child(root, parent, node, right);
		right->parent = parent;

		F_SYMBOL(avl_balance)(root, right);

		return;
	}

	/*
	 *      p           p
	 *     / \         / \
	 *    n   s -->  nil  s
	 *   / \
	 * nil nil
	 */
	_avl_change_child(root, parent, node, NULL);
	if (parent) {
		/* sibling node */
		if (parent->left) {
			F_SYMBOL(avl_balance)(root, parent->left);
		} else if (parent->right) {
			F_SYMBOL(avl_balance)(root, parent->right);
		} else { /* parent */
			F_SYMBOL(avl_balance)(root, parent);
		}
	} /* 'parent' is nil == root */
}

/* @func: avl_wrap_insert
 * #desc:
 *    insert a node in an avl-tree.
 *
 * #1: root [in/out] root node
 * #2: new  [in/out] new node
 * #3: cmp  [in]     cmp callback
 * #r:      [ret]    new node / NULL
 */
struct avl_node *F_SYMBOL(avl_wrap_insert)(struct avl_root *root,
		struct avl_node *new, int32_t (*cmp)(void *, void *))
{
	struct avl_node **link = &root->node, *parent = NULL;

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
	new->bf = 1;
	*link = new;
	F_SYMBOL(avl_balance)(root, new);

	return new;
}

/* @func: avl_wrap_search
 * #desc:
 *    search a node in an avl-tree.
 *
 * #1: root [in/out] root node
 * #2: arg  [in]     callback arg
 * #3: cmp  [in]     cmp callback
 * #r:      [ret]    search node / NULL
 */
struct avl_node *F_SYMBOL(avl_wrap_search)(struct avl_root *root,
		void *arg, int32_t (*cmp)(void *, void *))
{
	struct avl_node **link = &root->node, *parent = NULL;

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

/* @func: avl_wrap_erase
 * #desc:
 *    erase a node in an avl-tree.
 *
 * #1: root [in/out] root node
 * #2: old  [in/out] old node
 * #r:      [ret]    erase node / NULL
 */
struct avl_node *F_SYMBOL(avl_wrap_erase)(struct avl_root *root,
		struct avl_node *old)
{
	if (old)
		F_SYMBOL(avl_erase_fix)(root, old);

	return old;
}

/* @func: avl_wrap_erase2
 * #desc:
 *    search and erase a node in an avl-tree.
 *
 * #1: root [in/out] root node
 * #2: arg  [in]     callback arg
 * #3: cmp  [in]     cmp callback
 * #r:      [ret]    erase node / NULL
 */
struct avl_node *F_SYMBOL(avl_wrap_erase2)(struct avl_root *root,
		void *arg, int32_t (*cmp)(void *, void *))
{
	struct avl_node *old = F_SYMBOL(avl_wrap_search)(root, arg, cmp);
	if (old)
		F_SYMBOL(avl_erase_fix)(root, old);

	return old;
}

/* @func: avl_wrap_first
 * #desc:
 *    returns the first node in the avl-tree.
 *
 * #1: root [in/out] root node
 * #r:      [ret]    first node / NULL
 */
struct avl_node *F_SYMBOL(avl_wrap_first)(struct avl_root *root)
{
	struct avl_node *node = root->node;
	if (!node)
		return NULL;

	while (node->left)
		node = node->left;

	return node;
}

/* @func: avl_wrap_last
 * #desc:
 *    returns the last node in the avl-tree.
 *
 * #1: root [in/out] root node
 * #r:      [ret]    last node / NULL
 */
struct avl_node *F_SYMBOL(avl_wrap_last)(struct avl_root *root)
{
	struct avl_node *node = root->node;
	if (!node)
		return NULL;

	while (node->right)
		node = node->right;

	return node;
}
