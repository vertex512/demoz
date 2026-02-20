/* @file: hmac_blake2b.c
 * #desc:
 *    The implementations of hash message authentication code (blake2b).
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
#include <demoz/lib/blake2.h>
#include <demoz/lib/hmac.h>


/* @func: hmac_blake2b_init
 * #desc:
 *    hmac-blake2b struct context initialization.
 *
 * #1: ctx     [out] hmac-blake2b struct context
 * #2: key     [in]  input key
 * #3: key_len [in]  key length
 */
void F_SYMBOL(hmac_blake2b_init)(struct hmac_blake2b_ctx *ctx,
		const uint8_t *key, uint32_t key_len)
{
	BLAKE2B_NEW(blake2b_ctx);
	F_SYMBOL(blake2b_init)(&blake2b_ctx, BLAKE2B_512_LEN);

	if (key_len > BLAKE2B_BLOCKSIZE) {
		F_SYMBOL(blake2b)(&blake2b_ctx, key, key_len);
		key = &(BLAKE2B_STATE(&blake2b_ctx, 0));
		key_len = BLAKE2B_512_LEN;
	}

	C_SYMBOL(memset)(ctx->ipad, 0x36, BLAKE2B_BLOCKSIZE);
	C_SYMBOL(memset)(ctx->opad, 0x5c, BLAKE2B_BLOCKSIZE);
	for (uint32_t i = 0; i < key_len; i++) {
		ctx->ipad[i] ^= key[i];
		ctx->opad[i] ^= key[i];
	}

	F_SYMBOL(blake2b_init)(&ctx->ipad_ctx, BLAKE2B_512_LEN);
	F_SYMBOL(blake2b_init)(&ctx->opad_ctx, BLAKE2B_512_LEN);
	F_SYMBOL(blake2b_process)(&ctx->ipad_ctx, ctx->ipad,
		BLAKE2B_BLOCKSIZE);

	ctx->state = &(BLAKE2B_STATE(&ctx->opad_ctx, 0));
}

/* @func: hmac_blake2b_process
 * #desc:
 *    hmac-blake2b processing buffer function.
 *
 * #1: ctx [in/out] hmac-blake2b struct context
 * #2: s   [in]     input buffer
 * #3: len [in]     input length
 */
void F_SYMBOL(hmac_blake2b_process)(struct hmac_blake2b_ctx *ctx,
		const uint8_t *s, size_t len)
{
	F_SYMBOL(blake2b_process)(&ctx->ipad_ctx, s, len);
}

/* @func: hmac_blake2b_finish
 * #desc:
 *    hmac-blake2b process the remaining bytes in the buffer and end.
 *
 * #1: ctx [in/out] hmac-blake2b struct context
 */
void F_SYMBOL(hmac_blake2b_finish)(struct hmac_blake2b_ctx *ctx)
{
	F_SYMBOL(blake2b_finish)(&ctx->ipad_ctx);

	F_SYMBOL(blake2b_process)(&ctx->opad_ctx, ctx->opad,
		BLAKE2B_BLOCKSIZE);
	F_SYMBOL(blake2b_process)(&ctx->opad_ctx,
		&(BLAKE2B_STATE(&ctx->ipad_ctx, 0)),
		BLAKE2B_512_LEN);
	F_SYMBOL(blake2b_finish)(&ctx->opad_ctx);
}

/* @func: hmac_blake2b
 * #desc:
 *    hmac-blake2b single-time processing function.
 *
 * #1: ctx [in/out] hmac-blake2b struct context
 * #2: s   [in]     input buffer
 * #3: len [in]     input length
 */
void F_SYMBOL(hmac_blake2b)(struct hmac_blake2b_ctx *ctx, const uint8_t *s,
		size_t len)
{
	F_SYMBOL(hmac_blake2b_process)(ctx, s, len);
	F_SYMBOL(hmac_blake2b_finish)(ctx);
}
