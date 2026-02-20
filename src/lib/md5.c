/* @file: md5.c
 * #desc:
 *    The implementations of md5 hash algorithm.
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
#include <demoz/lib/md5.h>


/* @def: _
 * md5 round constants */
static const uint32_t md5_constants[64] = {
	0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
	0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
	0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
	0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
	0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
	0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
	0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
	0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
	0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
	0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
	0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
	0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
	0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
	0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
	0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
	0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
	};

/* md5 round number */
static const uint32_t md5_number[64] = {
	7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
	5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
	4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
	6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21
	};

#define ROL(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

/* round functions */
#define FF(b, c, d) ((d) ^ ((b) & ((c) ^ (d))))
#define FG(b, c, d) FF(d, b, c)
#define FH(b, c, d) ((b) ^ (c) ^ (d))
#define FI(b, c, d) ((c) ^ ((b) | ~(d)))
/* end */

/* @func: _md5_compress (static)
 * #desc:
 *    md5 compression function.
 *
 * #1: ctx [in/out] md5 struct context
 * #2: s   [in]     input block (length: MD5_BLOCKSIZE)
 */
static void _md5_compress(struct md5_ctx *ctx, const uint8_t *s)
{
	uint32_t A, B, C, D, m[16];
	A = ctx->state[0];
	B = ctx->state[1];
	C = ctx->state[2];
	D = ctx->state[3];

	for (int32_t i = 0; i < 16; i++) {
		m[i] = ((uint32_t)s[0])
			| ((uint32_t)s[1]) << 8
			| ((uint32_t)s[2]) << 16
			| ((uint32_t)s[3]) << 24;
		s += 4;
	}

	uint32_t tmp = 0, f = 0, g = 0;
	for (uint32_t i = 0; i < 64; i++) {
		if (i < 16) {
			f = FF(B, C, D);
			g = i;
		} else if (i < 32) {
			f = FG(B, C, D);
			g = (5 * i + 1) % 16;
		} else if (i < 48) {
			f = FH(B, C, D);
			g = (3 * i + 5) % 16;
		} else {
			f = FI(B, C, D);
			g = (7 * i) % 16;
		}

		tmp = D;
		D = C;
		C = B;
		B = B + ROL(A + f + md5_constants[i] + m[g], md5_number[i]);
		A = tmp;
	}

	ctx->state[0] += A;
	ctx->state[1] += B;
	ctx->state[2] += C;
	ctx->state[3] += D;
}

/* @func: md5_init
 * #desc:
 *    md5 struct context initialization.
 *
 * #1: ctx [out] md5 struct context
 */
void F_SYMBOL(md5_init)(struct md5_ctx *ctx)
{
	ctx->count = 0;
	ctx->state[0] = 0x67452301;
	ctx->state[1] = 0xefcdab89;
	ctx->state[2] = 0x98badcfe;
	ctx->state[3] = 0x10325476;
}

/* @func: md5_process
 * #desc:
 *    md5 processing buffer function.
 *
 * #1: ctx [in/out] md5 struct context
 * #2: s   [in]     input buffer
 * #3: len [in]     input length
 */
void F_SYMBOL(md5_process)(struct md5_ctx *ctx, const uint8_t *s,
		size_t len)
{
#define BLOCKSIZE MD5_BLOCKSIZE

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
			_md5_compress(ctx, ctx->buf);
			n = 0;
			s += h;
			len -= h;
		case 0:
			while (len >= BLOCKSIZE) {
				/* processing */
				_md5_compress(ctx, s);
				s += BLOCKSIZE;
				len -= BLOCKSIZE;
			}

			n = len;
			if (n)
				C_SYMBOL(memcpy)(ctx->buf, s, n);
	}
	ctx->count = n;
}

/* @func: md5_finish
 * #desc:
 *    md5 process the remaining bytes in the buffer and end.
 *
 * #1: ctx [in/out] md5 struct context
 * #2: len [in]     total length
 */
void F_SYMBOL(md5_finish)(struct md5_ctx *ctx, uint64_t len)
{
	/* padding */
	uint8_t padbuf[MD5_BLOCKSIZE];
	C_SYMBOL(memset)(padbuf, 0, sizeof(padbuf));
	padbuf[0] = 0x80;
	F_SYMBOL(md5_process)(ctx, padbuf,
		1 + ((119 - (len % 64)) % 64));

	/* bit length */
	len *= 8;
	for (int32_t i = 0; i < 8; i++) {
		ctx->buf[56 + i] = (uint8_t)len;
		len >>= 8;
	}
	_md5_compress(ctx, ctx->buf);
}

/* @func: md5
 * #desc:
 *    md5 single-time processing function.
 *
 * #1: ctx [in/out] md5 struct context
 * #2: s   [in]     input buffer
 * #3: len [in]     input length
 */
void F_SYMBOL(md5)(struct md5_ctx *ctx, const uint8_t *s, size_t len)
{
	F_SYMBOL(md5_process)(ctx, s, len);
	F_SYMBOL(md5_finish)(ctx, len);
}
