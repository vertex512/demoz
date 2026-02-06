/* @file: test_bits_add.c
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
#include <demoz/ds/bits_add.h>


void test_bits_add(void)
{
	uint8_t *s = (uint8_t *)"Hell";
	BITS_ADD_NEW(ctx);

	F_SYMBOL(bits_add)(&ctx, s[0], 8);
	F_SYMBOL(bits_add)(&ctx, s[1], 3);
	F_SYMBOL(bits_add)(&ctx, s[1] >> 3, 4);
	F_SYMBOL(bits_add)(&ctx, s[1] >> 7, 1);
	F_SYMBOL(bits_add)(&ctx, s[2], 8);

	F_SYMBOL(bits_add)(&ctx, 12, 2); /* 0b1100 */

	printf("Hel: %c'%c'%c'(%u)\n",
		BITS_ADD_BUF(&ctx)[0],
		BITS_ADD_BUF(&ctx)[1],
		BITS_ADD_BUF(&ctx)[2],
		BITS_ADD_BUF(&ctx)[3]);
}

int main(void)
{
	test_bits_add();

	return 0;
}
