/* @file: minheap.h
 * #desc:
 *    The definitions of minheap data structure.
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

#ifndef _DEMOZ_DS_MINHEAP_H
#define _DEMOZ_DS_MINHEAP_H

#include <demoz/config.h>
#include <demoz/c/stddef.h>
#include <demoz/c/stdint.h>


/* @def: _ */
struct minheap_head {
	void **array;
	size_t size;
	size_t total_size;
};

#define MINHEAP_NEW(name, _array, _total_size) \
	struct minheap_head name = { \
		.array = _array, \
		.size = 0, \
		.total_size = _total_size \
		}

#define MINHEAP_INIT(name, _array, _total_size) \
	(name)->array = _array; \
	(name)->size = 0; \
	(name)->total_size = _total_size

#define MINHEAP_PARENT(x) (((x) - 1) / 2)
#define MINHEAP_LEFT(x) ((x) * 2 + 1)
#define MINHEAP_RIGHT(x) ((x) * 2 + 2)
#define MINHEAP_SIZEOF (sizeof(void *))

#define MINHEAP_SIZE(x) ((x)->size)
#define MINHEAP_ARRAY(x, n) ((x)->array[n])
/* end */


#ifdef __cplusplus
extern "C" {
#endif

/* ds/minheap.c */

extern
int32_t F_SYMBOL(minheap_insert)(struct minheap_head *head,
		void *_new, int32_t (*cmp)(void *, void *))
;

extern
void F_SYMBOL(minheap_build)(struct minheap_head *head,
		int32_t (*cmp)(void *, void *))
;

extern
ssize_t F_SYMBOL(minheap_search)(struct minheap_head *head,
		void *p)
;

extern
ssize_t F_SYMBOL(minheap_search2)(struct minheap_head *head,
		void *arg, int32_t (*cmp)(void *, void *))
;

extern
int32_t F_SYMBOL(minheap_erase)(struct minheap_head *head,
		size_t ind, int32_t (*cmp)(void *, void *))
;

extern
void* F_SYMBOL(minheap_extract)(struct minheap_head *head,
		int32_t (*cmp)(void *, void *))
;

#ifdef __cplusplus
}
#endif


#endif
