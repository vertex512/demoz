/* @file: mman.h
 * #desc:
 *    The definitions of memory management.
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

#ifndef _DEMOZ_C_SYS_MMAN_H
#define _DEMOZ_C_SYS_MMAN_H

#include <demoz/config.h>
#include <demoz/c/stdint.h>
#include <demoz/c/sys/types.h>


/* @def: _ */
#define X_PROT_EXEC 0x01   /* page can be execute */
#define X_PROT_READ 0x02   /* page can be read */
#define X_PROT_WRITE 0x04  /* page can be write */
#define X_PROT_NONE 0x08   /* page cannot be accessed */

#define X_MAP_FILE 0x00    /* file mapping to page (default) */
#define X_MAP_FIXED 0x01   /* fixed address mapping */
#define X_MAP_PRIVATE 0x02 /* private page mapping (cow) */
#define X_MAP_SHARED 0x04  /* share page mapping */

/* anonymous page mapping (conjunction with MAP_PRIVATE, fd: -1) */
#define X_MAP_ANONYMOUS 0x08

#define X_MAP_FAILED ((void *)-1) /* error return */

#define X_MS_ASYNC 0x01      /* non-blocking synchronous */
#define X_MS_SYNC 0x02       /* blocking synchronous */
#define X_MS_INVALIDATE 0x04 /* synchronous and marked as dirty page */
/* end */


#ifdef __cplusplus
extern "C" {
#endif

/* c/sys/mman_mm.c */

extern
void *C_SYMBOL(mmap)(void *addr, size_t len, int32_t prot, int32_t flags,
		int32_t fd, x_off_t off)
;

extern
int32_t C_SYMBOL(munmap)(void *addr, size_t len)
;

extern
int32_t C_SYMBOL(mprotect)(void *addr, size_t len, int32_t prot)
;

/* c/sys/mman_sync.c */

extern
int32_t C_SYMBOL(msync)(void *addr, size_t len, int32_t flags)
;

#ifdef __cplusplus
}
#endif


#endif
