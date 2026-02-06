/* @file: unistd_rw.c
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
#include <demoz/c/stddef.h>
#include <demoz/c/stdint.h>
#include <demoz/c/errno.h>
#include <demoz/c/sys/unistd.h>
#include <demoz/c/sys/syscall.h>


/* @func: read
 * #desc:
 *    read from a file descriptor.
 *
 * #1: file descriptor
 * #2: buffer
 * #3: buffer length
 * #r: >=0: read length, -1: errno
 */
ssize_t C_SYMBOL(read)(int32_t fd, void *buf, size_t len)
{
#if (DEMOZ_PLATFORM == DEMOZ_PLATFORM_LINUX)

	ssize_t ret = 0;

	ret = C_SYMBOL(syscall_linux)(
		XSYSCALL_read,
		fd,
		buf,
		len);

	if (ret < 0) {
		/* errno */
		switch (ret) {
			case -EAGAIN:
				x_errno = X_EAGAIN;
				break;
			case -EBADF:
				x_errno = X_EBADF;
				break;
			case -EFAULT:
				x_errno = X_EFAULT;
				break;
			case -EINTR:
				x_errno = X_EINTR;
				break;
			case -EINVAL:
				x_errno = X_EINVAL;
				break;
			case -EIO:
				x_errno = X_EIO;
				break;
			case -EISDIR:
				x_errno = X_EISDIR;
				break;
			default:
				x_errno = X_EUNKNOWN;
				break;
		}
		return -1;
	}

#else
#	error "!!!unknown DEMOZ_PLATFORM!!!"
#endif

	return ret;
}

/* @func: write
 * #desc:
 *    write to a file descriptor.
 *
 * #1: file descriptor
 * #2: input buffer
 * #3: buffer length
 * #r: >=0: write length, -1: errno
 */
ssize_t C_SYMBOL(write)(int32_t fd, const void *buf, size_t len)
{
#if (DEMOZ_PLATFORM == DEMOZ_PLATFORM_LINUX)

	ssize_t ret = 0;

	ret = C_SYMBOL(syscall_linux)(
		XSYSCALL_write,
		fd,
		buf,
		len);

	if (ret < 0) {
		/* errno */
		switch (ret) {
			case -EAGAIN:
				x_errno = X_EAGAIN;
				break;
			case -EBADF:
				x_errno = X_EBADF;
				break;
			case -EDESTADDRREQ:
				x_errno = X_EDESTADDRREQ;
				break;
			case -EDQUOT:
				x_errno = X_EDQUOT;
				break;
			case -EFAULT:
				x_errno = X_EFAULT;
				break;
			case -EFBIG:
				x_errno = X_EFBIG;
				break;
			case -EINTR:
				x_errno = X_EINTR;
				break;
			case -EINVAL:
				x_errno = X_EINVAL;
				break;
			case -EIO:
				x_errno = X_EIO;
				break;
			case -ENOSPC:
				x_errno = X_ENOSPC;
				break;
			case -EPERM:
				x_errno = X_EPERM;
				break;
			case -EPIPE:
				x_errno = X_EPIPE;
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
