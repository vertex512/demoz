/* @file: signal_kill.c
 * #desc:
 *    The implementations of signal functions.
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
#include <demoz/c/sys/signal.h>
#include <demoz/c/sys/syscall.h>


/* @func: kill
 * #desc:
 *    send a signal to process or process group.
 *
 * #1: process id (0/-pid: process group)
 * #2: signal number
 * #r: 0: no error, -1: errno
 */
int32_t C_SYMBOL(kill)(int32_t pid, int32_t sig)
{
#if (DEMOZ_PLATFORM == DEMOZ_PLATFORM_LINUX)

	int32_t ret = 0;

	if (sig < X_SIGZER || sig > X_SIGMAX) {
		/* errno */
		x_errno = X_EINVAL;
		return -1;
	}

	sig = C_SYMBOL(___signal_sig)(sig);

	ret = C_SYMBOL(syscall_linux)(
		XSYSCALL_kill,
		pid,
		sig);

	if (ret) {
		/* errno */
		switch (ret) {
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

/* @func: killpg
 * #desc:
 *    send a signal to process group.
 *
 * #1: process id
 * #2: signal number
 * #r: 0: no error, -1: errno
 */
int32_t C_SYMBOL(killpg)(int32_t pg, int32_t sig)
{
	return C_SYMBOL(kill)(-pg, sig);
}
