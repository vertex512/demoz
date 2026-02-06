/* @file: time.h
 * #desc:
 *    The definitions of time functions.
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

#ifndef _DEMOZ_C_SYS_TIME_H
#define _DEMOZ_C_SYS_TIME_H

#include <demoz/config.h>
#include <demoz/c/stdint.h>
#include <demoz/c/sys/types.h>


/* @def: _ */
struct x_timespec {
	x_time_t tv_sec; /* seconds */
	long tv_nsec;  /* nanoseconds */
};

struct x_timeval {
	x_time_t tv_sec;       /* seconds */
	x_suseconds_t tv_usec; /* microseconds */
};

struct x_itimerval {
	struct x_timeval it_interval; /* timer interval */
	struct x_timeval it_value;    /* first start time */
};

#define X_CLOCKS_PER_SEC 1000000

/* calendar/date time (system time) */
#define X_CLOCK_REALTIME 0
/* monotonic clock (not affected by system time) */
#define X_CLOCK_MONOTONIC 1
/* process cpu time */
#define X_CLOCK_PROCESS_CPUTIME_ID 2
/* thread cpu time */
#define X_CLOCK_THREAD_CPUTIME_ID 3

/* system time */
#define X_ITIMER_REAL 0
/* userspace process cpu time */
#define X_ITIMER_VIRTUAL 1
/* userspace and kernelspace process cpu time */
#define X_ITIMER_PROF 2
/* end */


#ifdef __cplusplus
extern "C" {
#endif

/* c/sys/time_clock.c */

extern
int32_t C_SYMBOL(clock_gettime)(int32_t cid, struct x_timespec *ts)
;

extern
int32_t C_SYMBOL(clock_settime)(int32_t cid, struct x_timespec *ts)
;

extern
x_clock_t C_SYMBOL(clock)(void)
;

extern
x_time_t C_SYMBOL(time)(x_time_t *r)
;

/* c/sys/time_sleep.c */

extern
int32_t C_SYMBOL(nanosleep)(const struct x_timespec *req,
		struct x_timespec *rem)
;

/* c/sys/time_timer.c */

extern
int32_t C_SYMBOL(setitimer)(int32_t which, const struct x_itimerval *_new,
		struct x_itimerval *old)
;

extern
int32_t C_SYMBOL(getitimer)(int32_t which, struct x_itimerval *curr)
;

#ifdef __cplusplus
}
#endif


#endif
