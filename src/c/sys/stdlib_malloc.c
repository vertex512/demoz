/* @file: stdlib_malloc.c
 * #desc:
 *    The implementations of standard library.
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
#include <demoz/c/atomic.h>
#include <demoz/c/sys/mman.h>
#include <demoz/c/sys/umalloc.h>
#include <demoz/c/sys/stdlib.h>


/* @def: _ */
static void *_alloc(size_t size, void *arg) {
	(void)arg;

	void *p = C_SYMBOL(mmap)(NULL, (size + 4095) & ~4095UL,
		X_PROT_READ | X_PROT_WRITE,
		X_MAP_PRIVATE | X_MAP_ANONYMOUS, -1, 0);

	return (p == X_MAP_FAILED) ? NULL : p;
}

static void _free(void *p, size_t size, void *arg) {
	(void)arg;

	C_SYMBOL(munmap)(p, (size + 4095) & ~4095UL);
}

UMALLOC_NEW(___global_umalloc_ctx, _alloc, _free, NULL);

static volatile int32_t _lock = 0;
/* end */

/* @func: malloc
 * #desc:
 *    heap memory allocator.
 *
 * #1: alloc length
 * #r: heap pointer
 */
void *C_SYMBOL(malloc)(size_t size)
{
	int32_t lock = C_SYMBOL(atomic_cas)(&_lock, 0, 1);
	while (lock && C_SYMBOL(atomic_cas)(&_lock, 0, 1));

	void *p = C_SYMBOL(___umalloc)(&___global_umalloc_ctx, size);

	while (!C_SYMBOL(atomic_cas)(&_lock, 1, 0));

	return p;
}

/* @func: free
 * #desc:
 *    heap memory free.
 *
 * #1: heap pointer
 */
void C_SYMBOL(free)(void *p)
{
	if (!p)
		return;

	int32_t lock = C_SYMBOL(atomic_cas)(&_lock, 0, 1);
	while (lock && C_SYMBOL(atomic_cas)(&_lock, 0, 1));

	C_SYMBOL(___ufree)(&___global_umalloc_ctx, p);

	while (!C_SYMBOL(atomic_cas)(&_lock, 1, 0));
}
