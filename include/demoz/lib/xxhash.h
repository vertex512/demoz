/* @file: xxhash.c
 * #desc:
 *    The definitions of extremely fast hash algorithm.
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

#ifndef _DEMOZ_LIB_XXHASH_C
#define _DEMOZ_LIB_XXHASH_C

#include <demoz/config.h>
#include <demoz/c/stddef.h>
#include <demoz/c/stdint.h>


/* @def: _ */
#define XXHASH32_BLOCKSIZE 16

struct xxhash32_ctx {
	uint32_t state[4];
	uint8_t buf[XXHASH32_BLOCKSIZE];
	uint32_t count;
	uint32_t seed;
};

#define XXHASH32_NEW(x) struct xxhash32_ctx x
/* end */

/* @def: _ */
#define XXHASH64_BLOCKSIZE 32

struct xxhash64_ctx {
	uint64_t state[4];
	uint8_t buf[XXHASH64_BLOCKSIZE];
	uint32_t count;
	uint64_t seed;
};

#define XXHASH64_NEW(x) struct xxhash64_ctx x
/* end */


#ifdef __cplusplus
extern "C" {
#endif

/* lib/xxhash32.c */

extern
void F_SYMBOL(xxhash32_init)(struct xxhash32_ctx *ctx, uint32_t seed)
;

extern
void F_SYMBOL(xxhash32_process)(struct xxhash32_ctx *ctx, const uint8_t *s,
		size_t len)
;

extern
uint32_t F_SYMBOL(xxhash32_finish)(struct xxhash32_ctx *ctx, size_t len)
;

extern
uint32_t F_SYMBOL(xxhash32)(struct xxhash32_ctx *ctx, const uint8_t *s,
		size_t len)
;

/* lib/xxhash64.c */

extern
void F_SYMBOL(xxhash64_init)(struct xxhash64_ctx *ctx, uint64_t seed)
;

extern
void F_SYMBOL(xxhash64_process)(struct xxhash64_ctx *ctx, const uint8_t *s,
		size_t len)
;

extern
uint64_t F_SYMBOL(xxhash64_finish)(struct xxhash64_ctx *ctx, uint64_t len)
;

extern
uint64_t F_SYMBOL(xxhash64)(struct xxhash64_ctx *ctx, const uint8_t *s,
		size_t len)
;

#ifdef __cplusplus
}
#endif


#endif
