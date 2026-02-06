/* @file: config.h
 * #desc:
 *    The definitions of demoz configuration.
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

#ifndef _DEMOZ_CONFIG_H
#define _DEMOZ_CONFIG_H


/* @def: _
 * symbol prefix */
#define F_SYMBOL(_x) f_##_x
#define C_SYMBOL(_x) c_##_x

/* demoz byte order default to little-endian */

#define DEMOZ_MARCH_32 1
#define DEMOZ_MARCH_64 2

#ifndef DEMOZ_MARCH_BITS
#	ifdef __SIZEOF_POINTER__
#		if (__SIZEOF_POINTER__ == 4)
#			define DEMOZ_MARCH_BITS DEMOZ_MARCH_32
#		elif (__SIZEOF_POINTER__ == 8)
#			define DEMOZ_MARCH_BITS DEMOZ_MARCH_64
#		else
#			error "!!!unknown __SIZEOF_POINTER__!!!"
#		endif
#	else
#		error "!!!undefined __SIZEOF_POINTER__!!!"
#	endif
#endif

#define DEMOZ_MARCH_X86_32 1
#define DEMOZ_MARCH_X86_64 2
#define DEMOZ_MARCH_ARM_32 3
#define DEMOZ_MARCH_ARM_64 4
#define DEMOZ_MARCH_RISCV_32 5
#define DEMOZ_MARCH_RISCV_64 6
#define DEMOZ_MARCH_MIPS_32 7
#define DEMOZ_MARCH_MIPS_64 8

#ifndef DEMOZ_MARCH_TYPE
#	if defined(__i386__)
#		define DEMOZ_MARCH_TYPE DEMOZ_MARCH_X86_32
#	elif defined(__x86_64__)
#		define DEMOZ_MARCH_TYPE DEMOZ_MARCH_X86_64
#	elif (defined(__arm__) || defined(__thumb__) || defined(__ARM_EABI__))
#		define DEMOZ_MARCH_TYPE DEMOZ_MARCH_ARM_32
#	elif defined(__aarch64__)
#		define DEMOZ_MARCH_TYPE DEMOZ_MARCH_ARM_64
#	elif (defined(__riscv) && defined(__riscv_xlen))
#		if (__riscv_xlen == 32)
#			define DEMOZ_MARCH_TYPE DEMOZ_MARCH_RISCV_32
#		else
#			define DEMOZ_MARCH_TYPE DEMOZ_MARCH_RISCV_64
#		endif
#	else
#		error "!!!unknown DEMOZ_MARCH_TYPE!!!"
#	endif
#endif

/* default linux platform */

#define DEMOZ_PLATFORM_NOSYS 1
#define DEMOZ_PLATFORM_LINUX 2

/* enable thread */
#define DEMOZ_ENABLE_THREAD 1

#ifndef DEMOZ_THREAD
#	define DEMOZ_THREAD DEMOZ_ENABLE_THREAD
#endif

#ifndef DEMOZ_PLATFORM
#	define DEMOZ_PLATFORM DEMOZ_PLATFORM_LINUX
#endif
/* end */


#endif
