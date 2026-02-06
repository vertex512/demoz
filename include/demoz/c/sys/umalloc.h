/* @file: umalloc.h
 * #desc:
 *    The definitions of a simple memory allocator.
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

#ifndef _DEMOZ_C_SYS_UMALLOC_H
#define _DEMOZ_C_SYS_UMALLOC_H

#include <demoz/config.h>
#include <demoz/c/stddef.h>
#include <demoz/c/stdint.h>
#include <demoz/ds/list.h>


/* @def: _
 * stack 16-byte aligend */
struct umalloc_chunk {
	uint32_t prev_size;
	uint32_t size_flags;
};

struct umalloc_chunk_node {
	struct list_node list;
	size_t size;
#if (DEMOZ_MARCH_BITS == DEMOZ_MARCH_32)
	uint8_t _allgend[12]; /* 8+4+12+8 */
#endif
	struct umalloc_chunk chunk;
};

struct umalloc_ctx {
	struct list_head chunk;
	struct umalloc_chunk *idle;
	void *arg;
	/* size, arg */
	void *(*call_alloc)(size_t, void *);
	/* alloc pointer, size, arg */
	void (*call_free)(void *, size_t, void *);
};

#define UMALLOC_NEW(name, alloc, free, _arg) \
	struct umalloc_ctx name = { \
		.chunk.node = NULL, \
		.call_alloc = alloc, \
		.call_free = free, \
		.arg = _arg \
		}

#define UMALLOC_INIT(name, alloc, free, _arg) \
	(name)->chunk.node = NULL; \
	(name)->call_alloc = alloc; \
	(name)->call_free = free; \
	(name)->arg = _arg
/* end */


#ifdef __cplusplus
extern "C" {
#endif

/* c/sys/umalloc.c */

extern
void *C_SYMBOL(___umalloc)(struct umalloc_ctx *ctx, size_t size)
;

extern
int32_t C_SYMBOL(___ufree)(struct umalloc_ctx *ctx, void *p)
;

extern
void C_SYMBOL(___ufree_all)(struct umalloc_ctx *ctx)
;

#ifdef __cplusplus
}
#endif


#endif
