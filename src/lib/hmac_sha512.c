/* @file: hmac_sha512.c
 * #desc:
 *    The implementations of hash message authentication code (sha512).
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


/* @func: hmac_sha512_init
 * #desc:
 *    hmac-sha512 struct context initialization.
 *
 * #1: ctx     [out] hmac-sha512 struct context
 * #2: key     [in]  input key
 * #3: key_len [in]  key length
 */
void F_SYMBOL(hmac_sha512_init)(struct hmac_sha512_ctx *ctx,
		const uint8_t *key, uint32_t key_len)
{
	SHA512_NEW(sha512_ctx);
	F_SYMBOL(sha512_init)(&sha512_ctx);

	if (key_len > SHA512_BLOCKSIZE) {
		F_SYMBOL(sha512)(&sha512_ctx, key, key_len);
		key = &(SHA512_STATE(&sha512_ctx, 0));
		key_len = SHA512_LEN;
	}

	C_SYMBOL(memset)(ctx->ipad, 0x36, SHA512_BLOCKSIZE);
	C_SYMBOL(memset)(ctx->opad, 0x5c, SHA512_BLOCKSIZE);
	for (uint32_t i = 0; i < key_len; i++) {
		ctx->ipad[i] ^= key[i];
		ctx->opad[i] ^= key[i];
	}

	F_SYMBOL(sha512_init)(&ctx->ipad_ctx);
	F_SYMBOL(sha512_init)(&ctx->opad_ctx);
	F_SYMBOL(sha512_process)(&ctx->ipad_ctx, ctx->ipad,
		SHA512_BLOCKSIZE);

	ctx->state = &(SHA512_STATE(&ctx->opad_ctx, 0));
}

/* @func: hmac_sha512_process
 * #desc:
 *    hmac-sha512 processing buffer function.
 *
 * #1: ctx [in/out] hmac-sha512 struct context
 * #2: s   [in]     input buffer
 * #3: len [in]     input length
 */
void F_SYMBOL(hmac_sha512_process)(struct hmac_sha512_ctx *ctx,
		const uint8_t *s, size_t len)
{
	F_SYMBOL(sha512_process)(&ctx->ipad_ctx, s, len);
}

/* @func: hmac_sha512_finish
 * #desc:
 *    hmac-sha512 process the remaining bytes in the buffer and end.
 *
 * #1: ctx [in/out] hmac-sha512 struct context
 * #2: len [in]     total length
 */
void F_SYMBOL(hmac_sha512_finish)(struct hmac_sha512_ctx *ctx,
		uint64_t len)
{
	F_SYMBOL(sha512_finish)(&ctx->ipad_ctx, len + SHA512_BLOCKSIZE);

	F_SYMBOL(sha512_process)(&ctx->opad_ctx, ctx->opad,
		SHA512_BLOCKSIZE);
	F_SYMBOL(sha512_process)(&ctx->opad_ctx,
		&(SHA512_STATE(&ctx->ipad_ctx, 0)),
		SHA512_LEN);
	F_SYMBOL(sha512_finish)(&ctx->opad_ctx,
		SHA512_BLOCKSIZE + SHA512_LEN);
}

/* @func: hmac_sha512
 * #desc:
 *    hmac-sha512 single-time processing function.
 *
 * #1: ctx [in/out] hmac-sha512 struct context
 * #2: s   [in]     input buffer
 * #3: len [in]     input length
 */
void F_SYMBOL(hmac_sha512)(struct hmac_sha512_ctx *ctx, const uint8_t *s,
		size_t len)
{
	F_SYMBOL(hmac_sha512_process)(ctx, s, len);
	F_SYMBOL(hmac_sha512_finish)(ctx, len);
}

/* @func: hmac_sha384_init
 * #desc:
 *    hmac-sha384 initialize the hmac-sha512 struct context.
 *
 * #1: ctx     [out] hmac-sha512 struct context
 * #2: key     [in]  input key
 * #3: key_len [in]  key length
 */
void F_SYMBOL(hmac_sha384_init)(struct hmac_sha512_ctx *ctx,
		const uint8_t *key, uint32_t key_len)
{
	SHA384_NEW(sha384_ctx);
	F_SYMBOL(sha384_init)(&sha384_ctx);

	if (key_len > SHA512_BLOCKSIZE) {
		F_SYMBOL(sha512)(&sha384_ctx, key, key_len);
		key = &(SHA384_STATE(&sha384_ctx, 0));
		key_len = SHA384_LEN;
	}

	C_SYMBOL(memset)(ctx->ipad, 0x36, SHA512_BLOCKSIZE);
	C_SYMBOL(memset)(ctx->opad, 0x5c, SHA512_BLOCKSIZE);
	for (uint32_t i = 0; i < key_len; i++) {
		ctx->ipad[i] ^= key[i];
		ctx->opad[i] ^= key[i];
	}

	F_SYMBOL(sha384_init)(&ctx->ipad_ctx);
	F_SYMBOL(sha384_init)(&ctx->opad_ctx);
	F_SYMBOL(sha512_process)(&ctx->ipad_ctx, ctx->ipad,
		SHA512_BLOCKSIZE);

	ctx->state = (uint8_t *)&(SHA384_STATE(&ctx->opad_ctx, 0));
}

/* @func: hmac_sha384_process
 * #desc:
 *    hmac-sha384 processing buffer function.
 *
 * #1: ctx [in/out] hmac-sha512 struct context
 * #2: s   [in]     input buffer
 * #3: len [in]     input length
 */
void F_SYMBOL(hmac_sha384_process)(struct hmac_sha512_ctx *ctx,
		const uint8_t *s, size_t len)
{
	F_SYMBOL(sha512_process)(&ctx->ipad_ctx, s, len);
}

/* @func: hmac_sha384_finish
 * #desc:
 *    hmac-sha384 process the remaining bytes in the buffer and end.
 *
 * #1: ctx [in/out] hmac-sha512 struct context
 * #2: len [in]     total length
 */
void F_SYMBOL(hmac_sha384_finish)(struct hmac_sha512_ctx *ctx,
		uint64_t len)
{
	F_SYMBOL(sha512_finish)(&ctx->ipad_ctx, len + SHA512_BLOCKSIZE);

	F_SYMBOL(sha512_process)(&ctx->opad_ctx, ctx->opad,
		SHA512_BLOCKSIZE);
	F_SYMBOL(sha512_process)(&ctx->opad_ctx,
		&(SHA384_STATE(&ctx->ipad_ctx, 0)),
		SHA384_LEN);
	F_SYMBOL(sha512_finish)(&ctx->opad_ctx,
		SHA512_BLOCKSIZE + SHA384_LEN);
}

/* @func: hmac_sha384
 * #desc:
 *    hmac-sha384 single-time processing function.
 *
 * #1: ctx [in/out] hmac-sha512 struct context
 * #2: s   [in]     input buffer
 * #3: len [in]     input length
 */
void F_SYMBOL(hmac_sha384)(struct hmac_sha512_ctx *ctx, const uint8_t *s,
		size_t len)
{
	F_SYMBOL(hmac_sha512_process)(ctx, s, len);
	F_SYMBOL(hmac_sha512_finish)(ctx, len);
}
