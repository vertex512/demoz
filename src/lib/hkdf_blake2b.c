/* @file: hkdf_blake2b.c
 * #desc:
 *    The implementations of hmac-base key derivation function (blake2b).
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
#include <demoz/lib/hkdf.h>


/* @func: hkdf_blake2b
 * #desc:
 *    hkdf-blake2b key derivation function.
 *
 * #1: ikm      [in]  ikm
 * #2: ikm      [in]  ikm length
 * #3: salt     [in]  salt
 * #4: salt_len [in]  salt length
 * #5: info     [in]  info
 * #6: info_len [in]  info length
 * #7: okm      [out] output key
 * #8: len      [in]  key length
 * #r:          [ret] 0: no error, -1: key length error
 */
int32_t F_SYMBOL(hkdf_blake2b)(const uint8_t *ikm, uint32_t ikm_len,
		const uint8_t *salt, uint32_t salt_len,
		const uint8_t *info, uint32_t info_len,
		uint8_t *okm, uint32_t len)
{
	HMAC_BLAKE2B_NEW(ctx);
	uint8_t tmp[BLAKE2B_512_LEN], prk[BLAKE2B_512_LEN], count[1];
	uint32_t f = 0, tmp_len = 0;

	if (len < 1 || len > (255 * BLAKE2B_512_LEN))
		return -1;

	F_SYMBOL(hmac_blake2b_init)(&ctx, salt, salt_len);
	F_SYMBOL(hmac_blake2b_process)(&ctx, ikm, ikm_len);
	F_SYMBOL(hmac_blake2b_finish)(&ctx);
	C_SYMBOL(memcpy)(prk, &(HMAC_BLAKE2B_STATE(&ctx, 0)),
		BLAKE2B_512_LEN);

	uint32_t n = (len + BLAKE2B_512_LEN - 1) / BLAKE2B_512_LEN;
	for (uint32_t i = 1; i <= n; i++) {
		count[0] = i & 0xff;

		F_SYMBOL(hmac_blake2b_init)(&ctx, prk, BLAKE2B_512_LEN);
		F_SYMBOL(hmac_blake2b_process)(&ctx, tmp, tmp_len);
		F_SYMBOL(hmac_blake2b_process)(&ctx, info, info_len);
		F_SYMBOL(hmac_blake2b_process)(&ctx, count, 1);
		F_SYMBOL(hmac_blake2b_finish)(&ctx);
		tmp_len = BLAKE2B_512_LEN;

		C_SYMBOL(memcpy)(tmp, &(HMAC_BLAKE2B_STATE(&ctx, 0)),
			BLAKE2B_512_LEN);
		C_SYMBOL(memcpy)(okm + f, tmp, ((f + BLAKE2B_512_LEN) > len) ?
			(len % BLAKE2B_512_LEN) : BLAKE2B_512_LEN);
		f += BLAKE2B_512_LEN;
	}

	return 0;
}
