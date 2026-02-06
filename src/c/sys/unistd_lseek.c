/* @file: unistd_lseek.c
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


/* @func: lseek
 * #desc:
 *    repositions the file offset.
 *
 * #1: file descriptor
 * #2: file offset
 * #3: seek type
 * #r: 0: no error, -1: errno
 */
x_off_t C_SYMBOL(lseek)(int32_t fd, x_off_t off, int32_t whence)
{
#if (DEMOZ_PLATFORM == DEMOZ_PLATFORM_LINUX)

	x_off_t ret = 0, _off = 0;

	switch (whence) {
		case X_SEEK_SET:
			whence = SEEK_SET;
			break;
		case X_SEEK_CUR:
			whence = SEEK_CUR;
			break;
		case X_SEEK_END:
			whence = SEEK_END;
			break;
		default:
			/* errno */
			x_errno = X_EINVAL;
			return -1;
	}

#if (DEMOZ_MARCH_BITS == DEMOZ_MARCH_32)

	ret = C_SYMBOL(syscall_linux)(
		XSYSCALL_llseek,
		fd,
		off >> 32,
		off,
		&_off,
		whence);

#elif (DEMOZ_MARCH_BITS == DEMOZ_MARCH_64)

	ret = _off = C_SYMBOL(syscall_linux)(
		XSYSCALL_lseek,
		fd,
		off,
		whence);

#else
#	error "!!!unknown DEMOZ_MARCH_BITS!!!"
#endif

	if (ret < 0) {
		/* errno */
		switch (ret) {
			case -EBADF:
				x_errno = X_EBADF;
				break;
			case -ENXIO:
				x_errno = X_ENXIO;
				break;
			case -EOVERFLOW:
				x_errno = X_EOVERFLOW;
				break;
			case -ESPIPE:
				x_errno = X_ESPIPE;
				break;
			default:
				x_errno = X_EUNKNOWN;
				break;
		}
		return -1;
	}

	return ret = _off;

#else
#	error "!!!unknown DEMOZ_PLATFORM!!!"
#endif
}
