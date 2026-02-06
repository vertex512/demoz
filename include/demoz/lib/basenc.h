/* @file: basenc.h
 * #desc:
 *    The definitions of base coding.
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

#ifndef _DEMOZ_LIB_BASENC_H
#define _DEMOZ_LIB_BASENC_H

#include <demoz/config.h>
#include <demoz/c/stdint.h>


/* @def: _ */
#define BASE16_ENC_IALIGN(x) (x)
#define BASE16_ENC_OALIGN(x) (2 * (((x) + 1) / 2))

#define BASE16_DEC_IALIGN(x) BASE16_ENC_OALIGN(x)
#define BASE16_DEC_OALIGN(x) (x)

#define BASE32_ENC_IALIGN(x) (5 * (((x) + 4) / 5))
#define BASE32_ENC_OALIGN(x) (8 * (((x) + 4) / 5))

#define BASE32_DEC_IALIGN(x) BASE32_ENC_OALIGN(x)
#define BASE32_DEC_OALIGN(x) ((5 * BASE32_DEC_IALIGN(x)) / 8)

#define BASE64_ENC_IALIGN(x) (3 * (((x) + 2) / 3))
#define BASE64_ENC_OALIGN(x) (4 * (((x) + 2) / 3))

#define BASE64_DEC_IALIGN(x) BASE64_ENC_OALIGN(x)
#define BASE64_DEC_OALIGN(x) ((3 * BASE64_DEC_IALIGN(x)) / 4)
/* end */


#ifdef __cplusplus
extern "C" {
#endif

/* lib/base16.c */

extern
void F_SYMBOL(base16_enc)(const char *s, uint32_t slen, char *t,
		uint32_t *tlen)
;

extern
int32_t F_SYMBOL(base16_dec)(const char *s, uint32_t slen, char *t,
		uint32_t *tlen)
;

/* lib/base32.c */

extern
void F_SYMBOL(base32_enc)(const char *s, uint32_t slen, char *t,
		uint32_t *tlen)
;

extern
int32_t F_SYMBOL(base32_dec)(const char *s, uint32_t slen, char *t,
		uint32_t *tlen)
;

/* lib/base64.c */

extern
void F_SYMBOL(base64_enc)(const char *s, uint32_t slen, char *t,
		uint32_t *tlen)
;

extern
int32_t F_SYMBOL(base64_dec)(const char *s, uint32_t slen, char *t,
		uint32_t *tlen)
;

#ifdef __cplusplus
}
#endif


#endif
