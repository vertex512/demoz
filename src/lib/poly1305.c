/* @file: poly1305.c
 * #desc:
 *    The implementations of poly1305 message authentication code.
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
#include <demoz/lib/poly1305.h>


/* @def: _
 * P = 2^130 - 5
 * h = ((h + m) * r) % P
 */

#define PACK4(x) ((uint32_t)((x)[0]) | (uint32_t)((x)[1]) << 8 \
	| (uint32_t)((x)[2]) << 16 | (uint32_t)((x)[3]) << 24)
/* end */

/* @func: poly1305_init
 * #desc:
 *    poly1305 initialization function.
 *
 * #1: poly1305 struct context
 * #2: input key (length: POLY1305_KEY_LEN)
 */
void F_SYMBOL(poly1305_init)(struct poly1305_ctx *ctx, const uint8_t *key)
{
	ctx->r[0] = PACK4(key) & 0x3ffffff;
	ctx->r[1] = (PACK4(key + 3) >> 2) & 0x3ffff03;
	ctx->r[2] = (PACK4(key + 6) >> 4) & 0x3ffc0ff;
	ctx->r[3] = (PACK4(key + 9) >> 6) & 0x3f03fff;
	ctx->r[4] = (PACK4(key + 12) >> 8) & 0x00fffff;

	ctx->s[0] = PACK4(key + 16);
	ctx->s[1] = PACK4(key + 20);
	ctx->s[2] = PACK4(key + 24);
	ctx->s[3] = PACK4(key + 28);

	C_SYMBOL(memset)(ctx->h, 0, sizeof(ctx->h));
	ctx->count = 0;
}

/* @func: poly1305_block
 * #desc:
 *    poly1305 block processing function.
 *
 * #1: poly1305 struct context
 * #2: input block (length: POLY1305_BLOCKSIZE)
 * #3: padding bit
 */
void F_SYMBOL(poly1305_block)(struct poly1305_ctx *ctx, const uint8_t *s,
		uint32_t padbit)
{
	uint32_t h[5], r[5], w[5];
	uint64_t d[5];

	h[0] = ctx->h[0];
	h[1] = ctx->h[1];
	h[2] = ctx->h[2];
	h[3] = ctx->h[3];
	h[4] = ctx->h[4];

	r[0] = ctx->r[0];
	r[1] = ctx->r[1];
	r[2] = ctx->r[2];
	r[3] = ctx->r[3];
	r[4] = ctx->r[4];

	w[0] = r[0] * 5;
	w[1] = r[1] * 5;
	w[2] = r[2] * 5;
	w[3] = r[3] * 5;
	w[4] = r[4] * 5;

	/* h = h + m */
	h[0] += PACK4(s) & 0x3ffffff;
	h[1] += (PACK4(s + 3) >> 2) & 0x3ffffff;
	h[2] += (PACK4(s + 6) >> 4) & 0x3ffffff;
	h[3] += (PACK4(s + 9) >> 6) & 0x3ffffff;
	h[4] += (PACK4(s + 12) >> 8) | (padbit << 24);

	/* d = h * r */
	d[0] = ((uint64_t)h[0] * r[0])
		+ ((uint64_t)h[1] * w[4])
		+ ((uint64_t)h[2] * w[3])
		+ ((uint64_t)h[3] * w[2])
		+ ((uint64_t)h[4] * w[1]);
	d[1] = (d[0] >> 26)
		+ ((uint64_t)h[0] * r[1])
		+ ((uint64_t)h[1] * r[0])
		+ ((uint64_t)h[2] * w[4])
		+ ((uint64_t)h[3] * w[3])
		+ ((uint64_t)h[4] * w[2]);
	d[2] = (d[1] >> 26)
		+ ((uint64_t)h[0] * r[2])
		+ ((uint64_t)h[1] * r[1])
		+ ((uint64_t)h[2] * r[0])
		+ ((uint64_t)h[3] * w[4])
		+ ((uint64_t)h[4] * w[3]);
	d[3] = (d[2] >> 26)
		+ ((uint64_t)h[0] * r[3])
		+ ((uint64_t)h[1] * r[2])
		+ ((uint64_t)h[2] * r[1])
		+ ((uint64_t)h[3] * r[0])
		+ ((uint64_t)h[4] * w[4]);
	d[4] = (d[3] >> 26)
		+ ((uint64_t)h[0] * r[4])
		+ ((uint64_t)h[1] * r[3])
		+ ((uint64_t)h[2] * r[2])
		+ ((uint64_t)h[3] * r[1])
		+ ((uint64_t)h[4] * r[0]);

	/* h = d % p modular reduction */
	h[0] = (uint32_t)d[0] & 0x3ffffff;
	h[1] = (uint32_t)d[1] & 0x3ffffff;
	h[2] = (uint32_t)d[2] & 0x3ffffff;
	h[3] = (uint32_t)d[3] & 0x3ffffff;
	h[4] = (uint32_t)d[4] & 0x3ffffff;

	h[0] += (uint32_t)(d[4] >> 26) * 5;
	h[1] += h[0] >> 26;
	h[0] = h[0] & 0x3ffffff;

	ctx->h[0] = h[0];
	ctx->h[1] = h[1];
	ctx->h[2] = h[2];
	ctx->h[3] = h[3];
	ctx->h[4] = h[4];
}

/* @func: poly1305_tag
 * #desc:
 *    output poly1305 authentication tag.
 *
 * #1: poly1305 struct context
 * #2: result tag (length: POLY1305_TAG_LEN)
 */
void F_SYMBOL(poly1305_tag)(struct poly1305_ctx *ctx, uint8_t *tag)
{
	uint32_t h[5], t[5];

	h[0] = ctx->h[0];
	h[1] = ctx->h[1];
	h[2] = ctx->h[2];
	h[3] = ctx->h[3];
	h[4] = ctx->h[4];

	/* h = h % p modular reduction */
	h[2] += h[1] >> 26;
	h[1] &= 0x3ffffff;
	h[3] += h[2] >> 26;
	h[2] &= 0x3ffffff;
	h[4] += h[3] >> 26;
	h[3] &= 0x3ffffff;

	h[0] += 5 * (h[4] >> 26);
	h[4] &= 0x3ffffff;
	h[1] += h[0] >> 26;
	h[0] &= 0x3ffffff;

	/* t = h - p */
	t[0] = h[0] + 5;
	t[1] = h[1] + (t[0] >> 26);
	t[2] = h[2] + (t[1] >> 26);
	t[3] = h[3] + (t[2] >> 26);
	t[4] = h[4] + (t[3] >> 26) - (1 << 26);
	t[0] &= 0x3ffffff;
	t[1] &= 0x3ffffff;
	t[2] &= 0x3ffffff;
	t[3] &= 0x3ffffff;

	/* swap */
	uint32_t t_mask = (t[4] >> 31) - 1;
	uint32_t h_mask = ~t_mask;
	h[0] = (h[0] & h_mask) | (t[0] & t_mask);
	h[1] = (h[1] & h_mask) | (t[1] & t_mask);
	h[2] = (h[2] & h_mask) | (t[2] & t_mask);
	h[3] = (h[3] & h_mask) | (t[3] & t_mask);
	h[4] = (h[4] & h_mask) | (t[4] & t_mask);

	/* merge limbs */
	h[0] |= h[1] << 26;
	h[1] = (h[1] >> 6) | (h[2] << 20);
	h[2] = (h[2] >> 12) | (h[3] << 14);
	h[3] = (h[3] >> 18) | (h[4] << 8);

	/* h = h + s */
	uint64_t tt = (uint64_t)h[0] + ctx->s[0];
	h[0] = (uint32_t)tt;
	tt = (uint64_t)h[1] + ctx->s[1] + (tt >> 32);
	h[1] = (uint32_t)tt;
	tt = (uint64_t)h[2] + ctx->s[2] + (tt >> 32);
	h[2] = (uint32_t)tt;
	tt = (uint64_t)h[3] + ctx->s[3] + (tt >> 32);
	h[3] = (uint32_t)tt;

	C_SYMBOL(memcpy)(tag, h, POLY1305_TAG_LEN);
}

/* @func: poly1305_tag_auth
 * #desc:
 *    poly1305 tag comparison is equal.
 *
 * #1: tag
 * #2: tag
 * #r: 0: a != b, 1: a == b
 */
int32_t F_SYMBOL(poly1305_tag_auth)(const uint8_t *a, const uint8_t *b)
{
	uint32_t r = 0;
	for (int32_t i = 0; i < POLY1305_TAG_LEN; i++)
		r |= a[i] ^ b[i];

	return ((r - 1) >> 31) & 1;
}

/* @func: poly1305_process
 * #desc:
 *    poly1305 processing buffer function.
 *
 * #1: poly1305 struct context
 * #2: input buffer
 * #3: input length
 */
void F_SYMBOL(poly1305_process)(struct poly1305_ctx *ctx, const uint8_t *s,
		size_t len)
{
#define BLOCKSIZE POLY1305_BLOCKSIZE

	size_t n = ctx->count, h;
	switch (n) {
		default:
			h = BLOCKSIZE - n;
			h = (len < h) ? len : h;
			C_SYMBOL(memcpy)(ctx->u.buf + n, s, h);
			n += h;
			if (n != BLOCKSIZE)
				break;

			/* processing */
			F_SYMBOL(poly1305_block)(ctx, ctx->u.buf, 1);
			n = 0;
			s += h;
			len -= h;
		case 0:
			while (len >= BLOCKSIZE) {
				/* processing */
				F_SYMBOL(poly1305_block)(ctx, s, 1);
				s += BLOCKSIZE;
				len -= BLOCKSIZE;
			}

			n = len;
			if (n)
				C_SYMBOL(memcpy)(ctx->u.buf, s, n);
	}
	ctx->count = n;
}

/* @func: poly1305_finish
 * #desc:
 *    poly1305 process the remaining bytes in the buffer and end.
 *
 * #1: poly1305 struct context
 */
void F_SYMBOL(poly1305_finish)(struct poly1305_ctx *ctx)
{
	if (ctx->count) {
		C_SYMBOL(memset)(ctx->u.buf + ctx->count, 0,
			POLY1305_BLOCKSIZE - ctx->count);
		ctx->u.buf[ctx->count] = 0x01;
		F_SYMBOL(poly1305_block)(ctx, ctx->u.buf, 0);
	}

	F_SYMBOL(poly1305_tag)(ctx, ctx->u.tag);
}

/* @func: poly1305
 * #desc:
 *    poly1305 single-time processing function.
 *
 * #1: poly1305 struct context
 * #2: input buffer
 * #3: input length
 */
void F_SYMBOL(poly1305)(struct poly1305_ctx *ctx, const uint8_t *s,
		size_t len)
{
	F_SYMBOL(poly1305_process)(ctx, s, len);
	F_SYMBOL(poly1305_finish)(ctx);
}
