/* @file: mman_sync.c
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


/* @func: msync
 * #desc:
 *    synchronize a file with a memory map.
 *
 * #1: pages address
 * #2: pages length
 * #3: flags
 * #r: 0: no error, -1: errno
 */
int32_t C_SYMBOL(msync)(void *addr, size_t len, int32_t flags)
{
#if (DEMOZ_PLATFORM == DEMOZ_PLATFORM_LINUX)

	int32_t ret = 0;
	int32_t _flags = 0;

	if ((addr && (uintptr_t)addr & 4095) || len & 4095) {
		/* errno */
		x_errno = X_EINVAL;
		return -1;
	}

	if (flags & X_MS_ASYNC)
		_flags |= MS_ASYNC;
	if (flags & X_MS_SYNC)
		_flags |= MS_SYNC;
	if (flags & X_MS_INVALIDATE)
		_flags |= MS_INVALIDATE;

	ret = C_SYMBOL(syscall_linux)(
		XSYSCALL_msync,
		addr,
		len,
		_flags);

	if (ret) {
		/* errno */
		switch (ret) {
			case -EBUSY:
				x_errno = X_EINVAL;
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
