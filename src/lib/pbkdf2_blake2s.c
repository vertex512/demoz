/* @file: pbkdf2_blake2s.c
 * #desc:
 *    The implementations of password-based key derivation function (blake2s).
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


/* @func: pbkdf2_blake2s
 * #desc:
 *    pbkdf2-blake2s key derivation function.
 *
 * #1: pass     [in]  password
 * #2: pass_len [in]  password length
 * #3: salt     [in]  salt
 * #4: salt_len [in]  salt length
 * #5: odk      [out] output key
 * #6: len      [in]  key length
 * #7: k        [in]  iterations number
 */
void F_SYMBOL(pbkdf2_blake2s)(const uint8_t *pass, uint32_t pass_len,
		const uint8_t *salt, uint32_t salt_len, uint8_t *odk,
		uint32_t len, uint32_t k)
{
	HMAC_BLAKE2S_NEW(ctx);
	uint8_t tmp[BLAKE2S_256_LEN], u[BLAKE2S_256_LEN], count[4];
	uint32_t f = 0;

	uint32_t n = (len + BLAKE2S_256_LEN - 1) / BLAKE2S_256_LEN;
	for (uint32_t i = 1; i <= n; i++) {
		count[0] = (i >> 24) & 0xff;
		count[1] = (i >> 16) & 0xff;
		count[2] = (i >> 8) & 0xff;
		count[3] = i & 0xff;

		F_SYMBOL(hmac_blake2s_init)(&ctx, pass, pass_len);
		F_SYMBOL(hmac_blake2s_process)(&ctx, salt, salt_len);
		F_SYMBOL(hmac_blake2s_process)(&ctx, count, 4);
		F_SYMBOL(hmac_blake2s_finish)(&ctx);
		C_SYMBOL(memcpy)(u, &(HMAC_BLAKE2S_STATE(&ctx, 0)),
			BLAKE2S_256_LEN);
		C_SYMBOL(memcpy)(tmp, &(HMAC_BLAKE2S_STATE(&ctx, 0)),
			BLAKE2S_256_LEN);

		for (uint32_t j = 1; j < k; j++) {
			F_SYMBOL(hmac_blake2s_init)(&ctx, pass, pass_len);
			F_SYMBOL(hmac_blake2s_process)(&ctx, u,
				BLAKE2S_256_LEN);
			F_SYMBOL(hmac_blake2s_finish)(&ctx);
			C_SYMBOL(memcpy)(u, &(HMAC_BLAKE2S_STATE(&ctx, 0)),
				BLAKE2S_256_LEN);

			for (int32_t l = 0; l < BLAKE2S_256_LEN; l++)
				tmp[l] ^= HMAC_BLAKE2S_STATE(&ctx, l);
		}

		C_SYMBOL(memcpy)(odk + f, tmp, ((f + BLAKE2S_256_LEN) > len) ?
			(len % BLAKE2S_256_LEN) : BLAKE2S_256_LEN);
		f += BLAKE2S_256_LEN;
	}
}
