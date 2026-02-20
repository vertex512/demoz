/* @file: pbkdf2_sha256.c
 * #desc:
 *    The implementations of password-based key derivation function (sha256).
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
#include <demoz/lib/pbkdf2.h>


/* @func: pbkdf2_sha256
 * #desc:
 *    pbkdf2-sha256 key derivation function.
 *
 * #1: pass     [in]  password
 * #2: pass_len [in]  password length
 * #3: salt     [in]  salt
 * #4: salt_len [in]  salt length
 * #5: odk      [out] output key
 * #6: len      [in]  key length
 * #7: k        [in]  iterations number
 */
void F_SYMBOL(pbkdf2_sha256)(const uint8_t *pass, uint32_t pass_len,
		const uint8_t *salt, uint32_t salt_len, uint8_t *odk,
		uint32_t len, uint32_t k)
{
	HMAC_SHA256_NEW(ctx);
	uint8_t tmp[SHA256_LEN], u[SHA256_LEN], count[4];
	uint32_t f = 0;

	uint32_t n = (len + SHA256_LEN - 1) / SHA256_LEN;
	for (uint32_t i = 1; i <= n; i++) {
		count[0] = (i >> 24) & 0xff;
		count[1] = (i >> 16) & 0xff;
		count[2] = (i >> 8) & 0xff;
		count[3] = i & 0xff;

		F_SYMBOL(hmac_sha256_init)(&ctx, pass, pass_len);
		F_SYMBOL(hmac_sha256_process)(&ctx, salt, salt_len);
		F_SYMBOL(hmac_sha256_process)(&ctx, count, 4);
		F_SYMBOL(hmac_sha256_finish)(&ctx, salt_len + 4);
		C_SYMBOL(memcpy)(u, &(HMAC_SHA256_STATE(&ctx, 0)),
			SHA256_LEN);
		C_SYMBOL(memcpy)(tmp, &(HMAC_SHA256_STATE(&ctx, 0)),
			SHA256_LEN);

		for (uint32_t j = 1; j < k; j++) {
			F_SYMBOL(hmac_sha256_init)(&ctx, pass, pass_len);
			F_SYMBOL(hmac_sha256_process)(&ctx, u, SHA256_LEN);
			F_SYMBOL(hmac_sha256_finish)(&ctx, SHA256_LEN);
			C_SYMBOL(memcpy)(u, &(HMAC_SHA256_STATE(&ctx, 0)),
				SHA256_LEN);

			for (int32_t l = 0; l < SHA256_LEN; l++)
				tmp[l] ^= HMAC_SHA256_STATE(&ctx, l);
		}

		C_SYMBOL(memcpy)(odk + f, tmp, ((f + SHA256_LEN) > len) ?
			(len % SHA256_LEN) : SHA256_LEN);
		f += SHA256_LEN;
	}
}

/* @func: pbkdf2_sha224
 * #desc:
 *    pbkdf2-sha224 key derivation function.
 *
 * #1: pass     [in]  password
 * #2: pass_len [in]  password length
 * #3: salt     [in]  salt
 * #4: salt_len [in]  salt length
 * #5: odk      [out] output key
 * #6: len      [in]  key length
 * #7: k        [in]  iterations number
 */
void F_SYMBOL(pbkdf2_sha224)(const uint8_t *pass, uint32_t pass_len,
		const uint8_t *salt, uint32_t salt_len, uint8_t *odk,
		uint32_t len, uint32_t k)
{
	HMAC_SHA224_NEW(ctx);
	uint8_t tmp[SHA224_LEN], u[SHA224_LEN], count[4];
	uint32_t f = 0;

	uint32_t n = (len + SHA224_LEN - 1) / SHA224_LEN;
	for (uint32_t i = 1; i <= n; i++) {
		count[0] = (i >> 24) & 0xff;
		count[1] = (i >> 16) & 0xff;
		count[2] = (i >> 8) & 0xff;
		count[3] = i & 0xff;

		F_SYMBOL(hmac_sha224_init)(&ctx, pass, pass_len);
		F_SYMBOL(hmac_sha224_process)(&ctx, salt, salt_len);
		F_SYMBOL(hmac_sha224_process)(&ctx, count, 4);
		F_SYMBOL(hmac_sha224_finish)(&ctx, salt_len + 4);
		C_SYMBOL(memcpy)(u, &(HMAC_SHA224_STATE(&ctx, 0)),
			SHA224_LEN);
		C_SYMBOL(memcpy)(tmp, &(HMAC_SHA224_STATE(&ctx, 0)),
			SHA224_LEN);

		for (uint32_t j = 1; j < k; j++) {
			F_SYMBOL(hmac_sha224_init)(&ctx, pass, pass_len);
			F_SYMBOL(hmac_sha224_process)(&ctx, u, SHA224_LEN);
			F_SYMBOL(hmac_sha224_finish)(&ctx, SHA224_LEN);
			C_SYMBOL(memcpy)(u, &(HMAC_SHA224_STATE(&ctx, 0)),
				SHA224_LEN);

			for (int32_t l = 0; l < SHA224_LEN; l++)
				tmp[l] ^= HMAC_SHA224_STATE(&ctx, l);
		}

		C_SYMBOL(memcpy)(odk + f, tmp, ((f + SHA224_LEN) > len) ?
			(len % SHA224_LEN) : SHA224_LEN);
		f += SHA224_LEN;
	}
}
