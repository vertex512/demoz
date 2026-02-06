/* @file: stdio_printf.c
 * #desc:
 *    The implementations of printf functions internal.
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
#include <demoz/c/stdarg.h>
#include <demoz/c/string.h>
#include <demoz/c/stdlib.h>
#include <demoz/c/float.h>
#include <demoz/c/math.h>
#include <demoz/c/stdio.h>


/* @def: _ */
struct printf_ctx {
	char specifiers;
	int32_t align;
	int32_t precise;
	int32_t flags;
	union {
		uint64_t i;
		double f;
		char c;
		const char *s;
	} va;
	void *arg;
	/* string, length, arg */
	int32_t (*out)(const char *, int32_t, void *);
	/* context, character, arg */
	int32_t (*pad)(struct printf_ctx *, char, int32_t);
};

#define FG_LONG 0x01
#define FG_LONG_LONG 0x02
#define FG_SHORT 0x04
#define FG_CHAR 0x08
#define FG_LONG_DOUBLE 0x10
#define FG_ALIGN_LEFT 0x20
#define FG_ALIGN_RIGHT 0x40
#define FG_ALIGN_RIGHT_ZERO 0x80
#define FG_BASE_PREFIX 0x100
#define FG_POSITIVE_NEG 0x200
#define FG_PRECISE 0x400

#define DOU2STR_DF_PREMAX 324
#define DOU2STR_DF_LENMAX 680
/* end */

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

/* @func: _int2str_o (static)
 * #desc:
 *    unsigned number to octal conversion.
 *
 * #1: buffer offset
 * #2: buffer
 * #3: input number
 * #r: output length (+offset)
 */
static int32_t _int2str_o(int32_t n, char *p, uint64_t v)
{
	int32_t t = n;

	do {
		p[n++] = (v & 7) + '0';
		v >>= 3;
	} while (v);

	int32_t k = n;
	while (k > t) {
		char w = p[--k];
		p[k] = p[t];
		p[t++] = w;
	}

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

/* @func: _int2str_x (static)
 * #desc:
 *    unsigned number to hexadecimal conversion.
 *
 * #1: buffer offset
 * #2: buffer
 * #3: input number
 * #r: output length (+offset)
 */
static int32_t _int2str_x(int32_t n, char *p, uint64_t v)
{
	int32_t t = n;

	do {
		int32_t k = v & 15;
		p[n++] = (k < 10) ? (k + '0') : (k + 'a' - 10);
		v >>= 4;
	} while (v);

	int32_t k = n;
	while (k > t) {
		char w = p[--k];
		p[k] = p[t];
		p[t++] = w;
	}

	return n;
}

/* @func: _int2str_X (static)
 * #desc:
 *    unsigned number to hexadecimal (uppercase) conversion.
 *
 * #1: buffer offset
 * #2: buffer
 * #3: input number
 * #r: output length (+offset)
 */
static int32_t _int2str_X(int32_t n, char *p, uint64_t v)
{
	int32_t t = n;

	do {
		int32_t k = v & 15;
		p[n++] = (k < 10) ? (k + '0') : (k + 'A' - 10);
		v >>= 4;
	} while (v);

	int32_t k = n;
	while (k > t) {
		char w = p[--k];
		p[k] = p[t];
		p[t++] = w;
	}

	return n;
}

/* @func: _dou2str_df (static)
 * #desc:
 *    floating number to string conversion.
 *
 * #1: buffer offset
 * #2: buffer
 * #3: input floating
 * #4: floating precision
 * #r: output length (+offset)
 */
static int32_t _dou2str_df(int32_t n, char *p, double v, int32_t pre)
{
	uint32_t big[90], *a, *z, *r;
	int32_t e = 0, intlen = 0, need = 0;

	v = C_SYMBOL(frexp)(C_SYMBOL(fabs)(v), &e);

	/* integer length, log10(v) + e * log10(2) == log10(v * 2^e) */
	intlen = (int32_t)C_SYMBOL(floor)(C_SYMBOL(log10)(v)
		+ e * 0.3010299956639812) + 1;

	/* compensate precision */
	v *= 2;
	if (v)
		e--;
	if (v) {
		v *= 0x1p28;
		e -= 28;
	}

	if (v < 0) {
		a = z = r = big;
	} else {
		a = z = r = big + (sizeof(big) / sizeof(uint32_t))
			- X_FP_DBL_MANT_DIG - 1;
	}

	/* enlarge */
	do {
		*z = (uint32_t)v;
		v = (v - (uint32_t)(*z++)) * 1000000000;
	} while (v);

#undef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

	/* max precision */
	pre = (pre < 0) ? -pre : pre;
	pre = MIN(pre, DOU2STR_DF_PREMAX);
	need = ((pre + (X_FP_DBL_MANT_DIG / 3) + 8) / 9) + 1;

	while (e > 0) { /* positive */
		int32_t sh = MIN(29, e);
		uint32_t carry = 0;
		for (uint32_t *d = z - 1; d >= a; d--) {
			uint64_t k = ((uint64_t)*d << sh) + carry;
			*d = k % 1000000000;
			carry = k / 1000000000;
		}
		if (carry)
			*--a = carry;
		while (z > a && !z[-1])
			z--;
		e -= sh;
	}
	while (e < 0) { /* negative */
		int32_t sh = MIN(9, -e);
		uint32_t carry = 0;
		for (uint32_t *d = a; d < z; d++) {
			uint32_t k = *d & ((1 << sh) - 1);
			*d = (*d >> sh) + carry;
			carry = (1000000000 >> sh) * k;
		}
		if (!*a)
			a++;
		if (carry)
			*z++ = carry;
		if ((int32_t)(z - r) > need)
			z = r + need;
		e += sh;
	}

	if (intlen <= 0) {
		p[n++] = '0';
		if (pre > 0) { /* prefix padding */
			p[n++] = '.';
			n = _out_pad(n, p, '0', -intlen);
			pre -= MIN(-intlen, pre);
			intlen = 0;
		}
	}

	char buf[12], *w;
	for (uint32_t *d = a; d < z; d++) {
		int32_t len = _int2str_d(0, buf, *d);
		if (d != a && len < 9) {
			_out_pad(0, buf, '0', 9 - len);
			_int2str_d(9 - len, buf, *d);
			len = 9;
		}
		buf[len] = '\0';
		w = buf;

		if (intlen > 0) { /* integer */
			for (; len && intlen; len--, intlen--)
				p[n++] = *w++;
			if (!intlen && pre > 0) { /* decimals */
				p[n++] = '.';
				for (; len && pre; len--, pre--)
					p[n++] = *w++;
			}
		} else if (pre > 0) { /* decimals */
			for (; len && pre; len--, pre--)
				p[n++] = *w++;
		} else {
			break;
		}
	}

	return _out_pad(n, p, '0', pre); /* suffix padding */
}

/* @func: _printf_di (static)
 * #desc:
 *    print in integer (signed decimal) format.
 *
 * #1: printf struct context
 * #r: 0: no error, -1: error
 */
static int32_t _printf_di(struct printf_ctx *ctx)
{
	char buf[32];
	int32_t neg = 0, a_len = 0, z_len = 0, len = 0;
	int32_t flags = ctx->flags;
	int64_t v = (int64_t)(ctx->va.i);

	if (v < 0) {
		neg = 1;
		v = -v;
	}
	len = _int2str_d(0, buf, (uint64_t)v);

	a_len = ctx->align - len;
	if (neg || flags & FG_POSITIVE_NEG)
		a_len--;

	if (ctx->precise) {
		z_len = ctx->precise - len;
		a_len -= z_len;
	} else if (flags & FG_ALIGN_RIGHT_ZERO) {
		z_len = a_len;
		a_len = 0;
	}

	/* right aligned */
	if (a_len > 0 && flags & FG_ALIGN_RIGHT) {
		if (ctx->pad(ctx, ' ', a_len))
			return -1;
	}
	/* number symbol */
	if (neg) {
		if (ctx->pad(ctx, '-', 1))
			return -1;
	} else if (flags & FG_POSITIVE_NEG) {
		if (ctx->pad(ctx, '+', 1))
			return -1;
	}
	/* zero aligned */
	if (z_len > 0) {
		if (ctx->pad(ctx, '0', z_len))
			return -1;
	}

	if (ctx->out(buf, len, ctx->arg))
		return -1;

	/* left aligned */
	if (a_len > 0 && flags & FG_ALIGN_LEFT) {
		if (ctx->pad(ctx, ' ', a_len))
			return -1;
	}

	return 0;
}

/* @func: _printf_oux (static)
 * #desc:
 *    print in integer (unsigned octal/decimal/hexadecimal) format.
 *
 * #1: printf struct context
 * #r: 0: no error, -1: error
 */
static int32_t _printf_oux(struct printf_ctx *ctx)
{
	char buf[32];
	const char *p_str = NULL;
	int32_t a_len = 0, z_len = 0, p_len = 0, len = 0;
	int32_t flags = ctx->flags;
	uint64_t v = ctx->va.i;

	switch (ctx->specifiers) {
		case 'x':
		case 'p':
			len = _int2str_x(0, buf, v);
			p_str = "0x";
			p_len = 2;
			break;
		case 'X':
			len = _int2str_X(0, buf, v);
			p_str = "0X";
			p_len = 2;
			break;
		case 'o':
			len = _int2str_o(0, buf, v);
			p_str = "0";
			p_len = 1;
			break;
		case 'u':
			len = _int2str_d(0, buf, v);
			break;
		default:
			return -1;
	}

	a_len = ctx->align - len;
	if (flags & FG_BASE_PREFIX)
		a_len -= p_len;

	if (ctx->precise) {
		z_len = ctx->precise - len;
		a_len -= z_len;
	} else if (flags & FG_ALIGN_RIGHT_ZERO) {
		z_len = a_len;
		a_len = 0;
	}

	/* right aligned */
	if (a_len > 0 && flags & FG_ALIGN_RIGHT) {
		if (ctx->pad(ctx, ' ', a_len))
			return -1;
	}
	/* number prefix */
	if (p_len > 0 && flags & FG_BASE_PREFIX) {
		if (ctx->out(p_str, p_len, ctx->arg))
			return -1;
	}
	/* zero aligned */
	if (z_len > 0) {
		if (ctx->pad(ctx, '0', z_len))
			return -1;
	}

	if (ctx->out(buf, len, ctx->arg))
		return -1;

	/* left aligned */
	if (a_len > 0 && flags & FG_ALIGN_LEFT) {
		if (ctx->pad(ctx, ' ', a_len))
			return -1;
	}

	return 0;
}

/* @func: _printf_c (static)
 * #desc:
 *    print in character format.
 *
 * #1: printf struct context
 * #r: 0: no error, -1: error
 */
static int32_t _printf_c(struct printf_ctx *ctx)
{
	int32_t a_len = 0, z_len = 0;
	int32_t flags = ctx->flags;
	char v = ctx->va.c;

	a_len = ctx->align - 1;
	if (flags & FG_ALIGN_RIGHT_ZERO) {
		z_len = a_len;
		a_len = 0;
	}

	/* right aligned */
	if (a_len > 0 && flags & FG_ALIGN_RIGHT) {
		if (ctx->pad(ctx, ' ', a_len))
			return -1;
	}
	/* zero aligned */
	if (z_len > 0) {
		if (ctx->pad(ctx, '0', z_len))
			return -1;
	}

	if (ctx->pad(ctx, v, 1))
		return -1;

	/* left aligned */
	if (a_len > 0 && flags & FG_ALIGN_LEFT) {
		if (ctx->pad(ctx, ' ', a_len))
			return -1;
	}

	return 0;
}

/* @func: _printf_s (static)
 * #desc:
 *    print in string format.
 *
 * #1: printf struct context
 * #r: 0: no error, -1: error
 */
static int32_t _printf_s(struct printf_ctx *ctx)
{
	const char *p = "(null)";
	int32_t a_len = 0, z_len = 0, len = 6;
	int32_t flags = ctx->flags;

	if (ctx->va.s) {
		p = ctx->va.s;
		len = (int32_t)C_SYMBOL(strlen)(p);
	}

	a_len = ctx->align - len;
	if (flags & FG_ALIGN_RIGHT_ZERO) {
		z_len = a_len;
		a_len = 0;
	}

	/* right aligned */
	if (a_len > 0 && flags & FG_ALIGN_RIGHT) {
		if (ctx->pad(ctx, ' ', a_len))
			return -1;
	}
	/* zero aligned */
	if (z_len > 0) {
		if (ctx->pad(ctx, '0', z_len))
			return -1;
	}

	if (ctx->out(p, len, ctx->arg))
		return -1;

	/* left aligned */
	if (a_len > 0 && flags & FG_ALIGN_LEFT) {
		if (ctx->pad(ctx, ' ', a_len))
			return -1;
	}

	return 0;
}

/* @func: _printf_f (static)
 * #desc:
 *    print in floating format.
 *
 * #1: printf struct context
 * #r: 0: no error, -1: error
 */
static int32_t _printf_f(struct printf_ctx *ctx)
{
	char buf[DOU2STR_DF_LENMAX];
	int32_t neg = 0, a_len = 0, z_len = 0, p_len = 0, len = 0;
	int32_t flags = ctx->flags;
	double v = ctx->va.f;

	if (v < 0)
		neg = 1;
	len = _dou2str_df(0, buf, v, ctx->precise);

	if (ctx->precise > DOU2STR_DF_PREMAX)
		p_len = ctx->precise - DOU2STR_DF_PREMAX;

	a_len = ctx->align - len - p_len;
	if (neg || flags & FG_POSITIVE_NEG)
		a_len--;

	if (flags & FG_ALIGN_RIGHT_ZERO) {
		z_len = a_len;
		a_len = 0;
	}

	/* right aligned */
	if (a_len > 0 && flags & FG_ALIGN_RIGHT) {
		if (ctx->pad(ctx, ' ', a_len))
			return -1;
	}
	/* number symbol */
	if (neg) {
		if (ctx->pad(ctx, '-', 1))
			return -1;
	} else if (flags & FG_POSITIVE_NEG) {
		if (ctx->pad(ctx, '+', 1))
			return -1;
	}
	/* zero aligned */
	if (z_len > 0) {
		if (ctx->pad(ctx, '0', z_len))
			return -1;
	}

	if (ctx->out(buf, len, ctx->arg))
		return -1;

	/* padding zero */
	if (p_len > 0) {
		if (ctx->pad(ctx, '0', p_len))
			return -1;
	}
	/* left aligned */
	if (a_len > 0 && flags & FG_ALIGN_LEFT) {
		if (ctx->pad(ctx, ' ', a_len))
			return -1;
	}

	return 0;
}

/* @func: _printf_pad (static)
 * #desc:
 *    output padding character.
 *
 * #1: printf struct context
 * #2: character
 * #3: padding length
 * #r: 0: no error, -1: error
 */
static int32_t _printf_pad(struct printf_ctx *ctx, char c, int32_t len)
{
	char buf[32];
	for (; len > 32; len -= 32) {
		_out_pad(0, buf, c, 32);
		if (ctx->out(buf, 32, ctx->arg))
			return -1;
	}
	if (len > 0) {
		_out_pad(0, buf, c, len);
		if (ctx->out(buf, len, ctx->arg))
			return -1;
	}

	return 0;
}

/* @func: ___printf
 * #desc:
 *    formatted output conversion.
 *
 * #1: format string
 * #2: variable parameter pointer
 * #3: callback arg
 * #4: callback
 * #r: 0: no error, -1: error
 */
int32_t C_SYMBOL(___printf)(const char *fmt, va_list ap, void *arg,
		int32_t (*out)(const char *, int32_t, void *))
{
	struct printf_ctx ctx = {
		.arg = arg, .out = out,
		.pad = _printf_pad
		};

	for (; *fmt != '\0'; fmt++) {
		if (*fmt == '%') {
			fmt++;
			if (*fmt == '%') {
				if (ctx.pad(&ctx, *fmt, 1))
					return -1;
				continue;
			}
		} else {
			if (ctx.pad(&ctx, *fmt, 1))
				return -1;
			continue;
		}

		ctx.align = 0;
		ctx.precise = 0;
		ctx.flags = 0;

		while (1) {
			switch (*fmt) {
				case '#': /* base prefix */
					ctx.flags |= FG_BASE_PREFIX;
					fmt++;
					break;
				case '+': /* show positive and negative */
					ctx.flags |= FG_POSITIVE_NEG;
					fmt++;
					break;
				case '-': /* align left */
					ctx.flags |= FG_ALIGN_LEFT;
					fmt++;
					break;
				case '0': /* zero padding */
					ctx.flags |= FG_ALIGN_RIGHT_ZERO;
					fmt++;
					break;
				default:
					goto e;
			}
		}
e:

#undef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

		/* align left and right */
		if (*fmt == '*' || (*fmt > '0' && *fmt <= '9')) {
			if (ctx.flags & FG_ALIGN_LEFT) {
				ctx.flags &= ~FG_ALIGN_RIGHT_ZERO;
			} else { /* align right */
				ctx.flags |= FG_ALIGN_RIGHT;
			}
			if (ctx.flags & FG_ALIGN_RIGHT_ZERO)
				ctx.flags &= ~FG_ALIGN_RIGHT;

			if (*fmt == '*') { /* dynamic */
				ctx.align = va_arg(ap, int32_t);
				fmt++;
			} else {
				ctx.align = (int32_t)C_SYMBOL(strtol)(fmt,
					(char **)&fmt, 10);
			}
			ctx.align = MAX(ctx.align, 0);
		}

		/* precise */
		if (*fmt == '.') {
			fmt++;
			if (*fmt == '*') { /* dynamic */
				ctx.precise = va_arg(ap, int32_t);
				fmt++;
			} else {
				ctx.precise = (int32_t)C_SYMBOL(strtol)(fmt,
					(char **)&fmt, 10);
			}
			ctx.precise = MAX(ctx.precise, 0);
			ctx.flags |= FG_PRECISE;
		}

		/* length modifier */
		switch (*fmt) {
			case 'L': /* long double */
				ctx.flags |= FG_LONG_DOUBLE;
				fmt++;
				break;
			case 'l':
				fmt++;
				if (*fmt == 'l') { /* long long */
					ctx.flags |= FG_LONG_LONG;
				} else { /* long */
					ctx.flags |= FG_LONG;
					fmt--;
				}
				fmt++;
				break;
			case 'h':
				fmt++;
				if (*fmt == 'h') { /* char */
					ctx.flags |= FG_CHAR;
				} else { /* short */
					ctx.flags |= FG_SHORT;
					fmt--;
				}
				fmt++;
				break;
			case 'z': /* size_t */
			case 't': /* ptrdiff_t */
				ctx.flags |= FG_LONG;
				fmt++;
				break;
			default:
				break;
		}

		/* conversion specifiers */
		ctx.specifiers = *fmt;
		switch (ctx.specifiers) {
			case 'n':
				return -1;
			case 'd': /* decimal */
			case 'i':
				if (ctx.flags & FG_LONG) {
					ctx.va.i = va_arg(ap,
						long);
				} else if (ctx.flags & FG_LONG_LONG) {
					ctx.va.i = va_arg(ap,
						long long);
				} else if (ctx.flags & FG_SHORT) {
					ctx.va.i = (signed short)va_arg(ap,
						int);
				} else if (ctx.flags & FG_CHAR) {
					ctx.va.i = (signed char)va_arg(ap,
						int);
				} else {
					ctx.va.i = va_arg(ap,
						int);
				}

				if (_printf_di(&ctx))
					return -1;
				break;
			case 'o': /* octal */
			case 'u': /* decimal */
			case 'x': /* hexadecimal */
			case 'X':
			case 'p': /* pointer */
				if (ctx.flags & FG_LONG || *fmt == 'p') {
					ctx.va.i = va_arg(ap,
						unsigned long);
				} else if (ctx.flags & FG_LONG_LONG) {
					ctx.va.i = va_arg(ap,
						unsigned long long);
				} else if (ctx.flags & FG_SHORT) {
					ctx.va.i = (unsigned short)va_arg(ap,
						unsigned int);
				} else if (ctx.flags & FG_CHAR) {
					ctx.va.i = (unsigned char)va_arg(ap,
						unsigned int);
				} else {
					ctx.va.i = va_arg(ap,
						unsigned int);
				}

				if (_printf_oux(&ctx))
					return -1;
				break;
			case 'c': /* character */
				ctx.va.c = (char)va_arg(ap, int);

				if (_printf_c(&ctx))
					return -1;
				break;
			case 's': /* string */
				ctx.va.s = va_arg(ap, char *);

				if (_printf_s(&ctx))
					return -1;
				break;
			case 'e': /* floating */
			case 'E':
			case 'f': case 'F': case 'g':
			case 'G': case 'a': case 'A':
				if (!ctx.precise && ctx.flags & FG_PRECISE)
					ctx.precise = 6;
				ctx.va.f = va_arg(ap, double);

				if (_printf_f(&ctx))
					return -1;
				break;
			default:
				return -1;
		}
	}

	return 0;
}
