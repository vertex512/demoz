/* @file: sha1.c
 * #desc:
 *    The implementations sha1 hash algorithm.
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
#include <demoz/lib/sha1.h>


/* @def: _ */
#define ROL(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

/* round functions */
#define F1(b, c, d) ((d) ^ ((b) & ((c) ^ (d))))
#define F2(b, c, d) ((b) ^ (c) ^ (d))
#define F3(b, c, d) (((b) & (c)) | ((d) & ((b) | (c))))
#define F4(b, c, d) ((b) ^ (c) ^ (d))

/* round constants */
#define K1 0x5a827999
#define K2 0x6ed9eba1
#define K3 0x8f1bbcdc
#define K4 0xca62c1d6

#define BSWAP32(x) \
	((((x) & 0xff) << 24) \
	| (((x) & 0xff00) << 8) \
	| (((x) & 0xff0000) >> 8) \
	| (((x) & 0xff000000) >> 24))
/* end */

/* @func: _sha1_compress (static)
 * #desc:
 *    sha1 compression function.
 *
 * #1: ctx [in/out] sha1 struct context
 * #2: s   [in]     input block (length: SHA1_BLOCKSIZE)
 */
static void _sha1_compress(struct sha1_ctx *ctx, const uint8_t *s)
{
	uint32_t A, B, C, D, E, m[80];
	A = ctx->state[0];
	B = ctx->state[1];
	C = ctx->state[2];
	D = ctx->state[3];
	E = ctx->state[4];

	for (int32_t i = 0; i < 16; i++) {
		m[i] = ((uint32_t)s[0]) << 24
			| ((uint32_t)s[1]) << 16
			| ((uint32_t)s[2]) << 8
			| ((uint32_t)s[3]);
		s += 4;
	}

	for (int32_t i = 16; i < 80; i++)
		m[i] = ROL(m[i - 3] ^ m[i - 8] ^ m[i - 14] ^ m[i - 16], 1);

	uint32_t tmp = 0;
	for (int32_t i = 0; i < 80; i++) {
		if (i < 20) {
			tmp = ROL(A, 5) + F1(B, C, D) + E + K1 + m[i];
		} else if (i < 40) {
			tmp = ROL(A, 5) + F2(B, C, D) + E + K2 + m[i];
		} else if (i < 60) {
			tmp = ROL(A, 5) + F3(B, C, D) + E + K3 + m[i];
		} else {
			tmp = ROL(A, 5) + F4(B, C, D) + E + K4 + m[i];
		}

		E = D;
		D = C;
		C = ROL(B, 30);
		B = A;
		A = tmp;
	}

	ctx->state[0] += A;
	ctx->state[1] += B;
	ctx->state[2] += C;
	ctx->state[3] += D;
	ctx->state[4] += E;
}

/* @func: sha1_init
 * #desc:
 *    sha1 struct context initialization.
 *
 * #1: ctx [out] sha1 struct context
 */
void F_SYMBOL(sha1_init)(struct sha1_ctx *ctx)
{
	ctx->count = 0;
	ctx->state[0] = 0x67452301;
	ctx->state[1] = 0xefcdab89;
	ctx->state[2] = 0x98badcfe;
	ctx->state[3] = 0x10325476;
	ctx->state[4] = 0xc3d2e1f0;
}

/* @func: sha1_process
 * #desc:
 *    sha1 processing buffer function.
 *
 * #1: ctx [in/out] sha1 struct context
 * #2: s   [in]     input buffer
 * #3: len [in]     input length
 */
void F_SYMBOL(sha1_process)(struct sha1_ctx *ctx, const uint8_t *s,
		size_t len)
{
#define BLOCKSIZE SHA1_BLOCKSIZE

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
			_sha1_compress(ctx, ctx->buf);
			n = 0;
			s += h;
			len -= h;
		case 0:
			while (len >= BLOCKSIZE) {
				/* processing */
				_sha1_compress(ctx, s);
				s += BLOCKSIZE;
				len -= BLOCKSIZE;
			}

			n = len;
			if (n)
				C_SYMBOL(memcpy)(ctx->buf, s, n);
	}
	ctx->count = n;
}

/* @func: sha1_finish
 * #desc:
 *    sha1 process the remaining bytes in the buffer and end.
 *
 * #1: ctx [in/out] sha1 struct context
 * #2: len [in]     total length
 */
void F_SYMBOL(sha1_finish)(struct sha1_ctx *ctx, uint64_t len)
{
	/* padding */
	uint8_t padbuf[SHA1_BLOCKSIZE];
	C_SYMBOL(memset)(padbuf, 0, sizeof(padbuf));
	padbuf[0] = 0x80;
	F_SYMBOL(sha1_process)(ctx, padbuf,
		1 + ((119 - (len % 64)) % 64));

	/* bit length */
	len *= 8;
	for (int32_t i = 0; i < 8; i++) {
		ctx->buf[63 - i] = (uint8_t)len;
		len >>= 8;
	}
	_sha1_compress(ctx, ctx->buf);

	for (int32_t i = 0; i < 5; i++)
		ctx->state[i] = BSWAP32(ctx->state[i]);
}

/* @func: sha1
 * #desc:
 *    sha1 single-time processing function.
 *
 * #1: ctx [in/out] sha1 struct context
 * #2: s   [in]     input buffer
 * #3: len [in]     input length
 */
void F_SYMBOL(sha1)(struct sha1_ctx *ctx, const uint8_t *s, size_t len)
{
	F_SYMBOL(sha1_process)(ctx, s, len);
	F_SYMBOL(sha1_finish)(ctx, len);
}
