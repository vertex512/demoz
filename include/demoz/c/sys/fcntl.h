/* @file: fcntl.h
 * #desc:
 *    The definitions of file control options.
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

#ifndef _DEMOZ_C_SYS_FCNTL_H
#define _DEMOZ_C_SYS_FCNTL_H

#include <demoz/config.h>
#include <demoz/c/stdint.h>
#include <demoz/c/sys/types.h>


/* @def: _
 * duplicate file descriptor */
#define X_F_DUPFD 0
/* duplicate file descriptor with the close-on-exec flag */
#define X_F_DUPFD_CLOEXEC 1

/* get file descriptor flags */
#define X_F_GETFD 2
/* set file descriptor flags */
#define X_F_SETFD 3

/* close file descriptors when executing the exec */
#define X_FD_CLOEXEC 1

/* get file status flags and file access modes */
#define X_F_GETFL 4
/* set file status flags */
#define X_F_SETFL 5

/* get record locking info */
#define X_F_GETLK 6
/* get record locking info */
#define X_F_SETLK 7
/* set record locking info(wait if blocked) */
#define X_F_SETLKW 8

/* get process or process group id to receive SIGIO/SIGURG signal */
#define X_F_GETOWN 9
/* set process or process group id to receive SIGIO/SIGURG signal */
#define X_F_SETOWN 10

struct x_flock {
	int16_t l_type;   /* type of lock F_*LCK */
	int16_t l_whence; /* offset type (LSEEK_*) */
	x_off_t l_start;    /* relative offset */
	x_off_t l_len;      /* size (if 0 then until EOF) */
	x_pid_t l_pid;      /* process id of the process holding the lock */
};

/* unlock */
#define X_F_UNLCK 0
/* read lock */
#define X_F_RDLCK 1
/* write lock */
#define X_F_WRLCK 2

/* mask of read and write */
#define X_O_ACCMODE 0x03

/* read only */
#define X_O_RDONLY 0x00
/* write only */
#define X_O_WRONLY 0x01
/* read and write */
#define X_O_RDWR 0x02
/* open directory for search only (*at() functions) */
#define X_O_SEARCH 0x04

/* close file descriptors when executing the exec */
#define X_O_CLOEXEC 0x08
/* create file if it does not exist */
#define X_O_CREAT 0x10
/* exclusive flag */
#define X_O_EXCL 0x20

/* do not assign controlling terminal (SIGTTOU/SIGTTIN signal) */
#define X_O_NOCTTY 0x40
/* do not follow symbolic links */
#define X_O_NOFOLLOW 0x80

/* truncate flag */
#define X_O_TRUNC 0x100
/* set append mode */
#define X_O_APPEND 0x200
/* non-blocking mode */
#define X_O_NONBLOCK 0x400

/* write according to synchronized I/O data integrity */
#define X_O_SYNC 0x800
/* synchronized writing */
#define X_O_DSYNC 0x1000
/* synchronized reading */
#define X_O_RSYNC 0x2000

#define X_AT_FDCWD -100
/* end */


#ifdef __cplusplus
extern "C" {
#endif

/* c/sys/fcntl_diff.c */

extern
uint32_t C_SYMBOL(___fcntl_oflags)(uint32_t flags)
;

extern
uint32_t C_SYMBOL(___fcntl_oflags_inv)(uint32_t flags)
;

extern
int32_t C_SYMBOL(___fcntl_flock)(struct x_flock *a, void *b)
;

extern
int32_t C_SYMBOL(___fcntl_flock_inv)(void *a, struct x_flock *b)
;

/* c/sys/fcntl_fd.c */

extern
int32_t C_SYMBOL(fcntl)(int32_t fd, int32_t cmd, ...)
;

/* c/sys/fcntl_open.c */

extern
int32_t C_SYMBOL(openat)(int32_t fd, const char *path, int32_t flags, ...)
;

extern
int32_t C_SYMBOL(open)(const char *path, int32_t flags, ...)
;

extern
int32_t C_SYMBOL(creat)(const char *path, x_mode_t mode)
;

#ifdef __cplusplus
}
#endif


#endif
