/* @file: t_deflate_table.c
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

#define DEFLATE_MATCH_MIN 3
#define DEFLATE_MATCH_MAX 258

#define DEFLATE_LEN_CODES 29
#define DEFLATE_DIST_CODES 30

#define DEFLATE_LEN_SIZE (DEFLATE_MATCH_MAX - DEFLATE_MATCH_MIN + 1)
#define DEFLATE_DIST_SIZE ((DEFLATE_MATCH_MAX - DEFLATE_MATCH_MIN + 1) * 2)

#define DEFLATE_BITS_MAX 15
#define DEFLATE_LITERALS 256

/* NOTE: codes 256 End + 1 */

#define DEFLATE_L_CODES (DEFLATE_LITERALS + 1 + DEFLATE_LEN_CODES)
#define DEFLATE_D_CODES DEFLATE_DIST_CODES

#define DEFLATE_STATIC_L_SIZE (DEFLATE_L_CODES + 2)
#define DEFLATE_STATIC_D_SIZE DEFLATE_D_CODES

/* NOTE: codes 286 and 287 reserved placeholders */

struct deflate_ctdata {
	union {
		uint16_t freq; /* character freq */
		uint16_t code; /* huffman code */
	} fc;
	union {
		uint16_t dad; /* huffman parent node */
		uint16_t len; /* code length */
	} dl;
};

/* extra length bits */
static const uint8_t extra_len_bits[DEFLATE_LEN_CODES] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 2, 2, 2, 2,
	3, 3, 3, 3, 4, 4, 4, 4,
	5, 5, 5, 5, 0
	};

/* extra distance bits */
static const uint8_t extra_dist_bits[DEFLATE_DIST_CODES] = {
	 0,  0,  0,  0,  1,  1,  2,  2,
	 3,  3,  4,  4,  5,  5,  6,  6,
	 7,  7,  8,  8,  9,  9, 10, 10,
	11, 11, 12, 12, 13, 13
	};

uint8_t base_len[DEFLATE_LEN_CODES];     /* base length */
uint16_t base_dist[DEFLATE_DIST_CODES];  /* base distance */

uint8_t len_code[DEFLATE_LEN_SIZE];   /* length code table */
uint8_t dist_code[DEFLATE_DIST_SIZE]; /* distance code table */

struct deflate_ctdata static_ltree[DEFLATE_STATIC_L_SIZE];
struct deflate_ctdata static_dtree[DEFLATE_STATIC_D_SIZE];

static uint16_t _bit_reverse(uint16_t c, uint16_t len)
{
	/* 10001010 -> 01010001 */
	uint16_t r = 0;
	while (len--) {
		r |= c & 1;
		r <<= 1;
		c >>= 1;
	}

	return r >> 1;
}

/* canonical huffman codes generation */
static void _gen_codes(struct deflate_ctdata *tree, const uint16_t *bl_count,
		int32_t n)
{
	uint16_t next_code[DEFLATE_BITS_MAX + 1];
	uint32_t code = 0;

	for (int32_t i = 1; i <= DEFLATE_BITS_MAX; i++) {
		code = (code + bl_count[i - 1]) << 1;
		next_code[i] = code;
	}

	for (int32_t i = 0; i < n; i++) {
		uint16_t len = tree[i].dl.len;
		if (!len)
			continue;
		tree[i].fc.code = _bit_reverse(next_code[len]++, len);
	}
}

void deflate_len_code_init(void)
{
	uint32_t code, len;

	/* length code table */
	len = 0;
	for (code = 0; code < (DEFLATE_LEN_CODES - 1); code++) {
		base_len[code] = len;
		for (int32_t j = 0; j < (1 << extra_len_bits[code]); j++)
			len_code[len++] = code;
	}
	base_len[code] = 0;
	len_code[len - 1] = code;
}

void deflate_dist_code_init(void)
{
	uint32_t code, len;

	/* distance code table */
	len = 0;
	for (code = 0; code < 16; code++) {
		base_dist[code] = len;
		for (int32_t j = 0; j < (1 << extra_dist_bits[code]); j++)
			dist_code[len++] = code;
	}

	len >>= 7;
	for (; code < DEFLATE_DIST_CODES; code++) {
		base_dist[code] = len << 7;
		for (int32_t j = 0; j < (1 << (extra_dist_bits[code] - 7)); j++)
			dist_code[256 + len++] = code;
	}
}

void deflate_base_len_print(char *s1, char *s2)
{
	printf("/* %s */\n", s1);
	printf("static const uint8_t %sbase_len = {\n\t", s2);
	for (int32_t i = 0; i < DEFLATE_LEN_CODES; i++) {
		printf("%3u", base_len[i]);
		if ((i + 1) != DEFLATE_LEN_CODES) {
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

void deflate_base_dist_print(char *s1, char *s2)
{
	printf("/* %s */\n", s1);
	printf("static const uint16_t %sbase_dist = {\n\t", s2);
	for (int32_t i = 0; i < DEFLATE_DIST_CODES; i++) {
		printf("%5u", base_dist[i]);
		if ((i + 1) != DEFLATE_DIST_CODES) {
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

void deflate_len_code_print(char *s1, char *s2)
{
	printf("/* %s */\n", s1);
	printf("static const uint8_t %slen_code = {\n\t", s2);
	for (int32_t i = 0; i < DEFLATE_LEN_SIZE; i++) {
		printf("%2u", len_code[i]);
		if ((i + 1) != DEFLATE_LEN_SIZE) {
			printf(",");
			if ((i + 1) % 16)
				printf(" ");
			if (!((i + 1) % 16))
				printf("\n\t");
		} else {
			printf("\n\t");
		}
	}
	printf("};\n\n");
}

void deflate_dist_code_print(char *s1, char *s2)
{
	printf("/* %s */\n", s1);
	printf("static const uint8_t %sdist_code = {\n\t", s2);
	for (int32_t i = 0; i < DEFLATE_DIST_SIZE; i++) {
		printf("%2u", dist_code[i]);
		if ((i + 1) != DEFLATE_DIST_SIZE) {
			printf(",");
			if ((i + 1) % 16)
				printf(" ");
			if (!((i + 1) % 16))
				printf("\n\t");
		} else {
			printf("\n\t");
		}
	}
	printf("};\n\n");
}

void deflate_ltree_init(void)
{
	uint16_t bl_count[DEFLATE_BITS_MAX + 1];
	uint32_t code;

	for (code = 0; code <= DEFLATE_BITS_MAX; code++)
		bl_count[code] = 0;

	for (code = 0; code < 144; code++) { /* 0~143: 8-bit length */
		static_ltree[code].dl.len = 8;
		bl_count[8]++;
	}
	for (; code < 256; code++) { /* 144~255: 9-bit length */
		static_ltree[code].dl.len = 9;
		bl_count[9]++;
	}
	for (; code < 280; code++) { /* 256~279: 7-bit length */
		static_ltree[code].dl.len = 7;
		bl_count[7]++;
	}
	for (; code < DEFLATE_STATIC_L_SIZE; code++) { /* 280~287: 8-bit length */
		static_ltree[code].dl.len = 8;
		bl_count[8]++;
	}

	_gen_codes(static_ltree, bl_count, code);
}

void deflate_dtree_init(void)
{
	uint32_t code;

	for (code = 0; code < DEFLATE_STATIC_D_SIZE; code++) {
		static_dtree[code].dl.len = 5;
		static_dtree[code].fc.code = _bit_reverse(code, 5);
	}
}

void deflate_ltree_print(char *s1, char *s2)
{
	printf("/* %s */\n", s1);
	printf("static const struct deflate_ctdata %stree = {\n\t", s2);
	for (int32_t i = 0; i < DEFLATE_STATIC_L_SIZE; i++) {
		printf("{ { %3u }, { %u } }",
			static_ltree[i].fc.code,
			static_ltree[i].dl.len);
		if ((i + 1) != DEFLATE_STATIC_L_SIZE) {
			printf(",");
			if ((i + 1) % 3)
				printf(" ");
			if (!((i + 1) % 3))
				printf("\n\t");
		} else {
			printf("\n\t");
		}
	}
	printf("};\n\n");
}

void deflate_dtree_print(char *s1, char *s2)
{
	printf("/* %s */\n", s1);
	printf("static const struct deflate_ctdata %stree = {\n\t", s2);
	for (int32_t i = 0; i < DEFLATE_STATIC_D_SIZE; i++) {
		printf("{ { %2u }, { %u } }",
			static_dtree[i].fc.code,
			static_dtree[i].dl.len);
		if ((i + 1) != DEFLATE_STATIC_D_SIZE) {
			printf(",");
			if ((i + 1) % 3)
				printf(" ");
			if (!((i + 1) % 3))
				printf("\n\t");
		} else {
			printf("\n\t");
		}
	}
	printf("};\n\n");
}

int main(void)
{
	deflate_len_code_init();
	deflate_dist_code_init();

	deflate_base_dist_print("base distance", "");
	deflate_base_len_print("base length", "");

	deflate_len_code_print("length code table", "");
	deflate_dist_code_print("distance code table", "");

	deflate_ltree_init();
	deflate_ltree_print("static literal/length tree", "static_l");

	deflate_dtree_init();
	deflate_dtree_print("static distance tree", "static_d");

	return 0;
}
