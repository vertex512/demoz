/* @file: deflate.c
 * #desc:
 *    The implementations of deflate compression algorithm.
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
#include <demoz/c/stdint.h>
#include <demoz/c/string.h>
#include <demoz/ds/bits_add.h>
#include <demoz/lib/deflate.h>


/* @def: _
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
 *
 * Bit-Length:
 *   |Code|Extra bits|Length
 *   |0   |0         |0
 *   |1   |0         |1
 *   |2   |0         |2
 *   |3   |0         |3
 *   |4   |0         |4
 *   |5   |0         |5
 *   |6   |0         |6
 *   |7   |0         |7
 *   |8   |0         |8
 *   |9   |0         |9
 *   |10  |0         |10
 *   |11  |0         |11
 *   |12  |0         |12
 *   |13  |0         |13
 *   |14  |0         |14
 *   |15  |0         |15
 *   |16  |2         |3-6
 *   |17  |3         |3-10
 *   |18  |7         |11-138
 */

/* bitlen extra bits */
/* static const uint8_t extra_bl_bits[DEFLATE_BL_CODES] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	2, 3, 7
	}; */

/* bitlen output order */
static const uint8_t bl_order[DEFLATE_BL_CODES] = {
	16, 17, 18,  0,  8,  7,  9,  6,
	10,  5, 11,  4, 12,  3, 13,  2,
	14,  1, 15
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

/* base length */
static const uint8_t base_len[DEFLATE_LEN_CODES] = {
	  0,   1,   2,   3,   4,   5,   6,   7,
	  8,  10,  12,  14,  16,  20,  24,  28,
	 32,  40,  48,  56,  64,  80,  96, 112,
	128, 160, 192, 224,   0
	};

/* base distance */
static const uint16_t base_dist[DEFLATE_DIST_CODES] = {
	   0,     1,     2,     3,     4,     6,     8,    12,
	  16,    24,    32,    48,    64,    96,   128,   192,
	 256,   384,   512,   768,  1024,  1536,  2048,  3072,
	4096,  6144,  8192, 12288, 16384, 24576
	};

/* length code table */
static const uint8_t len_code[DEFLATE_LEN_SIZE] = {
	 0,  1,  2,  3,  4,  5,  6,  7,  8,  8,  9,  9, 10, 10, 11, 11,
	12, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, 15,
	16, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 17, 17,
	18, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 19, 19,
	20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
	21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
	22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
	26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
	27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
	27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 28
	};

/* distance code table */
static const uint8_t dist_code[DEFLATE_DIST_SIZE] = {
	 0,  1,  2,  3,  4,  4,  5,  5,  6,  6,  6,  6,  7,  7,  7,  7,
	 8,  8,  8,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9,  9,
	10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
	11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
	12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
	12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
	13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
	13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
	14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
	14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
	14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
	14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
	15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
	 0,  0, 16, 17, 18, 18, 19, 19, 20, 20, 20, 20, 21, 21, 21, 21,
	22, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 23, 23,
	24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
	26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
	27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
	27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
	28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
	28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
	28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
	28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
	29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
	29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
	29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
	29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29
	};

/* static length tree */
static const struct deflate_ctdata static_ltree[DEFLATE_L_CODES + 2] = {
	{ {  12 }, { 8 } }, { { 140 }, { 8 } }, { {  76 }, { 8 } },
	{ { 204 }, { 8 } }, { {  44 }, { 8 } }, { { 172 }, { 8 } },
	{ { 108 }, { 8 } }, { { 236 }, { 8 } }, { {  28 }, { 8 } },
	{ { 156 }, { 8 } }, { {  92 }, { 8 } }, { { 220 }, { 8 } },
	{ {  60 }, { 8 } }, { { 188 }, { 8 } }, { { 124 }, { 8 } },
	{ { 252 }, { 8 } }, { {   2 }, { 8 } }, { { 130 }, { 8 } },
	{ {  66 }, { 8 } }, { { 194 }, { 8 } }, { {  34 }, { 8 } },
	{ { 162 }, { 8 } }, { {  98 }, { 8 } }, { { 226 }, { 8 } },
	{ {  18 }, { 8 } }, { { 146 }, { 8 } }, { {  82 }, { 8 } },
	{ { 210 }, { 8 } }, { {  50 }, { 8 } }, { { 178 }, { 8 } },
	{ { 114 }, { 8 } }, { { 242 }, { 8 } }, { {  10 }, { 8 } },
	{ { 138 }, { 8 } }, { {  74 }, { 8 } }, { { 202 }, { 8 } },
	{ {  42 }, { 8 } }, { { 170 }, { 8 } }, { { 106 }, { 8 } },
	{ { 234 }, { 8 } }, { {  26 }, { 8 } }, { { 154 }, { 8 } },
	{ {  90 }, { 8 } }, { { 218 }, { 8 } }, { {  58 }, { 8 } },
	{ { 186 }, { 8 } }, { { 122 }, { 8 } }, { { 250 }, { 8 } },
	{ {   6 }, { 8 } }, { { 134 }, { 8 } }, { {  70 }, { 8 } },
	{ { 198 }, { 8 } }, { {  38 }, { 8 } }, { { 166 }, { 8 } },
	{ { 102 }, { 8 } }, { { 230 }, { 8 } }, { {  22 }, { 8 } },
	{ { 150 }, { 8 } }, { {  86 }, { 8 } }, { { 214 }, { 8 } },
	{ {  54 }, { 8 } }, { { 182 }, { 8 } }, { { 118 }, { 8 } },
	{ { 246 }, { 8 } }, { {  14 }, { 8 } }, { { 142 }, { 8 } },
	{ {  78 }, { 8 } }, { { 206 }, { 8 } }, { {  46 }, { 8 } },
	{ { 174 }, { 8 } }, { { 110 }, { 8 } }, { { 238 }, { 8 } },
	{ {  30 }, { 8 } }, { { 158 }, { 8 } }, { {  94 }, { 8 } },
	{ { 222 }, { 8 } }, { {  62 }, { 8 } }, { { 190 }, { 8 } },
	{ { 126 }, { 8 } }, { { 254 }, { 8 } }, { {   1 }, { 8 } },
	{ { 129 }, { 8 } }, { {  65 }, { 8 } }, { { 193 }, { 8 } },
	{ {  33 }, { 8 } }, { { 161 }, { 8 } }, { {  97 }, { 8 } },
	{ { 225 }, { 8 } }, { {  17 }, { 8 } }, { { 145 }, { 8 } },
	{ {  81 }, { 8 } }, { { 209 }, { 8 } }, { {  49 }, { 8 } },
	{ { 177 }, { 8 } }, { { 113 }, { 8 } }, { { 241 }, { 8 } },
	{ {   9 }, { 8 } }, { { 137 }, { 8 } }, { {  73 }, { 8 } },
	{ { 201 }, { 8 } }, { {  41 }, { 8 } }, { { 169 }, { 8 } },
	{ { 105 }, { 8 } }, { { 233 }, { 8 } }, { {  25 }, { 8 } },
	{ { 153 }, { 8 } }, { {  89 }, { 8 } }, { { 217 }, { 8 } },
	{ {  57 }, { 8 } }, { { 185 }, { 8 } }, { { 121 }, { 8 } },
	{ { 249 }, { 8 } }, { {   5 }, { 8 } }, { { 133 }, { 8 } },
	{ {  69 }, { 8 } }, { { 197 }, { 8 } }, { {  37 }, { 8 } },
	{ { 165 }, { 8 } }, { { 101 }, { 8 } }, { { 229 }, { 8 } },
	{ {  21 }, { 8 } }, { { 149 }, { 8 } }, { {  85 }, { 8 } },
	{ { 213 }, { 8 } }, { {  53 }, { 8 } }, { { 181 }, { 8 } },
	{ { 117 }, { 8 } }, { { 245 }, { 8 } }, { {  13 }, { 8 } },
	{ { 141 }, { 8 } }, { {  77 }, { 8 } }, { { 205 }, { 8 } },
	{ {  45 }, { 8 } }, { { 173 }, { 8 } }, { { 109 }, { 8 } },
	{ { 237 }, { 8 } }, { {  29 }, { 8 } }, { { 157 }, { 8 } },
	{ {  93 }, { 8 } }, { { 221 }, { 8 } }, { {  61 }, { 8 } },
	{ { 189 }, { 8 } }, { { 125 }, { 8 } }, { { 253 }, { 8 } },
	{ {  19 }, { 9 } }, { { 275 }, { 9 } }, { { 147 }, { 9 } },
	{ { 403 }, { 9 } }, { {  83 }, { 9 } }, { { 339 }, { 9 } },
	{ { 211 }, { 9 } }, { { 467 }, { 9 } }, { {  51 }, { 9 } },
	{ { 307 }, { 9 } }, { { 179 }, { 9 } }, { { 435 }, { 9 } },
	{ { 115 }, { 9 } }, { { 371 }, { 9 } }, { { 243 }, { 9 } },
	{ { 499 }, { 9 } }, { {  11 }, { 9 } }, { { 267 }, { 9 } },
	{ { 139 }, { 9 } }, { { 395 }, { 9 } }, { {  75 }, { 9 } },
	{ { 331 }, { 9 } }, { { 203 }, { 9 } }, { { 459 }, { 9 } },
	{ {  43 }, { 9 } }, { { 299 }, { 9 } }, { { 171 }, { 9 } },
	{ { 427 }, { 9 } }, { { 107 }, { 9 } }, { { 363 }, { 9 } },
	{ { 235 }, { 9 } }, { { 491 }, { 9 } }, { {  27 }, { 9 } },
	{ { 283 }, { 9 } }, { { 155 }, { 9 } }, { { 411 }, { 9 } },
	{ {  91 }, { 9 } }, { { 347 }, { 9 } }, { { 219 }, { 9 } },
	{ { 475 }, { 9 } }, { {  59 }, { 9 } }, { { 315 }, { 9 } },
	{ { 187 }, { 9 } }, { { 443 }, { 9 } }, { { 123 }, { 9 } },
	{ { 379 }, { 9 } }, { { 251 }, { 9 } }, { { 507 }, { 9 } },
	{ {   7 }, { 9 } }, { { 263 }, { 9 } }, { { 135 }, { 9 } },
	{ { 391 }, { 9 } }, { {  71 }, { 9 } }, { { 327 }, { 9 } },
	{ { 199 }, { 9 } }, { { 455 }, { 9 } }, { {  39 }, { 9 } },
	{ { 295 }, { 9 } }, { { 167 }, { 9 } }, { { 423 }, { 9 } },
	{ { 103 }, { 9 } }, { { 359 }, { 9 } }, { { 231 }, { 9 } },
	{ { 487 }, { 9 } }, { {  23 }, { 9 } }, { { 279 }, { 9 } },
	{ { 151 }, { 9 } }, { { 407 }, { 9 } }, { {  87 }, { 9 } },
	{ { 343 }, { 9 } }, { { 215 }, { 9 } }, { { 471 }, { 9 } },
	{ {  55 }, { 9 } }, { { 311 }, { 9 } }, { { 183 }, { 9 } },
	{ { 439 }, { 9 } }, { { 119 }, { 9 } }, { { 375 }, { 9 } },
	{ { 247 }, { 9 } }, { { 503 }, { 9 } }, { {  15 }, { 9 } },
	{ { 271 }, { 9 } }, { { 143 }, { 9 } }, { { 399 }, { 9 } },
	{ {  79 }, { 9 } }, { { 335 }, { 9 } }, { { 207 }, { 9 } },
	{ { 463 }, { 9 } }, { {  47 }, { 9 } }, { { 303 }, { 9 } },
	{ { 175 }, { 9 } }, { { 431 }, { 9 } }, { { 111 }, { 9 } },
	{ { 367 }, { 9 } }, { { 239 }, { 9 } }, { { 495 }, { 9 } },
	{ {  31 }, { 9 } }, { { 287 }, { 9 } }, { { 159 }, { 9 } },
	{ { 415 }, { 9 } }, { {  95 }, { 9 } }, { { 351 }, { 9 } },
	{ { 223 }, { 9 } }, { { 479 }, { 9 } }, { {  63 }, { 9 } },
	{ { 319 }, { 9 } }, { { 191 }, { 9 } }, { { 447 }, { 9 } },
	{ { 127 }, { 9 } }, { { 383 }, { 9 } }, { { 255 }, { 9 } },
	{ { 511 }, { 9 } }, { {   0 }, { 7 } }, { {  64 }, { 7 } },
	{ {  32 }, { 7 } }, { {  96 }, { 7 } }, { {  16 }, { 7 } },
	{ {  80 }, { 7 } }, { {  48 }, { 7 } }, { { 112 }, { 7 } },
	{ {   8 }, { 7 } }, { {  72 }, { 7 } }, { {  40 }, { 7 } },
	{ { 104 }, { 7 } }, { {  24 }, { 7 } }, { {  88 }, { 7 } },
	{ {  56 }, { 7 } }, { { 120 }, { 7 } }, { {   4 }, { 7 } },
	{ {  68 }, { 7 } }, { {  36 }, { 7 } }, { { 100 }, { 7 } },
	{ {  20 }, { 7 } }, { {  84 }, { 7 } }, { {  52 }, { 7 } },
	{ { 116 }, { 7 } }, { {   3 }, { 8 } }, { { 131 }, { 8 } },
	{ {  67 }, { 8 } }, { { 195 }, { 8 } }, { {  35 }, { 8 } },
	{ { 163 }, { 8 } }, { {  99 }, { 8 } }, { { 227 }, { 8 } }
	};

/* static distance tree */
static const struct deflate_ctdata static_dtree[DEFLATE_D_CODES] = {
	{ {  0 }, { 5 } }, { { 16 }, { 5 } }, { {  8 }, { 5 } },
	{ { 24 }, { 5 } }, { {  4 }, { 5 } }, { { 20 }, { 5 } },
	{ { 12 }, { 5 } }, { { 28 }, { 5 } }, { {  2 }, { 5 } },
	{ { 18 }, { 5 } }, { { 10 }, { 5 } }, { { 26 }, { 5 } },
	{ {  6 }, { 5 } }, { { 22 }, { 5 } }, { { 14 }, { 5 } },
	{ { 30 }, { 5 } }, { {  1 }, { 5 } }, { { 17 }, { 5 } },
	{ {  9 }, { 5 } }, { { 25 }, { 5 } }, { {  5 }, { 5 } },
	{ { 21 }, { 5 } }, { { 13 }, { 5 } }, { { 29 }, { 5 } },
	{ {  3 }, { 5 } }, { { 19 }, { 5 } }, { { 11 }, { 5 } },
	{ { 27 }, { 5 } }, { {  7 }, { 5 } }, { { 23 }, { 5 } }
	};

const uint16_t config_table[10][4] = {
	/* lazy nice good chain */
	{    0,    0,    0,    0 }, /* store only */
	{    4,    8,    4,    4 }, /* max speed, no lazy matches */
	{    5,   16,    4,    8 },
	{    6,   32,    4,   32 },
	{    4,   16,    4,   16 }, /* lazy matches */
	{   16,   32,    8,   32 },
	{   16,  128,    8,  128 },
	{   32,  128,    8,  256 },
	{  128,  258,   32, 1024 },
	{  258,  258,   32, 4096 }
	};

#define SEND_SKIP(s) _send_bits_skip(s)
#define SEND_BITS(s, x, n) _send_bits(s, x, n)
#define SEND_CODE(s, x, n) _send_bits(s, (x)[n].fc.code, (x)[n].dl.len)
#define SEND_FINISH(s) _send_bits_finish(s)

#define LEN_CODE(x) (DEFLATE_LITERALS + 1 + len_code[x])
#define DIST_CODE(x) (((x) < 256) ? dist_code[x] \
	: dist_code[256 + ((x) >> 7)])
/* end */

/* @func: _send_bits (static)
 * #desc:
 *    send bits to buffer.
 *
 * #1: deflate struct context
 * #2: bits value
 * #3: bits length
 */
static void _send_bits(struct deflate_ctx *ctx, uint32_t v, uint32_t len)
{
	if (!F_SYMBOL(bits_add)(&ctx->bits_ctx, v, len))
		return;

	C_SYMBOL(memcpy)(ctx->buf + ctx->len,
		BITS_ADD_BUF(&ctx->bits_ctx), BITS_ADD_BUFSIZE);
	BITS_ADD_FLUSH(&ctx->bits_ctx);
	ctx->len += BITS_ADD_BUFSIZE;

	if (BITS_ADD_REM(&ctx->bits_ctx))
		F_SYMBOL(bits_add)(&ctx->bits_ctx, v, len);
}

/* @func: _send_bits_finish (static)
 * #desc:
 *    flush bits to buffer.
 *
 * #1: deflate struct context
 */
static void _send_bits_finish(struct deflate_ctx *ctx)
{
	uint32_t n = BITS_ADD_GETSIZE(&ctx->bits_ctx);
	if (!n)
		return;

	C_SYMBOL(memcpy)(ctx->buf + ctx->len,
		BITS_ADD_BUF(&ctx->bits_ctx), n);
	BITS_ADD_FLUSH(&ctx->bits_ctx);
	ctx->len += n;
}

/* @func: _send_bits_skip (static)
 * #desc:
 *    skip the remaining bits in the buffer byte.
 *
 * #1: deflate struct context
 */
static void _send_bits_skip(struct deflate_ctx *ctx)
{
	BITS_ADD_SKIP(&ctx->bits_ctx);
}

/* @func: _bit_reverse (static)
 * #desc:
 *    bit reverse conversion.
 *
 * #1: input
 * #2: length
 * #r: reverse
 */
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

/* @func: _gen_codes (static)
 * #desc:
 *    generate codes based on length.
 *
 * #1: tree ctdata
 * #2: bit-length count
 * #3: codes number
 */
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

/* @func: _heapdown (static)
 * #desc:
 *    minheap down shift.
 *
 * #1: deflate struct context
 * #2: tree ctdata
 * #3: parent node
 */
static void _heapdown(struct deflate_ctx *ctx,
		const struct deflate_ctdata *tree, int32_t k)
{
	int32_t v = ctx->heap[k];
	int32_t j = k * 2; /* left child of the k (start at index 1) */

	while (j <= ctx->heap_size) {
		int32_t l = ctx->heap[j];
		int32_t r = ctx->heap[j + 1]; /* right child */

		if (j < ctx->heap_size
				&& (tree[r].fc.freq < tree[l].fc.freq
				|| (tree[r].fc.freq == tree[l].fc.freq
				&& ctx->depth[r] <= ctx->depth[l]))) {
			j++;
			l = ctx->heap[j];
		}

		if (tree[v].fc.freq < tree[l].fc.freq
				|| (tree[v].fc.freq == tree[l].fc.freq
				&& ctx->depth[v] <= ctx->depth[l]))
			break;

		ctx->heap[k] = ctx->heap[j];
		k = j;
		j *= 2;
	}

	ctx->heap[k] = v;
}

/* @func: _gen_bitlen (static)
 * #desc:
 *    generate bit-length based on dynamic tree.
 *
 * #1: deflate struct context
 * #2: tree description
 */
static void _gen_bitlen(struct deflate_ctx *ctx, struct deflate_tree_desc *desc)
{
	struct deflate_ctdata *tree = desc->tree;
	int32_t bits_max = desc->bits_max;
	int32_t code_max = desc->code_max;
	int32_t n, m, len, overflow = 0;

	for (int32_t i = 0; i <= DEFLATE_BITS_MAX; i++)
		ctx->bl_count[i] = 0;

	/* root */
	tree[ctx->heap[ctx->heap_back]].dl.len = 0;

	for (int32_t i = ctx->heap_back + 1; i < DEFLATE_HEAP_SIZE; i++) {
		n = ctx->heap[i]; /* tree index */
		len = tree[tree[n].dl.dad].dl.len + 1;
		if (len > bits_max) {
			len = bits_max;
			overflow++;
		}

		tree[n].dl.len = len;
		if (n > code_max) /* parent node */
			continue;

		/* leaf node */
		ctx->bl_count[len]++;
	}
	if (!overflow)
		return;

	do {
		n = bits_max - 1;
		for (; !ctx->bl_count[n]; n--);
		ctx->bl_count[n]--; /* move the leaf down */
		ctx->bl_count[n + 1] += 2; /* move the overflow item */
		ctx->bl_count[bits_max]--;
		overflow -= 2;
	} while (overflow > 0);

	/* recompute codes length */
	len = DEFLATE_HEAP_SIZE;
	for (int32_t i = bits_max; i > 0; i--) {
		n = ctx->bl_count[i];
		while (n--) {
			m = ctx->heap[--len];
			if (m > code_max)
				continue;
			if (tree[m].dl.len != i)
				tree[m].dl.len = i;
		}
	}
}

/* @func: _build_tree (static)
 * #desc:
 *    build dynamic huffman tree.
 *
 * #1: struct struct context
 * #2: tree description
 */
static void _build_tree(struct deflate_ctx *ctx, struct deflate_tree_desc *desc)
{
	const struct deflate_ctdata *stree = desc->stree;
	struct deflate_ctdata *tree = desc->tree;
	int32_t elems = desc->elems, code_max = -1;
	int32_t n, m, node;

	ctx->heap_size = 0;
	ctx->heap_back = DEFLATE_HEAP_SIZE;
	desc->opt_slen = 0;
	desc->opt_dlen = 0;

	/* non-zero freq add to the heap */
	for (int32_t i = 0; i < elems; i++) {
		if (tree[i].fc.freq) {
			ctx->heap[++(ctx->heap_size)] = code_max = i;
			ctx->depth[i] = 0;
		} else {
			tree[i].dl.len = 0;
		}
	}

	while (ctx->heap_size < 2) {
		node = (code_max < 2) ? ++code_max : 0;
		ctx->heap[++ctx->heap_size] = node;
		tree[node].fc.freq = 1;
		ctx->depth[node] = 0;
	}
	desc->code_max = code_max;

	for (int32_t k = ctx->heap_size / 2; k > 0; k--)
		_heapdown(ctx, tree, k);

	/*
	 * canonical Huffman coding, generating a coding tree
	 * based on the codes length.
	 *
	 * freq:
	 *   'A'(65): 2
	 *   'B'(66): 2
	 *   'C'(67): 7
	 *   'D'(68): 4
	 *
	 * heap-back: 290 289 67 288 68 66 65
	 *      290
	 *      / \
	 *    67  289
	 *        / \
	 *      68   288
	 *           / \
	 *         66   65
	 *
	 * skip 290 - 290.len     = 0
	 * each 289 - 290.len + 1 = 1
	 * each 67  - 290.len + 1 = 1
	 * each 288 - 289.len + 1 = 2
	 * each 68  - 289.len + 1 = 2
	 * each 66  - 288.len + 1 = 3
	 * each 65  - 288.len + 1 = 3
	 *
	 * len:
	 *   'A'(65): 3
	 *   'B'(66): 3
	 *   'C'(67): 1
	 *   'D'(68): 2
	 */

#undef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

	node = elems;
	do {
		/* min nodes */
		n = ctx->heap[1];
		ctx->heap[1] = ctx->heap[ctx->heap_size--];
		_heapdown(ctx, tree, 1);
		m = ctx->heap[1];

		/* heap-back */
		ctx->heap[--ctx->heap_back] = n;
		ctx->heap[--ctx->heap_back] = m;

		/* parent node */
		tree[node].fc.freq = tree[n].fc.freq + tree[m].fc.freq;
		tree[n].dl.dad = tree[m].dl.dad = node;
		ctx->depth[node] = MAX(ctx->depth[n], ctx->depth[m]) + 1;

		/* insert node */
		ctx->heap[1] = node++;
		_heapdown(ctx, tree, 1);
	} while (ctx->heap_size > 1);
	ctx->heap[--ctx->heap_back] = ctx->heap[1]; /* root node */

	/* generate bits length */
	_gen_bitlen(ctx, desc);

	/* statistical the static and dynamic size */
	for (int32_t i = 0; i < elems; i++) {
		int32_t f = tree[i].fc.freq;
		if (f) {
			if (stree)
				desc->opt_slen += f * stree[i].dl.len;
			desc->opt_dlen += f * tree[i].dl.len;
		}
	}

	/* generate huffman coding */
	_gen_codes(tree, ctx->bl_count, desc->code_max + 1);
}

/* @func: _scan_tree (static)
 * #desc:
 *    scan tree length to bit-length tree.
 *
 * #1: struct struct context
 * #2: input tree
 * #3: codes number
 */
static void _scan_tree(struct deflate_ctx *ctx,
		struct deflate_ctdata *tree, int32_t n)
{
	struct deflate_ctdata *bl_tree = ctx->dyn_bltree;
	int32_t count = 0, count_min = 4, count_max = 7;
	int32_t curr, next, prev = -1;

	next = tree[0].dl.len;
	if (!next) { /* next is zero */
		count_min = 3;
		count_max = 138;
	}
	tree[n].dl.len = (uint16_t)-1; /* high-mask */

	for (int32_t i = 0; i < n; i++) {
		curr = next;
		next = tree[i + 1].dl.len;

		if (++count < count_max && curr == next)
			continue;

		if (count < count_min) {
			bl_tree[curr].fc.freq += count;
		} else if (curr) { /* curr not is zero */
			if (curr != prev)
				bl_tree[curr].fc.freq++;
			bl_tree[DEFLATE_REP_3_6].fc.freq++;
		} else if (count < 11) { /* curr is zero */
			bl_tree[DEFLATE_REPZ_3_10].fc.freq++;
		} else {
			bl_tree[DEFLATE_REPZ_11_138].fc.freq++;
		}

		count = 0;
		prev = curr;
		if (!next) { /* next is zero */
			count_min = 3;
			count_max = 138;
		} else if (curr == next) {
			count_min = 3;
			count_max = 6;
		} else { /* curr not equal next */
			count_min = 4;
			count_max = 7;
		}
	}
}

/* @func: _send_tree (static)
 * #desc:
 *    send tree length according to bit-length tree.
 *
 * #1: struct struct context
 * #2: input tree
 * #3: codes number
 */
static void _send_tree(struct deflate_ctx *ctx,
		const struct deflate_ctdata *tree, int32_t n)
{
	struct deflate_ctdata *bl_tree = ctx->dyn_bltree;
	int32_t count = 0, count_min = 4, count_max = 7;
	int32_t curr, next, prev = -1;

	next = tree[0].dl.len;
	if (!next) { /* next is zero */
		count_min = 3;
		count_max = 138;
	}

	for (int32_t i = 0; i < n; i++) {
		curr = next;
		next = tree[i + 1].dl.len;

		if (++count < count_max && curr == next)
			continue;

		if (count < count_min) {
			do {
				SEND_CODE(ctx, bl_tree, curr);
			} while (--count);
		} else if (curr) { /* curr not is zero */
			if (curr != prev) {
				SEND_CODE(ctx, bl_tree, curr);
				count--;
			}
			SEND_CODE(ctx, bl_tree, DEFLATE_REP_3_6);
			SEND_BITS(ctx, count - 3, 2);
		} else if (count < 11) { /* curr is zero */
			SEND_CODE(ctx, bl_tree, DEFLATE_REPZ_3_10);
			SEND_BITS(ctx, count - 3, 3);
		} else {
			SEND_CODE(ctx, bl_tree, DEFLATE_REPZ_11_138);
			SEND_BITS(ctx, count - 11, 7);
		}

		count = 0;
		prev = curr;
		if (!next) { /* next is zero */
			count_min = 3;
			count_max = 138;
		} else if (curr == next) {
			count_min = 3;
			count_max = 6;
		} else { /* curr not equal next */
			count_min = 4;
			count_max = 7;
		}
	}
}

/* @func: _send_block (static)
 * #desc:
 *    send huffman coding block.
 *
 * #1: struct struct context
 * #2: literal/length tree
 * #3: distance tree
 */
static void _send_block(struct deflate_ctx *ctx,
		const struct deflate_ctdata *ltree,
		const struct deflate_ctdata *dtree)
{
	uint32_t len, dist, code, extra;

	for (int32_t i = 0; i < ctx->sym_size; i++) {
		if (ctx->sym_d[i]) {
			/* length */
			len = ctx->sym_ll[i];
			code = LEN_CODE(len);
			SEND_CODE(ctx, ltree, code);

			/* length extra */
			code = len_code[len];
			extra = extra_len_bits[code];
			if (extra) {
				len -= base_len[code];
				SEND_BITS(ctx, len, extra);
			}

			/* distance */
			dist = ctx->sym_d[i] - 1;
			code = DIST_CODE(dist);
			SEND_CODE(ctx, dtree, code);

			/* distance extra */
			extra = extra_dist_bits[code];
			if (extra) {
				dist -= base_dist[code];
				SEND_BITS(ctx, dist, extra);
			}
		} else { /* literal */
			code = ctx->sym_ll[i];
			SEND_CODE(ctx, ltree, code);
		}
	}

	/* end-block */
	SEND_CODE(ctx, ltree, DEFLATE_END_BLOCK);
}

/* @func: _bits_overflow (static)
 * #desc:
 *    check bits overflow.
 *
 * #1: deflate struct context
 * #2: input tree
 * #3: codes number
 * #r: <0: overflow
 */
static int32_t _bits_overflow(struct deflate_ctx *ctx,
		const struct deflate_ctdata *tree, int32_t n)
{
	for (int32_t i = 0; i <= DEFLATE_BITS_MAX; i++)
		ctx->bl_count[i] = 0;
	for (int32_t i = 0; i < n; i++)
		ctx->bl_count[tree[i].dl.len]++;

	int32_t m, g, w;
	for (m = 1; m <= DEFLATE_BITS_MAX; m++) {
		if (ctx->bl_count[m])
			break;
	}
	for (g = DEFLATE_BITS_MAX; g > 0; g--) {
		if (ctx->bl_count[g])
			break;
	}

	for (w = 1 << m; m < g; w <<= 1, m++) {
		w -= ctx->bl_count[m];
		if (w < 0)
			return w;
	}
	w -= ctx->bl_count[g];
	if (w < 0)
		return w;

	return 0;
}

/* @func: _init_block (static)
 * #desc:
 *    initialization block.
 *
 * #1: deflate struct context
 */
static void _init_block(struct deflate_ctx *ctx)
{
	C_SYMBOL(memset)(ctx->dyn_ltree, 0, sizeof(ctx->dyn_ltree));
	C_SYMBOL(memset)(ctx->dyn_dtree, 0, sizeof(ctx->dyn_dtree));
	C_SYMBOL(memset)(ctx->dyn_bltree, 0, sizeof(ctx->dyn_bltree));

	ctx->dyn_ltree[DEFLATE_END_BLOCK].fc.freq = 1;
	ctx->sym_size = 0;
}

/* @func: _flush_block (static)
 * #desc:
 *    send compressed data block.
 *
 * #1: deflate struct context
 * #2: is finish
 */
static void _flush_block(struct deflate_ctx *ctx, int32_t flush)
{
	int32_t opt_slen, opt_dlen, code_max;

	if (ctx->lev < 2) { /* level-1 static */
		SEND_BITS(ctx, 0x01 & flush, 1);
		SEND_BITS(ctx, 0x01, 2);
		_send_block(ctx, static_ltree, static_dtree);
		ctx->sym_size = 0;
		return;
	}
	/* level 2-9 dynamic or static */

	/* build literal/length and distance tree */
	_build_tree(ctx, &ctx->desc_ltree);
	_build_tree(ctx, &ctx->desc_dtree);

	/* scan the ltree and dtree, and build bit-length tree */
	_scan_tree(ctx, ctx->dyn_ltree, ctx->desc_ltree.code_max + 1);
	_scan_tree(ctx, ctx->dyn_dtree, ctx->desc_dtree.code_max + 1);
	_build_tree(ctx, &ctx->desc_bltree);

	/* max of the bl-tree code */
	code_max = DEFLATE_BL_CODES - 1;
	for (; code_max >= 3; code_max--) {
		if (ctx->dyn_bltree[bl_order[code_max]].dl.len)
			break;
	}

	/* static and dynamic size */
	opt_slen = ctx->desc_ltree.opt_slen + ctx->desc_dtree.opt_slen
		+ 3;
	opt_dlen = ctx->desc_ltree.opt_dlen + ctx->desc_dtree.opt_dlen
		+ (3 + 5 + 5 + 4 + (3 * 19)) + 15;

	if (_bits_overflow(ctx, ctx->dyn_bltree,
			ctx->desc_bltree.code_max + 1))
		opt_slen = 0;

	/* dynamic */
	if (opt_slen > opt_dlen) {
		SEND_BITS(ctx, 0x01 & flush, 1);
		SEND_BITS(ctx, 0x02, 2);
		SEND_BITS(ctx, ctx->desc_ltree.code_max - 256, 5);
		SEND_BITS(ctx, ctx->desc_dtree.code_max, 5);
		SEND_BITS(ctx, code_max - 3, 4);

		/* send bl-tree length in order */
		for (int32_t i = 0; i <= code_max; i++) {
			SEND_BITS(ctx,
				ctx->dyn_bltree[bl_order[i]].dl.len, 3);
		}

		/* send ltree and dtree length */
		_send_tree(ctx, ctx->dyn_ltree, ctx->desc_ltree.code_max + 1);
		_send_tree(ctx, ctx->dyn_dtree, ctx->desc_dtree.code_max + 1);

		_send_block(ctx, ctx->dyn_ltree, ctx->dyn_dtree);
	} else { /* static */
		SEND_BITS(ctx, 0x01 & flush, 1);
		SEND_BITS(ctx, 0x01, 2);
		_send_block(ctx, static_ltree, static_dtree);
	}

	/* initialization block */
	_init_block(ctx);
}

/* @func: _symbol_add (static)
 * #desc:
 *    deflate symbol add.
 *
 * #1: deflate struct context
 * #2: distance (0: literal)
 * #3: literal and length (length: - DEFLATE_MATCH_MIN)
 * #r: 0: not full, 1: symbol full
 */
static int32_t _symbol_add(struct deflate_ctx *ctx, uint32_t dist, uint32_t ll)
{
	ctx->sym_d[ctx->sym_size] = (uint16_t)dist;
	ctx->sym_ll[ctx->sym_size++] = (uint8_t)ll;

	if (dist) {
		ll = LEN_CODE(ll);
		dist = DIST_CODE(dist - 1);
		ctx->dyn_ltree[ll].fc.freq++;
		ctx->dyn_dtree[dist].fc.freq++;
	} else {
		ctx->dyn_ltree[ll].fc.freq++;
	}

	return ctx->sym_size == DEFLATE_SYMSIZE;
}

/* @func: _update_hash (static)
 * #desc:
 *    update current hash.
 *
 * #1: deflate struct context
 * #2: start position of the window
 * #r: return new hash
 */
static uint32_t _update_hash(struct deflate_ctx *ctx, uint32_t n)
{
	ctx->hash = ((ctx->hash << 5) ^ ctx->window[n]) & DEFLATE_HMASK;

	return ctx->hash;
}

/* @func: _insert_hash (static)
 * #desc:
 *    insert new hash and return prev hash.
 *
 * #1: deflate struct context
 * #2: start position of the window
 * #r: prev a position
 */
static uint32_t _insert_hash(struct deflate_ctx *ctx, uint32_t n)
{
	uint32_t h, m;

	h = _update_hash(ctx, n + (DEFLATE_MATCH_MIN - 1));
	ctx->prev[n & DEFLATE_HMASK] = m = ctx->head[h];
	ctx->head[h] = n;

	return m;
}

/* @func: _longest_match (static)
 * #desc:
 *    longest match function.
 *
 * #1: deflate struct context
 * #2: match position
 * #r: best length
 */
static uint32_t _longest_match(struct deflate_ctx *ctx, uint32_t pos)
{
	uint8_t *match, *scan, *scan_end = ctx->window + ctx->start;
	uint32_t best_len = ctx->prev_len;
	uint32_t chain_len = ctx->chain;
	uint32_t nice_match = ctx->nice;
	uint32_t len, len_max = DEFLATE_MATCH_MAX;
	uint32_t lim = (ctx->start > DEFLATE_WSIZE) ?
		(ctx->start - DEFLATE_WSIZE) : 0; /* history limit */
	uint8_t *scan_end2 = scan_end + best_len;

	if (len_max > ctx->lsize) /* max scan limit */
		len_max = ctx->lsize;
	scan_end += len_max;

	if (ctx->prev_len >= ctx->good) /* reduce chain depth */
		chain_len >>= 2;
	if (ctx->nice > ctx->lsize) /* max nice length limit */
		nice_match = ctx->lsize;

	/*
	 * 64-KiB
	 * +-------------------+
	 * |  :history  |lsize |
	 * +-------------------+
	 *              ^
	 *              start
	 *     -<--<--<- --
	 *     match     best_len
	 */

	do {
		match = ctx->window + pos;
		scan = ctx->window + ctx->start;

		/* quick skip */
		if (match[best_len] != *scan_end2 || *match != *scan) {
			pos = ctx->prev[pos & DEFLATE_HMASK];
			continue;
		}

		for (; scan < scan_end && *scan == *match;
				scan++, match++);

		len = len_max - (uint32_t)(scan_end - scan);
		if (len > best_len) {
			ctx->match_start = pos; /* start position */
			best_len = len;
			if (len >= nice_match) /* meet max expect */
				break;
			scan_end2 = scan;
		}

		pos = ctx->prev[pos & DEFLATE_HMASK];
	} while (pos > lim && --chain_len);

	return best_len;
}

/* @func: _fill_window (static)
 * #desc:
 *    fill sliding window.
 *
 * #1: deflate struct context
 */
static void _fill_window(struct deflate_ctx *ctx)
{
	uint32_t n, m;
	n = DEFLATE_TSIZE - ctx->start - ctx->lsize;

	/* history processing */
	if (ctx->start > (DEFLATE_TSIZE - DEFLATE_LSIZE)) {
		C_SYMBOL(memcpy)(ctx->window,
			ctx->window + DEFLATE_WSIZE, DEFLATE_WSIZE);
		ctx->start -= DEFLATE_WSIZE;
		ctx->block_start -= DEFLATE_WSIZE;
		ctx->match_start -= DEFLATE_WSIZE;

		/* sliding hash */
		for (int32_t i = 0; i < DEFLATE_HSIZE; i++) {
			m = ctx->head[i];
			ctx->head[i] = (m > DEFLATE_WSIZE) ?
				(m - DEFLATE_WSIZE) : 0;
			m = ctx->prev[i];
			ctx->prev[i] = (m > DEFLATE_WSIZE) ?
				(m - DEFLATE_WSIZE) : 0;
		}

		n += DEFLATE_WSIZE;
	}

#undef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

	/* fill lookahead */
	n = MIN(n, ctx->s_len);
	if (!n)
		return;

	C_SYMBOL(memcpy)(ctx->window + ctx->start + ctx->lsize,
		ctx->s, n);
	ctx->s += n;
	ctx->s_len -= n;
	ctx->lsize += n;
}

/* @func: _deflate_slow (static)
 * #desc:
 *    deflate slow function.
 *
 * #1: deflate struct context
 * #2: input buffer
 * #3: input length
 * #4: is finish
 * #r: 0: no error, >0 IS_FLUSH: flush block, IS_END: flush block and end
 */
static int32_t _deflate_slow(struct deflate_ctx *ctx, const uint8_t *s,
		uint32_t len, int32_t flush)
{
	if (!ctx->flush) {
		ctx->s = s;
		ctx->s_len = len;
	} else { /* continue */
		ctx->len = 0;
		ctx->flush = 0;
	}

	uint32_t hash, dist, _len, insert_max;
	while (1) {
		/* fill sliding window */
		if (ctx->lsize < DEFLATE_LSIZE) {
			_fill_window(ctx);
			if (!ctx->lsize) /* end */
				break;
			if (!flush && (ctx->lsize < DEFLATE_LSIZE
					&& !ctx->s_len)) /* next input */
				break;
		}

		/* insert hash */
		hash = 0;
		if (ctx->lsize >= DEFLATE_MATCH_MIN)
			hash = _insert_hash(ctx, ctx->start);

		ctx->prev_start = ctx->match_start;
		ctx->prev_len = ctx->match_len;
		ctx->match_len = DEFLATE_MATCH_MIN - 1; /* init match */

		/* longest match */
		if (hash && ctx->prev_len < ctx->lazy
				&& (ctx->start - hash) < DEFLATE_WSIZE) {
			_len = ctx->match_len = _longest_match(ctx, hash);
			dist = ctx->start - ctx->match_start;

			/* distance cost selection */
			if (_len == DEFLATE_MATCH_MIN
					&& dist > DEFLATE_TOO_FAR)
				ctx->match_len = DEFLATE_MATCH_MIN - 1;
		}

		/* match less than the prev a match */
		if (ctx->prev_len >= DEFLATE_MATCH_MIN
				&& ctx->match_len <= ctx->prev_len) {
			/* prev a match */
			_len = ctx->prev_len - DEFLATE_MATCH_MIN;
			dist = ctx->start - 1 - ctx->prev_start;

			ctx->flush = _symbol_add(ctx, dist, _len);

			/* update hash and sliding position */
			insert_max = ctx->start + ctx->lsize
				- DEFLATE_MATCH_MIN;
			ctx->lsize -= ctx->prev_len - 1;
			ctx->prev_len -= 2;
			do {
				if (++ctx->start <= insert_max)
					_insert_hash(ctx, ctx->start);
			} while (--ctx->prev_len);
			ctx->start++;

			ctx->match_avail = 0;
			ctx->match_len = DEFLATE_MATCH_MIN - 1;

			/* symbol full */
			if (ctx->flush) {
				_flush_block(ctx, ctx->lsize ? 0 : flush);
				ctx->block_start = ctx->start;
				return DEFLATE_IS_FLUSH;
			}
		} else if (ctx->match_avail) {
			/* prev a literal */
			if (_symbol_add(ctx, 0, ctx->window[ctx->start - 1])) {
				/* symbol full */
				ctx->start++;
				ctx->lsize--;
				_flush_block(ctx, ctx->lsize ? 0 : flush);
				ctx->block_start = ctx->start;
				ctx->flush = 1;
				return DEFLATE_IS_FLUSH;
			}

			/* next */
			ctx->start++;
			ctx->lsize--;
		} else { /* next */
			ctx->start++;
			ctx->lsize--;
			ctx->match_avail = 1;
		}

		/* flush block > DEFLATE_WSIZE */
		if ((ctx->start - ctx->block_start) > DEFLATE_WSIZE) {
			_flush_block(ctx, ctx->lsize ? 0 : flush);
			ctx->block_start = ctx->start;
			ctx->flush = 1;
			return DEFLATE_IS_FLUSH;
		}
	}

	/* end */
	if (flush && !ctx->lsize && (ctx->sym_size || ctx->match_avail)) {
		if (ctx->match_avail)
			_symbol_add(ctx, 0, ctx->window[ctx->start - 1]);
		_flush_block(ctx, flush);
		SEND_FINISH(ctx);
		return DEFLATE_IS_END;
	} else if (flush && !ctx->lsize) {
		SEND_FINISH(ctx);
		return DEFLATE_IS_END;
	}

	return 0;
}

/* @func: _deflate_fast (static)
 * #desc:
 *    deflate fast function.
 *
 * #1: deflate struct context
 * #2: input buffer
 * #3: input length
 * #4: is finish
 * #r: 0: no error, >0 IS_FLUSH: flush block, IS_END: flush block and end
 */
static int32_t _deflate_fast(struct deflate_ctx *ctx, const uint8_t *s,
		uint32_t len, int32_t flush)
{
	if (!ctx->flush) {
		ctx->s = s;
		ctx->s_len = len;
	} else { /* continue */
		ctx->len = 0;
		ctx->flush = 0;
	}

	uint32_t hash, dist, _len;
	while (1) {
		/* fill sliding window */
		if (ctx->lsize < DEFLATE_LSIZE) {
			_fill_window(ctx);
			if (!ctx->lsize) /* end */
				break;
			if (!flush && (ctx->lsize < DEFLATE_LSIZE
					&& !ctx->s_len)) /* next input */
				break;
		}

		/* insert hash */
		hash = 0;
		if (ctx->lsize >= DEFLATE_MATCH_MIN)
			hash = _insert_hash(ctx, ctx->start);

		/* longest match */
		if (hash && (ctx->start - hash) < DEFLATE_WSIZE)
			ctx->match_len = _longest_match(ctx, hash);

		if (ctx->match_len >= DEFLATE_MATCH_MIN) {
			_len = ctx->match_len - DEFLATE_MATCH_MIN;
			dist = ctx->start - ctx->match_start;

			ctx->flush = _symbol_add(ctx, dist, _len);

			/* update hash and sliding position */
			ctx->lsize -= ctx->match_len;
			if (ctx->match_len <= ctx->lazy) {
				ctx->match_len--;
				do {
					_insert_hash(ctx, ++ctx->start);
				} while (--ctx->match_len);
				ctx->start++;
			} else {
				ctx->start += ctx->match_len;
				ctx->match_len = 0;
				ctx->hash = ctx->window[ctx->start];
				_update_hash(ctx, ctx->start + 1);
			}

			/* symbol full */
			if (ctx->flush) {
				_flush_block(ctx, ctx->lsize ? 0 : flush);
				ctx->block_start = ctx->start;
				return DEFLATE_IS_FLUSH;
			}
		} else {
			if (_symbol_add(ctx, 0, ctx->window[ctx->start])) {
				/* symbol full */
				ctx->start++;
				ctx->lsize--;
				_flush_block(ctx, ctx->lsize ? 0 : flush);
				ctx->block_start = ctx->start;
				ctx->flush = 1;
				return DEFLATE_IS_FLUSH;
			}
			ctx->start++;
			ctx->lsize--;
		}

		/* flush block > DEFLATE_WSIZE */
		if ((ctx->start - ctx->block_start) > DEFLATE_WSIZE) {
			_flush_block(ctx, ctx->lsize ? 0 : flush);
			ctx->block_start = ctx->start;
			ctx->flush = 1;
			return DEFLATE_IS_FLUSH;
		}
	}

	/* end */
	if (flush && !ctx->lsize && ctx->sym_size) {
		_flush_block(ctx, flush);
		SEND_FINISH(ctx);
		return DEFLATE_IS_END;
	} else if (flush && !ctx->lsize) {
		SEND_FINISH(ctx);
		return DEFLATE_IS_END;
	}

	return 0;
}

/* @func: _deflate_stored (static)
 * #desc:
 *    deflate stored function.
 *
 * #1: deflate struct context
 * #2: input buffer
 * #3: input length
 * #4: is finish
 * #r: 0: no error, >0 IS_FLUSH: flush block, IS_END: flush block and end
 */
static int32_t _deflate_stored(struct deflate_ctx *ctx, const uint8_t *s,
		uint32_t len, int32_t flush)
{
	if (!ctx->flush) {
		ctx->s = s;
		ctx->s_len = len;
	} else { /* continue */
		ctx->len = 0;
		ctx->flush = 0;
	}

	while (1) {
		/* fill sliding window */
		if (ctx->lsize < DEFLATE_LSIZE) {
			_fill_window(ctx);
			if (!ctx->lsize) /* end */
				break;
			if (!flush && (ctx->lsize < DEFLATE_LSIZE
					&& !ctx->s_len)) /* next input */
				break;
		}

		if (ctx->lsize > DEFLATE_LSIZE) {
			ctx->start += DEFLATE_LSIZE;
			ctx->lsize -= DEFLATE_LSIZE;
		} else {
			ctx->start += ctx->lsize;
			ctx->lsize = 0;
		}

		/* flush block > (DEFLATE_WSIZE - DEFLATE_LSIZE) */
		if (ctx->start > (DEFLATE_WSIZE - DEFLATE_LSIZE)) {
			SEND_BITS(ctx, 0x01 & flush, 1);
			SEND_BITS(ctx, 0x00, 2);
			SEND_SKIP(ctx);
			SEND_BITS(ctx, ctx->start, 16);
			SEND_BITS(ctx, 0xffff - ctx->start, 16);
			SEND_FINISH(ctx);

			C_SYMBOL(memcpy)(ctx->buf + ctx->len,
				ctx->window, ctx->start);
			ctx->len += ctx->start;

			C_SYMBOL(memcpy)(ctx->window,
				ctx->window + ctx->start, ctx->lsize);

			ctx->start = 0;
			ctx->flush = 1;
			return DEFLATE_IS_FLUSH;
		}
	}

	/* end */
	if (flush && !ctx->lsize) {
		SEND_BITS(ctx, 0x01 & flush, 1);
		SEND_BITS(ctx, 0x00, 2);
		SEND_SKIP(ctx);
		SEND_BITS(ctx, ctx->start, 16);
		SEND_BITS(ctx, 0xffff - ctx->start, 16);
		SEND_FINISH(ctx);

		C_SYMBOL(memcpy)(ctx->buf + ctx->len, ctx->window,
			ctx->start);
		ctx->len += ctx->start;
		return DEFLATE_IS_END;
	}

	return 0;
}

/* @func: deflate_init
 * #desc:
 *    deflate initialization.
 *
 * #1: deflate struct context
 * #2: compress level
 * #r: 0: no error, -1: level error
 */
int32_t F_SYMBOL(deflate_init)(struct deflate_ctx *ctx, int32_t lev)
{
	if (!(lev >= 0 && lev <= 9))
		return -1;

	/* initialization */
	C_SYMBOL(memset)(ctx->head, 0, sizeof(ctx->head));
	ctx->hash = 0;

	ctx->start = 0;
	ctx->lsize = 0;
	ctx->block_start = 0;

	ctx->prev_start = ctx->match_start = 0;
	ctx->prev_len = ctx->match_len = DEFLATE_MATCH_MIN - 1;
	ctx->match_avail = 0;

	ctx->lazy = config_table[lev][0];
	ctx->nice = config_table[lev][1];
	ctx->good = config_table[lev][2];
	ctx->chain = config_table[lev][3];

	/* dynamic tree */
	ctx->desc_ltree.stree = static_ltree;
	ctx->desc_ltree.tree = ctx->dyn_ltree;
	ctx->desc_ltree.elems = DEFLATE_L_CODES;
	ctx->desc_ltree.bits_max = DEFLATE_BITS_MAX;
	ctx->desc_dtree.stree = static_dtree;
	ctx->desc_dtree.tree = ctx->dyn_dtree;
	ctx->desc_dtree.elems = DEFLATE_D_CODES;
	ctx->desc_dtree.bits_max = DEFLATE_BITS_MAX;
	ctx->desc_bltree.stree = NULL;
	ctx->desc_bltree.tree = ctx->dyn_bltree;
	ctx->desc_bltree.elems = DEFLATE_BL_CODES;
	ctx->desc_bltree.bits_max = DEFLATE_BL_BITS_MAX;

	/* initialization block */
	_init_block(ctx);

	BITS_ADD_INIT(&ctx->bits_ctx);
	ctx->lev = lev;
	ctx->flush = 0;
	ctx->len = 0;

	return 0;
}

/* @func: deflate
 * #desc:
 *    deflate compression function.
 *
 * #1: deflate struct context
 * #2: input buffer
 * #3: input length
 * #4: is finish
 * #r: 0: no error, >0 IS_FLUSH: flush block, IS_END: flush block and end
 */
int32_t F_SYMBOL(deflate)(struct deflate_ctx *ctx, const uint8_t *s,
		uint32_t len, int32_t flush)
{
	if (!len && !flush)
		return 0;

	if (ctx->lev > 3) {
		return _deflate_slow(ctx, s, len, flush);
	} else if (ctx->lev > 0) {
		return _deflate_fast(ctx, s, len, flush);;
	}

	return _deflate_stored(ctx, s, len, flush);
}
