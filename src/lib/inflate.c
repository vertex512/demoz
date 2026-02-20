/* @file: inflate.c
 * #desc:
 *    The implementations of deflate decompression.
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
#include <demoz/ds/bits_get.h>
#include <demoz/lib/inflate.h>


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

/* bitlen output order */
static const uint8_t bl_order[INFLATE_BL_CODES] = {
	16, 17, 18,  0,  8,  7,  9,  6,
	10,  5, 11,  4, 12,  3, 13,  2,
	14,  1, 15
	};

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

/* base length */
static const uint16_t base_len[INFLATE_LEN_CODES] = {
	  3,   4,   5,   6,   7,   8,   9,  10,
	 11,  13,  15,  17,  19,  23,  27,  31,
	 35,  43,  51,  59,  67,  83,  99, 115,
	131, 163, 195, 227, 258
	};

/* base distance */
static const uint16_t base_dist[INFLATE_DIST_CODES] = {
	   1,     2,     3,     4,     5,     7,     9,    13,
	  17,    25,    33,    49,    65,    97,   129,   193,
	 257,   385,   513,   769,  1025,  1537,  2049,  3073,
	4097,  6145,  8193, 12289, 16385, 24577
	};

#define BITS_SKIP(s) _bits_skip(s)
#define BITS_FILL(s, f) \
	do { \
		if (_bits_fill(s) && !f) \
			goto e; \
	} while (0)
#define BITS_PEEK(s, v, n) \
	do { \
		if (_bits_peek(s, v, n)) \
			return INFLATE_ERR_INCOMP; \
	} while (0)
#define BITS_PEEK2(s, v, t, m, g) \
	do { \
		t = g; \
		if (_bits_peek(s, v, t)) { \
			t = m; \
			if (_bits_peek(s, v, t)) \
				return INFLATE_ERR_INCOMP; \
		} \
	} while (0)
#define BITS_DUMP(s, v, n) \
	do { \
		if (_bits_dump(s, v, n)) \
			return INFLATE_ERR_INCOMP; \
	} while (0)
/* end */

/* @func: _bits_fill (static)
 * #desc:
 *    fill the bits buffer.
 *
 * #1: ctx [in/out] inflate struct context
 * #r:     [ret]    0: no error, 1: remaining bits are less than the expected
 */
static int32_t _bits_fill(struct inflate_ctx *ctx)
{
	uint32_t n = F_SYMBOL(bits_get_fill)(&ctx->bits_ctx,
		ctx->s, ctx->s_len);
	ctx->s += n;
	ctx->s_len -= n;

	if (!ctx->s_len && BITS_GET_REMLEN(&ctx->bits_ctx) < 4)
		return 1;

	return 0;
}

/* @func: _bits_peek (static)
 * #desc:
 *    peek at the bits in the buffer.
 *
 * #1: ctx [in/out] inflate struct context
 * #2: v   [out]    bits value
 * #3: len [in]     bits length
 * #r:     [ret]    0: no error, -1: bits of no extra
 */
static int32_t _bits_peek(struct inflate_ctx *ctx, uint32_t *v, uint32_t len)
{
	if (F_SYMBOL(bits_get)(&ctx->bits_ctx, v, len, 1))
		return -1;

	return 0;
}

/* @func: _bits_dump (static)
 * #desc:
 *    bits in the dump buffer.
 *
 * #1: ctx [in/out] inflate struct context
 * #2: v   [out]    bits value
 * #3: len [in]     bits length
 * #r:     [ret]    0: no error, -1: bits of no extra
 */
static int32_t _bits_dump(struct inflate_ctx *ctx, uint32_t *v, uint32_t len)
{
	if (F_SYMBOL(bits_get)(&ctx->bits_ctx, v, len, 0))
		return -1;

	return 0;
}

/* @func: _bits_skip (static)
 * #desc:
 *    skip extra bits in the buffer byte.
 *
 * #1: ctx [in/out] inflate struct context
 */
static void _bits_skip(struct inflate_ctx *ctx)
{
	BITS_GET_SKIP(&ctx->bits_ctx);
}

/* @func: _build_sym (static)
 * #desc:
 *    generate bit-length based on symbol.
 *
 * #1: desc [in/out] inflate symbol description
 * #2: lens [in]     bit-length of the codes
 * #r:      [ret]    0: no error, -1: bits overflow
 */
static int32_t _build_sym(struct inflate_sym_desc *desc, const uint8_t *lens)
{
	uint16_t offs[INFLATE_BITS_MAX + 1];
	uint32_t elems = desc->elems;
	int32_t m, g, w;

	/* statistical bit-length */
	for (int32_t i = 0; i <= INFLATE_BITS_MAX; i++)
		desc->count[i] = 0;
	for (uint32_t i = 0; i < elems; i++)
		desc->count[lens[i]]++;

	/* max and min of the bits */
	for (m = 1; m <= INFLATE_BITS_MAX; m++) {
		if (desc->count[m])
			break;
	}
	for (g = INFLATE_BITS_MAX; g > 0; g--) {
		if (desc->count[g])
			break;
	}
	desc->bits_min = m;
	desc->bits_max = g;

	/* check bits overflow */
	for (w = 1 << m; m < g; w <<= 1, m++) {
		w -= desc->count[m];
		if (w < 0)
			return -1;
	}
	w -= desc->count[g];
	if (w < 0)
		return -1;

	/* symbol offset */
	offs[1] = 0;
	for (int32_t i = 1; i < INFLATE_BITS_MAX; i++)
		offs[i + 1] = offs[i] + desc->count[i];

	/* generate symbol */
	for (uint32_t i = 0; i < elems; i++) {
		int32_t len = lens[i];
		if (!len)
			continue;
		desc->sym[offs[len]++] = i;
	}

	return 0;
}

/* @func: _decode_sym (static)
 * #desc:
 *    decoding the symbol codes.
 *
 * #1: desc [in/out] inflate symbol description
 * #2: v    [in]     input bits
 * #3: len  [in/out] input the length and return the length used
 * #r:      [ret]    -1: decode error, >=0: symbol codes
 */
static int32_t _decode_sym(const struct inflate_sym_desc *desc, uint32_t v,
		uint32_t *len)
{
	uint32_t base = 0, offs = 0, m = 1, g = *len;

	for (; m <= g; m++) {
		offs = (offs << 1) + (v & 1);
		if (offs < desc->count[m])
			break;

		base += desc->count[m];
		offs -= desc->count[m];
		v >>= 1;
	}
	*len = m;

	offs += base;
	if (offs >= desc->elems)
		return -1;

	return desc->sym[offs];
}

/* @func: _build_fixed (static)
 * #desc:
 *    build the fixed symbol.
 *
 * #1: ctx [in/out] inflate struct context
 */
static void _build_fixed(struct inflate_ctx *ctx)
{
	uint32_t code;

	/* literal/length */
	for (code = 0; code < 144; code++)
		ctx->lens[code] = 8; /* 0~143: 8-bit length */
	for (; code < 256; code++)
		ctx->lens[code] = 9; /* 144~255: 9-bit length */
	for (; code < 280; code++)
		ctx->lens[code] = 7; /* 256~279: 7-bit length */
	for (; code < (INFLATE_L_CODES + 2); code++)
		ctx->lens[code] = 8; /* 280~287: 8-bit length */

	ctx->desc_lsym.elems = code;
	_build_sym(&ctx->desc_lsym, ctx->lens);

	/* distance */
	for (code = 0; code < INFLATE_D_CODES; code++)
		ctx->lens[code] = 5;

	ctx->desc_dsym.elems = code;
	_build_sym(&ctx->desc_dsym, ctx->lens);
}

/* @func: _inflate_block (static)
 * #desc:
 *    inflate block function.
 *
 * #1: ctx   [in/out] inflate struct context
 * #2: s     [in]     input buffer
 * #3: len   [in]     input length
 * #4: flush [in]     is finish
 * #r:       [ret]
 *    0: no error, >0 IS_FLUSH: flush block, IS_END: flush block and end,
 *    <0: ERR_INCOMP ...
 */
static int32_t _inflate_block(struct inflate_ctx *ctx, const uint8_t *s,
		uint32_t len, int32_t flush)
{
	if (!ctx->flush) {
		ctx->s = s;
		ctx->s_len = len;
	} else {
		if (ctx->flush == 2) {
			C_SYMBOL(memcpy)(ctx->window,
				ctx->window + ctx->len, INFLATE_WSIZE);
			ctx->start = INFLATE_WSIZE;
		}
		ctx->flush = 0;
		ctx->len = 0;
	}

	uint8_t *s1, *s2;
	int32_t sym;
	uint32_t v, t;
	do {
		switch (ctx->state) {
			case 0:
				BITS_FILL(ctx, flush);
				BITS_DUMP(ctx, &v, 3);
				ctx->last = v & 1;
				ctx->state = (v >> 1) + 1;
				break;
			case 1:
				BITS_SKIP(ctx);
				ctx->state = 4;
				break;
			case 2: /* fixed */
				_build_fixed(ctx);
				ctx->state = 6;
				break;
			case 3: /* dynamic */
				BITS_FILL(ctx, flush);

				/* literal/length codes */
				BITS_DUMP(ctx, &v, 5);
				v += 257;

				ctx->desc_lsym.elems = v;
				ctx->t_k = v;
				if (v > INFLATE_L_CODES)
					return INFLATE_ERR_DYN_HEAD;

				/* distance codes */
				BITS_DUMP(ctx, &v, 5);
				v += 1;

				ctx->desc_dsym.elems = v;
				ctx->t_k += v;
				if (v > INFLATE_D_CODES)
					return INFLATE_ERR_DYN_HEAD;

				/* bl-tree codes */
				BITS_DUMP(ctx, &v, 4);
				v += 4;

				ctx->t_j = v;
				if (v > INFLATE_BL_CODES)
					return INFLATE_ERR_DYN_HEAD;

				ctx->t_i = 0;
				ctx->state = 9;
				break;
			case 4: /* stored */
				BITS_FILL(ctx, flush);
				BITS_DUMP(ctx, &v, 16);
				BITS_DUMP(ctx, &t, 16);
				ctx->t_i = v;

				if (v != (~t & 0xffff))
					return INFLATE_ERR_STORED_HEAD;
				ctx->state = 5;
			case 5:
				while (ctx->t_i) {
					BITS_FILL(ctx, flush);
					BITS_DUMP(ctx, &v, 8);
					ctx->window[ctx->start++] = v;
					ctx->t_i--;
					if (ctx->start == INFLATE_TSIZE)
						break;
				}

				/* flush window */
				if (ctx->start > (INFLATE_TSIZE
						- INFLATE_LSIZE)) {
					ctx->buf = ctx->window;
					ctx->len = ctx->start
						- INFLATE_WSIZE;
					ctx->flush = 2;
					return INFLATE_IS_FLUSH;
				}

				/* last block */
				if (ctx->last) {
					ctx->buf = ctx->window;
					ctx->len = ctx->start;
					ctx->last = 2;
					return INFLATE_IS_END;
				}

				/* next block */
				ctx->flush = 1;
				ctx->state = 0;
				return INFLATE_IS_FLUSH;
			case 6: /* literal/length */
				BITS_FILL(ctx, flush);
				BITS_PEEK2(ctx, &v, t,
					ctx->desc_lsym.bits_min,
					ctx->desc_lsym.bits_max);

				/* decoding */
				sym = _decode_sym(&ctx->desc_lsym, v, &t);
				if (sym < 0)
					return INFLATE_ERR_LCODES;

				BITS_DUMP(ctx, &v, t);

				/* literal and end-block */
				if (sym < INFLATE_END_BLOCK) {
					ctx->window[ctx->start++] = sym;

					/* flush window */
					if (ctx->start > (INFLATE_TSIZE
							- INFLATE_LSIZE)) {
						ctx->buf = ctx->window;
						ctx->len = ctx->start
							- INFLATE_WSIZE;
						ctx->flush = 2;
						return INFLATE_IS_FLUSH;
					}
					break;
				} else if (sym == INFLATE_END_BLOCK) {
					/* last block */
					if (ctx->last) {
						ctx->buf = ctx->window;
						ctx->len = ctx->start;
						ctx->last = 2;
						return INFLATE_IS_END;
					}

					/* next block */
					ctx->flush = 1;
					ctx->state = 0;
					return INFLATE_IS_FLUSH;
				}

				/* length */
				sym -= INFLATE_LITERALS + 1;

				/* extra */
				t = extra_len_bits[sym];
				BITS_DUMP(ctx, &v, t);

				ctx->t_len = v + base_len[sym];
				ctx->state = 7;
			case 7: /* distance */
				BITS_FILL(ctx, flush);
				BITS_PEEK2(ctx, &v, t,
					ctx->desc_dsym.bits_min,
					ctx->desc_dsym.bits_max);

				/* decoding */
				sym = _decode_sym(&ctx->desc_dsym, v, &t);
				if (sym < 0)
					return INFLATE_ERR_DCODES;

				BITS_DUMP(ctx, &v, t);

				/* extra */
				t = extra_dist_bits[sym];
				BITS_DUMP(ctx, &v, t);

				ctx->t_dist = v + base_dist[sym];
				ctx->state = 8;
			case 8:
				v = ctx->t_len;
				t = ctx->t_dist;
				if (v > INFLATE_MATCH_MAX)
					return INFLATE_ERR_LCODES;
				if (t > ctx->start || t > INFLATE_WSIZE)
					return INFLATE_ERR_DCODES;

				/* flush window */
				if (ctx->start > (INFLATE_TSIZE
						- INFLATE_LSIZE)) {
					ctx->buf = ctx->window;
					ctx->len = ctx->start
						- INFLATE_WSIZE;
					ctx->flush = 2;
					return INFLATE_IS_FLUSH;
				}

				s1 = ctx->window + ctx->start;
				s2 = s1 - t;
				while (v--)
					*s1++ = *s2++;

				ctx->start += ctx->t_len;
				ctx->state = 6;
				break;
			case 9: /* bl-tree */
				while (ctx->t_i < ctx->t_j) {
					BITS_FILL(ctx, flush);
					BITS_DUMP(ctx, &v, 3);
					ctx->lens[bl_order[ctx->t_i++]] = v;
				}

				while (ctx->t_i < INFLATE_BL_CODES)
					ctx->lens[bl_order[ctx->t_i++]] = 0;

				/* build bl-tree */
				if (_build_sym(&ctx->desc_blsym, ctx->lens))
					return INFLATE_ERR_DYN_BLCODES;

				ctx->t_i = 0;
				ctx->state = 10;
			case 10: /* literal/length and distance tree */
				while (ctx->t_i < ctx->t_k) {
					BITS_FILL(ctx, flush);
					BITS_PEEK2(ctx, &v, t,
						ctx->desc_blsym.bits_min,
						ctx->desc_blsym.bits_max);

					/* decoding */
					sym = _decode_sym(&ctx->desc_blsym,
						v, &t);
					if (sym < 0)
						return INFLATE_ERR_DYN_HEAD;

					BITS_DUMP(ctx, &v, t);

					/* repeat */
					v = 1;
					if (sym == 16) {
						sym = ctx->lens[ctx->t_i - 1];
						BITS_DUMP(ctx, &v, 2);
						v += 3;
					} else if (sym == 17) {
						sym = 0;
						BITS_DUMP(ctx, &v, 3);
						v += 3;
					} else if (sym == 18) {
						sym = 0;
						BITS_DUMP(ctx, &v, 7);
						v += 11;
					}

					if ((ctx->t_i + (int32_t)v) > ctx->t_k)
						return INFLATE_ERR_DYN_HEAD;

					while (v--)
						ctx->lens[ctx->t_i++] = sym;
				}

				/* build ltree */
				if (_build_sym(&ctx->desc_lsym, ctx->lens))
					return INFLATE_ERR_DYN_LCODES;

				/* build dtree */
				if (_build_sym(&ctx->desc_dsym, ctx->lens
						+ ctx->desc_lsym.elems))
					return INFLATE_ERR_DYN_DCODES;

				ctx->state = 6;
				break;
			default: /* state error */
				break;
		}
	} while (1);
e:

	return 0;
}

/* @func: inflate_init
 * #desc:
 *    inflate initialization.
 *
 * #1: ctx [out] inflate struct context
 */
void F_SYMBOL(inflate_init)(struct inflate_ctx *ctx)
{
	ctx->start = 0;
	ctx->desc_lsym.sym = ctx->l_sym;
	ctx->desc_dsym.sym = ctx->d_sym;
	ctx->desc_blsym.sym = ctx->bl_sym;
	ctx->desc_blsym.elems = INFLATE_BL_CODES;

	BITS_GET_INIT(&ctx->bits_ctx);
	ctx->last = 0;
	ctx->state = 0;
	ctx->flush = 0;
	ctx->len = 0;
}

/* @func: inflate
 * #desc:
 *    deflate decompression function.
 *
 * #1: ctx   [in/out] inflate struct context
 * #2: s     [in]     input buffer
 * #3: len   [in]     input length
 * #4: flush [in]     is finish
 * #r:       [ret]
 *    0: no error, >0 IS_FLUSH: flush block, IS_END: flush block and end,
 *    <0: ERR_INCOMP ...
 */
int32_t F_SYMBOL(inflate)(struct inflate_ctx *ctx, const uint8_t *s,
		uint32_t len, int32_t flush)
{
	if ((!len && !flush) || ctx->last == 2)
		return 0;

	return _inflate_block(ctx, s, len, flush);
}
