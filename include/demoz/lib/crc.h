/* @file: crc.h
 * #desc:
 *    The definitions of cyclic redundancy check.
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

#ifndef _DEMOZ_LIB_CRC_H
#define _DEMOZ_LIB_CRC_H

#include <demoz/config.h>
#include <demoz/c/stdint.h>


/* @def: _ */
#define CRC16_DEFAULT_MSB_TYPE 0
#define CRC16_DEFAULT_LSB_TYPE 1

#define CRC16_TABLE_SIZE 256

#define CRC32_DEFAULT_MSB_TYPE 0
#define CRC32_DEFAULT_LSB_TYPE 1
#define CRC32_CASTAGNOLI_MSB_TYPE 2
#define CRC32_CASTAGNOLI_LSB_TYPE 3
#define CRC32_KOOPMAN_MSB_TYPE 4
#define CRC32_KOOPMAN_LSB_TYPE 5
#define CRC32_Q_MSB_TYPE 6
#define CRC32_Q_LSB_TYPE 7
#define CRC32_CKSUM_MSB_TYPE 8

#define CRC32_TABLE_SIZE 256

#define CRC64_DEFAULT_MSB_TYPE 0
#define CRC64_DEFAULT_LSB_TYPE 1
#define CRC64_ECMA_MSB_TYPE 2
#define CRC64_ECMA_LSB_TYPE 3

#define CRC64_TABLE_SIZE 256
/* end */


#ifdef __cplusplus
extern "C" {
#endif

/* lib/crc16.c */

extern
const uint16_t *F_SYMBOL(crc16_table)(int32_t type)
;

extern
uint16_t F_SYMBOL(crc16_msb)(const uint16_t *t, uint16_t c, const uint8_t *s,
		uint32_t len)
;

extern
uint16_t F_SYMBOL(crc16_lsb)(const uint16_t *t, uint16_t c, const uint8_t *s,
		uint32_t len)
;

extern
uint32_t F_SYMBOL(crc16)(const uint8_t *s, uint32_t len, int32_t type)
;

/* lib/crc32.c */

extern
const uint32_t *F_SYMBOL(crc32_table)(int32_t type)
;

extern
uint32_t F_SYMBOL(crc32_msb)(const uint32_t *t, uint32_t c, const uint8_t *s,
		uint32_t len)
;

extern
uint32_t F_SYMBOL(crc32_lsb)(const uint32_t *t, uint32_t c, const uint8_t *s,
		uint32_t len)
;

extern
uint32_t F_SYMBOL(crc32_cksum_size_msb)(const uint32_t *t, uint32_t c,
		uint32_t len)
;

extern
uint32_t F_SYMBOL(crc32)(const uint8_t *s, uint32_t len, int32_t type)
;

/* lib/crc64.c */

extern
const uint64_t *F_SYMBOL(crc64_table)(int32_t type)
;

extern
uint64_t F_SYMBOL(crc64_msb)(const uint64_t *t, uint64_t c, const uint8_t *s,
		uint32_t len)
;

extern
uint64_t F_SYMBOL(crc64_lsb)(const uint64_t *t, uint64_t c, const uint8_t *s,
		uint32_t len)
;

extern
uint64_t F_SYMBOL(crc64)(const uint8_t *s, uint32_t len, int32_t type)
;

#ifdef __cplusplus
}
#endif


#endif
