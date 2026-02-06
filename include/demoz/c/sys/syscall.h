/* @file: syscall.h
 * #desc:
 *    The definitions of system call.
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

#ifndef _DEMOZ_C_SYS_SYSCALL_H
#define _DEMOZ_C_SYS_SYSCALL_H

#include <demoz/config.h>
#if (DEMOZ_PLATFORM == DEMOZ_PLATFORM_LINUX)
#	if (DEMOZ_MARCH_TYPE == DEMOZ_MARCH_X86_32)
#		undef __i386__
#		define __i386__
#		include <asm/unistd.h>
#		include <asm/signal.h>
#		include <asm/mman.h>
#	elif (DEMOZ_MARCH_TYPE == DEMOZ_MARCH_X86_64)
#		undef __i386__
#		undef __ILP32__
#		include <asm/unistd.h>
#		include <asm/signal.h>
#		include <asm/mman.h>
#	elif (DEMOZ_MARCH_TYPE == DEMOZ_MARCH_ARM_32)
#		undef __ARM_EABI__
#		define __ARM_EABI__
#		include <asm/unistd.h>
#		include <asm/signal.h>
#		include <asm/mman.h>
#	elif (DEMOZ_MARCH_TYPE == DEMOZ_MARCH_ARM_64)
#		include <asm/unistd.h>
#		include <asm/signal.h>
#		include <asm/mman.h>
#	elif (DEMOZ_MARCH_TYPE == DEMOZ_MARCH_RISCV_32)
#		undef __LP64__
#		include <asm/unistd.h>
#		include <asm/signal.h>
#		include <asm/mman.h>
#	elif (DEMOZ_MARCH_TYPE == DEMOZ_MARCH_RISCV_64)
#		undef __LP64__
#		define __LP64__
#		include <asm/unistd.h>
#		include <asm/signal.h>
#		include <asm/mman.h>
#	else
#		error "!!!unknown DEMOZ_MARCH_TYPE!!!"
#	endif
#	include <linux/time.h>
#	include <linux/mman.h>
#	include <linux/fcntl.h>
#	include <linux/fs.h>
#	include <linux/stat.h>
#	include <linux/errno.h>
#else
#	error "!!!unknown DEMOZ_PLATFORM!!!"
#endif


/* @def: _
 * signal.h */
#define XSYSCALL_kill (__NR_kill)
#define XSYSCALL_rt_sigaction (__NR_rt_sigaction)
#define XSYSCALL_rt_sigprocmask (__NR_rt_sigprocmask)
#define XSYSCALL_rt_sigpending (__NR_rt_sigpending)
#define XSYSCALL_rt_sigsuspend (__NR_rt_sigsuspend)

/* time.h */
#define XSYSCALL_clock_gettime (__NR_clock_gettime)
#define XSYSCALL_clock_settime (__NR_clock_settime)
#define XSYSCALL_nanosleep (__NR_nanosleep)
#define XSYSCALL_setitimer (__NR_setitimer)
#define XSYSCALL_getitimer (__NR_getitimer)

/* mman.h */
#define XSYSCALL_mmap (__NR_mmap)
#define XSYSCALL_munmap (__NR_munmap)
#define XSYSCALL_mprotect (__NR_mprotect)
#define XSYSCALL_msync (__NR_msync)

/* fcntl.h */
#define XSYSCALL_fcntl (__NR_fcntl)
#define XSYSCALL_creat (__NR_creat)
#define XSYSCALL_open (__NR_open)
#define XSYSCALL_openat (__NR_openat)

/* unistd.h */
#define XSYSCALL_exit (__NR_exit)
#define XSYSCALL_exit_group (__NR_exit_group)
#define XSYSCALL_clone (__NR_clone)
#define XSYSCALL_read (__NR_read)
#define XSYSCALL_write (__NR_write)
#define XSYSCALL_close (__NR_close)

#define XSYSCALL_lseek (__NR_lseek)
#define XSYSCALL_llseek (__NR_llseek)

#define XSYSCALL_unlink (__NR_unlink)
#define XSYSCALL_unlinkat (__NR_unlinkat)
#define XSYSCALL_rmdir (__NR_rmdir)
#define XSYSCALL_execve (__NR_execve)
#define XSYSCALL_execveat (__NR_execveat)
#define XSYSCALL_getpid (__NR_getpid)
#define XSYSCALL_getppid (__NR_getppid)
#define XSYSCALL_gettid (__NR_gettid)
#define XSYSCALL_getpgid (__NR_getpgid)
#define XSYSCALL_setpgid (__NR_setpgid)

/* stat.h */
#define XSYSCALL_stat (__NR_stat)
#define XSYSCALL_lstat (__NR_lstat)
#define XSYSCALL_fstat (__NR_fstat)
#define XSYSCALL_fstatat (__NR_fstatat)
#define XSYSCALL_mkdir (__NR_mkdir)
#define XSYSCALL_mkdirat (__NR_mkdirat)
#define XSYSCALL_mknod (__NR_mknod)
#define XSYSCALL_mknodat (__NR_mknodat)
#define XSYSCALL_chmod (__NR_chmod)
#define XSYSCALL_fchmod (__NR_fchmod)
#define XSYSCALL_fchmodat (__NR_fchmodat)
#define XSYSCALL_futimens (__NR_futimesat)
#define XSYSCALL_futimensat (__NR_utimensat)
#define XSYSCALL_umask (__NR_umask)
/* end */


#ifdef __cplusplus
extern "C" {
#endif

/* c/sys/syscall_linux.S */
extern
long C_SYMBOL(syscall_linux)(long n, ...)
;

#ifdef __cplusplus
}
#endif


#endif
