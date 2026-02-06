/* @file: stdio_sscanf.c
 * #desc:
 *    The implementations of standard input/output.
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
#include <demoz/c/stdio.h>


/* @func: vsscanf
 * #desc:
 *    string convert to formatted value.
 *
 * #1: input string
 * #2: formatted string
 * #3: variable argument
 * #r: >0: number of matching, <0: format error
 */
int32_t C_SYMBOL(vsscanf)(const char *s, const char *fmt, va_list ap)
{
	return C_SYMBOL(___scanf)(s, NULL, fmt, ap);
}

/* @func: sscanf
 * #desc:
 *    string convert to formatted value.
 *
 * #1: input string
 * #2: formatted string
 * #N: variable argument
 * #r: >0: number of matching, <0: format error
 */
int32_t C_SYMBOL(sscanf)(const char *s, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);

	return C_SYMBOL(vsscanf)(s, fmt, ap);
}
