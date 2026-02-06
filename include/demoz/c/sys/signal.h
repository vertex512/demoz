/* @file: signal.h
 * #desc:
 *    The definitions of signal functions.
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

#ifndef _DEMOZ_C_SYS_SIGNAL_H
#define _DEMOZ_C_SYS_SIGNAL_H

#include <demoz/config.h>
#include <demoz/c/stdint.h>


/* @def: _ */
#define X_SIGZER     0 /* undefined behavior */
#define X_SIGHUP     1 /* hang up */
#define X_SIGINT     2 /* interrupt */
#define X_SIGQUIT    3 /* quit */
#define X_SIGILL     4 /* illegal instruction */
#define X_SIGTRAP    5 /* trace/breakpoint trap */
#define X_SIGABRT    6 /* abort */
#define X_SIGBUS     7 /* bus/address error */
#define X_SIGFPE     8 /* floating-point exception */
#define X_SIGKILL    9 /* kill */
#define X_SIGUSR1   10 /* user-defined signal */
#define X_SIGSEGV   11 /* segmentation violation */
#define X_SIGUSR2   12 /* user-defined signal */
#define X_SIGPIPE   13 /* broken pipe */
#define X_SIGALRM   14 /* alarm clock */
#define X_SIGTERM   15 /* termination */
#define X_SIGSTKFLT 16 /* stack fault on coprocessor */
#define X_SIGCHLD   17 /* child status changed */
#define X_SIGCONT   18 /* continue */
#define X_SIGSTOP   19 /* stop */
#define X_SIGTSTP   20 /* terminal stop */
#define X_SIGTTIN   21 /* background read from tty */
#define X_SIGTTOU   22 /* background write to tty */
#define X_SIGURG    23 /* urgent data available */
#define X_SIGXCPU   24 /* cpu time limit exceeded */
#define X_SIGXFSZ   25 /* file size limit exceeded */
#define X_SIGVTALRM 26 /* virtual alarm clock */
#define X_SIGPROF   27 /* profiling timer expired */
#define X_SIGWINCH  28 /* window resize */
#define X_SIGPOLL   29 /* pollable event possible */
#define X_SIGPWR    30 /* power failure */
#define X_SIGSYS    31 /* bad system call */
#define X_SIGMAX   127 /* signal max */

typedef struct { /* 128 a signals */
	uint8_t _bits[128 / 8];
} x_sigset_t;

typedef struct {
	int32_t _v; /* unimplemented */
} x_siginfo_t;

struct x_sigaction {
	union { /* callback function */
		void (*_sa_handler)(int32_t);
		void (*_sa_sigaction)(int32_t, x_siginfo_t *, void *);
	} u;
	x_sigset_t sa_mask; /* blocking signal set */
	int32_t sa_flags;
};

/* NOTE: blocking signal queued only while callback is executing */

/* add blocking signals */
#define X_SIG_BLOCK 1
/* unblocking signals */
#define X_SIG_UNBLOCK 2
/* set new signal mask */
#define X_SIG_SETMASK 3

/* do not generate SIGCHLD */
#define X_SA_NOCLDSTOP 0x01
/* do not generate zombie processes */
#define X_SA_NOCLDWAIT 0x02
/* use sa_sigaction info */
#define X_SA_SIGINFO 0x04
/* execute on an alternate signal stack */
#define X_SA_ONSTACK 0x08
/* restart an interrupted system call (EINTR) */
#define X_SA_RESTART 0x10
/* set non-blocking signal wait (default wait) */
#define X_SA_NODEFER 0x20
/* one-time signal */
#define X_SA_RESETHAND 0x40

#define X_SIG_DFL ((void (*)(int32_t))0)  /* default signal handling */
#define X_SIG_IGN ((void (*)(int32_t))1)  /* ignore signal */
#define X_SIG_ERR ((void (*)(int32_t))-1) /* error return */
/* end */


#ifdef __cplusplus
extern "C" {
#endif

/* c/sys/signal_diff.c */

extern
int32_t C_SYMBOL(___signal_sig)(int32_t sig)
;

extern
int32_t C_SYMBOL(___signal_sig_inv)(int32_t sig)
;

extern
int32_t C_SYMBOL(___signal_set)(const x_sigset_t *a, void *b)
;

extern
int32_t C_SYMBOL(___signal_set_inv)(const void *a, x_sigset_t *b)
;

/* c/sys/signal_kill.c */

extern
int32_t C_SYMBOL(kill)(int32_t pid, int32_t sig)
;

extern
int32_t C_SYMBOL(killpg)(int32_t pg, int32_t sig)
;

/* c/sys/signal_new.c */

extern
int32_t C_SYMBOL(sigaction)(int32_t sig, const struct x_sigaction *act,
		struct x_sigaction *old)
;

extern
int32_t C_SYMBOL(sigprocmask)(int32_t how, const x_sigset_t *set,
		x_sigset_t *old)
;

extern
int32_t C_SYMBOL(sigpending)(x_sigset_t *set)
;

extern
int32_t C_SYMBOL(sigsuspend)(const x_sigset_t *set)
;

/* c/sys/signal_old.c */

extern
void (*C_SYMBOL(signal)(int32_t sig, void (*handler)(int32_t)))(int32_t)
;

/* c/sys/signal_set.c */

extern
int32_t C_SYMBOL(sigaddset)(x_sigset_t *set, int32_t sig)
;

extern
int32_t C_SYMBOL(sigdelset)(x_sigset_t *set, int32_t sig)
;

extern
int32_t C_SYMBOL(sigismember)(const x_sigset_t *set, int32_t sig)
;

extern
int32_t C_SYMBOL(sigemptyset)(x_sigset_t *set)
;

extern
int32_t C_SYMBOL(sigfillset)(x_sigset_t *set)
;

#ifdef __cplusplus
}
#endif


#endif
