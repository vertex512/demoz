/* @file: stdio_snprintf.c
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


/* @def: _ */
struct snprintf_ctx {
	char *p;
	char *e;
	size_t len;
};

static int32_t _call(const char *s, int32_t len, void *arg) {
	struct snprintf_ctx *ctx = arg;

	ctx->len += len;
	if (ctx->p < ctx->e) {
		if ((ctx->p + len) > ctx->e)
			len = (int32_t)(ctx->e - ctx->p);
		while (len--)
			*(ctx->p++) = *s++;
	}

	return 0;
}
/* end */

/* @func: vsnprintf
 * #desc:
 *    convert formatted output to buffer.
 *
 * #1: buf [out] output buffer
 * #2: len [in]  buffer length
 * #3: fmt [in]  format string
 * #4: ap  [in]  variable parameter
 * #r:     [ret] >=0: output length, -1: error
 */
int32_t C_SYMBOL(vsnprintf)(char *buf, size_t len, const char *fmt,
		va_list ap)
{
	if (!len)
		return 0;

	struct snprintf_ctx ctx = {
		.p = buf, .e = buf + len - 1,
		.len = 0
		};

	int32_t r = C_SYMBOL(___printf)(fmt, ap, &ctx, _call);
	if (!r) {
		*(ctx.p) = '\0';
		return (int32_t)ctx.len;
	}

	return r;
}

/* @func: snprintf
 * #desc:
 *    convert formatted output to buffer.
 *
 * #1: buf [out] output buffer
 * #2: len [in]  buffer length
 * #3: fmt [in]  format string
 * #N:     [in]  variable parameter
 * #r:     [ret] >=0: output length, -1: error
 */
int32_t C_SYMBOL(snprintf)(char *buf, size_t len, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);

	return C_SYMBOL(vsnprintf)(buf, len, fmt, ap);
}
