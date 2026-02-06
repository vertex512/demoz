/* @file: mman_mm.c
 * #desc:
 *    The implementations of memory management.
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
#include <demoz/c/sys/types.h>
#include <demoz/c/sys/mman.h>
#include <demoz/c/sys/syscall.h>


/* @func: mmap
 * #desc:
 *    map memory pages (assigned by page, 4096 aligned).
 *
 * #1: fixed address / NULL (kernel decision)
 * #2: pages length
 * #3: pages permission
 * #4: mapping flags
 * #5: file descriptor
 * #6: file offset
 * #r: return address (X_MAP_FAILED: errno)
 */
void *C_SYMBOL(mmap)(void *addr, size_t len, int32_t prot, int32_t flags,
		int32_t fd, x_off_t off)
{
#if (DEMOZ_PLATFORM == DEMOZ_PLATFORM_LINUX)

	ssize_t ret = 0;
	int32_t _prot = 0, _flags = 0;

	if ((addr && (uintptr_t)addr & 4095) || len & 4095 || off & 4095) {
		/* errno */
		x_errno = X_EINVAL;
		return X_MAP_FAILED;
	}

	if (prot & X_PROT_EXEC)
		_prot |= PROT_EXEC;
	if (prot & X_PROT_READ)
		_prot |= PROT_READ;
	if (prot & X_PROT_WRITE)
		_prot |= PROT_WRITE;
	if (prot & X_PROT_NONE)
		_prot |= PROT_NONE;
	if (flags & X_MAP_FIXED)
		_flags |= MAP_FIXED;
	if (flags & X_MAP_PRIVATE)
		_flags |= MAP_PRIVATE;
	if (flags & X_MAP_SHARED)
		_flags |= MAP_SHARED;
	if (flags & X_MAP_ANONYMOUS)
		_flags |= MAP_ANONYMOUS;

	ret = C_SYMBOL(syscall_linux)(
		XSYSCALL_mmap,
		addr,
		len,
		_prot,
		_flags,
		fd,
		off);

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
			case -EINVAL:
				x_errno = X_EINVAL;
				break;
			case -ENFILE:
				x_errno = X_ENFILE;
				break;
			case -ENODEV:
				x_errno = X_ENODEV;
				break;
			case -ENOMEM:
				x_errno = X_ENOMEM;
				break;
			case -EOVERFLOW:
				x_errno = X_EOVERFLOW;
				break;
			case -EPERM:
				x_errno = X_EPERM;
				break;
			default:
				x_errno = X_EUNKNOWN;
				break;
		}
		return X_MAP_FAILED;
	}

	return (void *)ret;

#else
#	error "!!!unknown DEMOZ_PLATFORM!!!"
#endif
}

/* @func: munmap
 * #desc:
 *    unmap memory pages (4096 aligned).
 *
 * #1: pages address
 * #2: pages length
 * #r: 0: no error, -1: errno
 */
int32_t C_SYMBOL(munmap)(void *addr, size_t len)
{
#if (DEMOZ_PLATFORM == DEMOZ_PLATFORM_LINUX)

	int32_t ret = 0;

	if ((addr && (uintptr_t)addr & 4095) || len & 4095) {
		/* errno */
		x_errno = X_EINVAL;
		return -1;
	}

	ret = C_SYMBOL(syscall_linux)(
		XSYSCALL_munmap,
		addr,
		len);

	if (ret) {
		/* errno */
		switch (ret) {
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

/* @func: mprotect
 * #desc:
 *    set memory pages permission (4096 aligned).
 *
 * #1: pages address
 * #2: pages length
 * #3: pages permission
 * #r: 0: no error, -1: errno
 */
int32_t C_SYMBOL(mprotect)(void *addr, size_t len, int32_t prot)
{
#if (DEMOZ_PLATFORM == DEMOZ_PLATFORM_LINUX)

	int32_t ret = 0;
	int32_t _prot = 0;

	if ((addr && (uintptr_t)addr & 4095) || len & 4095) {
		/* errno */
		x_errno = X_EINVAL;
		return -1;
	}

	if (prot & X_PROT_EXEC)
		_prot |= PROT_EXEC;
	if (prot & X_PROT_READ)
		_prot |= PROT_READ;
	if (prot & X_PROT_WRITE)
		_prot |= PROT_WRITE;
	if (prot & X_PROT_NONE)
		_prot |= PROT_NONE;

	ret = C_SYMBOL(syscall_linux)(
		XSYSCALL_mprotect,
		addr,
		_prot);

	if (ret < 0) {
		/* errno */
		switch (ret) {
			case -EACCES:
				x_errno = X_EACCES;
				break;
			case -EINVAL:
				x_errno = X_EINVAL;
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
