/* @file: xxhash32.c
 * #desc:
 *    The implementations of extremely fast hash algorithm (32bit).
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
#include <demoz/lib/xxhash.h>


/* @def: _ */
#define PRIME32_1 0x9e3779b1
#define PRIME32_2 0x85ebca77
#define PRIME32_3 0xc2b2ae3d
#define PRIME32_4 0x27d4eb2f
#define PRIME32_5 0x165667b1

#define ROTL32(x, n) (((x) << (n)) | ((x) >> (32 - (n))))
/* end */

/* @func: _xxhash32_compress (static)
 * #desc:
 *    xxh32 compress function.
 *
 * #1: xxh32 struct context
 * #2: input block (length: XXHASH32_BLOCKSIZE)
 */
static void _xxhash32_compress(struct xxhash32_ctx *ctx, const uint8_t *s)
{
	uint32_t m[4], a, b, c, d;
	a = ctx->state[0];
	b = ctx->state[1];
	c = ctx->state[2];
	d = ctx->state[3];

	for (int32_t i = 0; i < 4; i++) {
		m[i] = (uint32_t)s[0]
			| (uint32_t)s[1] << 8
			| (uint32_t)s[2] << 16
			| (uint32_t)s[3] << 24;
		s += 4;
	}

	/* round */
	a += m[0] * PRIME32_2;
	a = ROTL32(a, 13) * PRIME32_1;
	b += m[1] * PRIME32_2;
	b = ROTL32(b, 13) * PRIME32_1;
	c += m[2] * PRIME32_2;
	c = ROTL32(c, 13) * PRIME32_1;
	d += m[3] * PRIME32_2;
	d = ROTL32(d, 13) * PRIME32_1;

	ctx->state[0] = a;
	ctx->state[1] = b;
	ctx->state[2] = c;
	ctx->state[3] = d;
}

/* @func: xxhash32_init
 * #desc:
 *    xxh32 struct context initialization.
 *
 * #1: xxh32 struct context
 * #2: init seed (default: 0)
 */
void F_SYMBOL(xxhash32_init)(struct xxhash32_ctx *ctx, uint32_t seed)
{
	ctx->seed = seed;
	ctx->state[0] = seed + PRIME32_1 + PRIME32_2;
	ctx->state[1] = seed + PRIME32_2;
	ctx->state[2] = seed;
	ctx->state[3] = seed - PRIME32_1;
	ctx->count = 0;
}

/* @func: xxhash32_process
 * #desc:
 *    xxh32 processing buffer function.
 *
 * #1: xxh32 struct context
 * #2: input buffer
 * #3: input length
 */
void F_SYMBOL(xxhash32_process)(struct xxhash32_ctx *ctx, const uint8_t *s,
		size_t len)
{
#define BLOCKSIZE XXHASH32_BLOCKSIZE

	size_t n = ctx->count, h;
	switch (n) {
		default:
			h = BLOCKSIZE - n;
			h = (len < h) ? len : h;
			C_SYMBOL(memcpy)(ctx->buf + n, s, h);
			n += h;
			if (n != BLOCKSIZE)
				break;

			/* processing */
			_xxhash32_compress(ctx, ctx->buf);
			n = 0;
			s += h;
			len -= h;
		case 0:
			while (len >= BLOCKSIZE) {
				/* processing */
				_xxhash32_compress(ctx, s);
				s += BLOCKSIZE;
				len -= BLOCKSIZE;
			}

			n = len;
			if (n)
				C_SYMBOL(memcpy)(ctx->buf, s, n);
	}
	ctx->count = n;
}

/* @func: xxhash32_finish
 * #desc:
 *    xxh32 process the remaining bytes in the buffer and end.
 *
 * #1: xxh32 struct context
 * #2: total length
 * #r: return hash digest
 */
uint32_t F_SYMBOL(xxhash32_finish)(struct xxhash32_ctx *ctx, size_t len)
{
	uint32_t a = ctx->state[0];
	uint32_t b = ctx->state[1];
	uint32_t c = ctx->state[2];
	uint32_t d = ctx->state[3];
	uint32_t hash = 0;
	uint8_t *p = ctx->buf;

	if (len < XXHASH32_BLOCKSIZE) {
		hash = ctx->seed + PRIME32_5;
	} else {
		hash = ROTL32(a, 1) + ROTL32(b, 7)
			+ ROTL32(c, 12) + ROTL32(d, 18);
	}
	hash += (uint32_t)len;

	/* remaining processing */
	while (ctx->count > 3) {
		a = *((uint32_t *)p);
		hash += a * PRIME32_3;
		hash = ROTL32(hash, 17) * PRIME32_4;
		p += 4;
		ctx->count -= 4;
	}
	while (ctx->count--) {
		a = *p++;
		hash += a * PRIME32_5;
		hash = ROTL32(hash, 11) * PRIME32_1;
	}

	/* final mix */
	hash ^= hash >> 15;
	hash *= PRIME32_2;
	hash ^= hash >> 13;
	hash *= PRIME32_3;
	hash ^= hash >> 16;

	return hash;
}

/* @func: xxhash32
 * #desc:
 *    xxh32 single-time processing function.
 *
 * #1: xxh32 struct context
 * #2: input buffer
 * #3: input length
 * #r: return hash digest
 */
uint32_t F_SYMBOL(xxhash32)(struct xxhash32_ctx *ctx, const uint8_t *s,
		size_t len)
{
	F_SYMBOL(xxhash32_process)(ctx, s, len);
	return F_SYMBOL(xxhash32_finish)(ctx, len);
}
