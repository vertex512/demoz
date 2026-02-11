/* @file: rsa.h
 * #desc:
 *    The definitions of rsa (rivest shamir adleman) cryptosystem.
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

#ifndef _DEMOZ_LIB_RSA_H
#define _DEMOZ_LIB_RSA_H

#include <demoz/config.h>
#include <demoz/c/stdint.h>


/* @def: _ */
#define RSA2048_PRIME_LEN 128
#define RSA2048_LEN 256

#define RSA3072_PRIME_LEN 192
#define RSA3072_LEN 384

#define RSA4096_PRIME_LEN 256
#define RSA4096_LEN 512
/* end */


#ifdef __cplusplus
extern "C" {
#endif

/* lib/rsa2048.c */

extern
int32_t F_SYMBOL(rsa2048_genkey)(const uint8_t *p, const uint8_t *q,
		uint8_t *e, uint8_t *d, uint8_t *n)
;

extern
int32_t F_SYMBOL(rsa2048_encrypt)(const uint8_t *e, const uint8_t *n,
		uint8_t *msg)
;

extern
int32_t F_SYMBOL(rsa2048_decrypt)(const uint8_t *d, const uint8_t *n,
		uint8_t *msg)
;

extern
int32_t F_SYMBOL(rsa2048_gencrt)(const uint8_t *p, const uint8_t *q,
		const uint8_t *d, uint8_t *dp, uint8_t *dq, uint8_t *qinv)
;

extern
int32_t F_SYMBOL(rsa2048_crt_decrypt)(const uint8_t *p, const uint8_t *q,
		const uint8_t *dp, const uint8_t *dq, const uint8_t *qinv,
		uint8_t *msg)
;

/* lib/rsa3072.c */

extern
int32_t F_SYMBOL(rsa3072_genkey)(const uint8_t *p, const uint8_t *q,
		uint8_t *e, uint8_t *d, uint8_t *n)
;

extern
int32_t F_SYMBOL(rsa3072_encrypt)(const uint8_t *e, const uint8_t *n,
		uint8_t *msg)
;

extern
int32_t F_SYMBOL(rsa3072_decrypt)(const uint8_t *d, const uint8_t *n,
		uint8_t *msg)
;

extern
int32_t F_SYMBOL(rsa3072_gencrt)(const uint8_t *p, const uint8_t *q,
		const uint8_t *d, uint8_t *dp, uint8_t *dq, uint8_t *qinv)
;

extern
int32_t F_SYMBOL(rsa3072_crt_decrypt)(const uint8_t *p, const uint8_t *q,
		const uint8_t *dp, const uint8_t *dq, const uint8_t *qinv,
		uint8_t *msg)
;

/* lib/rsa4096.c */

extern
int32_t F_SYMBOL(rsa4096_genkey)(const uint8_t *p, const uint8_t *q,
		uint8_t *e, uint8_t *d, uint8_t *n)
;

extern
int32_t F_SYMBOL(rsa4096_encrypt)(const uint8_t *e, const uint8_t *n,
		uint8_t *msg)
;

extern
int32_t F_SYMBOL(rsa4096_decrypt)(const uint8_t *d, const uint8_t *n,
		uint8_t *msg)
;

extern
int32_t F_SYMBOL(rsa4096_gencrt)(const uint8_t *p, const uint8_t *q,
		const uint8_t *d, uint8_t *dp, uint8_t *dq, uint8_t *qinv)
;

extern
int32_t F_SYMBOL(rsa4096_crt_decrypt)(const uint8_t *p, const uint8_t *q,
		const uint8_t *dp, const uint8_t *dq, const uint8_t *qinv,
		uint8_t *msg)
;

#ifdef __cplusplus
}
#endif


#endif
