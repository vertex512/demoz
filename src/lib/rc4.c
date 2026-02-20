/* @file: rc4.c
 * #desc:
 *    The implementations of rivest cipher 4 encryption algorithm.
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
#include <demoz/c/stdint.h>
#include <demoz/lib/rc4.h>


/* @func: rc4_init
 * #desc:
 *    rc4 initialization function.
 *
 * #1: ctx [out] rc4 struct context
 * #2: key [in]  input key
 * #3: len [in]  key length (max-length: 256)
 */
void F_SYMBOL(rc4_init)(struct rc4_ctx *ctx, const uint8_t *key, uint32_t len) {
	for (int32_t i = 0; i < 256; i++)
		ctx->s[i] = i;

	for (uint32_t i = 0, j = 0; i < 256; i++) {
		j = (j + ctx->s[i] + key[i % len]) & 0xff;

		uint8_t t = ctx->s[i];
		ctx->s[i] = ctx->s[j];
		ctx->s[j] = t;
	}

	ctx->i = ctx->j = 0;
}

/* @func: rc4_crypto
 * #desc:
 *    rc4 stream encryption function.
 *
 * #1: ctx [in/out] rc4 struct context
 * #2: buf [in/out] buffer
 * #3: len [in]     length
 */
void F_SYMBOL(rc4_crypto)(struct rc4_ctx *ctx, uint8_t *buf, uint32_t len) {
	int32_t i = ctx->i, j = ctx->j;

	for (uint32_t k = 0; k < len; k++) {
		i = (i + 1) & 0xff;
		j = (j + ctx->s[i]) & 0xff;

		uint8_t t = ctx->s[i];
		ctx->s[i] = ctx->s[j];
		ctx->s[j] = t;

		buf[k] ^= ctx->s[(ctx->s[i] + ctx->s[j]) & 0xff];
	}

	ctx->i = i;
	ctx->j = j;
}
