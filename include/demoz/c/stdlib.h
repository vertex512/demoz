/* @file: stdlib.h
 * #desc:
 *    The definitions of standard library.
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

#ifndef _DEMOZ_C_STDLIB_H
#define _DEMOZ_C_STDLIB_H

#include <demoz/config.h>
#include <demoz/c/stdint.h>


/* @def: _ */
struct random_ctx {
	int32_t type;
	int32_t *state;
	int32_t *fptr;
	int32_t *bptr;
	int32_t *eptr;
};

/* LCG (Linear Congruential Generator) */
#define RANDOM_TYPE0_NEW(name, seed) \
	int32_t name##_random_tab0[1] = { \
		seed \
		}; \
	struct random_ctx name = { \
		.type = 0, \
		.state = name##_random_tab0 \
		}

/*
 * LFSR (Linear Feedback Shift Register)
 * x^31 + x^3 + 1
 */
#define RANDOM_TYPE1_NEW(name) \
	int32_t name##_random_tab1[32] = { \
		-1726662223, 379960547, 1735697613, 1040273694, \
		1313901226, 1627687941, -179304937, -2073333483, \
		1780058412, -1989503057, -615974602, 344556628, \
		939512070, -1249116260, 1507946756, -812545463, \
		154635395, 1388815473, -1926676823, 525320961, \
		-1009028674, 968117788, -123449607, 1284210865, \
		435012392, -2017506339, -911064859, -370259173, \
		1132637927, 1398500161, -205601318, 0 \
		}; \
	struct random_ctx name = { \
		.type = 1, \
		.state = name##_random_tab1, \
		.fptr = &name##_random_tab1[3], \
		.bptr = &name##_random_tab1[0], \
		.eptr = &name##_random_tab1[31] \
		}
/* end */


#ifdef __cplusplus
extern "C" {
#endif

/* c/stdlib_abs.c */

extern
int C_SYMBOL(abs)(int n)
;

extern
long C_SYMBOL(labs)(long n)
;

extern
long long C_SYMBOL(llabs)(long long n)
;

/* c/stdlib_atof.c */

extern
double C_SYMBOL(atof)(const char *s)
;

extern
double C_SYMBOL(strtod)(const char *s, char **e)
;

/* c/stdlib_atoi.c */

extern
uint64_t C_SYMBOL(___strtoull)(const char *s, char **e, uint64_t *m, int32_t b)
;

extern
int C_SYMBOL(atoi)(const char *s)
;

extern
long C_SYMBOL(atol)(const char *s)
;

extern
long long C_SYMBOL(atoll)(const char *s)
;

extern
long C_SYMBOL(strtol)(const char *s, char **e, int32_t b)
;

extern
unsigned long C_SYMBOL(strtoul)(const char *s, char **e, int32_t b)
;

extern
long long C_SYMBOL(strtoll)(const char *s, char **e, int32_t b)
;

extern
unsigned long long C_SYMBOL(strtoull)(const char *s, char **e, int32_t b)
;

/* c/stdlib_qsort.c */

extern
void C_SYMBOL(qsort)(void *b, uint64_t n, uint64_t w,
		int32_t (*cmp)(const void *, const void *))
;

extern
void *C_SYMBOL(bsearch)(const void *k, const void *b, uint64_t n, uint64_t w,
		int32_t (*cmp)(const void *, const void *))
;

/* c/stdlib_rand.c */

extern
int32_t C_SYMBOL(random_r)(struct random_ctx *p, int32_t *v)
;

#ifdef __cplusplus
}
#endif


#endif
