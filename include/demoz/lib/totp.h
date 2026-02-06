/* @file: totp.h
 * #desc:
 *    The definitions of time-based one-time password.
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

#ifndef _DEMOZ_LIB_TOTP_H
#define _DEMOZ_LIB_TOTP_H

#include <demoz/config.h>
#include <demoz/c/stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

/* lib/totp_sha1.c */

extern
uint32_t F_SYMBOL(totp_sha1)(const uint8_t *key, uint32_t key_len,
		uint64_t time_base, uint32_t n)
;

/* lib/totp_sha256.c */

extern
uint32_t F_SYMBOL(totp_sha256)(const uint8_t *key, uint32_t key_len,
		uint64_t time_base, uint32_t n)
;

/* lib/totp_sha512.c */

extern
uint32_t F_SYMBOL(totp_sha512)(const uint8_t *key, uint32_t key_len,
		uint64_t time_base, uint32_t n)
;

#ifdef __cplusplus
}
#endif


#endif
