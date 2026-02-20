/* @file: ini_parse.c
 * #desc:
 *    The implementations of ini (initial configuration) parser.
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
#include <demoz/conf/ini_parse.h>


/* @def: _ */
// [section] # comment
//
// [  section	] ; comment
//   	key = val @ ; comment
//   	key ="string"# comment
//
//   key =	# comment
//

/* [A-Za-z0-9\x80-\xff._-] */
static const uint8_t ini_name[256] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};

#define INI_COMMENT(x) ((x) == '#' || (x) == ';')
#define SKIP_CHAR(x) ((x) == ' ' || (x) == '\t')
#define INVALID_CHAR(x) ((uint8_t)(x) < 0x20)
/* end */

/* @func: _ini_section (static)
 * #desc:
 *    parse ini section.
 *
 * #1: ctx [in/out] ini struct context
 * #2: a   [in]     input row
 * #3: n   [in]     row length
 * #r:     [ret]    0: no error, -1: error, -2: call error
 */
static int32_t _ini_section(struct ini_ctx *ctx, const char *a, int32_t n)
{
	const char *p = NULL;
	int32_t st = 0, len = 0;
	for (int32_t i = 0; i < n; a++, i++) {
		char c = *a;
		switch (st) {
			case 0:
				if (c == '[')
					st = 1;
				break;
			case 1:
				if (SKIP_CHAR(c))
					break;
				p = a;
				st = 2;
			case 2:
				if (SKIP_CHAR(c)) {
					st = 3;
					break;
				} else if (c == ']') {
					if (ctx->call(INI_SECTION_TYPE,
							p,
							len,
							ctx->arg))
						return -2;
					st = 4;
					break;
				}

				if (ini_name[(uint8_t)c]) {
					ctx->err = INI_ERR_SECTION_NAME;
					return -1;
				}
				len++;
				break;
			case 3:
				if (SKIP_CHAR(c))
					break;
				if (c != ']') {
					ctx->err = INI_ERR_SECTION_END;
					return -1;
				}

				if (ctx->call(INI_SECTION_TYPE,
						p,
						len,
						ctx->arg))
					return -2;
				st = 4;
				break;
			case 4:
				if (SKIP_CHAR(c))
					break;
				if (INI_COMMENT(c))
					return 0;
				ctx->err = INI_ERR_SECTION_END;
				return -1;
			default:
				return -1;
		}
	}
	if (st != 4) {
		ctx->err = INI_ERR_SECTION_END;
		return -1;
	}

	return 0;
}

/* @func: _ini_key (static)
 * #desc:
 *    parse ini key and value.
 *
 * #1: ctx [in/out] ini struct context
 * #2: a   [in]     input row
 * #3: n   [in]     row length
 * #r:     [ret]    0: no error, -1: error, -2: call error
 */
static int32_t _ini_key(struct ini_ctx *ctx, const char *a, int32_t n)
{
	const char *p = NULL, *e = NULL;
	int32_t st = 0, len = 0;
	for (int32_t i = 0; i < n; a++, i++) {
		char c = *a;
		switch (st) {
			case 0:
				if (ini_name[(uint8_t)c]) {
					ctx->err = INI_ERR_KEY_NAME;
					return -1;
				}
				p = a;
				len++;
				st = 1;
				break;
			case 1:
				if (SKIP_CHAR(c)) {
					st = 2;
					break;
				}
				if (c == '=') {
					if (ctx->call(INI_KEY_TYPE,
							p,
							len,
							ctx->arg))
						return -2;
					st = 3;
					break;
				}

				if (ini_name[(uint8_t)c]) {
					ctx->err = INI_ERR_KEY_NAME;
					return -1;
				}
				len++;
				break;
			case 2:
				if (SKIP_CHAR(c))
					break;
				if (c != '=') {
					ctx->err = INI_ERR_KEY_VALUE;
					return -1;
				}

				if (ctx->call(INI_KEY_TYPE,
						p,
						len,
						ctx->arg))
					return -2;
				st = 3;
				break;
			case 3:
				if (SKIP_CHAR(c))
					break;
				if (INI_COMMENT(c))
					return 0;
				if (INVALID_CHAR(c)) {
					ctx->err = INI_ERR_VALUE_NAME;
					return -1;
				}
				p = a;
				st = 4;
			case 4:
				if (INVALID_CHAR(c)) {
					ctx->err = INI_ERR_VALUE_NAME;
					return -1;
				}

				if (INI_COMMENT(c)) {
					len = (int32_t)((e ? (e + 1) : a) - p);
					if (ctx->call(INI_VALUE_TYPE,
							p,
							len,
							ctx->arg))
						return -2;
					return 0;
				}

				if (!SKIP_CHAR(c))
					e = a;
				break;
			default:
				return -1;
		}
	}
	if (st == 4) {
		len = (int32_t)((e ? (e + 1) : a) - p);
		if (ctx->call(INI_VALUE_TYPE,
				p,
				len,
				ctx->arg))
			return -2;
		return 0;
	}

	return (st == 3) ? 0 : -1;
}

/* @func: ini_parse
 * #desc:
 *    ini (initial configuration) parser.
 *
 * #1: ctx [in/out] ini struct context
 * #2: s   [in]     input string
 * #r:     [ret]    0: no error, -1: error, -2: call error
 */
int32_t F_SYMBOL(ini_parse)(struct ini_ctx *ctx, const char *s)
{
	ctx->str = s;
	ctx->len = 0;
	ctx->err = 0;

	const char *p = NULL, *a = NULL;
	int32_t n = 0, k = 0;
	for (; *(ctx->str) != '\0'; ctx->str = p, ctx->len += n) {
		a = ctx->str;
		p = C_SYMBOL(strchr)(ctx->str, '\n');
		if (!p) {
			p = ctx->str;
			p += C_SYMBOL(strlen)(p);
			n = (int32_t)(p - ctx->str);
		} else {
			n = (int32_t)(p - ctx->str);
			p++;
			ctx->len++;
		}
		if (!n)
			continue;

		for (k = n; k; a++, k--) {
			char c = *a;
			if (SKIP_CHAR(c))
				continue;
			if (INVALID_CHAR(c)) {
				ctx->err = INI_ERR_INVALID;
				ctx->str = a;
				ctx->len = n;
				return -1;
			}
			if (INI_COMMENT(c))
				break;
			break;
		}
		if (!k || INI_COMMENT(*a))
			continue;

		if (*a == '[') {
			k = _ini_section(ctx, a, k);
			if (k < 0)
				return k;
		} else {
			k = _ini_key(ctx, a, k);
			if (k < 0)
				return k;
		}
	}

	if (ctx->call_end) {
		k = ctx->call_end(ctx->arg);
		if (k)
			return -1;
	}

	return 0;
}
