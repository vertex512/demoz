/* @file: blake2.h
 * #desc:
 *    The definitions of blake2 cryptographic hash.
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

#ifndef _DEMOZ_LIB_BLAKE2_H
#define _DEMOZ_LIB_BLAKE2_H

#include <demoz/config.h>
#include <demoz/c/stddef.h>
#include <demoz/c/stdint.h>


/* @def: _ */
#define BLAKE2B_224_LEN 28
#define BLAKE2B_256_LEN 32
#define BLAKE2B_384_LEN 48
#define BLAKE2B_512_LEN 64

#define BLAKE2B_BLOCKSIZE 128

struct blake2b_param {
	uint8_t digest_length; /* 1 */
	uint8_t key_length;    /* 2 */
	uint8_t fanout;        /* 3 */
	uint8_t depth;         /* 4 */
	uint32_t leaf_length;  /* 8 */
	uint32_t node_offset;  /* 12 */
	uint32_t xof_length;   /* 16 */
	uint8_t node_depth;    /* 17 */
	uint8_t inner_length;  /* 18 */
	uint8_t reserved[14];  /* 32 */
	uint8_t salt[16];      /* 48 */
	uint8_t personal[16];  /* 64 */
};

struct blake2b_ctx {
	uint64_t state[8];
	uint64_t tsize[2];
	uint64_t flags[2];
	union {
		struct blake2b_param param;
		uint64_t state[8];
	} u;
	uint8_t buf[BLAKE2B_BLOCKSIZE];
	uint32_t count;
};

#define BLAKE2B_NEW(x) struct blake2b_ctx x
#define BLAKE2B_STATE(x, n) (((uint8_t *)(x)->state)[n])
/* end */

/* @def: _ */
#define BLAKE2S_128_LEN 16
#define BLAKE2S_196_LEN 24
#define BLAKE2S_224_LEN 28
#define BLAKE2S_256_LEN 32

#define BLAKE2S_BLOCKSIZE 64

struct blake2s_param {
	uint8_t digest_length; /* 1 */
	uint8_t key_length;    /* 2 */
	uint8_t fanout;        /* 3 */
	uint8_t depth;         /* 4 */
	uint32_t leaf_length;  /* 8 */
	uint32_t node_offset;  /* 12 */
	uint16_t xof_length;   /* 14 */
	uint8_t node_depth;    /* 15 */
	uint8_t inner_length;  /* 16 */
	uint8_t salt[8];       /* 24 */
	uint8_t personal[8];   /* 32 */
};

struct blake2s_ctx {
	uint32_t state[8];
	uint32_t tsize[2];
	uint32_t flags[2];
	union {
		struct blake2s_param param;
		uint32_t state[8];
	} u;
	uint8_t buf[BLAKE2S_BLOCKSIZE];
	uint32_t count;
};

#define BLAKE2S_NEW(x) struct blake2s_ctx x
#define BLAKE2S_STATE(x, n) (((uint8_t *)(x)->state)[n])
/* end */


#ifdef __cplusplus
extern "C" {
#endif

/* lib/blake2b.c */

extern
int32_t F_SYMBOL(blake2b_init)(struct blake2b_ctx *ctx, uint32_t dsize)
;

extern
void F_SYMBOL(blake2b_init_param)(struct blake2b_ctx *ctx,
		const struct blake2b_param *p)
;

extern
void F_SYMBOL(blake2b_process)(struct blake2b_ctx *ctx, const uint8_t *s,
		size_t len)
;

extern
void F_SYMBOL(blake2b_finish)(struct blake2b_ctx *ctx)
;

extern
void F_SYMBOL(blake2b)(struct blake2b_ctx *ctx, const uint8_t *s,
		size_t len)
;

/* lib/blake2s.c */

extern
int32_t F_SYMBOL(blake2s_init)(struct blake2s_ctx *ctx, uint32_t dsize)
;

extern
void F_SYMBOL(blake2s_init_param)(struct blake2s_ctx *ctx,
		const struct blake2s_param *p)
;

extern
void F_SYMBOL(blake2s_process)(struct blake2s_ctx *ctx, const uint8_t *s,
		size_t len)
;

extern
void F_SYMBOL(blake2s_finish)(struct blake2s_ctx *ctx)
;

extern
void F_SYMBOL(blake2s)(struct blake2s_ctx *ctx, const uint8_t *s,
		size_t len)
;

#ifdef __cplusplus
}
#endif


#endif
