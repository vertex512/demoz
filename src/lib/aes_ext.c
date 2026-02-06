/* @file: aes_ext.c
 * #desc:
 *    The implementations of AES extended cipher mode.
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
#include <demoz/lib/aes.h>


/* @func: aes_encrypt_cbc
 * #desc:
 *    aes encryption cbc (cipher block chaining) mode.
 *
 * #1: aes struct context
 * #2: invector
 * #3: state buffer
 */
void F_SYMBOL(aes_encrypt_cbc)(struct aes_ctx *ctx, uint8_t *iv,
		uint8_t *state)
{
	for (int32_t i = 0; i < AES_BLOCKSIZE; i++)
		state[i] ^= iv[i];

	F_SYMBOL(aes_encrypt)(ctx, state);

	for (int32_t i = 0; i < AES_BLOCKSIZE; i++)
		iv[i] = state[i];
}

/* @func: aes_decrypt_cbc
 * #desc:
 *    aes decryption cbc (cipher block chaining) mode.
 *
 * #1: aes struct context
 * #2: invector
 * #3: state buffer
 */
void F_SYMBOL(aes_decrypt_cbc)(struct aes_ctx *ctx, uint8_t *iv,
		uint8_t *state)
{
	uint8_t tmp[AES_BLOCKSIZE];
	for (int32_t i = 0; i < AES_BLOCKSIZE; i++)
		tmp[i] = state[i];

	F_SYMBOL(aes_decrypt)(ctx, state);

	for (int32_t i = 0; i < AES_BLOCKSIZE; i++) {
		state[i] ^= iv[i];
		iv[i] = tmp[i];
	}
}

/* @func: aes_encrypt_cfb
 * #desc:
 *    aes encryption cfb (cipher feedback) mode.
 *
 * #1: aes struct context
 * #2: invector
 * #3: state buffer
 */
void F_SYMBOL(aes_encrypt_cfb)(struct aes_ctx *ctx, uint8_t *iv,
		uint8_t *state)
{
	F_SYMBOL(aes_encrypt)(ctx, iv);

	for (int32_t i = 0; i < AES_BLOCKSIZE; i++) {
		state[i] ^= iv[i];
		iv[i] = state[i];
	}
}

/* @func: aes_decrypt_cfb
 * #desc:
 *    aes decryption cfb (cipher feedback) mode.
 *
 * #1: aes struct context
 * #2: invector
 * #3: state buffer
 */
void F_SYMBOL(aes_decrypt_cfb)(struct aes_ctx *ctx, uint8_t *iv,
		uint8_t *state)
{
	uint8_t tmp[AES_BLOCKSIZE];
	for (int32_t i = 0; i < AES_BLOCKSIZE; i++)
		tmp[i] = state[i];

	F_SYMBOL(aes_encrypt)(ctx, iv);

	for (int32_t i = 0; i < AES_BLOCKSIZE; i++) {
		state[i] ^= iv[i];
		iv[i] = tmp[i];
	}
}

/* @func: aes_crypto_ofb
 * #desc:
 *    aes stream encryption ofb (output feedback) mode.
 *
 * #1: aes struct context
 * #2: invector
 * #3: buffer
 * #4: length
 */
void F_SYMBOL(aes_crypto_ofb)(struct aes_ctx *ctx, uint8_t *iv,
		uint8_t *buf, size_t len)
{
	uint32_t n = AES_BLOCKSIZE;
	while (len) {
		if (len < n)
			n = len;

		F_SYMBOL(aes_encrypt)(ctx, iv);
		for (uint32_t i = 0; i < n; i++)
			buf[i] ^= iv[i];

		buf += n;
		len -= n;
	}
}

/* @func: aes_crypto_ctr
 * #desc:
 *    aes stream encryption ctr (counter) mode.
 *
 * #1: aes struct context
 * #2: nonce
 * #3: buffer
 * #4: length
 */
void F_SYMBOL(aes_crypto_ctr)(struct aes_ctx *ctx, uint8_t *ran,
		uint8_t *buf, size_t len)
{
	union {
		uint32_t count0;
		uint8_t counter[AES_BLOCKSIZE];
	} u;

	for (int32_t i = 0; i < AES_BLOCKSIZE; i++)
		u.counter[i] = ran[i];

	uint32_t n = AES_BLOCKSIZE;
	while (len) {
		if (len < n)
			n = len;

		F_SYMBOL(aes_encrypt)(ctx, ran);
		for (uint32_t i = 0; i < n; i++)
			buf[i] ^= ran[i];

		buf += n;
		len -= n;

		u.count0++;
		for (int32_t i = 0; i < AES_BLOCKSIZE; i++)
			ran[i] = u.counter[i];
	}
}
