/* @file: stdlib_atoi.c
 * #desc:
 *    The implementations of standard library.
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
#include <demoz/c/limits.h>
#include <demoz/c/stdlib.h>


/* @func: ___strtoull
 * #desc:
 *    string to integer conversion.
 *
 * #1: s [in]     input string
 * #2: e [out]    end pointer / NULL
 * #3: m [in/out] overflow limit
 * #4: b [in]     base type
 * #r:   [ret]    return unsigned long long
 */
uint64_t C_SYMBOL(___strtoull)(const char *s, char **e, uint64_t *m, int32_t b)
{
	for (; *s == ' ' || *s == '\t'; s++);

	int32_t neg = 0; /* negative */
	if (*s == '-' || *s == '+')
		neg = (*s++ == '-') ? 1 : 0;

	if (b) {
		if (*s == '0') {
			s++;
			if (*s == 'B' || *s == 'b') {
				if (b != 2)
					goto e;
				s++;
			} else if (*s == 'X' || *s == 'x') {
				if (b != 16)
					goto e;
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

	uint64_t n = 0;
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

		if (n > ((*m - d) / b))
			*m = 0;
		n = (n * b) + d;
	}

e:
	if (e) /* end position */
		*e = (char *)s;

	return neg ? -n : n;
}

/* @func: atoi
 * #desc:
 *    string to int conversion.
 *
 * #1: s [in]  input pointer
 * #r:   [ret] return int
 */
int C_SYMBOL(atoi)(const char *s)
{
	uint64_t m = INT_MAX;
	uint64_t n = C_SYMBOL(___strtoull)(s, NULL, &m, 10);

	if (!m) { /* errno */
		n = 0;
	}

	return (int)n;
}

/* @func: atol
 * #desc:
 *    string to long conversion.
 *
 * #1: s [in]  input string
 * #r:   [ret] return long
 */
long C_SYMBOL(atol)(const char *s)
{
	uint64_t m = LONG_MAX;
	uint64_t n = C_SYMBOL(___strtoull)(s, NULL, &m, 10);

	if (!m) { /* errno */
		n = 0;
	}

	return (long)n;
}

/* @func: atoll
 * #desc:
 *    string to long long conversion.
 *
 * #1: s [in]  input string
 * #r:   [ret] return long long
 */
long long C_SYMBOL(atoll)(const char *s)
{
	uint64_t m = LLONG_MAX;
	uint64_t n = C_SYMBOL(___strtoull)(s, NULL, &m, 10);

	if (!m) { /* errno */
		n = 0;
	}

	return (long long)n;
}

/* @func: strtol
 * #desc:
 *    string to long conversion.
 *
 * #1: s [in]  input string
 * #2: e [out] end pointer / NULL
 * #3: b [in]  base type
 * #r:   [ret] return long
 */
long C_SYMBOL(strtol)(const char *s, char **e, int32_t b)
{
	uint64_t m = LONG_MAX;
	uint64_t n = C_SYMBOL(___strtoull)(s, e, &m, b);

	if (!m) { /* errno */
		n = 0;
	}

	return (long)n;
}

/* @func: strtoul
 * #desc:
 *    string to unsigned long conversion.
 *
 * #1: s [in]  input string
 * #2: e [out] end pointer / NULL
 * #3: b [in]  base type
 * #r:   [ret] return unsigned long
 */
unsigned long C_SYMBOL(strtoul)(const char *s, char **e, int32_t b)
{
	uint64_t m = ULONG_MAX;
	uint64_t n = C_SYMBOL(___strtoull)(s, e, &m, b);

	if (!m) { /* errno */
		n = 0;
	}

	return (unsigned long)n;
}

/* @func: strtoll
 * #desc:
 *    string to long long conversion.
 *
 * #1: s [in]  input string
 * #2: e [out] end pointer / NULL
 * #3: b [in]  base type
 * #r:   [ret] return long long
 */
long long C_SYMBOL(strtoll)(const char *s, char **e, int32_t b)
{
	uint64_t m = LLONG_MAX;
	uint64_t n = C_SYMBOL(___strtoull)(s, e, &m, b);

	if (!m) { /* errno */
		n = 0;
	}

	return (long long)n;
}

/* @func: strtoull
 * #desc:
 *    string to unsigned long long conversion.
 *
 * #1: s [in]  input string
 * #2: e [out] end pointer / NULL
 * #3: b [in]  base type
 * #r:   [ret] return unsigned long long
 */
unsigned long long C_SYMBOL(strtoull)(const char *s, char **e, int32_t b)
{
	uint64_t m = ULLONG_MAX;
	uint64_t n = C_SYMBOL(___strtoull)(s, e, &m, b);

	if (!m) { /* errno */
		n = 0;
	}

	return (unsigned long long)n;
}
