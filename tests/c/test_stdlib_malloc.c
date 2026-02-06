/* @file: test_stdlib_malloc.c
 * #desc:
 *
 * #copy:
 *    Copyright (C) 1970 Public Free Software
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program. If not,
 *    see <https://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <demoz/c/stdint.h>
#include <demoz/c/sys/stdlib.h>


int main(void)
{
	void *p = C_SYMBOL(malloc)(0);
	printf("%p: %zu\n", p, (uintptr_t)p & 15);

	p = C_SYMBOL(malloc)(0);
	printf("%p: %zu\n", p, (uintptr_t)p & 15);
	C_SYMBOL(free)(p);

	p = C_SYMBOL(malloc)(17);
	printf("%p: %zu\n", p, (uintptr_t)p & 15);

	p = C_SYMBOL(malloc)(1);
	printf("%p: %zu\n", p, (uintptr_t)p & 15);

	p = C_SYMBOL(malloc)(1*1024*1024*1024);
	printf("%p: %zu\n", p, (uintptr_t)p & 15);

	return 0;
}
