/* @file: atomic_cas.S
 * #desc:
 *    The implementations of atomic operations.
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
	.globl C_SYMBOL(atomic_cas)

/* @func: atomic_cas
 * #desc:
 *    atomic cas (compare-and-swap).
 *
 * #1: (int*) variable pointer
 * #2: (int)  old value
 * #3: (int)  new value
 * #r: (int)  old: success, *var: fail
 */
C_SYMBOL(atomic_cas):
#if (DEMOZ_MARCH_TYPE == DEMOZ_MARCH_X86_32)

	pushl %ebx
	movl 8(%esp), %ebx		// $ebx = var;
	movl 12(%esp), %eax		// $eax = old;
	movl 16(%esp), %ecx		// $ecx = new;
	lock cmpxchg %ecx, (%ebx)	// cmpxchg(new, var);
	popl %ebx
	ret

#elif (DEMOZ_MARCH_TYPE == DEMOZ_MARCH_X86_64)

	movq %rsi, %rax			// $rax = old;
	lock cmpxchg %rdx, (%rdi)	// cmpxchg(new, var);
	ret

#elif (DEMOZ_MARCH_TYPE == DEMOZ_MARCH_ARM_32)

	dmb ish			// # memory access order
.L1:				// do {
	ldrex r3, [r0]		//   ldrex($r3, var);
	cmp r3, r1		//   if ($r3 != old)
	bne .L2			//     goto ret;
	strex r3, r2, [r0]	//   strex($r3, new, var);
	cmp r3, #0		// } while ($r3
	bne .L1			//   );
	mov r3, r1		// $r3 = old;
.L2:				// ret:
	mov r0, r3		// ret = $r3;
	dmb ish			// # memory access order
	bx lr

#elif (DEMOZ_MARCH_TYPE == DEMOZ_MARCH_ARM_64)

	dmb ish			// # memory access order
.L1:				// do {
	ldxr w3, [x0]		//   ldrex($w3, var);
	cmp w3, w1		//   if ($w3 != old)
	bne .L2			//     goto ret;
	stxr w3, w2, [x0]	//   stxr($w3, new, var);
	cbnz w3, .L1		// } while ($w3);
	mov w3, w1		// $w3 = old;
.L2:				// ret:
	mov w0, w3		// ret = $w3;
	dmb ish			// # memory access order
	ret

#elif (DEMOZ_MARCH_TYPE == DEMOZ_MARCH_RISCV_32 \
		|| DEMOZ_MARCH_TYPE == DEMOZ_MARCH_RISCV_64)

	fence rw, rw		// # memory access order
.L1:				// do {
	lr.w.aqrl a3, (a0)	//   lr($a3, var);
	bne a3, a1, .L2		//   if ($a3 != old) goto ret;
	sc.w.aqrl a3, a2, (a0)	//   sc($a3, new, var);
	bnez a3, .L1		// } while ($a3);
	mv a3, a1		// $a3 = old;
.L2:				// ret:
	mv a0, a3		// ret = $a3;
	fence rw, rw		// # memory access order
	ret

#else
#	error "!!!unknown DEMOZ_MARCH_TYPE!!!"
#endif
