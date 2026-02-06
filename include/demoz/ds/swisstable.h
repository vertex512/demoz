/* @file: swisstable.h
 * #desc:
 *    The definitions of swiss hash table.
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

#ifndef _DEMOZ_DS_SWISSTABLE_H
#define _DEMOZ_DS_SWISSTABLE_H

#include <demoz/config.h>
#include <demoz/c/stddef.h>
#include <demoz/c/stdint.h>


/* @def: _ */
struct swisstable_group {
	/* TODO: simd optimization */
	uint8_t ctrl;
};

struct swisstable_head {
	struct swisstable_group *group;
	void *array;
	size_t wsize; /* size of each bucket */
	size_t size;
	size_t total_size;
	/* input key, length */
	uint64_t (*call_hash)(const void *, size_t);
	/* bucket, input key, length */
	int32_t (*call_cmp)(void *, const void *, size_t);
};

#define SWISSTABLE_NEW(name, _ctrl, _array, _wsize, \
		_total_size, _hash, _cmp) \
	struct swisstable_head name = { \
		.group = _ctrl, \
		.array = _array, \
		.wsize = _wsize, \
		.size = 0, \
		.total_size = _total_size, \
		.call_hash = _hash, \
		.call_cmp = _cmp \
		}

#define SWISSTABLE_INIT(x, _ctrl, _array, _wsize, \
		_total_size, _hash, _cmp) \
	(x)->group = _ctrl; \
	(x)->array = _array; \
	(x)->wsize = _wsize; \
	(x)->size = 0; \
	(x)->total_size = _total_size; \
	(x)->call_hash = _hash; \
	(x)->call_cmp = _cmp

/* 0b1'0000000 */
#define SWISSTABLE_EMPTY 0x80
/* 0b1'1111110 */
#define SWISSTABLE_DELETE 0xfe

#define SWISSTABLE_FACTOR(x) (((x)->size * 1000) / (x)->total_size)

#define SWISSTABLE_CTRLALIGN(x) (((x) + 0) / 1)
#define SWISSTABLE_CTRLSIZE(x) \
	(sizeof(struct swisstable_group) * SWISSTABLE_CTRLALIGN(x))

#define SWISSTABLE_SIZE(x) ((x)->size)
#define SWISSTABLE_TOTAL(x) ((x)->total_size)
#define SWISSTABLE_UNUSED(x, n) (((x)->group[n].ctrl >> 7) & 1)
/* end */


#ifdef __cplusplus
extern "C" {
#endif

/* ds/swisstable.c */

extern
void *F_SYMBOL(swisstable_insert)(struct swisstable_head *head,
		const void *key, size_t len)
;

extern
void *F_SYMBOL(swisstable_find)(struct swisstable_head *head,
		const void *key, size_t len)
;

extern
void *F_SYMBOL(swisstable_delete)(struct swisstable_head *head,
		const void *key, size_t len)
;

#ifdef __cplusplus
}
#endif


#endif
