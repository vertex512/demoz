/* @file: nist-p521.c
 * #desc:
 *    The implementations of nist p-521 ecdh and ecdsa.
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
 * P = 2^521 - 1 */
static const uint32_t _fp521_p[17] = {
	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
	0x000001ff
	};

/* A = -3 % P */
static const uint32_t _p521_a[17] = {
	0xfffffffc, 0xffffffff, 0xffffffff, 0xffffffff,
	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
	0x000001ff
	};

/*
 * B = 1093849038073734274511112390766805569936207598951683748994586394495
 *     9531161507350160137087375737596232485921322967063133094384525315910
 *     12912142327488478985984
 */
static const uint32_t _p521_b[17] = {
	0x6b503f00, 0xef451fd4, 0x3d2c34f1, 0x3573df88,
	0x3bb1bf07, 0x1652c0bd, 0xec7e937b, 0x56193951,
	0x8ef109e1, 0xb8b48991, 0x99b315f3, 0xa2da725b,
	0xb68540ee, 0x929a21a0, 0x8e1c9a1f, 0x953eb961,
	0x00000051
	};

/*
 * Q = 6864797660130609714981900799081393217269435300143305409394463459185
 *     5431833976553942450577463332171975329639963713633211138647686124403
 *     80340372808892707005449
 */
static const uint32_t _sc521_q[17] = {
	0x91386409, 0xbb6fb71e, 0x899c47ae, 0x3bb5c9b8,
	0xf709a5d0, 0x7fcc0148, 0xbf2f966b, 0x51868783,
	0xfffffffa, 0xffffffff, 0xffffffff, 0xffffffff,
	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
	0x000001ff
	};

/*
 * Bx = 266174080205021706322876871672336096072985916875697314770667136841
 *      880294499642780849154508062777190235209424122506555866215711354557
 *      0916814161637315895999846
 * By = 375718002577002046354550722449118360359445513476976248669456777961
 *      554447744055631669123440501294553956214444453728942852258566672919
 *      6580810124344277578376784
 */
struct p521_point {
	uint32_t x[17];
	uint32_t y[17];
	uint32_t z[17]; /* jacobian coordinates */
};

static const struct p521_point _p521_base = {
	{
		0xc2e5bd66, 0xf97e7e31, 0x856a429b, 0x3348b3c1,
		0xa2ffa8de, 0xfe1dc127, 0xefe75928, 0xa14b5e77,
		0x6b4d3dba, 0xf828af60, 0x053fb521, 0x9c648139,
		0x2395b442, 0x9e3ecb66, 0x0404e9cd, 0x858e06b7,
		0x000000c6
	},
	{
		0x9fd16650, 0x88be9476, 0xa272c240, 0x353c7086,
		0x3fad0761, 0xc550b901, 0x5ef42640, 0x97ee7299,
		0x273e662c, 0x17afbd17, 0x579b4468, 0x98f54449,
		0x2c7d1bd9, 0x5c8a5fb4, 0x9a3bc004, 0x39296a78,
		0x00000118
	},
	{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	}
	};

/*
 * R = 2^544 % Q
 *   551867647541077700652255638561243627813117473089660986076197301492665
 *   5240969780199424
 */
static const uint32_t _sc521_R[17] = {
	0xfb800000, 0x70b763cd, 0x28a24824, 0x23bb31dc,
	0x17e2251b, 0x5b847b2d, 0xca4019ff, 0x3e206834,
	0x02d73cbc, 0, 0, 0, 0, 0, 0, 0, 0
	};
/* end */

/* @func: _fp521_swap (static)
 * #desc:
 *    constant-time the numerical swap.
 *
 * #1: number
 * #2: number
 * #3: low bit (0: keep, 1: swap)
 */
static void _fp521_swap(uint32_t a[17], uint32_t b[17], uint32_t bit)
{
	bit = (~bit & 1) - 1;
	for (int32_t i = 0; i < 17; i++) {
		uint32_t t = bit & (a[i] ^ b[i]);
		a[i] ^= t;
		b[i] ^= t;
	}
}

/* @func: _fp521_move (static)
 * #desc:
 *    constant-time the move numerical.
 *
 * #1: number
 * #2: number
 * #3: low bit (0: keep, 1: swap)
 */
static void _fp521_move(uint32_t a[17], const uint32_t b[17], uint32_t bit)
{
	bit = (~bit & 1) - 1;
	for (int32_t i = 0; i < 17; i++) {
		uint32_t t = bit & (a[i] ^ b[i]);
		a[i] ^= t;
	}
}

/* @func: _fp521_iszero (static)
 * #desc:
 *    check if a number is zero in constant-time.
 *
 * #1: number
 * #r: 0: non-zero, 1: is zero
 */
static uint32_t _fp521_iszero(const uint32_t a[17])
{
	uint32_t r = 0;
	for (int32_t i = 0; i < (17 * 2); i++)
		r |= ((uint16_t *)a)[i];

	/*
	 * 0x0000ffff - 1 == 0x0000fffe
	 *   0x0000fffe >> 31 == 0x00000000
	 * 0x00000000 - 1 == 0xffffffff
	 *   0xffffffff >> 31 == 0x00000001
	 */
	return ((r - 1) >> 31) & 1;
}

/* @func: _fp521_add (static)
 * #desc:
 *    prime field addition.
 *
 * #1: sum
 * #2: addend
 * #3: addend
 */
static void _fp521_add(uint32_t r[17],
		const uint32_t a[17], const uint32_t b[17])
{
	uint32_t carry = 0;
	uint64_t tmp = 0;

	/* r = a + b */
	for (int32_t i = 0; i < 17; i++) {
		tmp = (uint64_t)a[i] + b[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	/* NOTE: 2^544 == 8388608 (mod 2^521-1) */

	/* r = r % p modular reduction */
	carry *= 8388608;
	for (int32_t i = 0; i < 17; i++) {
		tmp = (uint64_t)r[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	carry *= 8388608;
	for (int32_t i = 0; i < 17; i++) {
		tmp = (uint64_t)r[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
}

/* @func: _fp521_sub (static)
 * #desc:
 *    prime field subtraction.
 *
 * #1: difference
 * #2: minuend
 * #3: subtract
 */
static void _fp521_sub(uint32_t r[17],
		const uint32_t a[17], const uint32_t b[17])
{
	uint32_t carry = 0;
	uint64_t tmp = 0;

	/* r = a - b */
	for (int32_t i = 0; i < 17; i++) {
		tmp = (uint64_t)a[i] - b[i] + (int32_t)carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	/* NOTE: 2^544 == 8388608 (mod 2^521-1) */

	/* r = r % p modular reduction */
	tmp = (uint64_t)r[0] - (carry & 8388608);
	r[0] = tmp & 0xffffffff;
	carry = tmp >> 32;
	for (int32_t i = 1; i < 17; i++) {
		tmp = (uint64_t)r[i] + (int32_t)carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	tmp = (uint64_t)r[0] - (carry & 8388608);
	r[0] = tmp & 0xffffffff;
	carry = tmp >> 32;
	for (int32_t i = 1; i < 17; i++) {
		tmp = (uint64_t)r[i] + (int32_t)carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
}

/* @func: _fp521_mul (static)
 * #desc:
 *    prime field multiplication.
 *
 * #1: product
 * #2: multiplicand
 * #3: multiplier
 */
static void _fp521_mul(uint32_t r[17],
		const uint32_t a[17], const uint32_t b[17])
{
	uint32_t rr[34];
	uint32_t carry = 0;
	uint64_t tmp = 0;

	for (int32_t i = 0; i < 34; i++)
		rr[i] = 0;

	/* rr = a * b */
	for (int32_t i = 0; i < 17; i++) {
		tmp = (uint64_t)a[0] * b[i] + carry;
		rr[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
	rr[17] = carry;

	for (int32_t i = 1; i < 17; i++) {
		carry = 0;
		for (int32_t j = 0; j < 17; j++) {
			tmp = (uint64_t)a[i] * b[j] + rr[i + j] + carry;
			rr[i + j] = tmp & 0xffffffff;
			carry = tmp >> 32;
		}
		rr[i + 17] = carry;
	}

	/* NOTE: 2^544 == 8388608 (mod 2^521-1) */

	/* r = rr % p modular reduction */
	carry = 0;
	for (int32_t i = 0; i < 17; i++) { /* fold */
		tmp = (uint64_t)rr[i + 17] * 8388608 + rr[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	tmp = (uint64_t)carry * 8388608 + r[0];
	r[0] = tmp & 0xffffffff;
	carry = tmp >> 32;
	for (int32_t i = 1; i < 17; i++) {
		tmp = (uint64_t)r[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	tmp = (uint64_t)carry * 8388608 + r[0];
	r[0] = tmp & 0xffffffff;
	carry = tmp >> 32;
	for (int32_t i = 1; i < 17; i++) {
		tmp = (uint64_t)r[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
}

/* @func: _fp521_mod (static)
 * #desc:
 *    prime field modular reduction (norm).
 *
 * #1: number
 */
static void _fp521_mod(uint32_t r[17])
{
	uint32_t carry = 0, carry2 = 0;
	uint64_t tmp = 0;

	/* NOTE: 2^544 == 8388608 (mod 2^521-1) */

	carry = r[16] >> 9; /* fold */
	r[16] &= 0x1ff;
	for (int32_t i = 0; i < 17; i++) {
		tmp = (uint64_t)r[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	/* r = r - p */
	for (int32_t i = 0; i < 17; i++) {
		tmp = (uint64_t)r[i] - _fp521_p[i] + (int32_t)carry2;
		r[i] = tmp & 0xffffffff;
		carry2 = tmp >> 32;
	}

	/* r = if r < p : adjust r */
	carry2 = ~(((uint64_t)carry2 - 1) >> 32);
	carry = 0;
	for (int32_t i = 0; i < 17; i++) {
		tmp = (uint64_t)r[i] + (_fp521_p[i] & carry2) + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
}

/* @func: _fp521_inv (static)
 * #desc:
 *    modular inversion based on fermat's little theorem.
 *
 * #1: inverse modulus
 * #2: number
 */
static void _fp521_inv(uint32_t r[17], const uint32_t z[17])
{
	uint32_t x[17], e[17];
	for (int32_t i = 0; i < 17; i++) {
		x[i] = z[i];
		e[i] = _fp521_p[i];
		r[i] = 0;
	}
	e[0] -= 2;
	r[0] = 1;

	for (int32_t i = 0; i < 521; i++) {
		uint32_t k_i = e[i / 32] >> (i % 32);
		if (k_i & 1)
			_fp521_mul(r, r, x);
		_fp521_mul(x, x, x);
	}
}

/* @func: _sc521_modw (static)
 * #desc:
 *    scalar high-word modular reduction fold.
 *
 * #1: result
 * #2: number
 * #3: high-word
 */
static void _sc521_modw(uint32_t r[17],
		const uint32_t a[17], uint32_t b)
{
	uint32_t rr[17];
	uint32_t carry = 0, carry2 = 0;
	uint64_t tmp = 0;

	/* rr = b * R */
	for (int32_t i = 0; i < 17; i++) {
		tmp = (uint64_t)b * _sc521_R[i] + carry;
		rr[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	/* r = a + rr */
	for (int32_t i = 0; i < 17; i++) {
		tmp = (uint64_t)a[i] + rr[i] + carry2;
		r[i] = tmp & 0xffffffff;
		carry2 = tmp >> 32;
	}

	/* rr = carry2 * R */
	carry = 0;
	for (int32_t i = 0; i < 17; i++) {
		tmp = (uint64_t)carry2 * _sc521_R[i] + carry;
		rr[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	/* r = r + rr */
	carry = 0;
	for (int32_t i = 0; i < 17; i++) {
		tmp = (uint64_t)r[i] + rr[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
}

/* @func: _sc521_mod (static)
 * #desc:
 *    scalar modular reduction (norm).
 *
 * #1: number
 */
static void _sc521_mod(uint32_t r[17])
{
	uint32_t rr[17];
	uint32_t carry = 0, carry2 = 0;
	uint64_t tmp = 0;

	/* rr = ((r >> 521) + 1) * q */
	carry2 = (r[16] >> 9) + 1;
	for (int32_t i = 0; i < 17; i++) {
		tmp = (uint64_t)carry2 * _sc521_q[i] + carry;
		rr[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	/* r = r - rr */
	carry2 = 0;
	for (int32_t i = 0; i < 17; i++) {
		tmp = (uint64_t)r[i] - rr[i] + (int32_t)carry2;
		r[i] = tmp & 0xffffffff;
		carry2 = tmp >> 32;
	}

	/* r = if r < q : adjust r */
	carry2 = ~(((uint64_t)carry2 - 1) >> 32);
	carry = 0;
	for (int32_t i = 0; i < 17; i++) {
		tmp = (uint64_t)r[i] + (_sc521_q[i] & carry2) + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
}

/* @func: _sc521_add (static)
 * #desc:
 *    scalar addition.
 *
 * #1: sum
 * #2: addend
 * #3: addend
 */
static void _sc521_add(uint32_t r[17],
		const uint32_t a[17], const uint32_t b[17])
{
	uint32_t carry = 0;
	uint64_t tmp = 0;

	/* r = a + b */
	for (int32_t i = 0; i < 17; i++) {
		tmp = (uint64_t)a[i] + b[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	/* r = r % q */
	_sc521_modw(r, r, carry);
}

/* @func: _sc521_mul (static)
 * #desc:
 *    scalar multiplication.
 *
 * #1: product
 * #2: multiplicand
 * #3: multiplier
 */
static void _sc521_mul(uint32_t r[17],
		const uint32_t a[17], const uint32_t b[17])
{
	uint32_t rr[34];
	uint32_t carry = 0;
	uint64_t tmp = 0;

	for (int32_t i = 0; i < 34; i++)
		rr[i] = 0;

	/* rr = a * b */
	for (int32_t i = 0; i < 17; i++) {
		tmp = (uint64_t)a[0] * b[i] + carry;
		rr[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
	rr[17] = carry;

	for (int32_t i = 1; i < 17; i++) {
		carry = 0;
		for (int32_t j = 0; j < 17; j++) {
			tmp = (uint64_t)a[i] * b[j] + rr[i + j] + carry;
			rr[i + j] = tmp & 0xffffffff;
			carry = tmp >> 32;
		}
		rr[i + 17] = carry;
	}

	/* r = rr % q */
	_sc521_modw(rr + 16, rr + 16, rr[33]);
	_sc521_modw(rr + 15, rr + 15, rr[32]);
	_sc521_modw(rr + 14, rr + 14, rr[31]);
	_sc521_modw(rr + 13, rr + 13, rr[30]);
	_sc521_modw(rr + 12, rr + 12, rr[29]);
	_sc521_modw(rr + 11, rr + 11, rr[28]);
	_sc521_modw(rr + 10, rr + 10, rr[27]);
	_sc521_modw(rr + 9, rr + 9, rr[26]);
	_sc521_modw(rr + 8, rr + 8, rr[25]);
	_sc521_modw(rr + 7, rr + 7, rr[24]);
	_sc521_modw(rr + 6, rr + 6, rr[23]);
	_sc521_modw(rr + 5, rr + 5, rr[22]);
	_sc521_modw(rr + 4, rr + 4, rr[21]);
	_sc521_modw(rr + 3, rr + 3, rr[20]);
	_sc521_modw(rr + 2, rr + 2, rr[19]);
	_sc521_modw(rr + 1, rr + 1, rr[18]);
	_sc521_modw(r, rr, rr[17]);
}

/* @func: _sc521_inv (static)
 * #desc:
 *    modular inversion based on fermat's little theorem.
 *
 * #1: inverse modulus
 * #2: number
 */
static void _sc521_inv(uint32_t r[17], const uint32_t z[17])
{
	uint32_t x[17], e[17];
	for (int32_t i = 0; i < 17; i++) {
		x[i] = z[i];
		e[i] = _sc521_q[i];
		r[i] = 0;
	}
	e[0] -= 2;
	r[0] = 1;

	for (int32_t i = 0; i < 521; i++) {
		uint32_t k_i = e[i / 32] >> (i % 32);
		if (k_i & 1)
			_sc521_mul(r, r, x);
		_sc521_mul(x, x, x);
	}
}

/* @func: _sc521_digest (static)
 * #desc:
 *    sha512 digest modular reduction.
 *
 * #1: sha521 digest
 * #2: result
 */
static void _sc521_digest(const uint8_t dig[64], uint32_t r[17])
{
	r[16] = 0;

	/* big endian */
	for (int32_t i = 0; i < 64; i++)
		((uint8_t *)r)[63 - i] = dig[i];

	/* C_SYMBOL(memcpy)(r, dig, 64); */

	/* r = r % q */
	_sc521_mod(r);
}

/* @func: _p521_sub (static)
 * #desc:
 *    nonprime field subtraction.
 *
 * #1: difference
 * #2: minuend
 * #3: subtract
 * #r: overflow
 */
static uint32_t _p521_sub(uint32_t r[17],
		const uint32_t a[17], const uint32_t b[17])
{
	uint32_t carry = 0;
	uint64_t tmp = 0;

	/* r = a - b */
	for (int32_t i = 0; i < 17; i++) {
		tmp = (uint64_t)a[i] - b[i] + (int32_t)carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	return carry;
}

/* @func: _p521_point_add (static)
 * #desc:
 *    jacobian-affine curve point addition.
 *
 * #1: addend
 * #2: addend
 * #3: result
 */
static void _p521_point_add(const struct p521_point *xyz1,
		const struct p521_point *xyz2, struct p521_point *xyz3)
{
	uint32_t H1[17], R1[17], H2[17], H3[17], C1[17], T1[17], T2[17];
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
	_fp521_mul(T1, xyz1->z, xyz1->z);
	_fp521_mul(H1, T1, xyz2->x);
	_fp521_sub(H1, H1, xyz1->x);

	/* R1 = (y2 * (z1 ^ 3)) - y1 */
	_fp521_mul(R1, T1, xyz1->z);
	_fp521_mul(R1, R1, xyz2->y);
	_fp521_sub(R1, R1, xyz1->y);

	/* H2 = H1 ^ 2 */
	_fp521_mul(H2, H1, H1);
	/* H3 = H1 ^ 3 */
	_fp521_mul(H3, H2, H1);
	/* C1 = x1 * H2 */
	_fp521_mul(C1, xyz1->x, H2);

	/* x3 = (R1 ^ 2) - (C1 * 2) - H3 */
	_fp521_mul(T1, R1, R1);
	_fp521_sub(T1, T1, C1);
	_fp521_sub(T1, T1, C1);
	_fp521_sub(xyz3->x, T1, H3);

	/* y3 = (R1 * (C1 - x3)) - (y1 * H3) */
	_fp521_sub(T1, C1, xyz3->x);
	_fp521_mul(T1, T1, R1);
	_fp521_mul(T2, xyz1->y, H3);
	_fp521_sub(xyz3->y, T1, T2);

	/* z3 = z1 * H1 */
	_fp521_mul(xyz3->z, xyz1->z, H1);
}

/* @func: _p521_point_double (static)
 * #desc:
 *    jacobian curve point doubling.
 *
 * #1: addend
 * #2: result
 */
static void _p521_point_double(const struct p521_point *xyz1,
		struct p521_point *xyz3)
{
	uint32_t M1[17], S1[17], U1[17], T1[17];
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
	_fp521_mul(T1, xyz1->x, xyz1->x);
	_fp521_add(M1, T1, T1);
	_fp521_add(M1, M1, T1);
	_fp521_mul(T1, xyz1->z, xyz1->z);
	_fp521_mul(T1, T1, T1);
	_fp521_mul(T1, T1, _p521_a);
	_fp521_add(M1, M1, T1);

	/* S1 = (x1 * 4) * (y1 ^ 2) */
	_fp521_add(S1, xyz1->x, xyz1->x);
	_fp521_add(S1, S1, S1);
	_fp521_mul(T1, xyz1->y, xyz1->y);
	_fp521_mul(S1, S1, T1);

	/* U1 = (y1 ^ 4) * 8 */
	_fp521_mul(T1, T1, T1);
	_fp521_add(T1, T1, T1);
	_fp521_add(T1, T1, T1);
	_fp521_add(U1, T1, T1);

	/* z3 = (y1 * z1) * 2 */
	_fp521_mul(T1, xyz1->y, xyz1->z);
	_fp521_add(xyz3->z, T1, T1);

	/* x3 = (M1 ^ 2) - (S1 * 2) */
	_fp521_mul(T1, M1, M1);
	_fp521_sub(T1, T1, S1);
	_fp521_sub(xyz3->x, T1, S1);

	/* y3 = (M1 * (S1 - x3)) - U1 */
	_fp521_sub(T1, S1, xyz3->x);
	_fp521_mul(T1, T1, M1);
	_fp521_sub(xyz3->y, T1, U1);
}

/* @func: _p521_point_to_affine (static)
 * #desc:
 *    jacobian to affine coordinates conversion.
 *
 * #1: jacobian point
 * #2: affine point
 */
static void _p521_point_to_affine(const struct p521_point *xyz1,
		struct p521_point *xy2)
{
	uint32_t z[17], t[17];

	/* z = inv(z1) */
	_fp521_inv(z, xyz1->z);

	/* x2 = (x1 * (z ^ 2)) % p */
	_fp521_mul(t, z, z);
	_fp521_mul(xy2->x, xyz1->x, t);
	_fp521_mod(xy2->x);

	/* y2 = (y1 * (z ^ 3)) % p */
	_fp521_mul(t, t, z);
	_fp521_mul(xy2->y, xyz1->y, t);
	_fp521_mod(xy2->y);
}

/* @func: _p521_scalar_mul (static)
 * #desc:
 *    p-521 signed binary ladder scalar multiplication.
 *
 * #1: input point
 * #2: base point
 * #3: scalar
 */
static void _p521_scalar_mul(const uint32_t k[17],
		const struct p521_point *xy1, struct p521_point *xy2)
{
	struct p521_point _xyz1, _xyz2;
	uint32_t t[17], y_neg[17], neg = 0;
	for (int32_t i = 0; i < 17; i++) {
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
	_p521_sub(t, _sc521_q, k);
	neg = ~k[0] & 1;
	_fp521_move(t, k, ~neg);

	/* y_neg = -y1 */
	_fp521_sub(y_neg, y_neg, xy1->y);
	_fp521_move(_xyz1.y, y_neg, neg);

	/* signed binary ladder and jacobian-affine coordinates */
	for (int32_t i = 520; i > 0; i--) {
		uint32_t k_i = t[i / 32] >> (i % 32);
		_fp521_move(_xyz2.y, xy1->y, k_i ^ neg);
		_fp521_move(_xyz2.y, y_neg, ~k_i ^ neg);
		/* R = (R * 2) +- P */

		_p521_point_double(&_xyz1, &_xyz1);
		_p521_point_add(&_xyz1, &_xyz2, &_xyz1);
	}

	_p521_point_to_affine(&_xyz1, xy2);
}

/* @func: _p521_check_key (static)
 * #desc:
 *    input point legality check.
 *
 * #1: input point
 * #r: 0: no error, -1: error
 */
static int32_t _p521_check_key(const uint32_t k[17])
{
	uint32_t t[17];
	if (_fp521_iszero(k) || !_p521_sub(t, k, _sc521_q))
		return -1;

	return 0;
}

/* @func: _p521_check_point (static)
 * #desc:
 *    curve point legality check.
 *
 * #1: curve point
 * #r: 0: no error, -1: error
 */
static int32_t _p521_check_point(const struct p521_point *xy1)
{
	uint32_t t1[17], t2[17];

	if (!_p521_sub(t1, xy1->x, _fp521_p))
		return -1;
	if (!_p521_sub(t1, xy1->y, _fp521_p))
		return -1;

	/* t1 = ((x1 ^ 3) - (x1 * 3) + b) % p */
	_fp521_mul(t1, xy1->x, xy1->x);
	_fp521_mul(t1, t1, xy1->x);
	_fp521_sub(t1, t1, xy1->x);
	_fp521_sub(t1, t1, xy1->x);
	_fp521_sub(t1, t1, xy1->x);
	_fp521_add(t1, t1, _p521_b);
	_fp521_mod(t1);

	/* t2 = (y1 ^ 2) % p */
	_fp521_mul(t2, xy1->y, xy1->y);
	_fp521_mod(t2);

	_p521_sub(t1, t1, t2);

	return _fp521_iszero(t1) - 1;
}

/* @func: _p521_affine_add (static)
 * #desc:
 *    affine curve point addition.
 *
 * #1: addend
 * #2: addend
 * #3: result
 */
static void _p521_affine_add(const struct p521_point *xy1,
		const struct p521_point *xy2, struct p521_point *xy3)
{
	struct p521_point _xyz1, _xyz2, _xyz3, _xy4;
	uint32_t t[17], iswap1, iswap2;
	for (int32_t i = 0; i < 17; i++) {
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

	_p521_sub(t, xy1->x, xy2->x);
	iswap1 = _fp521_iszero(t);
	_p521_sub(t, xy1->y, xy2->y);
	iswap2 = _fp521_iszero(t);

	/* P != +- Q */
	_p521_point_add(&_xyz1, &_xyz2, &_xyz1);
	/* P == Q */
	_p521_point_double(&_xyz1, &_xyz3);

	/* xyz1 = if x1 == x2 && y1 == y2 : swap xyz3 */
	_fp521_swap(_xyz1.x, _xyz3.x, iswap1 & iswap2);
	_fp521_swap(_xyz1.y, _xyz3.y, iswap1 & iswap2);
	_fp521_swap(_xyz1.z, _xyz3.z, iswap1 & iswap2);

	_p521_point_to_affine(&_xyz1, xy3);

	/* xy3 = if x1 == x2 && y1 != y2 : adjust zero */
	_fp521_swap(xy3->x, _xy4.x, iswap1 & ~iswap2);
	_fp521_swap(xy3->y, _xy4.y, iswap1 & ~iswap2);
}

/* @func: ecdh_p521_public_key
 * #desc:
 *    p-521 public key create function.
 *
 * #1: private key
 * #2: public key
 * #r: 0: success, -1: fail
 */
int32_t F_SYMBOL(ecdh_p521_public_key)(const uint8_t *pri, uint8_t *pub)
{
	uint32_t _pri[17];
	struct p521_point xy1;
	_pri[16] = 0;

	C_SYMBOL(memcpy)(_pri, pri, ECDH_P521_PRI_LEN);
	if (_p521_check_key(_pri))
		return -1;

	/* xy1 = scalar(_pri, base) */
	_p521_scalar_mul(_pri, &_p521_base, &xy1);

	C_SYMBOL(memcpy)(pub, xy1.x, ECDH_P521_LEN);
	C_SYMBOL(memcpy)(pub + ECDH_P521_LEN, xy1.y, ECDH_P521_LEN);

	return 0;
}

/* @func: ecdh_p521_shared_key
 * #desc:
 *    p-521 shared key create function.
 *
 * #1: private key
 * #2: public key
 * #3: shared key
 * #r: 0: success, -1: fail
 */
int32_t F_SYMBOL(ecdh_p521_shared_key)(const uint8_t *pri,
		const uint8_t *pub, uint8_t *key)
{
	uint32_t _pri[17];
	struct p521_point xy1, xy2;
	_pri[16] = 0;
	xy1.x[16] = xy1.y[16] = 0;

	C_SYMBOL(memcpy)(_pri, pri, ECDH_P521_PRI_LEN);
	if (_p521_check_key(_pri))
		return -1;

	C_SYMBOL(memcpy)(xy1.x, pub, ECDH_P521_LEN);
	C_SYMBOL(memcpy)(xy1.y, pub + ECDH_P521_LEN, ECDH_P521_LEN);
	if (_p521_check_point(&xy1))
		return -1;

	/* xy2 = scalar(_pri, xy1) */
	_p521_scalar_mul(_pri, &xy1, &xy2);

	C_SYMBOL(memcpy)(key, xy2.x, ECDH_P521_KEY_LEN);

	return 0 - _fp521_iszero(xy2.x);
}

/* @func: ecdsa_p521_public_key
 * #desc:
 *    p-521 public key create function.
 *
 * #1: private key
 * #2: public key
 */
void F_SYMBOL(ecdsa_p521_public_key)(const uint8_t *pri, uint8_t *pub)
{
	uint32_t _pri[17];
	struct p521_point xy1;
	_pri[16] = 0;

	C_SYMBOL(memcpy)(_pri, pri, ECDSA_P521_PRI_LEN);

	/* xy1 = scalar(_pri, base) */
	_p521_scalar_mul(_pri, &_p521_base, &xy1);

	C_SYMBOL(memcpy)(pub, xy1.x, ECDSA_P521_LEN);
	C_SYMBOL(memcpy)(pub + ECDSA_P521_LEN, xy1.y, ECDSA_P521_LEN);
}

/* @func: ecdsa_p521_sign
 * #desc:
 *    p-521 signature function.
 *
 * #1: private key
 * #2: nonce
 * #3: input message
 * #4: message length
 * #5: signature
 * #r: 0: success, -1: fail
 */
int32_t F_SYMBOL(ecdsa_p521_sign)(const uint8_t *pri, const uint8_t *ran,
		const uint8_t *msg, uint32_t len, uint8_t *sign)
{
	uint32_t _pri[17], _ran[17], z[17], s[17];
	struct p521_point xy1;
	SHA512_NEW(ctx);
	_pri[16] = 0;
	_ran[16] = 0;

	C_SYMBOL(memcpy)(_pri, pri, ECDSA_P521_PRI_LEN);
	C_SYMBOL(memcpy)(_ran, ran, ECDSA_P521_RAN_LEN);

	/* x1 = scalar(_ran, base) */
	_p521_scalar_mul(_ran, &_p521_base, &xy1);
	_sc521_mod(xy1.x);

	if (_fp521_iszero(xy1.x))
		return -1;

	/* z = sha(msg) % q */
	F_SYMBOL(sha512_init)(&ctx);
	F_SYMBOL(sha512)(&ctx, msg, len);
	_sc521_digest(&(SHA512_STATE(&ctx, 0)), z);

	/* s = (x1 * _pri) + z */
	_sc521_mul(s, xy1.x, _pri);
	_sc521_add(s, s, z);

	/* s = (s * inv(_ran)) % q */
	_sc521_inv(z, _ran);
	_sc521_mul(s, s, z);
	_sc521_mod(s);

	if (_fp521_iszero(s))
		return -1;

	C_SYMBOL(memcpy)(sign, xy1.x, ECDSA_P521_LEN);
	C_SYMBOL(memcpy)(sign + ECDSA_P521_LEN, s, ECDSA_P521_LEN);

	return 0;
}

/* @func: ecdsa_p521_verify
 * #desc:
 *    p-521 signature verification function.
 *
 * #1: public key
 * #2: signature
 * #3: input message
 * #4: message length
 * #r: 0: success, -1: fail
 */
int32_t F_SYMBOL(ecdsa_p521_verify)(const uint8_t *pub,
		const uint8_t *sign, const uint8_t *msg, uint32_t len)
{
	uint32_t z[17], t[17], u1[17], u2[17];
	struct p521_point xy1, xy2, xy3;
	SHA512_NEW(ctx);
	xy1.x[16] = xy1.y[16] = 0;
	xy2.x[16] = xy2.y[16] = 0;

	/* xy1 = sign */
	C_SYMBOL(memcpy)(xy1.x, sign, ECDSA_P521_LEN);
	C_SYMBOL(memcpy)(xy1.y, sign + ECDSA_P521_LEN, ECDSA_P521_LEN);
	if (_p521_check_key(xy1.x) || _p521_check_key(xy1.y))
		return -1;

	/* xy2 = pub */
	C_SYMBOL(memcpy)(xy2.x, pub, ECDSA_P521_LEN);
	C_SYMBOL(memcpy)(xy2.y, pub + ECDSA_P521_LEN, ECDSA_P521_LEN);
	if (_p521_check_point(&xy2))
		return -1;

	/* z = sha(msg) % q */
	F_SYMBOL(sha512_init)(&ctx);
	F_SYMBOL(sha512)(&ctx, msg, len);
	_sc521_digest(&(SHA512_STATE(&ctx, 0)), z);

	/* t = inv(y1) */
	_sc521_inv(t, xy1.y);

	/* u1 = (z * t) % q */
	_sc521_mul(u1, z, t);
	_sc521_mod(u1);

	/* u2 = (x1 * t) % q */
	_sc521_mul(u2, xy1.x, t);
	_sc521_mod(u2);

	/* xy2 = scalar(u2, xy2) */
	_p521_scalar_mul(u2, &xy2, &xy2);
	/* xy3 = scalar(u1, base) */
	_p521_scalar_mul(u1, &_p521_base, &xy3);

	/* x2 = add(xy3, xy2) % q */
	_p521_affine_add(&xy3, &xy2, &xy2);
	_sc521_mod(xy2.x);

	_p521_sub(t, xy1.x, xy2.x);

	return _fp521_iszero(t) - 1;
}
