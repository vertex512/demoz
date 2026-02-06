/* @file: sha512.c
 * #desc:
 *    The implementations of sha512 hash algorithm.
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


/* @def: _
 * sha512 round constants */
static const uint64_t sha512_constants[80] = {
	0x428a2f98d728ae22ULL, 0x7137449123ef65cdULL, 0xb5c0fbcfec4d3b2fULL,
	0xe9b5dba58189dbbcULL, 0x3956c25bf348b538ULL, 0x59f111f1b605d019ULL,
	0x923f82a4af194f9bULL, 0xab1c5ed5da6d8118ULL, 0xd807aa98a3030242ULL,
	0x12835b0145706fbeULL, 0x243185be4ee4b28cULL, 0x550c7dc3d5ffb4e2ULL,
	0x72be5d74f27b896fULL, 0x80deb1fe3b1696b1ULL, 0x9bdc06a725c71235ULL,
	0xc19bf174cf692694ULL, 0xe49b69c19ef14ad2ULL, 0xefbe4786384f25e3ULL,
	0x0fc19dc68b8cd5b5ULL, 0x240ca1cc77ac9c65ULL, 0x2de92c6f592b0275ULL,
	0x4a7484aa6ea6e483ULL, 0x5cb0a9dcbd41fbd4ULL, 0x76f988da831153b5ULL,
	0x983e5152ee66dfabULL, 0xa831c66d2db43210ULL, 0xb00327c898fb213fULL,
	0xbf597fc7beef0ee4ULL, 0xc6e00bf33da88fc2ULL, 0xd5a79147930aa725ULL,
	0x06ca6351e003826fULL, 0x142929670a0e6e70ULL, 0x27b70a8546d22ffcULL,
	0x2e1b21385c26c926ULL, 0x4d2c6dfc5ac42aedULL, 0x53380d139d95b3dfULL,
	0x650a73548baf63deULL, 0x766a0abb3c77b2a8ULL, 0x81c2c92e47edaee6ULL,
	0x92722c851482353bULL, 0xa2bfe8a14cf10364ULL, 0xa81a664bbc423001ULL,
	0xc24b8b70d0f89791ULL, 0xc76c51a30654be30ULL, 0xd192e819d6ef5218ULL,
	0xd69906245565a910ULL, 0xf40e35855771202aULL, 0x106aa07032bbd1b8ULL,
	0x19a4c116b8d2d0c8ULL, 0x1e376c085141ab53ULL, 0x2748774cdf8eeb99ULL,
	0x34b0bcb5e19b48a8ULL, 0x391c0cb3c5c95a63ULL, 0x4ed8aa4ae3418acbULL,
	0x5b9cca4f7763e373ULL, 0x682e6ff3d6b2b8a3ULL, 0x748f82ee5defb2fcULL,
	0x78a5636f43172f60ULL, 0x84c87814a1f0ab72ULL, 0x8cc702081a6439ecULL,
	0x90befffa23631e28ULL, 0xa4506cebde82bde9ULL, 0xbef9a3f7b2c67915ULL,
	0xc67178f2e372532bULL, 0xca273eceea26619cULL, 0xd186b8c721c0c207ULL,
	0xeada7dd6cde0eb1eULL, 0xf57d4f7fee6ed178ULL, 0x06f067aa72176fbaULL,
	0x0a637dc5a2c898a6ULL, 0x113f9804bef90daeULL, 0x1b710b35131c471bULL,
	0x28db77f523047d84ULL, 0x32caab7b40c72493ULL, 0x3c9ebe0a15c9bebcULL,
	0x431d67c49c100d4cULL, 0x4cc5d4becb3e42b6ULL, 0x597f299cfc657e2aULL,
	0x5fcb6fab3ad6faecULL, 0x6c44198c4a475817ULL
	};

#define SROL(a, b) (((a) >> (b)) | ((a) << (64 - (b))))
#define SHR(a, b) ((a) >> (b))
#define SCH(a, b, c) (((a) & (b)) ^ (~(a) & (c)))
#define SMAJ(a, b, c) (((a) & (b)) ^ ((a) & (c)) ^ ((b) & (c)))
#define SIG0(x) (SROL(x, 28) ^ SROL(x, 34) ^ SROL(x, 39))
#define SIG1(x) (SROL(x, 14) ^ SROL(x, 18) ^ SROL(x, 41))
#define SEP0(x) (SROL(x, 1) ^ SROL(x, 8) ^ SHR(x, 7))
#define SEP1(x) (SROL(x, 19) ^ SROL(x, 61) ^ SHR(x, 6))

#define BSWAP64(x) \
	((((x) & 0xffULL) << 56) \
	| (((x) & 0xff00ULL) << 40) \
	| (((x) & 0xff0000ULL) << 24) \
	| (((x) & 0xff000000ULL) << 8) \
	| (((x) & 0xff00000000ULL) >> 8) \
	| (((x) & 0xff0000000000ULL) >> 24) \
	| (((x) & 0xff000000000000ULL) >> 40) \
	| (((x) & 0xff00000000000000ULL) >> 56))
/* end */

/* @func: _sha512_compress (static)
 * #desc:
 *    sha512 compression function.
 *
 * #1: sha512 struct context
 * #2: input block (length: SHA512_BLOCKSIZE)
 */
static void _sha512_compress(struct sha512_ctx *ctx, const uint8_t *s)
{
	uint64_t A, B, C, D, E, F, G, H, t1, t2, m[80];
	A = ctx->state[0];
	B = ctx->state[1];
	C = ctx->state[2];
	D = ctx->state[3];
	E = ctx->state[4];
	F = ctx->state[5];
	G = ctx->state[6];
	H = ctx->state[7];

	for (int32_t i = 0; i < 16; i++) {
		m[i] = (uint64_t)s[0] << 56
			| (uint64_t)s[1] << 48
			| (uint64_t)s[2] << 40
			| (uint64_t)s[3] << 32
			| (uint64_t)s[4] << 24
			| (uint64_t)s[5] << 16
			| (uint64_t)s[6] << 8
			| (uint64_t)s[7];
		s += 8;
	}

	for (int32_t i = 16; i < 80; i++)
		m[i] = SEP1(m[i - 2]) + m[i - 7] + SEP0(m[i - 15]) + m[i - 16];

	for (int32_t i = 0; i < 80; i++) {
		t1 = H + SIG1(E) + SCH(E, F, G) + sha512_constants[i] + m[i];
		t2 = SIG0(A) + SMAJ(A, B, C);
		H = G;
		G = F;
		F = E;
		E = D + t1;
		D = C;
		C = B;
		B = A;
		A = t1 + t2;
	}

	ctx->state[0] += A;
	ctx->state[1] += B;
	ctx->state[2] += C;
	ctx->state[3] += D;
	ctx->state[4] += E;
	ctx->state[5] += F;
	ctx->state[6] += G;
	ctx->state[7] += H;
}

/* @func: sha512_init
 * #desc:
 *    sha512 struct context initialization.
 *
 * #1: sha512 struct context
 */
void F_SYMBOL(sha512_init)(struct sha512_ctx *ctx)
{
	ctx->count = 0;
	ctx->state[0] = 0x6a09e667f3bcc908ULL;
	ctx->state[1] = 0xbb67ae8584caa73bULL;
	ctx->state[2] = 0x3c6ef372fe94f82bULL;
	ctx->state[3] = 0xa54ff53a5f1d36f1ULL;
	ctx->state[4] = 0x510e527fade682d1ULL;
	ctx->state[5] = 0x9b05688c2b3e6c1fULL;
	ctx->state[6] = 0x1f83d9abfb41bd6bULL;
	ctx->state[7] = 0x5be0cd19137e2179ULL;
}

/* @func: sha384_init
 * #desc:
 *    initialize the sha512 struct context with sha384.
 *
 * #1: sha512 struct context
 */
void F_SYMBOL(sha384_init)(struct sha512_ctx *ctx)
{
	ctx->count = 0;
	ctx->state[0] = 0xcbbb9d5dc1059ed8ULL;
	ctx->state[1] = 0x629a292a367cd507ULL;
	ctx->state[2] = 0x9159015a3070dd17ULL;
	ctx->state[3] = 0x152fecd8f70e5939ULL;
	ctx->state[4] = 0x67332667ffc00b31ULL;
	ctx->state[5] = 0x8eb44a8768581511ULL;
	ctx->state[6] = 0xdb0c2e0d64f98fa7ULL;
	ctx->state[7] = 0x47b5481dbefa4fa4ULL;
}

/* @func: sha512_process
 * #desc:
 *    sha512 processing buffer function.
 *
 * #1: sha512 struct context
 * #2: input buffer
 * #3: input length
 */
void F_SYMBOL(sha512_process)(struct sha512_ctx *ctx, const uint8_t *s,
		size_t len)
{
#define BLOCKSIZE SHA512_BLOCKSIZE

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
			_sha512_compress(ctx, ctx->buf);
			n = 0;
			s += h;
			len -= h;
		case 0:
			while (len >= BLOCKSIZE) {
				/* processing */
				_sha512_compress(ctx, s);
				s += BLOCKSIZE;
				len -= BLOCKSIZE;
			}

			n = len;
			if (n)
				C_SYMBOL(memcpy)(ctx->buf, s, n);
	}
	ctx->count = n;
}

/* @func: sha512_finish
 * #desc:
 *    sha512 process the remaining bytes in the buffer and end.
 *
 * #1: sha512 struct context
 * #2: total length
 */
void F_SYMBOL(sha512_finish)(struct sha512_ctx *ctx, uint64_t len)
{
	/* padding */
	uint8_t padbuf[SHA512_BLOCKSIZE];
	C_SYMBOL(memset)(padbuf, 0, sizeof(padbuf));
	padbuf[0] = 0x80;
	F_SYMBOL(sha512_process)(ctx, padbuf,
		1 + ((239 - (len % 128)) % 128));

	/* bit length */
	len *= 8;
	for (int32_t i = 0; i < 16; i++) {
		ctx->buf[127 - i] = (uint8_t)len;
		len >>= 8;
	}
	_sha512_compress(ctx, ctx->buf);

	for (int32_t i = 0; i < 8; i++)
		ctx->state[i] = BSWAP64(ctx->state[i]);
}

/* @func: sha512
 * #desc:
 *    sha512 single-time processing function.
 *
 * #1: sha512 struct context
 * #2: input buffer
 * #3: input length
 */
void F_SYMBOL(sha512)(struct sha512_ctx *ctx, const uint8_t *s,
		size_t len)
{
	F_SYMBOL(sha512_process)(ctx, s, len);
	F_SYMBOL(sha512_finish)(ctx, len);
}
