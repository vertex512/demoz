/* @file: test_bits_get.c
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
#include <demoz/ds/bits_get.h>


void test_bits_get(void)
{
	uint8_t *s = (uint8_t *)"Hell";
	BITS_GET_NEW(ctx);
	F_SYMBOL(bits_get_fill)(&ctx, s, 4);

	uint32_t v = 0, v1 = 0, v2;

	F_SYMBOL(bits_get)(&ctx, &v, 8, 0);
	printf("H: %c\n", v);

	F_SYMBOL(bits_get)(&ctx, &v, 4, 0);
	F_SYMBOL(bits_get)(&ctx, &v1, 3, 0);
	F_SYMBOL(bits_get)(&ctx, &v2, 9, 0);

	v |= (v1 << 4) | (v2 << 7);

	printf("el: %c%c\n", v & 0xff, v >> 8);

	F_SYMBOL(bits_get)(&ctx, &v1, 8, 1);
	F_SYMBOL(bits_get)(&ctx, &v2, 8, 0);

	v = 0xff;
	F_SYMBOL(bits_get)(&ctx, &v, 8, 0);

	printf("ll(ff): %c%c(%x)\n", v1, v2, v);
}

int main(void)
{
	test_bits_get();

	return 0;
}
