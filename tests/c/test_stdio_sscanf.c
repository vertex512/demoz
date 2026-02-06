/* @file: test_stdio_sscanf.c
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
#include <demoz/c/stdio.h>


int main(void)
{
	char buf[16];
	uint64_t u;

	int32_t r = C_SYMBOL(sscanf)(
		"-0x123 [Hello], world",
		"%li [%*[^]]%s %[A-z]",
		&u,
		&buf,
		&buf);

	printf("%d: %ld %s\n", r, u, buf);

	return 0;
}
