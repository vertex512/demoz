/* @file: nist-p256.c
 * #desc:
 *    The implementations of nist p-256 ecdh and ecdsa.
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
#include <demoz/lib/ecc.h>


/* @def: _
 * P = 2^256 - 2^224 + 2^192 + 2^96 - 1 */
static const uint32_t _fp256_p[8] = {
	0xffffffff, 0xffffffff, 0xffffffff, 0x00000000,
	0x00000000, 0x00000000, 0x00000001, 0xffffffff
	};

/* p_R = 2^256 % P */
static const uint32_t _fp256_p_R[8] = {
	0x00000001, 0x00000000, 0x00000000, 0xffffffff,
	0xffffffff, 0xffffffff, 0xfffffffe, 0x00000000
	};

/* A = -3 % P */
static const uint32_t _p256_a[8] = {
	0xfffffffc, 0xffffffff, 0xffffffff, 0x00000000,
	0x00000000, 0x00000000, 0x00000001, 0xffffffff
	};

/*
 * B = 4105836372515214212932612978004726840911444101599372555483525631403
 *     9467401291
 */
static const uint32_t _p256_b[8] = {
	0x27d2604b, 0x3bce3c3e, 0xcc53b0f6, 0x651d06b0,
	0x769886bc, 0xb3ebbd55, 0xaa3a93e7, 0x5ac635d8
	};

/*
 * Q = 1157920892103562487626974469494075735299969552241357603424222590610
 *     68512044369
 */
static const uint32_t _sc256_q[8] = {
	0xfc632551, 0xf3b9cac2, 0xa7179e84, 0xbce6faad,
	0xffffffff, 0xffffffff, 0x00000000, 0xffffffff
	};

/*
 * Bx = 484395612939064517590525852527979142027629495260417479958440807170
 *      82404635286
 * By = 361342509567497957985851279195878819566111066729850150718771982535
 *      68414405109
 */
struct p256_point {
	uint32_t x[8];
	uint32_t y[8];
	uint32_t z[8]; /* jacobian coordinates */
};

static const struct p256_point _p256_base = {
	{
		0xd898c296, 0xf4a13945, 0x2deb33a0, 0x77037d81,
		0x63a440f2, 0xf8bce6e5, 0xe12c4247, 0x6b17d1f2
	},
	{
		0x37bf51f5, 0xcbb64068, 0x6b315ece, 0x2bce3357,
		0x7c0f9e16, 0x8ee7eb4a, 0xfe1a7f9b, 0x4fe342e2
	},
	{
		0, 0, 0, 0, 0, 0, 0, 0
	}
	};

/*
 * R = 2^256 % Q
 *   26959946660873538059280334323273029441504803697035324946844617595567
 */
static const uint32_t _sc256_R[8] = {
	0x039cdaaf, 0x0c46353d, 0x58e8617b, 0x43190552,
	0, 0, 0xffffffff, 0
	};
/* end */

/* @func: _fp256_swap (static)
 * #desc:
 *    constant-time the numerical swap.
 *
 * #1: a   [in/out] number
 * #2: b   [in/out] number
 * #3: bit [in]     low bit (0: keep, 1: swap)
 */
static void _fp256_swap(uint32_t a[8], uint32_t b[8], uint32_t bit)
{
	bit = (~bit & 1) - 1;
	for (int32_t i = 0; i < 8; i++) {
		uint32_t t = bit & (a[i] ^ b[i]);
		a[i] ^= t;
		b[i] ^= t;
	}
}

/* @func: _fp256_move (static)
 * #desc:
 *    constant-time the move numerical.
 *
 * #1: a   [in/out] number
 * #2: b   [in]     number
 * #3: bit [in]     low bit (0: keep, 1: swap)
 */
static void _fp256_move(uint32_t a[8], const uint32_t b[8], uint32_t bit)
{
	bit = (~bit & 1) - 1;
	for (int32_t i = 0; i < 8; i++) {
		uint32_t t = bit & (a[i] ^ b[i]);
		a[i] ^= t;
	}
}

/* @func: _fp256_iszero (static)
 * #desc:
 *    check if a number is zero in constant-time.
 *
 * #1: a [in]  number
 * #r:   [ret] 0: non-zero, 1: is zero
 */
static uint32_t _fp256_iszero(const uint32_t a[8])
{
	uint32_t r = 0;
	for (int32_t i = 0; i < (8 * 2); i++)
		r |= ((uint16_t *)a)[i];

	/*
	 * 0x0000ffff - 1 == 0x0000fffe
	 *   0x0000fffe >> 31 == 0x00000000
	 * 0x00000000 - 1 == 0xffffffff
	 *   0xffffffff >> 31 == 0x00000001
	 */
	return ((r - 1) >> 31) & 1;
}

/* @func: _fp256_add (static)
 * #desc:
 *    prime field addition.
 *
 * #1: r [out] sum
 * #2: a [in]  addend
 * #3: b [in]  addend
 */
static void _fp256_add(uint32_t r[8],
		const uint32_t a[8], const uint32_t b[8])
{
	uint32_t carry = 0, carry2 = 0;
	uint64_t tmp = 0;

	/* r = a + b */
	for (int32_t i = 0; i < 8; i++) {
		tmp = (uint64_t)a[i] + b[i] + carry2;
		r[i] = tmp & 0xffffffff;
		carry2 = tmp >> 32;
	}

	/* r = r % p modular reduction */
	carry2 = ~(((uint64_t)carry2 - 1) >> 32);
	for (int32_t i = 0; i < 8; i++) {
		tmp = (uint64_t)r[i] - (_fp256_p[i] & carry2) + (int32_t)carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
}

/* @func: _fp256_sub (static)
 * #desc:
 *    prime field subtraction.
 *
 * #1: r [out] difference
 * #2: a [in]  minuend
 * #3: b [in]  subtract
 */
static void _fp256_sub(uint32_t r[8],
		const uint32_t a[8], const uint32_t b[8])
{
	uint32_t carry = 0, carry2 = 0;
	uint64_t tmp = 0;

	/* r = a - b */
	for (int32_t i = 0; i < 8; i++) {
		tmp = (uint64_t)a[i] - b[i] + (int32_t)carry2;
		r[i] = tmp & 0xffffffff;
		carry2 = tmp >> 32;
	}

	/* r = r % p modular reduction */
	carry2 = ~(((uint64_t)carry2 - 1) >> 32);
	for (int32_t i = 0; i < 8; i++) {
		tmp = (uint64_t)r[i] + (_fp256_p[i] & carry2) + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
}

/* @func: _fp256_mul (static)
 * #desc:
 *    prime field multiplication.
 *
 * #1: r [out] product
 * #2: a [in]  multiplicand
 * #3: b [in]  multiplier
 */
static void _fp256_mul(uint32_t r[8],
		const uint32_t a[8], const uint32_t b[8])
{
	uint32_t rr[16], t[8];
	uint32_t carry = 0;
	uint64_t tmp = 0;

	for (int32_t i = 0; i < 16; i++)
		rr[i] = 0;

	/* rr = a * b */
	for (int32_t i = 0; i < 8; i++) {
		tmp = (uint64_t)a[0] * b[i] + carry;
		rr[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
	rr[8] = carry;

	for (int32_t i = 1; i < 8; i++) {
		carry = 0;
		for (int32_t j = 0; j < 8; j++) {
			tmp = (uint64_t)a[i] * b[j] + rr[i + j] + carry;
			rr[i + j] = tmp & 0xffffffff;
			carry = tmp >> 32;
		}
		rr[i + 8] = carry;
	}

	/* NOTE: NIST.FIPS.186-4 -- D.2.3 */

	/* r = rr % p modular reduction fold */
	for (int32_t i = 0; i < 8; i++)
		r[i] = rr[i];

	/* B  = T + 2S1 + 2S2 + S3 + S4 - D1 - D2 - D3 - D4 */

	/* S1 = 0  |0  |0  |A11|A12|A13|A14|A15 */
	t[0] = t[1] = t[2] = 0;
	t[3] = rr[11]; t[4] = rr[12]; t[5] = rr[13];
	t[6] = rr[14]; t[7] = rr[15];

	_fp256_add(t, t, t);
	_fp256_add(r, r, t);

	/* S2 = 0  |0  |0  |A12|A13|A14|A15|0 */
	t[0] = t[1] = t[2] = 0;
	t[3] = rr[12]; t[4] = rr[13]; t[5] = rr[14]; t[6] = rr[15];
	t[7] = 0;

	_fp256_add(t, t, t);
	_fp256_add(r, r, t);

	/* S3 = A8 |A9 |A10|0  |0  |0  |A14|A15 */
	t[0] = rr[8]; t[1] = rr[9]; t[2] = rr[10];
	t[3] = t[4] = t[5] = 0;
	t[6] = rr[14]; t[7] = rr[15];

	_fp256_add(r, r, t);

	/* S4 = A9 |A10|A11|A13|A14|A15|A13|A8 */
	t[0] = rr[9]; t[1] = rr[10]; t[2] = rr[11];
	t[3] = rr[13]; t[4] = rr[14]; t[5] = rr[15];
	t[6] = rr[13];
	t[7] = rr[8];

	_fp256_add(r, r, t);

	/* D1 = A11|A12|A13|0  |0  |0  |A8 |A10 */
	t[0] = rr[11]; t[1] = rr[12]; t[2] = rr[13];
	t[3] = t[4] = t[5] = 0;
	t[6] = rr[8];
	t[7] = rr[10];

	_fp256_sub(r, r, t);

	/* D2 = A12|A13|A14|A15|0  |0  |A9 |A11 */
	t[0] = rr[12]; t[1] = rr[13]; t[2] = rr[14]; t[3] = rr[15];
	t[4] = t[5] = 0;
	t[6] = rr[9];
	t[7] = rr[11];

	_fp256_sub(r, r, t);

	/* D3 = A13|A14|A15|A8 |A9 |A10|0  |A12 */
	t[0] = rr[13]; t[1] = rr[14]; t[2] = rr[15];
	t[3] = rr[8]; t[4] = rr[9]; t[5] = rr[10];
	t[6] = 0;
	t[7] = rr[12];

	_fp256_sub(r, r, t);

	/* D4 = A14|A15|0  |A9 |A10|A11|0  |A13 */
	t[0] = rr[14]; t[1] = rr[15];
	t[2] = 0;
	t[3] = rr[9]; t[4] = rr[10]; t[5] = rr[11];
	t[6] = 0;
	t[7] = rr[13];

	_fp256_sub(r, r, t);
}

/* @func: _fp256_mod (static)
 * #desc:
 *    prime field modular reduction (norm).
 *
 * #1: r [in/out] number
 */
static void _fp256_mod(uint32_t r[8])
{
	uint32_t carry = 0, carry2 = 0;
	uint64_t tmp = 0;

	/* if r < p : adjust carry2 */
	for (int32_t i = 0; i < 8; i++) {
		tmp = (uint64_t)r[i] + _fp256_p_R[i] + carry2;
		carry2 = tmp >> 32;
	}

	/* r = r % p modular reduction */
	carry2 = ~(((uint64_t)carry2 - 1) >> 32);
	for (int32_t i = 0; i < 8; i++) {
		tmp = (uint64_t)r[i] - (_fp256_p[i] & carry2) + (int32_t)carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
}

/* @func: _fp256_inv (static)
 * #desc:
 *    modular inversion based on fermat's little theorem.
 *
 * #1: r [out] inverse modulus
 * #2: z [in]  number
 */
static void _fp256_inv(uint32_t r[8], const uint32_t z[8])
{
	uint32_t x[8], e[8];
	for (int32_t i = 0; i < 8; i++) {
		x[i] = z[i];
		e[i] = _fp256_p[i];
		r[i] = 0;
	}
	e[0] -= 2;
	r[0] = 1;

	for (int32_t i = 0; i < 256; i++) {
		uint32_t k_i = e[i / 32] >> (i % 32);
		if (k_i & 1)
			_fp256_mul(r, r, x);
		_fp256_mul(x, x, x);
	}
}

/* @func: _sc256_modw (static)
 * #desc:
 *    scalar high-word modular reduction fold.
 *
 * #1: r [out] result
 * #2: a [in]  number
 * #3: b [in]  high-word
 */
static void _sc256_modw(uint32_t r[8],
		const uint32_t a[8], uint32_t b)
{
	uint32_t rr[8];
	uint32_t carry = 0, carry2 = 0;
	uint64_t tmp = 0;

	/* rr = b * R */
	for (int32_t i = 0; i < 8; i++) {
		tmp = (uint64_t)b * _sc256_R[i] + carry;
		rr[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	/* r = a + rr */
	for (int32_t i = 0; i < 8; i++) {
		tmp = (uint64_t)a[i] + rr[i] + carry2;
		r[i] = tmp & 0xffffffff;
		carry2 = tmp >> 32;
	}

	/* rr = carry2 * R */
	carry = 0;
	for (int32_t i = 0; i < 8; i++) {
		tmp = (uint64_t)carry2 * _sc256_R[i] + carry;
		rr[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	/* r = r + rr */
	carry = 0;
	for (int32_t i = 0; i < 8; i++) {
		tmp = (uint64_t)r[i] + rr[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
}

/* @func: _sc256_mod (static)
 * #desc:
 *    scalar modular reduction (norm).
 *
 * #1: r [in/out] number
 */
static void _sc256_mod(uint32_t r[8])
{
	uint32_t carry = 0, carry2 = 0;
	uint64_t tmp = 0;

	/* r = r - q */
	for (int32_t i = 0; i < 8; i++) {
		tmp = (uint64_t)r[i] - _sc256_q[i] + (int32_t)carry2;
		r[i] = tmp & 0xffffffff;
		carry2 = tmp >> 32;
	}

	/* r = if r < q : adjust r */
	carry2 = ~(((uint64_t)carry2 - 1) >> 32);
	for (int32_t i = 0; i < 8; i++) {
		tmp = (uint64_t)r[i] + (_sc256_q[i] & carry2) + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
}

/* @func: _sc256_add (static)
 * #desc:
 *    scalar addition.
 *
 * #1: r [out] sum
 * #2: a [in]  addend
 * #3: b [in]  addend
 */
static void _sc256_add(uint32_t r[8],
		const uint32_t a[8], const uint32_t b[8])
{
	uint32_t carry = 0;
	uint64_t tmp = 0;

	/* r = a + b */
	for (int32_t i = 0; i < 8; i++) {
		tmp = (uint64_t)a[i] + b[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	/* r = r % q */
	_sc256_modw(r, r, carry);
}

/* @func: _sc256_mul (static)
 * #desc:
 *    scalar multiplication.
 *
 * #1: r [out] product
 * #2: a [in]  multiplicand
 * #3: b [in]  multiplier
 */
static void _sc256_mul(uint32_t r[8],
		const uint32_t a[8], const uint32_t b[8])
{
	uint32_t rr[16];
	uint32_t carry = 0;
	uint64_t tmp = 0;

	for (int32_t i = 0; i < 16; i++)
		rr[i] = 0;

	/* rr = a * b */
	for (int32_t i = 0; i < 8; i++) {
		tmp = (uint64_t)a[0] * b[i] + carry;
		rr[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
	rr[8] = carry;

	for (int32_t i = 1; i < 8; i++) {
		carry = 0;
		for (int32_t j = 0; j < 8; j++) {
			tmp = (uint64_t)a[i] * b[j] + rr[i + j] + carry;
			rr[i + j] = tmp & 0xffffffff;
			carry = tmp >> 32;
		}
		rr[i + 8] = carry;
	}

	/* r = rr % q */
	_sc256_modw(rr + 7, rr + 7, rr[15]);
	_sc256_modw(rr + 6, rr + 6, rr[14]);
	_sc256_modw(rr + 5, rr + 5, rr[13]);
	_sc256_modw(rr + 4, rr + 4, rr[12]);
	_sc256_modw(rr + 3, rr + 3, rr[11]);
	_sc256_modw(rr + 2, rr + 2, rr[10]);
	_sc256_modw(rr + 1, rr + 1, rr[9]);
	_sc256_modw(r, rr, rr[8]);
}

/* @func: _sc256_inv (static)
 * #desc:
 *    modular inversion based on fermat's little theorem.
 *
 * #1: r [out] inverse modulus
 * #2: z [in]  number
 */
static void _sc256_inv(uint32_t r[8], const uint32_t z[8])
{
	uint32_t x[8], e[8];
	for (int32_t i = 0; i < 8; i++) {
		x[i] = z[i];
		e[i] = _sc256_q[i];
		r[i] = 0;
	}
	e[0] -= 2;
	r[0] = 1;

	for (int32_t i = 0; i < 256; i++) {
		uint32_t k_i = e[i / 32] >> (i % 32);
		if (k_i & 1)
			_sc256_mul(r, r, x);
		_sc256_mul(x, x, x);
	}
}

/* @func: _sc256_digest (static)
 * #desc:
 *    sha256 digest modular reduction.
 *
 * #1: dig [in]  sha256 digest
 * #2: r   [out] result
 */
static void _sc256_digest(const uint8_t dig[32], uint32_t r[8])
{
	/* big endian */
	for (int32_t i = 0; i < 32; i++)
		((uint8_t *)r)[31 - i] = dig[i];

	/* C_SYMBOL(memcpy)(r, dig, 32); */

	/* r = r % q */
	_sc256_mod(r);
}

/* @func: _p256_sub (static)
 * #desc:
 *    nonprime field subtraction.
 *
 * #1: r [out] difference
 * #2: a [in]  minuend
 * #3: b [in]  subtract
 * #r:   [ret] overflow
 */
static uint32_t _p256_sub(uint32_t r[8],
		const uint32_t a[8], const uint32_t b[8])
{
	uint32_t carry = 0;
	uint64_t tmp = 0;

	/* r = a - b */
	for (int32_t i = 0; i < 8; i++) {
		tmp = (uint64_t)a[i] - b[i] + (int32_t)carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	return carry;
}

/* @func: _p256_point_add (static)
 * #desc:
 *    jacobian-affine curve point addition.
 *
 * #1: xyz1 [in]  addend
 * #2: xyz2 [in]  addend
 * #3: xyz3 [out] result
 */
static void _p256_point_add(const struct p256_point *xyz1,
		const struct p256_point *xyz2, struct p256_point *xyz3)
{
	uint32_t H1[8], R1[8], H2[8], H3[8], C1[8], T1[8], T2[8];
	/*
	 * H1 = (x2 * (z1 ^ 2)) - x1
	 * R1 = (y2 * (z1 ^ 3)) - y1
	 * H2 = H1 ^ 2
	 * H3 = H1 ^ 3
	 * C1 = x1 * H2
	 * x3 = (R1 ^ 2) - (C1 * 2) - H3
	 * y3 = (R1 * (C1 - x3)) - (y1 * H3)
	 * z3 = z1 * H1
	 */

	/* NOTE: jacobian-affine point addition in CMO98 */

	/* H1 = (x2 * (z1 ^ 2)) - x1 */
	_fp256_mul(T1, xyz1->z, xyz1->z);
	_fp256_mul(H1, T1, xyz2->x);
	_fp256_sub(H1, H1, xyz1->x);

	/* R1 = (y2 * (z1 ^ 3)) - y1 */
	_fp256_mul(R1, T1, xyz1->z);
	_fp256_mul(R1, R1, xyz2->y);
	_fp256_sub(R1, R1, xyz1->y);

	/* H2 = H1 ^ 2 */
	_fp256_mul(H2, H1, H1);
	/* H3 = H1 ^ 3 */
	_fp256_mul(H3, H2, H1);
	/* C1 = x1 * H2 */
	_fp256_mul(C1, xyz1->x, H2);

	/* x3 = (R1 ^ 2) - (C1 * 2) - H3 */
	_fp256_mul(T1, R1, R1);
	_fp256_sub(T1, T1, C1);
	_fp256_sub(T1, T1, C1);
	_fp256_sub(xyz3->x, T1, H3);

	/* y3 = (R1 * (C1 - x3)) - (y1 * H3) */
	_fp256_sub(T1, C1, xyz3->x);
	_fp256_mul(T1, T1, R1);
	_fp256_mul(T2, xyz1->y, H3);
	_fp256_sub(xyz3->y, T1, T2);

	/* z3 = z1 * H1 */
	_fp256_mul(xyz3->z, xyz1->z, H1);
}

/* @func: _p256_point_double (static)
 * #desc:
 *    jacobian curve point doubling.
 *
 * #1: xyz1 [in]  addend
 * #2: xyz3 [out] result
 */
static void _p256_point_double(const struct p256_point *xyz1,
		struct p256_point *xyz3)
{
	uint32_t M1[8], S1[8], U1[8], T1[8];
	/*
	 * M1 = ((x1 ^ 2) * 3) + (a * (z1 ^ 4))
	 * S1 = (x1 * 4) * (y1 ^ 2)
	 * U1 = (y1 ^ 4) * 8
	 * z3 = (y1 * z1) * 2
	 * x3 = (M1 ^ 2) - (S1 * 2)
	 * y3 = (M1 * (S1 - x3)) - U1
	 */

	/* NOTE: jacobian point doubling in CMO98 */

	/* M1 = ((x1 ^ 2) * 3) + (a * (z1 ^ 4)) */
	_fp256_mul(T1, xyz1->x, xyz1->x);
	_fp256_add(M1, T1, T1);
	_fp256_add(M1, M1, T1);
	_fp256_mul(T1, xyz1->z, xyz1->z);
	_fp256_mul(T1, T1, T1);
	_fp256_mul(T1, T1, _p256_a);
	_fp256_add(M1, M1, T1);

	/* S1 = (x1 * 4) * (y1 ^ 2) */
	_fp256_add(S1, xyz1->x, xyz1->x);
	_fp256_add(S1, S1, S1);
	_fp256_mul(T1, xyz1->y, xyz1->y);
	_fp256_mul(S1, S1, T1);

	/* U1 = (y1 ^ 4) * 8 */
	_fp256_mul(T1, T1, T1);
	_fp256_add(T1, T1, T1);
	_fp256_add(T1, T1, T1);
	_fp256_add(U1, T1, T1);

	/* z3 = (y1 * z1) * 2 */
	_fp256_mul(T1, xyz1->y, xyz1->z);
	_fp256_add(xyz3->z, T1, T1);

	/* x3 = (M1 ^ 2) - (S1 * 2) */
	_fp256_mul(T1, M1, M1);
	_fp256_sub(T1, T1, S1);
	_fp256_sub(xyz3->x, T1, S1);

	/* y3 = (M1 * (S1 - x3)) - U1 */
	_fp256_sub(T1, S1, xyz3->x);
	_fp256_mul(T1, T1, M1);
	_fp256_sub(xyz3->y, T1, U1);
}

/* @func: _p256_point_to_affine (static)
 * #desc:
 *    jacobian to affine coordinates conversion.
 *
 * #1: xyz1 [in]  jacobian point
 * #2: xy2  [out] affine point
 */
static void _p256_point_to_affine(const struct p256_point *xyz1,
		struct p256_point *xy2)
{
	uint32_t z[8], t[8];

	/* z = inv(z1) */
	_fp256_inv(z, xyz1->z);

	/* x2 = (x1 * (z ^ 2)) % p */
	_fp256_mul(t, z, z);
	_fp256_mul(xy2->x, xyz1->x, t);
	_fp256_mod(xy2->x);

	/* y2 = (y1 * (z ^ 3)) % p */
	_fp256_mul(t, t, z);
	_fp256_mul(xy2->y, xyz1->y, t);
	_fp256_mod(xy2->y);
}

/* @func: _p256_scalar_mul (static)
 * #desc:
 *    p-256 signed binary ladder scalar multiplication.
 *
 * #1: k   [in]  input point
 * #2: xy1 [in]  base point
 * #3: xy2 [out] scalar
 */
static void _p256_scalar_mul(const uint32_t k[8],
		const struct p256_point *xy1, struct p256_point *xy2)
{
	struct p256_point _xyz1, _xyz2;
	uint32_t t[8], y_neg[8], neg = 0;
	for (int32_t i = 0; i < 8; i++) {
		_xyz1.x[i] = xy1->x[i];
		_xyz1.y[i] = xy1->y[i];
		_xyz1.z[i] = 0;
		_xyz2.x[i] = xy1->x[i];
		y_neg[i] = 0;
	}
	_xyz1.z[0] = 1;
	/*
	 * _xyz1 = (x1, y1, 1)
	 * _xyz2 = (x1, y1 ? y_neg)
	 */

	/* t = if k & 1 : move k */
	_p256_sub(t, _sc256_q, k);
	neg = ~k[0] & 1;
	_fp256_move(t, k, ~neg);

	/* y_neg = -y1 */
	_fp256_sub(y_neg, y_neg, xy1->y);
	_fp256_move(_xyz1.y, y_neg, neg);

	/* signed binary ladder and jacobian-affine coordinates */
	for (int32_t i = 255; i > 0; i--) {
		uint32_t k_i = t[i / 32] >> (i % 32);
		_fp256_move(_xyz2.y, xy1->y, k_i ^ neg);
		_fp256_move(_xyz2.y, y_neg, ~k_i ^ neg);
		/* R = (R * 2) +- P */

		_p256_point_double(&_xyz1, &_xyz1);
		_p256_point_add(&_xyz1, &_xyz2, &_xyz1);
	}

	_p256_point_to_affine(&_xyz1, xy2);
}

/* @func: _p256_check_key (static)
 * #desc:
 *    input point legality check.
 *
 * #1: k [in]  input point
 * #r:   [ret] 0: no error, -1: error
 */
static int32_t _p256_check_key(const uint32_t k[8])
{
	uint32_t t[8];
	if (_fp256_iszero(k) || !_p256_sub(t, k, _sc256_q))
		return -1;

	return 0;
}

/* @func: _p256_check_point (static)
 * #desc:
 *    curve point legality check.
 *
 * #1: xy1 [in]  curve point
 * #r:     [ret] 0: no error, -1: error
 */
static int32_t _p256_check_point(const struct p256_point *xy1)
{
	uint32_t t1[8], t2[8];

	if (!_p256_sub(t1, xy1->x, _fp256_p))
		return -1;
	if (!_p256_sub(t1, xy1->y, _fp256_p))
		return -1;

	/* t1 = ((x1 ^ 3) - (x1 * 3) + b) % p */
	_fp256_mul(t1, xy1->x, xy1->x);
	_fp256_mul(t1, t1, xy1->x);
	_fp256_sub(t1, t1, xy1->x);
	_fp256_sub(t1, t1, xy1->x);
	_fp256_sub(t1, t1, xy1->x);
	_fp256_add(t1, t1, _p256_b);
	_fp256_mod(t1);

	/* t2 = (y1 ^ 2) % p */
	_fp256_mul(t2, xy1->y, xy1->y);
	_fp256_mod(t2);

	_p256_sub(t1, t1, t2);

	return _fp256_iszero(t1) - 1;
}

/* @func: _p256_affine_add (static)
 * #desc:
 *    affine curve point addition.
 *
 * #1: xy1 [in]  addend
 * #2: xy2 [in]  addend
 * #3: xy3 [out] result
 */
static void _p256_affine_add(const struct p256_point *xy1,
		const struct p256_point *xy2, struct p256_point *xy3)
{
	struct p256_point _xyz1, _xyz2, _xyz3, _xy4;
	uint32_t t[8], iswap1, iswap2;
	for (int32_t i = 0; i < 8; i++) {
		_xyz1.x[i] = xy1->x[i];
		_xyz1.y[i] = xy1->y[i];
		_xyz1.z[i] = 0;
		_xyz2.x[i] = xy2->x[i];
		_xyz2.y[i] = xy2->y[i];
		_xyz2.z[i] = 0;
		_xy4.x[i] = 0;
		_xy4.y[i] = 0;
	}
	_xyz1.z[0] = 1;
	_xyz2.z[0] = 1;

	_p256_sub(t, xy1->x, xy2->x);
	iswap1 = _fp256_iszero(t);
	_p256_sub(t, xy1->y, xy2->y);
	iswap2 = _fp256_iszero(t);

	/* P != +- Q */
	_p256_point_add(&_xyz1, &_xyz2, &_xyz1);
	/* P == Q */
	_p256_point_double(&_xyz1, &_xyz3);

	/* xyz1 = if x1 == x2 && y1 == y2 : swap xyz3 */
	_fp256_swap(_xyz1.x, _xyz3.x, iswap1 & iswap2);
	_fp256_swap(_xyz1.y, _xyz3.y, iswap1 & iswap2);
	_fp256_swap(_xyz1.z, _xyz3.z, iswap1 & iswap2);

	_p256_point_to_affine(&_xyz1, xy3);

	/* xy3 = if x1 == x2 && y1 != y2 : adjust zero */
	_fp256_swap(xy3->x, _xy4.x, iswap1 & ~iswap2);
	_fp256_swap(xy3->y, _xy4.y, iswap1 & ~iswap2);
}

/* @func: ecdh_p256_public_key
 * #desc:
 *    p-256 public key create function.
 *
 * #1: pri [in]  private key
 * #2: pub [out] public key
 * #r:     [ret] 0: success, -1: fail
 */
int32_t F_SYMBOL(ecdh_p256_public_key)(const uint8_t *pri, uint8_t *pub)
{
	uint32_t _pri[8];
	struct p256_point xy1;

	C_SYMBOL(memcpy)(_pri, pri, ECDH_P256_PRI_LEN);
	if (_p256_check_key(_pri))
		return -1;

	/* xy1 = scalar(_pri, base) */
	_p256_scalar_mul(_pri, &_p256_base, &xy1);

	C_SYMBOL(memcpy)(pub, xy1.x, ECDH_P256_LEN);
	C_SYMBOL(memcpy)(pub + ECDH_P256_LEN, xy1.y, ECDH_P256_LEN);

	return 0;
}

/* @func: ecdh_p256_shared_key
 * #desc:
 *    p-256 shared key create function.
 *
 * #1: pri [in]  private key
 * #2: pub [in]  public key
 * #3: key [in]  shared key
 * #r:     [ret] 0: success, -1: fail
 */
int32_t F_SYMBOL(ecdh_p256_shared_key)(const uint8_t *pri,
		const uint8_t *pub, uint8_t *key)
{
	uint32_t _pri[8];
	struct p256_point xy1, xy2;

	C_SYMBOL(memcpy)(_pri, pri, ECDH_P256_PRI_LEN);
	if (_p256_check_key(_pri))
		return -1;

	C_SYMBOL(memcpy)(xy1.x, pub, ECDH_P256_LEN);
	C_SYMBOL(memcpy)(xy1.y, pub + ECDH_P256_LEN, ECDH_P256_LEN);
	if (_p256_check_point(&xy1))
		return -1;

	/* xy2 = scalar(_pri, xy1) */
	_p256_scalar_mul(_pri, &xy1, &xy2);

	C_SYMBOL(memcpy)(key, xy2.x, ECDH_P256_KEY_LEN);

	return 0 - _fp256_iszero(xy2.x);
}

/* @func: ecdsa_p256_public_key
 * #desc:
 *    p-256 public key create function.
 *
 * #1: pri [in]  private key
 * #2: pub [out] public key
 */
void F_SYMBOL(ecdsa_p256_public_key)(const uint8_t *pri, uint8_t *pub)
{
	uint32_t _pri[8];
	struct p256_point xy1;

	C_SYMBOL(memcpy)(_pri, pri, ECDSA_P256_PRI_LEN);

	/* xy1 = scalar(_pri, base) */
	_p256_scalar_mul(_pri, &_p256_base, &xy1);

	C_SYMBOL(memcpy)(pub, xy1.x, ECDSA_P256_LEN);
	C_SYMBOL(memcpy)(pub + ECDSA_P256_LEN, xy1.y, ECDSA_P256_LEN);
}

/* @func: ecdsa_p256_sign
 * #desc:
 *    p-256 signature function.
 *
 * #1: pri  [in]  private key
 * #2: ran  [in]  nonce
 * #3: msg  [in]  input message
 * #4: len  [in]  message length
 * #5: sign [out] signature
 * #r:      [ret] 0: success, -1: fail
 */
int32_t F_SYMBOL(ecdsa_p256_sign)(const uint8_t *pri, const uint8_t *ran,
		const uint8_t *msg, uint32_t len, uint8_t *sign)
{
	uint32_t _pri[8], _ran[8], z[8], s[8];
	struct p256_point xy1;
	SHA256_NEW(ctx);

	C_SYMBOL(memcpy)(_pri, pri, ECDSA_P256_PRI_LEN);
	C_SYMBOL(memcpy)(_ran, ran, ECDSA_P256_RAN_LEN);

	/* x1 = scalar(_ran, base) */
	_p256_scalar_mul(_ran, &_p256_base, &xy1);
	_sc256_mod(xy1.x);

	if (_fp256_iszero(xy1.x))
		return -1;

	/* z = sha(msg) % q */
	F_SYMBOL(sha256_init)(&ctx);
	F_SYMBOL(sha256)(&ctx, msg, len);
	_sc256_digest(&(SHA256_STATE(&ctx, 0)), z);

	/* s = (x1 * _pri) + z */
	_sc256_mul(s, xy1.x, _pri);
	_sc256_add(s, s, z);

	/* s = (s * inv(_ran)) % q */
	_sc256_inv(z, _ran);
	_sc256_mul(s, s, z);
	_sc256_mod(s);

	if (_fp256_iszero(s))
		return -1;

	C_SYMBOL(memcpy)(sign, xy1.x, ECDSA_P256_LEN);
	C_SYMBOL(memcpy)(sign + ECDSA_P256_LEN, s, ECDSA_P256_LEN);

	return 0;
}

/* @func: ecdsa_p256_verify
 * #desc:
 *    p-256 signature verification function.
 *
 * #1: pub  [in]  public key
 * #2: sign [in]  signature
 * #3: msg  [in]  input message
 * #4: len  [in]  message length
 * #r:      [ret] 0: success, -1: fail
 */
int32_t F_SYMBOL(ecdsa_p256_verify)(const uint8_t *pub,
		const uint8_t *sign, const uint8_t *msg, uint32_t len)
{
	uint32_t z[8], t[8], u1[8], u2[8];
	struct p256_point xy1, xy2, xy3;
	SHA256_NEW(ctx);

	/* xy1 = sign */
	C_SYMBOL(memcpy)(xy1.x, sign, ECDSA_P256_LEN);
	C_SYMBOL(memcpy)(xy1.y, sign + ECDSA_P256_LEN, ECDSA_P256_LEN);
	if (_p256_check_key(xy1.x) || _p256_check_key(xy1.y))
		return -1;

	/* xy2 = pub */
	C_SYMBOL(memcpy)(xy2.x, pub, ECDSA_P256_LEN);
	C_SYMBOL(memcpy)(xy2.y, pub + ECDSA_P256_LEN, ECDSA_P256_LEN);
	if (_p256_check_point(&xy2))
		return -1;

	/* z = sha(msg) % q */
	F_SYMBOL(sha256_init)(&ctx);
	F_SYMBOL(sha256)(&ctx, msg, len);
	_sc256_digest(&(SHA256_STATE(&ctx, 0)), z);

	/* t = inv(y1) */
	_sc256_inv(t, xy1.y);

	/* u1 = (z * t) % q */
	_sc256_mul(u1, z, t);
	_sc256_mod(u1);

	/* u2 = (x1 * t) % q */
	_sc256_mul(u2, xy1.x, t);
	_sc256_mod(u2);

	/* xy2 = scalar(u2, xy2) */
	_p256_scalar_mul(u2, &xy2, &xy2);
	/* xy3 = scalar(u1, base) */
	_p256_scalar_mul(u1, &_p256_base, &xy3);

	/* x2 = add(xy3, xy2) % q */
	_p256_affine_add(&xy3, &xy2, &xy2);
	_sc256_mod(xy2.x);

	_p256_sub(t, xy1.x, xy2.x);

	return _fp256_iszero(t) - 1;
}
