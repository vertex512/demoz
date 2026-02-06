/* @file: xml_parse.h
 * #desc:
 *    The definitions of xml (extensible markup language) parser.
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

#ifndef _DEMOZ_CONF_XML_PARSE_H
#define _DEMOZ_CONF_XML_PARSE_H

#include <demoz/config.h>
#include <demoz/c/stdint.h>


/* @def: */
#define XML_ELEMENT_START 1
#define XML_ELEMENT_ATTR_NAME 2
#define XML_ELEMENT_ATTR_VALUE 3
#define XML_ELEMENT_STRING 4
#define XML_ELEMENT_HEAD 5
#define XML_ELEMENT_EMPTY_END 6
#define XML_ELEMENT_END 7
#define XML_DOCTYPE_START 8
#define XML_DOCTYPE_NAME 9
#define XML_DOCTYPE_STRING 10
#define XML_DOCTYPE_END 11
#define XML_STATEMENT_START 12
#define XML_STATEMENT_ATTR_NAME 13
#define XML_STATEMENT_ATTR_VALUE 14
#define XML_STATEMENT_END 15
#define XML_COMMENT 16
#define XML_CDATA 17

/* xml error code */
#define XML_ERR_TOKEN 1
#define XML_ERR_ELEMENT_CHAR 2
#define XML_ERR_ELEMENT_EMPTY_END 3
#define XML_ERR_ELEMENT_ATTR_CHAR 4
#define XML_ERR_ELEMENT_ATTR_VALUE 5
#define XML_ERR_ELEMENT_END_CHAR 6
#define XML_ERR_DOCTYPE_CHAR 7
#define XML_ERR_DOCTYPE_STRING 8
#define XML_ERR_STATEMENT_CHAR 9
#define XML_ERR_STATEMENT_ATTR_CHAR 10
#define XML_ERR_STATEMENT_ATTR_VALUE 11
#define XML_ERR_COMMENT 12
#define XML_ERR_CDATA 13

struct xml_ctx {
	const char *str;
	int32_t len;
	int32_t err;
	void *arg;
	/* type, string, length, arg */
	int32_t (*call)(int32_t, const char *, int32_t, void *);
	/* type, string, length, arg */
	int32_t (*call_end)(int32_t, const char *, int32_t, void *);
};

#define XML_NEW(name, _call, _call_end, _arg) \
	struct xml_ctx name = { \
		.call = _call, \
		.call_end = _call_end, \
		.arg = _arg \
	}

#define XML_INIT(name, _call, _call_end, _arg) \
	(name)->call = _call; \
	(name)->call_end = _call_end; \
	(name)->arg = _arg

#define XML_ERR(name) ((name)->err)
#define XML_LEN(name) ((name)->len)
/* end */


#ifdef __cplusplus
extern "C" {
#endif

/* conf/xml_parse.c */

extern
int32_t F_SYMBOL(xml_parse)(struct xml_ctx *ctx, const char *s)
;

#ifdef __cplusplus
}
#endif


#endif
