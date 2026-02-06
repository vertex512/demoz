/* @file: hmac_sha3.c
 * #desc:
 *    The implementations of hash message authentication code (sha3).
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
#include <demoz/lib/sha3.h>
#include <demoz/lib/hmac.h>


/* @func: hmac_sha3_init
 * #desc:
 *    hmac-sha3 struct context initialization.
 *
 * #1: hmac-sha3 struct context
 * #2: input key
 * #3: key length
 * #4: digest type
 * #r: 0: no error, -1: type or init error
 */
int32_t F_SYMBOL(hmac_sha3_init)(struct hmac_sha3_ctx *ctx,
		const uint8_t *key, uint32_t key_len, int32_t type)
{
	SHA3_NEW(sha3_ctx);

	switch (type) {
		case SHA3_224_TYPE: case SHA3_256_TYPE:
		case SHA3_384_TYPE: case SHA3_512_TYPE:
			break;
		default:
			return -1;
	}
	if (F_SYMBOL(sha3_init)(&sha3_ctx, type, 0))
		return -1;

	if (key_len > SHA3_GETRATE(&sha3_ctx)) {
		F_SYMBOL(sha3)(&sha3_ctx, key, key_len);
		key = &(SHA3_STATE(&sha3_ctx, 0));
		key_len = SHA3_GETDSIZE(&sha3_ctx);
	}

	C_SYMBOL(memset)(ctx->ipad, 0x36, SHA3_STATE_SIZE);
	C_SYMBOL(memset)(ctx->opad, 0x5c, SHA3_STATE_SIZE);
	for (uint32_t i = 0; i < key_len; i++) {
		ctx->ipad[i] ^= key[i];
		ctx->opad[i] ^= key[i];
	}

	if (F_SYMBOL(sha3_init)(&ctx->ipad_ctx, type, 0))
		return -1;
	if (F_SYMBOL(sha3_init)(&ctx->opad_ctx, type, 0))
		return -1;

	F_SYMBOL(sha3_process)(&ctx->ipad_ctx, ctx->ipad,
		SHA3_GETRATE(&ctx->ipad_ctx));

	ctx->state = &(SHA3_STATE(&ctx->opad_ctx, 0));

	return 0;
}

/* @func: hmac_sha3_process
 * #desc:
 *    hmac-sha3 processing buffer function.
 *
 * #1: hmac-sha3 struct context
 * #2: input buffer
 * #3: input length
 */
void F_SYMBOL(hmac_sha3_process)(struct hmac_sha3_ctx *ctx, const uint8_t *s,
		size_t len)
{
	F_SYMBOL(sha3_process)(&ctx->ipad_ctx, s, len);
}

/* @func: hmac_sha3_finish
 * #desc:
 *    hmac-sha3 process the remaining bytes in the buffer and end.
 *
 * #1: hmac-sha3 struct context
 */
void F_SYMBOL(hmac_sha3_finish)(struct hmac_sha3_ctx *ctx)
{
	F_SYMBOL(sha3_finish)(&ctx->ipad_ctx);

	F_SYMBOL(sha3_process)(&ctx->opad_ctx, ctx->opad,
		SHA3_GETRATE(&ctx->opad_ctx));
	F_SYMBOL(sha3_process)(&ctx->opad_ctx,
		&(SHA3_STATE(&ctx->ipad_ctx, 0)),
		SHA3_GETDSIZE(&ctx->ipad_ctx));
	F_SYMBOL(sha3_finish)(&ctx->opad_ctx);
}

/* @func: hmac_sha3
 * #desc:
 *    hmac-sha3 single-time processing function.
 *
 * #1: hmac-sha3 struct context
 * #2: input buffer
 * #3: input length
 */
void F_SYMBOL(hmac_sha3)(struct hmac_sha3_ctx *ctx, const uint8_t *s,
		size_t len)
{
	F_SYMBOL(hmac_sha3_process)(ctx, s, len);
	F_SYMBOL(hmac_sha3_finish)(ctx);
}
