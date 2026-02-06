/* @file: fcntl_open.c
 * #desc:
 *    The implementations of file control options.
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
#include <demoz/c/stdarg.h>
#include <demoz/c/errno.h>
#include <demoz/c/sys/types.h>
#include <demoz/c/sys/stat.h>
#include <demoz/c/sys/fcntl.h>
#include <demoz/c/sys/syscall.h>


/* @func: _openat (static)
 * #desc:
 *    open and possibly create a file (TOCTOU).
 *
 * #1: directory file descriptor
 * #2: path name
 * #3: open flags
 * #4: file mode (O_CREAT)
 * #r: >=0: file descriptor, -1: errno
 */
static int32_t _openat(int32_t fd, const char *path, int32_t flags,
		x_mode_t mode)
{
#if (DEMOZ_PLATFORM == DEMOZ_PLATFORM_LINUX)

	int32_t ret = 0;
	uint32_t _flags = O_LARGEFILE;
	uint32_t _mode = 0;

	_flags = C_SYMBOL(___fcntl_oflags)(flags);

	if (flags & X_O_CREAT)
		_mode = C_SYMBOL(___stat_mode)(mode);
	if (fd == X_AT_FDCWD)
		fd = AT_FDCWD;

	ret = C_SYMBOL(syscall_linux)(
		XSYSCALL_openat,
		fd,
		path,
		_flags,
		_mode);

	if (ret < 0) {
		/* errno */
		switch (ret) {
			case -EACCES:
				x_errno = X_EACCES;
				break;
			case -EBADF:
				x_errno = X_EBADF;
				break;
			case -EBUSY:
				x_errno = X_EBUSY;
				break;
			case -EDQUOT:
				x_errno = X_EDQUOT;
				break;
			case -EEXIST:
				x_errno = X_EEXIST;
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
			case -EISDIR:
				x_errno = X_ENOLCK;
				break;
			case -ELOOP:
				x_errno = X_ELOOP;
				break;
			case -EMFILE:
				x_errno = X_EMFILE;
				break;
			case -ENAMETOOLONG:
				x_errno = X_ENAMETOOLONG;
				break;
			case -ENFILE:
				x_errno = X_ENFILE;
				break;
			case -ENODEV:
				x_errno = X_ENODEV;
				break;
			case -ENOENT:
				x_errno = X_ENOENT;
				break;
			case -ENOMEM:
				x_errno = X_ENOMEM;
				break;
			case -ENOTDIR:
				x_errno = X_ENOTDIR;
				break;
			case -ENXIO:
				x_errno = X_ENXIO;
				break;
			case -EOPNOTSUPP:
				x_errno = X_EOPNOTSUPP;
				break;
			case -EOVERFLOW:
				x_errno = X_EOVERFLOW;
				break;
			case -EPERM:
				x_errno = X_EPERM;
				break;
			case -EROFS:
				x_errno = X_EROFS;
				break;
			case -ETXTBSY:
				x_errno = X_ETXTBSY;
				break;
			case -EWOULDBLOCK:
				x_errno = X_EWOULDBLOCK;
				break;
			default:
				x_errno = X_EUNKNOWN;
				break;
		}
		return -1;
	}

	if (flags & X_O_CLOEXEC)
		C_SYMBOL(fcntl)(ret, X_F_SETFD, X_FD_CLOEXEC);

	return ret;

#else
#	error "!!!unknown DEMOZ_PLATFORM!!!"
#endif
}

/* @func: openat
 * #desc:
 *    open and possibly create a file (TOCTOU).
 *
 * #1: directory file descriptor
 * #2: path name
 * #3: open flags
 * #N: file mode (O_CREAT)
 * #r: >=0: file descriptor, -1: errno
 */
int32_t C_SYMBOL(openat)(int32_t fd, const char *path, int32_t flags, ...)
{
	va_list ap;
	va_start(ap, flags);
	x_mode_t mode = va_arg(ap, x_mode_t);

	return _openat(fd, path, flags, mode);
}

/* @func: open
 * #desc:
 *    open and possibly create a file.
 *
 * #1: path name
 * #2: open flags
 * #N: file mode (O_CREAT)
 * #r: >=0: file descriptor, -1: errno
 */
int32_t C_SYMBOL(open)(const char *path, int32_t flags, ...)
{
	va_list ap;
	va_start(ap, flags);
	x_mode_t mode = va_arg(ap, x_mode_t);

	return _openat(X_AT_FDCWD, path, flags, mode);
}

/* @func: creat
 * #desc:
 *    possibly create a file.
 *
 * #1: path name
 * #2: file mode
 * #r: 0: no errno, -1: errno
 */
int32_t C_SYMBOL(creat)(const char *path, x_mode_t mode)
{
	return C_SYMBOL(open)(path, X_O_CREAT | X_O_WRONLY | X_O_TRUNC, mode);
}
