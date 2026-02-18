/* @file: bn4352.c
 * #desc:
 *    The implementations of big number calculation (4352-bit).
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
#include <demoz/lib/bn.h>


/* @func: bn4352_move
 * #desc:
 *    big number move.
 *
 * #1: target
 * #2: source
 */
void F_SYMBOL(bn4352_move)(bn4352_t *t, const bn4352_t *s)
{
	C_SYMBOL(memcpy)(t, s, sizeof(bn4352_t));
}

/* @func: bn4352_swap
 * #desc:
 *    big number swap.
 *
 * #1: target
 * #2: source
 */
void F_SYMBOL(bn4352_swap)(bn4352_t *a, bn4352_t *b)
{
	bn4352_t t;
	C_SYMBOL(memcpy)(&t, a, sizeof(bn4352_t));
	C_SYMBOL(memcpy)(a, b, sizeof(bn4352_t));
	C_SYMBOL(memcpy)(b, &t, sizeof(bn4352_t));
}

/* @func: bn4352_set1
 * #desc:
 *    big number value set (single a word).
 *
 * #1: target
 * #2: number
 */
void F_SYMBOL(bn4352_set1)(bn4352_t *t, uint32_t n)
{
	C_SYMBOL(memset)(t, 0, sizeof(bn4352_t));
	t->n[0] = n;
}

/* @func: bn4352_from_bytes
 * #desc:
 *    copy from bytes to big number.
 *
 * #1: target
 * #2: source bytes
 * #3: bytes length
 */
void F_SYMBOL(bn4352_from_bytes)(bn4352_t *t, const uint8_t *s, uint32_t len)
{
	F_SYMBOL(bn4352_set1)(t, 0);
	C_SYMBOL(memcpy)(t->n, s, len);
}

/* @func: bn4352_to_bytes
 * #desc:
 *    copy from big number to bytes.
 *
 * #1: source
 * #2: target bytes
 * #3: bytes length
 */
void F_SYMBOL(bn4352_to_bytes)(const bn4352_t *s, uint8_t *t, uint32_t len)
{
	C_SYMBOL(memcpy)(t, s->n, len);
}

/* @func: bn4352_bits
 * #desc:
 *    get the bits length of big number.
 *
 * #1: target
 * #r: bits number
 */
int32_t F_SYMBOL(bn4352_bits)(const bn4352_t *t)
{
	const uint32_t *_n = t->n;

	static const uint32_t pow2[32] = {
		0x00000001, 0x00000002, 0x00000004,
		0x00000008, 0x00000010, 0x00000020,
		0x00000040, 0x00000080, 0x00000100,
		0x00000200, 0x00000400, 0x00000800,
		0x00001000, 0x00002000, 0x00004000,
		0x00008000, 0x00010000, 0x00020000,
		0x00040000, 0x00080000, 0x00100000,
		0x00200000, 0x00400000, 0x00800000,
		0x01000000, 0x02000000, 0x04000000,
		0x08000000, 0x10000000, 0x20000000,
		0x40000000, 0x80000000
		};

	for (int32_t i = BN4352_N - 1; i >= 0; i--) {
		if (_n[i]) {
			for (int32_t k = 31; k >= 0; k--) {
				if (_n[i] & pow2[k])
					return (i * 32) + k + 1;
			}
		}
	}

	return 0;
}

/* @func: bn4352_lshift
 * #desc:
 *    big number left shift.
 *
 * #1: target
 * #2: bits number
 */
void F_SYMBOL(bn4352_lshift)(bn4352_t *t, uint32_t n)
{
	bn4352_t r;
	uint32_t *_t = t->n;
	uint32_t *_r = r.n;
	int32_t m, w, x;

	if (!n)
		return;

	F_SYMBOL(bn4352_set1)(&r, 0);
	r.sign = t->sign;

	m = n / 32;
	w = n % 32;
	if (w) {
		x = 32 - w;
		uint32_t carry = 0;
		for (int32_t i = m; i < BN4352_N; i++) {
			uint32_t tmp = _t[i - m] >> x;
			_r[i] = (_t[i - m] << w) | carry;
			carry = tmp;
		}
	} else {
		for (int32_t i = m; i < BN4352_N; i++)
			_r[i] = _t[i - m];
	}

	F_SYMBOL(bn4352_move)(t, &r);
}

/* @func: bn4352_rshift
 * #desc:
 *    big number right shift.
 *
 * #1: target
 * #2: bits number
 */
void F_SYMBOL(bn4352_rshift)(bn4352_t *t, uint32_t n)
{
	bn4352_t r;
	uint32_t *_t = t->n;
	uint32_t *_r = r.n;
	int32_t m, w, x;

	if (!n)
		return;

	F_SYMBOL(bn4352_set1)(&r, 0);
	r.sign = t->sign;

	m = n / 32;
	w = n % 32;
	if (w) {
		x = 32 - w;
		uint32_t u = (1U << w) - 1;
		uint32_t carry = 0;
		for (int32_t i = BN4352_N - 1; i >= m; i--) {
			uint32_t tmp = _t[i] & u;
			_r[i - m] = (_t[i] >> w) | (carry << x);
			carry = tmp;
		}
	} else {
		for (int32_t i = m; i < BN4352_N; i++)
			_r[i - m] = _t[i];
	}

	F_SYMBOL(bn4352_move)(t, &r);
}

/* @func: bn4352_and
 * #desc:
 *    big number and operation.
 *
 * #1: result
 * #2: number
 * #3: number
 */
void F_SYMBOL(bn4352_and)(bn4352_t *r,
		const bn4352_t *a, const bn4352_t *b)
{
	const uint32_t *_a = a->n;
	const uint32_t *_b = b->n;
	uint32_t *_r = r->n;

	for (int32_t i = 0; i < BN4352_N; i++)
		_r[i] = _a[i] & _b[i];

	r->sign = 0;
}

/* @func: bn4352_or
 * #desc:
 *    big number or operation.
 *
 * #1: result
 * #2: number
 * #3: number
 */
void F_SYMBOL(bn4352_or)(bn4352_t *r,
		const bn4352_t *a, const bn4352_t *b)
{
	const uint32_t *_a = a->n;
	const uint32_t *_b = b->n;
	uint32_t *_r = r->n;

	for (int32_t i = 0; i < BN4352_N; i++)
		_r[i] = _a[i] | _b[i];

	r->sign = 0;
}

/* @func: bn4352_xor
 * #desc:
 *    big number xor operation.
 *
 * #1: result
 * #2: number
 * #3: number
 */
void F_SYMBOL(bn4352_xor)(bn4352_t *r,
		const bn4352_t *a, const bn4352_t *b)
{
	const uint32_t *_a = a->n;
	const uint32_t *_b = b->n;
	uint32_t *_r = r->n;

	for (int32_t i = 0; i < BN4352_N; i++)
		_r[i] = _a[i] ^ _b[i];

	r->sign = 0;
}

/* @func: bn4352_not
 * #desc:
 *    big number not operation.
 *
 * #1: result
 * #2: number
 */
void F_SYMBOL(bn4352_not)(bn4352_t *r,
		const bn4352_t *a)
{
	const uint32_t *_a = a->n;
	uint32_t *_r = r->n;

	for (int32_t i = 0; i < BN4352_N; i++)
		_r[i] = ~_a[i];

	r->sign = 0;
}

/* @func: bn4352_ucmp
 * #desc:
 *    big number compare (unsigned).
 *
 * #1: number
 * #2: number
 * #r: 0: a==b, 1: a>b, -1: a<b
 */
int32_t F_SYMBOL(bn4352_ucmp)(const bn4352_t *a, const bn4352_t *b)
{
	const uint32_t *_a = a->n;
	const uint32_t *_b = b->n;

	for (int32_t i = BN4352_N - 1; i >= 0; i--) {
		if (_a[i] > _b[i])
			return 1;
		if (_a[i] < _b[i])
			return -1;
	}

	return 0;
}

/* @func: bn4352_ucmp_1
 * #desc:
 *    big number compare (unsigned, single a word).
 *
 * #1: number
 * #2: number
 * #r: 0: a==b, 1: a>b, -1: a<b
 */
int32_t F_SYMBOL(bn4352_ucmp_1)(const bn4352_t *a, uint32_t b)
{
	const uint32_t *_a = a->n;

	for (int32_t i = BN4352_N - 1; i >= 0; i--) {
		if (i) {
			if (_a[i])
				return 1;
		} else {
			if (_a[i] > b)
				return 1;
			if (_a[i] < b)
				return -1;
		}
	}

	return 0;
}

/* @func: bn4352_cmp
 * #desc:
 *    big number compare (signed).
 *
 * #1: number
 * #2: number
 * #r: 0: a==b, 1: a>b, -1: a<b
 */
int32_t F_SYMBOL(bn4352_cmp)(const bn4352_t *a, const bn4352_t *b)
{
	const uint32_t *_a = a->n;
	const uint32_t *_b = b->n;

	if (a->sign < b->sign)
		return 1;
	if (a->sign > b->sign)
		return -1;

	for (int32_t i = BN4352_N - 1; i >= 0; i--) {
		if (_a[i] < _b[i])
			return a->sign ? 1 : -1;
		if (_a[i] > _b[i])
			return a->sign ? -1 : 1;
	}

	return 0;
}

/* @func: bn4352_cmp_1
 * #desc:
 *    big number compare (signed, single a word).
 *
 * #1: number
 * #2: number
 * #r: 0: a==b, 1: a>b, -1: a<b
 */
int32_t F_SYMBOL(bn4352_cmp_1)(const bn4352_t *a, uint32_t b)
{
	const uint32_t *_a = a->n;

	if (a->sign)
		return -1;

	for (int32_t i = BN4352_N - 1; i >= 0; i--) {
		if (i) {
			if (_a[i])
				return 1;
		} else {
			if (_a[i] > b)
				return 1;
			if (_a[i] < b)
				return -1;
		}
	}

	return 0;
}

/* @func: bn4352_uadd
 * #desc:
 *    big number addition (unsigned).
 *
 * #1: sum
 * #2: addend
 * #3: addend
 * #r: overflow
 */
uint32_t F_SYMBOL(bn4352_uadd)(bn4352_t *r,
		const bn4352_t *a, const bn4352_t *b)
{
	const uint32_t *_a = a->n;
	const uint32_t *_b = b->n;
	uint32_t *_r = r->n;

	uint32_t carry = 0;
	for (int32_t i = 0; i < BN4352_N; i++) {
		uint64_t tmp = (uint64_t)_a[i] + _b[i] + carry;
		_r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	r->sign = 0;

	return carry;
}

/* @func: bn4352_uadd_1
 * #desc:
 *    big number addition (unsigned, single a word).
 *
 * #1: sum
 * #2: addend
 * #3: addend
 * #r: overflow
 */
uint32_t F_SYMBOL(bn4352_uadd_1)(bn4352_t *r,
		const bn4352_t *a, uint32_t b)
{
	const uint32_t *_a = a->n;
	uint32_t *_r = r->n;

	uint32_t carry = b;
	for (int32_t i = 0; i < BN4352_N; i++) {
		uint64_t tmp = (uint64_t)_a[i] + carry;
		_r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	r->sign = 0;

	return carry;
}

/* @func: bn4352_usub
 * #desc:
 *    big number subtraction (unsigned).
 *
 * #1: difference
 * #2: minuend
 * #3: subtract
 * #r: overflow
 */
uint32_t F_SYMBOL(bn4352_usub)(bn4352_t *r,
		const bn4352_t *a, const bn4352_t *b)
{
	const uint32_t *_a = a->n;
	const uint32_t *_b = b->n;
	uint32_t *_r = r->n;

	uint32_t borrow = 0;
	for (int32_t i = 0; i < BN4352_N; i++) {
		uint64_t tmp = (uint64_t)_a[i] - _b[i] - borrow;
		_r[i] = tmp & 0xffffffff;
		borrow = (tmp >> 32) ? 1 : 0;
	}

	r->sign = 0;

	return borrow;
}

/* @func: bn4352_usub_1
 * #desc:
 *    big number subtraction (unsigned, single a word).
 *
 * #1: difference
 * #2: minuend
 * #3: subtract
 * #r: overflow
 */
uint32_t F_SYMBOL(bn4352_usub_1)(bn4352_t *r,
		const bn4352_t *a, uint32_t b)
{
	const uint32_t *_a = a->n;
	uint32_t *_r = r->n;

	uint32_t borrow = b;
	for (int32_t i = 0; i < BN4352_N; i++) {
		uint64_t tmp = (uint64_t)_a[i] - borrow;
		_r[i] = tmp & 0xffffffff;
		borrow = (tmp >> 32) ? 1 : 0;
	}

	r->sign = 0;

	return borrow;
}

/* @func: _umul_karatsuba_2 (static)
 * #desc:
 *    big number karatsuba multiplication.
 *
 * #1: product
 * #2: multiplicand
 * #3: multiplier
 */
static void _umul_karatsuba_2(uint32_t *r,
		const uint32_t *a, const uint32_t *b)
{
#undef N0
#undef N1
#undef N2
#undef N3
#undef N4
#define N0 (BN4352_N / 2)
#define N1 (N0 / 2)
#define N2 (N1 / 2)
#define N3 (N2 + 1)
#define N4 (N3 * 2)

	uint32_t z0[N4], z1[N4], z2[N4], a1[N3], b1[N3];
	uint32_t carry, borrow;
	uint64_t tmp;

	C_SYMBOL(memset)(z0, 0, sizeof(z0));
	C_SYMBOL(memset)(z1, 0, sizeof(z1));
	C_SYMBOL(memset)(z2, 0, sizeof(z2));
	C_SYMBOL(memset)(a1, 0, sizeof(a1));
	C_SYMBOL(memset)(b1, 0, sizeof(b1));

	/* z0 = a(low) * b(low) */
	for (int32_t i = 0; i < N2; i++) {
		carry = 0;
		for (int32_t j = 0; j < N2; j++) {
			tmp = (uint64_t)a[i] * b[j] + z0[i + j] + carry;
			z0[i + j] = tmp & 0xffffffff;
			carry = tmp >> 32;
		}
		z0[i + N2] = carry;
	}

	/* a1 = a(low) + a(hig) */
	carry = 0;
	for (int32_t i = 0; i < N2; i++) {
		tmp = (uint64_t)a[i] + a[i + N2] + carry;
		a1[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
	a1[N2] = carry;

	/* b1 = b(low) + b(hig) */
	carry = 0;
	for (int32_t i = 0; i < N2; i++) {
		tmp = (uint64_t)b[i] + b[i + N2] + carry;
		b1[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
	b1[N2] = carry;

	/* z1 = a1 * b1 */
	for (int32_t i = 0; i < N3; i++) {
		carry = 0;
		for (int32_t j = 0; j < N3; j++) {
			tmp = (uint64_t)a1[i] * b1[j] + z1[i + j] + carry;
			z1[i + j] = tmp & 0xffffffff;
			carry = tmp >> 32;
		}
		z1[i + N3] = carry;
	}

	/* z2 = a(hig) * b(hig) */
	a += N2;
	b += N2;
	for (int32_t i = 0; i < N2; i++) {
		carry = 0;
		for (int32_t j = 0; j < N2; j++) {
			tmp = (uint64_t)a[i] * b[j] + z2[i + j] + carry;
			z2[i + j] = tmp & 0xffffffff;
			carry = tmp >> 32;
		}
		z2[i + N2] = carry;
	}

	/* z1 = z1 - z2 - z0 */
	borrow = 0;
	for (int32_t i = 0; i < N4; i++) {
		tmp = (uint64_t)z1[i] - z2[i] - borrow;
		z1[i] = tmp & 0xffffffff;
		borrow = (tmp >> 32) ? 1 : 0;
	}

	borrow = 0;
	for (int32_t i = 0; i < N4; i++) {
		tmp = (uint64_t)z1[i] - z0[i] - borrow;
		z1[i] = tmp & 0xffffffff;
		borrow = (tmp >> 32) ? 1 : 0;
	}

	/* r = (z2 << N1) + (z1 << N2) + z0 */
	C_SYMBOL(memset)(r, 0, sizeof(uint32_t) * N0);
	for (int32_t i = 0; i < N1; i++)
		r[i + N1] = z2[i];

	carry = 0;
	for (int32_t i = N2; i < (N2 + N4); i++) {
		tmp = (uint64_t)r[i] + z1[i - N2] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
	for (int32_t i = (N2 + N4); i < N0; i++) {
		tmp = (uint64_t)r[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	carry = 0;
	for (int32_t i = 0; i < N1; i++) {
		tmp = (uint64_t)r[i] + z0[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
	for (int32_t i = N1; i < N0; i++) {
		tmp = (uint64_t)r[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
}

/* @func: _umul_karatsuba_2_1 (static)
 * #desc:
 *    big number karatsuba multiplication.
 *
 * #1: product
 * #2: multiplicand
 * #3: multiplier
 */
static void _umul_karatsuba_2_1(uint32_t *r,
		const uint32_t *a, const uint32_t *b)
{
#undef N0
#undef N1
#undef N2
#undef N3
#undef N4
#define N0 (BN4352_N / 2 + 4)
#define N1 (N0 / 2)
#define N2 (N1 / 2)
#define N3 (N2 + 1)
#define N4 (N3 * 2)

	uint32_t z0[N4], z1[N4], z2[N4], a1[N3], b1[N3];
	uint32_t carry, borrow;
	uint64_t tmp;

	C_SYMBOL(memset)(z0, 0, sizeof(z0));
	C_SYMBOL(memset)(z1, 0, sizeof(z1));
	C_SYMBOL(memset)(z2, 0, sizeof(z2));
	C_SYMBOL(memset)(a1, 0, sizeof(a1));
	C_SYMBOL(memset)(b1, 0, sizeof(b1));

	/* z0 = a(low) * b(low) */
	for (int32_t i = 0; i < N2; i++) {
		carry = 0;
		for (int32_t j = 0; j < N2; j++) {
			tmp = (uint64_t)a[i] * b[j] + z0[i + j] + carry;
			z0[i + j] = tmp & 0xffffffff;
			carry = tmp >> 32;
		}
		z0[i + N2] = carry;
	}

	/* a1 = a(low) + a(hig) */
	carry = 0;
	for (int32_t i = 0; i < N2; i++) {
		tmp = (uint64_t)a[i] + a[i + N2] + carry;
		a1[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
	a1[N2] = carry;

	/* b1 = b(low) + b(hig) */
	carry = 0;
	for (int32_t i = 0; i < N2; i++) {
		tmp = (uint64_t)b[i] + b[i + N2] + carry;
		b1[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
	b1[N2] = carry;

	/* z1 = a1 * b1 */
	for (int32_t i = 0; i < N3; i++) {
		carry = 0;
		for (int32_t j = 0; j < N3; j++) {
			tmp = (uint64_t)a1[i] * b1[j] + z1[i + j] + carry;
			z1[i + j] = tmp & 0xffffffff;
			carry = tmp >> 32;
		}
		z1[i + N3] = carry;
	}

	/* z2 = a(hig) * b(hig) */
	a += N2;
	b += N2;
	for (int32_t i = 0; i < N2; i++) {
		carry = 0;
		for (int32_t j = 0; j < N2; j++) {
			tmp = (uint64_t)a[i] * b[j] + z2[i + j] + carry;
			z2[i + j] = tmp & 0xffffffff;
			carry = tmp >> 32;
		}
		z2[i + N2] = carry;
	}

	/* z1 = z1 - z2 - z0 */
	borrow = 0;
	for (int32_t i = 0; i < N4; i++) {
		tmp = (uint64_t)z1[i] - z2[i] - borrow;
		z1[i] = tmp & 0xffffffff;
		borrow = (tmp >> 32) ? 1 : 0;
	}

	borrow = 0;
	for (int32_t i = 0; i < N4; i++) {
		tmp = (uint64_t)z1[i] - z0[i] - borrow;
		z1[i] = tmp & 0xffffffff;
		borrow = (tmp >> 32) ? 1 : 0;
	}

	/* r = (z2 << N1) + (z1 << N2) + z0 */
	C_SYMBOL(memset)(r, 0, sizeof(uint32_t) * N0);
	for (int32_t i = 0; i < N1; i++)
		r[i + N1] = z2[i];

	carry = 0;
	for (int32_t i = N2; i < (N2 + N4); i++) {
		tmp = (uint64_t)r[i] + z1[i - N2] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
	for (int32_t i = (N2 + N4); i < N0; i++) {
		tmp = (uint64_t)r[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	carry = 0;
	for (int32_t i = 0; i < N1; i++) {
		tmp = (uint64_t)r[i] + z0[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
	for (int32_t i = N1; i < N0; i++) {
		tmp = (uint64_t)r[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
}

/* @func: _umul_karatsuba (static)
 * #desc:
 *    big number karatsuba multiplication.
 *
 * #1: product
 * #2: multiplicand
 * #3: multiplier
 */
static void _umul_karatsuba(uint32_t *r,
		const uint32_t *a, const uint32_t *b)
{
#undef N0
#undef N1
#undef N2
#undef N3
#undef N4
#define N0 BN4352_N
#define N1 (N0 / 2)
#define N2 (N1 / 2)
#define N3 (N2 + 2)
#define N4 (N3 * 2)

	uint32_t z0[N4], z1[N4], z2[N4], a1[N3], b1[N3];
	uint32_t carry, borrow;
	uint64_t tmp;

	C_SYMBOL(memset)(z0, 0, sizeof(z0));
	C_SYMBOL(memset)(z1, 0, sizeof(z1));
	C_SYMBOL(memset)(z2, 0, sizeof(z2));
	C_SYMBOL(memset)(a1, 0, sizeof(a1));
	C_SYMBOL(memset)(b1, 0, sizeof(b1));

	/* z0 = a(low) * b(low) */
	_umul_karatsuba_2(z0, a, b);

	/* a1 = a(low) + a(hig) */
	carry = 0;
	for (int32_t i = 0; i < N2; i++) {
		tmp = (uint64_t)a[i] + a[i + N2] + carry;
		a1[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
	a1[N2] = carry;

	/* b1 = b(low) + b(hig) */
	carry = 0;
	for (int32_t i = 0; i < N2; i++) {
		tmp = (uint64_t)b[i] + b[i + N2] + carry;
		b1[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
	b1[N2] = carry;

	/* z1 = a1 * b1 */
	_umul_karatsuba_2_1(z1, a1, b1);

	/* z2 = a(hig) * b(hig) */
	a += N2;
	b += N2;
	_umul_karatsuba_2(z2, a, b);

	/* z1 = z1 - z2 - z0 */
	borrow = 0;
	for (int32_t i = 0; i < N4; i++) {
		tmp = (uint64_t)z1[i] - z2[i] - borrow;
		z1[i] = tmp & 0xffffffff;
		borrow = (tmp >> 32) ? 1 : 0;
	}

	borrow = 0;
	for (int32_t i = 0; i < N4; i++) {
		tmp = (uint64_t)z1[i] - z0[i] - borrow;
		z1[i] = tmp & 0xffffffff;
		borrow = (tmp >> 32) ? 1 : 0;
	}

	/* r = (z2 << N1) + (z1 << N2) + z0 */
	C_SYMBOL(memset)(r, 0, sizeof(uint32_t) * N0);
	for (int32_t i = 0; i < N1; i++)
		r[i + N1] = z2[i];

	carry = 0;
	for (int32_t i = N2; i < (N2 + N4); i++) {
		tmp = (uint64_t)r[i] + z1[i - N2] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
	for (int32_t i = (N2 + N4); i < N0; i++) {
		tmp = (uint64_t)r[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	carry = 0;
	for (int32_t i = 0; i < N1; i++) {
		tmp = (uint64_t)r[i] + z0[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
	for (int32_t i = N1; i < N0; i++) {
		tmp = (uint64_t)r[i] + carry;
		r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}
}

/* @func: bn4352_umul
 * #desc:
 *    big number multiplication (unsigned).
 *
 * #1: product
 * #2: multiplicand
 * #3: multiplier
 */
void F_SYMBOL(bn4352_umul)(bn4352_t *r,
		const bn4352_t *a, const bn4352_t *b)
{
	bn4352_t rr;
	const uint32_t *_a = a->n;
	const uint32_t *_b = b->n;
	uint32_t *_rr = rr.n;

	F_SYMBOL(bn4352_set1)(&rr, 0);

	int32_t a_len = F_SYMBOL(bn4352_bits)(a);
	int32_t b_len = F_SYMBOL(bn4352_bits)(b);
	if (!a_len || !b_len)
		goto e;

	a_len = a_len > b_len ? a_len : b_len;
	if (a_len <= (BN4352_NB / 4)) {
		_umul_karatsuba_2(_rr, _a, _b);
		goto e;
	} else if (a_len <= (BN4352_NB / 2)) {
		_umul_karatsuba(_rr, _a, _b);
		goto e;
	}

	uint32_t carry = 0;
	for (int32_t i = 0; i < BN4352_N; i++) {
		carry = 0;
		for (int32_t j = 0; j < BN4352_N; j++) {
			if ((i + j) >= BN4352_N)
				break;
			uint64_t tmp = (uint64_t)_a[i] * _b[j]
				+ _rr[i + j] + carry;
			_rr[i + j] = tmp & 0xffffffff;
			carry = tmp >> 32;
		}
	}

e:
	F_SYMBOL(bn4352_move)(r, &rr);
}

/* @func: bn4352_umul_1
 * #desc:
 *    big number multiplication (unsigned, single a word).
 *
 * #1: product
 * #2: multiplicand
 * #3: multiplier
 * #r: overflow
 */
uint32_t F_SYMBOL(bn4352_umul_1)(bn4352_t *r,
		const bn4352_t *a, uint32_t b)
{
	const uint32_t *_a = a->n;
	uint32_t *_r = r->n;

	uint32_t carry = 0;
	for (int32_t i = 0; i < BN4352_N; i++) {
		uint64_t tmp = (uint64_t)_a[i] * b + carry;
		_r[i] = tmp & 0xffffffff;
		carry = tmp >> 32;
	}

	r->sign = 0;

	return carry;
}

/* @func: _udiv_ucmp (static)
 * #desc:
 *    division of the big number compare.
 *
 * #1: number
 * #2: number
 * #r: 0: a==b, 1: a>b, -1: a<b
 */
static int32_t _udiv_ucmp(const uint32_t *a, const uint32_t *b)
{
#undef N0
#undef N1
#define N0 BN4352_N
#define N1 (N0 + 1)

	for (int32_t i = N1 - 1; i >= 0; i--) {
		if (a[i] > b[i])
			return 1;
		if (a[i] < b[i])
			return -1;
	}

	return 0;
}

/* @func: _udiv_lshift (static)
 * #desc:
 *    division of the big number left shift.
 *
 * #1: target
 * #2: bits number
 */
static void _udiv_lshift(uint32_t *t, uint32_t n)
{
#undef N0
#undef N1
#define N0 BN4352_N
#define N1 (N0 + 1)

	uint32_t r[N1];
	int32_t m, w, x;

	if (!n)
		return;

	C_SYMBOL(memset)(r, 0, sizeof(r));

	m = n / 32;
	w = n % 32;
	if (w) {
		x = 32 - w;
		uint32_t carry = 0;
		for (int32_t i = m; i < N1; i++) {
			uint32_t tmp = t[i - m] >> x;
			r[i] = (t[i - m] << w) | carry;
			carry = tmp;
		}
	} else {
		for (int32_t i = m; i < N1; i++)
			r[i] = t[i - m];
	}

	C_SYMBOL(memcpy)(t, r, sizeof(r));
}

/* @func: _udiv_rshift (static)
 * #desc:
 *    division of the big number right shift.
 *
 * #1: target
 * #2: bits number
 */
static void _udiv_rshift(uint32_t *t, uint32_t n)
{
#undef N0
#undef N1
#define N0 BN4352_N
#define N1 (N0 + 1)

	uint32_t r[N1];
	int32_t m, w, x;

	if (!n)
		return;

	C_SYMBOL(memset)(r, 0, sizeof(r));

	m = n / 32;
	w = n % 32;
	if (w) {
		x = 32 - w;
		uint32_t u = (1U << w) - 1;
		uint32_t carry = 0;
		for (int32_t i = N1 - 1; i >= m; i--) {
			uint32_t tmp = t[i] & u;
			r[i - m] = (t[i] >> w) | (carry << x);
			carry = tmp;
		}
	} else {
		for (int32_t i = m; i < N1; i++)
			r[i - m] = t[i];
	}

	C_SYMBOL(memcpy)(t, r, sizeof(r));
}

/* @func: _udiv_usub (static)
 * #desc:
 *    division of the big number subtraction.
 *
 * #1: difference
 * #2: minuend
 * #3: subtract
 */
static void _udiv_usub(uint32_t *r,
		const uint32_t *a, const uint32_t *b)
{
#undef N0
#undef N1
#define N0 BN4352_N
#define N1 (N0 + 1)

	uint32_t borrow = 0;
	for (int32_t i = 0; i < N1; i++) {
		uint64_t tmp = (uint64_t)a[i] - b[i] - borrow;
		r[i] = tmp & 0xffffffff;
		borrow = (tmp >> 32) ? 1 : 0;
	}
}

/* @func: _udiv_base (static)
 * #desc:
 *    big number division.
 *
 * #1: quotient
 * #2: remainder
 * #3: dividend
 * #4: divisor
 * #5: dividend length
 * #6: divisor length
 */
static void _udiv_base(uint32_t *quo, uint32_t *rem,
		const uint32_t *a, const uint32_t *b,
		int32_t a_len, int32_t b_len)
{
#undef N0
#undef N1
#define N0 BN4352_N
#define N1 (N0 + 1)

	uint32_t rquo[N1], rrem[N1], sh_b[N1];
	int32_t sh = a_len - b_len;

	C_SYMBOL(memcpy)(rrem, a, sizeof(uint32_t) * N0);
	C_SYMBOL(memcpy)(sh_b, b, sizeof(uint32_t) * N0);
	rrem[N0] = sh_b[N0] = 0;

	_udiv_lshift(sh_b, sh);
	while (_udiv_ucmp(rrem, sh_b) >= 0) {
		_udiv_lshift(sh_b, 1);
		sh++;
	}

	C_SYMBOL(memset)(rquo, 0, sizeof(rquo));
	for (; sh > 0; sh--) {
		_udiv_rshift(sh_b, 1);
		_udiv_lshift(rquo, 1);
		if (_udiv_ucmp(rrem, sh_b) >= 0) {
			_udiv_usub(rrem, rrem, sh_b);
			rquo[0] |= 1;
		}
	}

	C_SYMBOL(memcpy)(rem, rrem, sizeof(uint32_t) * N0);
	C_SYMBOL(memcpy)(quo, rquo, sizeof(uint32_t) * N0);
}

/* @func: bn4352_udiv
 * #desc:
 *    big number division (unsigned).
 *
 * #1: quotient
 * #2: remainder
 * #3: dividend
 * #4: divisor
 */
void F_SYMBOL(bn4352_udiv)(bn4352_t *quo, bn4352_t *rem,
		const bn4352_t *a, const bn4352_t *b)
{
	bn4352_t rquo, rrem;
	F_SYMBOL(bn4352_move)(&rrem, a);
	F_SYMBOL(bn4352_move)(&rquo, b);

	if (rem)
		F_SYMBOL(bn4352_move)(rem, &rrem);
	if (quo)
		F_SYMBOL(bn4352_set1)(quo, 0);

	if (!F_SYMBOL(bn4352_ucmp_1)(&rquo, 0)) {
		if (rem)
			F_SYMBOL(bn4352_set1)(rem, 0);
		return;
	}
	if (F_SYMBOL(bn4352_ucmp)(&rrem, &rquo) < 0)
		return;

	int32_t a_len = F_SYMBOL(bn4352_bits)(&rrem);
	int32_t b_len = F_SYMBOL(bn4352_bits)(&rquo);
	if (!a_len || !b_len)
		return;

	_udiv_base(rquo.n, rrem.n, rrem.n, rquo.n, a_len, b_len);
	rrem.sign = rquo.sign = 0;

	if (quo)
		F_SYMBOL(bn4352_move)(quo, &rquo);
	if (rem)
		F_SYMBOL(bn4352_move)(rem, &rrem);
}

/* @func: bn4352_udiv_1
 * #desc:
 *    big number division (unsigned, single a word).
 *
 * #1: quotient
 * #2: remainder
 * #3: dividend
 * #4: divisor
 */
void F_SYMBOL(bn4352_udiv_1)(bn4352_t *quo, bn4352_t *rem,
		const bn4352_t *a, uint32_t b)
{
	bn4352_t t;
	F_SYMBOL(bn4352_set1)(&t, b);

	F_SYMBOL(bn4352_udiv)(quo, rem, a, &t);
}

/* @func: bn4352_add
 * #desc:
 *    big number addition (signed).
 *
 * #1: sum
 * #2: addend
 * #3: addend
 */
void F_SYMBOL(bn4352_add)(bn4352_t *r,
		const bn4352_t *a, const bn4352_t *b)
{
	uint32_t sign = a->sign;

	if (sign == b->sign) {
		F_SYMBOL(bn4352_uadd)(r, a, b);
		r->sign = sign;
		return;
	}

	int32_t k = F_SYMBOL(bn4352_ucmp)(a, b);
	if (k > 0) {
		F_SYMBOL(bn4352_usub)(r, a, b);
		r->sign = sign;
	} else if (k < 0) {
		sign = b->sign;
		F_SYMBOL(bn4352_usub)(r, b, a);
		r->sign = sign;
	} else {
		F_SYMBOL(bn4352_set1)(r, 0);
	}
}

/* @func: bn4352_add_1
 * #desc:
 *    big number addition (signed, single a word).
 *
 * #1: sum
 * #2: addend
 * #3: addend
 */
void F_SYMBOL(bn4352_add_1)(bn4352_t *r,
		const bn4352_t *a, uint32_t b)
{
	bn4352_t t;
	F_SYMBOL(bn4352_set1)(&t, b);

	F_SYMBOL(bn4352_add)(r, a, &t);
}

/* @func: bn4352_sub
 * #desc:
 *    big number subtraction (signed).
 *
 * #1: difference
 * #2: minuend
 * #3: subtract
 */
void F_SYMBOL(bn4352_sub)(bn4352_t *r,
		const bn4352_t *a, const bn4352_t *b)
{
	bn4352_t t;
	F_SYMBOL(bn4352_move)(&t, b);
	t.sign = !b->sign;

	F_SYMBOL(bn4352_add)(r, a, &t);
}

/* @func: bn4352_sub_1
 * #desc:
 *    big number subtraction (signed, single a word).
 *
 * #1: difference
 * #2: minuend
 * #3: subtract
 */
void F_SYMBOL(bn4352_sub_1)(bn4352_t *r,
		const bn4352_t *a, uint32_t b)
{
	bn4352_t t;
	F_SYMBOL(bn4352_set1)(&t, b);
	t.sign = 1;

	F_SYMBOL(bn4352_add)(r, a, &t);
}

/* @func: bn4352_mul
 * #desc:
 *    big number multiplication (signed).
 *
 * #1: product
 * #2: multiplicand
 * #3: multiplier
 */
void F_SYMBOL(bn4352_mul)(bn4352_t *r,
		const bn4352_t *a, const bn4352_t *b)
{
	uint32_t sign = a->sign ^ b->sign;

	F_SYMBOL(bn4352_umul)(r, a, b);

	r->sign = F_SYMBOL(bn4352_bits)(r) ? sign : 0;
}

/* @func: bn4352_mul_1
 * #desc:
 *    big number multiplication (single a word).
 *
 * #1: product
 * #2: multiplicand
 * #3: multiplier
 */
void F_SYMBOL(bn4352_mul_1)(bn4352_t *r,
		const bn4352_t *a, uint32_t b)
{
	uint32_t sign = a->sign ^ 0;

	F_SYMBOL(bn4352_umul_1)(r, a, b);

	r->sign = F_SYMBOL(bn4352_bits)(r) ? sign : 0;
}

/* @func: bn4352_div
 * #desc:
 *    big number division (signed).
 *
 * #1: quotient
 * #2: remainder
 * #3: dividend
 * #4: divisor
 */
void F_SYMBOL(bn4352_div)(bn4352_t *quo, bn4352_t *rem,
		const bn4352_t *a, const bn4352_t *b)
{
	uint32_t a_sign = a->sign, b_sign = b->sign;

	F_SYMBOL(bn4352_udiv)(quo, rem, a, b);

	if (rem)
		rem->sign = F_SYMBOL(bn4352_bits)(rem) ? a_sign : 0;
	if (quo) {
		a_sign ^= b_sign;
		quo->sign = F_SYMBOL(bn4352_bits)(quo) ? a_sign : 0;
	}
}

/* @func: bn4352_div_1
 * #desc:
 *    big number division (single a word).
 *
 * #1: quotient
 * #2: remainder
 * #3: dividend
 * #4: divisor
 */
void F_SYMBOL(bn4352_div_1)(bn4352_t *quo, bn4352_t *rem,
		const bn4352_t *a, uint32_t b)
{
	uint32_t a_sign = a->sign, b_sign = 0;

	F_SYMBOL(bn4352_udiv_1)(quo, rem, a, b);

	if (rem)
		rem->sign = F_SYMBOL(bn4352_bits)(rem) ? a_sign : 0;
	if (quo) {
		a_sign ^= b_sign;
		quo->sign = F_SYMBOL(bn4352_bits)(quo) ? a_sign : 0;
	}
}

/* @func: bn4352_divmod
 * #desc:
 *    big number division modular (signed).
 *
 * #1: quotient
 * #2: remainder
 * #3: dividend
 * #4: divisor
 */
void F_SYMBOL(bn4352_divmod)(bn4352_t *quo, bn4352_t *rem,
		const bn4352_t *a, const bn4352_t *b)
{
	bn4352_t t;
	F_SYMBOL(bn4352_move)(&t, b);

	F_SYMBOL(bn4352_div)(quo, rem, a, b);

	if (rem && rem->sign) {
		F_SYMBOL(bn4352_add)(rem, rem, &t);
		rem->sign = 0;
	}
}

/* @func: bn4352_divmod_1
 * #desc:
 *    big number division modular (signed, single a word).
 *
 * #1: quotient
 * #2: remainder
 * #3: dividend
 * #4: divisor
 */
void F_SYMBOL(bn4352_divmod_1)(bn4352_t *quo, bn4352_t *rem,
		const bn4352_t *a, uint32_t b)
{
	F_SYMBOL(bn4352_div_1)(quo, rem, a, b);

	if (rem && rem->sign) {
		F_SYMBOL(bn4352_add_1)(rem, rem, b);
		rem->sign = 0;
	}
}

/* @func: bn4352_modpow
 * #desc:
 *    big number fast modular exponentiation.
 *
 * #1: result
 * #2: number
 * #3: exponential
 * #4: modules
 */
void F_SYMBOL(bn4352_modpow)(bn4352_t *r,
		const bn4352_t *a, const bn4352_t *e, const bn4352_t *m)
{
	bn4352_t rr, a1;
	F_SYMBOL(bn4352_set1)(&rr, 1);

	/* a1 = a % m */
	F_SYMBOL(bn4352_divmod)(NULL, &a1, a, m);

	int32_t n = F_SYMBOL(bn4352_bits)(e);
	for (int32_t i = 0; i < n; i++) {
		uint32_t k_i = e->n[i / 32] >> (i % 32);
		if (k_i & 1) {
			/* rr = (rr * a1) % m */
			F_SYMBOL(bn4352_mul)(&rr, &rr, &a1);
			F_SYMBOL(bn4352_divmod)(NULL, &rr, &rr, m);
		}

		/* a1 = (a1 * a1) % m */
		F_SYMBOL(bn4352_mul)(&a1, &a1, &a1);
		F_SYMBOL(bn4352_divmod)(NULL, &a1, &a1, m);
	}

	F_SYMBOL(bn4352_move)(r, &rr);
}

/* @func: bn4352_gcd
 * #desc:
 *    big number extended euclidean algorithm (non-recursive).
 *
 * #1: greatest common divisor
 * #2: bezout coefficient
 * #3: bezout coefficient
 * #4: number
 * #5: number
 */
void F_SYMBOL(bn4352_gcd)(bn4352_t *r, bn4352_t *x, bn4352_t *y,
		const bn4352_t *a, const bn4352_t *b)
{
	bn4352_t a1, b1, s1, s2, t1, t2, t3, t4;
	F_SYMBOL(bn4352_move)(&a1, a);
	F_SYMBOL(bn4352_move)(&b1, b);
	F_SYMBOL(bn4352_set1)(&s1, 1);
	F_SYMBOL(bn4352_set1)(&s2, 0);
	F_SYMBOL(bn4352_set1)(&t1, 0);
	F_SYMBOL(bn4352_set1)(&t2, 1);

	while (F_SYMBOL(bn4352_bits)(&b1)) {
		/* t3 = a1 % b1 */
		F_SYMBOL(bn4352_divmod)(&t3, NULL, &a1, &b1);

		/* (a1, b1) = (b1, a1 - (b1 * t3)) */
		F_SYMBOL(bn4352_mul)(&t4, &b1, &t3);
		F_SYMBOL(bn4352_sub)(&t4, &a1, &t4);
		F_SYMBOL(bn4352_move)(&a1, &b1);
		F_SYMBOL(bn4352_move)(&b1, &t4);

		/* (s1, s2) = (s2, s1 - (s2 * t3)) */
		F_SYMBOL(bn4352_mul)(&t4, &s2, &t3);
		F_SYMBOL(bn4352_sub)(&t4, &s1, &t4);
		F_SYMBOL(bn4352_move)(&s1, &s2);
		F_SYMBOL(bn4352_move)(&s2, &t4);

		/* (t1, t2) = (t2, t1 - (t2 * t3)) */
		F_SYMBOL(bn4352_mul)(&t4, &t2, &t3);
		F_SYMBOL(bn4352_sub)(&t4, &t1, &t4);
		F_SYMBOL(bn4352_move)(&t1, &t2);
		F_SYMBOL(bn4352_move)(&t2, &t4);
	}

	F_SYMBOL(bn4352_move)(r, &a1);
	F_SYMBOL(bn4352_move)(x, &s1);
	F_SYMBOL(bn4352_move)(y, &t1);
}

/* @func: bn4352_inv
 * #desc:
 *    big number modular inverse.
 *
 * #1: inverse modulus
 * #2: number
 * #3: modulus
 * #r: 0: success, -1: fail
 */
int32_t F_SYMBOL(bn4352_inv)(bn4352_t *r,
		const bn4352_t *a, const bn4352_t *b)
{
	bn4352_t g, x, y;

	F_SYMBOL(bn4352_gcd)(&g, &x, &y, a, b);
	if (F_SYMBOL(bn4352_cmp_1)(&g, 1)) {
		F_SYMBOL(bn4352_set1)(r, 0);
		return -1;
	}

	/* r = ((x % b) + b) % b */
	F_SYMBOL(bn4352_divmod)(NULL, &x, &x, b);
	F_SYMBOL(bn4352_add)(&x, &x, b);
	F_SYMBOL(bn4352_divmod)(NULL, r, &x, b);

	return 0;
}

/* @func: bn4352_redc_init
 * #desc:
 *    big number redc montgomery reduction initialization.
 *
 * #1: redc context
 * #2: modules
 * #r: 0: success, -1: fail
 */
int32_t F_SYMBOL(bn4352_redc_init)(bn4352_redc_t *s, const bn4352_t *n)
{
	s->k = F_SYMBOL(bn4352_bits)(n);
	F_SYMBOL(bn4352_move)(&s->n, n);

	/* R = 1 << k */
	F_SYMBOL(bn4352_set1)(&s->R, 1);
	F_SYMBOL(bn4352_lshift)(&s->R, s->k);

	/* n_prime = -inv(n, R) */
	if (F_SYMBOL(bn4352_inv)(&s->np, n, &s->R))
		return -1;
	s->np.sign = 1;

	/* n_prime = n_prime % R */
	F_SYMBOL(bn4352_divmod)(NULL, &s->np, &s->np, &s->R);

	/* mask = R - 1 */
	F_SYMBOL(bn4352_sub_1)(&s->mask, &s->R, 1);

	/* R = (R * R) % n */
	F_SYMBOL(bn4352_mul)(&s->R, &s->R, &s->R);
	F_SYMBOL(bn4352_divmod)(NULL, &s->R, &s->R, n);

	return 0;
}

/* @func: bn4352_redc_mod
 * #desc:
 *    big number redc montgomery reduction (norm).
 *
 * #1: redc context
 * #2: result
 * #3: number
 */
void F_SYMBOL(bn4352_redc_mod)(const bn4352_redc_t *s,
		bn4352_t *r, const bn4352_t *a)
{
	bn4352_t t;

	/* t = ((a & mask) * n_prime) & mask */
	F_SYMBOL(bn4352_and)(&t, a, &s->mask);
	F_SYMBOL(bn4352_mul)(&t, &t, &s->np);
	F_SYMBOL(bn4352_and)(&t, &t, &s->mask);

	/* t = (a + (t * n)) >> k */
	F_SYMBOL(bn4352_mul)(&t, &t, &s->n);
	F_SYMBOL(bn4352_add)(&t, &t, a);
	F_SYMBOL(bn4352_rshift)(&t, s->k);

	/* t = if t >= n : adjust */
	if (F_SYMBOL(bn4352_cmp)(&t, &s->n) >= 0)
		F_SYMBOL(bn4352_sub)(&t, &t, &s->n);

	F_SYMBOL(bn4352_move)(r, &t);
}

/* @func: bn4352_redc_mont
 * #desc:
 *    big number redc montgomery domain.
 *
 * #1: redc context
 * #2: result
 * #3: number
 */
void F_SYMBOL(bn4352_redc_mont)(const bn4352_redc_t *s,
		bn4352_t *r, const bn4352_t *a)
{
	bn4352_t t;

	/* r = redc(a * R) */
	F_SYMBOL(bn4352_mul)(&t, a, &s->R);
	F_SYMBOL(bn4352_redc_mod)(s, r, &t);
}

/* @func: bn4352_redc_pow
 * #desc:
 *    big number redc montgomery modular exponentiation.
 *
 * #1: result
 * #2: number
 * #3: exponential
 * #4: modules
 * #r: 0: success, -1: fail
 */
int32_t F_SYMBOL(bn4352_redc_pow)(bn4352_t *r,
		const bn4352_t *a, const bn4352_t *e, const bn4352_t *m)
{
	bn4352_redc_t s;
	bn4352_t rr, a1;

	if (F_SYMBOL(bn4352_redc_init)(&s, m))
		return -1;

	F_SYMBOL(bn4352_set1)(&rr, 1);
	F_SYMBOL(bn4352_redc_mont)(&s, &rr, &rr);
	F_SYMBOL(bn4352_redc_mont)(&s, &a1, a);

	int32_t n = F_SYMBOL(bn4352_bits)(e);
	for (int32_t i = 0; i < n; i++) {
		uint32_t k_i = e->n[i / 32] >> (i % 32);
		if (k_i & 1) {
			/* rr = (rr * a1) % m */
			F_SYMBOL(bn4352_mul)(&rr, &rr, &a1);
			F_SYMBOL(bn4352_redc_mod)(&s, &rr, &rr);
		}

		/* a1 = (a1 * a1) % m */
		F_SYMBOL(bn4352_mul)(&a1, &a1, &a1);
		F_SYMBOL(bn4352_redc_mod)(&s, &a1, &a1);
	}

	/* r = rr % m */
	F_SYMBOL(bn4352_redc_mod)(&s, r, &rr);

	return 0;
}

/* @func: bn4352_str2num
 * #desc:
 *    string to big number conversion.
 *
 * #1: result
 * #2: string pointer
 * #3: end pointer / NULL
 * #4: base type
 */
void F_SYMBOL(bn4352_str2num)(bn4352_t *t,
		const char *s, char **e, int32_t b)
{
	for (; *s == ' '; s++);

	int32_t neg = 0;
	if (*s == '-' || *s == '+')
		neg = (*s++ == '-') ? 1 : 0;

	if (b) {
		if (*s == '0') {
			s++;
			if (*s == 'B' || *s == 'b') {
				s++;
			} else if (*s == 'X' || *s == 'x') {
				s++;
			}
		}
	} else if (*s == '0') { /* b == 0 */
		s++;
		if (*s == 'B' || *s == 'b') {
			b = 2;
			s++;
		} else if (*s == 'X' || *s == 'x') {
			b = 16;
			s++;
		} else if (*s >= '0' && *s <= '7') {
			b = 8;
		} else {
			b = 10;
		}
	} else { /* default */
		b = 10;
	}

	for (; *s == '0'; s++);

	F_SYMBOL(bn4352_set1)(t, 0);
	for (int32_t d = 0; *s != '\0'; s++) {
		if (*s >= '0' && *s <= '9') {
			d = (*s - '0');
		} else if (*s >= 'A' && *s <= 'Z') {
			d = (*s - 'A') + 10;
		} else if (*s >= 'a' && *s <= 'z') {
			d = (*s - 'a') + 10;
		} else {
			break;
		}
		if (d >= b)
			break;

		F_SYMBOL(bn4352_umul_1)(t, t, b);
		F_SYMBOL(bn4352_uadd_1)(t, t, d);
	}

	if (e) /* end position */
		*e = (char *)s;

	t->sign = neg;
}

/* @func: _out_pad (static)
 * #desc:
 *    padding character to buffer.
 *
 * #1: buffer offset
 * #2: buffer
 * #3: character
 * #4: padding length
 * #r: padding length (+offset)
 */
static int32_t _out_pad(int32_t n, char *p, char c, int32_t x)
{
	while (x-- > 0)
		p[n++] = c;

	return n;
}

/* @func: _int2str_d (static)
 * #desc:
 *    unsigned number to decimal conversion.
 *
 * #1: buffer offset
 * #2: buffer
 * #3: input number
 * #r: output length (+offset)
 */
static int32_t _int2str_d(int32_t n, char *p, uint64_t v)
{
	int32_t t = n;

	do {
		p[n++] = (v % 10) + '0';
		v /= 10;
	} while (v);

	int32_t k = n;
	while (k > t) {
		char w = p[--k];
		p[k] = p[t];
		p[t++] = w;
	}

	return n;
}

/* @func: bn4352_num2str
 * #desc:
 *    big number to string conversion.
 *
 * #1: number
 * #2: output buffer
 */
void F_SYMBOL(bn4352_num2str)(const bn4352_t *a, char *p)
{
	bn4352_t _a, r, b;
	F_SYMBOL(bn4352_move)(&_a, a);

	if (a->sign)
		*p++ = '-';

	char buf[24];
	int32_t k = 0, n = 0, t = 0;
	uint64_t tmp = 10000000000000000000ULL;

	F_SYMBOL(bn4352_set1)(&b, (uint32_t)tmp);
	b.n[1] = tmp >> 32;

	do {
		F_SYMBOL(bn4352_udiv)(&_a, &r, &_a, &b);
		tmp = (uint64_t)r.n[0] | (uint64_t)r.n[1] << 32;
		k = _int2str_d(0, buf, tmp);
		n = _out_pad(n, p, '0', 19 - k);
		n = _int2str_d(n, p, tmp);
	} while (F_SYMBOL(bn4352_cmp_1)(&_a, 0));

	k = n;
	while (k > t) {
		k -= 19;
		C_SYMBOL(memcpy)(buf, p + k, 19);
		C_SYMBOL(memcpy)(p + k, p + t, 19);
		C_SYMBOL(memcpy)(p + t, buf, 19);
		t += 19;
	}

	for (k = 0; n > 1 && p[k] == '0'; k++, n--);
	C_SYMBOL(memcpy)(p, p + k, n);
	p[n] = '\0';
}
