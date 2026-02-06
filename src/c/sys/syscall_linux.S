/* @file: syscall_linux.S
 * #desc:
 *    The implementations of linux system call.
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
	.globl C_SYMBOL(syscall_linux)

/* @func: syscall_linux
 * #desc:
 *    linux system call.
 *
 * #1: (long) syscall number
 * #2: (long) syscall arg1
 * #3: (long) syscall arg2
 * #4: (long) syscall arg3
 * #5: (long) syscall arg4
 * #6: (long) syscall arg5
 * #7: (long) syscall arg6
 * #r: (long) syscall return
 */
C_SYMBOL(syscall_linux):
#if (DEMOZ_MARCH_TYPE == DEMOZ_MARCH_X86_32)

	pushl %ebp
	pushl %edi
	pushl %esi
	pushl %ebx
	movl 20(%esp), %eax /* syscall number */
	movl 24(%esp), %ebx
	movl 28(%esp), %ecx
	movl 32(%esp), %edx
	movl 36(%esp), %esi
	movl 40(%esp), %edi
	movl 44(%esp), %ebp
	int $0x80
	popl %ebx
	popl %esi
	popl %edi
	popl %ebp
	ret

#elif (DEMOZ_MARCH_TYPE == DEMOZ_MARCH_X86_64)

	movq %rdi, %rax /* syscall number */
	movq %rsi, %rdi
	movq %rdx, %rsi
	movq %rcx, %rdx
	movq %r8, %r10
	movq %r9, %r8
	movq 8(%rsp), %r9
	syscall
	ret

#elif (DEMOZ_MARCH_TYPE == DEMOZ_MARCH_ARM_32)

	mov ip, sp
	push { r4, r5, r6, r7 } /* callee */
	mov r7, r0 /* syscall number */
	mov r0, r1
	mov r1, r2
	mov r2, r3
	ldmfd ip, { r3, r4, r5, r6 /* arg7 */ }
	swi #0
	pop { r4, r5, r6, r7 }
	bx lr

#elif (DEMOZ_MARCH_TYPE == DEMOZ_MARCH_ARM_64)

	mov x8, x0 /* syscall number */
	mov x0, x1
	mov x1, x2
	mov x2, x3
	mov x3, x4
	mov x4, x5
	mov x5, x6
	mov x6, x7 /* arg7 */
	svc #0
	ret

#elif (DEMOZ_MARCH_TYPE == DEMOZ_MARCH_RISCV_32 \
		|| DEMOZ_MARCH_TYPE == DEMOZ_MARCH_RISCV_64)

	mv a7, a0 /* syscall number */
	mv a0, a1
	mv a1, a2
	mv a2, a3
	mv a3, a4
	mv a4, a5
	mv a5, a6
	ecall
	ret

#else
#	error "!!!unknown DEMOZ_MARCH_TYPE!!!"
#endif
