/* @file: hkdf.h
 * #desc:
 *    The definitions of hmac-base key derivation function.
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

#ifndef _DEMOZ_LIB_HKDF_H
#define _DEMOZ_LIB_HKDF_H

#include <demoz/config.h>
#include <demoz/c/stdint.h>


/* @def: _ */
#define HKDF_SHA3_224_TYPE 1
#define HKDF_SHA3_256_TYPE 2
#define HKDF_SHA3_384_TYPE 3
#define HKDF_SHA3_512_TYPE 4
/* end */


#ifdef __cplusplus
extern "C" {
#endif

/* lib/hkdf_blake2b.c */

extern
int32_t F_SYMBOL(hkdf_blake2b)(const uint8_t *ikm, uint32_t ikm_len,
		const uint8_t *salt, uint32_t salt_len,
		const uint8_t *info, uint32_t info_len,
		uint8_t *okm, uint32_t len)
;

/* lib/hkdf_blake2s.c */

extern
int32_t F_SYMBOL(hkdf_blake2s)(const uint8_t *ikm, uint32_t ikm_len,
		const uint8_t *salt, uint32_t salt_len,
		const uint8_t *info, uint32_t info_len,
		uint8_t *okm, uint32_t len)
;

/* lib/hkdf_md5.c */

extern
int32_t F_SYMBOL(hkdf_md5)(const uint8_t *ikm, uint32_t ikm_len,
		const uint8_t *salt, uint32_t salt_len,
		const uint8_t *info, uint32_t info_len,
		uint8_t *okm, uint32_t len)
;

/* lib/hkdf_sha1.c */

extern
int32_t F_SYMBOL(hkdf_sha1)(const uint8_t *ikm, uint32_t ikm_len,
		const uint8_t *salt, uint32_t salt_len,
		const uint8_t *info, uint32_t info_len,
		uint8_t *okm, uint32_t len)
;

/* lib/hkdf_sha256.c */

extern
int32_t F_SYMBOL(hkdf_sha256)(const uint8_t *ikm, uint32_t ikm_len,
		const uint8_t *salt, uint32_t salt_len,
		const uint8_t *info, uint32_t info_len,
		uint8_t *okm, uint32_t len)
;

extern
int32_t F_SYMBOL(hkdf_sha224)(const uint8_t *ikm, uint32_t ikm_len,
		const uint8_t *salt, uint32_t salt_len,
		const uint8_t *info, uint32_t info_len,
		uint8_t *okm, uint32_t len)
;

/* lib/hkdf_sha3.c */

extern
int32_t F_SYMBOL(hkdf_sha3)(const uint8_t *ikm, uint32_t ikm_len,
		const uint8_t *salt, uint32_t salt_len,
		const uint8_t *info, uint32_t info_len,
		uint8_t *okm, uint32_t len, int32_t type)
;

/* lib/hkdf_sha512.c */

extern
int32_t F_SYMBOL(hkdf_sha512)(const uint8_t *ikm, uint32_t ikm_len,
		const uint8_t *salt, uint32_t salt_len,
		const uint8_t *info, uint32_t info_len,
		uint8_t *okm, uint32_t len)
;

extern
int32_t F_SYMBOL(hkdf_sha384)(const uint8_t *ikm, uint32_t ikm_len,
		const uint8_t *salt, uint32_t salt_len,
		const uint8_t *info, uint32_t info_len,
		uint8_t *okm, uint32_t len)
;

#ifdef __cplusplus
}
#endif


#endif
