/* @file: siphash24.h
 * #desc:
 *    The definitions of siphash message authentication code.
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

#ifndef _DEMOZ_LIB_SIPHASH24_H
#define _DEMOZ_LIB_SIPHASH24_H

#include <demoz/config.h>
#include <demoz/c/stddef.h>
#include <demoz/c/stdint.h>


/* @def: _ */
#define SIPHASH24_KEYLEN 16
/* end */


#ifdef __cplusplus
extern "C" {
#endif

/* lib/siphash24.c */

extern
uint64_t F_SYMBOL(siphash24)(const uint8_t *s, size_t len, const uint8_t *key)
;

extern
uint64_t F_SYMBOL(siphashx24)(const uint8_t *s, size_t len, const uint8_t *key,
		uint64_t *h2)
;

#ifdef __cplusplus
}
#endif


#endif
