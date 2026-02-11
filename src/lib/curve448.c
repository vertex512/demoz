/* @file: curve448.c
 * #desc:
 *    The implementations of curve448 ecdh and eddsa.
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
#include <demoz/lib/ecc.h>


/* @def: _
 * P = 2^448 - 2^224 - 1 */
static const uint32_t _fp448_p[14] = {
	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
	0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffe,
	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
	0xffffffff, 0xffffffff
	};

/* B = 5 */
static const uint32_t _x448_b[14] = {
	5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};

/* D = -39081 % P */
static const uint32_t _ed448_d[14] = {
	0xffff6756, 0xffffffff, 0xffffffff, 0xffffffff,
	0xffffffff, 0xffffffff, 0xffffffff, 0xfffffffe,
	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
	0xffffffff, 0xffffffff
	};

/* one = 1 */
static const uint32_t _ed448_one[14] = {
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};

/* p14 = (P + 1) / 4 */
static const uint32_t _ed448_p14[14] = {
	0, 0, 0, 0, 0, 0,
	0xc0000000, 0xffffffff, 0xffffffff, 0xffffffff,
	0xffffffff, 0xffffffff, 0xffffffff, 0x3fffffff
	};

/* Q = 2^446 - 0x8335dc163bb124b65129c96fde933d8d723a70aadc873d6d54a7bb0d */
/* static const uint32_t _sc448_q[14] = {
	0xab5844f3, 0x2378c292, 0x8dc58f55, 0x216cc272,
	0xaed63690, 0xc44edb49, 0x7cca23e9, 0xffffffff,
	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
	0xffffffff, 0x3fffffff
	}; */

/*
 * By = 298819210078481492676017930443930673437544040154080242095928241372
 *      331506189835876003536878655418784733982303233503462500531545062832
 *      660
 * Bx = rec_x(By, 0)
 *   224580040295924300187604334099896036246789641632564134246125461686950
 *   415467406032909029192869357953282578032075146446173674602635247710
 * Bz = 1
 */
struct ed448_point {
	uint32_t x[14];
	uint32_t y[14];
	uint32_t z[14];
};

static const struct ed448_point _ed448_base = {
	{
		0xc70cc05e, 0x2626a82b, 0x8b00938e, 0x433b80e1,
		0x2ab66511, 0x12ae1af7, 0xa3d3a464, 0xea6de324,
		0x470f1767, 0x9e146570, 0x22bf36da, 0x221d15a6,
		0x6bed0ded, 0x4f1970c6
	},
	{
		0xf230fa14, 0x9808795b, 0x4ed7c8ad, 0xfdbd132c,
		0xe67c39c4, 0x3ad3ff1c, 0x05a0c2d7, 0x87789c1e,
		0x6ca39840, 0x4bea7373, 0x56c9c762, 0x88762037,
		0x6eb6bc24, 0x693f4671
	},
	{
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	}
	};

/*
 * Base Point Order
 *   q = 2^446 - 0x8335dc163bb124b65129c96fde933d8d723a70aadc873d6d54a7bb0d
 *   P1..P4 = q * (1..4)
 */
static const uint32_t _sc448_BPO[5][14] = {
	{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	},
	{
		0xab5844f3, 0x2378c292, 0x8dc58f55, 0x216cc272,
		0xaed63690, 0xc44edb49, 0x7cca23e9, 0xffffffff,
		0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
		0xffffffff, 0x3fffffff
	},
	{
		0x56b089e6, 0x46f18525, 0x1b8b1eaa, 0x42d984e5,
		0x5dac6d20, 0x889db693, 0xf99447d3, 0xfffffffe,
		0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
		0xffffffff, 0x7fffffff
	},
	{
		0x0208ced9, 0x6a6a47b8, 0xa950adff, 0x64464757,
		0x0c82a3b0, 0x4cec91dd, 0x765e6bbd, 0xfffffffe,
		0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
		0xffffffff, 0xbfffffff
	},
	{
		0xad6113cc, 0x8de30a4a, 0x37163d54, 0x85b309ca,
		0xbb58da40, 0x113b6d26, 0xf3288fa7, 0xfffffffd,
		0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
		0xffffffff, 0xffffffff
	}
	};

/*
 * R = 2^448 % q
 *   55272267239580461408029546994061707521346769899528714439578190015540
 */
static const uint32_t _sc448_R[14] = {
	0x529eec34, 0x721cf5b5, 0xc8e9c2ab, 0x7a4cf635,
	0x44a725bf, 0xeec492d9, 0x0cd77058,
	2, 0, 0, 0, 0, 0, 0
	};

/* ed448 context prefix */
#define ED448_CTX_LEN 10

static const uint8_t _ed448_ctx[] = "SigEd448\0\0";
/* end */

/* @func: _fp448_swap (static)
 * #desc:
 *    constant-time the numerical swap.
 *
 * #1: number
 * #2: number
 * #3: low bit (0: keep, 1: swap)
 */
static void _fp448_swap(uint32_t a[14], uint32_t b[14], uint32_t bit)
{
	bit = (~bit & 1) - 1;
	for (int32_t i = 0; i < 14; i++) {
		uint32_t t = bit & (a[i] ^ b[i]);
		a[i] ^= t;
		b[i] ^= t;
	}
}

/* @func: _fp448_iszero (static)
 * #desc:
 *    check if a number is zero in constant-time.
 *
 * #1: number
 * #r: 0: non-zero, 1: is zero
 */
static uint32_t _fp448_iszero(const uint32_t a[14])
{
	uint32_t r = 0;
	for (int32_t i = 0; i < (14 * 2); i++)
		r |= ((uint16_t *)a)[i];

	/*
	 * 0x0000ffff - 1 == 0x0000fffe
	 *   0x0000fffe >> 31 == 0x00000000
	 * 0x00000000 - 1 == 0xffffffff
	 *   0xffffffff >> 31 == 0x00000001
	 */
	return ((r - 1) >> 31) & 1;
}

/* @func: _fp448_add (static)
 * #desc:
 *    prime field addition.
 *
 * #1: sum
 * #2: addend
 * #3: addend
 */
static void _fp448_add(uint32_t r[14],
		const uint32_t a[14], const uint32_t b[14])
{
	uint32_t carry = 0, carry2 = 0;
	uint64_t tmp = 0;

	/* r = a + b */
	for (int32_t i = 0; i < 14; i++) {
		tmp = (uint64_t)a[i] + b[i] + carry2;
		r[i] = tmp & 0xffffffff;
		carry2 = tmp >> 32;
	}

	/* NOTE: 2^448 == 2^224+1 (mod 2^448-2^224-1) */

 	/* r = r % p modular reduction */
	carry += carry2;
	for (int32_t i = 0; i < 7; i++) {
		tmp = (uint64_t)r[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	carry += carry2;
	for (int32_t i = 7; i < 14; i++) {
		tmp = (uint64_t)r[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
}

/* @func: _fp448_sub (static)
 * #desc:
 *    prime field subtraction.
 *
 * #1: difference
 * #2: minuend
 * #3: subtract
 */
static void _fp448_sub(uint32_t r[14],
		const uint32_t a[14], const uint32_t b[14])
{
	uint32_t carry = 0, carry2 = 0;
	uint64_t tmp = 0;

	/* r = a - b */
	for (int32_t i = 0; i < 14; i++) {
		tmp = (uint64_t)a[i] - b[i] + (int32_t)carry2;
		r[i] = tmp & 0xffffffff;
		carry2 = tmp >> 32;
	}

	/* NOTE: 2^448 == 2^224+1 (mod 2^448-2^224-1) */

 	/* r = r % p modular reduction */
	tmp = (uint64_t)r[0] - (carry2 & 1);
	r[0] = tmp & 0xffffffff;
	carry = tmp >> 32;
	for (int32_t i = 1; i < 7; i++) {
		tmp = (uint64_t)r[i] + (int32_t)carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	tmp = (uint64_t)r[7] - (carry2 & 1);
	r[7] = tmp & 0xffffffff;
	carry = tmp >> 32;
	for (int32_t i = 8; i < 14; i++) {
		tmp = (uint64_t)r[i] + (int32_t)carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
}

/* @func: _fp448_mul (static)
 * #desc:
 *    prime field multiplication.
 *
 * #1: product
 * #2: multiplicand
 * #3: multiplier
 */
static void _fp448_mul(uint32_t r[14],
		const uint32_t a[14], const uint32_t b[14])
{
	uint32_t rr[28];
	uint32_t carry = 0, carry2 = 0;
	uint64_t tmp = 0;

	/* XXX: karatsuba optimization */

	/* rr = a * b */
	for (int32_t i = 0; i < 28; i++)
		rr[i] = 0;

	for (int32_t i = 0; i < 14; i++) {
		tmp = (uint64_t)a[0] * b[i] + carry2;
		rr[i] = tmp & 0xffffffff;
		carry2 = tmp >> 32;
	}
	rr[14] = carry2;

	for (int32_t i = 1; i < 14; i++) {
		carry2 = 0;
		for (int32_t j = 0; j < 14; j++) {
			tmp = (uint64_t)a[i] * b[j] + rr[i + j] + carry2;
			rr[i + j] = tmp & 0xffffffff;
			carry2 = tmp >> 32;
		}
		rr[i + 14] = carry2;
	}

	/* NOTE: 2^448 == 2^224+1 (mod 2^448-2^224-1) */

 	/* r = rr % p modular reduction */
	carry2 = 0;
	for (int32_t i = 0; i < 7; i++) { /* fold */
		tmp = (uint64_t)rr[i] + rr[i + 14] + rr[i + 21] + carry2;
		r[i] = tmp & 0xffffffff;
		carry2 = tmp >> 32;
	}
	for (int32_t i = 7; i < 14; i++) {
		tmp = (uint64_t)rr[i] + rr[i + 14] + rr[i + 14]
			+ rr[i + 7] + carry2;
		r[i] = tmp & 0xffffffff;
		carry2 = tmp >> 32;
	}

	carry += carry2;
	for (int32_t i = 0; i < 7; i++) {
		tmp = (uint64_t)r[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	carry += carry2;
	for (int32_t i = 7; i < 14; i++) {
		tmp = (uint64_t)r[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
}

/* @func: _fp448_mod (static)
 * #desc:
 *    prime field modular reduction (norm).
 *
 * #1: number
 */
static void _fp448_mod(uint32_t r[14])
{
	uint32_t carry = 0, carry2 = 0;
	uint64_t tmp = 0;

	/* NOTE: 2^448 == 2^224+1 (mod 2^448-2^224-1) */

	/* if r < p : adjust carry2 */
	carry2 += 1;
	for (int32_t i = 0; i < 7; i++) {
		tmp = (uint64_t)r[i] + carry2;
		carry2 = tmp >> 32;
	}

	carry2 += 1;
	for (int32_t i = 7; i < 14; i++) {
		tmp = (uint64_t)r[i] + carry2;
		carry2 = tmp >> 32;
	}

 	/* r = r % p modular reduction */
	carry += carry2;
	for (int32_t i = 0; i < 7; i++) {
		tmp = (uint64_t)r[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	carry += carry2;
	for (int32_t i = 7; i < 14; i++) {
		tmp = (uint64_t)r[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
}

/* @func: _fp448_inv (static)
 * #desc:
 *    modular inversion based on fermat's little theorem.
 *
 * #1: inverse modulus
 * #2: number
 */
static void _fp448_inv(uint32_t r[14], const uint32_t z[14])
{
	uint32_t t[14];

	_fp448_mul(t, z, z);
	_fp448_mul(t, t, z);
	for (int32_t i = 0; i < 221; i++) {
		_fp448_mul(t, t, t);
		_fp448_mul(t, t, z);
	}

	_fp448_mul(t, t, t);
	for (int32_t i = 0; i < 222; i++) {
		_fp448_mul(t, t, t);
		_fp448_mul(t, t, z);
	}

	_fp448_mul(t, t, t);
	_fp448_mul(t, t, t);
	_fp448_mul(r, t, z);
}

/* @func: _fp448_mul39081 (static)
 * #desc:
 *    multiplier/39081 and addition (x448).
 *
 * #1: product
 * #2: multiplicand
 * #3: addend
 */
static void _fp448_mul39081(uint32_t r[14],
		const uint32_t a[14], const uint32_t b[14])
{
	uint32_t carry = 0, carry2 = 0;
	uint64_t tmp = 0;

	/* r = a * 39081 + b, (156326 - 2) / 4 = 39081 */
	for (int32_t i = 0; i < 14; i++) {
		tmp = (uint64_t)a[i] * 39081 + b[i] + carry2;
		r[i] = tmp & 0xffffffff;
		carry2 = tmp >> 32;
	}

	/* NOTE: 2^448 == 2^224+1 (mod 2^448-2^224-1) */

 	/* r = rr % p modular reduction */
	carry += carry2;
	for (int32_t i = 0; i < 7; i++) {
		tmp = (uint64_t)r[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	carry += carry2;
	for (int32_t i = 7; i < 14; i++) {
		tmp = (uint64_t)r[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
}

/* @func: _fp448_pow (static)
 * #desc:
 *    fast exponentiation in constant-time.
 *
 * #1: result
 * #2: number
 * #3: exponential
 */
static void _fp448_pow(uint32_t r[14],
		const uint32_t a[14], const uint32_t b[14])
{
	uint32_t x[14], t[14], e[14];
	for (int32_t i = 0; i < 14; i++) {
		x[i] = a[i];
		e[i] = b[i];
		r[i] = 0;
	}
	r[0] = 1;

	for (int32_t i = 0; i < 448; i++) {
		_fp448_mul(t, r, x);
		_fp448_swap(r, t, e[i / 32] >> (i % 32));
		_fp448_mul(x, x, x);
	}
}

/* @func: _sc448_modw (static)
 * #desc:
 *    scalar high-word modular reduction fold.
 *
 * #1: result
 * #2: number
 * #3: high-word
 */
static void _sc448_modw(uint32_t r[14],
		const uint32_t a[14], uint32_t b)
{
	uint32_t rr[14];
	uint32_t carry = 0, carry2 = 0;
	uint64_t tmp = 0;

	/* rr = b * R */
	for (int32_t i = 0; i < 14; i++) {
		tmp = (uint64_t)b * _sc448_R[i] + carry;
		rr[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	/* r = a + rr */
	for (int32_t i = 0; i < 14; i++) {
		tmp = (uint64_t)a[i] + rr[i] + carry2;
		r[i] = tmp & 0xffffffff;
		carry2 = tmp >> 32;
	}

	/* rr = carry2 * R */
	carry = 0;
	for (int32_t i = 0; i < 14; i++) {
		tmp = (uint64_t)carry2 * _sc448_R[i] + carry;
		rr[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	/* r = r + rr */
	carry = 0;
	for (int32_t i = 0; i < 14; i++) {
		tmp = (uint64_t)r[i] + rr[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
}

/* @func: _sc448_mod (static)
 * #desc:
 *    scalar modular reduction (norm).
 *
 * #1: number
 */
static void _sc448_mod(uint32_t r[14])
{
	const uint32_t *p = _sc448_BPO[(r[13] >> 30) + 1]; /* 448 - 446 = 2 */
	uint32_t carry = 0;
	uint64_t tmp = 0;

	/* r = r - p */
	for (int32_t i = 0; i < 14; i++) {
		tmp = (uint64_t)r[i] - p[i] + (int32_t)carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	/* r = r + p */
	p = _sc448_BPO[carry & 1];
	carry = 0;
	for (int32_t i = 0; i < 14; i++) {
		tmp = (uint64_t)r[i] + p[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
}

/* @func: _sc448_add (static)
 * #desc:
 *    scalar addition.
 *
 * #1: sum
 * #2: addend
 * #3: addend
 */
static void _sc448_add(uint32_t r[14],
		const uint32_t a[14], const uint32_t b[14])
{
	uint32_t carry = 0;
	uint64_t tmp = 0;

	/* r = a + b */
	for (int32_t i = 0; i < 14; i++) {
		tmp = (uint64_t)a[i] + b[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	/* r = r % q */
	_sc448_modw(r, r, carry);
}

/* @func: _sc448_mul (static)
 * #desc:
 *    scalar multiplication.
 *
 * #1: product
 * #2: multiplicand
 * #3: multiplier
 */
static void _sc448_mul(uint32_t r[14],
		const uint32_t a[14], const uint32_t b[14])
{
	uint32_t rr[28];
	uint32_t carry = 0;
	uint64_t tmp = 0;

	for (int32_t i = 0; i < 28; i++)
		rr[i] = 0;

	/* rr = a * b */
	for (int32_t i = 0; i < 14; i++) {
		tmp = (uint64_t)a[0] * b[i] + carry;
		rr[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
	rr[14] = carry;

	for (int32_t i = 1; i < 14; i++) {
		carry = 0;
		for (int32_t j = 0; j < 14; j++) {
			tmp = (uint64_t)a[i] * b[j] + rr[i + j] + carry;
			rr[i + j] = tmp & 0xffffffff;
			carry = tmp >> 32;
		}
		rr[i + 14] = carry;
	}

	/* r = rr % q */
	_sc448_modw(rr + 13, rr + 13, rr[27]);
	_sc448_modw(rr + 12, rr + 12, rr[26]);
	_sc448_modw(rr + 11, rr + 11, rr[25]);
	_sc448_modw(rr + 10, rr + 10, rr[24]);
	_sc448_modw(rr + 9, rr + 9, rr[23]);
	_sc448_modw(rr + 8, rr + 8, rr[22]);
	_sc448_modw(rr + 7, rr + 7, rr[21]);
	_sc448_modw(rr + 6, rr + 6, rr[20]);
	_sc448_modw(rr + 5, rr + 5, rr[19]);
	_sc448_modw(rr + 4, rr + 4, rr[18]);
	_sc448_modw(rr + 3, rr + 3, rr[17]);
	_sc448_modw(rr + 2, rr + 2, rr[16]);
	_sc448_modw(rr + 1, rr + 1, rr[15]);
	_sc448_modw(r, rr, rr[14]);
}

/* @func: _sc448_digest (static)
 * #desc:
 *    shake256 digest modular reduction.
 *
 * #1: shake256 digest
 * #2: result
 */
static void _sc448_digest(const uint8_t dig[114], uint32_t r[14])
{
	uint32_t rr[29]; /* (456 * 2) < 928-bits */
	rr[28] = 0; /* mask */

	C_SYMBOL(memcpy)(rr, dig, 114),

	/* r = rr % q */
	_sc448_modw(rr + 14, rr + 14, rr[28]);
	_sc448_modw(rr + 13, rr + 13, rr[27]);
	_sc448_modw(rr + 12, rr + 12, rr[26]);
	_sc448_modw(rr + 11, rr + 11, rr[25]);
	_sc448_modw(rr + 10, rr + 10, rr[24]);
	_sc448_modw(rr + 9, rr + 9, rr[23]);
	_sc448_modw(rr + 8, rr + 8, rr[22]);
	_sc448_modw(rr + 7, rr + 7, rr[21]);
	_sc448_modw(rr + 6, rr + 6, rr[20]);
	_sc448_modw(rr + 5, rr + 5, rr[19]);
	_sc448_modw(rr + 4, rr + 4, rr[18]);
	_sc448_modw(rr + 3, rr + 3, rr[17]);
	_sc448_modw(rr + 2, rr + 2, rr[16]);
	_sc448_modw(rr + 1, rr + 1, rr[15]);
	_sc448_modw(r, rr, rr[14]);
}

/* @func: _x448_scalar_mul (static)
 * #desc:
 *    x448 montgomery ladder scalar multiplication.
 *
 * #1: input point
 * #2: base point
 * #3: scalar
 */
static void _x448_scalar_mul(const uint32_t k[14], const uint32_t b[14],
		uint32_t r[14])
{
	uint32_t x1[14], z1[14], x2[14], z2[14];
	for (int32_t i = 0; i < 14; i++) {
		x1[i] = 0;
		z1[i] = 0;
		x2[i] = b[i];
		z2[i] = 0;
	}
	x1[0] = 1;
	z2[0] = 1;
	/*
	 * x1, z1 = (1, 0)
	 * x2, z2 = (b, 1)
	 */

	/* montgomery projective coordinates */
	uint32_t A[14], AA[14], B[14], BB[14], E[14], C[14], D[14],
		DA[14], CB[14];
	for (int32_t i = 447; i >= 0; i--) {
		uint32_t k_i = k[i / 32] >> (i % 32);
		_fp448_swap(x1, x2, k_i);
		_fp448_swap(z1, z2, k_i);

		/* A = x1 + z1 */
		_fp448_add(A, x1, z1);
		/* AA = A ^ 2 */
		_fp448_mul(AA, A, A);

		/* B = x1 - z1 */
		_fp448_sub(B, x1, z1);
		/* BB = B ^ 2 */
		_fp448_mul(BB, B, B);

		/* E = AA - BB */
		_fp448_sub(E, AA, BB);
		/* C = x2 + z2 */
		_fp448_add(C, x2, z2);
		/* D = x2 - z2 */
		_fp448_sub(D, x2, z2);

		/* DA = D * A */
		_fp448_mul(DA, D, A);
		/* CB = C * B */
		_fp448_mul(CB, C, B);

		/* x2 = (DA * CB) ^ 2 */
		_fp448_add(x2, DA, CB);
		_fp448_mul(x2, x2, x2);

		/* z2 = b * ((DA - CB) ^ 2) */
		_fp448_sub(z2, DA, CB);
		_fp448_mul(z2, z2, z2);
		_fp448_mul(z2, z2, b);

		/* x1 = AA * BB */
		_fp448_mul(x1, AA, BB);

		/* z1 = E * (AA + 39081 * E) */
		_fp448_mul39081(z1, E, AA);
		_fp448_mul(z1, z1, E);

		_fp448_swap(x1, x2, k_i);
		_fp448_swap(z1, z2, k_i);
	}

	/* r = (inv(z1) * x1) % p */
	_fp448_inv(z1, z1);
	_fp448_mul(r, z1, x1);
	_fp448_mod(r);
}

/* @func: _ed448_point_add (static)
 * #desc:
 *    edwards curve point addition.
 *
 * #1: addend
 * #2: addend
 * #3: result
 */
static void _ed448_point_add(const struct ed448_point *xyz1,
		const struct ed448_point *xyz2, struct ed448_point *xyz3)
{
	uint32_t A[14], B[14], A1[14], B1[14], C1[14], D1[14], E1[14],
		F1[14], G1[14], H1[14];
	/*
	 * A1 = z1 * z2
	 * B1 = A1 ^ 2
	 * C1 = x1 * x2
	 * D1 = y1 * y2
	 * E1 = (d * C1) * D1
	 * F1 = B1 - E1
	 * G1 = B1 + E1
	 * H1 = (x1 + y1) * (x2 + y2)
	 * x3 = (A1 * F1) * (H1 - C1 - D1)
	 * y3 = (A1 * G1) * (D1 - C1)
	 * z3 = F1 * G1
	 */

	/* A1 = z1 * z2 */
	_fp448_mul(A1, xyz1->z, xyz2->z);
	/* B1 = A1 ^ 2 */
	_fp448_mul(B1, A1, A1);
	/* c1 = x1 * x2 */
	_fp448_mul(C1, xyz1->x, xyz2->x);
	/* d1 = y1 * y2 */
	_fp448_mul(D1, xyz1->y, xyz2->y);

	/* E1 = (d * C1) * D1 */
	_fp448_mul(E1, C1, _ed448_d);
	_fp448_mul(E1, E1, D1);

	/* F1 = B1 - E1 */
	_fp448_sub(F1, B1, E1);
	/* G1 = B1 + E1 */
	_fp448_add(G1, B1, E1);

	/* H1 = (x1 + y1) * (x2 + y2) */
	_fp448_add(A, xyz1->x, xyz1->y);
	_fp448_add(B, xyz2->x, xyz2->y);
	_fp448_mul(H1, A, B);

	/* x3 = (A1 * F1) * (H1 - C1 - D1) */
	_fp448_mul(A, A1, F1);
	_fp448_sub(B, H1, C1);
	_fp448_sub(B, B, D1);
	_fp448_mul(xyz3->x, A, B);

	/* y3 = (A1 * G1) * (D1 - C1) */
	_fp448_mul(A, A1, G1);
	_fp448_sub(B, D1, C1);
	_fp448_mul(xyz3->y, A, B);

	/* z3 = F1 * G1 */
	_fp448_mul(xyz3->z, F1, G1);
}

/* @func: _ed448_point_double (static)
 * #desc:
 *    edwards curve point doubling.
 *
 * #1: addend
 * #2: result
 */
static void _ed448_point_double(const struct ed448_point *xyz1,
		struct ed448_point *xyz3)
{
	uint32_t A[14], B1[14], C1[14], D1[14], E1[14], H1[14], J1[14];
	/*
	 * B1 = (x1 + y1) ^ 2
	 * C1 = x1 ^ 2
	 * D1 = y1 ^ 2
	 * E1 = C1 + D1
	 * H1 = z1 ^ 2
	 * J1 = E1 - (H1 * 2)
	 * x3 = J1 * (B1 - E1)
	 * y3 = E1 * (C1 - D1)
	 * x3 = E1 * J1
	 */

	/* B1 = (x1 + y1) ^ 2 */
	_fp448_add(B1, xyz1->x, xyz1->y);
	_fp448_mul(B1, B1, B1);

	/* C1 = x1 ^ 2 */
	_fp448_mul(C1, xyz1->x, xyz1->x);
	/* D1 = y1 ^ 2 */
	_fp448_mul(D1, xyz1->y, xyz1->y);
	/* E1 = C1 + D1 */
	_fp448_add(E1, C1, D1);
	/* H1 = z1 ^ 2 */
	_fp448_mul(H1, xyz1->z, xyz1->z);

	/* J1 = E1 - (H1 * 2) */
	_fp448_add(J1, H1, H1);
	_fp448_sub(J1, E1, J1);

	/* X3 = J1 * (B1 - E1) */
	_fp448_sub(A, B1, E1);
	_fp448_mul(xyz3->x, J1, A);

	/* y3 = E1 * (C1 - D1) */
	_fp448_sub(A, C1, D1);
	_fp448_mul(xyz3->y, E1, A);

	/* y3 = E1 * J1 */
	_fp448_mul(xyz3->z, E1, J1);
}

/* @func: _ed448_scalar_mul (static)
 * #desc:
 *    ed448 montgomery ladder scalar multiplication.
 *
 * #1: input point
 * #2: base point
 * #3: scalar
 */
static void _ed448_scalar_mul(const uint32_t k[14],
		const struct ed448_point *xyz1, struct ed448_point *xyz2)
{
	struct ed448_point _xyz1, _xyz2, _xyz3;
	for (int32_t i = 0; i < 14; i++) {
		_xyz1.x[i] = 0;
		_xyz1.y[i] = 0;
		_xyz1.z[i] = 0;
		_xyz2.x[i] = xyz1->x[i];
		_xyz2.y[i] = xyz1->y[i];
		_xyz2.z[i] = xyz1->z[i];
	}
	_xyz1.y[0] = 1;
	_xyz1.z[0] = 1;
	/*
	 * _xyz1 = (0, 1, 1)
	 * _xyz2 = xyz1
	 */

	/* edwards projective coordinates */
	for (int32_t i = 0; i < 448; i++) {
		_ed448_point_add(&_xyz1, &_xyz2, &_xyz3);

		uint32_t k_i = k[i / 32] >> (i % 32);
		_fp448_swap(_xyz1.x, _xyz3.x, k_i);
		_fp448_swap(_xyz1.y, _xyz3.y, k_i);
		_fp448_swap(_xyz1.z, _xyz3.z, k_i);

		/* _ed448_point_add(&_xyz2, &_xyz2, &_xyz2); */
		_ed448_point_double(&_xyz2, &_xyz2);
	}

	for (int32_t i = 0; i < 14; i++) {
		xyz2->x[i] = _xyz1.x[i];
		xyz2->y[i] = _xyz1.y[i];
		xyz2->z[i] = _xyz1.z[i];
	}
}

/* @func: _ed448_point_equal (static)
 * #desc:
 *    curve point comparison is equal.
 *
 * #1: curve point
 * #2: curve point
 * #r: 0: p1 != p2, 1: p1 == p2
 */
static int32_t _ed448_point_equal(const struct ed448_point *xyz1,
		const struct ed448_point *xyz2)
{
	uint32_t a[14], b[14], r = 0;
	/*
	 * if ((x1 * z2) - (x2 * z1))
	 *   return 0;
	 * if ((y1 * z2) - (y2 * z1))
	 *   return 0;
	 * return 1
	 */

	_fp448_mul(a, xyz1->x, xyz2->z);
	_fp448_mul(b, xyz2->x, xyz1->z);
	_fp448_sub(a, a, b);
	r = _fp448_iszero(a);

	_fp448_mul(a, xyz1->y, xyz2->z);
	_fp448_mul(b, xyz2->y, xyz1->z);
	_fp448_sub(a, a, b);
	r &= _fp448_iszero(a);

	/*
	 * 0 & 1 == 0
	 * 1 & 0 == 0
	 * 1 & 1 == 1
	 */
	return r;
}

/* @func: _ed448_point_recover_x (static)
 * #desc:
 *    calculate the corresponding curve point x.
 *
 * #1: curve point y
 * #2: sign of x
 * #3: curve point x
 */
static void _ed448_point_recover_x(const uint32_t y[14], uint32_t sign,
		uint32_t r[14])
{
	uint32_t x1[14], x2[14], y2[14], x[14];
	/*
	 * y2 = y ^ 2
	 * x1 = y2 - 1
	 * x2 = (d * y2) - 1
	 * x2 = (x1 * inv(x2, p)) % p
	 * x = modpow(x2, (p + 1) / 4, p)
	 * if ((x & 1) != sign)
	 *   x = p - x
	 */

	/* y2 = y ^ 2 */
	_fp448_mul(y2, y, y);
	/* x1 = y2 - 1 */
	_fp448_sub(x1, y2, _ed448_one);

	/* x2 = (d * y2) - 1 */
	_fp448_mul(x2, y2, _ed448_d);
	_fp448_sub(x2, x2, _ed448_one);

	/* x2 = (x1 * inv(x2)) % p */
	_fp448_inv(x2, x2);
	_fp448_mul(x2, x2, x1);
	_fp448_mod(x2);

	/* x = modpow(x2, (p + 1) / 4) % p */
	_fp448_pow(x, x2, _ed448_p14);
	_fp448_mod(x);

	/* x1 = p - x */
	_fp448_sub(x1, _fp448_p, x);
	/* x = if x & 1 != sign : swap x1 */
	_fp448_swap(x, x1, (x[0] & 1) ^ (sign & 1));

	/* r = x */
	for (int32_t i = 0; i < 14; i++)
		r[i] = x[i];
}

/* @func: _ed448_point_compress (static)
 * #desc:
 *    curve point compression.
 *
 * #1: curve point
 * #2: compress point
 */
static void _ed448_point_compress(const struct ed448_point *xyz1,
		uint32_t r[15])
{
	uint32_t x[14], z[14];
	/*
	 * x = (x1 * inv(z, p)) % p
	 * y = (y1 * inv(z, p)) % p
	 * r = y | ((x & 1) << 455)
	 */

	/* z = inv(z) */
	_fp448_inv(z, xyz1->z);

	/* x = (x1 * z) % p */
	_fp448_mul(x, xyz1->x, z);
	_fp448_mod(x);

	/* r = (y1 * z) % p */
	_fp448_mul(r, xyz1->y, z);
	_fp448_mod(r);

	/* r = y | ((x & 1) << 455) */
	r[14] = (x[0] & 1) << 7; /* x % 2 = 0 (non-negative and even) */
}

/* @func: _ed448_point_decompress (static)
 * #desc:
 *    curve point decompression.
 *
 * #1: compress point
 * #2: curve point
 */
static void _ed448_point_decompress(const uint32_t k[15],
		struct ed448_point *xyz1)
{
	/*
	 * y1 = k & ((1 << 448) - 1)
	 * x1 = rec_x(y1, (k >> 455) & 1)
	 * z1 = 1
	 */

	/* y1 = k & ((1 << 448) - 1) */
	for (int32_t i = 0; i < 14; i++)
		xyz1->y[i] = k[i];

	/* x1 = rec_x(y1, (k >> 455) & 1) */
	_ed448_point_recover_x(xyz1->y, (k[14] >> 7) & 1, xyz1->x);

	/* z1 = 1 */
	for (int32_t i = 0; i < 14; i++)
		xyz1->z[i] = 0;
	xyz1->z[0] = 1;
}

/* @func: ecdh_x448_public_key
 * #desc:
 *    x448 public key create function.
 *
 * #1: private key
 * #2: public key
 */
void F_SYMBOL(ecdh_x448_public_key)(const uint8_t *pri, uint8_t *pub)
{
	uint32_t _pri[14], _pub[14];

	C_SYMBOL(memcpy)(_pri, pri, ECDH_X448_PRI_LEN);

	/* key clamping */
	_pri[0] &= ~0x03U;
	_pri[13] |= 0x80000000;

	/* _pub = scalar(_pri, base) */
	_x448_scalar_mul(_pri, _x448_b, _pub);

	C_SYMBOL(memcpy)(pub, _pub, ECDH_X448_PUB_LEN);
}

/* @func: ecdh_x448_shared_key
 * #desc:
 *    x448 shared key create function.
 *
 * #1: private key
 * #2: public key
 * #3: shared key
 * #r: 0: success, -1: fail
 */
int32_t F_SYMBOL(ecdh_x448_shared_key)(const uint8_t *pri,
		const uint8_t *pub, uint8_t *key)
{
	uint32_t _pri[14], _pub[14], _key[14];

	C_SYMBOL(memcpy)(_pri, pri, ECDH_X448_PRI_LEN);
	C_SYMBOL(memcpy)(_pub, pub, ECDH_X448_PUB_LEN);

	/* key clamping */
	_pri[0] &= ~0x03U;
	_pri[13] |= 0x80000000;

	/* _key = scalar(_pri, _pub) */
	_x448_scalar_mul(_pri, _pub, _key);

	C_SYMBOL(memcpy)(key, _key, ECDH_X448_KEY_LEN);

	return 0 - _fp448_iszero(_key);
}

/* @func: eddsa_ed448_nonce_key
 * #desc:
 *    ed448 private and nonce operation function.
 *
 * #1: input private key
 * #2: private key
 * #3: nonce
 */
void F_SYMBOL(eddsa_ed448_nonce_key)(const uint8_t *inpri, uint8_t *pri,
		uint8_t *ran)
{
	uint32_t _pri[15];
	SHA3_NEW(sha_ctx);

	F_SYMBOL(sha3_init)(&sha_ctx, SHA3_SHAKE256_TYPE, 114);
	F_SYMBOL(sha3)(&sha_ctx, inpri, EDDSA_ED448_PRI_LEN);

	C_SYMBOL(memcpy)(_pri,
		&(SHA3_STATE(&sha_ctx, 0)),
		EDDSA_ED448_PRI_LEN);
	C_SYMBOL(memcpy)(ran,
		&(SHA3_STATE(&sha_ctx, EDDSA_ED448_RAN_LEN)),
		EDDSA_ED448_RAN_LEN);

	/* key clamping */
	_pri[0] &= ~0x03U;
	_pri[13] |= 0x80000000;

	C_SYMBOL(memcpy)(pri, _pri, EDDSA_ED448_PRI_LEN);
}

/* @func: eddsa_ed448_public_key
 * #desc:
 *    ed448 public key create function.
 *
 * #1: private key
 * #2: public key
 */
void F_SYMBOL(eddsa_ed448_public_key)(const uint8_t *pri, uint8_t *pub)
{
	uint32_t _pri[15], _pub[15], _ran[15];
	struct ed448_point xyz1;

	F_SYMBOL(eddsa_ed448_nonce_key)(pri, (uint8_t *)_pri,
		(uint8_t *)_ran);

	/* _pub = compress(scalar(_pri, base)) */
	_ed448_scalar_mul(_pri, &_ed448_base, &xyz1);
	_ed448_point_compress(&xyz1, _pub);

	C_SYMBOL(memcpy)(pub, _pub, EDDSA_ED448_PUB_LEN);
}

/* @func: eddsa_ed448_sign
 * #desc:
 *    ed448 signature function.
 *
 * #1: private key
 * #2: input message
 * #3: message length
 * #4: signature
 */
void F_SYMBOL(eddsa_ed448_sign)(const uint8_t *pri,
		const uint8_t *msg, uint32_t len, uint8_t *sign)
{
	uint32_t _pri[15], _pub[15], _ran[15], r[15], rs[15],
		h[15], s[15];
	struct ed448_point xyz1;
	SHA3_NEW(sha_ctx);

	F_SYMBOL(eddsa_ed448_nonce_key)(pri, (uint8_t *)_pri,
		(uint8_t *)_ran);
	F_SYMBOL(eddsa_ed448_public_key)(pri, (uint8_t *)_pub);

	/* r = sha(ctx + _ran + msg) % q */
	F_SYMBOL(sha3_init)(&sha_ctx, SHA3_SHAKE256_TYPE, 114);
	F_SYMBOL(sha3_process)(&sha_ctx,
		_ed448_ctx, ED448_CTX_LEN);
	F_SYMBOL(sha3_process)(&sha_ctx,
		(uint8_t *)_ran, EDDSA_ED448_RAN_LEN);
	F_SYMBOL(sha3_process)(&sha_ctx, msg, len);
	F_SYMBOL(sha3_finish)(&sha_ctx);
	_sc448_digest(&(SHA3_STATE(&sha_ctx, 0)), r);
	_sc448_mod(r);

	/* rs = compress(scalar(r, base)) */
	_ed448_scalar_mul(r, &_ed448_base, &xyz1);
	_ed448_point_compress(&xyz1, rs);

	/* h = sha(ctx + rs + _pub + msg) % q */
	F_SYMBOL(sha3_init)(&sha_ctx, SHA3_SHAKE256_TYPE, 114);
	F_SYMBOL(sha3_process)(&sha_ctx,
		_ed448_ctx, ED448_CTX_LEN);
	F_SYMBOL(sha3_process)(&sha_ctx,
		(uint8_t *)rs, EDDSA_ED448_LEN);
	F_SYMBOL(sha3_process)(&sha_ctx,
		(uint8_t *)_pub, EDDSA_ED448_PUB_LEN);
	F_SYMBOL(sha3_process)(&sha_ctx, msg, len);
	F_SYMBOL(sha3_finish)(&sha_ctx);
	_sc448_digest(&(SHA3_STATE(&sha_ctx, 0)), h);

	/* s = ((h * _pri) + r) % q */
	_sc448_mul(s, h, _pri);
	_sc448_add(s, s, r);
	_sc448_mod(s);
	s[14] = 0; /* mask */

	C_SYMBOL(memcpy)(sign, rs, EDDSA_ED448_LEN);
	C_SYMBOL(memcpy)(sign + EDDSA_ED448_LEN, s, EDDSA_ED448_LEN);
}

/* @func: eddsa_ed448_verify
 * #desc:
 *    ed448 signature verification function.
 *
 * #1: public key
 * #2: signature
 * #3: input message
 * #4: message length
 * #r: 0: success, -1: fail
 */
int32_t F_SYMBOL(eddsa_ed448_verify)(const uint8_t *pub,
		const uint8_t *sign, const uint8_t *msg, uint32_t len)
{
	uint32_t _pub[15], rs[15], s[15], h[15];
	struct ed448_point xyz1, xyz2, xyz3;
	SHA3_NEW(sha_ctx);

	C_SYMBOL(memcpy)(_pub, pub, EDDSA_ED448_PUB_LEN);
	C_SYMBOL(memcpy)(rs, sign, EDDSA_ED448_LEN);
	C_SYMBOL(memcpy)(s, sign + EDDSA_ED448_LEN, EDDSA_ED448_LEN);

	/* xyz1 = decompress(_pub) */
	_ed448_point_decompress(_pub, &xyz1);
	/* xyz2 = decompress(rs) */
	_ed448_point_decompress(rs, &xyz2);

	/* h = sha(ctx + rs + _pub + msg) % q */
	F_SYMBOL(sha3_init)(&sha_ctx, SHA3_SHAKE256_TYPE, 114);
	F_SYMBOL(sha3_process)(&sha_ctx,
		_ed448_ctx, ED448_CTX_LEN);
	F_SYMBOL(sha3_process)(&sha_ctx,
		(uint8_t *)rs, EDDSA_ED448_LEN);
	F_SYMBOL(sha3_process)(&sha_ctx,
		(uint8_t *)_pub, EDDSA_ED448_PUB_LEN);
	F_SYMBOL(sha3_process)(&sha_ctx, msg, len);
	F_SYMBOL(sha3_finish)(&sha_ctx);
	_sc448_digest(&(SHA3_STATE(&sha_ctx, 0)), h);
	_sc448_mod(h);

	/* xyz3 = scalar(h, xyz1) */
	_ed448_scalar_mul(h, &xyz1, &xyz3);
	/* xyz3 = add(xyz2, xyz3) */
	_ed448_point_add(&xyz2, &xyz3, &xyz3);

	/* xyz2 = scalar(s, base) */
	_ed448_scalar_mul(s, &_ed448_base, &xyz2);

	return _ed448_point_equal(&xyz3, &xyz2) - 1;
}
