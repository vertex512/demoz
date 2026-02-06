/* @file: math.c
 * #desc:
 *    The implementations of mathematical.
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
#include <demoz/c/math.h>


/* @func: fabs
 * #desc:
 *    return an floating-point absolute value.
 *
 * #1: input value
 * #r: absolute value
 */
double C_SYMBOL(fabs)(double x)
{
	union {
		double f;
		uint64_t i;
	} u = { x };

	u.i &= 0x7fffffffffffffffULL;

	return u.f;
}

/* @func: fpclassifyf
 * #desc:
 *    classify real floating-point type.
 *
 * #1: input value
 * #r: floating-point type
 */
int32_t C_SYMBOL(fpclassifyf)(float x)
{
	union {
		float f;
		uint32_t i;
	} u = { x };
	int32_t e = (u.i >> 23) & 0xff;
	int32_t f = u.i & 0x7fffff;

	if (!e)
		return f ? X_FP_SUBNORMAL : X_FP_ZERO;
	if (e == 0xff)
		return f ? X_FP_NAN : X_FP_INFINITE;

	return X_FP_NORMAL;
}

/* @func: fpclassify
 * #desc:
 *    classify real floating-point type.
 *
 * #1: input value
 * #r: floating-point type
 */
int32_t C_SYMBOL(fpclassify)(double x)
{
	union {
		double f;
		uint64_t i;
	} u = { x };
	uint64_t e = (u.i >> 52) & 0x7ff;
	uint64_t f = u.i & 0xfffffffffffffULL;

	if (!e)
		return f ? X_FP_SUBNORMAL : X_FP_ZERO;
	if (e == 0x7ff)
		return f ? X_FP_NAN : X_FP_INFINITE;

	return X_FP_NORMAL;
}

/* @func: floor
 * #desc:
 *    compute the maximum integer value of x.
 *
 * #1: input value
 * #r: maximum integer value
 */
double C_SYMBOL(floor)(double x)
{
	union {
		double f;
		uint64_t i;
	} u = { x };
	uint32_t e = ((u.i >> 52) & 0x7ff) - 1023;

	if (e > 51)
		return x;
	if (e < 0)
		return (u.i << 63) ? -1.0 : 0.0;

	uint64_t m = 1ULL << (52 - e);
	if (!(u.i & (m - 1)))
		return x;

	if (x < 0.0)
		u.i += m;
	u.i &= ~(m - 1);

	return u.f;
}

/* @func: fmod
 * #desc:
 *    remainder function.
 *
 * #1: input value
 * #2: denominator
 * #r: remainder
 */
double C_SYMBOL(fmod)(double x, double y)
{
	double n = C_SYMBOL(floor)(x / y);

	return x - (n * y);
}

/* @func: frexp
 * #desc:
 *    extract the mantissa and exponent from floating-point.
 *
 * #1: input value
 * #2: exponent
 * #r: mantissa
 */
double C_SYMBOL(frexp)(double x, int32_t *e)
{
	union {
		double f;
		uint64_t i;
	} u = { x };
	int32_t ee = (u.i >> 52) & 0x7ff;

	if (!ee) {
		if (x) {
			x = C_SYMBOL(frexp)(x * 0x1p64, e);
			*e -= 64;
		} else {
			*e = 0;
		}
		return x;
	} else if (ee == 0x7ff) {
		return x;
	}

	*e = ee - 1022;
	u.i &= 0x800fffffffffffffULL;
	u.i |= 0x3fe0000000000000ULL;

	return u.f;
}

/* @func: modf
 * #desc:
 *    extract floating-point integer and decimals
 *
 * #1: input value
 * #2: integer
 * #r: decimals
 */
double C_SYMBOL(modf)(double x, double *n)
{
	union {
		double f;
		uint64_t i;
	} u = { x };
	uint32_t e = ((u.i >> 52) & 0x7ff) - 1023;

	if (e > 51) {
		*n = x;
		return 0.0;
	}
	if (e < 0) {
		*n = 0.0;
		return x;
	}

	uint64_t m = 1ULL << (52 - e);
	if (!(u.i & (m - 1))) {
		*n = x;
		u.i &= 0x8000000000000000ULL;
		return u.f;
	}

	u.i &= ~(m - 1);
	*n = u.f;

	return x - u.f;
}

/* @func: ldexp
 * #desc:
 *    multiply by an integer power of 2.
 *
 * #1: input floating-point
 * #2: exponent
 * #r: return x*2^n
 */
double C_SYMBOL(ldexp)(double x, int32_t n)
{
	union {
		double f;
		uint64_t i;
	} u = { x };

	if (n > 1023) {
		x *= 0x1p1023;
		n -= 1023;
		if (n > 1023) {
			x *= 0x1p1023;
			n -= 1023;
			if (n > 1023)
				n = 1023;
		}
	} else if (n < -1022) {
		x *= 0x1p-1022 * 0x1p53;
		n += 1022 - 53;
		if (n < -1022) {
			x *= 0x1p-1022 * 0x1p53;
			n += 1022 - 53;
			if (n < -1022)
				n = -1022;
		}
	}

	u.i = (uint64_t)(1023 + n) << 52;
	x *= u.f;

	return x;
}

/* @func: cos
 * #desc:
 *    cosine function.
 *
 * #1: input value
 * #r: cosine of value
 */
double C_SYMBOL(cos)(double x)
{
	int32_t dneg = 1;
	x = C_SYMBOL(fmod)(x, X_M_PI * 2);
	if (x > X_M_PI) {
		x -= X_M_PI;
		dneg = -1;
	}

	/* taylor series running product */
	int32_t sign = -1;
	double m = 1.0, q = 1.0, x2 = x * x;
	for (int32_t i = 1; i <= 10; i++) {
		q *= x2 / ((2 * i) * (2 * i - 1.0));
		m += sign * q;
		sign *= -1;
	}

	return m * dneg;
}

/* @func: sin
 * #desc:
 *    sine function.
 *
 * #1: input value
 * #r: sine of value
 */
double C_SYMBOL(sin)(double x)
{
	int32_t dneg = 1;
	x = C_SYMBOL(fmod)(x, X_M_PI * 2);
	if (x > X_M_PI) {
		x -= X_M_PI;
		dneg = -1;
	}

	/* taylor series running product */
	int32_t sign = -1;
	double m = x, q = x, x2 = x * x;
	for (int32_t i = 1; i <= 10; i++) {
		q *= x2 / ((2 * i) * (2 * i + 1));
		m += sign * q;
		sign *= -1;
	}

	return m * dneg;
}

/* @func: tan
 * #desc:
 *    tangent function.
 *
 * #1: input value
 * #r: tangent of value
 */
double C_SYMBOL(tan)(double x)
{
	return C_SYMBOL(sin)(x) / C_SYMBOL(cos)(x);
}

/* @func: acos
 * #desc:
 *    arc cosine function.
 *
 * #1: input value
 * #r: arc cosine of value
 */
double C_SYMBOL(acos)(double x)
{
	if (x < -1.0 || x > 1.0)
		return NAN;

	double m = X_M_PI_2;
	for (int32_t i = 0; i < 5; i++)
		m -= (C_SYMBOL(cos)(m) - x) / (-C_SYMBOL(sin)(m));

	return m;
}

/* @func: atan
 * #desc:
 *    arc tangent function.
 *
 * #1: input value
 * #r: arc tangent of value
 */
double C_SYMBOL(atan)(double x)
{
	if (x > 1.0) {
		return X_M_PI_2 - C_SYMBOL(atan)(1.0 / x);
	} else if (x < -1.0) {
		return -X_M_PI_2 - C_SYMBOL(atan)(1.0 / x);
	}

#if 0

	/* taylor series running product */
	double m = x, q = x, x2 = x * x;
	for (int32_t i = 1; i <= 10; i++) {
		q *= -x2;
		m += q / (2 * i + 1);
	}

#else

	/* precomputed coefficients */
	static const double coeffs[10] = {
		1.0,
		-1.0 / 3,
		1.0 / 5,
		-1.0 / 7,
		1.0 / 9,
		-1.0 / 11,
		1.0 / 13,
		-1.0 / 15,
		1.0 / 17,
		-1.0 / 19
		};

	double m = 0.0, p = x;
	for (int32_t i = 0; i < 10; i++) {
		m += coeffs[i] * p;
		p *= x * x;
	}

#endif

	return m;
}

/* @func: sqrt
 * #desc:
 *    square root function.
 *
 * #1: input value
 * #r: square root of value
 */
double C_SYMBOL(sqrt)(double x)
{
	if (x < 0.0)
		return NAN;

#if 0

	/* newton-raphson method */
	double m = x, q = 1.0;
	for (int32_t i = 0; i < 20; i++) {
		m = (m + q) / 2;
		q = x / m;
	}

#else

	/* newton-raphson method */
	union {
		double f;
		uint64_t i;
	} u = { x };
	u.i = 0x1ff7a3bea91d9b1bULL + (u.i >> 1);
	double m = u.f;
	m = 0.5 * (m + x / m);
	m = 0.5 * (m + x / m);
	m = 0.5 * (m + x / m);
	m = 0.5 * (m + x / m);

#endif

	return m;
}

/* @func: asin
 * #desc:
 *    arc cosine function.
 *
 * #1: input value
 * #r: arc cosine of value
 */
double C_SYMBOL(asin)(double x)
{
	if (x < -1.0 || x > 1.0)
		return NAN;

	if (x == 1.0) {
		return X_M_PI / 2;
	} else if (x == -1.0) {
		return -X_M_PI / 2;
	}

	return C_SYMBOL(atan)(x / C_SYMBOL(sqrt)(1 - x * x));
}

/* @func: exp
 * #desc:
 *    exponential function.
 *
 * #1: input value
 * #r: cardinality exponent of value
 */
double C_SYMBOL(exp)(double x)
{
	x *= 1 / X_M_LN2;
	int32_t n = (int32_t)x;
	x -= n;

	/* horner + remez poly */
	double m = 0.00133335581;
	m = m * x + 0.00961812911;
	m = m * x + 0.05550410866;
	m = m * x + 0.24022650695;
	m = m * x + 0.69314718056;
	m = m * x + 1.0;

	return C_SYMBOL(ldexp)(m, n);
}

/* @func: log
 * #desc:
 *    natural logarithm function.
 *
 * #1: input value
 * #r: natural logarithm of value
 */
double C_SYMBOL(log)(double x)
{
	if (x <= 0.0)
		return NAN;
	if (x == 1)
		return 0.0;

	int32_t e;
	x = C_SYMBOL(frexp)(x, &e);

	if (x < X_M_SQRT1_2) {
		x *= 2.0;
		e -= 1;
	}
	x -= 1.0;

	/* horner + remez poly */
	double m = 0.14218894;
	m = m * x + -0.16679639;
	m = m * x + 0.19992875;
	m = m * x + -0.24997585;
	m = m * x + 0.33333736;
	m = m * x + -0.49999908;
	m = m * x + 0.99999955;
	m = m * x + -0.00027870118;

	m += e * X_M_LN2;

	return m;
}

/* @func: log2
 * #desc:
 *    natural logarithm function of base 2.
 *
 * #1: input value
 * #r: natural logarithm of value
 */
double C_SYMBOL(log2)(double x)
{
	return C_SYMBOL(log)(x) / X_M_LN2;
}

/* @func: log10
 * #desc:
 *    natural logarithm function of base 10.
 *
 * #1: input value
 * #r: natural logarithm of value
 */
double C_SYMBOL(log10)(double x)
{
	return C_SYMBOL(log)(x) / X_M_LN10;
}

/* @func: log1p
 * #desc:
 *    logarithm of 1 plus argument.
 *
 * #1: input value
 * #r: natural logarithm of value
 */
double C_SYMBOL(log1p)(double x)
{
	if (x <= -1.0)
		return NAN;

	return C_SYMBOL(log)(1.0 + x);
}

/* @func: _pow (static)
 * #desc:
 *    power function.
 *
 * #1: input value
 * #2: exponent value
 * #r: return x^e power value
 */
static double _pow(double x, double e)
{
	double m = 1.0;
	uint64_t abs_e = (uint64_t)C_SYMBOL(fabs)(e);
	while (abs_e > 0) {
		if (abs_e % 2 == 1)
			m *= x;
		abs_e >>= 1;
		x *= x;
	}
	m = (e < 0.0) ? (1.0 / m) : m;

	return m;
}

/* @func: pow
 * #desc:
 *    power function.
 *
 * #1: input value
 * #2: exponent value
 * #r: return x^e power value
 */
double C_SYMBOL(pow)(double x, double e)
{
	double m = 0.0, q = 0.0;
	m = _pow(C_SYMBOL(fabs)(x), e);

	q = e - (int64_t)e;
	if (q != 0.0)
		m *= C_SYMBOL(exp)(q * C_SYMBOL(log)(x));

	return m;
}
