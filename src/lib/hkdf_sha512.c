/* @file: hkdf_sha512.c
 * #desc:
 *    The implementations of hmac-base key derivation function (sha512).
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


/* @func: hkdf_sha512
 * #desc:
 *    hkdf-sha512 key derivation function.
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
int32_t F_SYMBOL(hkdf_sha512)(const uint8_t *ikm, uint32_t ikm_len,
		const uint8_t *salt, uint32_t salt_len,
		const uint8_t *info, uint32_t info_len,
		uint8_t *okm, uint32_t len)
{
	HMAC_SHA512_NEW(ctx);
	uint8_t tmp[SHA512_LEN], prk[SHA512_LEN], count[1];
	uint32_t f = 0, tmp_len = 0;

	if (len < 1 || len > (255 * SHA512_LEN))
		return -1;

	F_SYMBOL(hmac_sha512_init)(&ctx, salt, salt_len);
	F_SYMBOL(hmac_sha512_process)(&ctx, ikm, ikm_len);
	F_SYMBOL(hmac_sha512_finish)(&ctx, ikm_len);
	C_SYMBOL(memcpy)(prk, &(HMAC_SHA512_STATE(&ctx, 0)), SHA512_LEN);

	uint32_t n = (len + SHA512_LEN - 1) / SHA512_LEN;
	for (uint32_t i = 1; i <= n; i++) {
		count[0] = i & 0xff;

		F_SYMBOL(hmac_sha512_init)(&ctx, prk, SHA512_LEN);
		F_SYMBOL(hmac_sha512_process)(&ctx, tmp, tmp_len);
		F_SYMBOL(hmac_sha512_process)(&ctx, info, info_len);
		F_SYMBOL(hmac_sha512_process)(&ctx, count, 1);
		F_SYMBOL(hmac_sha512_finish)(&ctx, tmp_len + info_len + 1);
		tmp_len = SHA512_LEN;

		C_SYMBOL(memcpy)(tmp, &(HMAC_SHA512_STATE(&ctx, 0)),
			SHA512_LEN);
		C_SYMBOL(memcpy)(okm + f, tmp, ((f + SHA512_LEN) > len) ?
			(len % SHA512_LEN) : SHA512_LEN);
		f += SHA512_LEN;
	}

	return 0;
}

/* @func: hkdf_sha384
 * #desc:
 *    hkdf-sha384 key derivation function.
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
int32_t F_SYMBOL(hkdf_sha384)(const uint8_t *ikm, uint32_t ikm_len,
		const uint8_t *salt, uint32_t salt_len,
		const uint8_t *info, uint32_t info_len,
		uint8_t *okm, uint32_t len)
{
	HMAC_SHA384_NEW(ctx);
	uint8_t tmp[SHA384_LEN], prk[SHA384_LEN], count[1];
	uint32_t f = 0, tmp_len = 0;

	if (len < 1 || len > (255 * SHA384_LEN))
		return -1;

	F_SYMBOL(hmac_sha384_init)(&ctx, salt, salt_len);
	F_SYMBOL(hmac_sha384_process)(&ctx, ikm, ikm_len);
	F_SYMBOL(hmac_sha384_finish)(&ctx, ikm_len);
	C_SYMBOL(memcpy)(prk, &(HMAC_SHA384_STATE(&ctx, 0)), SHA384_LEN);

	uint32_t n = (len + SHA384_LEN - 1) / SHA384_LEN;
	for (uint32_t i = 1; i <= n; i++) {
		count[0] = i & 0xff;

		F_SYMBOL(hmac_sha384_init)(&ctx, prk, SHA384_LEN);
		F_SYMBOL(hmac_sha384_process)(&ctx, tmp, tmp_len);
		F_SYMBOL(hmac_sha384_process)(&ctx, info, info_len);
		F_SYMBOL(hmac_sha384_process)(&ctx, count, 1);
		F_SYMBOL(hmac_sha384_finish)(&ctx, tmp_len + info_len + 1);
		tmp_len = SHA384_LEN;

		C_SYMBOL(memcpy)(tmp, &(HMAC_SHA384_STATE(&ctx, 0)),
			SHA384_LEN);
		C_SYMBOL(memcpy)(okm + f, tmp, ((f + SHA384_LEN) > len) ?
			(len % SHA384_LEN) : SHA384_LEN);
		f += SHA384_LEN;
	}

	return 0;
}
