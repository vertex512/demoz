/* @file: _start.S
 * #desc:
 *    The implementations of userspace program entry.
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


	.text
	.globl _start
	.globl C_SYMBOL(___start_main)

/* @func: _start
 * #desc:
 *    userspace program entry.
 */
_start:
#if (DEMOZ_MARCH_TYPE == DEMOZ_MARCH_X86_32)

	xorl %ebp, %ebp
	movl %esp, %edi
	jump C_SYMBOL(___start_main)
	ret

#elif (DEMOZ_MARCH_TYPE == DEMOZ_MARCH_X86_64)

	xorq %rbp, %rbp
	movq %rsp, %rdi
	call C_SYMBOL(___start_main)
	ret

#elif (DEMOZ_MARCH_TYPE == DEMOZ_MARCH_ARM_32)

	mov r0, sp
	mov fp, #0
	mov lr, #0
	b C_SYMBOL(___start_main)
	bx lr

#elif (DEMOZ_MARCH_TYPE == DEMOZ_MARCH_ARM_64)

	mov x0, sp
	mov x29, #0
	mov x30, #0
	b C_SYMBOL(___start_main)
	ret

#elif (DEMOZ_MARCH_TYPE == DEMOZ_MARCH_RISCV_32 \
		|| DEMOZ_MARCH_TYPE == DEMOZ_MARCH_RISCV_64)

	mv a0, sp
	tail C_SYMBOL(___start_main)

#else
#	error "!!!unknown DEMOZ_MARCH_TYPE!!!"
#endif
