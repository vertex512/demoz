/* @file: list.c
 * #desc:
 *    The implementations of linked list.
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
#include <demoz/ds/list.h>


/* @func: list_add
 * #desc:
 *    add a new node in the linked list.
 *
 * #1: list head
 * #2: new node
 * #r: new node
 */
struct list_node *F_SYMBOL(list_add)(struct list_head *head,
		struct list_node *new)
{
	struct list_node *node = head->node;

	if (node) {
		new->prev = node->prev ? node->prev : node;
		new->next = node;
		node->prev = new;
	} else {
		new->prev = new->next = NULL;
	}
	head->node = new;

	return new;
}

/* @func: list_add_tail
 * #desc:
 *    add a new node in the linked list (add to tail).
 *
 * #1: list head
 * #2: new node
 * #r: new node
 */
struct list_node *F_SYMBOL(list_add_tail)(struct list_head *head,
		struct list_node *new)
{
	struct list_node *node = head->node, *prev = NULL;

	if (node) {
		prev = node->prev ? node->prev : node;
		new->prev = prev;
		new->next = NULL;
		node->prev = prev->next = new;
	} else {
		new->prev = new->next = NULL;
		head->node = new;
	}

	return new;
}

/* @func: list_del
 * #desc:
 *    delete a node in the linked list.
 *
 * #1: list head
 * #2: old node
 * #r: old node
 */
struct list_node *F_SYMBOL(list_del)(struct list_head *head,
		struct list_node *old)
{
	struct list_node *node = head->node,
		*prev = old->prev, *next = old->next;

	if (node == old) {
		if (next)
			next->prev = (prev != next) ? prev : NULL;
		head->node = next;
	} else {
		prev->next = next;
		if (next) {
			next->prev = prev;
		} else {
			node->prev = prev;
		}
	}

	return old;
}

/* @func: list_insert
 * #desc:
 *    insert a new node in the linked list node.
 *
 * #1: list head
 * #2: list node
 * #3: new node
 * #r: new node
 */
struct list_node *F_SYMBOL(list_insert)(struct list_head *head,
		struct list_node *node, struct list_node *new)
{
	struct list_node *prev = node->prev;

	if (prev) {
		new->prev = prev;
		new->next = node;
		node->prev = new;
		if (node != head->node) {
			prev->next = new;
		} else {
			head->node = new;
		}
	} else {
		F_SYMBOL(list_add)(head, new);
	}

	return new;
}

/* @func: list_insert_tail
 * #desc:
 *    insert a new node in the linked list node (add to tail).
 *
 * #1: list head
 * #2: list node
 * #3: new node
 * #r: new node
 */
struct list_node *F_SYMBOL(list_insert_tail)(struct list_head *head,
		struct list_node *node, struct list_node *new)
{
	struct list_node *next = node->next;

	if (next) {
		new->next = next;
		new->prev = node;
		node->next = next->prev = new;
	} else {
		F_SYMBOL(list_add_tail)(head, new);
	}

	return new;
}

/* @func: list_swap
 * #desc:
 *    node swap in a linked list.
 *
 * #1: list head
 * #2: old node
 * #3: new node
 */
void F_SYMBOL(list_swap)(struct list_head *head,
		struct list_node *old, struct list_node *new)
{
	struct list_node *node;

	if (old->next == new) {
		F_SYMBOL(list_del)(head, new);
		F_SYMBOL(list_insert)(head, old, new);
		return;
	} else if (old->prev == new) {
		F_SYMBOL(list_del)(head, new);
		F_SYMBOL(list_insert_tail)(head, old, new);
		return;
	}

	node = head->node;
	if (node == new) {
		node = new->next;

		F_SYMBOL(list_del)(head, new);
		F_SYMBOL(list_insert_tail)(head, old, new);

		F_SYMBOL(list_del)(head, old);
		F_SYMBOL(list_insert)(head, node, old);
	} else {
		node = new->prev;

		F_SYMBOL(list_del)(head, new);
		F_SYMBOL(list_insert_tail)(head, old, new);

		F_SYMBOL(list_del)(head, old);
		F_SYMBOL(list_insert_tail)(head, node, old);
	}
}
