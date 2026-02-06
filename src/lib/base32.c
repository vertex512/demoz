/* @file: base32.c
 * #desc:
 *    The implementations of base32 coding.
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
static const uint8_t b32e[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"234567";

static const uint8_t b32d[256] = {
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255,  26,  27,  28,  29,  30,  31, 255, 255, 255, 255,
	255, 254, 255, 255, 255,   0,   1,   2,   3,   4,   5,   6,
	  7,   8,   9,  10,  11,  12,  13,  14,  15,  16,  17,  18,
	 19,  20,  21,  22,  23,  24,  25, 255, 255, 255, 255, 255,
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
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255
	};
/* end */

/* @func: base32_enc
 * #desc:
 *    base32 encoding function.
 *
 * #1: input buffer
 * #2: input length
 * #3: output buffer
 * #4: remaining length of output buffer
 */
void F_SYMBOL(base32_enc)(const char *s, uint32_t slen, char *t,
		uint32_t *tlen)
{
	uint32_t n = *tlen;
	while (slen) {
		*t++ = b32e[((uint8_t)s[0] >> 3) & 0x1f];
		if (!--n)
			break;

		*t++ = b32e[(((uint8_t)s[0] << 2)
			+ (--slen ? ((uint8_t)s[1] >> 6) : 0)) & 0x1f];
		if (!--n)
			break;

		*t++ = slen ? b32e[((uint8_t)s[1] >> 1) & 0x1f] : '=';
		if (!--n)
			break;

		*t++ = slen ? b32e[(((uint8_t)s[1] << 4)
			+ (--slen ? ((uint8_t)s[2] >> 4) : 0)) & 0x1f] : '=';
		if (!--n)
			break;

		*t++ = slen ? b32e[(((uint8_t)s[2] << 1)
			+ (--slen ? ((uint8_t)s[3] >> 7) : 0)) & 0x1f] : '=';
		if (!--n)
			break;

		*t++ = slen ? b32e[((uint8_t)s[3] >> 2) & 0x1f] : '=';
		if (!--n)
			break;

		*t++ = slen ? b32e[(((uint8_t)s[3] << 3)
			+ (--slen ? ((uint8_t)s[4] >> 5) : 0)) & 0x1f] : '=';
		if (!--n)
			break;

		*t++ = slen ? b32e[(uint8_t)s[4] & 0x1f] : '=';
		if (!--n)
			break;

		if (slen)
			slen--;
		if (slen)
			s += 5;
	}
	*tlen = n;
}

/* @func: _base32_dec_8 (static)
 * #desc:
 *    base32 decoding.
 *
 * #1: input buffer (8byte)
 * #2: output buffer
 * #3: output buffer length
 * #r: >0: output size, -1: output buffer full, -2: invalid character error
 */
static int32_t _base32_dec_8(const char *s, char *t, uint32_t tlen)
{
	uint8_t c0, c1, c2, c3, c4, c5, c6, c7;
	c0 = b32d[(uint8_t)s[0]];
	c1 = b32d[(uint8_t)s[1]];
	c2 = b32d[(uint8_t)s[2]];
	c3 = b32d[(uint8_t)s[3]];
	c4 = b32d[(uint8_t)s[4]];
	c5 = b32d[(uint8_t)s[5]];
	c6 = b32d[(uint8_t)s[6]];
	c7 = b32d[(uint8_t)s[7]];

	/* c-1 */
	if (!tlen--)
		return -1;
	*t++ = (c0 << 3) | (c1 >> 2);

	if (c2 == 254) { /* '=' */
		if (c3 == 254 && c4 == 254 && c5 == 254
				&& c6 == 254 && c7 == 254)
			return 1;
		return -2;
	}

	/* c-2 */
	if (!tlen--)
		return -1;
	*t++ = (c1 << 6) | (c2 << 1 ) | (c3 >> 4);

	if (c4 == 254) { /* '=' */
		if (c5 == 254 && c6 == 254 && c7 == 254)
			return 2;
		return -2;
	}

	/* c-3 */
	if (!tlen--)
		return -1;
	*t++ = (c3 << 4) | (c4 >> 1);

	if (c5 == 254) { /* '=' */
		if (c6 == 254 && c7 == 254)
			return 3;
		return -2;
	}

	/* c-4 */
	if (!tlen--)
		return -1;
	*t++ = (c4 << 7) | (c5 << 2) | (c6 >> 3);

	if (c7 == 254) /* '=' */
		return 4;

	/* c-5 */
	if (!tlen--)
		return -1;
	*t++ = (c6 << 5) | c7;

	return 5;
}

/* @func: base32_dec
 * #desc:
 *    base32 decoding function.
 *
 * #1: input buffer
 * #2: input length
 * #3: output buffer
 * #4: remaining length of output buffer
 * #r:
 *    0: no error, >0: input error location, -1: output buffer full,
 *    -2: invalid character error, -3: input incomplete
 */
int32_t F_SYMBOL(base32_dec)(const char *s, uint32_t slen, char *t,
		uint32_t *tlen)
{
	char buf[8];
	int32_t k = 0;

	for (uint32_t i = 0; i < slen; i++) {
		char c = s[i];
		if (b32d[(uint8_t)c] == 255)
			return i + 1;

		buf[k++] = c;
		if (k == 8) {
			k = _base32_dec_8(buf, t, *tlen);
			if (k < 0)
				return k;
			t += k;
			*tlen -= k;
			k = 0;
		}
	}

	return k ? -3 : 0;
}
