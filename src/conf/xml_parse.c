/* @file: xml_parse.c
 * #desc:
 *    The implementations of xml (extensible markup language) parser.
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
#include <demoz/conf/xml_parse.h>


/* @def: _
 * NOTE: not support DTD
 *
 * <?xml attr="value"?>
 * <? xml attr="value" ?>
 * <!DOCTYPE name "string">
 * <!-- comment -->
 * < element attr="value">
 *   <element
 *   attr="value"/> <!-- comment -->
 *   <![CDATA[
 *       & <!-- <xml>>&<</xml> --> &
 *       []]
 *   ]]>
 * <
 * / element>
 * <element /> <!-- comment -->
 * <xml>1<!-- -->2<xml>4<!-- -->5<!-- -->6<xml>78</xml></xml><!--
 * --><![CDATA[..]]><!-- -->2<![CDATA[...]]><!-- -->0<xml><!-- --></xml></xml>
 *
 * <?xml version="1.0" encoding="utf8"?>
 * <!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN"
 *   "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
 * <html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en">
 *   <head>
 *     <title>Title</title>
 *   </head>
 *   <body>
 *     ...
 *   </body>
 * </html>
 */

enum {
	TOKEN_STATEMENT = 1,
	TOKEN_COMMENT,
	TOKEN_DOCTYPE,
	TOKEN_CDATA,
	TOKEN_ELEMENT,
	TOKEN_ELEMENT_END
};

/* [A-Za-z0-9\x80-\xff_.:-] */
static const uint8_t xml_name_table[256] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
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

#define SKIP_CHAR(x) ((x) == ' ' || (x) == '\n' || (x) == '\t')
#define INVALID_CHAR(x) ((uint8_t)(x) < 0x20)
/* end */

/* @func: _xml_token (static)
 * #desc:
 *    get the xml token.
 *
 * #1: input string
 * #r: token value (-1: unknown token)
 */
static int32_t _xml_token(const char *s)
{
	const char *p;
	char c;

	if (*s != '<')
		return -1;

	/* processing instruction */
	if (!C_SYMBOL(strncmp)("<?", s, 2))
		return TOKEN_STATEMENT;

	/* comment */
	if (!C_SYMBOL(strncmp)("<!--", s, 4))
		return TOKEN_COMMENT;

	/* document type */
	if (!C_SYMBOL(strncmp)("<!DOCTYPE", s, 9)) {
		c = *(s + 9);
		if (c == '>' || SKIP_CHAR(c))
			return TOKEN_DOCTYPE;
		return -1;
	}

	/* CDATA block */
	if (!C_SYMBOL(strncmp)("<![CDATA[", s, 9))
		return TOKEN_CDATA;

	/* end-element */
	p = s;
	c = *p++;
	while (c) {
		c = *p++;
		if (SKIP_CHAR(c))
			continue;
		if (c != '/')
			break;
		return TOKEN_ELEMENT_END;
	}

	/* element */
	p = s;
	c = *p++;
	while (c) {
		c = *p++;
		if (SKIP_CHAR(c))
			return TOKEN_ELEMENT;
		if (xml_name_table[(uint8_t)c])
			break;
		return TOKEN_ELEMENT;
	}

	return -1;
}

/* @func: _xml_string (static)
 * #desc:
 *    get the length of the xml string.
 *
 * #1: xml struct context
 * #r: string length (-1: error)
 */
static int32_t _xml_string(struct xml_ctx *ctx)
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

/* @func: _xml_comment (static)
 * #desc:
 *    get the length of the xml comment string.
 *
 * #1: xml struct context
 * #r: string length (-1: error)
 */
static int32_t _xml_comment(struct xml_ctx *ctx)
{
	int32_t st = 0, len = ctx->len;
	for (; *(ctx->str) != '\0'; ctx->str++, ctx->len++) {
		char c = *(ctx->str);
		switch (st) {
			case 0: /* start */
				ctx->str += 3;
				ctx->len += 3;
				st = 1;
				break;
			case 1: /* end */
				if (c != '-')
					break;
				if (C_SYMBOL(strncmp)("-->", ctx->str, 3))
					break;

				ctx->str += 3;
				ctx->len += 3;
				return ctx->len - len;
			default:
				return -1;
		}
	}

	return -1;
}

/* @func: _xml_cdata (static)
 * #desc:
 *    get the length of the xml character data.
 *
 * #1: xml struct context
 * #r: string length (-1: error)
 */
static int32_t _xml_cdata(struct xml_ctx *ctx)
{
	int32_t st = 0, len = ctx->len;
	for (; *(ctx->str) != '\0'; ctx->str++, ctx->len++) {
		char c = *(ctx->str);
		switch (st) {
			case 0: /* start */
				ctx->str += 8;
				ctx->len += 8;
				st = 1;
				break;
			case 1: /* end */
				if (c != ']')
					break;
				if (C_SYMBOL(strncmp)("]]>", ctx->str, 3))
					break;

				ctx->str += 3;
				ctx->len += 3;
				return ctx->len - len;
			default:
				return -1;
		}
	}

	return -1;
}

/* @func: _xml_statement_attr (static)
 * #desc:
 *    parsing xml statement attributes.
 *
 * #1: xml struct context
 * #r: 0: no error, -1: error, -2: call error
 */
static int32_t _xml_statement_attr(struct xml_ctx *ctx)
{
	int32_t st = 0, k = 0, len = 0;
	for (; *(ctx->str) != '\0'; ctx->str++, ctx->len++) {
		char c = *(ctx->str);
		switch (st) {
			case 0: /* name start, or end */
				if (SKIP_CHAR(c))
					break;

				/* end */
				if (!C_SYMBOL(strncmp)("?>", ctx->str, 2)) {
					ctx->str += 2;
					ctx->len += 2;
					return 0;
				}

				/* invalid character */
				if (xml_name_table[(uint8_t)c]) {
					ctx->err = XML_ERR_STATEMENT_ATTR_CHAR;
					return -1;
				}
				switch (c) {
					case '-': case '.': case ':':
					case '0': case '1': case '2': case '3':
					case '4': case '5': case '6': case '7':
					case '8': case '9':
						ctx->err = XML_ERR_STATEMENT_ATTR_CHAR;
						return -1;
					default:
						break;
				}
				len++;
				st = 1;
				break;
			case 1: /* name */
				if (c == '=') { /* value */
					if (ctx->call(XML_STATEMENT_ATTR_NAME,
							ctx->str - len,
							len,
							ctx->arg))
						return -2;
					st = 2;
					break;
				}

				/* invalid character */
				if (xml_name_table[(uint8_t)c]) {
					ctx->err = XML_ERR_STATEMENT_ATTR_CHAR;
					return -1;
				}
				len++;
				break;
			case 2: /* value and next */
				k = _xml_string(ctx);
				if (k < 0) {
					ctx->err = XML_ERR_STATEMENT_ATTR_VALUE;
					return -1;
				}
				if (ctx->call(XML_STATEMENT_ATTR_VALUE,
						ctx->str - k + 1,
						k - 2,
						ctx->arg))
					return -2;

				/* next */
				ctx->str--;
				ctx->len--;
				len = st = 0;
				break;
			default:
				return -1;
		}
	}

	return -1;
}

/* @func: _xml_statement (static)
 * #desc:
 *    parsing xml statement.
 *
 * #1: xml struct context
 * #r: 0: no error, -1: error, -2: call error
 */
static int32_t _xml_statement(struct xml_ctx *ctx)
{
	int32_t st = 0, k = 0, len = 0;
	for (; *(ctx->str) != '\0'; ctx->str++, ctx->len++) {
		char c = *(ctx->str);
		switch (st) {
			case 0: /* start */
				ctx->str++;
				ctx->len++;
				st = 1;
				break;
			case 1: /* name start, or end */
				if (SKIP_CHAR(c))
					break;

				/* end */
				if (!C_SYMBOL(strncmp)("?>", ctx->str, 2)) {
					ctx->str += 2;
					ctx->len += 2;
					return 0;
				}

				/* invalid character */
				if (xml_name_table[(uint8_t)c]) {
					ctx->err = XML_ERR_STATEMENT_CHAR;
					return -1;
				}
				switch (c) {
					case '-': case '.': case ':':
					case '0': case '1': case '2': case '3':
					case '4': case '5': case '6': case '7':
					case '8': case '9':
						ctx->err = XML_ERR_STATEMENT_CHAR;
						return -1;
					default:
						break;
				}
				len++;
				st = 2;
				break;
			case 2: /* name, attribute or end */
				if (SKIP_CHAR(c)) { /* attribute */
					if (ctx->call(XML_STATEMENT_START,
							ctx->str - len,
							len,
							ctx->arg))
						return -2;
					st = 3;
					break;
				}

				/* end */
				if (!C_SYMBOL(strncmp)("?>", ctx->str, 2)) {
					if (ctx->call(XML_STATEMENT_START,
							ctx->str - len,
							len,
							ctx->arg))
						return -2;
					if (ctx->call_end(XML_STATEMENT_END,
							NULL,
							0,
							ctx->arg))
						return -2;
					ctx->str += 2;
					ctx->len += 2;
					return 0;
				}

				/* invalid character */
				if (xml_name_table[(uint8_t)c]) {
					ctx->err = XML_ERR_STATEMENT_CHAR;
					return -1;
				}
				len++;
				break;
			case 3: /* attribute and end */
				k = _xml_statement_attr(ctx);
				if (k < 0)
					return k;

				/* end */
				if (ctx->call_end(XML_STATEMENT_END,
						NULL,
						0,
						ctx->arg))
					return -2;
				return 0;
			default:
				return -1;
		}
	}

	return -1;
}

/* @func: _xml_doctype (static)
 * #desc:
 *    parsing xml doctype.
 *
 * #1: xml struct context
 * #r: 0: no error, -1: error, -2: call error
 */
static int32_t _xml_doctype(struct xml_ctx *ctx)
{
	int32_t st = 0, k = 0, len = 0;
	for (; *(ctx->str) != '\0'; ctx->str++, ctx->len++) {
		char c = *(ctx->str);
		switch (st) {
			case 0: /* start or end */
				if (ctx->call(XML_DOCTYPE_START,
						NULL,
						0,
						ctx->arg))
					return -2;
				ctx->str += 9;
				ctx->len += 9;
				c = *(ctx->str);

				if (c == '>') { /* end */
					if (ctx->call_end(XML_DOCTYPE_END,
							NULL,
							0,
							ctx->arg))
						return -2;
					ctx->str++;
					ctx->len++;
					return 0;
				}

				st = 1;
				break;
			case 1: /* name start, string or end */
				if (SKIP_CHAR(c))
					break;

				if (c == '>') { /* end */
					if (ctx->call_end(XML_DOCTYPE_END,
							NULL,
							0,
							ctx->arg))
						return -2;
					ctx->str++;
					ctx->len++;
					return 0;
				}

				/* string */
				if (c == '"' || c == '\'') {
					k = _xml_string(ctx);
					if (k < 0) {
						ctx->err = XML_ERR_DOCTYPE_STRING;
						return -1;
					}
					if (ctx->call(XML_DOCTYPE_STRING,
							ctx->str - k + 1,
							k - 2,
							ctx->arg))
						return -2;
					ctx->str--;
					ctx->len--;
					st = 3;
					break;
				}

				/* invalid character */
				if (xml_name_table[(uint8_t)c]) {
					ctx->err = XML_ERR_DOCTYPE_CHAR;
					return -1;
				}
				switch (c) {
					case '-': case '.': case ':':
					case '0': case '1': case '2': case '3':
					case '4': case '5': case '6': case '7':
					case '8': case '9':
						ctx->err = XML_ERR_DOCTYPE_CHAR;
						return -1;
					default:
						break;
				}
				len++;
				st = 2;
				break;
			case 2: /* name, next or end */
				if (SKIP_CHAR(c)) { /* next */
					if (ctx->call(XML_DOCTYPE_NAME,
							ctx->str - len,
							len,
							ctx->arg))
						return -2;
					st = 3;
					break;
				}

				if (c == '>') { /* end */
					if (ctx->call(XML_DOCTYPE_NAME,
							ctx->str - len,
							len,
							ctx->arg))
						return -2;
					if (ctx->call_end(XML_DOCTYPE_END,
							NULL,
							0,
							ctx->arg))
						return -2;
					ctx->str++;
					ctx->len++;
					return 0;
				}

				/* invalid character */
				if (xml_name_table[(uint8_t)c]) {
					ctx->err = XML_ERR_DOCTYPE_CHAR;
					return -1;
				}
				len++;
				break;
			case 3: /* next or end */
				if (SKIP_CHAR(c))
					break;

				if (c == '>') { /* end */
					if (ctx->call_end(XML_DOCTYPE_END,
							NULL,
							0,
							ctx->arg))
						return -2;
					ctx->str++;
					ctx->len++;
					return 0;
				}

				/* next */
				ctx->str--;
				ctx->len--;
				len = 0;
				st = 1;
				break;
			default:
				return -1;
		}
	}

	return -1;
}

/* @func: _xml_element_attr (static)
 * #desc:
 *    parsing xml element attributes.
 *
 * #1: xml struct context
 * #r: 0: no error, 1: empty element, -1: error, -2: call error
 */
static int32_t _xml_element_attr(struct xml_ctx *ctx)
{
	int32_t st = 0, k = 0, len = 0;
	for (; *(ctx->str) != '\0'; ctx->str++, ctx->len++) {
		char c = *(ctx->str);
		switch (st) {
			case 0: /* name start, or end */
				if (SKIP_CHAR(c))
					break;

				if (c == '>') /* end */
					return 0;
				if (c == '/') /* empty */
					return 1;

				/* invalid character */
				if (xml_name_table[(uint8_t)c]) {
					ctx->err = XML_ERR_ELEMENT_ATTR_CHAR;
					return -1;
				}
				switch (c) {
					case '-': case '.': case ':':
					case '0': case '1': case '2': case '3':
					case '4': case '5': case '6': case '7':
					case '8': case '9':
						ctx->err = XML_ERR_ELEMENT_ATTR_CHAR;
						return -1;
					default:
						break;
				}
				len++;
				st = 1;
				break;
			case 1: /* name */
				if (c == '=') { /* value */
					if (ctx->call(XML_ELEMENT_ATTR_NAME,
							ctx->str - len,
							len,
							ctx->arg))
						return -2;
					st = 2;
					break;
				}

				/* invalid character */
				if (xml_name_table[(uint8_t)c]) {
					ctx->err = XML_ERR_ELEMENT_ATTR_CHAR;
					return -1;
				}
				len++;
				break;
			case 2: /* value and next */
				k = _xml_string(ctx);
				if (k < 0) {
					ctx->err = XML_ERR_ELEMENT_ATTR_VALUE;
					return -1;
				}
				if (ctx->call(XML_ELEMENT_ATTR_VALUE,
						ctx->str - k + 1,
						k - 2,
						ctx->arg))
					return -2;

				/* next */
				ctx->str--;
				ctx->len--;
				len = st = 0;
				break;
			default:
				return -1;
		}
	}

	return -1;
}

/* @func: _xml_element_start (static)
 * #desc:
 *    parsing xml element.
 *
 * #1: xml struct context
 * #r: 0: no error, 1: empty element, -1: error, -2: call error
 */
static int32_t _xml_element_start(struct xml_ctx *ctx)
{
	int32_t st = 0, k = 0, len = 0;
	for (; *(ctx->str) != '\0'; ctx->str++, ctx->len++) {
		char c = *(ctx->str);
		switch (st) {
			case 0: /* start */
				st = 1;
				break;
			case 1: /* name start */
				if (SKIP_CHAR(c))
					break;

				/* invalid character */
				if (xml_name_table[(uint8_t)c]) {
					ctx->err = XML_ERR_ELEMENT_CHAR;
					return -1;
				}
				switch (c) {
					case '-': case '.': case ':':
					case '0': case '1': case '2': case '3':
					case '4': case '5': case '6': case '7':
					case '8': case '9':
						ctx->err = XML_ERR_ELEMENT_CHAR;
						return -1;
					default:
						break;
				}
				len++;
				st = 2;
				break;
			case 2: /* name, attribute, empty or end */
				if (SKIP_CHAR(c)) { /* attribute */
					if (ctx->call(XML_ELEMENT_START,
							ctx->str - len,
							len,
							ctx->arg))
						return -2;
					st = 3;
					break;
				}

				if (c == '>') { /* end */
					if (ctx->call(XML_ELEMENT_START,
							ctx->str - len,
							len,
							ctx->arg))
						return -2;
					ctx->str++;
					ctx->len++;
					return 0;
				}
				if (c == '/') { /* empty */
					if (ctx->call(XML_ELEMENT_START,
							ctx->str - len,
							len,
							ctx->arg))
						return -2;
					st = 4;
					break;
				}

				/* invalid character */
				if (xml_name_table[(uint8_t)c]) {
					ctx->err = XML_ERR_ELEMENT_CHAR;
					return -1;
				}
				len++;
				break;
			case 3: /* attribute, empty or end */
				k = _xml_element_attr(ctx);
				if (k < 0)
					return k;
				if (k == 1) { /* empty */
					st = 4;
					break;
				}

				/* end */
				ctx->str++;
				ctx->len++;
				return 0;
			case 4: /* empty element end */
				if (SKIP_CHAR(c))
					break;

				if (c != '>') {
					ctx->err = XML_ERR_ELEMENT_EMPTY_END;
					return -1;
				}

				/* empty end */
				if (ctx->call_end(XML_ELEMENT_EMPTY_END,
						NULL,
						0,
						ctx->arg))
					return -2;
				ctx->str++;
				ctx->len++;
				return 1;
			default:
				return -1;
		}
	}

	return -1;
}

/* @func: _xml_element_end (static)
 * #desc:
 *    parsing xml end-element.
 *
 * #1: xml struct context
 * #r: 0: no error, -1: error, -2: call error
 */
static int32_t _xml_element_end(struct xml_ctx *ctx)
{
	int32_t st = 0, len = 0;
	for (; *(ctx->str) != '\0'; ctx->str++, ctx->len++) {
		char c = *(ctx->str);
		switch (st) {
			case 0: /* start */
				st = 1;
				break;
			case 1:
				if (SKIP_CHAR(c))
					break;
				st = 2;
				break;
			case 2: /* name start */
				if (SKIP_CHAR(c))
					break;

				/* invalid character */
				if (xml_name_table[(uint8_t)c]) {
					ctx->err = XML_ERR_ELEMENT_END_CHAR;
					return -1;
				}
				switch (c) {
					case '-': case '.': case ':':
					case '0': case '1': case '2': case '3':
					case '4': case '5': case '6': case '7':
					case '8': case '9':
						ctx->err = XML_ERR_ELEMENT_END_CHAR;
						return -1;
					default:
						break;
				}
				len++;
				st = 3;
				break;
			case 3: /* name or end */
				if (SKIP_CHAR(c)) { /* end */
					st = 4;
					break;
				}

				if (c == '>') { /* end */
					if (ctx->call_end(XML_ELEMENT_END,
							ctx->str - len,
							len,
							ctx->arg))
						return -2;
					ctx->str++;
					ctx->len++;
					return 0;
				}

				/* invalid character */
				if (xml_name_table[(uint8_t)c]) {
					ctx->err = XML_ERR_ELEMENT_END_CHAR;
					return -1;
				}
				len++;
				break;
			case 4: /* end */
				if (SKIP_CHAR(c))
					break;

				/* invalid character */
				if (c != '>') {
					ctx->err = XML_ERR_ELEMENT_END_CHAR;
					return -1;
				}

				/* end */
				if (ctx->call_end(XML_ELEMENT_END,
						ctx->str - len,
						len,
						ctx->arg))
					return -2;
				ctx->str++;
				ctx->len++;
				return 0;
			default:
				return -1;
		}
	}

	return -1;
}

/* @func: _xml_element (static)
 * #desc:
 *    parsing xml element.
 *
 * #1: xml struct context
 * #r: 0: no error, -1: error, -2: call error
 */
static int32_t _xml_element(struct xml_ctx *ctx)
{
	int32_t st = 0, k = 0, len = 0;
	for (; *(ctx->str) != '\0'; ctx->str++, ctx->len++) {
		char c = *(ctx->str);
		switch (st) {
			case 0: /* start */
				k = _xml_element_start(ctx);
				if (k < 0)
					return k;
				if (k == 1) /* empty */
					return 0;

				/* element head end */
				if (ctx->call(XML_ELEMENT_HEAD,
						NULL,
						0,
						ctx->arg))
					return -2;

				ctx->str--;
				ctx->len--;
				st = 1;
				break;
			case 1: /* element */
				if (c != '<') {
					len++;
					break;
				}

				/* string */
				if (ctx->call(XML_ELEMENT_STRING,
						ctx->str - len,
						len,
						ctx->arg))
					return -2;

				/* embed */
				switch (_xml_token(ctx->str)) {
					case TOKEN_COMMENT:
						k = _xml_comment(ctx);
						if (k < 0) {
							ctx->err = XML_ERR_COMMENT;
							return k;
						}
						if (ctx->call(XML_COMMENT,
								ctx->str - k + 4,
								k - 4 - 3,
								ctx->arg))
							return -2;
						break;
					case TOKEN_CDATA:
						k = _xml_cdata(ctx);
						if (k < 0) {
							ctx->err = XML_ERR_CDATA;
							return k;
						}
						if (ctx->call(XML_CDATA,
								ctx->str - k + 9,
								k - 9 - 3,
								ctx->arg))
							return -2;
						break;
					case TOKEN_ELEMENT: /* element */
						k = _xml_element(ctx);
						if (k < 0)
							return k;
						break;
					case TOKEN_ELEMENT_END: /* end */
						return _xml_element_end(ctx);
					default:
						ctx->err = XML_ERR_TOKEN;
						return -1;
				}
				len = 0;
				ctx->str--;
				ctx->len--;
				break;
			default:
				return -1;
		}
	}

	return -1;
}

/* @func: xml_parse
 * #desc:
 *    xml (extensible markup language) parser.
 *
 * #1: xml struct context
 * #2: input buffer
 * #r: 0: no error, -1: error, -2: call error
 */
int32_t F_SYMBOL(xml_parse)(struct xml_ctx *ctx, const char *s)
{
	ctx->str = s;
	ctx->len = 0;
	ctx->err = 0;

	int32_t k = 0;
	for (; *(ctx->str) != '\0'; ctx->str++, ctx->len++) {
		char c = *(ctx->str);
		if (SKIP_CHAR(c))
			continue;
		if (INVALID_CHAR(c))
			return -1;

		switch (_xml_token(ctx->str)) {
			case TOKEN_STATEMENT:
				k = _xml_statement(ctx);
				if (k < 0)
					return k;
				break;
			case TOKEN_COMMENT:
				k = _xml_comment(ctx);
				if (k < 0) {
					ctx->err = XML_ERR_COMMENT;
					return k;
				}
				if (ctx->call(XML_COMMENT,
						ctx->str - k + 4,
						k - 4 - 3,
						ctx->arg))
					return -2;
				break;
			case TOKEN_DOCTYPE:
				k = _xml_doctype(ctx);
				if (k < 0)
					return k;
				break;
			case TOKEN_ELEMENT:
				k = _xml_element(ctx);
				if (k < 0)
					return k;
				break;
			default:
				ctx->err = XML_ERR_TOKEN;
				return -1;
		}
		ctx->str--;
		ctx->len--;
	}

	return 0;
}
