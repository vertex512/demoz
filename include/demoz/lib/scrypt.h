/* @file: scrypt.h
 * #desc:
 *    the definitions of scrypt password-based key derivation function.
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

#ifndef _DEMOZ_LIB_SCRYPT_H
#define _DEMOZ_LIB_SCRYPT_H

#include <demoz/config.h>
#include <demoz/c/stdint.h>


/* @def: _ */
#define SCRYPT_BLOCKSIZE 128
#define SCRYPT_ELEMENTSIZE 64

#define SCRYPT_TMP_B(r, p) ((r) * SCRYPT_BLOCKSIZE * (p))
#define SCRYPT_TMP_V(r, n) ((r) * SCRYPT_BLOCKSIZE * (n))
#define SCRYPT_TMP_Y(r) ((r) * SCRYPT_BLOCKSIZE)
#define SCRYPT_TMPSIZE(n, r, p) \
	(SCRYPT_TMP_B(r, p) + SCRYPT_TMP_V(r, n) + SCRYPT_TMP_Y(r))
/* end */


#ifdef __cplusplus
extern "C" {
#endif

/* lib/scrypt.c */

extern
void F_SYMBOL(scrypt)(const uint8_t *pass, uint32_t pass_len,
		const uint8_t *salt, uint32_t salt_len, uint8_t *tmp,
		uint32_t n, uint32_t r, uint32_t p, uint8_t *dk,
		uint32_t len)
;

#ifdef __cplusplus
}
#endif


#endif
