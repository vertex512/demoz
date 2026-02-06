/* @file: start_main.h
 * #desc:
 *    The definitions of c runtime entry.
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

#ifndef _DEMOZ_C_CRT_START_MAIN_H
#define _DEMOZ_C_CRT_START_MAIN_H

#include <demoz/config.h>
#include <demoz/c/stddef.h>
#include <demoz/c/stdint.h>


/* @def: _ */
extern
int32_t main(int32_t argc, char *argv[], char *envp[]);

extern
char **C_SYMBOL(___envp);
/* end */


#ifdef __cplusplus
extern "C" {
#endif

/* c/crt/start_main.c */

extern
void C_SYMBOL(___start_main)(size_t *sp)
;

#ifdef __cplusplus
}
#endif


#endif
