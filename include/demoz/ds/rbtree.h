/* @file: rbtree.h
 * #desc:
 *    The definitions of Red-Black self-balancing binary tree.
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

#ifndef _DEMOZ_DS_RBTREE_H
#define _DEMOZ_DS_RBTREE_H

#include <demoz/config.h>
#include <demoz/c/stddef.h>
#include <demoz/c/stdint.h>


/* @def: _ */
struct rb_node {
	int32_t color;
	struct rb_node *parent, *left, *right;
};

struct rb_root {
	struct rb_node *node;
};

#define RB_ROOT_NEW(x) struct rb_root x = { NULL }
#define RB_ROOT_INIT(x) (x)->node = NULL

#define RB_RED 0
#define RB_BLACK 1
/* end */


#ifdef __cplusplus
extern "C" {
#endif

/* ds/rbtree.c */

extern
void F_SYMBOL(rb_insert_fix)(struct rb_root *root, struct rb_node *node)
;

extern
void F_SYMBOL(rb_erase_fix)(struct rb_root *root, struct rb_node *node)
;

extern
struct rb_node *F_SYMBOL(rb_wrap_insert)(struct rb_root *root,
		struct rb_node *_new, int32_t (*cmp)(void *, void *))
;

extern
struct rb_node *F_SYMBOL(rb_wrap_search)(struct rb_root *root,
		void *arg, int32_t (*cmp)(void *, void *))
;

extern
struct rb_node *F_SYMBOL(rb_wrap_erase)(struct rb_root *root,
		struct rb_node *old)
;

extern
struct rb_node *F_SYMBOL(rb_wrap_erase2)(struct rb_root *root,
		void *arg, int32_t (*cmp)(void *, void *))
;

extern
struct rb_node *F_SYMBOL(rb_wrap_first)(struct rb_root *root)
;

extern
struct rb_node *F_SYMBOL(rb_wrap_last)(struct rb_root *root)
;

#ifdef __cplusplus
}
#endif


#endif
