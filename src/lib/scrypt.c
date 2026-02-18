/* @file: scrypt.c
 * #desc:
 *    the implementations of scrypt password-based key derivation function.
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
#include <demoz/lib/pbkdf2.h>
#include <demoz/lib/scrypt.h>


/* @def: _ */
#define PACK4(x) ((uint32_t)((x)[0]) | (uint32_t)((x)[1]) << 8 \
	| (uint32_t)((x)[2]) << 16 | (uint32_t)((x)[3]) << 24)

#define ROTL32(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

#define QR(x, a, b, c, d) \
	(x)[b] ^= ROTL32((x)[a] + (x)[d], 7); \
	(x)[c] ^= ROTL32((x)[b] + (x)[a], 9); \
	(x)[d] ^= ROTL32((x)[c] + (x)[b], 13); \
	(x)[a] ^= ROTL32((x)[d] + (x)[c], 18)
/* end */

/* @func: _salsa208_block (static)
 * #desc:
 *    salsa20/8 block keystream function.
 *
 * #1: state buffer
 */
static void _salsa208_block(uint32_t state[16])
{
	uint32_t tmp[16];

	for (int32_t i = 0; i < 16; i++)
		tmp[i] = state[i];

	for (int32_t i = 0; i < 8; i += 2) {
		/* column */
		QR(tmp, 0, 4, 8, 12);
		QR(tmp, 5, 9, 13, 1);
		QR(tmp, 10, 14, 2, 6);
		QR(tmp, 15, 3, 7, 11);

		/* diagonal */
		QR(tmp, 0, 1, 2, 3);
		QR(tmp, 5, 6, 7, 4);
		QR(tmp, 10, 11, 8, 9);
		QR(tmp, 15, 12, 13, 14);
	}

	for (int32_t i = 0; i < 16; i++)
		state[i] += tmp[i];
}

/* @func: _scrypt_blockmix
 * #desc:
 *    scrypt block mix algorithm function.
 *
 * #1: scrypt block
 * #2: temporary buffer of y
 * #3: block number
 */
static void _scrypt_blockmix(uint8_t *b, uint8_t *y, uint32_t r)
{
	uint8_t *y0 = y, *y1 = y + (SCRYPT_ELEMENTSIZE * r);
	uint8_t *x = b + (SCRYPT_BLOCKSIZE * r - SCRYPT_ELEMENTSIZE);
	uint8_t *t = b;

	for (uint32_t j = 0; j < SCRYPT_ELEMENTSIZE; j++)
		y0[j] = x[j] ^ b[j];
	_salsa208_block((uint32_t *)y0);

	b += SCRYPT_ELEMENTSIZE;
	for (uint32_t j = 0; j < SCRYPT_ELEMENTSIZE; j++)
		y1[j] = y0[j] ^ b[j];
	_salsa208_block((uint32_t *)y1);

	for (uint32_t i = 1; i < r; i++) {
		b += SCRYPT_ELEMENTSIZE;
		y0 += SCRYPT_ELEMENTSIZE;
		for (uint32_t j = 0; j < SCRYPT_ELEMENTSIZE; j++)
			y0[j] = y1[j] ^ b[j];
		_salsa208_block((uint32_t *)y0);

		b += SCRYPT_ELEMENTSIZE;
		y1 += SCRYPT_ELEMENTSIZE;
		for (uint32_t j = 0; j < SCRYPT_ELEMENTSIZE; j++)
			y1[j] = y0[j] ^ b[j];
		_salsa208_block((uint32_t *)y1);
	}

	C_SYMBOL(memcpy)(t, y, SCRYPT_BLOCKSIZE * r);
}

/* @func: _scrypt_romix
 * #desc:
 *    scrypt rounds mix algorithm function.
 *
 * #1: scrypt block
 * #2: cpu/memory cost
 * #3: block number
 * #4: temporary buffer of v
 * #5: temporary buffer of y
 */
static void _scrypt_romix(uint8_t *b, uint32_t n, uint32_t r, uint8_t *v,
		uint8_t *y)
{
	uint8_t *t = v;
	uint32_t w = r * SCRYPT_BLOCKSIZE;

	for (uint32_t i = 0; i < n; i++) {
		C_SYMBOL(memcpy)(t, b, w);
		t += w;
		_scrypt_blockmix(b, y, r);
	}

	for (uint32_t i = 0; i < n; i++) {
		uint32_t j = PACK4(b + w - SCRYPT_ELEMENTSIZE) & (n - 1);
		for (uint32_t k = 0; k < w; k++)
			b[k] ^= v[j * w + k];
		_scrypt_blockmix(b, y, r);
	}
}

/* @func: scrypt
 * #desc:
 *    scrypt password-based key derivation function.
 *
 * #1: password
 * #2: password length
 * #3: salt
 * #4: salt length
 * #5: temporary buffer
 * #6: cpu/memory cost
 * #7: block number
 * #8: parallelization number
 * #9: output key
 * #10: key length
 */
void F_SYMBOL(scrypt)(const uint8_t *pass, uint32_t pass_len,
		const uint8_t *salt, uint32_t salt_len, uint8_t *tmp,
		uint32_t n, uint32_t r, uint32_t p, uint8_t *dk,
		uint32_t len)
{
	uint32_t b_len = SCRYPT_TMP_B(r, p);
	uint8_t *b = tmp;
	uint8_t *v = b + b_len;
	uint8_t *y = v + SCRYPT_TMP_V(r, n);

	F_SYMBOL(pbkdf2_sha256)(pass, pass_len, salt, salt_len,
		b, b_len, 1);

	for (uint32_t i = 0; i < p; i++)
		_scrypt_romix(b + i * r * SCRYPT_BLOCKSIZE, n, r, v, y);

	F_SYMBOL(pbkdf2_sha256)(pass, pass_len, b, b_len,
		dk, len, 1);
}
