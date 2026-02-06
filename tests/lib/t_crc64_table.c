/* @file: t_crc64_table.c
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


uint64_t crc64_table[256];

void crc64_table_msb(uint64_t n)
{
	for (int32_t i = 0; i < 256; i++) {
		uint64_t c = i;
		c <<= 56;
		for (int32_t j = 0; j < 8; j++) {
			if (c & 0x8000000000000000ULL) {
				c = (c << 1) ^ n;
			} else {
				c <<= 1;
			}
		}
		crc64_table[i] = c;
	}
}

void crc64_table_lsb(uint64_t n)
{
	for (int32_t i = 0; i < 256; i++) {
		uint64_t c = i;
		for (int32_t j = 0; j < 8; j++) {
			if (c & 0x01) {
				c = (c >> 1) ^ n;
			} else {
				c >>= 1;
			}
		}
		crc64_table[i] = c;
	}
}

void crc64_table_print(char *s1, char *s2)
{
	printf("/* %s */\n", s1);
	printf("static const uint64_t crc64_table%s = {\n\t", s2);
	for (int32_t i = 0; i < 256; i++) {
		printf("0x%016llxULL", crc64_table[i]);
		if ((i + 1) != 256) {
			printf(",");
			if ((i + 1) % 2)
				printf(" ");
			if (!((i + 1) % 2))
				printf("\n\t");
		} else {
			printf("\n\t");
		}
	}
	printf("};\n\n");
}

int main(void)
{
	/*
	 * msb: RefIn == false, RefOut == false, not reversed
	 * lsb: RefIn == true, RefOut == true, after reversed
	 *
	 * reversed: 00000011 after 11000000
	 */

	crc64_table_msb(0x000000000000001b);
	crc64_table_print("crc64/iso not reversed: 0x000000000000001b (msb)", "_m");
	crc64_table_lsb(0xd800000000000000);
	crc64_table_print("crc64/iso after reversed: 0xd800000000000000 (lsb)", "_l");

	crc64_table_msb(0x42f0e1eba9ea3693);
	crc64_table_print("crc64/ecma not reversed: 0x42f0e1eba9ea3693 (msb)", "_me");
	crc64_table_lsb(0xc96c5795d7870f42);
	crc64_table_print("crc64/ecma after reversed: 0xc96c5795d7870f42 (lsb)", "_le");

	return 0;
}
