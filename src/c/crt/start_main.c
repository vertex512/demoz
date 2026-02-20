/* @file: start_main.c
 * #desc:
 *    The implementations of c runtime entry.
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
#include <demoz/c/stddef.h>
#include <demoz/c/stdint.h>
#include <demoz/c/sys/stdlib.h>
#include <demoz/c/crt/start_main.h>


/* @def: _ */
char **C_SYMBOL(___envp) = NULL;
/* end */

/* @func: ___start_main
 * #desc:
 *    c runtime entry.
 *
 * #1: sp [in/out] stack pointer
 */
void C_SYMBOL(___start_main)(size_t *sp)
{
	int32_t argc = sp[0];
	char **argv = (char **)&sp[argc + 1];
	C_SYMBOL(___envp) = (char **)&sp[argc + 2];

	int32_t r = main(argc, argv, C_SYMBOL(___envp));

	C_SYMBOL(_Exit)(r);
}
