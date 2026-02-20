/* @file: nist-p384.c
 * #desc:
 *    The implementations of nist p-384 ecdh and ecdsa.
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
 * P = 2^384 - 2^128 - 2^96 + 2^32 - 1 */
static const uint32_t _fp384_p[12] = {
	0xffffffff, 0x00000000, 0x00000000, 0xffffffff,
	0xfffffffe, 0xffffffff, 0xffffffff, 0xffffffff,
	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff
	};

/* p_R = 2^384 % P */
static const uint32_t _fp384_p_R[12] = {
	1, 0xffffffff, 0xffffffff, 0, 1, 0, 0, 0, 0, 0, 0, 0
	};

/* A = -3 % P */
static const uint32_t _p384_a[12] = {
	0xfffffffc, 0x00000000, 0x00000000, 0xffffffff,
	0xfffffffe, 0xffffffff, 0xffffffff, 0xffffffff,
	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff
	};

/*
 * B = 2758019355995970587784901184038904809305690585636156852142870730198
 *     8689241309860865136260764883745107765439761230575
 */
static const uint32_t _p384_b[12] = {
	0xd3ec2aef, 0x2a85c8ed, 0x8a2ed19d, 0xc656398d,
	0x5013875a, 0x0314088f, 0xfe814112, 0x181d9c6e,
	0xe3f82d19, 0x988e056b, 0xe23ee7e4, 0xb3312fa7
	};

/*
 * Q = 3940200619639447921227904010014361380507973927046544666794690527962
 *     7659399113263569398956308152294913554433653942643
 */
static const uint32_t _sc384_q[12] = {
	0xccc52973, 0xecec196a, 0x48b0a77a, 0x581a0db2,
	0xf4372ddf, 0xc7634d81, 0xffffffff, 0xffffffff,
	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff
	};

/*
 * Bx = 262470350957996892686231567445669818918529234911092133878156159009
 *      25518854738050089022388053975719786650872476732087
 * By = 832571096148902998554675128952010817928785304886131559470920590248
 *      0503199884419224438643760392947333078086511627871
 */
struct p384_point {
	uint32_t x[12];
	uint32_t y[12];
	uint32_t z[12]; /* jacobian coordinates */
};

static const struct p384_point _p384_base = {
	{
		0x72760ab7, 0x3a545e38, 0xbf55296c, 0x5502f25d,
		0x82542a38, 0x59f741e0, 0x8ba79b98, 0x6e1d3b62,
		0xf320ad74, 0x8eb1c71e, 0xbe8b0537, 0xaa87ca22
	},
	{
		0x90ea0e5f, 0x7a431d7c, 0x1d7e819d, 0x0a60b1ce,
		0xb5f0b8c0, 0xe9da3113, 0x289a147c, 0xf8f41dbd,
		0x9292dc29, 0x5d9e98bf, 0x96262c6f, 0x3617de4a
	},
	{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	}
	};

/*
 * R = 2^384 % Q
 *   1388124618062372383947042015309946732620727252194336364173
 */
static const uint32_t _sc384_R[12] = {
	0x333ad68d, 0x1313e695, 0xb74f5885, 0xa7e5f24d,
	0x0bc8d220, 0x389cb27e, 0, 0, 0, 0, 0, 0
	};
/* end */

/* @func: _fp384_swap (static)
 * #desc:
 *    constant-time the numerical swap.
 *
 * #1: a   [in/out] number
 * #2: b   [in/out] number
 * #3: bit [in]     low bit (0: keep, 1: swap)
 */
static void _fp384_swap(uint32_t a[12], uint32_t b[12], uint32_t bit)
{
	bit = (~bit & 1) - 1;
	for (int32_t i = 0; i < 12; i++) {
		uint32_t t = bit & (a[i] ^ b[i]);
		a[i] ^= t;
		b[i] ^= t;
	}
}

/* @func: _fp384_move (static)
 * #desc:
 *    constant-time the move numerical.
 *
 * #1: a   [in/out] number
 * #2: b   [in]     number
 * #3: bit [in]     low bit (0: keep, 1: swap)
 */
static void _fp384_move(uint32_t a[12], const uint32_t b[12], uint32_t bit)
{
	bit = (~bit & 1) - 1;
	for (int32_t i = 0; i < 12; i++) {
		uint32_t t = bit & (a[i] ^ b[i]);
		a[i] ^= t;
	}
}

/* @func: _fp384_iszero (static)
 * #desc:
 *    check if a number is zero in constant-time.
 *
 * #1: a [in]  number
 * #r:   [ret] 0: non-zero, 1: is zero
 */
static uint32_t _fp384_iszero(const uint32_t a[12])
{
	uint32_t r = 0;
	for (int32_t i = 0; i < (12 * 2); i++)
		r |= ((uint16_t *)a)[i];

	/*
	 * 0x0000ffff - 1 == 0x0000fffe
	 *   0x0000fffe >> 31 == 0x00000000
	 * 0x00000000 - 1 == 0xffffffff
	 *   0xffffffff >> 31 == 0x00000001
	 */
	return ((r - 1) >> 31) & 1;
}

/* @func: _fp384_add (static)
 * #desc:
 *    prime field addition.
 *
 * #1: r [out] sum
 * #2: a [in]  addend
 * #3: b [in]  addend
 */
static void _fp384_add(uint32_t r[12],
		const uint32_t a[12], const uint32_t b[12])
{
	uint32_t carry = 0, carry2 = 0;
	uint64_t tmp = 0;

	/* r = a + b */
	for (int32_t i = 0; i < 12; i++) {
		tmp = (uint64_t)a[i] + b[i] + carry2;
		r[i] = tmp & 0xffffffff;
		carry2 = tmp >> 32;
	}

	/* r = r % p modular reduction */
	carry2 = ~(((uint64_t)carry2 - 1) >> 32);
	for (int32_t i = 0; i < 12; i++) {
		tmp = (uint64_t)r[i] - (_fp384_p[i] & carry2) + (int32_t)carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
}

/* @func: _fp384_sub (static)
 * #desc:
 *    prime field subtraction.
 *
 * #1: r [out] difference
 * #2: a [in]  minuend
 * #3: b [in]  subtract
 */
static void _fp384_sub(uint32_t r[12],
		const uint32_t a[12], const uint32_t b[12])
{
	uint32_t carry = 0, carry2 = 0;
	uint64_t tmp = 0;

	/* r = a - b */
	for (int32_t i = 0; i < 12; i++) {
		tmp = (uint64_t)a[i] - b[i] + (int32_t)carry2;
		r[i] = tmp & 0xffffffff;
		carry2 = tmp >> 32;
	}

	/* r = r % p modular reduction */
	carry2 = ~(((uint64_t)carry2 - 1) >> 32);
	for (int32_t i = 0; i < 12; i++) {
		tmp = (uint64_t)r[i] + (_fp384_p[i] & carry2) + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
}

/* @func: _fp384_mul (static)
 * #desc:
 *    prime field multiplication.
 *
 * #1: r [out] product
 * #2: a [in]  multiplicand
 * #3: b [in]  multiplier
 */
static void _fp384_mul(uint32_t r[12],
		const uint32_t a[12], const uint32_t b[12])
{
	uint32_t rr[24], t[12];
	uint32_t carry = 0;
	uint64_t tmp = 0;

	for (int32_t i = 0; i < 24; i++)
		rr[i] = 0;

	/* rr = a * b */
	for (int32_t i = 0; i < 12; i++) {
		tmp = (uint64_t)a[0] * b[i] + carry;
		rr[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
	rr[12] = carry;

	for (int32_t i = 1; i < 12; i++) {
		carry = 0;
		for (int32_t j = 0; j < 12; j++) {
			tmp = (uint64_t)a[i] * b[j] + rr[i + j] + carry;
			rr[i + j] = tmp & 0xffffffff;
			carry = tmp >> 32;
		}
		rr[i + 12] = carry;
	}

	/* NOTE: NIST.FIPS.186-4 -- D.2.4 */

	/* r = rr % p modular reduction fold */
	for (int32_t i = 0; i < 12; i++)
		r[i] = rr[i];

	/* B = T + 2S1 + S2 + S3 + S4 + S5 + S6 - D1 - D2 - D3 */

	/* S1 = 0  |0  |0  |0  |A21|A22|A23|0  |0  |0  |0  |0 */
	t[0] = t[1] = t[2] = t[3] = 0;
	t[4] = rr[21]; t[5] = rr[22]; t[6] = rr[23];
	t[7] = t[8] = t[9] = t[10] = t[11] = 0;

	_fp384_add(t, t, t);
	_fp384_add(r, r, t);

	/* S2 = A12|A13|A14|A15|A16|A17|A18|A19|A20|A21|A22|A23 */
	for (int32_t i = 0; i < 12; i++)
		t[i] = rr[i + 12];

	_fp384_add(r, r, t);

	/* S3 = A21|A22|A23|A12|A13|A14|A15|A16|A17|A18|A19|A20 */
	t[0] = rr[21]; t[1] = rr[22]; t[2] = rr[23];
	for (int32_t i = 3; i < 12; i++)
		t[i] = rr[i + 9];

	_fp384_add(r, r, t);

	/* S4 = 0  |A23|0  |A20|A12|A13|A14|A15|A16|A17|A18|A19 */
	t[0] = 0;
	t[1] = rr[23];
	t[2] = 0;
	t[3] = rr[20];
	for (int32_t i = 4; i < 12; i++)
		t[i] = rr[i + 8];

	_fp384_add(r, r, t);

	/* S5 = 0  |0  |0  |0  |A20|A21|A22|A23|0  |0  |0  |0 */
	t[0] = t[1] = t[2] = t[3] = 0;
	t[4] = rr[20]; t[5] = rr[21]; t[6] = rr[22]; t[7] = rr[23];
	t[8] = t[9] = t[10] = t[11] = 0;

	_fp384_add(r, r, t);

	/* S6 = A20|0  |0  |A21|A22|A23|0  |0  |0  |0  |0  |0 */
	t[0] = rr[20];
	t[1] = t[2] = 0;
	t[3] = rr[21]; t[4] = rr[22]; t[5] = rr[23];
	t[6] = t[7] = t[8] = t[9] = t[10] = t[11] = 0;

	_fp384_add(r, r, t);

	/* D1 = A23|A12|A13|A14|A15|A16|A17|A18|A19|A20|A21|A22 */
	t[0] = rr[23];
	for (int32_t i = 1; i < 12; i++)
		t[i] = rr[i + 11];

	_fp384_sub(r, r, t);

	/* D2 = 0  |A20|A21|A22|A23|0  |0  |0  |0  |0  |0  |0 */
	t[0] = 0;
	t[1] = rr[20]; t[2] = rr[21]; t[3] = rr[22]; t[4] = rr[23];
	t[5] = t[6] = t[7] = t[8] = t[9] = t[10] = t[11] = 0;

	_fp384_sub(r, r, t);

	/* D3 = 0  |0  |0  |A23|A23|0  |0  |0  |0  |0  |0  |0 */
	t[0] = t[1] = t[2] = 0;
	t[3] = t[4] = rr[23];
	t[5] = t[6] = t[7] = t[8] = t[9] = t[10] = t[11] = 0;

	_fp384_sub(r, r, t);
}

/* @func: _fp384_mod (static)
 * #desc:
 *    prime field modular reduction (norm).
 *
 * #1: r [in/out] number
 */
static void _fp384_mod(uint32_t r[12])
{
	uint32_t carry = 0, carry2 = 0;
	uint64_t tmp = 0;

	/* if r < p : adjust carry2 */
	for (int32_t i = 0; i < 12; i++) {
		tmp = (uint64_t)r[i] + _fp384_p_R[i] + carry2;
		carry2 = tmp >> 32;
	}

	/* r = r % p modular reduction */
	carry2 = ~(((uint64_t)carry2 - 1) >> 32);
	for (int32_t i = 0; i < 12; i++) {
		tmp = (uint64_t)r[i] - (_fp384_p[i] & carry2) + (int32_t)carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
}

/* @func: _fp384_inv (static)
 * #desc:
 *    modular inversion based on fermat's little theorem.
 *
 * #1: r [out] inverse modulus
 * #2: z [in]  number
 */
static void _fp384_inv(uint32_t r[12], const uint32_t z[12])
{
	uint32_t x[12], e[12];
	for (int32_t i = 0; i < 12; i++) {
		x[i] = z[i];
		e[i] = _fp384_p[i];
		r[i] = 0;
	}
	e[0] -= 2;
	r[0] = 1;

	for (int32_t i = 0; i < 384; i++) {
		uint32_t k_i = e[i / 32] >> (i % 32);
		if (k_i & 1)
			_fp384_mul(r, r, x);
		_fp384_mul(x, x, x);
	}
}

/* @func: _sc384_modw (static)
 * #desc:
 *    scalar high-word modular reduction fold.
 *
 * #1: r [out] result
 * #2: a [in]  number
 * #3: b [in]  high-word
 */
static void _sc384_modw(uint32_t r[12],
		const uint32_t a[12], uint32_t b)
{
	uint32_t rr[12];
	uint32_t carry = 0, carry2 = 0;
	uint64_t tmp = 0;

	/* rr = b * R */
	for (int32_t i = 0; i < 12; i++) {
		tmp = (uint64_t)b * _sc384_R[i] + carry;
		rr[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	/* r = a + rr */
	for (int32_t i = 0; i < 12; i++) {
		tmp = (uint64_t)a[i] + rr[i] + carry2;
		r[i] = tmp & 0xffffffff;
		carry2 = tmp >> 32;
	}

	/* rr = carry2 * R */
	carry = 0;
	for (int32_t i = 0; i < 12; i++) {
		tmp = (uint64_t)carry2 * _sc384_R[i] + carry;
		rr[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	/* r = r + rr */
	carry = 0;
	for (int32_t i = 0; i < 12; i++) {
		tmp = (uint64_t)r[i] + rr[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
}

/* @func: _sc384_mod (static)
 * #desc:
 *    scalar modular reduction (norm).
 *
 * #1: r [in/out] number
 */
static void _sc384_mod(uint32_t r[12])
{
	uint32_t carry = 0, carry2 = 0;
	uint64_t tmp = 0;

	/* r = r - q */
	for (int32_t i = 0; i < 12; i++) {
		tmp = (uint64_t)r[i] - _sc384_q[i] + (int32_t)carry2;
		r[i] = tmp & 0xffffffff;
		carry2 = tmp >> 32;
	}

	/* r = if r < q : adjust r */
	carry2 = ~(((uint64_t)carry2 - 1) >> 32);
	for (int32_t i = 0; i < 12; i++) {
		tmp = (uint64_t)r[i] + (_sc384_q[i] & carry2) + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
}

/* @func: _sc384_add (static)
 * #desc:
 *    scalar addition.
 *
 * #1: r [out] sum
 * #2: a [in]  addend
 * #3: b [in]  addend
 */
static void _sc384_add(uint32_t r[12],
		const uint32_t a[12], const uint32_t b[12])
{
	uint32_t carry = 0;
	uint64_t tmp = 0;

	/* r = a + b */
	for (int32_t i = 0; i < 12; i++) {
		tmp = (uint64_t)a[i] + b[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	/* r = r % q */
	_sc384_modw(r, r, carry);
}

/* @func: _sc384_mul (static)
 * #desc:
 *    scalar multiplication.
 *
 * #1: r [out] product
 * #2: a [in]  multiplicand
 * #3: b [in]  multiplier
 */
static void _sc384_mul(uint32_t r[12],
		const uint32_t a[12], const uint32_t b[12])
{
	uint32_t rr[24];
	uint32_t carry = 0;
	uint64_t tmp = 0;

	for (int32_t i = 0; i < 24; i++)
		rr[i] = 0;

	/* rr = a * b */
	for (int32_t i = 0; i < 12; i++) {
		tmp = (uint64_t)a[0] * b[i] + carry;
		rr[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
	rr[12] = carry;

	for (int32_t i = 1; i < 12; i++) {
		carry = 0;
		for (int32_t j = 0; j < 12; j++) {
			tmp = (uint64_t)a[i] * b[j] + rr[i + j] + carry;
			rr[i + j] = tmp & 0xffffffff;
			carry = tmp >> 32;
		}
		rr[i + 12] = carry;
	}

	/* r = rr % q */
	_sc384_modw(rr + 11, rr + 11, rr[23]);
	_sc384_modw(rr + 10, rr + 10, rr[22]);
	_sc384_modw(rr + 9, rr + 9, rr[21]);
	_sc384_modw(rr + 8, rr + 8, rr[20]);
	_sc384_modw(rr + 7, rr + 7, rr[19]);
	_sc384_modw(rr + 6, rr + 6, rr[18]);
	_sc384_modw(rr + 5, rr + 5, rr[17]);
	_sc384_modw(rr + 4, rr + 4, rr[16]);
	_sc384_modw(rr + 3, rr + 3, rr[15]);
	_sc384_modw(rr + 2, rr + 2, rr[14]);
	_sc384_modw(rr + 1, rr + 1, rr[13]);
	_sc384_modw(r, rr, rr[12]);
}

/* @func: _sc384_inv (static)
 * #desc:
 *    modular inversion based on fermat's little theorem.
 *
 * #1: r [out] inverse modulus
 * #2: z [in]  number
 */
static void _sc384_inv(uint32_t r[12], const uint32_t z[12])
{
	uint32_t x[12], e[12];
	for (int32_t i = 0; i < 12; i++) {
		x[i] = z[i];
		e[i] = _sc384_q[i];
		r[i] = 0;
	}
	e[0] -= 2;
	r[0] = 1;

	for (int32_t i = 0; i < 384; i++) {
		uint32_t k_i = e[i / 32] >> (i % 32);
		if (k_i & 1)
			_sc384_mul(r, r, x);
		_sc384_mul(x, x, x);
	}
}

/* @func: _sc384_digest (static)
 * #desc:
 *    sha384 digest modular reduction.
 *
 * #1: dig [in]  sha384 digest
 * #2: r   [out] result
 */
static void _sc384_digest(const uint8_t dig[48], uint32_t r[12])
{
	/* big endian */
	for (int32_t i = 0; i < 48; i++)
		((uint8_t *)r)[47 - i] = dig[i];

	/* C_SYMBOL(memcpy)(r, dig, 48); */

	/* r = r % q */
	_sc384_mod(r);
}

/* @func: _p384_sub (static)
 * #desc:
 *    nonprime field subtraction.
 *
 * #1: r [out] difference
 * #2: a [in]  minuend
 * #3: b [in]  subtract
 * #r:   [ret] overflow
 */
static uint32_t _p384_sub(uint32_t r[12],
		const uint32_t a[12], const uint32_t b[12])
{
	uint32_t carry = 0;
	uint64_t tmp = 0;

	/* r = a - b */
	for (int32_t i = 0; i < 12; i++) {
		tmp = (uint64_t)a[i] - b[i] + (int32_t)carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	return carry;
}

/* @func: _p384_point_add (static)
 * #desc:
 *    jacobian-affine curve point addition.
 *
 * #1: xyz1 [in]  addend
 * #2: xyz2 [in]  addend
 * #3: xyz3 [out] result
 */
static void _p384_point_add(const struct p384_point *xyz1,
		const struct p384_point *xyz2, struct p384_point *xyz3)
{
	uint32_t H1[12], R1[12], H2[12], H3[12], C1[12], T1[12], T2[12];
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
	_fp384_mul(T1, xyz1->z, xyz1->z);
	_fp384_mul(H1, T1, xyz2->x);
	_fp384_sub(H1, H1, xyz1->x);

	/* R1 = (y2 * (z1 ^ 3)) - y1 */
	_fp384_mul(R1, T1, xyz1->z);
	_fp384_mul(R1, R1, xyz2->y);
	_fp384_sub(R1, R1, xyz1->y);

	/* H2 = H1 ^ 2 */
	_fp384_mul(H2, H1, H1);
	/* H3 = H1 ^ 3 */
	_fp384_mul(H3, H2, H1);
	/* C1 = x1 * H2 */
	_fp384_mul(C1, xyz1->x, H2);

	/* x3 = (R1 ^ 2) - (C1 * 2) - H3 */
	_fp384_mul(T1, R1, R1);
	_fp384_sub(T1, T1, C1);
	_fp384_sub(T1, T1, C1);
	_fp384_sub(xyz3->x, T1, H3);

	/* y3 = (R1 * (C1 - x3)) - (y1 * H3) */
	_fp384_sub(T1, C1, xyz3->x);
	_fp384_mul(T1, T1, R1);
	_fp384_mul(T2, xyz1->y, H3);
	_fp384_sub(xyz3->y, T1, T2);

	/* z3 = z1 * H1 */
	_fp384_mul(xyz3->z, xyz1->z, H1);
}

/* @func: _p384_point_double (static)
 * #desc:
 *    jacobian curve point doubling.
 *
 * #1: xyz1 [in]  addend
 * #2: xyz3 [out] result
 */
static void _p384_point_double(const struct p384_point *xyz1,
		struct p384_point *xyz3)
{
	uint32_t M1[12], S1[12], U1[12], T1[12];
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
	_fp384_mul(T1, xyz1->x, xyz1->x);
	_fp384_add(M1, T1, T1);
	_fp384_add(M1, M1, T1);
	_fp384_mul(T1, xyz1->z, xyz1->z);
	_fp384_mul(T1, T1, T1);
	_fp384_mul(T1, T1, _p384_a);
	_fp384_add(M1, M1, T1);

	/* S1 = (x1 * 4) * (y1 ^ 2) */
	_fp384_add(S1, xyz1->x, xyz1->x);
	_fp384_add(S1, S1, S1);
	_fp384_mul(T1, xyz1->y, xyz1->y);
	_fp384_mul(S1, S1, T1);

	/* U1 = (y1 ^ 4) * 8 */
	_fp384_mul(T1, T1, T1);
	_fp384_add(T1, T1, T1);
	_fp384_add(T1, T1, T1);
	_fp384_add(U1, T1, T1);

	/* z3 = (y1 * z1) * 2 */
	_fp384_mul(T1, xyz1->y, xyz1->z);
	_fp384_add(xyz3->z, T1, T1);

	/* x3 = (M1 ^ 2) - (S1 * 2) */
	_fp384_mul(T1, M1, M1);
	_fp384_sub(T1, T1, S1);
	_fp384_sub(xyz3->x, T1, S1);

	/* y3 = (M1 * (S1 - x3)) - U1 */
	_fp384_sub(T1, S1, xyz3->x);
	_fp384_mul(T1, T1, M1);
	_fp384_sub(xyz3->y, T1, U1);
}

/* @func: _p384_point_to_affine (static)
 * #desc:
 *    jacobian to affine coordinates conversion.
 *
 * #1: xyz1 [in]  jacobian point
 * #2: xy2  [out] affine point
 */
static void _p384_point_to_affine(const struct p384_point *xyz1,
		struct p384_point *xy2)
{
	uint32_t z[12], t[12];

	/* z = inv(z1) */
	_fp384_inv(z, xyz1->z);

	/* x2 = (x1 * (z ^ 2)) % p */
	_fp384_mul(t, z, z);
	_fp384_mul(xy2->x, xyz1->x, t);
	_fp384_mod(xy2->x);

	/* y2 = (y1 * (z ^ 3)) % p */
	_fp384_mul(t, t, z);
	_fp384_mul(xy2->y, xyz1->y, t);
	_fp384_mod(xy2->y);
}

/* @func: _p384_scalar_mul (static)
 * #desc:
 *    p-384 signed binary ladder scalar multiplication.
 *
 * #1: k   [in]  input point
 * #2: xy1 [in]  base point
 * #3: xy2 [out] scalar
 */
static void _p384_scalar_mul(const uint32_t k[12],
		const struct p384_point *xy1, struct p384_point *xy2)
{
	struct p384_point _xyz1, _xyz2;
	uint32_t t[12], y_neg[12], neg = 0;
	for (int32_t i = 0; i < 12; i++) {
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
	_p384_sub(t, _sc384_q, k);
	neg = ~k[0] & 1;
	_fp384_move(t, k, ~neg);

	/* y_neg = -y1 */
	_fp384_sub(y_neg, y_neg, xy1->y);
	_fp384_move(_xyz1.y, y_neg, neg);

	/* signed binary ladder and jacobian-affine coordinates */
	for (int32_t i = 383; i > 0; i--) {
		uint32_t k_i = t[i / 32] >> (i % 32);
		_fp384_move(_xyz2.y, xy1->y, k_i ^ neg);
		_fp384_move(_xyz2.y, y_neg, ~k_i ^ neg);
		/* R = (R * 2) +- P */

		_p384_point_double(&_xyz1, &_xyz1);
		_p384_point_add(&_xyz1, &_xyz2, &_xyz1);
	}

	_p384_point_to_affine(&_xyz1, xy2);
}

/* @func: _p384_check_key (static)
 * #desc:
 *    input point legality check.
 *
 * #1: k [in]  input point
 * #r:   [ret] 0: no error, -1: error
 */
static int32_t _p384_check_key(const uint32_t k[12])
{
	uint32_t t[12];
	if (_fp384_iszero(k) || !_p384_sub(t, k, _sc384_q))
		return -1;

	return 0;
}

/* @func: _p384_check_point (static)
 * #desc:
 *    curve point legality check.
 *
 * #1: xy1 [in]  curve point
 * #r:     [ret] 0: no error, -1: error
 */
static int32_t _p384_check_point(const struct p384_point *xy1)
{
	uint32_t t1[12], t2[12];

	if (!_p384_sub(t1, xy1->x, _fp384_p))
		return -1;
	if (!_p384_sub(t1, xy1->y, _fp384_p))
		return -1;

	/* t1 = ((x1 ^ 3) - (x1 * 3) + b) % p */
	_fp384_mul(t1, xy1->x, xy1->x);
	_fp384_mul(t1, t1, xy1->x);
	_fp384_sub(t1, t1, xy1->x);
	_fp384_sub(t1, t1, xy1->x);
	_fp384_sub(t1, t1, xy1->x);
	_fp384_add(t1, t1, _p384_b);
	_fp384_mod(t1);

	/* t2 = (y1 ^ 2) % p */
	_fp384_mul(t2, xy1->y, xy1->y);
	_fp384_mod(t2);

	_p384_sub(t1, t1, t2);

	return _fp384_iszero(t1) - 1;
}

/* @func: _p384_affine_add (static)
 * #desc:
 *    affine curve point addition.
 *
 * #1: xy1 [in]  addend
 * #2: xy2 [in]  addend
 * #3: xy3 [out] result
 */
static void _p384_affine_add(const struct p384_point *xy1,
		const struct p384_point *xy2, struct p384_point *xy3)
{
	struct p384_point _xyz1, _xyz2, _xyz3, _xy4;
	uint32_t t[12], iswap1, iswap2;
	for (int32_t i = 0; i < 12; i++) {
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

	_p384_sub(t, xy1->x, xy2->x);
	iswap1 = _fp384_iszero(t);
	_p384_sub(t, xy1->y, xy2->y);
	iswap2 = _fp384_iszero(t);

	/* P != +- Q */
	_p384_point_add(&_xyz1, &_xyz2, &_xyz1);
	/* P == Q */
	_p384_point_double(&_xyz1, &_xyz3);

	/* xyz1 = if x1 == x2 && y1 == y2 : swap xyz3 */
	_fp384_swap(_xyz1.x, _xyz3.x, iswap1 & iswap2);
	_fp384_swap(_xyz1.y, _xyz3.y, iswap1 & iswap2);
	_fp384_swap(_xyz1.z, _xyz3.z, iswap1 & iswap2);

	_p384_point_to_affine(&_xyz1, xy3);

	/* xy3 = if x1 == x2 && y1 != y2 : adjust zero */
	_fp384_swap(xy3->x, _xy4.x, iswap1 & ~iswap2);
	_fp384_swap(xy3->y, _xy4.y, iswap1 & ~iswap2);
}

/* @func: ecdh_p384_public_key
 * #desc:
 *    p-384 public key create function.
 *
 * #1: pri [in]  private key
 * #2: pub [out] public key
 * #r:     [ret] 0: success, -1: fail
 */
int32_t F_SYMBOL(ecdh_p384_public_key)(const uint8_t *pri, uint8_t *pub)
{
	uint32_t _pri[12];
	struct p384_point xy1;

	C_SYMBOL(memcpy)(_pri, pri, ECDH_P384_PRI_LEN);
	if (_p384_check_key(_pri))
		return -1;

	/* xy1 = scalar(_pri, base) */
	_p384_scalar_mul(_pri, &_p384_base, &xy1);

	C_SYMBOL(memcpy)(pub, xy1.x, ECDH_P384_LEN);
	C_SYMBOL(memcpy)(pub + ECDH_P384_LEN, xy1.y, ECDH_P384_LEN);

	return 0;
}

/* @func: ecdh_p384_shared_key
 * #desc:
 *    p-384 shared key create function.
 *
 * #1: pri [in]  private key
 * #2: pub [in]  public key
 * #3: key [in]  shared key
 * #r:     [ret] 0: success, -1: fail
 */
int32_t F_SYMBOL(ecdh_p384_shared_key)(const uint8_t *pri,
		const uint8_t *pub, uint8_t *key)
{
	uint32_t _pri[12];
	struct p384_point xy1, xy2;

	C_SYMBOL(memcpy)(_pri, pri, ECDH_P384_PRI_LEN);
	if (_p384_check_key(_pri))
		return -1;

	C_SYMBOL(memcpy)(xy1.x, pub, ECDH_P384_LEN);
	C_SYMBOL(memcpy)(xy1.y, pub + ECDH_P384_LEN, ECDH_P384_LEN);
	if (_p384_check_point(&xy1))
		return -1;

	/* xy2 = scalar(_pri, xy1) */
	_p384_scalar_mul(_pri, &xy1, &xy2);

	C_SYMBOL(memcpy)(key, xy2.x, ECDH_P384_KEY_LEN);

	return 0 - _fp384_iszero(xy2.x);
}

/* @func: ecdsa_p384_public_key
 * #desc:
 *    p-384 public key create function.
 *
 * #1: pri [in]  private key
 * #2: pub [out] public key
 */
void F_SYMBOL(ecdsa_p384_public_key)(const uint8_t *pri, uint8_t *pub)
{
	uint32_t _pri[12];
	struct p384_point xy1;

	C_SYMBOL(memcpy)(_pri, pri, ECDSA_P384_PRI_LEN);

	/* xy1 = scalar(_pri, base) */
	_p384_scalar_mul(_pri, &_p384_base, &xy1);

	C_SYMBOL(memcpy)(pub, xy1.x, ECDSA_P384_LEN);
	C_SYMBOL(memcpy)(pub + ECDSA_P384_LEN, xy1.y, ECDSA_P384_LEN);
}

/* @func: ecdsa_p384_sign
 * #desc:
 *    p-384 signature function.
 *
 * #1: pri  [in]  private key
 * #2: ran  [in]  nonce
 * #3: msg  [in]  input message
 * #4: len  [in]  message length
 * #5: sign [out] signature
 * #r:      [ret] 0: success, -1: fail
 */
int32_t F_SYMBOL(ecdsa_p384_sign)(const uint8_t *pri, const uint8_t *ran,
		const uint8_t *msg, uint32_t len, uint8_t *sign)
{
	uint32_t _pri[12], _ran[12], z[12], s[12];
	struct p384_point xy1;
	SHA384_NEW(ctx);

	C_SYMBOL(memcpy)(_pri, pri, ECDSA_P384_PRI_LEN);
	C_SYMBOL(memcpy)(_ran, ran, ECDSA_P384_RAN_LEN);

	/* x1 = scalar(_ran, base) */
	_p384_scalar_mul(_ran, &_p384_base, &xy1);
	_sc384_mod(xy1.x);

	if (_fp384_iszero(xy1.x))
		return -1;

	/* z = sha(msg) % q */
	F_SYMBOL(sha384_init)(&ctx);
	F_SYMBOL(sha512)(&ctx, msg, len);
	_sc384_digest(&(SHA384_STATE(&ctx, 0)), z);

	/* s = (x1 * _pri) + z */
	_sc384_mul(s, xy1.x, _pri);
	_sc384_add(s, s, z);

	/* s = (s * inv(_ran)) % q */
	_sc384_inv(z, _ran);
	_sc384_mul(s, s, z);
	_sc384_mod(s);

	if (_fp384_iszero(s))
		return -1;

	C_SYMBOL(memcpy)(sign, xy1.x, ECDSA_P384_LEN);
	C_SYMBOL(memcpy)(sign + ECDSA_P384_LEN, s, ECDSA_P384_LEN);

	return 0;
}

/* @func: ecdsa_p384_verify
 * #desc:
 *    p-384 signature verification function.
 *
 * #1: pub  [in]  public key
 * #2: sign [in]  signature
 * #3: msg  [in]  input message
 * #4: len  [in]  message length
 * #r:      [ret] 0: success, -1: fail
 */
int32_t F_SYMBOL(ecdsa_p384_verify)(const uint8_t *pub,
		const uint8_t *sign, const uint8_t *msg, uint32_t len)
{
	uint32_t z[12], t[12], u1[12], u2[12];
	struct p384_point xy1, xy2, xy3;
	SHA384_NEW(ctx);

	/* xy1 = sign */
	C_SYMBOL(memcpy)(xy1.x, sign, ECDSA_P384_LEN);
	C_SYMBOL(memcpy)(xy1.y, sign + ECDSA_P384_LEN, ECDSA_P384_LEN);
	if (_p384_check_key(xy1.x) || _p384_check_key(xy1.y))
		return -1;

	/* xy2 = pub */
	C_SYMBOL(memcpy)(xy2.x, pub, ECDSA_P384_LEN);
	C_SYMBOL(memcpy)(xy2.y, pub + ECDSA_P384_LEN, ECDSA_P384_LEN);
	if (_p384_check_point(&xy2))
		return -1;

	/* z = sha(msg) % q */
	F_SYMBOL(sha384_init)(&ctx);
	F_SYMBOL(sha512)(&ctx, msg, len);
	_sc384_digest(&(SHA384_STATE(&ctx, 0)), z);

	/* t = inv(y1) */
	_sc384_inv(t, xy1.y);

	/* u1 = (z * t) % q */
	_sc384_mul(u1, z, t);
	_sc384_mod(u1);

	/* u2 = (x1 * t) % q */
	_sc384_mul(u2, xy1.x, t);
	_sc384_mod(u2);

	/* xy2 = scalar(u2, xy2) */
	_p384_scalar_mul(u2, &xy2, &xy2);
	/* xy3 = scalar(u1, base) */
	_p384_scalar_mul(u1, &_p384_base, &xy3);

	/* x2 = add(xy3, xy2) % q */
	_p384_affine_add(&xy3, &xy2, &xy2);
	_sc384_mod(xy2.x);

	_p384_sub(t, xy1.x, xy2.x);

	return _fp384_iszero(t) - 1;
}
