/* @file: bits_add.c
 * #desc:
 *    The implementations of adding bits to buffer.
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
#include <demoz/ds/bits_add.h>


/* @func: bits_add
 * #desc:
 *    add bits to the buffer.  flush the buffer when it is full.
 *    if there are any remaining bits, keep it as is, then add it again.
 *
 * #1: ctx  [in/out] bits-add struct context
 * #2: v    [in]     bits value
 * #3: bits [in]     bits length
 * #r:      [ret]    0: not full, 1: buffer full
 */
int32_t F_SYMBOL(bits_add)(struct bits_add_ctx *ctx, uint32_t v, uint32_t bits)
{
	uint32_t n = ctx->size;
	uint32_t cur = ctx->cur;
	uint32_t rem = ctx->rem;

	if (n == BITS_ADD_BUFSIZE)
		return 1;

	if (rem) {
		v >>= bits - rem;
		bits = rem;
	}

	static uint8_t mask[9] = {
		0x00,
		0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff
		};

#undef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

	while (n != BITS_ADD_BUFSIZE && bits) {
		rem = MIN(8 - cur, bits);
		ctx->buf[n] &= mask[cur];
		ctx->buf[n] |= (v & mask[rem]) << cur;

		cur += rem;
		if (cur == 8) {
			cur = 0;
			n++;
		}

		v >>= rem;
		bits -= rem;
	}

	ctx->size = n;
	ctx->cur = cur;
	ctx->rem = bits;

	return n == BITS_ADD_BUFSIZE;
}
