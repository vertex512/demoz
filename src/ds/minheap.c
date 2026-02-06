/* @file: minheap.c
 * #desc:
 *    The implementations of minheap data structure.
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
#include <demoz/ds/minheap.h>


/* @func: _minheapify_swap (static)
 * #desc:
 *    minheap member swap.
 *
 * #1: member pointer
 * #2: member pointer
 */
static void _minheapify_swap(void **a, void **b)
{
	void *t = *a;
	*a = *b;
	*b = t;
}

/* @func: _minheapify_up (static)
 * #desc:
 *    minheap member up shift.
 *
 * #1: minheap head
 * #2: array index
 * #3: callback
 */
static void _minheapify_up(struct minheap_head *head,
		size_t n, int32_t (*cmp)(void *, void *))
{
	while (n > 0) {
		size_t k = MINHEAP_PARENT(n);
		if (!(cmp(head->array[k], head->array[n]) > 0))
			break;

	_minheapify_swap(&head->array[k], &head->array[n]);
		n = k;
	}
}

/* @func: _minheapify_down (static)
 * #desc:
 *    minheap member down shift.
 *
 * #1: minheap head
 * #2: array index
 * #3: callback
 */
static void _minheapify_down(struct minheap_head *head,
		size_t n, int32_t (*cmp)(void *, void *))
{
	size_t m, l, r;
	while (1) {
		m = n;
		l = MINHEAP_LEFT(n);
		r = MINHEAP_RIGHT(n);

		if (l < head->size && cmp(head->array[l], head->array[m]) < 0)
			m = l;
		if (r < head->size && cmp(head->array[r], head->array[m]) < 0)
			m = r;
		if (m == n)
			break;

		_minheapify_swap(&head->array[n], &head->array[m]);
		n = m;
	}
}

/* @func: minheap_insert
 * #desc:
 *    insert a minheap array member.
 *
 * #1: minheap head
 * #2: new member pointer
 * #3: callback
 * #r: 0: no error, -1: error array full
 */
int32_t F_SYMBOL(minheap_insert)(struct minheap_head *head,
		void *new, int32_t (*cmp)(void *, void *))
{
	if (head->size >= head->total_size)
		return -1;

	head->array[head->size] = new;
	_minheapify_up(head, head->size, cmp);
	head->size++;

	return 0;
}

/* @func: minheap_build
 * #desc:
 *    build a minheap (array assignment).
 *
 * #1: minheap head
 * #2: callback
 */
void F_SYMBOL(minheap_build)(struct minheap_head *head,
		int32_t (*cmp)(void *, void *))
{
	if (head->size < 1)
		return;

	ssize_t n = MINHEAP_PARENT(head->size - 1);
	for (; n >= 0; n--)
		_minheapify_down(head, n, cmp);
}

/* @func: minheap_search
 * #desc:
 *    search for minheap array member index.
 *
 * #1: minheap head
 * #2: member pointer
 * #r: array index (-1: not found)
 */
ssize_t F_SYMBOL(minheap_search)(struct minheap_head *head,
		void *p)
{
	for (size_t i = 0; i < head->size; i++) {
		if (head->array[i] == p)
			return i;
	}

	return -1;
}

/* @func: minheap_search2
 * #desc:
 *    search for minheap array member index.
 *
 * #1: minheap head
 * #2: callback arg
 * #3: callback
 * #r: array index (-1: not found)
 */
ssize_t F_SYMBOL(minheap_search2)(struct minheap_head *head,
		void *arg, int32_t (*cmp)(void *, void *))
{
	for (size_t i = 0; i < head->size; i++) {
		if (!cmp(head->array[i], arg))
			return i;
	}

	return -1;
}

/* @func: minheap_erase
 * #desc:
 *    erase a minheap array member.
 *
 * #1: minheap head
 * #2: array index
 * #3: callback
 * #r: 0: no error, -1: error array full
 */
int32_t F_SYMBOL(minheap_erase)(struct minheap_head *head,
		size_t ind, int32_t (*cmp)(void *, void *))
{
	if (head->size < 1 || ind > (head->size - 1))
		return -1;

	head->array[ind] = head->array[head->size - 1];
	head->size--;

	size_t k = MINHEAP_PARENT(ind);
	if (ind > 0 && cmp(head->array[ind], head->array[k]) < 0) {
		_minheapify_up(head, ind, cmp);
	} else if (head->size) {
		_minheapify_down(head, ind, cmp);
	}

	return 0;
}

/* @func: minheap_extract
 * #desc:
 *    extract a minheap minimum member.
 *
 * #1: minheap head
 * #2: callback
 * #r: minimum member / NULL pointer
 */
void* F_SYMBOL(minheap_extract)(struct minheap_head *head,
		int32_t (*cmp)(void *, void *))
{
	if (head->size < 1)
		return NULL;

	void *p = head->array[0];
	head->array[0] = head->array[head->size - 1];
	head->size--;

	if (head->size)
		_minheapify_down(head, 0, cmp);

	return p;
}
