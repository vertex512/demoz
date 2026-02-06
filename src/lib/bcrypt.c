/* @file: bcrypt.c
 * #desc:
 *    The implementations of bcrypt password-hash.
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
#include <demoz/lib/blowfish.h>
#include <demoz/lib/bcrypt.h>


/* @func: bcrypt_hashpass
 * #desc:
 *    bcrypt password-hashing function.
 *
 * #1: password
 * #2: password length
 * #3: salt
 * #4: salt length
 * #5: output hashpass (length: BCRYPT_HASHPASS_LEN)
 * #6: cost (2^N)
 */
void F_SYMBOL(bcrypt_hashpass)(const uint8_t *pass, uint32_t pass_len,
		const uint8_t *salt, uint32_t salt_len, uint8_t *ohp,
		uint32_t k)
{
	BLOWFISH_NEW(ctx);
	uint32_t n = 1U << k;

	F_SYMBOL(blowfish_init)(&ctx, NULL, 0);
	F_SYMBOL(blowfish_ekskey)(&ctx, salt, salt_len, pass, pass_len);

	for (uint32_t i = 0; i < n; i++) {
		F_SYMBOL(blowfish_setkey)(&ctx, pass, pass_len);
		F_SYMBOL(blowfish_setkey)(&ctx, salt, salt_len);
	}

	uint32_t cdata[6] = { /* "OrpheanBeholderScryDoubt" 24-byte */
		0x4f727068, 0x65616e42, 0x65686f6c, 0x64657253,
		0x63727944, 0x6f756274
		};

	for (int32_t i = 0; i < 64; i++) {
		F_SYMBOL(blowfish_encrypt)(&ctx, &cdata[0], &cdata[1]);
		F_SYMBOL(blowfish_encrypt)(&ctx, &cdata[2], &cdata[3]);
		F_SYMBOL(blowfish_encrypt)(&ctx, &cdata[4], &cdata[5]);
	}

	for (int32_t i = 0; i < 6; i++) {
		ohp[4 * i] = (cdata[i] >> 24) & 0xff;
		ohp[4 * i + 1] = (cdata[i] >> 16) & 0xff;
		ohp[4 * i + 2] = (cdata[i] >> 8) & 0xff;
		ohp[4 * i + 3] = cdata[i] & 0xff;
	}
}

/* @func: bcrypt_auth
 * #desc:
 *    bcrypt hashpass comparison is equal.
 *
 * #1: bcrypt hashpass
 * #2: bcrypt hashpass
 * #r: 0: a != b, 1: a == b
 */
int32_t F_SYMBOL(bcrypt_auth)(const uint8_t *a, const uint8_t *b)
{
	uint32_t r = 0;
	for (int32_t i = 0; i < BCRYPT_HASHPASS_LEN; i++)
		r |= a[i] ^ b[i];

	return ((r - 1) >> 31) & 1;
}
