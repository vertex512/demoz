/* @file: atomic.h
 * #desc:
 *    The definitions of atomic operations.
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

#ifndef _DEMOZ_C_ATOMIC_H
#define _DEMOZ_C_ATOMIC_H

#include <demoz/config.h>
#include <demoz/c/stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

/* c/atomic_cas.S */

extern
int32_t C_SYMBOL(atomic_cas)(volatile int32_t *v, int32_t old, int32_t _new)
;

#ifdef __cplusplus
}
#endif


#endif
