/* @file: stdlib_atof.c
 * #desc:
 *    The implementations of.
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
#include <demoz/c/math.h>
#include <demoz/c/stdlib.h>


/* @func: _strtod_num (static)
 * #desc:
 *    string to floating-point (integer) conversion.
 *
 * #1: s [in]  input string
 * #2: e [out] end pointer / NULL
 * #3: b [in]  base type
 * #r:   [ret] return double
 */
static double _strtod_num(const char *s, char **e, int32_t b)
{
	double x = 0.0;
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

		x = (x * b) + d;
	}

	if (e)
		*e = (char *)s;

	return x;
}

/* @func: _strtod (static)
 * #desc:
 *    string to floating-point conversion.
 *
 * #1: s [in]  input string
 * #2: e [out] end pointer / NULL
 * #r:   [ret] return double
 */
static double _strtod(const char *s, char **e)
{
	for (; *s == ' ' || *s == '\t'; s++);

	int32_t neg = 0, b = 10; /* negative */
	if (*s == '-' || *s == '+')
		neg = (*s++ == '-') ? 1 : 0;

	if (*s == '0') {
		s++;
		if (*s == 'X' || *s == 'x') {
			b = 16;
			s++;
		}
	}

	char *ee = NULL;
	if (!e)
		e = &ee;

	/* integer and decimals */
	double x = _strtod_num(s, e, b);
	s = *e;
	if (*s == '.') {
		s++;
		double p = _strtod_num(s, e, b);
		x += p / C_SYMBOL(pow)(b, (uint32_t)(*e - s));
	}

	/* exponent */
	s = *e;
	if (b == 10 && (*s == 'E' || *s == 'e')) {
		s++;
		int32_t n = (int32_t)C_SYMBOL(strtol)(s, e, 10);
		x *= C_SYMBOL(pow)(10, n);
	} else if (b == 16 && (*s == 'P' || *s == 'p')) {
		s++;
		int32_t n = (int32_t)C_SYMBOL(strtol)(s, e, 10);
		x *= C_SYMBOL(pow)(2, n);
	}

	return neg ? -x : x;
}

/* @func: atof
 * #desc:
 *    string to float conversion.
 *
 * #1: s [in]  input string
 * #r:   [ret] return double
 */
double C_SYMBOL(atof)(const char *s)
{
	return _strtod(s, NULL);
}

/* @func: strtod
 * #desc:
 *    string to double conversion.
 *
 * #1: s [in]  input string
 * #2: e [out] end pointer / NULL
 * #r:   [ret] return double
 */
double C_SYMBOL(strtod)(const char *s, char **e)
{
	return _strtod(s, e);
}
