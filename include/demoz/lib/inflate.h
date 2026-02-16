/* @file: inflate.h
 * #desc:
 *    The definitions of deflate decompression.
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

#ifndef _DEMOZ_LIB_INFLATE_H
#define _DEMOZ_LIB_INFLATE_H

#include <demoz/config.h>
#include <demoz/c/stddef.h>
#include <demoz/c/stdint.h>
#include <demoz/ds/bits_get.h>


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

/* max bits */
#define INFLATE_BITS_MAX 15

/* min and max of the match length */
#define INFLATE_MATCH_MIN 3
#define INFLATE_MATCH_MAX 258

/* min lookahead size */
#define INFLATE_LSIZE (INFLATE_MATCH_MAX + INFLATE_MATCH_MIN + 1)
/* sliding window size */
#define INFLATE_WSIZE (1 << 15)
/* history and sliding window size */
#define INFLATE_TSIZE (1 << 16)

/* number of length codes */
#define INFLATE_LEN_CODES 29
/* number of distance codes */
#define INFLATE_DIST_CODES 30
/* number of bit-length codes */
#define INFLATE_BL_CODES 19
/* number of literal codes */
#define INFLATE_LITERALS 256

/* end-block code */
#define INFLATE_END_BLOCK 256

/* number of literal/length codes */
#define INFLATE_L_CODES (INFLATE_LITERALS + 1 + INFLATE_LEN_CODES)
/* number of distance codes */
#define INFLATE_D_CODES INFLATE_DIST_CODES

/* flush buffer */
#define INFLATE_IS_FLUSH 1
/* flush buffer and end */
#define INFLATE_IS_END 2
/* data incomplete */
#define INFLATE_ERR_INCOMP -1
/* literal/length codes error */
#define INFLATE_ERR_LCODES -2
/* distance codes error */
#define INFLATE_ERR_DCODES -3
/* stored block header error */
#define INFLATE_ERR_STORED_HEAD -4
/* dynamic block header error */
#define INFLATE_ERR_DYN_HEAD -5
/* dynamic bit-length tree error */
#define INFLATE_ERR_DYN_BLCODES -6
/* dynamic literal/length tree error */
#define INFLATE_ERR_DYN_LCODES -7
/* dynamic distance tree error */
#define INFLATE_ERR_DYN_DCODES -8

struct inflate_sym_desc {
	uint16_t count[INFLATE_BITS_MAX + 1];
	uint16_t *sym;
	uint32_t elems;
	uint16_t bits_min;
	uint16_t bits_max;
};

struct inflate_ctx {
	uint8_t window[INFLATE_TSIZE]; /* sliding window */
	uint32_t start;                /* sliding position of the window */

	/* dynamic symbol */
	struct inflate_sym_desc desc_lsym;
	struct inflate_sym_desc desc_dsym;
	struct inflate_sym_desc desc_blsym;
	uint16_t l_sym[INFLATE_L_CODES + 2];
	uint16_t d_sym[INFLATE_D_CODES];
	uint16_t bl_sym[INFLATE_BL_CODES];
	uint8_t lens[INFLATE_L_CODES + INFLATE_D_CODES + 2];

	struct bits_get_ctx bits_ctx;
	const uint8_t *s; /* input buffer */
	uint32_t s_len;   /* input length */

	uint32_t t_len;
	uint32_t t_dist;
	int32_t t_i;
	int32_t t_j;
	int32_t t_k;
	int32_t last;
	int32_t state;
	int32_t flush;

	const uint8_t *buf;
	uint32_t len;
};

#define INFLATE_NEW(x) struct inflate_ctx x

#define INFLATE_BUF(x) ((x)->buf)
#define INFLATE_LEN(x) ((x)->len)

/* inflate tail offset of input buffer */
#define INFLATE_OFFSET(x, n) \
	((n) - (x)->s_len - BITS_GET_REMLEN(&(x)->bits_ctx))
/* end */


#ifdef __cplusplus
extern "C" {
#endif

/* lib/inflate.c */

extern
void F_SYMBOL(inflate_init)(struct inflate_ctx *ctx)
;

extern
int32_t F_SYMBOL(inflate)(struct inflate_ctx *ctx, const uint8_t *s,
		uint32_t len, int32_t flush)
;

#ifdef __cplusplus
}
#endif


#endif
