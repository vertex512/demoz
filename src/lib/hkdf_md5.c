/* @file: hkdf_md5.c
 * #desc:
 *    The implementations of hmac-base key derivation function (md5).
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


/* @func: hkdf_md5
 * #desc:
 *    hkdf-md5 key derivation function.
 *
 * #1: ikm
 * #2: ikm length
 * #3: salt
 * #4: salt length
 * #5: info
 * #6: info length
 * #7: output key
 * #8: key length
 * #r: 0: no error, -1: key length error
 */
int32_t F_SYMBOL(hkdf_md5)(const uint8_t *ikm, uint32_t ikm_len,
		const uint8_t *salt, uint32_t salt_len,
		const uint8_t *info, uint32_t info_len,
		uint8_t *okm, uint32_t len)
{
	HMAC_MD5_NEW(ctx);
	uint8_t tmp[MD5_LEN], prk[MD5_LEN], count[1];
	uint32_t f = 0, tmp_len = 0;

	if (len < 1 || len > (255 * MD5_LEN))
		return -1;

	F_SYMBOL(hmac_md5_init)(&ctx, salt, salt_len);
	F_SYMBOL(hmac_md5_process)(&ctx, ikm, ikm_len);
	F_SYMBOL(hmac_md5_finish)(&ctx, ikm_len);
	C_SYMBOL(memcpy)(prk, &(HMAC_MD5_STATE(&ctx, 0)), MD5_LEN);

	uint32_t n = (len + MD5_LEN - 1) / MD5_LEN;
	for (uint32_t i = 1; i <= n; i++) {
		count[0] = i & 0xff;

		F_SYMBOL(hmac_md5_init)(&ctx, prk, MD5_LEN);
		F_SYMBOL(hmac_md5_process)(&ctx, tmp, tmp_len);
		F_SYMBOL(hmac_md5_process)(&ctx, info, info_len);
		F_SYMBOL(hmac_md5_process)(&ctx, count, 1);
		F_SYMBOL(hmac_md5_finish)(&ctx, tmp_len + info_len + 1);
		tmp_len = MD5_LEN;

		C_SYMBOL(memcpy)(tmp, &(HMAC_MD5_STATE(&ctx, 0)), MD5_LEN);
		C_SYMBOL(memcpy)(okm + f, tmp, ((f + MD5_LEN) > len) ?
			(len % MD5_LEN) : MD5_LEN);
		f += MD5_LEN;
	}

	return 0;
}
