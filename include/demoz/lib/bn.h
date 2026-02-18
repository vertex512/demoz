/* @file: bn.h
 * #desc:
 *    The definitions of big number calculation.
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

#ifndef _DEMOZ_LIB_BN_H
#define _DEMOZ_LIB_BN_H

#include <demoz/config.h>
#include <demoz/c/stdint.h>


/* @def: _ */
#define BN4352_N 136
#define BN4352_NB 4352
#define BN6400_N 200
#define BN6400_NB 6400
#define BN8448_N 264
#define BN8448_NB 8448

typedef struct {
	uint32_t n[BN4352_N];
	uint32_t sign;
} bn4352_t;

typedef struct {
	bn4352_t n;
	bn4352_t R;
	bn4352_t np;
	bn4352_t mask;
	int32_t k;
} bn4352_redc_t;

typedef struct {
	uint32_t n[BN6400_N];
	uint32_t sign;
} bn6400_t;

typedef struct {
	bn6400_t n;
	bn6400_t R;
	bn6400_t np;
	bn6400_t mask;
	int32_t k;
} bn6400_redc_t;

typedef struct {
	uint32_t n[BN8448_N];
	uint32_t sign;
} bn8448_t;

typedef struct {
	bn8448_t n;
	bn8448_t R;
	bn8448_t np;
	bn8448_t mask;
	int32_t k;
} bn8448_redc_t;
/* end */


#ifdef __cplusplus
extern "C" {
#endif

/* lib/bn4352.c */

extern
void F_SYMBOL(bn4352_move)(bn4352_t *t, const bn4352_t *s)
;

extern
void F_SYMBOL(bn4352_swap)(bn4352_t *a, bn4352_t *b)
;

extern
void F_SYMBOL(bn4352_set1)(bn4352_t *t, uint32_t n)
;

extern
void F_SYMBOL(bn4352_from_bytes)(bn4352_t *t, const uint8_t *s, uint32_t len)
;

extern
void F_SYMBOL(bn4352_to_bytes)(const bn4352_t *s, uint8_t *t, uint32_t len)
;

extern
int32_t F_SYMBOL(bn4352_bits)(const bn4352_t *t)
;

extern
void F_SYMBOL(bn4352_lshift)(bn4352_t *t, uint32_t n)
;

extern
void F_SYMBOL(bn4352_rshift)(bn4352_t *t, uint32_t n)
;

extern
void F_SYMBOL(bn4352_and)(bn4352_t *r,
		const bn4352_t *a, const bn4352_t *b)
;

extern
void F_SYMBOL(bn4352_or)(bn4352_t *r,
		const bn4352_t *a, const bn4352_t *b)
;

extern
void F_SYMBOL(bn4352_xor)(bn4352_t *r,
		const bn4352_t *a, const bn4352_t *b)
;

extern
void F_SYMBOL(bn4352_not)(bn4352_t *r,
		const bn4352_t *a)
;

extern
int32_t F_SYMBOL(bn4352_ucmp)(const bn4352_t *a, const bn4352_t *b)
;

extern
int32_t F_SYMBOL(bn4352_ucmp_1)(const bn4352_t *a, uint32_t b)
;

extern
int32_t F_SYMBOL(bn4352_cmp)(const bn4352_t *a, const bn4352_t *b)
;

extern
int32_t F_SYMBOL(bn4352_cmp_1)(const bn4352_t *a, uint32_t b)
;

extern
uint32_t F_SYMBOL(bn4352_uadd)(bn4352_t *r,
		const bn4352_t *a, const bn4352_t *b)
;

extern
uint32_t F_SYMBOL(bn4352_uadd_1)(bn4352_t *r,
		const bn4352_t *a, uint32_t b)
;

extern
uint32_t F_SYMBOL(bn4352_usub)(bn4352_t *r,
		const bn4352_t *a, const bn4352_t *b)
;

extern
uint32_t F_SYMBOL(bn4352_usub_1)(bn4352_t *r,
		const bn4352_t *a, uint32_t b)
;

extern
void F_SYMBOL(bn4352_umul)(bn4352_t *r,
		const bn4352_t *a, const bn4352_t *b)
;

extern
uint32_t F_SYMBOL(bn4352_umul_1)(bn4352_t *r,
		const bn4352_t *a, uint32_t b)
;

extern
void F_SYMBOL(bn4352_udiv)(bn4352_t *quo, bn4352_t *rem,
		const bn4352_t *a, const bn4352_t *b)
;

extern
void F_SYMBOL(bn4352_udiv_1)(bn4352_t *quo, bn4352_t *rem,
		const bn4352_t *a, uint32_t b)
;

extern
void F_SYMBOL(bn4352_add)(bn4352_t *r,
		const bn4352_t *a, const bn4352_t *b)
;

extern
void F_SYMBOL(bn4352_add_1)(bn4352_t *r,
		const bn4352_t *a, uint32_t b)
;

extern
void F_SYMBOL(bn4352_sub)(bn4352_t *r,
		const bn4352_t *a, const bn4352_t *b)
;

extern
void F_SYMBOL(bn4352_sub_1)(bn4352_t *r,
		const bn4352_t *a, uint32_t b)
;

extern
void F_SYMBOL(bn4352_mul)(bn4352_t *r,
		const bn4352_t *a, const bn4352_t *b)
;

extern
void F_SYMBOL(bn4352_mul_1)(bn4352_t *r,
		const bn4352_t *a, uint32_t b)
;

extern
void F_SYMBOL(bn4352_div)(bn4352_t *quo, bn4352_t *rem,
		const bn4352_t *a, const bn4352_t *b)
;

extern
void F_SYMBOL(bn4352_div_1)(bn4352_t *quo, bn4352_t *rem,
		const bn4352_t *a, uint32_t b)
;

extern
void F_SYMBOL(bn4352_divmod)(bn4352_t *quo, bn4352_t *rem,
		const bn4352_t *a, const bn4352_t *b)
;

extern
void F_SYMBOL(bn4352_divmod_1)(bn4352_t *quo, bn4352_t *rem,
		const bn4352_t *a, uint32_t b)
;

extern
void F_SYMBOL(bn4352_modpow)(bn4352_t *r,
		const bn4352_t *a, const bn4352_t *e, const bn4352_t *m)
;

extern
void F_SYMBOL(bn4352_gcd)(bn4352_t *r, bn4352_t *x, bn4352_t *y,
		const bn4352_t *a, const bn4352_t *b)
;

extern
int32_t F_SYMBOL(bn4352_inv)(bn4352_t *r,
		const bn4352_t *a, const bn4352_t *b)
;

extern
int32_t F_SYMBOL(bn4352_redc_init)(bn4352_redc_t *s, const bn4352_t *n)
;

extern
void F_SYMBOL(bn4352_redc_mod)(const bn4352_redc_t *s,
		bn4352_t *r, const bn4352_t *a)
;

extern
void F_SYMBOL(bn4352_redc_mont)(const bn4352_redc_t *s,
		bn4352_t *r, const bn4352_t *a)
;

extern
int32_t F_SYMBOL(bn4352_redc_pow)(bn4352_t *r,
		const bn4352_t *a, const bn4352_t *e, const bn4352_t *m)
;

extern
void F_SYMBOL(bn4352_str2num)(bn4352_t *t,
		const char *s, char **e, int32_t b)
;

extern
void F_SYMBOL(bn4352_num2str)(const bn4352_t *a, char *p)
;

/* lib/bn6400.c */

extern
void F_SYMBOL(bn6400_move)(bn6400_t *t, const bn6400_t *s)
;

extern
void F_SYMBOL(bn6400_swap)(bn6400_t *a, bn6400_t *b)
;

extern
void F_SYMBOL(bn6400_set1)(bn6400_t *t, uint32_t n)
;

extern
void F_SYMBOL(bn6400_from_bytes)(bn6400_t *t, const uint8_t *s, uint32_t len)
;

extern
void F_SYMBOL(bn6400_to_bytes)(const bn6400_t *s, uint8_t *t, uint32_t len)
;

extern
int32_t F_SYMBOL(bn6400_bits)(const bn6400_t *t)
;

extern
void F_SYMBOL(bn6400_lshift)(bn6400_t *t, uint32_t n)
;

extern
void F_SYMBOL(bn6400_rshift)(bn6400_t *t, uint32_t n)
;

extern
void F_SYMBOL(bn6400_and)(bn6400_t *r,
		const bn6400_t *a, const bn6400_t *b)
;

extern
void F_SYMBOL(bn6400_or)(bn6400_t *r,
		const bn6400_t *a, const bn6400_t *b)
;

extern
void F_SYMBOL(bn6400_xor)(bn6400_t *r,
		const bn6400_t *a, const bn6400_t *b)
;

extern
void F_SYMBOL(bn6400_not)(bn6400_t *r,
		const bn6400_t *a)
;

extern
int32_t F_SYMBOL(bn6400_ucmp)(const bn6400_t *a, const bn6400_t *b)
;

extern
int32_t F_SYMBOL(bn6400_ucmp_1)(const bn6400_t *a, uint32_t b)
;

extern
int32_t F_SYMBOL(bn6400_cmp)(const bn6400_t *a, const bn6400_t *b)
;

extern
int32_t F_SYMBOL(bn6400_cmp_1)(const bn6400_t *a, uint32_t b)
;

extern
uint32_t F_SYMBOL(bn6400_uadd)(bn6400_t *r,
		const bn6400_t *a, const bn6400_t *b)
;

extern
uint32_t F_SYMBOL(bn6400_uadd_1)(bn6400_t *r,
		const bn6400_t *a, uint32_t b)
;

extern
uint32_t F_SYMBOL(bn6400_usub)(bn6400_t *r,
		const bn6400_t *a, const bn6400_t *b)
;

extern
uint32_t F_SYMBOL(bn6400_usub_1)(bn6400_t *r,
		const bn6400_t *a, uint32_t b)
;

extern
void F_SYMBOL(bn6400_umul)(bn6400_t *r,
		const bn6400_t *a, const bn6400_t *b)
;

extern
uint32_t F_SYMBOL(bn6400_umul_1)(bn6400_t *r,
		const bn6400_t *a, uint32_t b)
;

extern
void F_SYMBOL(bn6400_udiv)(bn6400_t *quo, bn6400_t *rem,
		const bn6400_t *a, const bn6400_t *b)
;

extern
void F_SYMBOL(bn6400_udiv_1)(bn6400_t *quo, bn6400_t *rem,
		const bn6400_t *a, uint32_t b)
;

extern
void F_SYMBOL(bn6400_add)(bn6400_t *r,
		const bn6400_t *a, const bn6400_t *b)
;

extern
void F_SYMBOL(bn6400_add_1)(bn6400_t *r,
		const bn6400_t *a, uint32_t b)
;

extern
void F_SYMBOL(bn6400_sub)(bn6400_t *r,
		const bn6400_t *a, const bn6400_t *b)
;

extern
void F_SYMBOL(bn6400_sub_1)(bn6400_t *r,
		const bn6400_t *a, uint32_t b)
;

extern
void F_SYMBOL(bn6400_mul)(bn6400_t *r,
		const bn6400_t *a, const bn6400_t *b)
;

extern
void F_SYMBOL(bn6400_mul_1)(bn6400_t *r,
		const bn6400_t *a, uint32_t b)
;

extern
void F_SYMBOL(bn6400_div)(bn6400_t *quo, bn6400_t *rem,
		const bn6400_t *a, const bn6400_t *b)
;

extern
void F_SYMBOL(bn6400_div_1)(bn6400_t *quo, bn6400_t *rem,
		const bn6400_t *a, uint32_t b)
;

extern
void F_SYMBOL(bn6400_divmod)(bn6400_t *quo, bn6400_t *rem,
		const bn6400_t *a, const bn6400_t *b)
;

extern
void F_SYMBOL(bn6400_divmod_1)(bn6400_t *quo, bn6400_t *rem,
		const bn6400_t *a, uint32_t b)
;

extern
void F_SYMBOL(bn6400_modpow)(bn6400_t *r,
		const bn6400_t *a, const bn6400_t *e, const bn6400_t *m)
;

extern
void F_SYMBOL(bn6400_gcd)(bn6400_t *r, bn6400_t *x, bn6400_t *y,
		const bn6400_t *a, const bn6400_t *b)
;

extern
int32_t F_SYMBOL(bn6400_inv)(bn6400_t *r,
		const bn6400_t *a, const bn6400_t *b)
;

extern
int32_t F_SYMBOL(bn6400_redc_init)(bn6400_redc_t *s, const bn6400_t *n)
;

extern
void F_SYMBOL(bn6400_redc_mod)(const bn6400_redc_t *s,
		bn6400_t *r, const bn6400_t *a)
;

extern
void F_SYMBOL(bn6400_redc_mont)(const bn6400_redc_t *s,
		bn6400_t *r, const bn6400_t *a)
;

extern
int32_t F_SYMBOL(bn6400_redc_pow)(bn6400_t *r,
		const bn6400_t *a, const bn6400_t *e, const bn6400_t *m)
;

extern
void F_SYMBOL(bn6400_str2num)(bn6400_t *t,
		const char *s, char **e, int32_t b)
;

extern
void F_SYMBOL(bn6400_num2str)(const bn6400_t *a, char *p)
;

/* lib/bn8448.c */

extern
void F_SYMBOL(bn8448_move)(bn8448_t *t, const bn8448_t *s)
;

extern
void F_SYMBOL(bn8448_swap)(bn8448_t *a, bn8448_t *b)
;

extern
void F_SYMBOL(bn8448_set1)(bn8448_t *t, uint32_t n)
;

extern
void F_SYMBOL(bn8448_from_bytes)(bn8448_t *t, const uint8_t *s, uint32_t len)
;

extern
void F_SYMBOL(bn8448_to_bytes)(const bn8448_t *s, uint8_t *t, uint32_t len)
;

extern
int32_t F_SYMBOL(bn8448_bits)(const bn8448_t *t)
;

extern
void F_SYMBOL(bn8448_lshift)(bn8448_t *t, uint32_t n)
;

extern
void F_SYMBOL(bn8448_rshift)(bn8448_t *t, uint32_t n)
;

extern
void F_SYMBOL(bn8448_and)(bn8448_t *r,
		const bn8448_t *a, const bn8448_t *b)
;

extern
void F_SYMBOL(bn8448_or)(bn8448_t *r,
		const bn8448_t *a, const bn8448_t *b)
;

extern
void F_SYMBOL(bn8448_xor)(bn8448_t *r,
		const bn8448_t *a, const bn8448_t *b)
;

extern
void F_SYMBOL(bn8448_not)(bn8448_t *r,
		const bn8448_t *a)
;

extern
int32_t F_SYMBOL(bn8448_ucmp)(const bn8448_t *a, const bn8448_t *b)
;

extern
int32_t F_SYMBOL(bn8448_ucmp_1)(const bn8448_t *a, uint32_t b)
;

extern
int32_t F_SYMBOL(bn8448_cmp)(const bn8448_t *a, const bn8448_t *b)
;

extern
int32_t F_SYMBOL(bn8448_cmp_1)(const bn8448_t *a, uint32_t b)
;

extern
uint32_t F_SYMBOL(bn8448_uadd)(bn8448_t *r,
		const bn8448_t *a, const bn8448_t *b)
;

extern
uint32_t F_SYMBOL(bn8448_uadd_1)(bn8448_t *r,
		const bn8448_t *a, uint32_t b)
;

extern
uint32_t F_SYMBOL(bn8448_usub)(bn8448_t *r,
		const bn8448_t *a, const bn8448_t *b)
;

extern
uint32_t F_SYMBOL(bn8448_usub_1)(bn8448_t *r,
		const bn8448_t *a, uint32_t b)
;

extern
void F_SYMBOL(bn8448_umul)(bn8448_t *r,
		const bn8448_t *a, const bn8448_t *b)
;

extern
uint32_t F_SYMBOL(bn8448_umul_1)(bn8448_t *r,
		const bn8448_t *a, uint32_t b)
;

extern
void F_SYMBOL(bn8448_udiv)(bn8448_t *quo, bn8448_t *rem,
		const bn8448_t *a, const bn8448_t *b)
;

extern
void F_SYMBOL(bn8448_udiv_1)(bn8448_t *quo, bn8448_t *rem,
		const bn8448_t *a, uint32_t b)
;

extern
void F_SYMBOL(bn8448_add)(bn8448_t *r,
		const bn8448_t *a, const bn8448_t *b)
;

extern
void F_SYMBOL(bn8448_add_1)(bn8448_t *r,
		const bn8448_t *a, uint32_t b)
;

extern
void F_SYMBOL(bn8448_sub)(bn8448_t *r,
		const bn8448_t *a, const bn8448_t *b)
;

extern
void F_SYMBOL(bn8448_sub_1)(bn8448_t *r,
		const bn8448_t *a, uint32_t b)
;

extern
void F_SYMBOL(bn8448_mul)(bn8448_t *r,
		const bn8448_t *a, const bn8448_t *b)
;

extern
void F_SYMBOL(bn8448_mul_1)(bn8448_t *r,
		const bn8448_t *a, uint32_t b)
;

extern
void F_SYMBOL(bn8448_div)(bn8448_t *quo, bn8448_t *rem,
		const bn8448_t *a, const bn8448_t *b)
;

extern
void F_SYMBOL(bn8448_div_1)(bn8448_t *quo, bn8448_t *rem,
		const bn8448_t *a, uint32_t b)
;

extern
void F_SYMBOL(bn8448_divmod)(bn8448_t *quo, bn8448_t *rem,
		const bn8448_t *a, const bn8448_t *b)
;

extern
void F_SYMBOL(bn8448_divmod_1)(bn8448_t *quo, bn8448_t *rem,
		const bn8448_t *a, uint32_t b)
;

extern
void F_SYMBOL(bn8448_modpow)(bn8448_t *r,
		const bn8448_t *a, const bn8448_t *e, const bn8448_t *m)
;

extern
void F_SYMBOL(bn8448_gcd)(bn8448_t *r, bn8448_t *x, bn8448_t *y,
		const bn8448_t *a, const bn8448_t *b)
;

extern
int32_t F_SYMBOL(bn8448_inv)(bn8448_t *r,
		const bn8448_t *a, const bn8448_t *b)
;

extern
int32_t F_SYMBOL(bn8448_redc_init)(bn8448_redc_t *s, const bn8448_t *n)
;

extern
void F_SYMBOL(bn8448_redc_mod)(const bn8448_redc_t *s,
		bn8448_t *r, const bn8448_t *a)
;

extern
void F_SYMBOL(bn8448_redc_mont)(const bn8448_redc_t *s,
		bn8448_t *r, const bn8448_t *a)
;

extern
int32_t F_SYMBOL(bn8448_redc_pow)(bn8448_t *r,
		const bn8448_t *a, const bn8448_t *e, const bn8448_t *m)
;

extern
void F_SYMBOL(bn8448_str2num)(bn8448_t *t,
		const char *s, char **e, int32_t b)
;

extern
void F_SYMBOL(bn8448_num2str)(const bn8448_t *a, char *p)
;

#ifdef __cplusplus
}
#endif


#endif
