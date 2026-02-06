/* @file: avltree.h
 * #desc:
 *    The definitions of AVL (Adelson-Velsky and Landis) self-balancing
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

#ifndef _DEMOZ_DS_AVLTREE_H
#define _DEMOZ_DS_AVLTREE_H

#include <demoz/config.h>
#include <demoz/c/stddef.h>
#include <demoz/c/stdint.h>


/* @def: _ */
struct avl_node {
	int32_t bf;
	struct avl_node *parent, *left, *right;
};

struct avl_root {
	struct avl_node *node;
};

#define AVL_ROOT_NEW(x) struct avl_root x = { NULL }
#define AVL_ROOT_INIT(x) (x)->node = NULL

#define AVL_HEIGHT(x) ((x) ? (x)->bf : 0)
#define AVL_MAX(a, b) (((a) > (b)) ? (a) : (b))
/* end */


#ifdef __cplusplus
extern "C" {
#endif

/* ds/avltree.c */

extern
void F_SYMBOL(avl_balance)(struct avl_root *root, struct avl_node *node)
;

extern
void F_SYMBOL(avl_erase_fix)(struct avl_root *root, struct avl_node *node)
;

extern
struct avl_node *F_SYMBOL(avl_wrap_insert)(struct avl_root *root,
		struct avl_node *_new, int32_t (*cmp)(void *, void *))
;

extern
struct avl_node *F_SYMBOL(avl_wrap_search)(struct avl_root *root,
		void *arg, int32_t (*cmp)(void *, void *))
;

extern
struct avl_node *F_SYMBOL(avl_wrap_erase)(struct avl_root *root,
		struct avl_node *old)
;

extern
struct avl_node *F_SYMBOL(avl_wrap_erase2)(struct avl_root *root,
		void *arg, int32_t (*cmp)(void *, void *))
;

extern
struct avl_node *F_SYMBOL(avl_wrap_first)(struct avl_root *root)
;

extern
struct avl_node *F_SYMBOL(avl_wrap_last)(struct avl_root *root)
;

#ifdef __cplusplus
}
#endif


#endif
