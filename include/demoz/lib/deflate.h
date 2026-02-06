/* @file: deflate.h
 * #desc:
 *    The definitions of deflate compression algorithm.
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

#ifndef _DEMOZ_LIB_DEFLATE_H
#define _DEMOZ_LIB_DEFLATE_H

#include <demoz/config.h>
#include <demoz/c/stddef.h>
#include <demoz/c/stdint.h>
#include <demoz/ds/bits_add.h>


/* @def: _
 * max bits */
#define DEFLATE_BITS_MAX 15

/* max bits of the bit-length tree */
#define DEFLATE_BL_BITS_MAX 7

/* min and max of the match length */
#define DEFLATE_MATCH_MIN 3
#define DEFLATE_MATCH_MAX 258

/* min lookahead size */
#define DEFLATE_LSIZE (DEFLATE_MATCH_MAX + DEFLATE_MATCH_MIN + 1)
/* sliding window size */
#define DEFLATE_WSIZE (1 << 15)
/* history and sliding window size */
#define DEFLATE_TSIZE (1 << 16)

/* hash-chain size */
#define DEFLATE_HSIZE DEFLATE_WSIZE
/* hash size mask */
#define DEFLATE_HMASK (DEFLATE_HSIZE - 1)

/* number of length codes */
#define DEFLATE_LEN_CODES 29
/* number of distance codes */
#define DEFLATE_DIST_CODES 30
/* number of bit-length codes */
#define DEFLATE_BL_CODES 19
/* number of literal codes */
#define DEFLATE_LITERALS 256

/* end-block code */
#define DEFLATE_END_BLOCK 256

/* number of literal/length codes */
#define DEFLATE_L_CODES (DEFLATE_LITERALS + 1 + DEFLATE_LEN_CODES)
/* number of distance codes */
#define DEFLATE_D_CODES DEFLATE_DIST_CODES

/* number of length codes mapping */
#define DEFLATE_LEN_SIZE (DEFLATE_MATCH_MAX - DEFLATE_MATCH_MIN + 1)
/* number of distance codes mapping */
#define DEFLATE_DIST_SIZE ((DEFLATE_MATCH_MAX - DEFLATE_MATCH_MIN + 1) * 2)

/* dynamic tree size */
#define DEFLATE_HEAP_SIZE (DEFLATE_L_CODES * 2 + 1)
#define DEFLATE_DYN_L_SIZE DEFLATE_HEAP_SIZE
#define DEFLATE_DYN_D_SIZE (DEFLATE_D_CODES * 2 + 1)
#define DEFLATE_DYN_BL_SIZE (DEFLATE_BL_CODES * 2 + 1)

/* repeat previous bit length 3-6 times (2-bit) */
#define DEFLATE_REP_3_6 16
/* repeat a zero length 3-10 times (3-bit) */
#define DEFLATE_REPZ_3_10 17
/* repeat a zero length 11-138 times (7-bit) */
#define DEFLATE_REPZ_11_138 18

/* max symbol number */
#define DEFLATE_SYMSIZE ((1 << 13) + 2048)

/* max distance discard */
#define DEFLATE_TOO_FAR 4096

/* flush buffer */
#define DEFLATE_IS_FLUSH 1
/* flush buffer and end */
#define DEFLATE_IS_END 2

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

struct deflate_tree_desc {
	const struct deflate_ctdata *stree; /* static tree */
	struct deflate_ctdata *tree; /* dynamic tree */
	int32_t elems;    /* codes number (static) */
	int32_t bits_max; /* max codes bits (static) */
	int32_t code_max; /* codes of max */
	int32_t opt_slen; /* static coding length */
	int32_t opt_dlen; /* dynamic coding length */
};

struct deflate_ctx {
	uint8_t window[DEFLATE_TSIZE]; /* sliding window */
	uint16_t prev[DEFLATE_HSIZE];  /* prev hash-chain */
	uint16_t head[DEFLATE_HSIZE];  /* head hash-table */
	uint32_t hash; /* current hash */

	uint32_t start;       /* sliding position of the window */
	uint32_t lsize;       /* lookahead buffer length in the window */
	uint32_t block_start; /* previous block position in the window */

	uint32_t prev_start;  /* start of the prev match */
	uint32_t prev_len;    /* length of the prev match */
	uint32_t match_start; /* start of the match */
	uint32_t match_len;   /* length of the match */
	uint32_t match_avail; /* available lazy match state */

	uint32_t nice;  /* nice match length */
	uint32_t lazy;  /* max lazy match */
	uint32_t good;  /* prev length meet expect */
	uint32_t chain; /* hash chain length */

	uint16_t sym_d[DEFLATE_SYMSIZE];
	uint8_t sym_ll[DEFLATE_SYMSIZE];
	int32_t sym_size;

	/* bit-length count */
	uint16_t bl_count[DEFLATE_BITS_MAX + 1];
	/* minheap array (tree index) */
	uint16_t heap[DEFLATE_HEAP_SIZE];
	/* node depth (use when the freq are same) */
	uint8_t depth[DEFLATE_HEAP_SIZE];
	int32_t heap_size; /* heap elems number */
	int32_t heap_back; /* heap-back pos */

	/* dynamic tree */
	struct deflate_tree_desc desc_ltree;
	struct deflate_tree_desc desc_dtree;
	struct deflate_tree_desc desc_bltree;
	struct deflate_ctdata dyn_ltree[DEFLATE_DYN_L_SIZE];
	struct deflate_ctdata dyn_dtree[DEFLATE_DYN_D_SIZE];
	struct deflate_ctdata dyn_bltree[DEFLATE_DYN_BL_SIZE];

	const uint8_t *s; /* input buffer */
	uint32_t s_len;   /* input length */

	struct bits_add_ctx bits_ctx;
	int32_t lev;
	int32_t flush;

	uint8_t buf[DEFLATE_WSIZE + 16];
	uint32_t len;
};

#define DEFLATE_NEW(x) struct deflate_ctx x

#define DEFLATE_BUF(x) ((x)->buf)
#define DEFLATE_LEN(x) ((x)->len)
/* end */


#ifdef __cplusplus
extern "C" {
#endif

/* lib/deflate.c */

extern
int32_t F_SYMBOL(deflate_init)(struct deflate_ctx *ctx, int32_t lev)
;

extern
int32_t F_SYMBOL(deflate)(struct deflate_ctx *ctx, const uint8_t *s,
		uint32_t len, int32_t flush)
;

#ifdef __cplusplus
}
#endif


#endif
