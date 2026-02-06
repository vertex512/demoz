/* @file: bits_get.h
 * #desc:
 *    The definitions of get bits from buffer.
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

#ifndef _DEMOZ_DS_BITS_GET_H
#define _DEMOZ_DS_BITS_GET_H

#include <demoz/config.h>
#include <demoz/c/stdint.h>


/* @def: _ */
#define BITS_GET_BUFSIZE 32

struct bits_get_ctx {
	uint8_t buf[BITS_GET_BUFSIZE];
	uint16_t len; /* buffer current length */
	uint16_t pos; /* buffer current position */
	uint16_t cur; /* current used bits of the buffer value */
};

#define BITS_GET_NEW(name) \
	struct bits_get_ctx name = { \
		.len = 0, .pos = 0, .cur = 0 \
		}

#define BITS_GET_INIT(x) \
	(x)->len = 0; \
	(x)->pos = 0; \
	(x)->cur = 0

/* skip the remaining bits in the byte */
#define BITS_GET_SKIP(x) \
	do { \
		(x)->pos += (x)->cur ? 1 : 0; \
		(x)->cur = 0; \
	} while (0)

#define BITS_GET_BUF(x) ((x)->buf)
#define BITS_GET_LEN(x) ((x)->len)
#define BITS_GET_POS(x) ((x)->pos)
#define BITS_GET_CUR(x) ((x)->cur)

#define BITS_GET_GETPOS(x) ((x)->pos + ((((x)->cur + 7) & ~7) >> 3))
#define BITS_GET_REMBUF(x) (BITS_GET_BUF(x) + BITS_GET_GETPOS(x))
#define BITS_GET_REMLEN(x) (BITS_GET_LEN(x) - BITS_GET_GETPOS(x))
/* end */


#ifdef __cplusplus
extern "C" {
#endif

/* ds/bits_get.c */

extern
uint32_t F_SYMBOL(bits_get_fill)(struct bits_get_ctx *ctx, const uint8_t *s,
		uint32_t len)
;

extern
int32_t F_SYMBOL(bits_get)(struct bits_get_ctx *ctx, uint32_t *v, uint32_t bits,
		int32_t peek)
;

#ifdef __cplusplus
}
#endif


#endif
