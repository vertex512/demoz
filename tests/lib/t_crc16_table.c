/* @file: t_crc16_table.c
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


uint16_t crc16_table[256];

void crc16_table_msb(uint16_t n)
{
	for (int32_t i = 0; i < 256; i++) {
		uint16_t c = i;
		c <<= 8;
		for (int32_t j = 0; j < 8; j++) {
			if (c & 0x8000) {
				c = (c << 1) ^ n;
			} else {
				c <<= 1;
			}
		}
		crc16_table[i] = c;
	}
}

void crc16_table_lsb(uint16_t n)
{
	for (int32_t i = 0; i < 256; i++) {
		uint16_t c = i;
		for (int32_t j = 0; j < 8; j++) {
			if (c & 0x01) {
				c = (c >> 1) ^ n;
			} else {
				c >>= 1;
			}
		}
		crc16_table[i] = c;
	}
}

void crc16_table_print(char *s1, char *s2)
{
	printf("/* %s */\n", s1);
	printf("static const uint16_t crc16_table%s = {\n\t", s2);
	for (int32_t i = 0; i < 256; i++) {
		printf("0x%04x", crc16_table[i]);
		if ((i + 1) != 256) {
			printf(",");
			if ((i + 1) % 4)
				printf(" ");
			if (!((i + 1) % 4))
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

	crc16_table_msb(0x8005);
	crc16_table_print("crc16/ibm not reversed: 0x8005 (msb)", "_m");
	crc16_table_lsb(0xa001);
	crc16_table_print("crc16/ibm after reversed: 0xa001 (lsb)", "_l");

	return 0;
}
