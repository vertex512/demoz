/* @file: types.h
 * #desc:
 *    The definitions of system data types.
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

#ifndef _DEMOZ_C_SYS_TYPES_H
#define _DEMOZ_C_SYS_TYPES_H

#include <demoz/config.h>
#include <demoz/c/stddef.h>
#include <demoz/c/stdint.h>


/* @def: _ */
typedef int64_t x_off_t;

typedef int32_t x_id_t;
typedef x_id_t x_pid_t;
typedef x_id_t x_uid_t;
typedef x_id_t x_gid_t;

typedef size_t x_dev_t;
typedef size_t x_ino_t;
typedef size_t x_nlink_t;
typedef long x_blksize_t;
typedef ssize_t x_blkcnt_t;

typedef uint32_t x_mode_t;

typedef int64_t x_time_t;
typedef int64_t x_suseconds_t;

typedef long x_clock_t;
typedef int32_t x_clockid_t;
/* end */


#endif
