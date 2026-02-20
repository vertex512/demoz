/* @file: xxhash64.c
 * #desc:
 *    The implementations of extremely fast hash algorithm (64bit).
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
#define PRIME64_1 0x9e3779b185ebca87ULL
#define PRIME64_2 0xc2b2ae3d27d4eb4fULL
#define PRIME64_3 0x165667b19e3779f9ULL
#define PRIME64_4 0x85ebca77c2b2ae63ULL
#define PRIME64_5 0x27d4eb2f165667c5ULL

#define ROTL64(x, n) (((x) << (n)) | ((x) >> (64 - (n))))
/* end */

/* @func: _xxhash64_compress (static)
 * #desc:
 *    xxh64 compress function.
 *
 * #1: ctx [in/out] xxh64 struct context
 * #2: s   [in]     input block (length: XXHASH64_BLOCKSIZE)
 */
static void _xxhash64_compress(struct xxhash64_ctx *ctx, const uint8_t *s)
{
	uint64_t m[4], a, b, c, d;
	a = ctx->state[0];
	b = ctx->state[1];
	c = ctx->state[2];
	d = ctx->state[3];

	for (int32_t i = 0; i < 4; i++) {
		m[i] = (uint64_t)s[0]
			| (uint64_t)s[1] << 8
			| (uint64_t)s[2] << 16
			| (uint64_t)s[3] << 24
			| (uint64_t)s[4] << 32
			| (uint64_t)s[5] << 40
			| (uint64_t)s[6] << 48
			| (uint64_t)s[7] << 56;
		s += 8;
	}

	/* round */
	a += m[0] * PRIME64_2;
	a = ROTL64(a, 31) * PRIME64_1;
	b += m[1] * PRIME64_2;
	b = ROTL64(b, 31) * PRIME64_1;
	c += m[2] * PRIME64_2;
	c = ROTL64(c, 31) * PRIME64_1;
	d += m[3] * PRIME64_2;
	d = ROTL64(d, 31) * PRIME64_1;

	ctx->state[0] = a;
	ctx->state[1] = b;
	ctx->state[2] = c;
	ctx->state[3] = d;
}

/* @func: xxhash64_init
 * #desc:
 *    xxh64 struct context initialization.
 *
 * #1: ctx  [out] xxh64 struct context
 * #2: seed [in]  init seed (default: 0)
 */
void F_SYMBOL(xxhash64_init)(struct xxhash64_ctx *ctx, uint64_t seed)
{
	ctx->seed = seed;
	ctx->state[0] = seed + PRIME64_1 + PRIME64_2;
	ctx->state[1] = seed + PRIME64_2;
	ctx->state[2] = seed;
	ctx->state[3] = seed - PRIME64_1;
	ctx->count = 0;
}

/* @func: xxhash64_process
 * #desc:
 *    xxh64 processing buffer function.
 *
 * #1: ctx [in/out] xxh64 struct context
 * #2: s   [in]     input buffer
 * #3: len [in]     input length
 */
void F_SYMBOL(xxhash64_process)(struct xxhash64_ctx *ctx, const uint8_t *s,
		size_t len)
{
#define BLOCKSIZE XXHASH64_BLOCKSIZE

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
			_xxhash64_compress(ctx, ctx->buf);
			n = 0;
			s += h;
			len -= h;
		case 0:
			while (len >= BLOCKSIZE) {
				/* processing */
				_xxhash64_compress(ctx, s);
				s += BLOCKSIZE;
				len -= BLOCKSIZE;
			}

			n = len;
			if (n)
				C_SYMBOL(memcpy)(ctx->buf, s, n);
	}
	ctx->count = n;
}

/* @func: xxhash64_finish
 * #desc:
 *    xxh64 process the remaining bytes in the buffer and end.
 *
 * #1: ctx [in/out] xxh64 struct context
 * #2: len [in]     total length
 * #r:     [ret]    return hash digest
 */
uint64_t F_SYMBOL(xxhash64_finish)(struct xxhash64_ctx *ctx, uint64_t len)
{
	uint64_t a = ctx->state[0];
	uint64_t b = ctx->state[1];
	uint64_t c = ctx->state[2];
	uint64_t d = ctx->state[3];
	uint64_t hash = 0;
	uint8_t *p = ctx->buf;

	if (len < XXHASH64_BLOCKSIZE) {
		hash = ctx->seed + PRIME64_5;
	} else {
		hash = ROTL64(a, 1) + ROTL64(b, 7)
			+ ROTL64(c, 12) + ROTL64(d, 18);

		/* merge round */
		a *= PRIME64_2;
		hash ^= ROTL64(a, 31) * PRIME64_1;
		hash = (hash * PRIME64_1) + PRIME64_4;

		b *= PRIME64_2;
		hash ^= ROTL64(b, 31) * PRIME64_1;
		hash = (hash * PRIME64_1) + PRIME64_4;

		c *= PRIME64_2;
		hash ^= ROTL64(c, 31) * PRIME64_1;
		hash = (hash * PRIME64_1) + PRIME64_4;

		d *= PRIME64_2;
		hash ^= ROTL64(d, 31) * PRIME64_1;
		hash = (hash * PRIME64_1) + PRIME64_4;
	}
	hash += len;

	/* remaining processing */
	while (ctx->count > 7) {
		a = *((uint64_t *)p) * PRIME64_2;
		hash ^= ROTL64(a, 31) * PRIME64_1;
		hash = ROTL64(hash, 27) * PRIME64_1;
		hash += PRIME64_4;
		p += 8;
		ctx->count -= 8;
	}
	while (ctx->count > 3) {
		a = *((uint32_t *)p);
		hash ^= a * PRIME64_1;
		hash = ROTL64(hash, 23) * PRIME64_2;
		hash += PRIME64_3;
		p += 4;
		ctx->count -= 4;
	}
	while (ctx->count--) {
		a = *p++;
		hash ^= a * PRIME64_5;
		hash = ROTL64(hash, 11) * PRIME64_1;
	}

	/* final mix */
	hash ^= hash >> 33;
	hash *= PRIME64_2;
	hash ^= hash >> 29;
	hash *= PRIME64_3;
	hash ^= hash >> 32;

	return hash;
}

/* @func: xxhash64
 * #desc:
 *    xxh64 single-time processing function.
 *
 * #1: ctx [in/out] xxh64 struct context
 * #2: s   [in]     input buffer
 * #3: len [in]     input length
 * #r:     [ret]    return hash digest
 */
uint64_t F_SYMBOL(xxhash64)(struct xxhash64_ctx *ctx, const uint8_t *s,
		size_t len)
{
	F_SYMBOL(xxhash64_process)(ctx, s, len);
	return F_SYMBOL(xxhash64_finish)(ctx, len);
}
