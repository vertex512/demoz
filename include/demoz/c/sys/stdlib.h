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

#ifndef _DEMOZ_C_SYS_STDLIB_H
#define _DEMOZ_C_SYS_STDLIB_H

#include <demoz/config.h>
#include <demoz/c/stddef.h>
#include <demoz/c/stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

/* c/sys/stdlib_Exit.c */

extern
void C_SYMBOL(_Exit)(int32_t status)
;

/* c/sys/stdlib_abort.c */

extern
void C_SYMBOL(abort)(void)
;

/* c/sys/stdlib_exit.c */

extern
int32_t C_SYMBOL(atexit)(void (*func)(void))
;

extern
void C_SYMBOL(exit)(int32_t status)
;

/* c/sys/stdlib_malloc.c */

extern
void *C_SYMBOL(malloc)(size_t size)
;

extern
void C_SYMBOL(free)(void *p)
;

#ifdef __cplusplus
}
#endif


#endif
