/* @file: bits_add.h
 * #desc:
 *    The definitions of adding bits to buffer.
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

#ifndef _DEMOZ_DS_BITS_ADD_H
#define _DEMOZ_DS_BITS_ADD_H

#include <demoz/config.h>
#include <demoz/c/stdint.h>


/* @def: _ */
#define BITS_ADD_BUFSIZE 16

struct bits_add_ctx {
	uint8_t buf[BITS_ADD_BUFSIZE];
	uint16_t size; /* buffer current size */
	uint16_t cur;  /* current bits of the buffer value */
	uint16_t rem;  /* remaining bits of the value */
};

#define BITS_ADD_NEW(name) \
	struct bits_add_ctx name = { \
		.size = 0, .cur = 0, .rem = 0 \
		}

#define BITS_ADD_INIT(x) \
	(x)->size = 0; \
	(x)->cur = 0; \
	(x)->rem = 0

#define BITS_ADD_FLUSH(x) \
	do { \
		(x)->size = 0; \
		(x)->cur = 0; \
	} while (0)

/* skip the remaining bits in the byte */
#define BITS_ADD_SKIP(x) \
	do { \
		(x)->size += (x)->cur ? 1 : 0; \
		(x)->cur = 0; \
	} while (0)

#define BITS_ADD_BUF(x) ((x)->buf)
#define BITS_ADD_SIZE(x) ((x)->size)
#define BITS_ADD_CUR(x) ((x)->cur)
#define BITS_ADD_REM(x) ((x)->rem)

#define BITS_ADD_GETSIZE(x) ((x)->size + ((((x)->cur + 7) & ~7) >> 3))
/* end */


#ifdef __cplusplus
extern "C" {
#endif

/* ds/bits_add.c */

extern
int32_t F_SYMBOL(bits_add)(struct bits_add_ctx *ctx, uint32_t v, uint32_t bits)
;

#ifdef __cplusplus
}
#endif


#endif
