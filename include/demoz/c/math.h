/* @file: math.h
 * #desc:
 *    The definitions of mathematical.
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

#ifndef _DEMOZ_C_MATH_H
#define _DEMOZ_C_MATH_H

#include <demoz/config.h>
#include <demoz/c/stdint.h>


/* @def: _
 * logarithm e */
#define X_M_E 2.7182818284590452354
/* log2(e) */
#define X_M_LOG2E 2.7182818284590452354
/* log10(e) */
#define X_M_LOG10E 0.43429448190325182765
/* log(2) */
#define X_M_LN2 0.69314718055994530942
/* log(10) */
#define X_M_LN10 2.30258509299404568402
/* pi */
#define X_M_PI 3.14159265358979323846
/* pi/2 */
#define X_M_PI_2 1.57079632679489661923
/* pi/4 */
#define X_M_PI_4 0.78539816339744830962
/* 1/pi */
#define X_M_1_PI 0.31830988618379067154
/* 2/pi */
#define X_M_2_PI 0.63661977236758134308
/* 2/sqrt(pi) */
#define X_M_2_SQRTPI 1.12837916709551257390
/* square root of 2 */
#define X_M_SQRT2 1.41421356237309504880
/* 1/sqrt(2) */
#define X_M_SQRT1_2 0.70710678118654752440

#define X_FP_NAN 0
#define X_FP_INFINITE 1
#define X_FP_ZERO 2
#define X_FP_SUBNORMAL 3
#define X_FP_NORMAL 4

static const union {
	float f;
	uint32_t i;
} _ZERO = { .i = 0x0 };

static const union {
	float f;
	uint32_t i;
} _NAN = { .i = 0x7fc00000 };

static const union {
	float f;
	uint32_t i;
} _INFINITY = { .i = 0x7f800000 };

#undef ZERO
#define ZERO (_ZERO.f)
#undef NAN
#define NAN (_NAN.f)
#undef INFINITY
#define INFINITY (_INFINITY.f)
/* end */


#ifdef __cplusplus
extern "C" {
#endif

/* c/math.c */

extern
double C_SYMBOL(fabs)(double x)
;

extern
int32_t C_SYMBOL(fpclassifyf)(float x)
;

extern
int32_t C_SYMBOL(fpclassify)(double x)
;

extern
double C_SYMBOL(floor)(double x)
;

extern
double C_SYMBOL(fmod)(double x, double y)
;

extern
double C_SYMBOL(frexp)(double x, int32_t *e)
;

extern
double C_SYMBOL(modf)(double x, double *n)
;

extern
double C_SYMBOL(ldexp)(double x, int32_t n)
;

extern
double C_SYMBOL(cos)(double x)
;

extern
double C_SYMBOL(sin)(double x)
;

extern
double C_SYMBOL(tan)(double x)
;

extern
double C_SYMBOL(acos)(double x)
;

extern
double C_SYMBOL(atan)(double x)
;

extern
double C_SYMBOL(sqrt)(double x)
;

extern
double C_SYMBOL(asin)(double x)
;

extern
double C_SYMBOL(exp)(double x)
;

extern
double C_SYMBOL(log)(double x)
;

extern
double C_SYMBOL(log2)(double x)
;

extern
double C_SYMBOL(log10)(double x)
;

extern
double C_SYMBOL(log1p)(double x)
;

extern
double C_SYMBOL(pow)(double x, double e)
;

#ifdef __cplusplus
}
#endif


#endif
