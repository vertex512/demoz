/* @file: bcrypt.h
 * #desc:
 *    The definitions of bcrypt password-hash.
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

#ifndef _DEMOZ_LIB_BCRYPT_H
#define _DEMOZ_LIB_BCRYPT_H

#include <demoz/config.h>
#include <demoz/c/stddef.h>
#include <demoz/c/stdint.h>


/* @def: _ */
#define BCRYPT_HASHPASS_LEN 24
/* end */


#ifdef __cplusplus
extern "C" {
#endif

/* lib/bcrypt.c */

extern
void F_SYMBOL(bcrypt_hashpass)(const uint8_t *pass, uint32_t pass_len,
		const uint8_t *salt, uint32_t salt_len, uint8_t *ohp,
		uint32_t k)
;

extern
int32_t F_SYMBOL(bcrypt_auth)(const uint8_t *a, const uint8_t *b)
;

#ifdef __cplusplus
}
#endif


#endif
