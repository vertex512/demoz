/* @file: fcntl_fd.c
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
#include <demoz/c/stddef.h>
#include <demoz/c/stdint.h>
#include <demoz/c/stdarg.h>
#include <demoz/c/errno.h>
#include <demoz/c/sys/unistd.h>
#include <demoz/c/sys/fcntl.h>
#include <demoz/c/sys/syscall.h>


/* @func: fcntl
 * #desc:
 *    file descriptor operations.
 *
 * #1: file descriptor
 * #2: command
 * #N: command argument
 * #r: return value of command
 */
int32_t C_SYMBOL(fcntl)(int32_t fd, int32_t cmd, ...)
{
#if (DEMOZ_PLATFORM == DEMOZ_PLATFORM_LINUX)

	int32_t ret = 0;

	va_list ap;
	va_start(ap, cmd);
	unsigned long arg = va_arg(ap, unsigned long);

	unsigned long _arg = 0;
	int32_t _cmd, _ret = 0;
	struct flock _flock;

	switch (cmd) {
		case X_F_DUPFD:
			_cmd = F_DUPFD;
			break;
		case X_F_DUPFD_CLOEXEC:
			_cmd = F_DUPFD_CLOEXEC;
			break;
		case X_F_GETFD:
			_cmd = F_GETLK;
			break;
		case X_F_SETFD:
			_cmd = F_SETFD;
			if (arg == X_FD_CLOEXEC) {
				_arg = FD_CLOEXEC;
			} else {
				/* errno */
				x_errno = X_EINVAL;
				return -1;
			}
			break;
		case X_F_GETFL:
			_cmd = F_GETFL;
			break;
		case X_F_SETFL:
			_cmd = F_SETFL;
			_arg = C_SYMBOL(___fcntl_oflags)(arg);
			break;
		case X_F_GETLK:
			_cmd = F_GETLK;
			break;
		case X_F_SETLK:
			_cmd = F_SETLK;
			break;
		case X_F_SETLKW:
			_cmd = F_SETLKW;
			break;
		case X_F_GETOWN:
			_cmd = F_GETOWN;
			_arg = arg;
			break;
		case X_F_SETOWN:
			_cmd = F_SETOWN;
			_arg = arg;
			break;
		default:
			/* errno */
			x_errno = X_EINVAL;
			return -1;
	}

	switch (cmd) {
		case X_F_GETLK:
		case X_F_SETLK:
		case X_F_SETLKW:
			if (C_SYMBOL(___fcntl_flock)((struct x_flock *)arg,
					&_flock)) {
				/* errno */
				x_errno = X_EINVAL;
				return -1;
			}
			_arg = (unsigned long)&_flock;
		default:
			break;
	}

	_ret = C_SYMBOL(syscall_linux)(
		XSYSCALL_fcntl,
		fd,
		_cmd,
		_arg);

	switch (cmd) {
		case X_F_DUPFD:
		case X_F_DUPFD_CLOEXEC:
			ret = _ret;
			break;
		case X_F_GETFD:
			if (_ret == X_FD_CLOEXEC) {
				ret = FD_CLOEXEC;
			} else {
				/* errno */
				x_errno = X_EINVAL;
				return -1;
			}
			break;
		case X_F_GETFL:
			/* RDONLY is zero */
			ret = C_SYMBOL(___fcntl_oflags_inv)(_ret);
			break;
		case X_F_GETLK:
			if (C_SYMBOL(___fcntl_flock_inv)(&_flock,
					(struct x_flock *)arg)) {
				/* errno */
				x_errno = X_EINVAL;
				return -1;
			}
		default:
			break;
	}

	if (ret < 0) {
		/* errno */
		switch (ret) {
			case -EACCES:
				x_errno = X_EACCES;
				break;
			case -EAGAIN:
				x_errno = X_EAGAIN;
				break;
			case -EBADF:
				x_errno = X_EBADF;
				break;
			case -EBUSY:
				x_errno = X_EBUSY;
				break;
			case -EDEADLK:
				x_errno = X_EDEADLK;
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
			case -ENOLCK:
				x_errno = X_ENOLCK;
				break;
			case -ENOTDIR:
				x_errno = X_ENOTDIR;
				break;
			case -EPERM:
				x_errno = X_EPERM;
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
