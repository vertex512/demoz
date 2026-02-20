/* @file: base16.c
 * #desc:
 *    The implementations of base16 coding.
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
#include <demoz/lib/basenc.h>


/* @def: _ */
static const uint8_t b16e[] = "0123456789"
	"ABCDEF";

static const uint8_t b16d[256] = {
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	  0,   1,   2,   3,   4,   5,   6,   7,   8,   9, 255, 255,
	255, 255, 255, 255, 255,  10,  11,  12,  13,  14,  15, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255,  10,  11,  12,  13,  14,  15, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255
	};
/* end */

/* @func: base16_enc
 * #desc:
 *    base16 encoding function.
 *
 * #1: s    [in]     input buffer
 * #2: slen [in]     input length
 * #3: t    [out]    output buffer
 * #4: tlen [in/out] remaining length of output buffer
 */
void F_SYMBOL(base16_enc)(const char *s, uint32_t slen, char *t,
		uint32_t *tlen)
{
	uint32_t n = *tlen;
	while (slen) {
		*t++ = b16e[(uint8_t)s[0] >> 4];
		if (!--n)
			break;

		*t++ = b16e[(uint8_t)s[0] & 0x0f];
		if (!--n)
			break;

		slen--;
		s++;
	}
	*tlen = n;
}

/* @func: _base16_dec_2 (static)
 * #desc:
 *    base16 decoding.
 *
 * #1: s    [in]  input buffer (2byte)
 * #2: t    [out] output buffer
 * #3: tlen [in]  output buffer length
 * #r:      [ret] >0: output size, -1: output buffer full
 */
static int32_t _base16_dec_2(const char *s, char *t, uint32_t tlen)
{
	uint8_t c0, c1;
	c0 = b16d[(uint8_t)s[0]];
	c1 = b16d[(uint8_t)s[1]];

	/* c-1 */
	if (!tlen--)
		return -1;
	*t++ = (c0 << 4) | c1;

	return 1;
}

/* @func: base16_dec
 * #desc:
 *    base16 decoding function.
 *
 * #1: s    [in]     input buffer
 * #2: slen [in]     input length
 * #3: t    [out]    output buffer
 * #4: tlen [in/out] remaining length of output buffer
 * #r:      [ret]
 *    0: no error, >0: input error location, -1: output buffer full,
 *    -2: input incomplete
 */
int32_t F_SYMBOL(base16_dec)(const char *s, uint32_t slen, char *t,
		uint32_t *tlen)
{
	char buf[2];
	int32_t k = 0;

	for (uint32_t i = 0; i < slen; i++) {
		char c = s[i];
		if (b16d[(uint8_t)c] == 255)
			return i + 1;

		buf[k++] = c;
		if (k == 2) {
			k = _base16_dec_2(buf, t, *tlen);
			if (k < 0)
				return k;
			t += k;
			*tlen -= k;
			k = 0;
		}
	}

	return k ? -2 : 0;
}
