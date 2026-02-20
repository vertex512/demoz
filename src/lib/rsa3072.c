/* @file: rsa3072.c
 * #desc:
 *    The implementations of rsa (rivest shamir adleman) cryptosystem.
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
#include <demoz/lib/bn.h>
#include <demoz/lib/rsa.h>


/* @func: rsa3072_genkey
 * #desc:
 *    generate rsa encryption and decryption key.
 *
 * #1: p [in]  prime P
 * #2: q [in]  prime Q
 * #3: e [out] encryption key
 * #4: d [out] decryption key
 * #5: n [out] modulus
 * #r:   [ret] 0: success, -1: fail
 */
int32_t F_SYMBOL(rsa3072_genkey)(const uint8_t *p, const uint8_t *q,
		uint8_t *e, uint8_t *d, uint8_t *n)
{
	bn6400_t _p, _q, a, b;

	F_SYMBOL(bn6400_from_bytes)(&_p, p, RSA3072_PRIME_LEN);
	F_SYMBOL(bn6400_from_bytes)(&_q, q, RSA3072_PRIME_LEN);

	if (!F_SYMBOL(bn6400_cmp)(&_p, &_q))
		return -1;

	/* a = (p - 1) * (q - 1) */
	F_SYMBOL(bn6400_sub_1)(&a, &_p, 1);
	F_SYMBOL(bn6400_sub_1)(&b, &_q, 1);
	F_SYMBOL(bn6400_mul)(&a, &a, &b);

	/* e = 65537 */
	F_SYMBOL(bn6400_set1)(&b, 65537);

	/* d = inv(e, a) */
	if (F_SYMBOL(bn6400_inv)(&a, &b, &a))
		return -1;

	F_SYMBOL(bn6400_to_bytes)(&b, e, RSA3072_LEN);
	F_SYMBOL(bn6400_to_bytes)(&a, d, RSA3072_LEN);

	/* n = p * q */
	F_SYMBOL(bn6400_mul)(&a, &_p, &_q);
	F_SYMBOL(bn6400_to_bytes)(&a, n, RSA3072_LEN);

	return 0;
}

/* @func: rsa3072_encrypt
 * #desc:
 *    rsa encryption function.
 *
 * #1: e   [in]     encryption key
 * #2: n   [in]     modulus
 * #3: msg [in/out] message and cipher
 * #r:     [ret]    0: success, -1: fail
 */
int32_t F_SYMBOL(rsa3072_encrypt)(const uint8_t *e, const uint8_t *n,
		uint8_t *msg)
{
	bn6400_t _e, _n, _m;

	F_SYMBOL(bn6400_from_bytes)(&_n, n, RSA3072_LEN);
	F_SYMBOL(bn6400_from_bytes)(&_m, msg, RSA3072_LEN);

	if (F_SYMBOL(bn6400_cmp)(&_m, &_n) > 0)
		return -1;

	F_SYMBOL(bn6400_from_bytes)(&_e, e, RSA3072_LEN);

	/* m = pow(m, e, n) */
	if (F_SYMBOL(bn6400_redc_pow)(&_m, &_m, &_e, &_n))
		return -1;

	F_SYMBOL(bn6400_to_bytes)(&_m, msg, RSA3072_LEN);

	return 0;
}

/* @func: rsa3072_decrypt
 * #desc:
 *    rsa decryption function.
 *
 * #1: d   [in]     decryption key
 * #2: n   [in]     modulus
 * #3: msg [in/out] message and cipher
 * #r:     [ret]    0: success, -1: fail
 */
int32_t F_SYMBOL(rsa3072_decrypt)(const uint8_t *d, const uint8_t *n,
		uint8_t *msg)
{
	bn6400_t _d, _n, _m;

	F_SYMBOL(bn6400_from_bytes)(&_n, n, RSA3072_LEN);
	F_SYMBOL(bn6400_from_bytes)(&_m, msg, RSA3072_LEN);

	if (F_SYMBOL(bn6400_cmp)(&_m, &_n) > 0)
		return -1;

	F_SYMBOL(bn6400_from_bytes)(&_d, d, RSA3072_LEN);

	/* m = pow(m, d, n) */
	if (F_SYMBOL(bn6400_redc_pow)(&_m, &_m, &_d, &_n))
		return -1;

	F_SYMBOL(bn6400_to_bytes)(&_m, msg, RSA3072_LEN);

	return 0;
}

/* @func: rsa3072_gencrt
 * #desc:
 *    generate rsa chinese remainder theorem parameter.
 *
 * #1: p    [in]  prime P
 * #2: q    [in]  prime Q
 * #3: d    [in]  decryption key
 * #4: dp   [out] crt P parameter
 * #5: dq   [out] crt Q parameter
 * #6: qinv [out] Q inverse
 * #r:      [ret] 0: success, -1: fail
 */
int32_t F_SYMBOL(rsa3072_gencrt)(const uint8_t *p, const uint8_t *q,
		const uint8_t *d, uint8_t *dp, uint8_t *dq, uint8_t *qinv)
{
	bn6400_t _p, _q, _d, a;

	F_SYMBOL(bn6400_from_bytes)(&_p, p, RSA3072_PRIME_LEN);
	F_SYMBOL(bn6400_from_bytes)(&_q, q, RSA3072_PRIME_LEN);
	F_SYMBOL(bn6400_from_bytes)(&_d, d, RSA3072_LEN);

	/* qinv = inv(q, p) */
	if (F_SYMBOL(bn6400_inv)(&a, &_q, &_p))
		return -1;

	F_SYMBOL(bn6400_to_bytes)(&a, qinv, RSA3072_PRIME_LEN);

	/* dp = d % (p - 1) */
	F_SYMBOL(bn6400_sub_1)(&a, &_p, 1);
	F_SYMBOL(bn6400_divmod)(NULL, &a, &_d, &a);

	F_SYMBOL(bn6400_to_bytes)(&a, dp, RSA3072_PRIME_LEN);

	/* dq = d % (q - 1) */
	F_SYMBOL(bn6400_sub_1)(&a, &_q, 1);
	F_SYMBOL(bn6400_divmod)(NULL, &a, &_d, &a);

	F_SYMBOL(bn6400_to_bytes)(&a, dq, RSA3072_PRIME_LEN);

	return 0;
}

/* @func: rsa3072_crt_decrypt
 * #desc:
 *    rsa chinese remainder theorem decryption.
 *
 * #1: p    [in]     prime P
 * #2: q    [in]     prime Q
 * #3: dp   [in]     crt P parameter
 * #4: dq   [in]     crt Q parameter
 * #5: qinv [in]     Q inverse
 * #6: msg  [in/out] message and cipher
 * #r:      [ret]    0: success, -1: fail
 */
int32_t F_SYMBOL(rsa3072_crt_decrypt)(const uint8_t *p, const uint8_t *q,
		const uint8_t *dp, const uint8_t *dq, const uint8_t *qinv,
		uint8_t *msg)
{
	bn6400_t _p, _q, _m, mp, mq, a;

	F_SYMBOL(bn6400_from_bytes)(&_m, msg, RSA3072_LEN);
	F_SYMBOL(bn6400_from_bytes)(&_p, p, RSA3072_PRIME_LEN);
	F_SYMBOL(bn6400_from_bytes)(&a, dp, RSA3072_PRIME_LEN);

	/* mp = pow(m % p, dp, p) */
	F_SYMBOL(bn6400_divmod)(NULL, &mp, &_m, &_p);
	if (F_SYMBOL(bn6400_redc_pow)(&mp, &mp, &a, &_p))
		return -1;

	F_SYMBOL(bn6400_from_bytes)(&_q, q, RSA3072_PRIME_LEN);
	F_SYMBOL(bn6400_from_bytes)(&a, dq, RSA3072_PRIME_LEN);

	/* mq = pow(m % q, dq, q) */
	F_SYMBOL(bn6400_divmod)(NULL, &mq, &_m, &_q);
	if (F_SYMBOL(bn6400_redc_pow)(&mq, &mq, &a, &_q))
		return -1;

	F_SYMBOL(bn6400_from_bytes)(&a, qinv, RSA3072_PRIME_LEN);

	/* m = ((mp - mq) * qinv) % p */
	F_SYMBOL(bn6400_sub)(&_m, &mp, &mq);
	F_SYMBOL(bn6400_mul)(&_m, &_m, &a);
	F_SYMBOL(bn6400_divmod)(NULL, &_m, &_m, &_p);

	/* m = (m * q) + mq */
	F_SYMBOL(bn6400_mul)(&_m, &_m, &_q);
	F_SYMBOL(bn6400_add)(&_m, &_m, &mq);

	F_SYMBOL(bn6400_to_bytes)(&_m, msg, RSA3072_LEN);

	return 0;
}
