/* @file: totp_sha1.c
 * #desc:
 *    The implementations of time-based one-time password (sha1).
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
#include <demoz/lib/hmac.h>
#include <demoz/lib/totp.h>


/* @func: totp_sha1
 * #desc:
 *    totp-sha1 one-time password function.
 *
 * #1: input key
 * #2: key length
 * #3: time base (time / time_step)
 * #4: code digest length (1~8)
 * #r: return code digest
 */
uint32_t F_SYMBOL(totp_sha1)(const uint8_t *key, uint32_t key_len,
		uint64_t time_base, uint32_t n)
{
	HMAC_SHA1_NEW(ctx);
	uint8_t tmp[SHA1_LEN], base[8];

	base[0] = (uint8_t)(time_base >> 56);
	base[1] = (uint8_t)(time_base >> 48);
	base[2] = (uint8_t)(time_base >> 40);
	base[3] = (uint8_t)(time_base >> 32);
	base[4] = (uint8_t)(time_base >> 24);
	base[5] = (uint8_t)(time_base >> 16);
	base[6] = (uint8_t)(time_base >> 8);
	base[7] = (uint8_t)time_base;

	F_SYMBOL(hmac_sha1_init)(&ctx, key, key_len);
	F_SYMBOL(hmac_sha1)(&ctx, base, sizeof(base));
	C_SYMBOL(memcpy)(tmp, &(HMAC_SHA1_STATE(&ctx, 0)), SHA1_LEN);

	uint32_t f = tmp[SHA1_LEN - 1] & 0x0f;
	uint32_t b = (uint32_t)(tmp[f] & 0x7f) << 24 \
		| (uint32_t)(tmp[f + 1] & 0xff) << 16 \
		| (uint32_t)(tmp[f + 2] & 0xff) << 8 \
		| (uint32_t)(tmp[f + 3] & 0xff);

	static const uint32_t dig_pow[9] = {
		      1,       10,       100,
		   1000,    10000,    100000,
		1000000, 10000000, 100000000
		};

	return b % dig_pow[n % 9];
}
