/* @file: hmac.h
 * #desc:
 *    The definitions of hash message authentication code.
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

#ifndef _DEMOZ_LIB_HMAC_H
#define _DEMOZ_LIB_HMAC_H

#include <demoz/config.h>
#include <demoz/c/stddef.h>
#include <demoz/c/stdint.h>
#include <demoz/lib/md5.h>
#include <demoz/lib/sha1.h>
#include <demoz/lib/sha2.h>
#include <demoz/lib/sha3.h>
#include <demoz/lib/blake2.h>


/* @def: _ */
struct hmac_md5_ctx {
	uint8_t ipad[MD5_BLOCKSIZE];
	uint8_t opad[MD5_BLOCKSIZE];
	struct md5_ctx ipad_ctx;
	struct md5_ctx opad_ctx;
	uint8_t *state;
};

struct hmac_sha1_ctx {
	uint8_t ipad[SHA1_BLOCKSIZE];
	uint8_t opad[SHA1_BLOCKSIZE];
	struct sha1_ctx ipad_ctx;
	struct sha1_ctx opad_ctx;
	uint8_t *state;
};

struct hmac_sha256_ctx {
	uint8_t ipad[SHA256_BLOCKSIZE];
	uint8_t opad[SHA256_BLOCKSIZE];
	struct sha256_ctx ipad_ctx;
	struct sha256_ctx opad_ctx;
	uint8_t *state;
};

struct hmac_sha512_ctx {
	uint8_t ipad[SHA512_BLOCKSIZE];
	uint8_t opad[SHA512_BLOCKSIZE];
	struct sha512_ctx ipad_ctx;
	struct sha512_ctx opad_ctx;
	uint8_t *state;
};

struct hmac_sha3_ctx {
	uint8_t ipad[SHA3_STATE_SIZE];
	uint8_t opad[SHA3_STATE_SIZE];
	struct sha3_ctx ipad_ctx;
	struct sha3_ctx opad_ctx;
	uint8_t *state;
};

struct hmac_blake2b_ctx {
	uint8_t ipad[BLAKE2B_BLOCKSIZE];
	uint8_t opad[BLAKE2B_BLOCKSIZE];
	struct blake2b_ctx ipad_ctx;
	struct blake2b_ctx opad_ctx;
	uint8_t *state;
};

struct hmac_blake2s_ctx {
	uint8_t ipad[BLAKE2S_BLOCKSIZE];
	uint8_t opad[BLAKE2S_BLOCKSIZE];
	struct blake2s_ctx ipad_ctx;
	struct blake2s_ctx opad_ctx;
	uint8_t *state;
};

#define HMAC_MD5_NEW(x) struct hmac_md5_ctx x
#define HMAC_MD5_STATE(x, n) ((x)->state[n])

#define HMAC_SHA1_NEW(x) struct hmac_sha1_ctx x
#define HMAC_SHA1_STATE(x, n) ((x)->state[n])

#define HMAC_SHA256_NEW(x) struct hmac_sha256_ctx x
#define HMAC_SHA256_STATE(x, n) ((x)->state[n])

#define HMAC_SHA224_NEW(x) struct hmac_sha256_ctx x
#define HMAC_SHA224_STATE(x, n) ((x)->state[n])

#define HMAC_SHA512_NEW(x) struct hmac_sha512_ctx x
#define HMAC_SHA512_STATE(x, n) ((x)->state[n])

#define HMAC_SHA384_NEW(x) struct hmac_sha512_ctx x
#define HMAC_SHA384_STATE(x, n) ((x)->state[n])

#define HMAC_SHA3_NEW(x) struct hmac_sha3_ctx x
#define HMAC_SHA3_STATE(x, n) ((x)->state[n])

#define HMAC_BLAKE2S_NEW(x) struct hmac_blake2s_ctx x
#define HMAC_BLAKE2S_STATE(x, n) ((x)->state[n])

#define HMAC_BLAKE2B_NEW(x) struct hmac_blake2b_ctx x
#define HMAC_BLAKE2B_STATE(x, n) ((x)->state[n])
/* end */


#ifdef __cplusplus
extern "C" {
#endif

/* lib/hmac_blake2b.c */

extern
void F_SYMBOL(hmac_blake2b_init)(struct hmac_blake2b_ctx *ctx,
		const uint8_t *key, uint32_t key_len)
;

extern
void F_SYMBOL(hmac_blake2b_process)(struct hmac_blake2b_ctx *ctx,
		const uint8_t *s, size_t len)
;

extern
void F_SYMBOL(hmac_blake2b_finish)(struct hmac_blake2b_ctx *ctx)
;

extern
void F_SYMBOL(hmac_blake2b)(struct hmac_blake2b_ctx *ctx, const uint8_t *s,
		size_t len)
;

/* lib/hmac_blake2s.c */

extern
void F_SYMBOL(hmac_blake2s_init)(struct hmac_blake2s_ctx *ctx,
		const uint8_t *key, uint32_t key_len)
;

extern
void F_SYMBOL(hmac_blake2s_process)(struct hmac_blake2s_ctx *ctx,
		const uint8_t *s, size_t len)
;

extern
void F_SYMBOL(hmac_blake2s_finish)(struct hmac_blake2s_ctx *ctx)
;

extern
void F_SYMBOL(hmac_blake2s)(struct hmac_blake2s_ctx *ctx, const uint8_t *s,
		size_t len)
;

/* lib/hmac_md5.c */

extern
void F_SYMBOL(hmac_md5_init)(struct hmac_md5_ctx *ctx, const uint8_t *key,
		uint32_t key_len)
;

extern
void F_SYMBOL(hmac_md5_process)(struct hmac_md5_ctx *ctx, const uint8_t *s,
		size_t len)
;

extern
void F_SYMBOL(hmac_md5_finish)(struct hmac_md5_ctx *ctx, uint64_t len)
;

extern
void F_SYMBOL(hmac_md5)(struct hmac_md5_ctx *ctx, const uint8_t *s,
		size_t len)
;

/* lib/hmac_sha1.c */

extern
void F_SYMBOL(hmac_sha1_init)(struct hmac_sha1_ctx *ctx, const uint8_t *key,
		uint32_t key_len)
;

extern
void F_SYMBOL(hmac_sha1_process)(struct hmac_sha1_ctx *ctx, const uint8_t *s,
		size_t len)
;

extern
void F_SYMBOL(hmac_sha1_finish)(struct hmac_sha1_ctx *ctx, uint64_t len)
;

extern
void F_SYMBOL(hmac_sha1)(struct hmac_sha1_ctx *ctx, const uint8_t *s,
		size_t len)
;

/* lib/hmac_sha256.c */

extern
void F_SYMBOL(hmac_sha256_init)(struct hmac_sha256_ctx *ctx,
		const uint8_t *key, uint32_t key_len)
;

extern
void F_SYMBOL(hmac_sha256_process)(struct hmac_sha256_ctx *ctx,
		const uint8_t *s, size_t len)
;

extern
void F_SYMBOL(hmac_sha256_finish)(struct hmac_sha256_ctx *ctx,
		uint64_t len)
;

extern
void F_SYMBOL(hmac_sha256)(struct hmac_sha256_ctx *ctx, const uint8_t *s,
		size_t len)
;

extern
void F_SYMBOL(hmac_sha224_init)(struct hmac_sha256_ctx *ctx,
		const uint8_t *key, uint32_t key_len)
;

extern
void F_SYMBOL(hmac_sha224_process)(struct hmac_sha256_ctx *ctx,
		const uint8_t *s, size_t len)
;

extern
void F_SYMBOL(hmac_sha224_finish)(struct hmac_sha256_ctx *ctx,
		uint64_t len)
;

extern
void F_SYMBOL(hmac_sha224)(struct hmac_sha256_ctx *ctx, const uint8_t *s,
		size_t len)
;

/* lib/hmac_sha3.c */

extern
int32_t F_SYMBOL(hmac_sha3_init)(struct hmac_sha3_ctx *ctx,
		const uint8_t *key, uint32_t key_len, int32_t type)
;

extern
void F_SYMBOL(hmac_sha3_process)(struct hmac_sha3_ctx *ctx, const uint8_t *s,
		size_t len)
;

extern
void F_SYMBOL(hmac_sha3_finish)(struct hmac_sha3_ctx *ctx)
;

extern
void F_SYMBOL(hmac_sha3)(struct hmac_sha3_ctx *ctx, const uint8_t *s,
		size_t len)
;

/* lib/hmac_sha512.c */

extern
void F_SYMBOL(hmac_sha512_init)(struct hmac_sha512_ctx *ctx,
		const uint8_t *key, uint32_t key_len)
;

extern
void F_SYMBOL(hmac_sha512_process)(struct hmac_sha512_ctx *ctx,
		const uint8_t *s, size_t len)
;

extern
void F_SYMBOL(hmac_sha512_finish)(struct hmac_sha512_ctx *ctx,
		uint64_t len)
;

extern
void F_SYMBOL(hmac_sha512)(struct hmac_sha512_ctx *ctx, const uint8_t *s,
		size_t len)
;

extern
void F_SYMBOL(hmac_sha384_init)(struct hmac_sha512_ctx *ctx,
		const uint8_t *key, uint32_t key_len)
;

extern
void F_SYMBOL(hmac_sha384_process)(struct hmac_sha512_ctx *ctx,
		const uint8_t *s, size_t len)
;

extern
void F_SYMBOL(hmac_sha384_finish)(struct hmac_sha512_ctx *ctx,
		uint64_t len)
;

extern
void F_SYMBOL(hmac_sha384)(struct hmac_sha512_ctx *ctx, const uint8_t *s,
		size_t len)
;

#ifdef __cplusplus
}
#endif


#endif
