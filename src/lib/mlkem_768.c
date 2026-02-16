/* @file: mlkem_768.c
 * #desc:
 *    The implementations of module-lattice-based key-encapsulation
 *    mechanism (768'192).
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
#include <demoz/lib/mlkem.h>


/* @def: _
 *  |     n   q    k eta1 eta2 du dv ek   dk   ct-size k-size|
 *  +========================================================+
 *  | 512 256 3329 2 3    2    10 4   800 1632  768    32    |
 *  | 768 256 3329 3 2    2    10 4  1184 2400 1088    32    |
 *  |1024 256 3329 4 2    2    11 5  1568 3168 1568    32    |
 */
#define MLKEM_MONT -1044   /* 2^16 mod q == 2285 */
#define MLKEM_MONT_R2 1353 /* 2^32 mod q == 1353 */ 
#define MLKEM_QINV -3327   /* q^-1 mod 2^16 == 62209 */
#define MLKEM_NORM 1441    /* mont^2*128^-1 (mod 3329) == 1441 */

/* butterfly operations for root of unity */
static const int16_t zetas[MLKEM_N / 2] = {
	-1044,  -758,  -359, -1517,  1493,  1422,   287,   202,
	 -171,   622,  1577,   182,   962, -1202, -1474,  1468,
	  573, -1325,   264,   383,  -829,  1458, -1602,  -130,
	 -681,  1017,   732,   608, -1542,   411,  -205, -1571,
	 1223,   652,  -552,  1015, -1293,  1491,  -282, -1544,
	  516,    -8,  -320,  -666, -1618, -1162,   126,  1469,
	 -853,   -90,  -271,   830,   107, -1421,  -247,  -951,
	 -398,   961, -1508,  -725,   448, -1065,   677, -1275,
	-1103,   430,   555,   843, -1251,   871,  1550,   105,
	  422,   587,   177,  -235,  -291,  -460,  1574,  1653,
	 -246,   778,  1159,  -147,  -777,  1483,  -602,  1119,
	-1590,   644,  -872,   349,   418,   329,  -156,   -75,
	  817,  1097,   603,   610,  1322, -1285, -1465,   384,
	-1215,  -136,  1218, -1335,  -874,   220, -1187, -1659,
	-1185, -1530, -1278,   794, -1510,  -854,  -870,   478,
	 -108,  -308,   996,   991,   958, -1460,  1522,  1628
	};

struct poly {
	int16_t coeffs[MLKEM_N];
};

struct polyvec {
	struct poly vec[MLKEM_768_K];
};

#define MONT_MUL(a, b) _montgomery_reduce((int32_t)(a) * (b))
#define REDUCE_ONCE(a) _barrett_reduce((int16_t)a)
/* end */

/* @func: _montgomery_reduce (static)
 * #desc:
 *    montgomery reduction (-q+1..q-1).
 *
 * #1: number
 * #r: result
 */
static int16_t _montgomery_reduce(int32_t a)
{
	int16_t t = (int16_t)a * MLKEM_QINV;
	t = (a - (int32_t)t * MLKEM_Q) >> 16;

	return t;
}

/* @func: _barrett_reduce (static)
 * #desc:
 *    barrett reduction (-(q-1)/2..(q-1)/2).
 *
 * #1: number
 * #r: result
 */
static int16_t _barrett_reduce(int16_t a)
{
	/* 20159 = ((1 << 26) + MLKEM_Q / 2) / MLKEM_Q */
	int16_t t = ((int32_t)a * 20159 + (1 << 25)) >> 26;
	t *= MLKEM_Q;

	return a - t;
}

/* @func: _poly_reduce (static)
 * #desc:
 *    polynomial barrett reduction.
 *
 * #1: polynomial
 */
static void _poly_reduce(struct poly *r)
{
	for (int32_t i = 0; i < MLKEM_N; i++)
		r->coeffs[i] = REDUCE_ONCE(r->coeffs[i]);
}

/* @func: _poly_ntt (static)
 * #desc:
 *    polynomial number-theoretic transform.
 *
 * #1: polynomial
 */
static void _poly_ntt(struct poly *r)
{
	uint32_t len, start, j, k;
	int16_t t, zeta;
	int16_t *rr = r->coeffs;

	k = 1;
	for (len = MLKEM_N / 2; len >= 2; len >>= 1) {
		for (start = 0; start < MLKEM_N; start = j + len) {
			zeta = zetas[k++];
			for (j = start; j < start + len; j++) {
				t = MONT_MUL(zeta, rr[j + len]);
				rr[j + len] = rr[j] - t;
				rr[j] = rr[j] + t;
			}
		}
	}

	for (j = 0; j < MLKEM_N; j++)
		rr[j] = REDUCE_ONCE(rr[j]);
}

/* @func: _poly_invntt (static)
 * #desc:
 *    inplace inverse number-theoretic transform and multiplication
 *    by montgomery factor.
 *
 * #1: polynomial
 */
static void _poly_invntt(struct poly *r)
{
	uint32_t start, len, j, k;
	int16_t t, zeta;
	int16_t *rr = r->coeffs;

	k = MLKEM_N / 2 - 1;
	for (len = 2; len <= (MLKEM_N / 2); len <<= 1) {
		for (start = 0; start < MLKEM_N; start = j + len) {
			zeta = zetas[k--];
			for (j = start; j < start + len; j++) {
				t = rr[j];
				rr[j] = REDUCE_ONCE(t + rr[j + len]);
				rr[j + len] = MONT_MUL(zeta, rr[j + len] - t);
			}
		}
	}

	for (j = 0; j < MLKEM_N; j++)
		rr[j] = MONT_MUL(rr[j], MLKEM_NORM);
}

/* @func: _poly_basemul (static)
 * #desc:
 *    polynomial multiplication with montgomery reduction in ntt domain.
 *
 * #1: output polynomial
 * #2: input polynomial
 * #3: input polynomial
 */
static void _poly_basemul(struct poly *r,
		const struct poly *a, const struct poly *b)
{
	int32_t k;
	int16_t t0, t1, a0, a1, b0, b1, zeta;

	k = MLKEM_N / 4;
	for (int32_t i = 0; i < MLKEM_N; i += 4) {
		zeta = zetas[k++];

		a0 = a->coeffs[i];
		a1 = a->coeffs[i + 1];
		b0 = b->coeffs[i];
		b1 = b->coeffs[i + 1];
		t0 = MONT_MUL(a1, b1);
		t0 = MONT_MUL(t0, zeta);
		t0 += MONT_MUL(a0, b0);
		t1 = MONT_MUL(a0, b1);
		t1 += MONT_MUL(a1, b0);
		r->coeffs[i] = t0;
		r->coeffs[i + 1] = t1;

		a0 = a->coeffs[i + 2];
		a1 = a->coeffs[i + 3];
		b0 = b->coeffs[i + 2];
		b1 = b->coeffs[i + 3];
		t0 = MONT_MUL(a1, b1);
		t0 = MONT_MUL(t0, -zeta);
		t0 += MONT_MUL(a0, b0);
		t1 = MONT_MUL(a0, b1);
		t1 += MONT_MUL(a1, b0);
		r->coeffs[i + 2] = t0;
		r->coeffs[i + 3] = t1;
	}
}

/* @func: _poly_mont (static)
 * #desc:
 *    coefficients of a polynomial from normal domain to montgomery domain.
 *
 * #1: polynomial
 */
static void _poly_mont(struct poly *r)
{
	for (int32_t i = 0; i < MLKEM_N; i++)
		r->coeffs[i] = MONT_MUL(r->coeffs[i], MLKEM_MONT_R2);
}

/* @func: _poly_add (static)
 * #desc:
 *    polynomial additional (no modular reduction).
 *
 * #1: output polynomial
 * #2: input polynomial
 * #3: input polynomial
 */
static void _poly_add(struct poly *r,
		const struct poly *a, const struct poly *b)
{
	for (int32_t i = 0; i < MLKEM_N; i++)
		r->coeffs[i] = a->coeffs[i] + b->coeffs[i];
}

/* @func: _poly_sub (static)
 * #desc:
 *    polynomial subtraction (no modular reduction).
 *
 * #1: output polynomial
 * #2: input polynomial
 * #3: input polynomial
 */
static void _poly_sub(struct poly *r,
		const struct poly *a, const struct poly *b)
{
	for (int32_t i = 0; i < MLKEM_N; i++)
		r->coeffs[i] = a->coeffs[i] - b->coeffs[i];
}

/* @func: _poly_compress_du (static)
 * #desc:
 *    compression polynomial u vector.
 *
 * #1: input polynomial
 * #2: output buffer
 */
static void _poly_compress_du(const struct poly *a, uint8_t *r)
{
	uint16_t t[4];
	uint64_t u;

	for (int32_t i = 0; i < MLKEM_N; i += 4) {
		for (int32_t j = 0; j < 4; j++) {
				t[j] = a->coeffs[i + j];
				t[j] += ((int16_t)t[j] >> 15) & MLKEM_Q;
				u = t[j];
				/* ((u << 10) + MLKEM_Q / 2) / MLKEM_Q */
				u <<= 10;
				u += MLKEM_Q / 2;
				u *= 1290167;
				u >>= 32;
				t[j] = u & 0x3ff;
		}

		r[0] = t[0];
		r[1] = t[0] >> 8 | t[1] << 2;
		r[2] = t[1] >> 6 | t[2] << 4;
		r[3] = t[2] >> 4 | t[3] << 6;
		r[4] = t[3] >> 2;
		r += 5;
	}
}

/* @func: _poly_decompress_du (static)
 * #desc:
 *    decompression polynomial u vector.
 *
 * #1: input buffer
 * #2: output polynomial
 */
static void _poly_decompress_du(const uint8_t *a, struct poly *r)
{
	uint16_t t[4];

	for (int32_t i = 0; i < MLKEM_N; i += 4) {
		t[0] = (uint16_t)a[0] | (uint16_t)a[1] << 8;
		t[1] = (uint16_t)a[1] >> 2 | (uint16_t)a[2] << 6;
		t[2] = (uint16_t)a[2] >> 4 | (uint16_t)a[3] << 4;
		t[3] = (uint16_t)a[3] >> 6 | (uint16_t)a[4] << 2;
		a += 5;

		for (int32_t j = 0; j < 4; j++) {
			r->coeffs[i + j] = ((uint32_t)(t[j] & 0x3ff)
				* MLKEM_Q + 512) >> 10;
		}
	}
}

/* @func: _poly_compress_dv (static)
 * #desc:
 *    compression polynomial v vector.
 *
 * #1: input polynomial
 * #2: output buffer
 */
static void _poly_compress_dv(const struct poly *a, uint8_t *r)
{
	uint8_t t[8];
	int16_t d;
	uint64_t u;

	for (int32_t i = 0; i < MLKEM_N; i += 8) {
		for (int32_t j = 0; j < 8; j++) {
			d = a->coeffs[i + j];
			d += (d >> 15) & MLKEM_Q;
			/* ((u << 4) + KYBER_Q / 2) / KYBER_Q */
			u = d << 4;
			u += MLKEM_Q / 2;
			u *= 80635;
			u >>= 28;
			t[j] = u & 0xf;
		}

		r[0] = t[0] | t[1] << 4;
		r[1] = t[2] | t[3] << 4;
		r[2] = t[4] | t[5] << 4;
		r[3] = t[6] | t[7] << 4;
		r += 4;
	}
}

/* @func: _poly_decompress_dv (static)
 * #desc:
 *    decompression polynomial v vector.
 *
 * #1: input buffer
 * #2: output polynomial
 */
static void _poly_decompress_dv(const uint8_t *a, struct poly *r)
{
	for (int32_t i = 0; i < MLKEM_N; i += 2) {
		r->coeffs[i] = ((uint16_t)(a[0] & 15) * MLKEM_Q + 8) >> 4;
		r->coeffs[i + 1] = ((uint16_t)(a[0] >> 4) * MLKEM_Q + 8) >> 4;
		a++;
	}
}

/* @func: _poly_tobytes (static)
 * #desc:
 *    polynomial to bytes conversion.
 *
 * #1: input polynomial
 * #2: output buffer
 */
static void _poly_tobytes(const struct poly *a, uint8_t *r)
{
	uint16_t t0, t1;

	for (int32_t i = 0; i < MLKEM_N; i += 2) {
		t0 = a->coeffs[i];
		t0 += ((int16_t)t0 >> 15) & MLKEM_Q;
		t1 = a->coeffs[i + 1];
		t1 += ((int16_t)t1 >> 15) & MLKEM_Q;

		r[0] = t0;
		r[1] = t0 >> 8 | t1 << 4;
		r[2] = t1 >> 4;
		r += 3;
	}
}

/* @func: _poly_frombytes (static)
 * #desc:
 *    bytes to polynomial conversion.
 *
 * #1: output polynomial
 * #2: input buffer
 */
static void _poly_frombytes(struct poly *r, const uint8_t *a)
{
	uint16_t t0, t1;

	for (int32_t i = 0; i < MLKEM_N; i += 2) {
		t0 = (uint16_t)a[0] | (uint16_t)a[1] << 8;
		t1 = (uint16_t)a[1] >> 4 | (uint16_t)a[2] << 4;
		a += 3;

		r->coeffs[i] = t0 & 0xfff;
		r->coeffs[i + 1] = t1 & 0xfff;
	}
}

/* @func: _poly_tomsg (static)
 * #desc:
 *    polynomial to 32-bytes message conversion.
 *
 * #1: input polynomial
 * #2: output buffer
 */
static void _poly_tomsg(const struct poly *a, uint8_t *r)
{
	uint64_t t;

	for (int32_t i = 0; i < MLKEM_N; i += 8) {
		*r = 0;
		for (int32_t j = 0; j < 8; j++) {
			t = a->coeffs[i + j];
			/* ((t << 1) + MLKEM_Q / 2) / MLKEM_Q */
			t <<= 1;
			t += MLKEM_Q / 2;
			t *= 80635;
			t >>= 28;
			t &= 1;
			*r |= t << j;
		}
		r++;
	}
}

/* @func: _poly_frommsg (static)
 * #desc:
 *    32-bytes message to polynomial conversion.
 *
 * #1: output polynomial
 * #2: input buffer
 */
static void _poly_frommsg(struct poly *r, const uint8_t *a)
{
	uint8_t t;

	for (int32_t i = 0; i < MLKEM_N; i += 8) {
		t = *a++;
		for (int32_t j = 0; j < 8; j++) {
			r->coeffs[i + j] = (MLKEM_Q + 1) / 2;
			r->coeffs[i + j] &= ~((t & 1) - 1);
			t >>= 1;
		}
	}
}

/* @func: _polyvec_compress (static)
 * #desc:
 *     compression polynomial vector u.
 *
 * #1: input polynomial vector
 * #2: output buffer
 */
static void _polyvec_compress(const struct polyvec *a, uint8_t *r)
{
	for (int32_t i = 0; i < MLKEM_768_K; i++) {
		_poly_compress_du(&a->vec[i], r);
		r += MLKEM_768_POLY_COMPRESS_DU;
	}
}

/* @func: _polyvec_decompress (static)
 * #desc:
 *     decompression polynomial vector u.
 *
 * #1: input buffer
 * #2: output polynomial vector
 */
static void _polyvec_decompress(const uint8_t *a, struct polyvec *r)
{
	for (int32_t i = 0; i < MLKEM_768_K; i++) {
		_poly_decompress_du(a, &r->vec[i]);
		a += MLKEM_768_POLY_COMPRESS_DU;
	}
}

/* @func: _polyvec_tobytes (static)
 * #desc:
 *    polynomial vector to bytes conversion.
 *
 * #1: input polynomial vector
 * #2: output buffer
 */
static void _polyvec_tobytes(const struct polyvec *a, uint8_t *r)
{
	for (int32_t i = 0; i < MLKEM_768_K; i++) {
		_poly_tobytes(&a->vec[i], r);
		r += MLKEM_POLY_BYTES;
	}
}

/* @func: _polyvec_frombytes (static)
 * #desc:
 *    bytes to polynomial vector conversion.
 *
 * #1: input buffer
 * #2: output polynomial vector
 */
static void _polyvec_frombytes(struct polyvec *r, const uint8_t *a)
{
	for (int32_t i = 0; i < MLKEM_768_K; i++) {
		_poly_frombytes(&r->vec[i], a);
		a += MLKEM_POLY_BYTES;
	}
}

/* @func: _polyvec_ntt (static)
 * #desc:
 *    polynomial vector number-theoretic transform.
 *
 * #1: polynomial vector
 */
static void _polyvec_ntt(struct polyvec *r)
{
	for (int32_t i = 0; i < MLKEM_768_K; i++)
		_poly_ntt(&r->vec[i]);
}

/* @func: _polyvec_invntt (static)
 * #desc:
 *    polynomial vector inverse number-theoretic transform.
 *
 * #1: polynomial vector
 */
static void _polyvec_invntt(struct polyvec *r)
{
	for (int32_t i = 0; i < MLKEM_768_K; i++)
		_poly_invntt(&r->vec[i]);
}

/* @func: _polyvec_basemul (static)
 * #desc:
 *    polynomial vector matrix multiplication.
 *
 * #1: output polynomial
 * #2: input polynomial vector
 * #3: input polynomial vector
 */
static void _polyvec_basemul(struct poly *r,
		const struct polyvec *a, const struct polyvec *b)
{
	struct poly t;

	_poly_basemul(r, &a->vec[0], &b->vec[0]);
	for (int32_t i = 1; i < MLKEM_768_K; i++) {
		_poly_basemul(&t, &a->vec[i], &b->vec[i]);
		_poly_add(r, r, &t);
	}

	_poly_reduce(r);
}

/* @func: _polyvec_reduce (static)
 * #desc:
 *    polynomial vector reduction.
 *
 * #1: polynomial vector
 */
static void _polyvec_reduce(struct polyvec *r)
{
	for (int32_t i = 0; i < MLKEM_768_K; i++)
		_poly_reduce(&r->vec[i]);
}

/* @func: _poly_add (static)
 * #desc:
 *    polynomial vector additional (no modular reduction).
 *
 * #1: output polynomial vector
 * #2: input polynomial vector
 * #3: input polynomial vector
 */
static void _polyvec_add(struct polyvec *r,
		const struct polyvec *a, const struct polyvec *b)
{
	for (int32_t i = 0; i < MLKEM_768_K; i++)
		_poly_add(&r->vec[i], &a->vec[i], &b->vec[i]);
}

/* @func: _sample_ntt (static)
 * #desc:
 *    uniform sampling of ntt representations.
 *
 * #1: output coeffs
 * #2: coeffs number
 * #3: input buffer
 * #4: input length
 * #r: return sample number
 */
static uint32_t _sample_ntt(int16_t *r, uint32_t n, const uint8_t *buf,
		uint32_t len)
{
	uint16_t d1, d2;
	uint32_t k = 0;

	while (k < n && len > 2) {
		d1 = ((uint16_t)buf[0] | (uint16_t)buf[1] << 8) & 0xfff;
		d2 = ((uint16_t)buf[1] >> 4 | (uint16_t)buf[2] << 4) & 0xfff;
		buf += 3;
		len -= 3;

		if (d1 < MLKEM_Q)
			r[k++] = d1;
		if (d2 < MLKEM_Q && k < n)
			r[k++] = d2;
	}

	return k;
}

/* @func: _sample_ntt_xof (static)
 * #desc:
 *    uniform sampling of ntt representations and xof.
 *
 * #1: output polynomial
 * #2: input seed
 * #3: nonce x
 * #4: nonce y
 */
static void _sample_ntt_xof(struct poly *r, const uint8_t *seed,
		uint8_t x, uint8_t y)
{
	uint8_t buf[SHA3_SHAKE128_RATE];
	SHA3_NEW(ctx);

	buf[0] = x;
	buf[1] = y;
	F_SYMBOL(sha3_init)(&ctx, SHA3_SHAKE128_TYPE, 0);
	F_SYMBOL(sha3_process)(&ctx, seed, 32);
	F_SYMBOL(sha3_process)(&ctx, buf, 2);
	F_SYMBOL(sha3_finish)(&ctx);

	uint32_t n = 0;
	do {
		F_SYMBOL(sha3_shake_xof)(&ctx, buf, sizeof(buf));
		n += _sample_ntt(r->coeffs + n, MLKEM_N - n, buf, sizeof(buf));
	} while (n < MLKEM_N);
}

/* @func: _poly_cbd2 (static)
 * #desc:
 *    extract the polynomial of the central binomial distribution 2.
 *
 * #1: output polynomial
 * #2: input buffer
 */
static void _poly_cbd2(struct poly *r, const uint8_t *buf)
{
	uint32_t t, d;
	int16_t a, b;

	for (int32_t i = 0; i < MLKEM_N; i += 8) {
		t = (uint32_t)buf[0]
			| (uint32_t)buf[1] << 8
			| (uint32_t)buf[2] << 16
			| (uint32_t)buf[3] << 24;
		d = t & 0x55555555;
		d += (t >> 1) & 0x55555555;
		buf += 4;

		for (int32_t j = 0; j < 8; j++) {
			a = d & 0x3;
			d >>= 2;
			b = d & 0x3;
			d >>= 2;
			r->coeffs[i + j] = a - b;
		}
	}
}

/* @func: _sample_poly_eta1_prf (static)
 * #desc:
 *    uniformly sample from the distribution eta1.
 *
 * #1: output polynomial
 * #2: input seed
 * #3: nonce
 */
static void _sample_poly_eta1_prf(struct poly *r, const uint8_t *seed,
		uint8_t ran)
{
	uint8_t buf[MLKEM_768_ETA1 * MLKEM_N / 4];
	SHA3_NEW(ctx);

	buf[0] = ran;
	F_SYMBOL(sha3_init)(&ctx, SHA3_SHAKE256_TYPE, 0);
	F_SYMBOL(sha3_process)(&ctx, seed, MLKEM_SYM_LEN);
	F_SYMBOL(sha3_process)(&ctx, buf, 1);
	F_SYMBOL(sha3_finish)(&ctx);
	F_SYMBOL(sha3_shake_xof)(&ctx, buf, sizeof(buf));

	_poly_cbd2(r, buf);
}

/* @func: _sample_poly_eta2_prf (static)
 * #desc:
 *    uniformly sample from the distribution eta2.
 *
 * #1: output polynomial
 * #2: input seed
 * #3: nonce
 */
static void _sample_poly_eta2_prf(struct poly *r, const uint8_t *seed,
		uint8_t ran)
{
	uint8_t buf[MLKEM_768_ETA2 * MLKEM_N / 4];
	SHA3_NEW(ctx);

	buf[0] = ran;
	F_SYMBOL(sha3_init)(&ctx, SHA3_SHAKE256_TYPE, 0);
	F_SYMBOL(sha3_process)(&ctx, seed, MLKEM_SYM_LEN);
	F_SYMBOL(sha3_process)(&ctx, buf, 1);
	F_SYMBOL(sha3_finish)(&ctx);
	F_SYMBOL(sha3_shake_xof)(&ctx, buf, sizeof(buf));

	_poly_cbd2(r, buf);
}

/* @func: _hash_h (static)
 * #desc:
 *    output sha3-256 digest (32-bytes).
 *
 * #1: input buffer
 * #2: input length
 * #3: output digest
 */
static void _hash_h(const uint8_t *in, uint32_t len, uint8_t *out)
{
	SHA3_NEW(ctx);

	F_SYMBOL(sha3_init)(&ctx, SHA3_256_TYPE, 0);
	F_SYMBOL(sha3_process)(&ctx, in, len);
	F_SYMBOL(sha3_finish)(&ctx);
	C_SYMBOL(memcpy)(out, &(SHA3_STATE(&ctx, 0)), SHA3_256_LEN);
}

/* @func: _hash_g (static)
 * #desc:
 *    output sha3-512 digest (64-bytes).
 *
 * #1: input buffer
 * #2: input length
 * #3: output digest
 */
static void _hash_g(const uint8_t *in, uint32_t len, uint8_t *out)
{
	SHA3_NEW(ctx);

	F_SYMBOL(sha3_init)(&ctx, SHA3_512_TYPE, 0);
	F_SYMBOL(sha3_process)(&ctx, in, len);
	F_SYMBOL(sha3_finish)(&ctx);
	C_SYMBOL(memcpy)(out, &(SHA3_STATE(&ctx, 0)), SHA3_512_LEN);
}

/* @func: _hash_j (static)
 * #desc:
 *    output sha3 shake256 digest (32-bytes).
 *
 * #1: input buffer
 * #2: input length
 * #3: output digest
 */
static void _hash_j(const uint8_t *in, uint32_t len, uint8_t *out)
{
	SHA3_NEW(ctx);

	F_SYMBOL(sha3_init)(&ctx, SHA3_SHAKE256_TYPE, MLKEM_SYM_LEN);
	F_SYMBOL(sha3_process)(&ctx, in, len);
	F_SYMBOL(sha3_finish)(&ctx);
	C_SYMBOL(memcpy)(out, &(SHA3_STATE(&ctx, 0)), MLKEM_SYM_LEN);
}

/* @func: mlkem768_pke_genkey
 * #desc:
 *    mlkem768 k-pke generate encryption and decryption key function.
 *
 * #1: nonce
 * #2: encryption key
 * #3: decryption key
 */
void F_SYMBOL(mlkem768_pke_genkey)(const uint8_t *ran, uint8_t *ekp,
		uint8_t *dkp)
{
	uint8_t buf[MLKEM_SYM_LEN * 2], nonce = 0;
	const uint8_t *seed = buf;
	const uint8_t *noise = buf + MLKEM_SYM_LEN;
	struct polyvec a[MLKEM_768_K], s, e, t;

	/* (seed, noise) = hash_g(ran + k) */
	C_SYMBOL(memcpy)(buf, ran, MLKEM_RAN_LEN);
	buf[MLKEM_RAN_LEN] = MLKEM_768_K;
	_hash_g(buf, MLKEM_RAN_LEN + 1, buf);

	/* a = gen_matrix(seed, j, i) */
	for (int32_t i = 0; i < MLKEM_768_K; i++) {
		for (int32_t j = 0; j < MLKEM_768_K; j++) {
			_sample_ntt_xof(&a[i].vec[j], seed, j, i);
		}
	}

	/* s = eta1(noise, nonce++) */
	for (int32_t i = 0; i < MLKEM_768_K; i++)
		_sample_poly_eta1_prf(&s.vec[i], noise, nonce++);

	/* e = eta1(noise, nonce++) */
	for (int32_t i = 0; i < MLKEM_768_K; i++)
		_sample_poly_eta1_prf(&e.vec[i], noise, nonce++);

	/* s = ntt(s) */
	_polyvec_ntt(&s);
	/* e = ntt(e) */
	_polyvec_ntt(&e);

	/* t = mont(a * s) */
	for (int32_t i = 0; i < MLKEM_768_K; i++) {
		_polyvec_basemul(&t.vec[i], &a[i], &s);
		_poly_mont(&t.vec[i]);
	}

	/* t = redc(t + e) */
	_polyvec_add(&t, &t, &e);
	_polyvec_reduce(&t);

	C_SYMBOL(memcpy)(ekp + MLKEM_768_POLYVEC_BYTES, seed, MLKEM_SYM_LEN);
	_polyvec_tobytes(&t, ekp);
	_polyvec_tobytes(&s, dkp);
}

/* @func: mlkem768_pke_encrypt
 * #desc:
 *    mlkem768 k-pke encryption function.
 *
 * #1: nonce
 * #2: encryption key
 * #3: input nmessage
 * #4: output ciphertext
 */
void F_SYMBOL(mlkem768_pke_encrypt)(const uint8_t *ran, const uint8_t *ekp,
		const uint8_t *msg, uint8_t *ct)
{
	uint8_t seed[MLKEM_SYM_LEN], nonce = 0;
	struct polyvec a[MLKEM_768_K], r, e, u, t;
	struct poly e2, v, m;

	_polyvec_frombytes(&t, ekp);
	C_SYMBOL(memcpy)(seed, ekp + MLKEM_768_POLYVEC_BYTES, MLKEM_SYM_LEN);
	_poly_frommsg(&m, msg);

	/* a = gen_matrix(seed, i, j) */
	for (int32_t i = 0; i < MLKEM_768_K; i++) {
		for (int32_t j = 0; j < MLKEM_768_K; j++) {
			_sample_ntt_xof(&a[i].vec[j], seed, i, j);
		}
	}

	/* r = eta1(ran, nonce++) */
	for (int32_t i = 0; i < MLKEM_768_K; i++)
		_sample_poly_eta1_prf(&r.vec[i], ran, nonce++);

	/* e = eta2(ran, nonce++) */
	for (int32_t i = 0; i < MLKEM_768_K; i++)
		_sample_poly_eta2_prf(&e.vec[i], ran, nonce++);

	/* e2 = eta2(ran, nonce) */
	_sample_poly_eta2_prf(&e2, ran, nonce);

	/* r = ntt(r) */
	_polyvec_ntt(&r);

	/* u = a * r */
	for (int32_t i = 0; i < MLKEM_768_K; i++)
		_polyvec_basemul(&u.vec[i], &a[i], &r);

	/* u = redc(invntt(u) + e) */
	_polyvec_invntt(&u);
	_polyvec_add(&u, &u, &e);
	_polyvec_reduce(&u);

	/* v = invntt(t * r) */
	_polyvec_basemul(&v, &t, &r);
	_poly_invntt(&v);

	/* v = redc(v + e2 + m) */
	_poly_add(&v, &v, &e2);
	_poly_add(&v, &v, &m);
	_poly_reduce(&v);

	_polyvec_compress(&u, ct);
	_poly_compress_dv(&v, ct + MLKEM_768_POLYVEC_COMPRESS);
}

/* @func: mlkem768_pke_decrypt
 * #desc:
 *    mlkem768 k-pke decryption function.
 *
 * #1: decryption key
 * #2: input ciphertext
 * #3: output message
 */
void F_SYMBOL(mlkem768_pke_decrypt)(const uint8_t *dkp, const uint8_t *ct,
		uint8_t *msg)
{
	struct polyvec u, s;
	struct poly v, m;

	_polyvec_frombytes(&s, dkp);
	_polyvec_decompress(ct, &u);
	_poly_decompress_dv(ct + MLKEM_768_POLYVEC_COMPRESS, &v);

	/* m = invntt(s * ntt(u)) */
	_polyvec_ntt(&u);
	_polyvec_basemul(&m, &s, &u);
	_poly_invntt(&m);

	/* m = redc(v - m) */
	_poly_sub(&m, &v, &m);
	_poly_reduce(&m);

	_poly_tomsg(&m, msg);
}

/* @func: mlkem768_genkey
 * #desc:
 *    mlkem768 generate encapsulation and decapsulation key function.
 *
 * #1: nonce of k-pke
 * #2: nonce of dk
 * #3: encapsulation key
 * #4: decapsulation key
 */
void F_SYMBOL(mlkem768_genkey)(const uint8_t *ran, const uint8_t *ran2,
		uint8_t *ek, uint8_t *dk)
{
	/* (ekp, dkp) = pke_genkey(ran) */
	F_SYMBOL(mlkem768_pke_genkey)(ran, ek, dk);

	/* dk = dkp + ekp + hash_h(ekp) + ran2 */
	dk += MLKEM_768_DKPKE_LEN;
	C_SYMBOL(memcpy)(dk, ek, MLKEM_768_EK_LEN);

	dk += MLKEM_768_EK_LEN;
	_hash_h(ek, MLKEM_768_EK_LEN, dk);

	dk += MLKEM_SYM_LEN;
	C_SYMBOL(memcpy)(dk, ran2, MLKEM_RAN_LEN);
}

/* @func: mlkem768_encaps
 * #desc:
 *    mlkem768 encapsulation the function and generate a shared key.
 *
 * #1: nonce message
 * #2: encapsulation key
 * #3: output ciphertext
 * #4: output shared key
 */
void F_SYMBOL(mlkem768_encaps)(const uint8_t *msg, const uint8_t *ek,
		uint8_t *ct, uint8_t *sk)
{
	uint8_t buf[MLKEM_SYM_LEN * 2];
	const uint8_t *k = buf;
	const uint8_t *r = buf + MLKEM_SYM_LEN;

	/* (k, r) = hash_g(msg + hash_h(ek)) */
	C_SYMBOL(memcpy)(buf, msg, MLKEM_RAN_LEN);
	_hash_h(ek, MLKEM_768_EK_LEN, buf + MLKEM_RAN_LEN);
	_hash_g(buf, MLKEM_SYM_LEN * 2, buf);

	/* sk = k */
	C_SYMBOL(memcpy)(sk, k, MLKEM_SYM_LEN);

	/* ct = pke_encrypt(r, ek, msg) */
	F_SYMBOL(mlkem768_pke_encrypt)(r, ek, msg, ct);
}

/* @func: mlkem768_decaps
 * #desc:
 *    mlkem768 decapsulation the function and generate a shared key.
 *
 * #1: decapsulation key
 * #2: input ciphertext
 * #3: output shared key
 */
void F_SYMBOL(mlkem768_decaps)(const uint8_t *dk, const uint8_t *ct,
		uint8_t *sk)
{
	uint8_t buf[MLKEM_SYM_LEN * 2], k2[MLKEM_SYM_LEN * 2],
		msg[MLKEM_SYM_LEN], ct2[MLKEM_768_CT_LEN];
	const uint8_t *k = buf;
	const uint8_t *r = buf + MLKEM_SYM_LEN;
	const uint8_t *dkp = dk;
	const uint8_t *ekp = dk += MLKEM_768_DKPKE_LEN;
	const uint8_t *h = dk += MLKEM_768_EK_LEN;
	const uint8_t *z = dk += MLKEM_SYM_LEN;

	/* msg = pke_decrypt(dkp, ct, msg) */
	F_SYMBOL(mlkem768_pke_decrypt)(dkp, ct, msg);

	/* (k, r) = hash_g(msg + h) */
	C_SYMBOL(memcpy)(buf, msg, MLKEM_SYM_LEN);
	C_SYMBOL(memcpy)(buf + MLKEM_SYM_LEN, h, MLKEM_SYM_LEN);
	_hash_g(buf, MLKEM_SYM_LEN * 2, buf);

	/* sk = k */
	C_SYMBOL(memcpy)(sk, k, MLKEM_SYM_LEN);

	/* k2 = hash_j(z + ct) */
	C_SYMBOL(memcpy)(k2, z, MLKEM_RAN_LEN);
	C_SYMBOL(memcpy)(k2 + MLKEM_RAN_LEN, ct, MLKEM_SYM_LEN);
	_hash_j(k2, MLKEM_SYM_LEN * 2, k2);

	/* ct2 = pke_encrypt(r, ekp, msg) */
	F_SYMBOL(mlkem768_pke_encrypt)(r, ekp, msg, ct2);

	/* sk = if ct != ct2 : swap k2 */
	uint32_t rr = 0;
	for (int32_t i = 0; i < MLKEM_768_CT_LEN; i++)
		rr |= ct[i] ^ ct2[i];

	rr = (((rr - 1) >> 31) & 1) - 1;
	for (int32_t i = 0; i < MLKEM_KEY_LEN; i++) {
		uint32_t t = rr & (sk[i] ^ k2[i]);
		sk[i] ^= t;
		k2[i] ^= t;
	}
}
