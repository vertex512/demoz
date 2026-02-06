/* @file: pbkdf2.h
 * #desc:
 *    The definitions of password-based key derivation function.
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

#ifndef _DEMOZ_LIB_PBKDF2_H
#define _DEMOZ_LIB_PBKDF2_H

#include <demoz/config.h>
#include <demoz/c/stdint.h>


/* @def: _ */
#define PBKDF2_SHA3_224_TYPE 1
#define PBKDF2_SHA3_256_TYPE 2
#define PBKDF2_SHA3_384_TYPE 3
#define PBKDF2_SHA3_512_TYPE 4
/* end */


#ifdef __cplusplus
extern "C" {
#endif

/* lib/pbkdf2_blake2b.c */

extern
void F_SYMBOL(pbkdf2_blake2b)(const uint8_t *pass, uint32_t pass_len,
		const uint8_t *salt, uint32_t salt_len, uint8_t *odk,
		uint32_t len, uint32_t k)
;

/* lib/pbkdf2_blake2s.c */

extern
void F_SYMBOL(pbkdf2_blake2s)(const uint8_t *pass, uint32_t pass_len,
		const uint8_t *salt, uint32_t salt_len, uint8_t *odk,
		uint32_t len, uint32_t k)
;

/* lib/pbkdf2_sha1.c */

extern
void F_SYMBOL(pbkdf2_sha1)(const uint8_t *pass, uint32_t pass_len,
		const uint8_t *salt, uint32_t salt_len, uint8_t *odk,
		uint32_t len, uint32_t k)
;

/* lib/pbkdf2_sha256.c */

extern
void F_SYMBOL(pbkdf2_sha256)(const uint8_t *pass, uint32_t pass_len,
		const uint8_t *salt, uint32_t salt_len, uint8_t *odk,
		uint32_t len, uint32_t k)
;

extern
void F_SYMBOL(pbkdf2_sha224)(const uint8_t *pass, uint32_t pass_len,
		const uint8_t *salt, uint32_t salt_len, uint8_t *odk,
		uint32_t len, uint32_t k)
;

/* lib/pbkdf2_sha3.c */

extern
int32_t F_SYMBOL(pbkdf2_sha3)(const uint8_t *pass, uint32_t pass_len,
		const uint8_t *salt, uint32_t salt_len, uint8_t *odk,
		uint32_t len, uint32_t k, int32_t type)
;

/* lib/pbkdf2_sha512.c */

extern
void F_SYMBOL(pbkdf2_sha512)(const uint8_t *pass, uint32_t pass_len,
		const uint8_t *salt, uint32_t salt_len, uint8_t *odk,
		uint32_t len, uint32_t k)
;

extern
void F_SYMBOL(pbkdf2_sha384)(const uint8_t *pass, uint32_t pass_len,
		const uint8_t *salt, uint32_t salt_len, uint8_t *odk,
		uint32_t len, uint32_t k)
;

#ifdef __cplusplus
}
#endif


#endif
