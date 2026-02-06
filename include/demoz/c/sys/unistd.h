/* @file: unistd.h
 * #desc:
 *    The definitions of unix standard functions.
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

#ifndef _DEMOZ_C_SYS_UNISTD_H
#define _DEMOZ_C_SYS_UNISTD_H

#include <demoz/config.h>
#include <demoz/c/stddef.h>
#include <demoz/c/stdint.h>
#include <demoz/c/sys/types.h>


/* @def: _
 * absolute offset */
#define X_SEEK_SET 1
/* relative offset */
#define X_SEEK_CUR 2
/* tail start offset */
#define X_SEEK_END 3
/* end */


#ifdef __cplusplus
extern "C" {
#endif

/* c/sys/unistd_close.c */

extern
int32_t C_SYMBOL(close)(int32_t fd)
;

/* c/sys/unistd_fork.c */

extern
x_pid_t C_SYMBOL(fork)(void)
;

/* c/sys/unistd_lseek.c */

extern
x_off_t C_SYMBOL(lseek)(int32_t fd, x_off_t off, int32_t whence)
;

/* c/sys/unistd_pid.c */

extern
x_pid_t C_SYMBOL(getpid)(void)
;

extern
x_pid_t C_SYMBOL(getppid)(void)
;

extern
x_pid_t C_SYMBOL(getpgid)(x_pid_t pid)
;

extern
int32_t C_SYMBOL(setpgid)(x_pid_t pid, x_pid_t pg)
;

/* c/sys/unistd_rw.c */

extern
ssize_t C_SYMBOL(read)(int32_t fd, void *buf, size_t len)
;

extern
ssize_t C_SYMBOL(write)(int32_t fd, const void *buf, size_t len)
;

/* c/sys/unistd_sleep.c */

extern
int32_t C_SYMBOL(sleep)(uint32_t n)
;

#ifdef __cplusplus
}
#endif


#endif
