/* @file: curve25519.c
 * #desc:
 *    The implementations of curve25519 ecdh and eddsa.
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
 * P = 2^255 - 19 */
static const uint32_t _fp25519_p[8] = {
	0xffffffed, 0xffffffff, 0xffffffff, 0xffffffff,
	0xffffffff, 0xffffffff, 0xffffffff, 0x7fffffff
	};

/* B = 9 */
static const uint32_t _x25519_b[8] = {
	9, 0, 0, 0, 0, 0, 0, 0
	};

/* D = -121665 * inv(121666, P) % P */
static const uint32_t _ed25519_d[8] = {
	0x135978a3, 0x75eb4dca, 0x4141d8ab, 0x00700a4d,
	0x7779e898, 0x8cc74079, 0x2b6ffe73, 0x52036cee
	};

/* one = 1 */
static const uint32_t _ed25519_one[8] = {
	1, 0, 0, 0, 0, 0, 0, 0
	};

/* p38 = (P + 3) / 8 */
static const uint32_t _ed25519_p38[8] = {
	0xfffffffe, 0xffffffff, 0xffffffff, 0xffffffff,
	0xffffffff, 0xffffffff, 0xffffffff, 0x0fffffff,
	};

/* p14 = modpow(2, (P - 1) / 4, P) */
static const uint32_t _ed25519_p14[8] = {
	0x4a0ea0b0, 0xc4ee1b27, 0xad2fe478, 0x2f431806,
	0x3dfbd7a7, 0x2b4d0099, 0x4fc1df0b, 0x2b832480,
	};

/* Q = 2^252 + 27742317777372353535851937790883648493 */
/* static const uint32_t _sc25519_q[8] = {
	0x5cf5d3ed, 0x5812631a, 0xa2f79cd6, 0x14def9de,
	0, 0, 0, 0x10000000
	}; */

/*
 * By = 463168356949264781694283940034751631413079938662562256157830336031
 *      65251855960
 * Bx = rec_x(By, 0)
 *   151122213495354007725011514095885315114540126930418572060461132839498
 *   47762202
 * Bz = 1
 * Bt = (Bx * By) % P
 */
struct ed25519_point {
	uint32_t x[8];
	uint32_t y[8];
	uint32_t z[8];
	uint32_t t[8];
};

static const struct ed25519_point _ed25519_base = {
	{
		0x8f25d51a, 0xc9562d60, 0x9525a7b2, 0x692cc760,
		0xfdd6dc5c, 0xc0a4e231, 0xcd6e53fe, 0x216936d3
	},
	{
		0x66666658, 0x66666666, 0x66666666, 0x66666666,
		0x66666666, 0x66666666, 0x66666666, 0x66666666
	},
	{
		1, 0, 0, 0, 0, 0, 0, 0
	},
	{
		0xa5b7dda3, 0x6dde8ab3, 0x775152f5, 0x20f09f80,
		0x64abe37d, 0x66ea4e8e, 0xd78b7665, 0x67875f0f
	}
	};

/*
 * Base Point Order
 *   q = 2^252 + 27742317777372353535851937790883648493
 *   P1..P15 = q * (1..15)
 */
static const uint32_t _sc25519_BPO[16][8] = {
	{
		0, 0, 0, 0, 0, 0, 0, 0
	},
	{
		0x5cf5d3ed, 0x5812631a, 0xa2f79cd6, 0x14def9de,
		0, 0, 0, 0x10000000
	},
	{
		0xb9eba7da, 0xb024c634, 0x45ef39ac, 0x29bdf3bd,
		0, 0, 0, 0x20000000
	},
	{
		0x16e17bc7, 0x0837294f, 0xe8e6d683, 0x3e9ced9b,
		0, 0, 0, 0x30000000
	},
	{
		0x73d74fb4, 0x60498c69, 0x8bde7359, 0x537be77a,
		0, 0, 0, 0x40000000
	},
	{
		0xd0cd23a1, 0xb85bef83, 0x2ed6102f, 0x685ae159,
		0, 0, 0, 0x50000000
	},
	{
		0x2dc2f78e, 0x106e529e, 0xd1cdad06, 0x7d39db37,
		0, 0, 0, 0x60000000
	},
	{
		0x8ab8cb7b, 0x6880b5b8, 0x74c549dc, 0x9218d516,
		0, 0, 0, 0x70000000
	},
	{
		0xe7ae9f68, 0xc09318d2, 0x17bce6b2, 0xa6f7cef5,
		0, 0, 0, 0x80000000
	},
	{
		0x44a47355, 0x18a57bed, 0xbab48389, 0xbbd6c8d3,
		0, 0, 0, 0x90000000
	},
	{
		0xa19a4742, 0x70b7df07, 0x5dac205f, 0xd0b5c2b2,
		0, 0, 0, 0xa0000000
	},
	{
		0xfe901b2f, 0xc8ca4221, 0x00a3bd35, 0xe594bc91,
		0, 0, 0, 0xb0000000
	},
	{
		0x5b85ef1c, 0x20dca53c, 0xa39b5a0c, 0xfa73b66f,
		0, 0, 0, 0xc0000000
	},
	{
		0xb87bc309, 0x78ef0856, 0x4692f6e2, 0x0f52b04e,
		1, 0, 0, 0xd0000000
	},
	{
		0x157196f6, 0xd1016b71, 0xe98a93b8, 0x2431aa2c,
		1, 0, 0, 0xe0000000
	},
	{
		0x72676ae3, 0x2913ce8b, 0x8c82308f, 0x3910a40b,
		1, 0, 0, 0xf0000000
	}
	};

/*
 * -R = (q - 2^256) % q
 *   443877084437957656573631004654138375888
 */
static const uint32_t _sc25519_minusR[8] = {
	0xcf5d3ed0, 0x812631a5, 0x2f79cd65, 0x4def9dea,
	1, 0, 0, 0
	};
/* end */

/* @func: _fp25519_swap (static)
 * #desc:
 *    constant-time the numerical swap.
 *
 * #1: number
 * #2: number
 * #3: low bit (0: keep, 1: swap)
 */
static void _fp25519_swap(uint32_t a[8], uint32_t b[8], uint32_t bit)
{
	bit = (~bit & 1) - 1;
	for (int32_t i = 0; i < 8; i++) {
		uint32_t t = bit & (a[i] ^ b[i]);
		a[i] ^= t;
		b[i] ^= t;
	}
}

/* @func: _fp25519_iszero (static)
 * #desc:
 *    check if a number is zero in constant-time.
 *
 * #1: number
 * #r: 0: non-zero, 1: is zero
 */
static uint32_t _fp25519_iszero(const uint32_t a[8])
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

/* @func: _fp25519_add (static)
 * #desc:
 *    prime field addition.
 *
 * #1: sum
 * #2: addend
 * #3: addend
 */
static void _fp25519_add(uint32_t r[8],
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

	/* NOTE: 2^256 == 38 (mod 2^255-19) */

	/* r = r % p modular reduction */
	carry *= 38;
	for (int32_t i = 0; i < 8; i++) {
		tmp = (uint64_t)r[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	carry *= 38;
	for (int32_t i = 0; i < 8; i++) {
		tmp = (uint64_t)r[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
}

/* @func: _fp25519_sub (static)
 * #desc:
 *    prime field subtraction.
 *
 * #1: difference
 * #2: minuend
 * #3: subtract
 */
static void _fp25519_sub(uint32_t r[8],
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

	/* NOTE: 2^256 == 38 (mod 2^255-19) */

	/* r = r % p modular reduction */
	tmp = (uint64_t)r[0] - (carry & 38);
	r[0] = tmp & 0xffffffff;
	carry = tmp >> 32;
	for (int32_t i = 1; i < 8; i++) {
		tmp = (uint64_t)r[i] + (int32_t)carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	tmp = (uint64_t)r[0] - (carry & 38);
	r[0] = tmp & 0xffffffff;
	carry = tmp >> 32;
	for (int32_t i = 1; i < 8; i++) {
		tmp = (uint64_t)r[i] + (int32_t)carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
}

/* @func: _fp25519_mul (static)
 * #desc:
 *    prime field multiplication.
 *
 * #1: product
 * #2: multiplicand
 * #3: multiplier
 */
static void _fp25519_mul(uint32_t r[8],
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

	/* NOTE: 2^256 == 38 (mod 2^255-19) */

	/* r = rr % p modular reduction */
	carry = 0;
	for (int32_t i = 0; i < 8; i++) { /* fold */
		tmp = (uint64_t)rr[i + 8] * 38 + rr[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	carry *= 38;
	for (int32_t i = 0; i < 8; i++) {
		tmp = (uint64_t)r[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	carry *= 38;
	for (int32_t i = 0; i < 8; i++) {
		tmp = (uint64_t)r[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
}

/* @func: _fp25519_mod (static)
 * #desc:
 *    prime field modular reduction (norm).
 *
 * #1: number
 */
static void _fp25519_mod(uint32_t r[8])
{
	uint32_t t[8];
	uint32_t carry = 0;
	uint64_t tmp = 0;

	/* r = r - p */
	for (int32_t i = 0; i < 8; i++) {
		tmp = (uint64_t)r[i] - _fp25519_p[i] + (int32_t)carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	/* t = if r < p : adjust t (p & mask) */
	for (int32_t i = 0; i < 8; i++)
		t[i] = carry;
	t[0] &= 0xffffffed;
	t[7] >>= 1;

	/* r = r + t */
	carry = 0;
	for (int32_t i = 0; i < 8; i++) {
		tmp = (uint64_t)r[i] + t[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	/* NOTE: 2p > 2^256-1 */

	/* r = r - p */
	carry = 0;
	for (int32_t i = 0; i < 8; i++) {
		tmp = (uint64_t)r[i] - _fp25519_p[i] + (int32_t)carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	/* t = if r < p : adjust t (p & mask) */
	for (int32_t i = 0; i < 8; i++)
		t[i] = carry;
	t[0] &= 0xffffffed;
	t[7] >>= 1;

	/* r = r + t */
	carry = 0;
	for (int32_t i = 0; i < 8; i++) {
		tmp = (uint64_t)r[i] + t[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
}

/* @func: _fp25519_inv (static)
 * #desc:
 *    modular inversion based on fermat's little theorem.
 *
 * #1: inverse modulus
 * #2: number
 */
static void _fp25519_inv(uint32_t r[8], const uint32_t z[8])
{
	uint32_t z2[8], z9[8], z11[8];
	uint32_t z2_5_0[8], z2_10_0[8], z2_20_0[8], z2_50_0[8], z2_100_0[8];
	uint32_t t0[8], t1[8];

	/* 2 */
	_fp25519_mul(z2, z, z);
	/* 4 */
	_fp25519_mul(t1, z2, z2);
	/* 8 */
	_fp25519_mul(t0, t1, t1);
	/* 9 */
	_fp25519_mul(z9, t0, z);
	/* 11 */
	_fp25519_mul(z11, z9, z2);
	/* 22 */
	_fp25519_mul(t0, z11, z11);
	/* (2^5) - (2^0) */
	_fp25519_mul(z2_5_0, t0, z9);

	/* (2^6) - (2^1) */
	_fp25519_mul(t0, z2_5_0, z2_5_0);
	/* (2^7) - (2^2) */
	_fp25519_mul(t1, t0, t0);
	/* (2^8) - (2^3) */
	_fp25519_mul(t0, t1, t1);
	/* (2^9) - (2^4) */
	_fp25519_mul(t1, t0, t0);
	/* (2^10) - (2^5) */
	_fp25519_mul(t0, t1, t1);
	/* (2^10) - (2^0) */
	_fp25519_mul(z2_10_0, t0, z2_5_0);

	/* (2^11) - (2^1) */
	_fp25519_mul(t0, z2_10_0, z2_10_0);
	/* (2^12) - (2^2) */
	_fp25519_mul(t1, t0, t0);
	/* (2^20) - (2^10) */
	for (int32_t i = 2; i < 10; i += 2) {
		_fp25519_mul(t0, t1, t1);
		_fp25519_mul(t1, t0, t0);
	}
	/* (2^20) - (2^0) */
	_fp25519_mul(z2_20_0, t1, z2_10_0);

	/* (2^21) - (2^1) */
	_fp25519_mul(t0, z2_20_0, z2_20_0);
	/* (2^22) - (2^2) */
	_fp25519_mul(t1, t0, t0);
	/* (2^40) - (2^20) */
	for (int32_t i = 2; i < 20; i += 2) {
		_fp25519_mul(t0, t1, t1);
		_fp25519_mul(t1, t0, t0);
	}
	/* (2^40) - (2^0) */
	_fp25519_mul(t0,t1,z2_20_0);

	/* (2^41) - (2^1) */
	_fp25519_mul(t1, t0, t0);
	/* (2^42) - (2^2) */
	_fp25519_mul(t0, t1, t1);
	/* (2^50) - (2^10) */
	for (int32_t i = 2; i < 10; i += 2) {
		_fp25519_mul(t1, t0, t0);
		_fp25519_mul(t0, t1, t1);
	}
	/* (2^50) - (2^0) */
	_fp25519_mul(z2_50_0, t0, z2_10_0);

	/* (2^51) - (2^1) */
	_fp25519_mul(t0, z2_50_0, z2_50_0);
	/* (2^52) - (2^2) */
	_fp25519_mul(t1, t0, t0);
	/* (2^100) - (2^50) */
	for (int32_t i = 2; i < 50; i += 2) {
		_fp25519_mul(t0, t1, t1);
		_fp25519_mul(t1, t0, t0);
	}
	/* (2^100) - (2^0) */
	_fp25519_mul(z2_100_0, t1, z2_50_0);

	/* (2^101) - (2^1) */
	_fp25519_mul(t1, z2_100_0, z2_100_0);
	/* (2^102) - (2^2) */
	_fp25519_mul(t0, t1, t1);
	/* (2^200) - (2^100) */
	for (int32_t i = 2; i < 100; i += 2) {
		_fp25519_mul(t1, t0, t0);
		_fp25519_mul(t0, t1, t1);
	}
	/* (2^200) - (2^0) */
	_fp25519_mul(t1, t0, z2_100_0);

	/* (2^201) - (2^1) */
	_fp25519_mul(t0, t1, t1);
	/* (2^202) - (2^2) */
	_fp25519_mul(t1, t0, t0);
	/* (2^250) - (2^50) */
	for (int32_t i = 2; i < 50; i += 2) {
		_fp25519_mul(t0, t1, t1);
		_fp25519_mul(t1, t0, t0);
	}
	/* (2^250) - (2^0) */
	_fp25519_mul(t0, t1, z2_50_0);

	/* (2^251) - (2^1) */
	_fp25519_mul(t1, t0, t0);
	/* (2^252) - (2^2) */
	_fp25519_mul(t0, t1, t1);
	/* (2^253) - (2^3) */
	_fp25519_mul(t1, t0, t0);
	/* (2^254) - (2^4) */
	_fp25519_mul(t0, t1, t1);
	/* (2^255) - (2^5) */
	_fp25519_mul(t1, t0, t0);
	/* (2^255) - 21 */
	_fp25519_mul(r, t1, z11);
}

/* @func: _fp25519_mul121665 (static)
 * #desc:
 *    multiplier/121665 and addition (x25519).
 *
 * #1: product
 * #2: multiplicand
 * #3: addend
 */
static void _fp25519_mul121665(uint32_t r[8],
		const uint32_t a[8], const uint32_t b[8])
{
	uint32_t carry = 0;
	uint64_t tmp = 0;

	/* r = a * 121665 + b, (486662 - 2) / 4 = 121665 */
	for (int32_t i = 0; i < 8; i++) {
		tmp = (uint64_t)a[i] * 121665 + b[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	/* NOTE: (2^256) == 38 (mod 2^255-19) */

	/* r = r % p modular reduction */
	carry *= 38;
	for (int32_t i = 0; i < 8; i++) {
		tmp = (uint64_t)r[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	carry *= 38;
	for (int32_t i = 0; i < 8; i++) {
		tmp = (uint64_t)r[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
}

/* @func: _fp25519_pow (static)
 * #desc:
 *    fast exponentiation in constant-time.
 *
 * #1: result
 * #2: number
 * #3: exponential
 */
static void _fp25519_pow(uint32_t r[8],
		const uint32_t a[8], const uint32_t b[8])
{
	uint32_t x[8], t[8], e[8];
	for (int32_t i = 0; i < 8; i++) {
		x[i] = a[i];
		e[i] = b[i];
		r[i] = 0;
	}
	r[0] = 1;

	for (int32_t i = 0; i < 256; i++) {
		_fp25519_mul(t, r, x);
		_fp25519_swap(r, t, e[i / 32] >> (i % 32));
		_fp25519_mul(x, x, x);
	}
}

/* @func: _sc25519_modw (static)
 * #desc:
 *    scalar high-word modular reduction fold.
 *
 * #1: result
 * #2: number
 * #3: high-word
 */
static void _sc25519_modw(uint32_t r[8],
		const uint32_t a[8], uint32_t b)
{
	const uint32_t *p;
	uint32_t rr[8];
	uint32_t carry = 0;
	uint64_t tmp = 0;

	/* NOTE: (a + (b * R)) % q == (a - (b * (-R))) % q */

	/* rr = b * minusR */
	for (int32_t i = 0; i < 8; i++) {
		tmp = (uint64_t)b * _sc25519_minusR[i] + carry;
		rr[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	/* r = a - rr */
	carry = 0;
	for (int32_t i = 0; i < 8; i++) {
		tmp = (uint64_t)a[i] - rr[i] + (int32_t)carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	/* r = r + p */
	p = _sc25519_BPO[carry & 1];
	carry = 0;
	for (int32_t i = 0; i < 8; i++) {
		tmp = (uint64_t)r[i] + p[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
}

/* @func: _sc25519_mod (static)
 * #desc:
 *    scalar modular reduction (norm).
 *
 * #1: number
 */
static void _sc25519_mod(uint32_t r[8])
{
	const uint32_t *p = _sc25519_BPO[r[7] >> 28]; /* 256 - 252 = 4 */
	uint32_t carry = 0;
	uint64_t tmp = 0;

	/* r = r - p */
	for (int32_t i = 0; i < 8; i++) {
		tmp = (uint64_t)r[i] - p[i] + (int32_t)carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	/* r = r + p */
	p = _sc25519_BPO[carry & 1];
	carry = 0;
	for (int32_t i = 0; i < 8; i++) {
		tmp = (uint64_t)r[i] + p[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
}

/* @func: _sc25519_add (static)
 * #desc:
 *    scalar addition.
 *
 * #1: sum
 * #2: addend
 * #3: addend
 */
static void _sc25519_add(uint32_t r[8],
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
	_sc25519_modw(r, r, carry);
}

/* @func: _sc25519_mul (static)
 * #desc:
 *    scalar multiplication.
 *
 * #1: product
 * #2: multiplicand
 * #3: multiplier
 */
static void _sc25519_mul(uint32_t r[8],
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
	_sc25519_modw(rr + 7, rr + 7, rr[15]);
	_sc25519_modw(rr + 6, rr + 6, rr[14]);
	_sc25519_modw(rr + 5, rr + 5, rr[13]);
	_sc25519_modw(rr + 4, rr + 4, rr[12]);
	_sc25519_modw(rr + 3, rr + 3, rr[11]);
	_sc25519_modw(rr + 2, rr + 2, rr[10]);
	_sc25519_modw(rr + 1, rr + 1, rr[9]);
	_sc25519_modw(r, rr, rr[8]);
}

/* @func: _sc25519_digest (static)
 * #desc:
 *    sha512 digest modular reduction.
 *
 * #1: sha512 digest
 * #2: result
 */
static void _sc25519_digest(const uint8_t dig[64], uint32_t r[8])
{
	uint32_t rr[16];
	C_SYMBOL(memcpy)(rr, dig, 64),

	/* r = rr % q */
	_sc25519_modw(rr + 7, rr + 7, rr[15]);
	_sc25519_modw(rr + 6, rr + 6, rr[14]);
	_sc25519_modw(rr + 5, rr + 5, rr[13]);
	_sc25519_modw(rr + 4, rr + 4, rr[12]);
	_sc25519_modw(rr + 3, rr + 3, rr[11]);
	_sc25519_modw(rr + 2, rr + 2, rr[10]);
	_sc25519_modw(rr + 1, rr + 1, rr[9]);
	_sc25519_modw(r, rr, rr[8]);
}

/* @func: _x25519_scalar_mul (static)
 * #desc:
 *    x25519 montgomery ladder scalar multiplication.
 *
 * #1: input point
 * #2: base point
 * #3: scalar
 */
static void _x25519_scalar_mul(const uint32_t k[8], const uint32_t b[8],
		uint32_t r[8])
{
	uint32_t x1[8], z1[8], x2[8], z2[8];
	for (int32_t i = 0; i < 8; i++) {
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
	uint32_t A[8], AA[8], B[8], BB[8], E[8], C[8], D[8], DA[8], CB[8];
	for (int32_t i = 255; i >= 0; i--) {
		uint32_t k_i = k[i / 32] >> (i % 32);
		_fp25519_swap(x1, x2, k_i);
		_fp25519_swap(z1, z2, k_i);

		/* A = x1 + z1 */
		_fp25519_add(A, x1, z1);
		/* AA = A ^ 2 */
		_fp25519_mul(AA, A, A);

		/* B = x1 - z1 */
		_fp25519_sub(B, x1, z1);
		/* BB = B ^ 2 */
		_fp25519_mul(BB, B, B);

		/* E = AA - BB */
		_fp25519_sub(E, AA, BB);
		/* C = x2 + z2 */
		_fp25519_add(C, x2, z2);
		/* D = x2 - z2 */
		_fp25519_sub(D, x2, z2);

		/* DA = D * A */
		_fp25519_mul(DA, D, A);
		/* CB = C * B */
		_fp25519_mul(CB, C, B);

		/* x2 = (DA * CB) ^ 2 */
		_fp25519_add(x2, DA, CB);
		_fp25519_mul(x2, x2, x2);

		/* z2 = b * ((DA - CB) ^ 2) */
		_fp25519_sub(z2, DA, CB);
		_fp25519_mul(z2, z2, z2);
		_fp25519_mul(z2, z2, b);

		/* x1 = AA * BB */
		_fp25519_mul(x1, AA, BB);

		/* z1 = E * (AA + 121665 * E) */
		_fp25519_mul121665(z1, E, AA);
		_fp25519_mul(z1, z1, E);

		_fp25519_swap(x1, x2, k_i);
		_fp25519_swap(z1, z2, k_i);
	}

	/* r = (inv(z1) * x1) % p */
	_fp25519_inv(z1, z1);
	_fp25519_mul(r, z1, x1);
	_fp25519_mod(r);
}

/* @func: _ed25519_point_add (static)
 * #desc:
 *    edwards curve point addition.
 *
 * #1: addend
 * #2: addend
 * #3: result
 */
static void _ed25519_point_add(const struct ed25519_point *xyz1,
		const struct ed25519_point *xyz2, struct ed25519_point *xyz3)
{
	uint32_t A[8], B[8], A1[8], B1[8], C1[8], D1[8], E1[8], F1[8],
		G1[8], H1[8];
	/*
	 * A1 = (y1 - x1) * (y2 - x2)
	 * B1 = (y1 + x1) * (y2 + x2)
	 * C1 = ((t1 * t2) * d) * 2
	 * D1 = (z1 * z2) * 2
	 * E1 = B1 - A1
	 * F1 = D1 - C1
	 * G1 = D1 + C1
	 * H1 = B1 + A1
	 * x3 = E1 * F1
	 * y3 = G1 * H1
	 * t3 = E1 * H1
	 * z3 = F1 * G1
	 */

	/* A1 = (y1 - x1) * (y2 - x2) */
	_fp25519_sub(A, xyz1->y, xyz1->x);
	_fp25519_sub(B, xyz2->y, xyz2->x);
	_fp25519_mul(A1, A, B);

	/* B1 = (y1 + x1) * (y2 + x2) */
	_fp25519_add(A, xyz1->y, xyz1->x);
	_fp25519_add(B, xyz2->y, xyz2->x);
	_fp25519_mul(B1, A, B);

	/* C1 = ((t1 * t2) * d) * 2 */
	_fp25519_mul(C1, xyz1->t, xyz2->t);
	_fp25519_mul(C1, C1, _ed25519_d);
	_fp25519_add(C1, C1, C1);

	/* D1 = (z1 * z2) * 2 */
	_fp25519_mul(D1, xyz1->z, xyz2->z);
	_fp25519_add(D1, D1, D1);

	/* E1 = B1 - A1 */
	_fp25519_sub(E1, B1, A1);
	/* F1 = D1 - C1 */
	_fp25519_sub(F1, D1, C1);
	/* G1 = D1 + C1 */
	_fp25519_add(G1, D1, C1);
	/* H1 = B1 + A1 */
	_fp25519_add(H1, B1, A1);

	/* x3 = E1 * F1 */
	_fp25519_mul(xyz3->x, E1, F1);
	/* y3 = G1 * H1 */
	_fp25519_mul(xyz3->y, G1, H1);
	/* t3 = E1 * H1 */
	_fp25519_mul(xyz3->t, E1, H1);
	/* z3 = F1 * G1 */
	_fp25519_mul(xyz3->z, F1, G1);
}

/* @func: _ed25519_point_double (static)
 * #desc:
 *    edwards curve point doubling.
 *
 * #1: addend
 * #2: result
 */
static void _ed25519_point_double(const struct ed25519_point *xyz1,
		struct ed25519_point *xyz3)
{
	uint32_t A1[8], B1[8], C1[8], H1[8], E1[8], G1[8], F1[8];
	/*
	 * A1 = x1 ^ 2
	 * B1 = y1 ^ 2
	 * C1 = (z1 ^ 2) * 2
	 * H1 = A1 + B1
	 * E1 = H1 - ((x1 + y1) ^ 2)
	 * G1 = A1 - B1
	 * F1 = C1 + G1
	 * x3 = E1 * F1
	 * y3 = G1 * H1
	 * t3 = E1 * H1
	 * z3 = F1 * G1
	 */

	/* A1 = x1 ^ 2 */
	_fp25519_mul(A1, xyz1->x, xyz1->x);
	/* B1 = y1 ^ 2 */
	_fp25519_mul(B1, xyz1->y, xyz1->y);

	/* C1 = (z1 ^ 2) * 2 */
	_fp25519_mul(C1, xyz1->z, xyz1->z);
	_fp25519_add(C1, C1, C1);

	/* H1 = A1 + B1 */
	_fp25519_add(H1, A1, B1);

	/* E1 = H1 - ((x1 + y1) ^ 2) */
	_fp25519_add(E1, xyz1->x, xyz1->y);
	_fp25519_mul(E1, E1, E1);
	_fp25519_sub(E1, H1, E1);

	/* G1 = A1 - B1 */
	_fp25519_sub(G1, A1, B1);

	/* F1 = C1 + G1 */
	_fp25519_add(F1, C1, G1);

	/* x3 = E1 * F1 */
	_fp25519_mul(xyz3->x, E1, F1);
	/* y3 = G1 * H1 */
	_fp25519_mul(xyz3->y, G1, H1);
	/* t3 = E1 * H1 */
	_fp25519_mul(xyz3->t, E1, H1);
	/* z3 = F1 * G1 */
	_fp25519_mul(xyz3->z, F1, G1);
}

/* @func: _ed25519_scalar_mul (static)
 * #desc:
 *    ed25519 montgomery ladder scalar multiplication.
 *
 * #1: input point
 * #2: base point
 * #3: scalar
 */
static void _ed25519_scalar_mul(const uint32_t k[8],
		const struct ed25519_point *xyz1, struct ed25519_point *xyz2)
{
	struct ed25519_point _xyz1, _xyz2, _xyz3;
	for (int32_t i = 0; i < 8; i++) {
		_xyz1.x[i] = 0;
		_xyz1.y[i] = 0;
		_xyz1.z[i] = 0;
		_xyz1.t[i] = 0;
		_xyz2.x[i] = xyz1->x[i];
		_xyz2.y[i] = xyz1->y[i];
		_xyz2.z[i] = xyz1->z[i];
		_xyz2.t[i] = xyz1->t[i];
	}
	_xyz1.y[0] = 1;
	_xyz1.z[0] = 1;
	/*
	 * _xyz1 = (0, 1, 1, 0)
	 * _xyz2 = xyz1
	 */

	/* twisted edwards and extended projective coordinates */
	for (int32_t i = 0; i < 256; i++) {
		_ed25519_point_add(&_xyz1, &_xyz2, &_xyz3);

		uint32_t k_i = k[i / 32] >> (i % 32);
		_fp25519_swap(_xyz1.x, _xyz3.x, k_i);
		_fp25519_swap(_xyz1.y, _xyz3.y, k_i);
		_fp25519_swap(_xyz1.z, _xyz3.z, k_i);
		_fp25519_swap(_xyz1.t, _xyz3.t, k_i);

		/* _ed25519_point_add(&_xyz2, &_xyz2, &_xyz2); */
		_ed25519_point_double(&_xyz2, &_xyz2);
	}

	for (int32_t i = 0; i < 8; i++) {
		xyz2->x[i] = _xyz1.x[i];
		xyz2->y[i] = _xyz1.y[i];
		xyz2->z[i] = _xyz1.z[i];
		xyz2->t[i] = _xyz1.t[i];
	}
}

/* @func: _ed25519_point_equal (static)
 * #desc:
 *    curve point comparison is equal.
 *
 * #1: curve point
 * #2: curve point
 * #r: 0: p1 != p2, 1: p1 == p2
 */
static int32_t _ed25519_point_equal(const struct ed25519_point *xyz1,
		const struct ed25519_point *xyz2)
{
	uint32_t a[8], b[8], r = 0;
	/*
	 * if ((x1 * z2) - (x2 * z1))
	 *   return 0;
	 * if ((y1 * z2) - (y2 * z1))
	 *   return 0;
	 * return 1
	 */

	_fp25519_mul(a, xyz1->x, xyz2->z);
	_fp25519_mul(b, xyz2->x, xyz1->z);
	_fp25519_sub(a, a, b);
	r = _fp25519_iszero(a);

	_fp25519_mul(a, xyz1->y, xyz2->z);
	_fp25519_mul(b, xyz2->y, xyz1->z);
	_fp25519_sub(a, a, b);
	r &= _fp25519_iszero(a);

	/*
	 * 0 & 1 == 0
	 * 1 & 0 == 0
	 * 1 & 1 == 1
	 */
	return r;
}

/* @func: _ed25519_point_recover_x (static)
 * #desc:
 *    calculate the corresponding curve point x.
 *
 * #1: curve point y
 * #2: sign of x
 * #3: curve point x
 */
static void _ed25519_point_recover_x(const uint32_t y[8], uint32_t sign,
		uint32_t r[8])
{
	uint32_t x1[8], x2[8], y2[8], x[8], t1[8], iszero = 0;
	/*
	 * y2 = y ^ 2
	 * x1 = y2 - 1
	 * x2 = (d * y2) + 1
	 * x2 = (x1 * inv(x2, p)) % p
	 * x = modpow(x2, (p + 3) / 8, p)
	 * if ((x ^ 2) - x2)
	 *   x = x * modpow(2, (p - 1) / 4, p)
	 * if ((x & 1) != sign)
	 *   x = p - x
	 */

	/* y2 = y ^ 2 */
	_fp25519_mul(y2, y, y);
	/* x1 = y2 - 1 */
	_fp25519_sub(x1, y2, _ed25519_one);

	/* x2 = (d * y2) + 1 */
	_fp25519_mul(x2, y2, _ed25519_d);
	_fp25519_add(x2, x2, _ed25519_one);

	/* x2 = (x1 * inv(x2)) % p */
	_fp25519_inv(x2, x2);
	_fp25519_mul(x2, x2, x1);
	_fp25519_mod(x2);

	/* x = modpow(x2, (p + 3) / 8) % p */
	_fp25519_pow(x, x2, _ed25519_p38);
	_fp25519_mod(x);

	/* t1 = ((x ^ 2) % p) - x2 */
	_fp25519_mul(t1, x, x);
	_fp25519_mod(t1);
	_fp25519_sub(t1, t1, x2);
	iszero = _fp25519_iszero(t1);

	/* x2 = (x * modpow(2, (p - 1) / 4, p)) % p */
	_fp25519_mul(x2, x, _ed25519_p14);
	_fp25519_mod(x2);
	/* x = if t1 != 0 : swap x2 */
	_fp25519_swap(x, x2, ~iszero);

	/* x1 = p - x */
	_fp25519_sub(x1, _fp25519_p, x);
	/* x = if (x & 1) != sign : swap x1 */
	_fp25519_swap(x, x1, (x[0] & 1) ^ (sign & 1));

	/* r = x */
	for (int32_t i = 0; i < 8; i++)
		r[i] = x[i];
}

/* @func: _ed25519_point_compress (static)
 * #desc:
 *    curve point compression.
 *
 * #1: curve point
 * #2: compress point
 */
static void _ed25519_point_compress(const struct ed25519_point *xyz1,
		uint32_t r[8])
{
	uint32_t x[8], z[8];
	/*
	 * x = (x1 * inv(z, p)) % p
	 * y = (y1 * inv(z, p)) % p
	 * r = y | ((x & 1) << 255)
	 */

	/* z = inv(z1) */
	_fp25519_inv(z, xyz1->z);

	/* x = (x1 * z) % p */
	_fp25519_mul(x, xyz1->x, z);
	_fp25519_mod(x);

	/* r = (y1 * z) % p */
	_fp25519_mul(r, xyz1->y, z);
	_fp25519_mod(r);

	/* r = r | ((x & 1) << 255) */
	r[7] |= (x[0] & 1) << 31; /* x % 2 = 0 (non-negative and even) */
}

/* @func: _ed25519_point_decompress (static)
 * #desc:
 *    curve point decompression.
 *
 * #1: compress point
 * #2: curve point
 */
static void _ed25519_point_decompress(const uint32_t k[8],
		struct ed25519_point *xyz1)
{
	/*
	 * y1 = k & ((1 << 255) - 1)
	 * x1 = rec_x(y1, k >> 255)
	 * z1 = 1
	 * t1 = x1 * y1
	 */

	/* y1 = k & ((1 << 255) - 1) */
	for (int32_t i = 0; i < 8; i++)
		xyz1->y[i] = k[i];
	xyz1->y[7] &= 0x7fffffff; /* mask high-bit */

	/* x1 = rec_x(y1, k >> 255) */
	_ed25519_point_recover_x(xyz1->y, k[7] >> 31, xyz1->x);

	/* t1 = x1 * y1 */
	_fp25519_mul(xyz1->t, xyz1->x, xyz1->y);

	/* z1 = 1 */
	for (int32_t i = 0; i < 8; i++)
		xyz1->z[i] = 0;
	xyz1->z[0] = 1;
}

/* @func: ecdh_x25519_public_key
 * #desc:
 *    x25519 public key create function.
 *
 * #1: private key
 * #2: public key
 */
void F_SYMBOL(ecdh_x25519_public_key)(const uint8_t *pri, uint8_t *pub)
{
	uint32_t _pri[8], _pub[8];

	C_SYMBOL(memcpy)(_pri, pri, ECDH_X25519_PRI_LEN);

	/* key clamping */
	_pri[0] &= ~0x07U;
	_pri[7] |= 0x40000000;
	_pri[7] &= 0x7fffffff;

	/* _pub = scalar(_pri, base) */
	_x25519_scalar_mul(_pri, _x25519_b, _pub);

	C_SYMBOL(memcpy)(pub, _pub, ECDH_X25519_PUB_LEN);
}

/* @func: ecdh_x25519_shared_key
 * #desc:
 *    x25519 shared key create function.
 *
 * #1: private key
 * #2: public key
 * #3: shared key
 * #r: 0: success, -1: fail
 */
int32_t F_SYMBOL(ecdh_x25519_shared_key)(const uint8_t *pri,
		const uint8_t *pub, uint8_t *key)
{
	uint32_t _pri[8], _pub[8], _key[8];

	C_SYMBOL(memcpy)(_pri, pri, ECDH_X25519_PRI_LEN);
	C_SYMBOL(memcpy)(_pub, pub, ECDH_X25519_PUB_LEN);

	/* key clamping */
	_pri[0] &= ~0x07U;
	_pri[7] |= 0x40000000;
	_pri[7] &= 0x7fffffff;

	/* mask the most significant bit of 'u' (rfc7748-5.0) */
	_pub[7] &= 0x7fffffff;

	/* _key = scalar(_pri, _pub) */
	_x25519_scalar_mul(_pri, _pub, _key);

	C_SYMBOL(memcpy)(key, _key, ECDH_X25519_KEY_LEN);

	return 0 - _fp25519_iszero(_key);
}

/* @func: eddsa_ed25519_nonce_key
 * #desc:
 *    ed25519 private and nonce operation function.
 *
 * #1: input private key
 * #2: private key
 * #3: nonce
 */
void F_SYMBOL(eddsa_ed25519_nonce_key)(const uint8_t *inpri, uint8_t *pri,
		uint8_t *ran)
{
	uint32_t _pri[8];
	SHA512_NEW(sha_ctx);

	F_SYMBOL(sha512_init)(&sha_ctx);
	F_SYMBOL(sha512)(&sha_ctx, inpri, EDDSA_ED25519_PRI_LEN);

	C_SYMBOL(memcpy)(_pri,
		&(SHA512_STATE(&sha_ctx, 0)),
		EDDSA_ED25519_PRI_LEN);
	C_SYMBOL(memcpy)(ran,
		&(SHA512_STATE(&sha_ctx, EDDSA_ED25519_RAN_LEN)),
		EDDSA_ED25519_RAN_LEN);

	/* key clamping */
	_pri[0] &= ~0x07U;
	_pri[7] |= 0x40000000;
	_pri[7] &= 0x7fffffff;

	C_SYMBOL(memcpy)(pri, _pri, EDDSA_ED25519_PRI_LEN);
}

/* @func: eddsa_ed25519_public_key
 * #desc:
 *    ed25519 public key create function.
 *
 * #1: private key
 * #2: public key
 */
void F_SYMBOL(eddsa_ed25519_public_key)(const uint8_t *pri, uint8_t *pub)
{
	uint32_t _pri[8], _pub[8], _ran[8];
	struct ed25519_point xyz1;

	F_SYMBOL(eddsa_ed25519_nonce_key)(pri, (uint8_t *)_pri,
		(uint8_t *)_ran);

	/* _pub = compress(scalar(_pri, base)) */
	_ed25519_scalar_mul(_pri, &_ed25519_base, &xyz1);
	_ed25519_point_compress(&xyz1, _pub);

	C_SYMBOL(memcpy)(pub, _pub, EDDSA_ED25519_PUB_LEN);
}

/* @func: eddsa_ed25519_sign
 * #desc:
 *    ed25519 signature function.
 *
 * #1: private key
 * #2: input message
 * #3: message length
 * #4: signature
 */
void F_SYMBOL(eddsa_ed25519_sign)(const uint8_t *pri,
		const uint8_t *msg, uint32_t len, uint8_t *sign)
{
	uint32_t _pri[8], _pub[8], _ran[8], r[8], rs[8], h[8], s[8];
	struct ed25519_point xyz1;
	SHA512_NEW(sha_ctx);

	F_SYMBOL(eddsa_ed25519_nonce_key)(pri, (uint8_t *)_pri,
		(uint8_t *)_ran);
	F_SYMBOL(eddsa_ed25519_public_key)(pri, (uint8_t *)_pub);

	/* r = sha(_ran + msg) % q */
	F_SYMBOL(sha512_init)(&sha_ctx);
	F_SYMBOL(sha512_process)(&sha_ctx,
		(uint8_t *)_ran, EDDSA_ED25519_RAN_LEN);
	F_SYMBOL(sha512_process)(&sha_ctx, msg, len);
	F_SYMBOL(sha512_finish)(&sha_ctx,
		len + EDDSA_ED25519_RAN_LEN);
	_sc25519_digest(&(SHA512_STATE(&sha_ctx, 0)), r);
	_sc25519_mod(r);

	/* rs = compress(scalar(r, base)) */
	_ed25519_scalar_mul(r, &_ed25519_base, &xyz1);
	_ed25519_point_compress(&xyz1, rs);

	/* h = sha(rs + _pub + msg) % q */
	F_SYMBOL(sha512_init)(&sha_ctx);
	F_SYMBOL(sha512_process)(&sha_ctx,
		(uint8_t *)rs, EDDSA_ED25519_LEN);
	F_SYMBOL(sha512_process)(&sha_ctx,
		(uint8_t *)_pub, EDDSA_ED25519_PUB_LEN);
	F_SYMBOL(sha512_process)(&sha_ctx, msg, len);
	F_SYMBOL(sha512_finish)(&sha_ctx,
		len + (EDDSA_ED25519_LEN + EDDSA_ED25519_PUB_LEN));
	_sc25519_digest(&(SHA512_STATE(&sha_ctx, 0)), h);

	/* s = ((h * _pri) + r) % q */
	_sc25519_mul(s, h, _pri);
	_sc25519_add(s, s, r);
	_sc25519_mod(s);

	C_SYMBOL(memcpy)(sign, rs, EDDSA_ED25519_LEN);
	C_SYMBOL(memcpy)(sign + EDDSA_ED25519_LEN, s, EDDSA_ED25519_LEN);
}

/* @func: eddsa_ed25519_verify
 * #desc:
 *    ed25519 signature verification function.
 *
 * #1: public key
 * #2: signature
 * #3: input message
 * #4: message length
 * #r: 0: success, -1: fail
 */
int32_t F_SYMBOL(eddsa_ed25519_verify)(const uint8_t *pub,
		const uint8_t *sign, const uint8_t *msg, uint32_t len)
{
	uint32_t _pub[8], rs[8], s[8], h[8];
	struct ed25519_point xyz1, xyz2, xyz3;
	SHA512_NEW(sha_ctx);

	C_SYMBOL(memcpy)(_pub, pub, EDDSA_ED25519_PUB_LEN);
	C_SYMBOL(memcpy)(rs, sign, EDDSA_ED25519_LEN);
	C_SYMBOL(memcpy)(s, sign + EDDSA_ED25519_LEN, EDDSA_ED25519_LEN);

	/* xyz1 = decompress(_pub) */
	_ed25519_point_decompress(_pub, &xyz1);
	/* xyz2 = decompress(rs) */
	_ed25519_point_decompress(rs, &xyz2);

	/* h = sha(rs + _pub + msg) % q */
	F_SYMBOL(sha512_init)(&sha_ctx);
	F_SYMBOL(sha512_process)(&sha_ctx,
		(uint8_t *)rs, EDDSA_ED25519_LEN);
	F_SYMBOL(sha512_process)(&sha_ctx,
		(uint8_t *)_pub, EDDSA_ED25519_PUB_LEN);
	F_SYMBOL(sha512_process)(&sha_ctx, msg, len);
	F_SYMBOL(sha512_finish)(&sha_ctx,
		len + (EDDSA_ED25519_LEN + EDDSA_ED25519_PUB_LEN));
	_sc25519_digest(&(SHA512_STATE(&sha_ctx, 0)), h);
	_sc25519_mod(h);

	/* xyz3 = scalar(h, xyz1) */
	_ed25519_scalar_mul(h, &xyz1, &xyz3);
	/* xyz3 = add(xyz2, xyz3) */
	_ed25519_point_add(&xyz2, &xyz3, &xyz3);

	/* xyz2 = scalar(s, base) */
	_ed25519_scalar_mul(s, &_ed25519_base, &xyz2);

	return _ed25519_point_equal(&xyz3, &xyz2) - 1;
}
