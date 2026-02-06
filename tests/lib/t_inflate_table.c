/* @file: t_inflate_table.c
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


/*
 * Length:
 *   |Code|Extra bits|Length
 *   |257 |0         |3
 *   |258 |0         |4
 *   |259 |0         |5
 *   |260 |0         |6
 *   |261 |0         |7
 *   |262 |0         |8
 *   |263 |0         |9
 *   |264 |0         |10
 *   |265 |1         |11-12
 *   |266 |1         |13-14
 *   |267 |1         |15-16
 *   |268 |1         |17-18
 *   |269 |2         |19-22
 *   |270 |2         |23-26
 *   |271 |2         |27-30
 *   |272 |2         |31-34
 *   |273 |3         |35-42
 *   |274 |3         |43-50
 *   |275 |3         |51-58
 *   |276 |3         |59-66
 *   |277 |4         |67-82
 *   |278 |4         |83-98
 *   |279 |4         |99-114
 *   |280 |4         |115-130
 *   |281 |5         |131-162
 *   |282 |5         |163-194
 *   |283 |5         |195-226
 *   |284 |5         |227-257
 *   |285 |0         |258
 *
 * Distance:
 *   |Code|Extra bits|Distance
 *   |0   |0         |1
 *   |1   |0         |2
 *   |2   |0         |3
 *   |3   |0         |4
 *   |4   |1         |5-6
 *   |5   |1         |7-8
 *   |6   |2         |9-12
 *   |7   |2         |13-16
 *   |8   |3         |17-24
 *   |9   |3         |25-32
 *   |10  |4         |33-48
 *   |11  |4         |49-64
 *   |12  |5         |65-96
 *   |13  |5         |97-128
 *   |14  |6         |129-192
 *   |15  |6         |193-256
 *   |16  |7         |257-384
 *   |17  |7         |385-512
 *   |18  |8         |513-768
 *   |19  |8         |769-1024
 *   |20  |9         |1025-1536
 *   |21  |9         |1537-2048
 *   |22  |10        |2049-3072
 *   |23  |10        |3073-4096
 *   |24  |11        |4097-6144
 *   |25  |11        |6145-8192
 *   |26  |12        |8193-12288
 *   |27  |12        |12289-16384
 *   |28  |13        |16385-24576
 *   |29  |13        |24577-32768
 */

#define INFLATE_LEN_CODES 29
#define INFLATE_DIST_CODES 30

/* extra length bits */
static const uint8_t extra_len_bits[INFLATE_LEN_CODES] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 2, 2, 2, 2,
	3, 3, 3, 3, 4, 4, 4, 4,
	5, 5, 5, 5, 0
	};

/* extra distance bits */
static const uint8_t extra_dist_bits[INFLATE_DIST_CODES] = {
	 0,  0,  0,  0,  1,  1,  2,  2,
	 3,  3,  4,  4,  5,  5,  6,  6,
	 7,  7,  8,  8,  9,  9, 10, 10,
	11, 11, 12, 12, 13, 13
	};

uint16_t base_len[INFLATE_LEN_CODES];    /* base length */
uint16_t base_dist[INFLATE_DIST_CODES];  /* base distance */

void inflate_len_code_init(void)
{
	uint32_t code, len;

	/* length code table */
	len = 3;
	for (code = 0; code < (INFLATE_LEN_CODES - 1); code++) {
		base_len[code] = len;
		len += 1 << extra_len_bits[code];
	}
	base_len[code] = 258;
}

void inflate_dist_code_init(void)
{
	uint32_t code, len;

	/* distance code table */
	len = 1;
	for (code = 0; code < INFLATE_DIST_CODES; code++) {
		base_dist[code] = len;
		len += 1 << extra_dist_bits[code];
	}
}

void inflate_base_len_print(char *s1, char *s2)
{
	printf("/* %s */\n", s1);
	printf("static const uint16_t %sbase_len = {\n\t", s2);
	for (int32_t i = 0; i < INFLATE_LEN_CODES; i++) {
		printf("%3u", base_len[i]);
		if ((i + 1) != INFLATE_LEN_CODES) {
			printf(",");
			if ((i + 1) % 8)
				printf(" ");
			if (!((i + 1) % 8))
				printf("\n\t");
		} else {
			printf("\n\t");
		}
	}
	printf("};\n\n");
}

void inflate_base_dist_print(char *s1, char *s2)
{
	printf("/* %s */\n", s1);
	printf("static const uint16_t %sbase_dist = {\n\t", s2);
	for (int32_t i = 0; i < INFLATE_DIST_CODES; i++) {
		printf("%5u", base_dist[i]);
		if ((i + 1) != INFLATE_DIST_CODES) {
			printf(",");
			if ((i + 1) % 8)
				printf(" ");
			if (!((i + 1) % 8))
				printf("\n\t");
		} else {
			printf("\n\t");
		}
	}
	printf("};\n\n");
}

int main(void)
{
	inflate_len_code_init();
	inflate_dist_code_init();

	inflate_base_len_print("base length", "");
	inflate_base_dist_print("base distance", "");

	return 0;
}
