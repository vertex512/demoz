/* @file: sha3.h
 * #desc:
 *    The definitions of secure hash-3 algorithm.
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

#ifndef _DEMOZ_LIB_SHA3_H
#define _DEMOZ_LIB_SHA3_H

#include <demoz/config.h>
#include <demoz/c/stddef.h>
#include <demoz/c/stdint.h>


/* @def: _ */
#define SHA3_224_TYPE 1
#define SHA3_224_LEN 28
/* (1600 - (2 * 224)) / 8 */
#define SHA3_224_RATE 144

#define SHA3_256_TYPE 2
#define SHA3_256_LEN 32
/* (1600 - (2 * 256)) / 8 */
#define SHA3_256_RATE 136

#define SHA3_384_TYPE 3
#define SHA3_384_LEN 48
/* (1600 - (2 * 384)) / 8 */
#define SHA3_384_RATE 104

#define SHA3_512_TYPE 4
#define SHA3_512_LEN 64
/* (1600 - (2 * 512)) / 8 */
#define SHA3_512_RATE 72

#define SHA3_SHAKE128_TYPE 5
/* (1600 - (2 * 128)) / 8 */
#define SHA3_SHAKE128_RATE 168

#define SHA3_SHAKE256_TYPE 6
/* (1600 - (2 * 256)) / 8 */
#define SHA3_SHAKE256_RATE 136

#define SHA3_STATE_SIZE (5 * 5 * 8)
#define SHA3_KECCAK_ROUNDS 24

struct sha3_ctx {
	uint64_t state[5][5];
	union {
		uint8_t buf[SHA3_STATE_SIZE];
		uint8_t digest[SHA3_STATE_SIZE];
	} u;
	uint8_t pad;
	uint32_t rate;
	uint32_t dsize;
	uint32_t count;
};

#define SHA3_NEW(x) struct sha3_ctx x

#define SHA3_SETPAD(x, n) (x)->pad = n
#define SHA3_GETDSIZE(x) ((x)->dsize)
#define SHA3_GETRATE(x) ((x)->rate)
#define SHA3_STATE(x, n) ((x)->u.digest[n])
/* end */


#ifdef __cplusplus
extern "C" {
#endif

/* lib/sha3.c */

extern
void F_SYMBOL(keccak_f1600)(uint64_t state[5][5])
;

extern
size_t F_SYMBOL(keccak_absorb)(uint64_t state[5][5], const uint8_t *in,
		size_t len, uint32_t rate)
;

extern
void F_SYMBOL(keccak_squeeze)(uint64_t state[5][5], uint8_t *out,
		uint32_t len, uint32_t rate)
;

extern
int32_t F_SYMBOL(sha3_init)(struct sha3_ctx *ctx, int32_t type,
		uint32_t dsize)
;

extern
void F_SYMBOL(sha3_process)(struct sha3_ctx *ctx, const uint8_t *s,
		size_t len)
;

extern
void F_SYMBOL(sha3_finish)(struct sha3_ctx *ctx)
;

extern
void F_SYMBOL(sha3)(struct sha3_ctx *ctx, const uint8_t *s, size_t len)
;

extern
void F_SYMBOL(sha3_shake_xof)(struct sha3_ctx *ctx, uint8_t *out, uint32_t len)
;

#ifdef __cplusplus
}
#endif


#endif
