/* @file: stdio_scanf.c
 * #desc:
 *    The implementations of scanf functions internal.
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
#include <demoz/c/limits.h>
#include <demoz/c/string.h>
#include <demoz/c/stdlib.h>
#include <demoz/c/stdio.h>


/* @def: _ */
#define FG_LONG 0x01
#define FG_LONG_LONG 0x02
#define FG_SHORT 0x04
#define FG_CHAR 0x08
#define FG_LONG_DOUBLE 0x10
#define FG_SKIP 0x20
#define SC_SPACE " \t\n"
/* end */

/* @func: _scanf_expr (static)
 * #desc:
 *    expr string convert.
 *
 * #1: expr string
 * #2: input string
 * #r: 0: no error, -1: error
 */
int32_t _scanf_expr(const char **expr, const char **s)
{
	const char *p;
	char table[256];
	int32_t st = 0, tmp = 0, not = 0;
	C_SYMBOL(memset)(table, 0, sizeof(table));

	p = *expr;
	for (; *p != '\0'; p++) {
		char c = *p;
		switch (st) {
			case 0:
				if (c != '[')
					return -1;
				st = 1;
				break;
			case 1: /* [^char or [char */
				if (c == '^') {
					not = 1;
					st = 2;
					break;
				}
			case 2:
				tmp = (uint8_t)c;
				table[tmp] = 1;
				st = 3;
				break;
			case 3: /* range */
				if (c == ']')
					goto e;

				if (c == '-') {
					st = 4;
				} else {
					tmp = (uint8_t)c;
					table[tmp] = 1;
				}
				break;
			case 4:
				if (c == ']') {
					table['-'] = 1;
					goto e;
				}

				for (; tmp <= (uint8_t)c; tmp++)
					table[tmp] = 1;
				st = 3;
				break;
			default:
				return -1;
		}
	}
e:
	if (*p != ']')
		return -1;
	*expr = p;

	p = *s;
	for (; *p != '\0'; p++) {
		uint8_t c = *p;
		if (not) {
			if (table[c])
				break;
		} else {
			if (!table[c])
				break;
		}
	}
	*s = p;

	return 0;
}

/* @func: ___scanf
 * #desc:
 *    string convert to formatted value.
 *
 * #1: input string
 * #2: end pointer / NULL
 * #3: formatted string
 * #4: variable argument
 * #r: >0: number of matching, <0: format error
 */
int32_t C_SYMBOL(___scanf)(const char *s, char **e, const char *fmt,
		va_list ap)
{
	int32_t flags, buf_len, specifiers, len, b, neg, n = 0;
	const char *p;
	char *p2;
	uint64_t v, m;
	double f;

	for (; *fmt != '\0'; fmt++) {
		if (*fmt == '%') {
			fmt++;
			if (*fmt == '%') {
				if (*s != *fmt) {
					n = -n;
					goto e;
				}
				s++;
				continue;
			}
		} else {
			if (*fmt == ' ') {
				if (!C_SYMBOL(strchr)(SC_SPACE, *s)) {
					n = -n;
					goto e;
				}
			} else if (*s != *fmt) {
				n = -n;
				goto e;
			}
			s++;
			continue;
		}

		flags = 0;
		buf_len = INT32_MAX;

		/* assignment-suppression character */
		if (*fmt == '*') {
			flags |= FG_SKIP;
			fmt++;
		}

		/* buffer size limit */
		if (*fmt > '0' && *fmt <= '9') {
			buf_len = (int32_t)C_SYMBOL(strtol)(fmt,
				(char **)&fmt, 10);
		}

		/* length modifier */
		switch (*fmt) {
			case 'L': /* long double */
				flags |= FG_LONG_DOUBLE;
				fmt++;
				break;
			case 'l':
				fmt++;
				if (*fmt == 'l') { /* long long */
					flags |= FG_LONG_LONG;
				} else { /* long */
					flags |= FG_LONG;
					fmt--;
				}
				fmt++;
				break;
			case 'h':
				fmt++;
				if (*fmt == 'h') { /* char */
					flags |= FG_CHAR;
				} else { /* short */
					flags |= FG_SHORT;
					fmt--;
				}
				fmt++;
				break;
			case 'z': /* size_t */
			case 't': /* ptrdiff_t */
				flags |= FG_LONG;
				fmt++;
				break;
			default:
				break;
		}

		specifiers = *fmt;
		switch (specifiers) {
			case 'n':
				goto e;
			case 'd': /* decimal */
			case 'i': /* prefix determination */
				if (flags & FG_LONG) {
					m = LONG_MAX;
				} else if (flags & FG_LONG_LONG) {
					m = LLONG_MAX;
				} else if (flags & FG_SHORT) {
					m = SHRT_MAX;
				} else if (flags & FG_CHAR) {
					m = CHAR_MAX;
				} else {
					m = INT_MAX;
				}

				b = 10;
				neg = 0;
				if (*s == '-' || *s == '+') {
					neg = (*s == '-') ? 1 : 0;
					s++;
				}
				if (*s == '0') {
					s++;
					if (*s == 'X' || *s == 'x') {
						b = 16;
						s++;
					} else if (*s >= '0' && *s <= '7') {
						b = 8;
						s++;
					}
					if (specifiers == 'd' && b != 10)
						return -1;
				}

				v = C_SYMBOL(___strtoull)(s, (char **)&s,
					&m, b);
				v = m ? (neg ? -v : v) : 0;

				if (flags & FG_SKIP)
					break;

				if (flags & FG_LONG) {
					*va_arg(ap, long *) =
						(long)v;
				} else if (flags & FG_LONG_LONG) {
					*va_arg(ap, long long *) =
						(long long)v;
				} else if (flags & FG_SHORT) {
					*va_arg(ap, short *) =
						(short)v;
				} else if (flags & FG_CHAR) {
					*va_arg(ap, char *) =
						(char)v;
				} else {
					*va_arg(ap, int *) =
						(int)v;
				}
				break;
			case 'o': /* octal */
			case 'u': /* decimal */
			case 'x': /* hexadecimal */
			case 'X':
			case 'p': /* pointer */
				if (flags & FG_LONG) {
					m = ULONG_MAX;
				} else if (flags & FG_LONG_LONG) {
					m = ULLONG_MAX;
				} else if (flags & FG_SHORT) {
					m = USHRT_MAX;
				} else if (flags & FG_CHAR) {
					m = UCHAR_MAX;
				} else {
					m = UINT_MAX;
				}

				switch (specifiers) {
					case 'o':
						b = 8;
						break;
					case 'u':
						b = 10;
						break;
					case 'x': case 'X': case 'p':
						b = 16;
						break;
					default:
						return -1;
				}

				v = C_SYMBOL(___strtoull)(s, (char **)&s,
					&m, b);
				v = m ? v : 0;

				if (flags & FG_SKIP)
					break;

				if (specifiers == 'p') {
					*va_arg(ap, void **) =
						(void *)v;
				} else if (flags & FG_LONG) {
					*va_arg(ap, unsigned long *) =
						(unsigned long)v;
				} else if (flags & FG_LONG_LONG) {
					*va_arg(ap, unsigned long long *) =
						(unsigned long long)v;
				} else if (flags & FG_SHORT) {
					*va_arg(ap, unsigned short *) =
						(unsigned short)v;
				} else if (flags & FG_CHAR) {
					*va_arg(ap, unsigned char *) =
						(unsigned char)v;
				} else {
					*va_arg(ap, unsigned int *) =
						(unsigned int)v;
				}
				break;
			case 'c': /* character */
				if (*s == '\0')
					goto e;

				if (flags & FG_SKIP)
					break;

				*va_arg(ap, char *) = *s++;
				break;
			case 's': /* string */
				p = s;
				for (; *s != '\0'; s++) {
					if (C_SYMBOL(strchr)(SC_SPACE, *s))
						break;
				}

				len = (int32_t)(s - p);
				if (len > buf_len)
					goto e;

				if (flags & FG_SKIP)
					break;

				p2 = va_arg(ap, char *);
				C_SYMBOL(memcpy)(p2, p, len);
				p2[len] = '\0';
				break;
			case '[': /* string expr */
				p = s;
				if (_scanf_expr(&fmt, &s))
					goto e;

				len = (int32_t)(s - p);
				if (len > buf_len)
					goto e;

				if (flags & FG_SKIP)
					break;

				p2 = va_arg(ap, char *);
				C_SYMBOL(memcpy)(p2, p, len);
				p2[len] = '\0';
				break;
			case 'e': /* floating-point */
			case 'E':
			case 'f': case 'F': case 'g':
			case 'G': case 'a': case 'A':
				f = C_SYMBOL(strtod)(s, (char **)&s);

				if (flags & FG_SKIP)
					break;

				*va_arg(ap, double *) = f;
				break;
			default:
				n = -n;
				goto e;
		}
		n++;
	}

e:
	if (e) /* end position */
		*e = (char *)s;

	return n;
}
