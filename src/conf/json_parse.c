/* @file: json_parse.c
 * #desc:
 *    The implementations of json (javascript object notation) parser.
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
#include <demoz/conf/json_parse.h>


/* @def: _ */
// [
//   // Comment
//   {/* Comment */},
//   // Comment
//   [],
//   { /* Comment */
//     ""/* Comment */: /* Comment */123,
//     "": 123.123,/* Comment */
//     /* Comment */"": .123,
//     "": -12, // Comment
//     "": "",
//     "": {},
//     "": [],
//     "": true,
//     "": -123e12,
//     "": 3.14e+10,
//     '': 123,
//     '': Infinity,
//     '': NaN,
//     '': {
//       '': "Hello, World",
//       '': '',
//       "": 'Hello, \
// World',
//     }
//   },
//   [ 123, /* Comment */"", true, false, null, {}, [ 1, 2, "", {},
//   /* Comment */1.2 /* Comment */, '123'], ],
//   [],
// ]
//

enum {
	TOKEN_ARRAY = 1,
	TOKEN_ARRAY_END,
	TOKEN_OBJECT,
	TOKEN_OBJECT_END,
	TOKEN_VALUE,
	TOKEN_NEXT,
	TOKEN_STRING,
	TOKEN_NUMBER,
	TOKEN_NULL,
	TOKEN_TRUE,
	TOKEN_FALSE,
	TOKEN_COMMENT
};

static int32_t _json_token(char c);
static int32_t _json_string(struct json_ctx *ctx);
static int32_t _json_number(struct json_ctx *ctx);
static int32_t _json_null(struct json_ctx *ctx);
static int32_t _json_true(struct json_ctx *ctx);
static int32_t _json_false(struct json_ctx *ctx);
static int32_t _json_comment(struct json_ctx *ctx);
static int32_t _json_object(struct json_ctx *ctx);
static int32_t _json_array(struct json_ctx *ctx);

#define SKIP_CHAR(x) \
	((x) == ' ' || (x) == '\n' || (x) == '\t' || (x) == '\r')
#define INVALID_CHAR(x) ((uint8_t)(x) < 0x20)
/* end */

/* @func: _json_token (static)
 * #desc:
 *    get the json token.
 *
 * #1: character
 * #r: token value (-1: unknown token)
 */
static int32_t _json_token(char c)
{
	switch (c) {
		case '[':
			return TOKEN_ARRAY;
		case ']':
			return TOKEN_ARRAY_END;
		case '{':
			return TOKEN_OBJECT;
		case '}':
			return TOKEN_OBJECT_END;
		case ',':
			return TOKEN_NEXT;
		case ':':
			return TOKEN_VALUE;
		case '"':
		case '\'':
			return TOKEN_STRING;
		case 'n':
			return TOKEN_NULL;
		case 't':
			return TOKEN_TRUE;
		case 'f':
			return TOKEN_FALSE;
		case '/':
			return TOKEN_COMMENT;
		default:
			if (c == '+' || c == '-') {
				return TOKEN_NUMBER;
			} else if (c >= '0' && c <= '9') {
				return TOKEN_NUMBER;
			} else if (c == '.') {
				return TOKEN_NUMBER;
			} else if (c == 'I' || c == 'N') {
				return TOKEN_NUMBER;
			}

			return -1;
	}

	return -1;
}

/* @func: _json_string (static)
 * #desc:
 *    get the length of the json string.
 *
 * #1: json struct context
 * #r: string length (-1: error)
 */
static int32_t _json_string(struct json_ctx *ctx)
{
	char cc;
	int32_t st = 0, len = ctx->len;
	for (; *(ctx->str) != '\0'; ctx->str++, ctx->len++) {
		char c = *(ctx->str);
		switch (st) {
			case 0: /* start */
				if (c != '"' && c != '\'')
					return -1;
				cc = c;
				st = 1;
				break;
			case 1:
				if (c == cc) { /* end */
					ctx->str++;
					ctx->len++;
					return ctx->len - len;
				} else if (c == '\\') { /* escape */
					st = 2;
				}
				if (INVALID_CHAR(c))
					return -1;
				break;
			case 2: /* escape */
				st = 1;
				break;
			default:
				return -1;
		}
	}

	return -1;
}

/* @func: _json_number (static)
 * #desc:
 *    get the length of the json number.
 *
 * #1: json struct context
 * #r: number length (-1: error)
 */
static int32_t _json_number(struct json_ctx *ctx)
{
	int32_t st = 0, len = ctx->len;
	for (; *(ctx->str) != '\0'; ctx->str++, ctx->len++) {
		char c = *(ctx->str);
		switch (st) {
			case 0: /* start */
				if (c == '+' || c == '-') {
					st = 1;
					break;
				} else if (c >= '0' && c <= '9') {
					st = 1;
					break;
				} else if (c == '.') {
					st = 4;
					break;
				} else if (c == 'I' || c == 'N') {
					st = 7;
					break;
				}
				return -1;
			case 1: /* hexadecimal, decimal or floating */
				if (c == 'X' || c == 'x') {
					st = 2;
					break;
				} else if (c >= '0' && c <= '9') {
					st = 3;
					break;
				} else if (c == '.') {
					st = 4;
					break;
				} else if (c == 'I' || c == 'N') {
					st = 7;
					break;
				}
				return ctx->len - len;
			case 2: /* hexadecimal */
				if (c >= '0' && c <= '9') {
					break;
				} else if (c >= 'A' && c <= 'F') {
					break;
				} else if (c >= 'a' && c <= 'f') {
					break;
				}
				return ctx->len - len;
			case 3: /* decimal, floating, or exponent */
				if (c >= '0' && c <= '9') {
					break;
				} else if (c == '.') {
					st = 4;
					break;
				} else if (c == 'e') {
					st = 5;
					break;
				}
				return ctx->len - len;
			case 4: /* floating, or exponent */
				if (c >= '0' && c <= '9') {
					break;
				} else if (c == 'e') {
					st = 5;
					break;
				}
				return ctx->len - len;
			case 5: /* '+-', or exponent */
				if (c >= '0' && c <= '9') {
					break;
				} else if (c == '+' || c == '-') {
					st = 6;
					break;
				}
				return ctx->len - len;
			case 6: /* exponent */
				if (c >= '0' && c <= '9')
					break;
				return ctx->len - len;
			case 7: /* Infinity and NaN */
				ctx->str--;
				ctx->len--;
				if (!C_SYMBOL(strncmp)("Infinity",
						ctx->str, 8)) {
					ctx->str += 8;
					ctx->len += 8;
					return ctx->len - len;
				} else if (!C_SYMBOL(strncmp)("NaN",
						ctx->str, 3)) {
					ctx->str += 3;
					ctx->len += 3;
					return ctx->len - len;
				}
				return -1;
			default:
				return -1;
		}
	}

	return -1;
}

/* @func: _json_null (static)
 * #desc:
 *    get the length of the json null.
 *
 * #1: json struct context
 * #r: null length (-1: error)
 */
static int32_t _json_null(struct json_ctx *ctx)
{
	if (!C_SYMBOL(strncmp)("null", ctx->str, 4)) {
		ctx->str += 4;
		ctx->len += 4;
		return 4;
	}

	return -1;
}

/* @func: _json_true (static)
 * #desc:
 *    get the length of the json true.
 *
 * #1: json struct context
 * #r: true length (-1: error)
 */
static int32_t _json_true(struct json_ctx *ctx)
{
	if (!C_SYMBOL(strncmp)("true", ctx->str, 4)) {
		ctx->str += 4;
		ctx->len += 4;
		return 4;
	}

	return -1;
}

/* @func: _json_false (static)
 * #desc:
 *    get the length of the json false.
 *
 * #1: json struct context
 * #r: false length (-1: error)
 */
static int32_t _json_false(struct json_ctx *ctx)
{
	if (!C_SYMBOL(strncmp)("false", ctx->str, 5)) {
		ctx->str += 5;
		ctx->len += 5;
		return 5;
	}

	return -1;
}

/* @func: _json_comment (static)
 * #desc:
 *    get the length of the json comments.
 *
 * #1: json struct context
 * #r: comment length (-1: error)
 */
static int32_t _json_comment(struct json_ctx *ctx)
{
	int32_t st = 0, len = ctx->len;
	if (!C_SYMBOL(strncmp)("//", ctx->str, 2)) {
		ctx->str += 2;
		ctx->len += 2;
		for (; *(ctx->str) != '\0'; ctx->str++, ctx->len++) {
			if (*(ctx->str) == '\n') {
				ctx->str++;
				ctx->len++;
				break;
			}
		}
		return ctx->len - len;
	} else if (!C_SYMBOL(strncmp)("/*", ctx->str, 2)) {
		ctx->str += 2;
		ctx->len += 2;
		for (; *(ctx->str) != '\0'; ctx->str++, ctx->len++) {
			char c = *(ctx->str);
			switch (st) {
				case 0:
					if (c == '*')
						st = 1;
					break;
				case 1:
					if (c == '/') {
						ctx->str++;
						ctx->len++;
						return ctx->len - len;
					}
					st = 0;
					break;
				default:
					return -1;
			}
		}
	}

	return -1;
}

/* @func: _json_object (static)
 * #desc:
 *    parse json object.
 *
 * #1: json struct context
 * #r: 0: no error, -1: error, -2: call error
 */
static int32_t _json_object(struct json_ctx *ctx)
{
	int32_t st = 0, k = 0;
	for (; *(ctx->str) != '\0'; ctx->str++, ctx->len++) {
		char c = *(ctx->str);
		if (SKIP_CHAR(c))
			continue;
		if (INVALID_CHAR(c)) {
			ctx->err = JSON_ERR_INVALID;
			return -1;
		}

		switch (st) {
			case 0: /* start */
				if (ctx->call(JSON_OBJECT_TYPE,
						NULL,
						0,
						ctx->arg))
					return -2;
				st = 1;
				break;
			case 1:
				switch (_json_token(c)) {
					case TOKEN_COMMENT:
						k = _json_comment(ctx);
						if (k < 0) {
							ctx->err = JSON_ERR_COMMENT;
							return -1;
						}
						ctx->str--;
						ctx->len--;
						break;
					case TOKEN_STRING: /* value */
						k = _json_string(ctx);
						if (k < 0) {
							ctx->err = JSON_ERR_OBJECT_VALUE;
							return -1;
						}
						if (ctx->call(JSON_VALUE_TYPE,
								ctx->str - k + 1,
								k - 2,
								ctx->arg))
							return -2;
						ctx->str--;
						ctx->len--;
						st = 2;
						break;
					case TOKEN_OBJECT_END: /* end */
						if (ctx->call_end(JSON_OBJECT_TYPE,
								ctx->arg))
							return -2;
						ctx->str++;
						ctx->len++;
						return 0;
					default:
						ctx->err = JSON_ERR_OBJECT_TOKEN;
						return -1;
				}
				break;
			case 2: /* value */
				switch (_json_token(c)) {
					case TOKEN_COMMENT:
						k = _json_comment(ctx);
						if (k < 0) {
							ctx->err = JSON_ERR_COMMENT;
							return -1;
						}
						ctx->str--;
						ctx->len--;
						break;
					case TOKEN_VALUE:
						st = 3;
						break;
					default:
						ctx->err = JSON_ERR_OBJECT_VALUE;
						return -1;
				}
				break;
			case 3:
				if (_json_token(c) == TOKEN_COMMENT) {
					k = _json_comment(ctx);
					if (k < 0) {
						ctx->err = JSON_ERR_COMMENT;
						return -1;
					}
					ctx->str--;
					ctx->len--;
					break;
				}

				switch (_json_token(c)) {
					case TOKEN_STRING:
						k = _json_string(ctx);
						if (k < 0) {
							ctx->err = JSON_ERR_OBJECT_STRING;
							return -1;
						}
						if (ctx->call(JSON_STRING_TYPE,
								ctx->str - k + 1,
								k - 2,
								ctx->arg))
							return -2;
						break;
					case TOKEN_NUMBER:
						k = _json_number(ctx);
						if (k < 0) {
							ctx->err = JSON_ERR_OBJECT_NUMBER;
							return -1;
						}
						if (ctx->call(JSON_NUMBER_TYPE,
								ctx->str - k,
								k,
								ctx->arg))
							return -2;
						break;
					case TOKEN_NULL:
						k = _json_null(ctx);
						if (k < 0) {
							ctx->err = JSON_ERR_OBJECT_TOKEN;
							return -1;
						}
						if (ctx->call(JSON_NULL_TYPE,
								NULL,
								0,
								ctx->arg))
							return -2;
						break;
					case TOKEN_TRUE:
						k = _json_true(ctx);
						if (k < 0) {
							ctx->err = JSON_ERR_OBJECT_TOKEN;
							return -1;
						}
						if (ctx->call(JSON_TRUE_TYPE,
								NULL,
								0,
								ctx->arg))
							return -2;
						break;
					case TOKEN_FALSE:
						k = _json_false(ctx);
						if (k < 0) {
							ctx->err = JSON_ERR_OBJECT_TOKEN;
							return -1;
						}
						if (ctx->call(JSON_FALSE_TYPE,
								NULL,
								0,
								ctx->arg))
							return -2;
						break;
					case TOKEN_OBJECT: /* object */
						k = _json_object(ctx);
						if (k < 0)
							return k;
						break;
					case TOKEN_ARRAY: /* array */
						k = _json_array(ctx);
						if (k < 0)
							return k;
						break;
					default:
						ctx->err = JSON_ERR_OBJECT_TOKEN;
						return -1;
				}
				ctx->str--;
				ctx->len--;
				st = 4;
				break;
			case 4: /* next or end */
				switch (_json_token(c)) {
					case TOKEN_COMMENT:
						k = _json_comment(ctx);
						if (k < 0) {
							ctx->err = JSON_ERR_COMMENT;
							return -1;
						}
						ctx->str--;
						ctx->len--;
						break;
					case TOKEN_NEXT:
						st = 1;
						break;
					case TOKEN_OBJECT_END:
						if (ctx->call_end(JSON_OBJECT_TYPE,
								ctx->arg))
							return -2;
						ctx->str++;
						ctx->len++;
						return 0;
					default:
						ctx->err = JSON_ERR_OBJECT_END;
						return -1;
				}
				break;
			default:
				return -1;
		}
	}

	return -1;
}

/* @func: _json_array (static)
 * #desc:
 *    parse json array.
 *
 * #1: json struct context
 * #r: 0: no error, -1: error, -2: call error
 */
static int32_t _json_array(struct json_ctx *ctx)
{
	int32_t st = 0, k = 0;
	for (; *(ctx->str) != '\0'; ctx->str++, ctx->len++) {
		char c = *(ctx->str);
		if (SKIP_CHAR(c))
			continue;
		if (INVALID_CHAR(c)) {
			ctx->err = JSON_ERR_INVALID;
			return -1;
		}

		switch (st) {
			case 0: /* start */
				if (ctx->call(JSON_ARRAY_TYPE,
						NULL,
						0,
						ctx->arg))
					return -2;
				st = 1;
				break;
			case 1:
				if (_json_token(c) == TOKEN_COMMENT) {
					k = _json_comment(ctx);
					if (k < 0) {
						ctx->err = JSON_ERR_COMMENT;
						return -1;
					}
					ctx->str--;
					ctx->len--;
					break;
				}

				switch (_json_token(c)) {
					case TOKEN_STRING:
						k = _json_string(ctx);
						if (k < 0) {
							ctx->err = JSON_ERR_ARRAY_STRING;
							return -1;
						}
						if (ctx->call(JSON_STRING_TYPE,
								ctx->str - k + 1,
								k - 2,
								ctx->arg))
							return -2;
						break;
					case TOKEN_NUMBER:
						k = _json_number(ctx);
						if (k < 0) {
							ctx->err = JSON_ERR_ARRAY_NUMBER;
							return -1;
						}
						if (ctx->call(JSON_NUMBER_TYPE,
								ctx->str - k,
								k,
								ctx->arg))
							return -2;
						break;
					case TOKEN_NULL:
						k = _json_null(ctx);
						if (k < 0) {
							ctx->err = JSON_ERR_ARRAY_TOKEN;
							return -1;
						}
						if (ctx->call(JSON_NULL_TYPE,
								NULL,
								0,
								ctx->arg))
							return -2;
						break;
					case TOKEN_TRUE:
						k = _json_true(ctx);
						if (k < 0) {
							ctx->err = JSON_ERR_ARRAY_TOKEN;
							return -1;
						}
						if (ctx->call(JSON_TRUE_TYPE,
								NULL,
								0,
								ctx->arg))
							return -2;
						break;
					case TOKEN_FALSE:
						k = _json_false(ctx);
						if (k < 0) {
							ctx->err = JSON_ERR_ARRAY_TOKEN;
							return -1;
						}
						if (ctx->call(JSON_FALSE_TYPE,
								NULL,
								0,
								ctx->arg))
							return -2;
						break;
					case TOKEN_OBJECT: /* object */
						k = _json_object(ctx);
						if (k < 0)
							return k;
						break;
					case TOKEN_ARRAY: /* array */
						k = _json_array(ctx);
						if (k < 0)
							return k;
						break;
					case TOKEN_ARRAY_END: /* end */
						if (ctx->call_end(JSON_ARRAY_TYPE,
								ctx->arg))
							return -2;
						ctx->str++;
						ctx->len++;
						return 0;
					default:
						ctx->err = JSON_ERR_ARRAY_TOKEN;
						return -1;
				}
				ctx->str--;
				ctx->len--;
				st = 2;
				break;
			case 2: /* next or end */
				switch (_json_token(c)) {
					case TOKEN_COMMENT:
						k = _json_comment(ctx);
						if (k < 0) {
							ctx->err = JSON_ERR_COMMENT;
							return -1;
						}
						ctx->str--;
						ctx->len--;
						break;
					case TOKEN_NEXT:
						st = 1;
						break;
					case TOKEN_ARRAY_END:
						if (ctx->call_end(JSON_ARRAY_TYPE,
								ctx->arg))
							return -2;
						ctx->str++;
						ctx->len++;
						return 0;
					default:
						ctx->err = JSON_ERR_ARRAY_END;
						return -1;
				}
				break;
			default:
				return -1;
		}
	}

	return -1;
}

/* @func: json_parse
 * #desc:
 *    json (javascript object notation) parser.
 *
 * #1: json struct context
 * #2: input buffer
 * #r: 0: no error, -1: error, -2: call error
 */
int32_t F_SYMBOL(json_parse)(struct json_ctx *ctx, const char *s)
{
	ctx->str = s;
	ctx->len = 0;
	ctx->err = 0;

	int32_t k = 0;
	for (; *(ctx->str) != '\0'; ctx->str++, ctx->len++) {
		char c = *(ctx->str);
		if (SKIP_CHAR(c))
			continue;
		if (INVALID_CHAR(c)) {
			ctx->err = JSON_ERR_INVALID;
			return -1;
		}

		switch (_json_token(c)) {
			case TOKEN_COMMENT:
				k = _json_comment(ctx);
				if (k < 0) {
					ctx->err = JSON_ERR_COMMENT;
					return -1;
				}
				ctx->str--;
				ctx->len--;
				break;
			case TOKEN_OBJECT:
				return _json_object(ctx);
			case TOKEN_ARRAY:
				return _json_array(ctx);
			default:
				ctx->err = JSON_ERR_START_TOKEN;
				return -1;
		}
	}

	return 0;
}
