/* @file: stdlib_Exit.c
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
#include <demoz/c/sys/syscall.h>


/* @func: _Exit
 * #desc:
 *    terminate the calling process.
 *
 * #1: exit status
 */
void C_SYMBOL(_Exit)(int32_t status)
{
#if (DEMOZ_PLATFORM == DEMOZ_PLATFORM_LINUX)

	C_SYMBOL(syscall_linux)(
		XSYSCALL_exit_group,
		status);

	while (1) {
		C_SYMBOL(syscall_linux)(
			XSYSCALL_exit,
			status);
	}

#else
#	error "!!!unknown DEMOZ_PLATFORM!!!"
#endif
}
