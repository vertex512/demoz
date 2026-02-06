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
 * max bits */
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
