/* @file: ini_parse.h
 * #desc:
 *    The definitions of ini (initial configuration) parser.
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

#ifndef _DEMOZ_CONF_INI_PARSE_H
#define _DEMOZ_CONF_INI_PARSE_H

#include <demoz/config.h>
#include <demoz/c/stdint.h>


/* @def: _ */
#define INI_SECTION_TYPE 1
#define INI_KEY_TYPE 2
#define INI_VALUE_TYPE 3

/* error code */
#define INI_ERR_INVALID 1

#define INI_ERR_SECTION_NAME 2
#define INI_ERR_SECTION_END 3

#define INI_ERR_KEY_NAME 4
#define INI_ERR_KEY_VALUE 5
#define INI_ERR_VALUE_NAME 6

struct ini_ctx {
	const char *str;
	int32_t len;
	int32_t err;
	void *arg;
	/* type, string, length, arg */
	int32_t (*call)(int32_t, const char *, int32_t, void *);
	/* arg */
	int32_t (*call_end)(void *);
};

#define INI_NEW(name, _call, _call_end, _arg) \
	struct ini_ctx name = { \
		.call = _call, \
		.call_end = _call_end, \
		.arg = _arg \
		}

#define INI_INIT(x, _call, _call_end, _arg) \
	(x)->call = _call; \
	(x)->call_end = _call_end; \
	(x)->arg = _arg

#define INI_ERR(x) ((x)->err)
#define INI_LEN(x) ((x)->len)
/* end */


#ifdef __cplusplus
extern "C" {
#endif

/* conf/ini_parse.c */

extern
int32_t F_SYMBOL(ini_parse)(struct ini_ctx *ctx, const char *s)
;

#ifdef __cplusplus
}
#endif


#endif
