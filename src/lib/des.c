/* @file: des.c
 * #desc:
 *    the implementations of data encryption standard (lucifer).
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
#include <demoz/c/string.h>
#include <demoz/lib/des.h>


/* @def: _
 * initial permutation */
static const uint8_t des_ip[64] = {
	58, 50, 42, 34, 26, 18, 10,  2,
	60, 52, 44, 36, 28, 20, 12,  4,
	62, 54, 46, 38, 30, 22, 14,  6,
	64, 56, 48, 40, 32, 24, 16,  8,
	57, 49, 41, 33, 25, 17,  9,  1,
	59, 51, 43, 35, 27, 19, 11,  3,
	61, 53, 45, 37, 29, 21, 13,  5,
	63, 55, 47, 39, 31, 23, 15,  7
	};

/* final permutation */
static const uint8_t des_ip1[64] = {
	40,  8, 48, 16, 56, 24, 64, 32,
	39,  7, 47, 15, 55, 23, 63, 31,
	38,  6, 46, 14, 54, 22, 62, 30,
	37,  5, 45, 13, 53, 21, 61, 29,
	36,  4, 44, 12, 52, 20, 60, 28,
	35,  3, 43, 11, 51, 19, 59, 27,
	34,  2, 42, 10, 50, 18, 58, 26,
	33,  1, 41,  9, 49, 17, 57, 25
	};

/* expansion function */
static const uint8_t des_e[48] = {
	32,  1,  2,  3,  4,  5,
	 4,  5,  6,  7,  8,  9,
	 8,  9, 10, 11, 12, 13,
	12, 13, 14, 15, 16, 17,
	16, 17, 18, 19, 20, 21,
	20, 21, 22, 23, 24, 25,
	24, 25, 26, 27, 28, 29,
	28, 29, 30, 31, 32,  1
	};

/* permutation */
static const uint8_t des_p[32] = {
	16,  7, 20, 21,
	29, 12, 28, 17,
	 1, 15, 23, 26,
	 5, 18, 31, 10,
	 2,  8, 24, 14,
	32, 27,  3,  9,
	19, 13, 30,  6,
	22, 11,  4, 25
	};

/* permuted choice 1 */
static const uint8_t des_pc1[64] = {
	57, 49, 41, 33, 25, 17,  9,
	 1, 58, 50, 42, 34, 26, 18,
	10,  2, 59, 51, 43, 35, 27,
	19, 11,  3, 60, 52, 44, 36,
	63, 55, 47, 39, 31, 23, 15,
	 7, 62, 54, 46, 38, 30, 22,
	14,  6, 61, 53, 45, 37, 29,
	21, 13,  5, 28, 20, 12,  4
	};

/* permuted choice 2 */
static const uint8_t des_pc2[48] = {
	14, 17, 11, 24,  1,  5,
	 3, 28, 15,  6, 21, 10,
	23, 19, 12,  4, 26,  8,
	16,  7, 27, 20, 13,  2,
	41, 52, 31, 37, 47, 55,
	30, 40, 51, 45, 33, 48,
	44, 49, 39, 56, 34, 53,
	46, 42, 50, 36, 29, 32
	};

/* bits rotation */
static const uint8_t des_brot[16] = {
	1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
	};

/* substitution boxes */
static const uint8_t des_sbox[8][64] = {
	{
		14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9, 0,  7,
		 0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5, 3,  8,
		 4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10, 5,  0,
		15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0, 6, 13
	},
	{
		15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
		 3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
		 0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
		13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9
	},
	{
		10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,
		13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
		13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
		 1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12
	},
	{
		 7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,
		13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
		10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
		 3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14
	},
	{
		 2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,
		14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
		 4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
		11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3
	},
	{
		12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
		10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
		 9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
		 4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13,
	},
	{
		 4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
		13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
		 1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
		 6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12,
	},
	{
		13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
		 1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
		 7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
		 2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11
	}
	};
/* end */

/* @func: _des_permute_pc1 (static)
 * #desc:
 *    remove parity bit, 64bit to 56bit.
 *
 * #1: output 56bit key
 * #2: input 64bit key
 */
static void _des_permute_pc1(uint8_t *key_k, const uint8_t *key)
{
	for (int32_t i = 0; i < 56; i++) {
		uint8_t sh_size = des_pc1[i];
		uint8_t sh_byte = 0x80 >> ((sh_size - 1) % 8);
		sh_byte &= key[(sh_size - 1) / 8];
		sh_byte <<= (sh_size - 1) % 8;
		key_k[i / 8] |= sh_byte >> (i % 8);
	}
}

/* @func: _des_shiftpcd (static)
 * #desc:
 *    key 56bit to 2 * 28bit, c and d shift.
 *
 * #1: key 28bit c or d
 * #2: key expansion rounds
 */
static void _des_shiftpcd(uint8_t *key_cd, int32_t round)
{
	uint8_t sh_size = des_brot[round - 1];
	uint8_t sh_byte = (sh_size == 1) ? 0x80 : 0xc0;

	uint8_t a, b, c, d;
	a = sh_byte & key_cd[0];
	b = sh_byte & key_cd[1];
	c = sh_byte & key_cd[2];
	d = sh_byte & key_cd[3];

	key_cd[0] <<= sh_size;
	key_cd[0] |= b >> (8 - sh_size);
	key_cd[1] <<= sh_size;
	key_cd[1] |= c >> (8 - sh_size);
	key_cd[2] <<= sh_size;
	key_cd[2] |= d >> (8 - sh_size);
	key_cd[3] <<= sh_size;
	key_cd[3] |= a >> (4 - sh_size);
}

/* @func: _des_permute_pc2 (static)
 * #desc:
 *    key 28bit c and d merge 48bit key.
 *
 * #1: key 48bit merge
 * #2: key 28bit c
 * #3: key 28bit d
 */
static void _des_permute_pc2(uint8_t *key_k, uint8_t *key_c, uint8_t *key_d)
{
	for (int32_t i = 0; i < 48; i++) {
		uint8_t sh_size = des_pc2[i];
		uint8_t sh_byte = 0;
		if (sh_size < 29) {
			sh_byte = 0x80 >> ((sh_size - 1) % 8);
			sh_byte &= key_c[(sh_size - 1) / 8];
			sh_byte <<= (sh_size - 1) % 8;
		} else {
			sh_byte = 0x80 >> ((sh_size - 29) % 8);
			sh_byte &= key_d[(sh_size - 29) / 8];
			sh_byte <<= (sh_size - 29) % 8;
		}
		key_k[i / 8] |= sh_byte >> (i % 8);
	}
}

/* @func: _des_permute_ip (static)
 * #desc:
 *    buffer initial permute.
 *
 * #1: output buffer
 * #2: input buffer
 */
static void _des_permute_ip(uint8_t *out, uint8_t *in)
{
	for (int32_t i = 0; i < 64; i++) {
		uint8_t sh_size = des_ip[i];
		uint8_t sh_byte = 0x80 >> ((sh_size - 1) % 8);
		sh_byte &= in[(sh_size - 1) / 8];
		sh_byte <<= (sh_size - 1) % 8;
		out[i / 8] |= sh_byte >> (i % 8);
	}
}

/* @func: _des_expand_e (static)
 * #desc:
 *    expansion 32bit to 48bit.
 *
 * #1: output buffer
 * #2: input buffer
 */
static void _des_expand_e(uint8_t *out, uint8_t *in)
{
	for (int32_t i = 0; i < 48; i++) {
		uint8_t sh_size = des_e[i];
		uint8_t sh_byte = 0x80 >> ((sh_size - 1) % 8);
		sh_byte &= in[(sh_size - 1) / 8];
		sh_byte <<= (sh_size - 1) % 8;
		out[i / 8] |= sh_byte >> (i % 8);
	}
}

/* @func: _des_permute_sbox (static)
 * #desc:
 *    des sbox compress (48bit to 32bit).
 *
 * #1: output buffer
 * #2: input buffer
 */
static void _des_permute_sbox(uint8_t *out, uint8_t *in)
{
	uint32_t row = 0, column = 0;

	/* sbox-1 */
	row = ((in[0] & 0x80) >> 6) | ((in[0] & 0x04) >> 2);
	column = (in[0] & 0x78) >> 3;
	out[0] |= des_sbox[0][row * 16 + column] << 4;

	/* sbox-2 */
	row = (in[0] & 0x02) | ((in[1] & 0x10) >> 4);
	column = ((in[0] & 0x01) << 3) | ((in[1] & 0xe0) >> 5);
	out[0] |= des_sbox[1][row * 16 + column];

	/* sbox-3 */
	row = ((in[1] & 0x08) >> 2) | ((in[2] & 0x40) >> 6);
	column = ((in[1] & 0x07) << 1) | ((in[2] & 0x80) >> 7);
	out[1] |= des_sbox[2][row * 16 + column] << 4;

	/* sbox-4 */
	row = ((in[2] & 0x20) >> 4) | (in[2] & 0x01);
	column = (in[2] & 0x1e) >> 1;
	out[1] |= des_sbox[3][row * 16 + column];

	/* sbox-5 */
	row = ((in[3] & 0x80) >> 6) | ((in[3] & 0x04) >> 2);
	column = (in[3] & 0x78) >> 3;
	out[2] |= des_sbox[4][row * 16 + column] << 4;

	/* sbox-6 */
	row = (in[3] & 0x02) | ((in[4] & 0x10) >> 4);
	column = ((in[3] & 0x01) << 3) | ((in[4] & 0xe0) >> 5);
	out[2] |= des_sbox[5][row * 16 + column];

	/* sbox-7 */
	row = ((in[4] & 0x08) >> 2) | ((in[5] & 0x40) >> 6);
	column = ((in[4] & 0x07) << 1) | ((in[5] & 0x80) >> 7);
	out[3] |= des_sbox[6][row * 16 + column] << 4;

	/* sbox-8 */
	row = ((in[5] & 0x20) >> 4) | (in[5] & 0x01);
	column = (in[5] & 0x1e) >> 1;
	out[3] |= des_sbox[7][row * 16 + column];
}

/* @func: _des_permute_p (static)
 * #desc:
 *    32bit p permutation shuffles.
 *
 * #1: output buffer
 * #2: input buffer
 */
static void _des_permute_p(uint8_t *out, uint8_t *in)
{
	for (int32_t i = 0; i < 32; i++) {
		uint8_t sh_size = des_p[i];
		uint8_t sh_byte = 0x80 >> ((sh_size - 1) % 8);
		sh_byte &= in[(sh_size - 1) / 8];
		sh_byte <<= (sh_size - 1) % 8;
		out[i / 8] |= sh_byte >> (i %8);
	}
}

/* @func: _des_permute_ip1 (static)
 * #desc:
 *    buffer final permutation.
 *
 * #1: output buffer
 * #2: input buffer
 */
static void _des_permute_ip1(uint8_t *out, uint8_t *in)
{
	for (int32_t i = 0; i < 64; i++) {
		uint8_t sh_size = des_ip1[i];
		uint8_t sh_byte = 0x80 >> ((sh_size - 1) % 8);
		sh_byte &= in[(sh_size - 1) / 8];
		sh_byte <<= (sh_size - 1) % 8;
		out[i / 8] |= sh_byte >> (i % 8);
	}
}

/* @func: des_init
 * #desc:
 *    des initialization function.
 *
 * #1: des context struct
 * #2: input key (length: DES_KEYLEN)
 */
void F_SYMBOL(des_init)(struct des_ctx *ctx, const uint8_t *key)
{
	C_SYMBOL(memset)(ctx, 0, sizeof(struct des_ctx));
	uint8_t *key_k, *key_c, *key_d;
	key_k = ctx->key_k[0];
	key_c = ctx->key_c[0];
	key_d = ctx->key_d[0];

	_des_permute_pc1(key_k, key);

	for (int32_t i = 0; i < 3; i++)
		key_c[i] = key_k[i];
	key_c[3] = key_k[3] & 0xf0;

	for (int32_t i = 0; i < 3; i++) {
		key_d[i] = (key_k[i + 3] & 0x0f) << 4;
		key_d[i] |= (key_k[i + 4] & 0xf0) >> 4;
	}
	key_d[3] = (key_k[6] & 0x0f) << 4;

	for (int32_t j = 1; j < 17; j++) {
		key_k = ctx->key_k[j];
		key_c = ctx->key_c[j];
		key_d = ctx->key_d[j];

		for (int32_t i = 0; i < 4; i++) {
			key_c[i] = ctx->key_c[j - 1][i];
			key_d[i] = ctx->key_d[j - 1][i];
		}

		_des_shiftpcd(key_c, j);
		_des_shiftpcd(key_d, j);
		_des_permute_pc2(key_k, key_c, key_d);
	}
}

/* @func: des_crypto
 * #desc:
 *    des encryption and decryption function.
 *
 * #1: des struct context
 * #2: buffer (length: DES_BLOCKSIZE)
 * #3: 0: encrypt, 1: decrypt
 */
void F_SYMBOL(des_crypto)(struct des_ctx *ctx, uint8_t *buf,
		int32_t is_decrypt)
{
	uint8_t lr[8], l[4], r[4];
	C_SYMBOL(memset)(lr, 0, sizeof(lr));
	_des_permute_ip(lr, buf);
	for (int32_t i = 0; i < 4; i++) {
		l[i] = lr[i];
		r[i] = lr[i + 4];
	}

	uint8_t er[6], ers[4], rn[4];
	for (int32_t j = 1; j < 17; j++) {
		C_SYMBOL(memset)(er, 0, sizeof(er));
		_des_expand_e(er, r);

		int32_t k = is_decrypt ? (17 - j) : j;
		for (int32_t i = 0; i < 6; i++)
			er[i] ^= ctx->key_k[k][i];

		C_SYMBOL(memset)(ers, 0, sizeof(ers));
		_des_permute_sbox(ers, er);
		C_SYMBOL(memset)(rn, 0, sizeof(rn));
		_des_permute_p(rn, ers);

		for (int32_t i = 0; i < 4; i++)
			rn[i] ^= l[i];

		for (int32_t i = 0; i < 4; i++) {
			l[i] = r[i];
			r[i] = rn[i];
		}
	}

	for (int32_t i = 0; i < 4; i++) {
		lr[i] = r[i];
		lr[i + 4] = l[i];
	}

	C_SYMBOL(memset)(buf, 0, 8);
	_des_permute_ip1(buf, lr);
}
