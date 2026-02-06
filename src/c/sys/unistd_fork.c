/* @file: unistd_fork.c
 * #desc:
 *    The implementations of unix standard functions.
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
#include <demoz/c/errno.h>
#include <demoz/c/sys/types.h>
#include <demoz/c/sys/unistd.h>
#include <demoz/c/sys/syscall.h>


/* @func: fork
 * #desc:
 *    create a child process.
 *
 * #r: >0: child process id, -1: errno
 */
x_pid_t C_SYMBOL(fork)(void)
{
#if (DEMOZ_PLATFORM == DEMOZ_PLATFORM_LINUX)

	x_pid_t ret = 0;

	ret = C_SYMBOL(syscall_linux)(
		XSYSCALL_clone,
		SIGCHLD,
		0);

	if (ret < 0) {
		/* errno */
		switch (ret) {
			case -EAGAIN:
				x_errno = X_EAGAIN;
				break;
			case -ENOMEM:
				x_errno = X_ENOMEM;
				break;
			default:
				x_errno = X_EUNKNOWN;
				break;
		}
		return -1;
	}

	return ret;

#else
#	error "!!!unknown DEMOZ_PLATFORM!!!"
#endif
}
