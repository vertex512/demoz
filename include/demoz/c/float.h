/* @file: float.h
 * #desc:
 *    The definitions of floating type.
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

#ifndef _DEMOZ_C_FLOAT_H
#define _DEMOZ_C_FLOAT_H


/* @def: _
 * base */
#define X_FP_FLT_RADIX 2
/* mantissa */
#define X_FP_FLT_MANT_DIG 24
/* exact number of significant digits */
#define X_FP_FLT_DIG 6
/* accuracy error range */
#define X_FP_FLT_EPSILON 1.19209290e-7
/* exponent */
#define X_FP_FLT_MIN_EXP (-125)
#define X_FP_FLT_MAX_EXP 128
/* size */
#define X_FP_FLT_MIN 1.17549435e-38
#define X_FP_FLT_MAX 3.40282347e+38

/* base */
#define X_FP_DBL_RADIX 2
/* mantissa */
#define X_FP_DBL_MANT_DIG 53
/* exact number of significant digits */
#define X_FP_DBL_DIG 15
/* accuracy error range */
#define X_FP_DBL_EPSILON 2.2204460492503131e-16
/* exponent */
#define X_FP_DBL_MIN_EXP (-1021)
#define X_FP_DBL_MAX_EXP 1024
/* size */
#define X_FP_DBL_MIN 2.2250738585072014e-308
#define X_FP_DBL_MAX 1.7976931348623157e+308
/* end */


#endif
