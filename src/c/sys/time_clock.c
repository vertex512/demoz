/* @file: time_clock.c
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


/* @func: clock_gettime
 * #desc:
 *    get the clock and time function.
 *
 * #1: clock type
 * #2: timespec struct
 * #r: 0: no error, -1: errno
 */
int32_t C_SYMBOL(clock_gettime)(int32_t cid, struct x_timespec *ts)
{
#if (DEMOZ_PLATFORM == DEMOZ_PLATFORM_LINUX)

	int32_t ret = 0;

	switch (cid) {
		case X_CLOCK_REALTIME:
			cid = CLOCK_REALTIME;
			break;
		case X_CLOCK_MONOTONIC:
			cid = CLOCK_MONOTONIC;
			break;
		case X_CLOCK_PROCESS_CPUTIME_ID:
			cid = CLOCK_PROCESS_CPUTIME_ID;
			break;
		case X_CLOCK_THREAD_CPUTIME_ID:
			cid = CLOCK_THREAD_CPUTIME_ID;
			break;
		default:
			/* errno */
			x_errno = X_EINVAL;
			return -1;
	}

	ret = C_SYMBOL(syscall_linux)(
		XSYSCALL_clock_gettime,
		cid,
		ts);

	if (ret) {
		/* errno */
		switch (ret) {
			case -EFAULT:
				x_errno = X_EFAULT;
				break;
			case -EINVAL:
				x_errno = X_EINVAL;
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

/* @func: clock_settime
 * #desc:
 *    set the clock and time function.
 *
 * #1: clock type
 * #2: timespec struct
 * #r: 0: no error, -1: errno
 */
int32_t C_SYMBOL(clock_settime)(int32_t cid, struct x_timespec *ts)
{
#if (DEMOZ_PLATFORM == DEMOZ_PLATFORM_LINUX)

	int32_t ret = 0;

	switch (cid) {
		case X_CLOCK_REALTIME:
			cid = CLOCK_REALTIME;
			break;
		default:
			/* errno */
			x_errno = X_EINVAL;
			return -1;
	}

	ret = C_SYMBOL(syscall_linux)(
		XSYSCALL_clock_settime,
		cid,
		ts);

	if (ret) {
		/* errno */
		switch (ret) {
			case -EFAULT:
				x_errno = X_EFAULT;
				break;
			case -EINVAL:
				x_errno = X_EINVAL;
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

/* @func: clock
 * #desc:
 *    get the determine cpu time function.
 *
 * #r: >0: cpu time (usec), -1: errno
 */
x_clock_t C_SYMBOL(clock)(void)
{
	x_clock_t ret = 0;
	struct x_timespec st;

	ret = C_SYMBOL(clock_gettime)(X_CLOCK_PROCESS_CPUTIME_ID, &st);
	if (ret)
		return ret;

	ret = st.tv_sec * 1000000;
	ret += st.tv_nsec / 1000;

	return ret;
}

/* @func: time
 * #desc:
 *    get determine system time function.
 *
 * #1: result time / NULL
 * #r: >0: system time (sec), -1: errno
 */
x_time_t C_SYMBOL(time)(x_time_t *r)
{
	x_clock_t ret = 0;
	struct x_timespec st;

	ret = C_SYMBOL(clock_gettime)(X_CLOCK_REALTIME, &st);
	if (ret)
		return ret;

	ret = st.tv_sec;
	if (r)
		*r = ret;

	return ret;
}
