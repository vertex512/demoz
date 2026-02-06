/* @file: stdio.h
 * #desc:
 *    The definitions of standard input/output.
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

#ifndef _DEMOZ_C_STDIO_H
#define _DEMOZ_C_STDIO_H

#include <demoz/config.h>
#include <demoz/c/stdint.h>
#include <demoz/c/stdarg.h>


#ifdef __cplusplus
extern "C" {
#endif

/* c/stdio_printf.c */

extern
int32_t C_SYMBOL(___printf)(const char *fmt, va_list ap, void *arg,
		int32_t (*out)(const char *, int32_t, void *))
;

/* c/stdio_scanf.c */

extern
int32_t C_SYMBOL(___scanf)(const char *s, char **e, const char *fmt,
		va_list ap)
;

/* c/stdio_snprintf.c */

extern
int32_t C_SYMBOL(vsnprintf)(char *buf, size_t len, const char *fmt,
		va_list ap)
;

extern
int32_t C_SYMBOL(snprintf)(char *buf, size_t len, const char *fmt, ...)
;

/* c/stdio_sscanf.c */

extern
int32_t C_SYMBOL(vsscanf)(const char *s, const char *fmt, va_list ap)
;

extern
int32_t C_SYMBOL(sscanf)(const char *s, const char *fmt, ...)
;

#ifdef __cplusplus
}
#endif


#endif
