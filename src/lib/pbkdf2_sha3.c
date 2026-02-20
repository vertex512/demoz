/* @file: pbkdf2_sha3.c
 * #desc:
 *    The implementations of password-based key derivation function (sha3).
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


/* @func: pbkdf2_sha3
 * #desc:
 *    pbkdf2-sha3 key derivation function.
 *
 * #1: pass     [in]  password
 * #2: pass_len [in]  password length
 * #3: salt     [in]  salt
 * #4: salt_len [in]  salt length
 * #5: odk      [out] output key
 * #6: len      [in]  key length
 * #7: k        [in]  iterations number
 * #8: type     [in]  digest type
 * #r:          [ret] 0: no error, -1: digest type error
 */
int32_t F_SYMBOL(pbkdf2_sha3)(const uint8_t *pass, uint32_t pass_len,
		const uint8_t *salt, uint32_t salt_len, uint8_t *odk,
		uint32_t len, uint32_t k, int32_t type)
{
	HMAC_SHA3_NEW(ctx);
	uint8_t tmp[SHA3_512_LEN], u[SHA3_512_LEN], count[4];
	uint32_t f = 0, dsize = 0;

	switch (type) {
		case PBKDF2_SHA3_224_TYPE:
			type = SHA3_224_TYPE;
			dsize = SHA3_224_LEN;
			break;
		case PBKDF2_SHA3_256_TYPE:
			type = SHA3_256_TYPE;
			dsize = SHA3_256_LEN;
			break;
		case PBKDF2_SHA3_384_TYPE:
			type = SHA3_384_TYPE;
			dsize = SHA3_384_LEN;
			break;
		case PBKDF2_SHA3_512_TYPE:
			type = SHA3_512_TYPE;
			dsize = SHA3_512_LEN;
			break;
		default:
			return -1;
	}

	uint32_t n = (len + dsize - 1) / dsize;
	for (uint32_t i = 1; i <= n; i++) {
		count[0] = (i >> 24) & 0xff;
		count[1] = (i >> 16) & 0xff;
		count[2] = (i >> 8) & 0xff;
		count[3] = i & 0xff;

		F_SYMBOL(hmac_sha3_init)(&ctx, pass, pass_len, type);
		F_SYMBOL(hmac_sha3_process)(&ctx, salt, salt_len);
		F_SYMBOL(hmac_sha3_process)(&ctx, count, 4);
		F_SYMBOL(hmac_sha3_finish)(&ctx);
		C_SYMBOL(memcpy)(u, &(HMAC_SHA3_STATE(&ctx, 0)), dsize);
		C_SYMBOL(memcpy)(tmp, &(HMAC_SHA3_STATE(&ctx, 0)), dsize);

		for (uint32_t j = 1; j < k; j++) {
			F_SYMBOL(hmac_sha3_init)(&ctx, pass, pass_len, type);
			F_SYMBOL(hmac_sha3_process)(&ctx, u, dsize);
			F_SYMBOL(hmac_sha3_finish)(&ctx);
			C_SYMBOL(memcpy)(u, &(HMAC_SHA3_STATE(&ctx, 0)), dsize);

			for (uint32_t l = 0; l < dsize; l++)
				tmp[l] ^= HMAC_SHA3_STATE(&ctx, l);
		}

		C_SYMBOL(memcpy)(odk + f, tmp, ((f + dsize) > len) ?
			(len % dsize) : dsize);
		f += dsize;
	}

	return 0;
}
