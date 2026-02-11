/* @file: swissmap.h
 * #desc:
 *    The definitions of swiss high-performance hash table.
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

#ifndef _DEMOZ_DS_SWISSMAP_H
#define _DEMOZ_DS_SWISSMAP_H

#include <demoz/config.h>
#include <demoz/c/stddef.h>
#include <demoz/c/stdint.h>


/* @def: _ */
union swissmap_group {
	/* XXX: simd optimization */
	uint32_t ctrl;
	uint8_t _ctrl[4];
};

struct swissmap_head {
	union swissmap_group *group;
	void *array;
	size_t wsize; /* size of each bucket */
	size_t size;
	size_t total_size;
	/* input key, length */
	uint64_t (*call_hash)(const void *, size_t);
	/* bucket, input key, length */
	int32_t (*call_cmp)(void *, const void *, size_t);
};

#define SWISSMAP_NEW(name, _ctrl, _array, _wsize, \
		_total_size, _hash, _cmp) \
	struct swissmap_head name = { \
		.group = _ctrl, \
		.array = _array, \
		.wsize = _wsize, \
		.size = 0, \
		.total_size = _total_size, \
		.call_hash = _hash, \
		.call_cmp = _cmp \
		}

#define SWISSMAP_INIT(x, _ctrl, _array, _wsize, \
		_total_size, _hash, _cmp) \
	(x)->group = _ctrl; \
	(x)->array = _array; \
	(x)->wsize = _wsize; \
	(x)->size = 0; \
	(x)->total_size = _total_size; \
	(x)->call_hash = _hash; \
	(x)->call_cmp = _cmp

/* 0b1'0000000 */
#define SWISSMAP_EMPTY 0x80
/* 0b1'1111110 */
#define SWISSMAP_DELETE 0xfe

#define SWISSMAP_CLIGN(x) (((x) + 3) / 4)
#define SWISSMAP_ALIGN(x) (4 * SWISSMAP_CLIGN(x))

#define SWISSMAP_ACTRL(x, n) ((x)->group[(n) >> 2]._ctrl[(n) & 3])
#define SWISSMAP_ARRAY(x, n) ((void *)((char *)(x)->array + head->wsize * (n)))

#define SWISSMAP_FACTOR(x) (((x)->size * 1000) / (x)->total_size)
#define SWISSMAP_SIZE(x) ((x)->size)
#define SWISSMAP_TOTAL(x) ((x)->total_size)
/* end */


#ifdef __cplusplus
extern "C" {
#endif

/* ds/swissmap.c */

extern
void F_SYMBOL(swissmap_empty)(struct swissmap_head *head)
;

extern
void *F_SYMBOL(swissmap_insert)(struct swissmap_head *head,
		const void *key, size_t len)
;

extern
void *F_SYMBOL(swissmap_find)(struct swissmap_head *head,
		const void *key, size_t len)
;

extern
void *F_SYMBOL(swissmap_delete)(struct swissmap_head *head,
		const void *key, size_t len)
;

#ifdef __cplusplus
}
#endif


#endif
