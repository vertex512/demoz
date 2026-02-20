/* @file: stdlib_rand.c
 * #desc:
 *    The implementations of standard library.
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

#include <demoz/config.h>
#include <demoz/c/stdint.h>
#include <demoz/c/stdlib.h>


/* @func: random_r
 * #desc:
 *    reentrant random number generator.
 *
 * #1: p [in/out] random context struct
 * #2: v [out]    random number result
 * #r:   [ret]    0: no error, -1: param error
 */
int32_t C_SYMBOL(random_r)(struct random_ctx *p, int32_t *v)
{
	if (!(p && v)) {
		/* errno */
		return -1;
	}

	/* LCG (Linear Congruential Generator) */
	int32_t *state = p->state;
	if (p->type == 0) {
		*v = ((state[0] * 1103515245) + 12345) & INT32_MAX;
		state[0] = *v;
		return 0;
	}

	/* LFSR (Linear Feedback Shift Register) */
	int32_t *fptr = p->fptr, *bptr = p->bptr, *eptr = p->eptr;
	*v = (*fptr += *bptr) >> 1;
	if (++fptr >= eptr) {
		fptr = state;
		bptr++;
	} else if (++bptr >= eptr) {
		bptr = state;
	}
	p->fptr = fptr;
	p->bptr = bptr;

	return 0;
}
