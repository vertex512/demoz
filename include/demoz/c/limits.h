/* @file: limits.h
 * #desc:
 *    The definitions of implementation constants.
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

#ifndef _DEMOZ_C_LIMITS_H
#define _DEMOZ_C_LIMITS_H

#include <demoz/config.h>


/* @def: _ */
#undef CHAR_BIT
#define CHAR_BIT 8

#undef CHAR_MIN
#undef CHAR_MAX
#define CHAR_MIN (-0x80)
#define CHAR_MAX 0x7f

#undef SCHAR_MIN
#undef SCHAR_MIN
#undef UCHAR_MAX
#define SCHAR_MIN (-0x80)
#define SCHAR_MAX 0x7f
#define UCHAR_MAX 0xff

#undef SHRT_MIN
#undef SHRT_MAX
#undef USHRT_MAX
#define SHRT_MIN (-0x8000)
#define SHRT_MAX 0x7fff
#define USHRT_MAX 0xffff

#undef INT_MIN
#undef INT_MAX
#undef UINT_MAX
#define INT_MIN (-0x80000000)
#define INT_MAX 0x7fffffff
#define UINT_MAX 0xffffffffU

#undef LONG_MIN
#undef LONG_MAX
#undef ULONG_MAX
#undef LLONG_MIN
#undef LLONG_MAX
#undef ULLONG_MAX
#if (DEMOZ_MARCH_BITS == DEMOZ_MARCH_32)
#	define LONG_MIN INT_MIN
#	define LONG_MAX INT_MAX
#	define ULONG_MAX UINT_MAX
#	define LLONG_MIN (-0x8000000000000000LL)
#	define LLONG_MAX 0x7fffffffffffffffLL
#	define ULLONG_MAX 0xffffffffffffffffULL
#elif (DEMOZ_MARCH_BITS == DEMOZ_MARCH_64)
#	define LONG_MIN (-0x8000000000000000L)
#	define LONG_MAX 0x7fffffffffffffffL
#	define ULONG_MAX 0xffffffffffffffffUL
#	define LLONG_MIN (-0x8000000000000000LL)
#	define LLONG_MAX 0x7fffffffffffffffLL
#	define ULLONG_MAX 0xffffffffffffffffULL
#else
#	error "!!!unknown DEMOZ_MARCH_BITS!!!"
#endif
/* end */


#endif
