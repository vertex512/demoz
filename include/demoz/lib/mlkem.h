/* @file: mlkem.h
 * #desc:
 *    The definitions of module-lattice-based key-encapsulation mechanism.
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

#ifndef _DEMOZ_LIB_MLKEM_H
#define _DEMOZ_LIB_MLKEM_H

#include <demoz/config.h>
#include <demoz/c/stdint.h>


/* @def: _
 *  |     n   q    k eta1 eta2 du dv ek   dk   ct-size k-size|
 *  +========================================================+
 *  | 512 256 3329 2 3    2    10 4   800 1632  768    32    |
 *  | 768 256 3329 3 2    2    10 4  1184 2400 1088    32    |
 *  |1024 256 3329 4 2    2    11 5  1568 3168 1568    32    |
 */
#define MLKEM_N 256
#define MLKEM_Q 3329 /* 2^8*13+1 */
#define MLKEM_ROOT 17

#define MLKEM_RAN_LEN 32
#define MLKEM_KEY_LEN 32
#define MLKEM_SYM_LEN 32
#define MLKEM_POLY_BYTES (12 * MLKEM_N / 8) /* 384 */

/* mlkem-512 */
#define MLKEM_512_K 2
#define MLKEM_512_ETA1 3
#define MLKEM_512_ETA2 2
#define MLKEM_512_POLY_COMPRESS_DU (10 * MLKEM_N / 8) /* 320 */
#define MLKEM_512_POLY_COMPRESS_DV (4 * MLKEM_N / 8)  /* 128 */

#define MLKEM_512_POLYVEC_BYTES    /* 768 */ \
	(MLKEM_POLY_BYTES * MLKEM_512_K)
#define MLKEM_512_POLYVEC_COMPRESS /* 640 */ \
	(MLKEM_512_POLY_COMPRESS_DU * MLKEM_512_K)

#define MLKEM_512_EKPKE_LEN /* 800 */ \
	(MLKEM_512_POLYVEC_BYTES + MLKEM_SYM_LEN)
#define MLKEM_512_DKPKE_LEN /* 768 */ \
	MLKEM_512_POLYVEC_BYTES

#define MLKEM_512_EK_LEN /* 800 */ \
	MLKEM_512_EKPKE_LEN
#define MLKEM_512_DK_LEN /* 1632 */ \
	(MLKEM_512_DKPKE_LEN + MLKEM_512_EK_LEN + MLKEM_SYM_LEN * 2)
#define MLKEM_512_CT_LEN /* 768 */ \
	(MLKEM_512_POLYVEC_COMPRESS + MLKEM_512_POLY_COMPRESS_DV)

/* mlkem-768 */
#define MLKEM_768_K 3
#define MLKEM_768_ETA1 2
#define MLKEM_768_ETA2 2
#define MLKEM_768_POLY_COMPRESS_DU (10 * MLKEM_N / 8) /* 320 */
#define MLKEM_768_POLY_COMPRESS_DV (4 * MLKEM_N / 8)  /* 128 */

#define MLKEM_768_POLYVEC_BYTES    /* 1152 */ \
	(MLKEM_POLY_BYTES * MLKEM_768_K)
#define MLKEM_768_POLYVEC_COMPRESS /* 960 */ \
	(MLKEM_768_POLY_COMPRESS_DU * MLKEM_768_K)

#define MLKEM_768_EKPKE_LEN /* 1184 */ \
	(MLKEM_768_POLYVEC_BYTES + MLKEM_SYM_LEN)
#define MLKEM_768_DKPKE_LEN /* 1152 */ \
	MLKEM_768_POLYVEC_BYTES

#define MLKEM_768_EK_LEN /* 1184 */ \
	MLKEM_768_EKPKE_LEN
#define MLKEM_768_DK_LEN /* 2400 */ \
	(MLKEM_768_DKPKE_LEN + MLKEM_768_EK_LEN + MLKEM_SYM_LEN * 2)
#define MLKEM_768_CT_LEN /* 1088 */ \
	(MLKEM_768_POLYVEC_COMPRESS + MLKEM_768_POLY_COMPRESS_DV)

/* mlkem-1024 */
#define MLKEM_1024_K 4
#define MLKEM_1024_ETA1 2
#define MLKEM_1024_ETA2 2
#define MLKEM_1024_POLY_COMPRESS_DU (11 * MLKEM_N / 8) /* 352 */
#define MLKEM_1024_POLY_COMPRESS_DV (5 * MLKEM_N / 8)  /* 160 */

#define MLKEM_1024_POLYVEC_BYTES    /* 1536 */ \
	(MLKEM_POLY_BYTES * MLKEM_1024_K)
#define MLKEM_1024_POLYVEC_COMPRESS /* 1408 */ \
	(MLKEM_1024_POLY_COMPRESS_DU * MLKEM_1024_K)

#define MLKEM_1024_EKPKE_LEN /* 1568 */ \
	(MLKEM_1024_POLYVEC_BYTES + MLKEM_SYM_LEN)
#define MLKEM_1024_DKPKE_LEN /* 1536 */ \
	MLKEM_1024_POLYVEC_BYTES

#define MLKEM_1024_EK_LEN /* 1568 */ \
	MLKEM_1024_EKPKE_LEN
#define MLKEM_1024_DK_LEN /* 3160 */ \
	(MLKEM_1024_DKPKE_LEN + MLKEM_1024_EK_LEN + MLKEM_SYM_LEN * 2)
#define MLKEM_1024_CT_LEN /* 1568 */ \
	(MLKEM_1024_POLYVEC_COMPRESS + MLKEM_1024_POLY_COMPRESS_DV)
/* end */


#ifdef __cplusplus
extern "C" {
#endif

/* lib/mlkem_1024.c */

extern
void F_SYMBOL(mlkem1024_pke_genkey)(const uint8_t *ran, uint8_t *ekp,
		uint8_t *dkp)
;

extern
void F_SYMBOL(mlkem1024_pke_encrypt)(const uint8_t *ran, const uint8_t *ekp,
		const uint8_t *msg, uint8_t *ct)
;

extern
void F_SYMBOL(mlkem1024_pke_decrypt)(const uint8_t *dkp, const uint8_t *ct,
		uint8_t *msg)
;

extern
void F_SYMBOL(mlkem1024_genkey)(const uint8_t *ran, const uint8_t *ran2,
		uint8_t *ek, uint8_t *dk)
;

extern
void F_SYMBOL(mlkem1024_encaps)(const uint8_t *msg, const uint8_t *ek,
		uint8_t *ct, uint8_t *sk)
;

extern
void F_SYMBOL(mlkem1024_decaps)(const uint8_t *dk, const uint8_t *ct,
		uint8_t *sk)
;

/* lib/mlkem_512.c */

extern
void F_SYMBOL(mlkem512_pke_genkey)(const uint8_t *ran, uint8_t *ekp,
		uint8_t *dkp)
;

extern
void F_SYMBOL(mlkem512_pke_encrypt)(const uint8_t *ran, const uint8_t *ekp,
		const uint8_t *msg, uint8_t *ct)
;

extern
void F_SYMBOL(mlkem512_pke_decrypt)(const uint8_t *dkp, const uint8_t *ct,
		uint8_t *msg)
;

extern
void F_SYMBOL(mlkem512_genkey)(const uint8_t *ran, const uint8_t *ran2,
		uint8_t *ek, uint8_t *dk)
;

extern
void F_SYMBOL(mlkem512_encaps)(const uint8_t *msg, const uint8_t *ek,
		uint8_t *ct, uint8_t *sk)
;

extern
void F_SYMBOL(mlkem512_decaps)(const uint8_t *dk, const uint8_t *ct,
		uint8_t *sk)
;

/* lib/mlkem_768.c */

extern
void F_SYMBOL(mlkem768_pke_genkey)(const uint8_t *ran, uint8_t *ekp,
		uint8_t *dkp)
;

extern
void F_SYMBOL(mlkem768_pke_encrypt)(const uint8_t *ran, const uint8_t *ekp,
		const uint8_t *msg, uint8_t *ct)
;

extern
void F_SYMBOL(mlkem768_pke_decrypt)(const uint8_t *dkp, const uint8_t *ct,
		uint8_t *msg)
;

extern
void F_SYMBOL(mlkem768_genkey)(const uint8_t *ran, const uint8_t *ran2,
		uint8_t *ek, uint8_t *dk)
;

extern
void F_SYMBOL(mlkem768_encaps)(const uint8_t *msg, const uint8_t *ek,
		uint8_t *ct, uint8_t *sk)
;

extern
void F_SYMBOL(mlkem768_decaps)(const uint8_t *dk, const uint8_t *ct,
		uint8_t *sk)
;

#ifdef __cplusplus
}
#endif


#endif
