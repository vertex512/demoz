/* @file: fcntl_diff.c
 * #desc:
 *    The implementations of file control options difference conversion.
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
#include <demoz/c/sys/unistd.h>
#include <demoz/c/sys/fcntl.h>
#include <demoz/c/sys/syscall.h>


/* @func: ___fcntl_oflags
 * #desc:
 *    open flags difference conversion.
 *
 * #1: open flags
 * #r: open flags
 */
uint32_t C_SYMBOL(___fcntl_oflags)(uint32_t flags)
{
#if (DEMOZ_PLATFORM == DEMOZ_PLATFORM_LINUX)

	uint32_t ret = O_RDONLY;

	if (flags & X_O_WRONLY)
		ret |= O_WRONLY;
	if (flags & X_O_RDWR)
		ret |= O_RDWR;
	if (flags & X_O_SEARCH)
		ret |= O_DIRECTORY;
	if (flags & X_O_CLOEXEC)
		ret |= O_CLOEXEC;
	if (flags & X_O_CREAT)
		ret |= O_CREAT;
	if (flags & X_O_EXCL)
		ret |= O_EXCL;
	if (flags & X_O_NOCTTY)
		ret |= O_NOCTTY;
	if (flags & X_O_NOFOLLOW)
		ret |= O_NOFOLLOW;
	if (flags & X_O_TRUNC)
		ret |= O_TRUNC;
	if (flags & X_O_APPEND)
		ret |= O_APPEND;
	if (flags & X_O_NONBLOCK)
		ret |= O_NONBLOCK;
	if (flags & X_O_SYNC)
		ret |= O_SYNC;

	return ret;

#else
#	error "!!!unknown DEMOZ_PLATFORM!!!"
#endif
}

/* @func: ___fcntl_oflags_inv
 * #desc:
 *    open flags difference inverse conversion.
 *
 * #1: open flags
 * #r: open flags
 */
uint32_t C_SYMBOL(___fcntl_oflags_inv)(uint32_t flags)
{
#if (DEMOZ_PLATFORM == DEMOZ_PLATFORM_LINUX)

	uint32_t ret = X_O_RDONLY;

	if (flags & O_WRONLY)
		ret |= X_O_WRONLY;
	if (flags & O_RDWR)
		ret |= X_O_RDWR;
	if (flags & O_DIRECTORY)
		ret |= X_O_SEARCH;
	if (flags & O_CLOEXEC)
		ret |= X_O_CLOEXEC;
	if (flags & O_CREAT)
		ret |= X_O_CREAT;
	if (flags & O_EXCL)
		ret |= X_O_EXCL;
	if (flags & O_NOCTTY)
		ret |= X_O_NOCTTY;
	if (flags & O_NOFOLLOW)
		ret |= X_O_NOFOLLOW;
	if (flags & O_TRUNC)
		ret |= X_O_TRUNC;
	if (flags & O_APPEND)
		ret |= X_O_APPEND;
	if (flags & O_NONBLOCK)
		ret |= X_O_NONBLOCK;
	if (flags & O_SYNC)
		ret |= X_O_SYNC;

	return ret;

#else
#	error "!!!unknown DEMOZ_PLATFORM!!!"
#endif
}

/* @func: ___fcntl_flock
 * #desc:
 *    flock struct difference conversion.
 *
 * #1: flock struct
 * #2: flock struct
 * #r: 0: no error, -1: error
 */
int32_t C_SYMBOL(___fcntl_flock)(struct x_flock *a, void *b)
{
#if (DEMOZ_PLATFORM == DEMOZ_PLATFORM_LINUX)

	struct flock *_b = b;

	switch (a->l_type) {
		case X_F_UNLCK:
			_b->l_type = F_UNLCK;
			break;
		case X_F_RDLCK:
			_b->l_type = F_RDLCK;
			break;
		case X_F_WRLCK:
			_b->l_type = F_WRLCK;
			break;
		default:
			return -1;
	}

	switch (a->l_whence) {
		case X_SEEK_SET:
			_b->l_whence = SEEK_SET;
			break;
		case X_SEEK_CUR:
			_b->l_whence = SEEK_CUR;
			break;
		case X_SEEK_END:
			_b->l_whence = SEEK_END;
			break;
		default:
			return -1;
	}

	_b->l_start = a->l_start;
	_b->l_len = a->l_len;
	_b->l_pid = a->l_pid;

	return 0;

#else
#	error "!!!unknown DEMOZ_PLATFORM!!!"
#endif
}

/* @func: ___fcntl_flock_inv
 * #desc:
 *    flock struct difference inverse conversion.
 *
 * #1: flock struct
 * #2: flock struct
 * #r: 0: no error, -1: error
 */
int32_t C_SYMBOL(___fcntl_flock_inv)(void *a, struct x_flock *b)
{
#if (DEMOZ_PLATFORM == DEMOZ_PLATFORM_LINUX)

	struct flock *_a = a;

	switch (_a->l_type) {
		case F_UNLCK:
			b->l_type = X_F_UNLCK;
			break;
		case F_RDLCK:
			b->l_type = X_F_RDLCK;
			break;
		case F_WRLCK:
			b->l_type = X_F_WRLCK;
			break;
		default:
			return -1;
	}

	switch (_a->l_whence) {
		case SEEK_SET:
			b->l_whence = X_SEEK_SET;
			break;
		case SEEK_CUR:
			b->l_whence = X_SEEK_CUR;
			break;
		case SEEK_END:
			b->l_whence = X_SEEK_END;
			break;
		default:
			return -1;
	}

	b->l_start = _a->l_start;
	b->l_len = _a->l_len;
	b->l_pid = _a->l_pid;

	return 0;

#else
#	error "!!!unknown DEMOZ_PLATFORM!!!"
#endif
}
