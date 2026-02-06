/* @file: hmac_sha256.c
 * #desc:
 *    The implementations of hash message authentication code (sha256).
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
#include <demoz/c/string.h>
#include <demoz/lib/sha2.h>
#include <demoz/lib/hmac.h>


/* @func: hmac_sha256_init
 * #desc:
 *    hmac-sha256 struct context initialization.
 *
 * #1: hmac-sha256 struct context
 * #2: input key
 * #3: key length
 */
void F_SYMBOL(hmac_sha256_init)(struct hmac_sha256_ctx *ctx,
		const uint8_t *key, uint32_t key_len)
{
	SHA256_NEW(sha256_ctx);
	F_SYMBOL(sha256_init)(&sha256_ctx);

	if (key_len > SHA256_BLOCKSIZE) {
		F_SYMBOL(sha256)(&sha256_ctx, key, key_len);
		key = &(SHA256_STATE(&sha256_ctx, 0));
		key_len = SHA256_LEN;
	}

	C_SYMBOL(memset)(ctx->ipad, 0x36, SHA256_BLOCKSIZE);
	C_SYMBOL(memset)(ctx->opad, 0x5c, SHA256_BLOCKSIZE);
	for (uint32_t i = 0; i < key_len; i++) {
		ctx->ipad[i] ^= key[i];
		ctx->opad[i] ^= key[i];
	}

	F_SYMBOL(sha256_init)(&ctx->ipad_ctx);
	F_SYMBOL(sha256_init)(&ctx->opad_ctx);
	F_SYMBOL(sha256_process)(&ctx->ipad_ctx, ctx->ipad,
		SHA256_BLOCKSIZE);

	ctx->state = &(SHA256_STATE(&ctx->opad_ctx, 0));
}

/* @func: hmac_sha256_process
 * #desc:
 *    hmac-sha256 processing buffer function.
 *
 * #1: hmac-sha256 struct context
 * #2: input buffer
 * #3: input length
 */
void F_SYMBOL(hmac_sha256_process)(struct hmac_sha256_ctx *ctx,
		const uint8_t *s, size_t len)
{
	F_SYMBOL(sha256_process)(&ctx->ipad_ctx, s, len);
}

/* @func: hmac_sha256_finish
 * #desc:
 *    hmac-sha256 process the remaining bytes in the buffer and end.
 *
 * #1: hmac-sha256 struct context
 * #2: total length
 */
void F_SYMBOL(hmac_sha256_finish)(struct hmac_sha256_ctx *ctx,
		uint64_t len)
{
	F_SYMBOL(sha256_finish)(&ctx->ipad_ctx, len + SHA256_BLOCKSIZE);

	F_SYMBOL(sha256_process)(&ctx->opad_ctx, ctx->opad,
		SHA256_BLOCKSIZE);
	F_SYMBOL(sha256_process)(&ctx->opad_ctx,
		&(SHA256_STATE(&ctx->ipad_ctx, 0)),
		SHA256_LEN);
	F_SYMBOL(sha256_finish)(&ctx->opad_ctx,
		SHA256_BLOCKSIZE + SHA256_LEN);
}

/* @func: hmac_sha256
 * #desc:
 *    hmac-sha256 single-time processing function.
 *
 * #1: hmac-sha256 struct context
 * #2: input buffer
 * #3: input length
 */
void F_SYMBOL(hmac_sha256)(struct hmac_sha256_ctx *ctx, const uint8_t *s,
		size_t len)
{
	F_SYMBOL(hmac_sha256_process)(ctx, s, len);
	F_SYMBOL(hmac_sha256_finish)(ctx, len);
}

/* @func: hmac_sha224_init
 * #desc:
 *    hmac-sha224 initialize the hmac-sha256 struct context.
 *
 * #1: hmac-sha256 struct context
 * #2: input key
 * #3: key length
 */
void F_SYMBOL(hmac_sha224_init)(struct hmac_sha256_ctx *ctx,
		const uint8_t *key, uint32_t key_len)
{
	SHA224_NEW(sha224_ctx);
	F_SYMBOL(sha224_init)(&sha224_ctx);

	if (key_len > SHA256_BLOCKSIZE) {
		F_SYMBOL(sha256)(&sha224_ctx, key, key_len);
		key = &(SHA224_STATE(&sha224_ctx, 0));
		key_len = SHA224_LEN;
	}

	C_SYMBOL(memset)(ctx->ipad, 0x36, SHA256_BLOCKSIZE);
	C_SYMBOL(memset)(ctx->opad, 0x5c, SHA256_BLOCKSIZE);
	for (uint32_t i = 0; i < key_len; i++) {
		ctx->ipad[i] ^= key[i];
		ctx->opad[i] ^= key[i];
	}

	F_SYMBOL(sha224_init)(&ctx->ipad_ctx);
	F_SYMBOL(sha224_init)(&ctx->opad_ctx);
	F_SYMBOL(sha256_process)(&ctx->ipad_ctx, ctx->ipad,
		SHA256_BLOCKSIZE);

	ctx->state = (uint8_t *)&(SHA224_STATE(&ctx->opad_ctx, 0));
}

/* @func: hmac_sha224_process
 * #desc:
 *    hmac-sha224 processing buffer function.
 *
 * #1: hmac-sha256 struct context
 * #2: input buffer
 * #3: input length
 */
void F_SYMBOL(hmac_sha224_process)(struct hmac_sha256_ctx *ctx,
		const uint8_t *s, size_t len)
{
	F_SYMBOL(sha256_process)(&ctx->ipad_ctx, s, len);
}

/* @func: hmac_sha224_finish
 * #desc:
 *    hmac-sha224 process the remaining bytes in the buffer and end.
 *
 * #1: hmac-sha256 struct context
 * #2: total length
 */
void F_SYMBOL(hmac_sha224_finish)(struct hmac_sha256_ctx *ctx,
		uint64_t len)
{
	F_SYMBOL(sha256_finish)(&ctx->ipad_ctx, len + SHA256_BLOCKSIZE);

	F_SYMBOL(sha256_process)(&ctx->opad_ctx, ctx->opad,
		SHA256_BLOCKSIZE);
	F_SYMBOL(sha256_process)(&ctx->opad_ctx,
		&(SHA224_STATE(&ctx->ipad_ctx, 0)),
		SHA224_LEN);
	F_SYMBOL(sha256_finish)(&ctx->opad_ctx,
		SHA256_BLOCKSIZE + SHA224_LEN);
}

/* @func: hmac_sha224
 * #desc:
 *    hmac-sha224 single-time processing function.
 *
 * #1: hmac-sha256 struct context
 * #2: input buffer
 * #3: input length
 */
void F_SYMBOL(hmac_sha224)(struct hmac_sha256_ctx *ctx, const uint8_t *s,
		size_t len)
{
	F_SYMBOL(hmac_sha256_process)(ctx, s, len);
	F_SYMBOL(hmac_sha256_finish)(ctx, len);
}
