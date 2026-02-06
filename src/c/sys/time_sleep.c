/* @file: time_sleep.c
 * #desc:
 *    The implementations of time functions.
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
#include <demoz/c/sys/time.h>
#include <demoz/c/sys/syscall.h>


/* @func: nanosleep
 * #desc:
 *    nanoseconds sleep function.
 *
 * #1: request time
 * #2: remaining time / NULL
 * #r: 0: no error, -1: errno
 */
int32_t C_SYMBOL(nanosleep)(const struct x_timespec *req,
		struct x_timespec *rem)
{
#if (DEMOZ_PLATFORM == DEMOZ_PLATFORM_LINUX)

	int32_t ret = 0;

	ret = C_SYMBOL(syscall_linux)(
		XSYSCALL_nanosleep,
		req,
		rem);

	if (ret) {
		/* errno */
		switch (ret) {
			case -EFAULT:
				x_errno = X_EFAULT;
				break;
			case -EINVAL:
				x_errno = X_EINVAL;
				break;
			case -EINTR:
				x_errno = X_EINTR;
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
