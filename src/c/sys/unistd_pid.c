/* @file: unistd_pid.c
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
#include <demoz/c/stdint.h>
#include <demoz/c/errno.h>
#include <demoz/c/sys/types.h>
#include <demoz/c/sys/unistd.h>
#include <demoz/c/sys/syscall.h>


/* @func: getpid
 * #desc:
 *    get the process id.
 *
 * #r: process id
 */
x_pid_t C_SYMBOL(getpid)(void)
{
#if (DEMOZ_PLATFORM == DEMOZ_PLATFORM_LINUX)

	return C_SYMBOL(syscall_linux)(XSYSCALL_getpid);

#else
#	error "!!!unknown DEMOZ_PLATFORM!!!"
#endif
}

/* @func: getppid
 * #desc:
 *    get the parent process id of the process.
 *
 * #r: >=0: parent process id
 */
x_pid_t C_SYMBOL(getppid)(void)
{
#if (DEMOZ_PLATFORM == DEMOZ_PLATFORM_LINUX)

	return C_SYMBOL(syscall_linux)(XSYSCALL_getppid);

#else
#	error "!!!unknown DEMOZ_PLATFORM!!!"
#endif
}

/* @func: getpgid
 * #desc:
 *    get the process group id of the process.
 *
 * #1: process id
 * #r: >=0: process group id, -1: errno
 */
x_pid_t C_SYMBOL(getpgid)(x_pid_t pid)
{
#if (DEMOZ_PLATFORM == DEMOZ_PLATFORM_LINUX)

	x_pid_t ret = 0;

	if (pid < 0) {
		/* errno */
		x_errno = X_EINVAL;
		return -1;
	}

	ret = C_SYMBOL(syscall_linux)(
		XSYSCALL_getpgid,
		pid);

	if (ret < 0) {
		/* errno */
		switch (ret) {
			case -EINVAL:
				x_errno = X_EINVAL;
				break;
			case -ESRCH:
				x_errno = X_ESRCH;
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

/* @func: setpgid
 * #desc:
 *    set the process group id of the process.
 *
 * #1: process id
 * #r: 0: no error, -1: errno
 */
int32_t C_SYMBOL(setpgid)(x_pid_t pid, x_pid_t pg)
{
#if (DEMOZ_PLATFORM == DEMOZ_PLATFORM_LINUX)

	int32_t ret = 0;

	if (pid < 0) {
		/* errno */
		x_errno = X_EINVAL;
		return -1;
	}

	ret = C_SYMBOL(syscall_linux)(
		XSYSCALL_setpgid,
		pid,
		pg);

	if (ret < 0) {
		/* errno */
		switch (ret) {
			case -EACCES:
				x_errno = X_EACCES;
				break;
			case -EINVAL:
				x_errno = X_EINVAL;
				break;
			case -EPERM:
				x_errno = X_EPERM;
				break;
			case -ESRCH:
				x_errno = X_ESRCH;
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
