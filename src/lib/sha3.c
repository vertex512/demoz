/* @file: sha3.c
 * #desc:
 *    The implementations of secure hash-3 algorithm.
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


/* @def: _
 * iota round constant */
static const uint64_t keccak_rndc[24] = {
	0x0000000000000001ULL, 0x0000000000008082ULL, 0x800000000000808aULL,
	0x8000000080008000ULL, 0x000000000000808bULL, 0x0000000080000001ULL,
	0x8000000080008081ULL, 0x8000000000008009ULL, 0x000000000000008aULL,
	0x0000000000000088ULL, 0x0000000080008009ULL, 0x000000008000000aULL,
	0x000000008000808bULL, 0x800000000000008bULL, 0x8000000000008089ULL,
	0x8000000000008003ULL, 0x8000000000008002ULL, 0x8000000000000080ULL,
	0x000000000000800aULL, 0x800000008000000aULL, 0x8000000080008081ULL,
	0x8000000000008080ULL, 0x0000000080000001ULL, 0x8000000080008008ULL
	};

/* rotation offset */
static const uint8_t keccak_rho[5][5] = {
	{  0,  1, 62, 28, 27 },
	{ 36, 44,  6, 55, 20 },
	{  3, 10, 43, 25, 39 },
	{ 41, 45, 15, 21,  8 },
	{ 18,  2, 61, 56, 14 }
	};

#define ROTL64(x, n) (((x) << (n)) | ((x) >> (64 - (n))))
/* end */

/* @func: keccak_f1600
 * #desc:
 *    keccak-f permutation function.
 *
 * #1: state [in/out] state buffer
 */
void F_SYMBOL(keccak_f1600)(uint64_t state[5][5])
{
	uint64_t C[5], D[5], B[5][5];

	for (int32_t i = 0; i < SHA3_KECCAK_ROUNDS; i++) {
		/* theta
		 * [x] ^= [y][x]
		 */
#define THETA_1(x) \
	C[x] = state[0][x] ^ state[1][x] \
		^ state[2][x] ^ state[3][x] ^ state[4][x]

		THETA_1(0);
		THETA_1(1);
		THETA_1(2);
		THETA_1(3);
		THETA_1(4);

		/* [x] = [(x + 4) % 5] ^ ROTL64([(x + 1) % 5], 1) */
		D[0] = C[4] ^ ROTL64(C[1], 1);
		D[1] = C[0] ^ ROTL64(C[2], 1);
		D[2] = C[1] ^ ROTL64(C[3], 1);
		D[3] = C[2] ^ ROTL64(C[4], 1);
		D[4] = C[3] ^ ROTL64(C[0], 1);

		/* [y][x] ^= [x] */
#define THETA_2(y) \
	state[y][0] ^= D[0]; state[y][1] ^= D[1]; \
	state[y][2] ^= D[2]; state[y][3] ^= D[3]; state[y][4] ^= D[4]

		THETA_2(0);
		THETA_2(1);
		THETA_2(2);
		THETA_2(3);
		THETA_2(4);

		/* rho
		 * [y][x] = ROTL64([y][x], [y][x])
		 */
#define RHO_1(y) \
	state[y][0] = ROTL64(state[y][0], keccak_rho[y][0]); \
	state[y][1] = ROTL64(state[y][1], keccak_rho[y][1]); \
	state[y][2] = ROTL64(state[y][2], keccak_rho[y][2]); \
	state[y][3] = ROTL64(state[y][3], keccak_rho[y][3]); \
	state[y][4] = ROTL64(state[y][4], keccak_rho[y][4])

		RHO_1(0);
		RHO_1(1);
		RHO_1(2);
		RHO_1(3);
		RHO_1(4);

		/* pi
		 * [y][x] = [y][x]
		 */
#define PI_1(y) \
	B[y][0] = state[y][0]; \
	B[y][1] = state[y][1]; \
	B[y][2] = state[y][2]; \
	B[y][3] = state[y][3]; \
	B[y][4] = state[y][4]

		PI_1(0);
		PI_1(1);
		PI_1(2);
		PI_1(3);
		PI_1(4);

		/* [y][x] = [x][((3 * y) + x) % 5] */
#define PI_2(y) \
	state[y][0] = B[0][((3 * y) + 0) % 5]; \
	state[y][1] = B[1][((3 * y) + 1) % 5]; \
	state[y][2] = B[2][((3 * y) + 2) % 5]; \
	state[y][3] = B[3][((3 * y) + 3) % 5]; \
	state[y][4] = B[4][((3 * y) + 4) % 5]

		PI_2(0);
		PI_2(1);
		PI_2(2);
		PI_2(3);
		PI_2(4);

		/* chi
		 * [x] = [y][x] ^ (~[y][(x + 1) % 5]) & [y][(x + 2) % 5]
		 * [y][x] = [x]
		 */
#define CHI_1(y) \
	C[0] = state[y][0] ^ (~state[y][1] & state[y][2]); \
	C[1] = state[y][1] ^ (~state[y][2] & state[y][3]); \
	C[2] = state[y][2] ^ (~state[y][3] & state[y][4]); \
	C[3] = state[y][3] ^ (~state[y][4] & state[y][0]); \
	C[4] = state[y][4] ^ (~state[y][0] & state[y][1]); \
	state[y][0] = C[0]; \
	state[y][1] = C[1]; \
	state[y][2] = C[2]; \
	state[y][3] = C[3]; \
	state[y][4] = C[4]

		CHI_1(0);
		CHI_1(1);
		CHI_1(2);
		CHI_1(3);
		CHI_1(4);

		/* iota */
		state[0][0] ^= keccak_rndc[i];
	}
}

/* @func: keccak_absorb
 * #desc:
 *    keccak absorb function.
 *
 * #1: state [in/out] state buffer
 * #2: in    [in]     input buffer (length: >=rate)
 * #3: len   [in]     input length
 * #4: len   [in]     bitrate length (byte)
 * #r: rate  [in]     remaining length
 */
size_t F_SYMBOL(keccak_absorb)(uint64_t state[5][5], const uint8_t *in,
		size_t len, uint32_t rate)
{
	while (len >= rate) {
		for (uint32_t i = 0; i < (rate / 8); i++) {
			((uint64_t *)state)[i] ^= (uint64_t)in[0]
				| (uint64_t)in[1] << 8
				| (uint64_t)in[2] << 16
				| (uint64_t)in[3] << 24
				| (uint64_t)in[4] << 32
				| (uint64_t)in[5] << 40
				| (uint64_t)in[6] << 48
				| (uint64_t)in[7] << 56;
			in += 8;
		}
		len -= rate;

		F_SYMBOL(keccak_f1600)(state);
	}

	return len;
}

/* @func: keccak_squeeze
 * #desc:
 *    keccak squeeze function.
 *
 * #1: state [in/out] state buffer
 * #2: out   [out]    digest output
 * #3: len   [in]     digest length
 * #4: rate  [in]     bitrate length (byte)
 */
void F_SYMBOL(keccak_squeeze)(uint64_t state[5][5], uint8_t *out,
		uint32_t len, uint32_t rate)
{
	while (len) {
		for (uint32_t i = 0; i < (rate / 8) && len; i++) {
			uint64_t A = ((uint64_t *)state)[i];
			if (len < 8) {
				for (uint32_t j = 0; j < len; j++) {
					*out++ = (uint8_t)A;
					A >>= 8;
				}
				return;
			}

			out[0] = (uint8_t)A;
			out[1] = (uint8_t)(A >> 8);
			out[2] = (uint8_t)(A >> 16);
			out[3] = (uint8_t)(A >> 24);
			out[4] = (uint8_t)(A >> 32);
			out[5] = (uint8_t)(A >> 40);
			out[6] = (uint8_t)(A >> 48);
			out[7] = (uint8_t)(A >> 56);
			out += 8;
			len -= 8;
		}

		/* len > rate */
		F_SYMBOL(keccak_f1600)(state);
	}
}

/* @func: sha3_init
 * #desc:
 *    sha3 struct context initialization.
 *
 * #1: ctx   [out] sha3 struct context
 * #2: type  [in]  digest type
 * #3: dsize [in]  shake digest length (byte, 0: use xof function)
 * #r:       [ret] 0: no error, -1: type error, -2: digest size error
 */
int32_t F_SYMBOL(sha3_init)(struct sha3_ctx *ctx, int32_t type,
		uint32_t dsize)
{
	ctx->pad = 0x06;
	switch (type) {
		case SHA3_224_TYPE:
			ctx->rate = SHA3_224_RATE;
			ctx->dsize = SHA3_224_LEN;
			break;
		case SHA3_256_TYPE:
			ctx->rate = SHA3_256_RATE;
			ctx->dsize = SHA3_256_LEN;
			break;
		case SHA3_384_TYPE:
			ctx->rate = SHA3_384_RATE;
			ctx->dsize = SHA3_384_LEN;
			break;
		case SHA3_512_TYPE:
			ctx->rate = SHA3_512_RATE;
			ctx->dsize = SHA3_512_LEN;
			break;
		case SHA3_SHAKE128_TYPE:
			if (dsize > SHA3_STATE_SIZE)
				return -2;
			ctx->rate = SHA3_SHAKE128_RATE;
			ctx->dsize = dsize;
			ctx->pad = 0x1f;
			break;
		case SHA3_SHAKE256_TYPE:
			if (dsize > SHA3_STATE_SIZE)
				return -2;
			ctx->rate = SHA3_SHAKE256_RATE;
			ctx->dsize = dsize;
			ctx->pad = 0x1f;
			break;
		default:
			return -1;
	}

	C_SYMBOL(memset)(ctx->state, 0, sizeof(ctx->state));
	ctx->count = 0;

	return 0;
}

/* @func: sha3_process
 * #desc:
 *    sha3 processing buffer function.
 *
 * #1: ctx [in/out] sha3 struct context
 * #2: s   [in]     input buffer
 * #3: len [in]     input length
 */
void F_SYMBOL(sha3_process)(struct sha3_ctx *ctx, const uint8_t *s,
		size_t len)
{
	size_t n = ctx->count, h;
	switch (n) {
		default:
			h = ctx->rate - n;
			h = (len < h) ? len : h;
			C_SYMBOL(memcpy)(ctx->u.buf + n, s, h);
			n += h;
			if (n != ctx->rate)
				break;

			/* processing */
			F_SYMBOL(keccak_absorb)(ctx->state, ctx->u.buf,
				ctx->rate, ctx->rate);
			n = 0;
			s += h;
			len -= h;
		case 0:
			/* processing */
			n = F_SYMBOL(keccak_absorb)(ctx->state, s, len,
				ctx->rate);
			if (n) {
				s += len - n;
				C_SYMBOL(memcpy)(ctx->u.buf, s, n);
			}
	}
	ctx->count = n;
}

/* @func: sha3_finish
 * #desc:
 *    sha3 process the remaining bytes in the buffer and end.
 *
 * #1: ctx [in/out] sha3 struct context
 */
void F_SYMBOL(sha3_finish)(struct sha3_ctx *ctx)
{
	C_SYMBOL(memset)(ctx->u.buf + ctx->count, 0,
		ctx->rate - ctx->count);
	ctx->u.buf[ctx->count] = ctx->pad;
	ctx->u.buf[ctx->rate - 1] |= 0x80;

	F_SYMBOL(keccak_absorb)(ctx->state, ctx->u.buf, ctx->rate,
		ctx->rate);
	F_SYMBOL(keccak_squeeze)(ctx->state, ctx->u.digest, ctx->dsize,
		ctx->rate);
}

/* @func: sha3
 * #desc:
 *    sha3 single-time processing function.
 *
 * #1: ctx [in/out] sha3 struct context
 * #2: s   [in]     input buffer
 * #3: len [in]     input length
 */
void F_SYMBOL(sha3)(struct sha3_ctx *ctx, const uint8_t *s, size_t len)
{
	F_SYMBOL(sha3_process)(ctx, s, len);
	F_SYMBOL(sha3_finish)(ctx);
}

/* @func: sha3_shake_xof
 * #desc:
 *    keccak shake extendable output function.
 *
 * #1: ctx [in/out] sha3 struct context
 * #2: out [out]    digest output
 * #3: len [in]     digest length
 */
void F_SYMBOL(sha3_shake_xof)(struct sha3_ctx *ctx, uint8_t *out, uint32_t len)
{
	F_SYMBOL(keccak_squeeze)(ctx->state, out, len, ctx->rate);
}
