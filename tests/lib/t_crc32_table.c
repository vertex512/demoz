/* @file: t_crc32_table.c
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


uint32_t crc32_table[256];

void crc32_table_msb(uint32_t n)
{
	for (int32_t i = 0; i < 256; i++) {
		uint32_t c = i;
		c <<= 24;
		for (int32_t j = 0; j < 8; j++) {
			if (c & 0x80000000) {
				c = (c << 1) ^ n;
			} else {
				c <<= 1;
			}
		}
		crc32_table[i] = c;
	}
}

void crc32_table_lsb(uint32_t n)
{
	for (int32_t i = 0; i < 256; i++) {
		uint32_t c = i;
		for (int32_t j = 0; j < 8; j++) {
			if (c & 0x01) {
				c = (c >> 1) ^ n;
			} else {
				c >>= 1;
			}
		}
		crc32_table[i] = c;
	}
}

void crc32_table_print(char *s1, char *s2)
{
	printf("/* %s */\n", s1);
	printf("static const uint32_t crc32_table%s = {\n\t", s2);
	for (int32_t i = 0; i < 256; i++) {
		printf("0x%08x", crc32_table[i]);
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

	crc32_table_msb(0x04c11db7);
	crc32_table_print("crc32/iso not reversed: 0x04c11db7 (msb)", "_m");
	crc32_table_lsb(0xedb88320);
	crc32_table_print("crc32/iso after reversed: 0xedb88320 (lsb)", "_l");

	crc32_table_msb(0x1edc6f41);
	crc32_table_print("crc32/castagnoli not reversed: 0x1edc6f41 (msb)", "_mc");
	crc32_table_lsb(0x82f63b78);
	crc32_table_print("crc32/castagnoli after reversed: 0x82f63b78 (lsb)", "_lc");

	crc32_table_msb(0x741b8cd7);
	crc32_table_print("crc32/koopman not reversed: 0x741b8cd7 (msb)", "_mk");
	crc32_table_lsb(0xeb31d82e);
	crc32_table_print("crc32/koopman after reversed: 0xeb31d82e (lsb)", "_lk");

	crc32_table_msb(0x814141ab);
	crc32_table_print("crc32/q not reversed: 0x814141ab (msb)", "_mq");
	crc32_table_lsb(0xd5828281);
	crc32_table_print("crc32/q after reversed: 0xd5828281 (lsb)", "_lq");

	return 0;
}
