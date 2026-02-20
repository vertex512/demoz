/* @file: hkdf_sha256.c
 * #desc:
 *    The implementations of hmac-base key derivation function (sha256).
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


/* @func: hkdf_sha256
 * #desc:
 *    hkdf-sha256 key derivation function.
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
int32_t F_SYMBOL(hkdf_sha256)(const uint8_t *ikm, uint32_t ikm_len,
		const uint8_t *salt, uint32_t salt_len,
		const uint8_t *info, uint32_t info_len,
		uint8_t *okm, uint32_t len)
{
	HMAC_SHA256_NEW(ctx);
	uint8_t tmp[SHA256_LEN], prk[SHA256_LEN], count[1];
	uint32_t f = 0, tmp_len = 0;

	if (len < 1 || len > (255 * SHA256_LEN))
		return -1;

	F_SYMBOL(hmac_sha256_init)(&ctx, salt, salt_len);
	F_SYMBOL(hmac_sha256_process)(&ctx, ikm, ikm_len);
	F_SYMBOL(hmac_sha256_finish)(&ctx, ikm_len);
	C_SYMBOL(memcpy)(prk, &(HMAC_SHA256_STATE(&ctx, 0)), SHA256_LEN);

	uint32_t n = (len + SHA256_LEN - 1) / SHA256_LEN;
	for (uint32_t i = 1; i <= n; i++) {
		count[0] = i & 0xff;

		F_SYMBOL(hmac_sha256_init)(&ctx, prk, SHA256_LEN);
		F_SYMBOL(hmac_sha256_process)(&ctx, tmp, tmp_len);
		F_SYMBOL(hmac_sha256_process)(&ctx, info, info_len);
		F_SYMBOL(hmac_sha256_process)(&ctx, count, 1);
		F_SYMBOL(hmac_sha256_finish)(&ctx, tmp_len + info_len + 1);
		tmp_len = SHA256_LEN;

		C_SYMBOL(memcpy)(tmp, &(HMAC_SHA256_STATE(&ctx, 0)),
			SHA256_LEN);
		C_SYMBOL(memcpy)(okm + f, tmp, ((f + SHA256_LEN) > len) ?
			(len % SHA256_LEN) : SHA256_LEN);
		f += SHA256_LEN;
	}

	return 0;
}

/* @func: hkdf_sha224
 * #desc:
 *    hkdf-sha224 key derivation function.
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
int32_t F_SYMBOL(hkdf_sha224)(const uint8_t *ikm, uint32_t ikm_len,
		const uint8_t *salt, uint32_t salt_len,
		const uint8_t *info, uint32_t info_len,
		uint8_t *okm, uint32_t len)
{
	HMAC_SHA224_NEW(ctx);
	uint8_t tmp[SHA224_LEN], prk[SHA224_LEN], count[1];
	uint32_t f = 0, tmp_len = 0;

	if (len < 1 || len > (255 * SHA224_LEN))
		return -1;

	F_SYMBOL(hmac_sha224_init)(&ctx, salt, salt_len);
	F_SYMBOL(hmac_sha224_process)(&ctx, ikm, ikm_len);
	F_SYMBOL(hmac_sha224_finish)(&ctx, ikm_len);
	C_SYMBOL(memcpy)(prk, &(HMAC_SHA224_STATE(&ctx, 0)), SHA224_LEN);

	uint32_t n = (len + SHA224_LEN - 1) / SHA224_LEN;
	for (uint32_t i = 1; i <= n; i++) {
		count[0] = i & 0xff;

		F_SYMBOL(hmac_sha224_init)(&ctx, prk, SHA224_LEN);
		F_SYMBOL(hmac_sha224_process)(&ctx, tmp, tmp_len);
		F_SYMBOL(hmac_sha224_process)(&ctx, info, info_len);
		F_SYMBOL(hmac_sha224_process)(&ctx, count, 1);
		F_SYMBOL(hmac_sha224_finish)(&ctx, tmp_len + info_len + 1);
		tmp_len = SHA224_LEN;

		C_SYMBOL(memcpy)(tmp, &(HMAC_SHA224_STATE(&ctx, 0)),
			SHA224_LEN);
		C_SYMBOL(memcpy)(okm + f, tmp, ((f + SHA224_LEN) > len) ?
			(len % SHA224_LEN) : SHA224_LEN);
		f += SHA224_LEN;
	}

	return 0;
}
