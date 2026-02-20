/* @file: hkdf_sha3.c
 * #desc:
 *    The implementations of hmac-base key derivation function (sha3).
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


/* @func: hkdf_sha3
 * #desc:
 *    hkdf-sha3 key derivation function.
 *
 * #1: ikm      [in]  ikm
 * #2: ikm      [in]  ikm length
 * #3: salt     [in]  salt
 * #4: salt_len [in]  salt length
 * #5: info     [in]  info
 * #6: info_len [in]  info length
 * #7: okm      [out] output key
 * #8: len      [in]  key length
 * #9: type     [in]  digest type
 * #r:          [ret]
 *    0: no error, -1: key length error, -2: digest type error
 */
int32_t F_SYMBOL(hkdf_sha3)(const uint8_t *ikm, uint32_t ikm_len,
		const uint8_t *salt, uint32_t salt_len,
		const uint8_t *info, uint32_t info_len,
		uint8_t *okm, uint32_t len, int32_t type)
{
	HMAC_SHA3_NEW(ctx);
	uint8_t tmp[SHA3_512_LEN], prk[SHA3_512_LEN], count[1];
	uint32_t f = 0, tmp_len = 0, dsize = 0;

	switch (type) {
		case HKDF_SHA3_224_TYPE:
			type = SHA3_224_TYPE;
			dsize = SHA3_224_LEN;
			break;
		case HKDF_SHA3_256_TYPE:
			type = SHA3_256_TYPE;
			dsize = SHA3_256_LEN;
			break;
		case HKDF_SHA3_384_TYPE:
			type = SHA3_384_TYPE;
			dsize = SHA3_384_LEN;
			break;
		case HKDF_SHA3_512_TYPE:
			type = SHA3_512_TYPE;
			dsize = SHA3_512_LEN;
			break;
		default:
			return -2;
	}
	if (len < 1 || len > (255 * dsize))
		return -1;

	F_SYMBOL(hmac_sha3_init)(&ctx, salt, salt_len, type);
	F_SYMBOL(hmac_sha3_process)(&ctx, ikm, ikm_len);
	F_SYMBOL(hmac_sha3_finish)(&ctx);
	C_SYMBOL(memcpy)(prk, &(HMAC_SHA3_STATE(&ctx, 0)), dsize);

	uint32_t n = (len + dsize - 1) / dsize;
	for (uint32_t i = 1; i <= n; i++) {
		count[0] = i & 0xff;

		F_SYMBOL(hmac_sha3_init)(&ctx, prk, dsize, type);
		F_SYMBOL(hmac_sha3_process)(&ctx, tmp, tmp_len);
		F_SYMBOL(hmac_sha3_process)(&ctx, info, info_len);
		F_SYMBOL(hmac_sha3_process)(&ctx, count, 1);
		F_SYMBOL(hmac_sha3_finish)(&ctx);
		tmp_len = dsize;

		C_SYMBOL(memcpy)(tmp, &(HMAC_SHA3_STATE(&ctx, 0)), dsize);
		C_SYMBOL(memcpy)(okm + f, tmp, ((f + dsize) > len) ?
			(len % dsize) : dsize);
		f += dsize;
	}

	return 0;
}
