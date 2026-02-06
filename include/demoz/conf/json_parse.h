/* @file: json_parse.h
 * #desc:
 *    The definitions of json (javascript object notation) parser.
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

#ifndef _DEMOZ_CONF_JSON_PARSE_H
#define _DEMOZ_CONF_JSON_PARSE_H

#include <demoz/config.h>
#include <demoz/c/stdint.h>


/* @def: _ */
#define JSON_ARRAY_TYPE 1
#define JSON_OBJECT_TYPE 2
#define JSON_VALUE_TYPE 3
#define JSON_STRING_TYPE 4
#define JSON_NUMBER_TYPE 5
#define JSON_NULL_TYPE 6
#define JSON_TRUE_TYPE 7
#define JSON_FALSE_TYPE 8

/* json error code */
#define JSON_ERR_INVALID 1
#define JSON_ERR_START_TOKEN 2
#define JSON_ERR_OBJECT_TOKEN 3
#define JSON_ERR_OBJECT_END 4
#define JSON_ERR_OBJECT_VALUE 5
#define JSON_ERR_OBJECT_STRING 6
#define JSON_ERR_OBJECT_NUMBER 7
#define JSON_ERR_ARRAY_TOKEN 8
#define JSON_ERR_ARRAY_END 9
#define JSON_ERR_ARRAY_STRING 10
#define JSON_ERR_ARRAY_NUMBER 11
#define JSON_ERR_COMMENT 12

struct json_ctx {
	const char *str;
	int32_t len;
	int32_t err;
	void *arg;
	/* type, string, length, arg */
	int32_t (*call)(int32_t, const char *, int32_t, void *);
	/* type, arg */
	int32_t (*call_end)(int32_t, void *);
};

#define JSON_NEW(name, _call, _call_end, _arg) \
	struct json_ctx name = { \
		.call = _call, \
		.call_end = _call_end, \
		.arg = _arg \
	}

#define JSON_INIT(name, _call, _call_end, _arg) \
	(name)->call = _call; \
	(name)->call_end = _call_end; \
	(name)->arg = _arg

#define JSON_STR(x) ((x)->str)
#define JSON_ERR(x) ((x)->err)
#define JSON_LEN(x) ((x)->len)
/* end */


#ifdef __cplusplus
extern "C" {
#endif

/* conf/json_parse.c */

extern
int32_t F_SYMBOL(json_parse)(struct json_ctx *ctx, const char *s)
;

#ifdef __cplusplus
}
#endif


#endif
