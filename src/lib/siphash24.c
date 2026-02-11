/* @file: siphash24.c
 * #desc:
 *    The implementations of siphash message authentication code.
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
#include <demoz/c/stddef.h>
#include <demoz/c/stdint.h>
#include <demoz/lib/siphash24.h>


/* @def: _ */
#define PACK8(x) ((uint64_t)((x)[0]) | (uint64_t)((x)[1]) << 8 \
	| (uint64_t)((x)[2]) << 16 | (uint64_t)((x)[3]) << 24 \
	| (uint64_t)((x)[4]) << 32 | (uint64_t)((x)[5]) << 40 \
	| (uint64_t)((x)[6]) << 48 | (uint64_t)((x)[7]) << 56)

#define ROTL(x, n) (((x) << (n)) | ((x) >> (64 - (n))))

#define ROUND(v0, v1, v2, v3) \
	v0 += v1; \
	v1 = ROTL(v1, 13); \
	v1 ^= v0; \
	v0 = ROTL(v0, 32); \
	v2 += v3; \
	v3 = ROTL(v3, 16); \
	v3 ^= v2; \
	v0 += v3; \
	v3 = ROTL(v3, 21); \
	v3 ^= v0; \
	v2 += v1; \
	v1 = ROTL(v1, 17); \
	v1 ^= v2; \
	v2 = ROTL(v2, 32)
/* end */

/* @func: siphash24
 * #desc:
 *    siphash message authentication function.
 *
 * #1: input buffet
 * #2: input length
 * #3: input key
 * #r: return digest
 */
uint64_t F_SYMBOL(siphash24)(const uint8_t *s, size_t len, const uint8_t *key)
{
	uint64_t v0 = 0x736f6d6570736575ULL;
	uint64_t v1 = 0x646f72616e646f6dULL;
	uint64_t v2 = 0x6c7967656e657261ULL;
	uint64_t v3 = 0x7465646279746573ULL;
	uint64_t k0 = PACK8(key);
	uint64_t k1 = PACK8(key + 8);
	uint64_t m;

	v0 ^= k0;
	v1 ^= k1;
	v2 ^= k0;
	v3 ^= k1;

	const uint8_t *e = s + len - (len % 8);
	for (; s != e; s += 8) {
		m = PACK8(s);
		v3 ^= m;
		ROUND(v0, v1, v2, v3);
		ROUND(v0, v1, v2, v3);
		v0 ^= m;
	}

	m = (uint64_t)len << 56;
	switch (len & 7) {
		case 7: m |= (uint64_t)s[6] << 48;
		case 6: m |= (uint64_t)s[5] << 40;
		case 5: m |= (uint64_t)s[4] << 32;
		case 4: m |= (uint64_t)s[3] << 24;
		case 3: m |= (uint64_t)s[2] << 16;
		case 2: m |= (uint64_t)s[1] << 8;
		case 1: m |= (uint64_t)s[0];
		default: break;
	}

	v3 ^= m;
	ROUND(v0, v1, v2, v3);
	ROUND(v0, v1, v2, v3);
	v0 ^= m;

	v2 ^= 0xff;
	ROUND(v0, v1, v2, v3);
	ROUND(v0, v1, v2, v3);
	ROUND(v0, v1, v2, v3);
	ROUND(v0, v1, v2, v3);

	return v0 ^ v1 ^ v2 ^ v3;
}

/* @func: siphashx24
 * #desc:
 *    siphash of variants message authentication function.
 *
 * #1: input buffet
 * #2: input length
 * #3: input key
 * #4: digest (high) / NULL
 * #r: return digest (low)
 */
uint64_t F_SYMBOL(siphashx24)(const uint8_t *s, size_t len, const uint8_t *key,
		uint64_t *h2)
{
	uint64_t v0 = 0x736f6d6570736575ULL;
	uint64_t v1 = 0x646f72616e646f6dULL;
	uint64_t v2 = 0x6c7967656e657261ULL;
	uint64_t v3 = 0x7465646279746573ULL;
	uint64_t k0 = PACK8(key);
	uint64_t k1 = PACK8(key + 8);
	uint64_t m;

	v0 ^= k0;
	v1 ^= k1;
	v2 ^= k0;
	v3 ^= k1;

	const uint8_t *e = s + len - (len % 8);
	for (; s != e; s += 8) {
		m = PACK8(s);
		v3 ^= m;
		ROUND(v0, v1, v2, v3);
		ROUND(v0, v1, v2, v3);
		v0 ^= m;
	}

	m = (uint64_t)len << 56;
	switch (len & 7) {
		case 7: m |= (uint64_t)s[6] << 48;
		case 6: m |= (uint64_t)s[5] << 40;
		case 5: m |= (uint64_t)s[4] << 32;
		case 4: m |= (uint64_t)s[3] << 24;
		case 3: m |= (uint64_t)s[2] << 16;
		case 2: m |= (uint64_t)s[1] << 8;
		case 1: m |= (uint64_t)s[0];
		default: break;
	}

	v3 ^= m;
	ROUND(v0, v1, v2, v3);
	ROUND(v0, v1, v2, v3);
	v0 ^= m;

	v2 ^= 0xee;
	ROUND(v0, v1, v2, v3);
	ROUND(v0, v1, v2, v3);
	ROUND(v0, v1, v2, v3);
	ROUND(v0, v1, v2, v3);
	m = v0 ^ v1 ^ v2 ^ v3;

	if (h2) {
		v2 ^= 0xdd;
		ROUND(v0, v1, v2, v3);
		ROUND(v0, v1, v2, v3);
		ROUND(v0, v1, v2, v3);
		ROUND(v0, v1, v2, v3);
		*h2 = v0 ^ v1 ^ v2 ^ v3;
	}

	return m;
}
