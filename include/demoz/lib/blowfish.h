/* @file: blowfish.h
 * #desc:
 *    The definitions of blowfish symmetric-key block cipher.
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

#ifndef _DEMOZ_LIB_BLOWFISH_H
#define _DEMOZ_LIB_BLOWFISH_H

#include <demoz/config.h>
#include <demoz/c/stdint.h>


/* @def: _ */
struct blowfish_ctx {
	uint32_t P[18];
	uint32_t S1[256];
	uint32_t S2[256];
	uint32_t S3[256];
	uint32_t S4[256];
};

#define BLOWFISH_NEW(x) struct blowfish_ctx x
/* end */


#ifdef __cplusplus
extern "C" {
#endif

/* lib/blowfish.c */

extern
void F_SYMBOL(blowfish_encrypt)(struct blowfish_ctx *ctx,
		uint32_t *xl, uint32_t *xr)
;

extern
void F_SYMBOL(blowfish_decrypt)(struct blowfish_ctx *ctx,
		uint32_t *xl, uint32_t *xr)
;

extern
void F_SYMBOL(blowfish_setkey)(struct blowfish_ctx *ctx, const uint8_t *key,
		uint32_t len)
;

extern
void F_SYMBOL(blowfish_ekskey)(struct blowfish_ctx *ctx, const uint8_t *data,
		uint32_t data_len, const uint8_t *key, uint32_t key_len)
;

extern
void F_SYMBOL(blowfish_init)(struct blowfish_ctx *ctx, const uint8_t *key,
		uint32_t len)
;

#ifdef __cplusplus
}
#endif


#endif
