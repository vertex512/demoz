/* @file: test_bench_cipher.c
 * #desc:
 *
 * #copy:
 *    Copyright (C) 1970 Public Free Software
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program. If not,
 *    see <https://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <time.h>
#include <demoz/c/stdint.h>
#include <demoz/lib/aes.h>
#include <demoz/lib/des.h>
#include <demoz/lib/rc4.h>
#include <demoz/lib/chacha20.h>
#include <demoz/lib/salsa20.h>


static uint8_t g_buf[1 << 20];

void test_aes_ecb(void)
{
	clock_t start, end;
	double time;
	uint64_t len;

	AES_NEW(ctx);

	len = 0;
	F_SYMBOL(aes_init)(&ctx, g_buf, AES_128_TYPE);

	start = clock();
	for (int32_t i = 0; i < (1 << 21); i++) {
		F_SYMBOL(aes_encrypt)(&ctx, g_buf);
		len += AES_BLOCKSIZE;
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("aes-ecb enc: %.6f (%.2f MiB/s)\n", time,
		(len / time) / 1024 / 1024);

	len = 0;

	start = clock();
	for (int32_t i = 0; i < (1 << 21); i++) {
		F_SYMBOL(aes_decrypt)(&ctx, g_buf);
		len += AES_BLOCKSIZE;
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("aes-ecb dec: %.6f (%.2f MiB/s)\n", time,
		(len / time) / 1024 / 1024);
}

void test_aes_cbc(void)
{
	clock_t start, end;
	double time;
	uint64_t len;

	uint8_t iv[AES_BLOCKSIZE];
	AES_NEW(ctx);

	len = 0;
	F_SYMBOL(aes_init)(&ctx, g_buf, AES_128_TYPE);

	start = clock();
	for (int32_t i = 0; i < (1 << 21); i++) {
		F_SYMBOL(aes_encrypt_cbc)(&ctx, iv, g_buf);
		len += AES_BLOCKSIZE;
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("aes-cbc enc: %.6f (%.2f MiB/s)\n", time,
		(len / time) / 1024 / 1024);

	len = 0;

	start = clock();
	for (int32_t i = 0; i < (1 << 21); i++) {
		F_SYMBOL(aes_decrypt_cbc)(&ctx, iv, g_buf);
		len += AES_BLOCKSIZE;
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("aes-cbc dec: %.6f (%.2f MiB/s)\n", time,
		(len / time) / 1024 / 1024);
}

void test_aes_cfb(void)
{
	clock_t start, end;
	double time;
	uint64_t len;

	uint8_t iv[AES_BLOCKSIZE];
	AES_NEW(ctx);

	len = 0;
	F_SYMBOL(aes_init)(&ctx, g_buf, AES_128_TYPE);

	start = clock();
	for (int32_t i = 0; i < (1 << 21); i++) {
		F_SYMBOL(aes_encrypt_cfb)(&ctx, iv, g_buf);
		len += AES_BLOCKSIZE;
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("aes-cfb enc: %.6f (%.2f MiB/s)\n", time,
		(len / time) / 1024 / 1024);

	len = 0;

	start = clock();
	for (int32_t i = 0; i < (1 << 21); i++) {
		F_SYMBOL(aes_decrypt_cfb)(&ctx, iv, g_buf);
		len += AES_BLOCKSIZE;
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("aes-cfb dec: %.6f (%.2f MiB/s)\n", time,
		(len / time) / 1024 / 1024);
}

void test_aes_ofb(void)
{
	clock_t start, end;
	double time;
	uint64_t len;

	uint8_t iv[AES_BLOCKSIZE];
	AES_NEW(ctx);

	len = 0;
	F_SYMBOL(aes_init)(&ctx, g_buf, AES_128_TYPE);

	start = clock();
	for (int32_t i = 0; i < 32; i++) {
		F_SYMBOL(aes_crypto_ofb)(&ctx, iv, g_buf, sizeof(g_buf));
		len += sizeof(g_buf);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("aes-ofb enc: %.6f (%.2f MiB/s)\n", time,
		(len / time) / 1024 / 1024);

	len = 0;

	start = clock();
	for (int32_t i = 0; i < 32; i++) {
		F_SYMBOL(aes_crypto_ofb)(&ctx, iv, g_buf, sizeof(g_buf));
		len += sizeof(g_buf);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("aes-ofb dec: %.6f (%.2f MiB/s)\n", time,
		(len / time) / 1024 / 1024);
}

void test_aes_ctr(void)
{
	clock_t start, end;
	double time;
	uint64_t len;

	uint8_t iv[AES_BLOCKSIZE];
	AES_NEW(ctx);

	len = 0;
	F_SYMBOL(aes_init)(&ctx, g_buf, AES_128_TYPE);

	start = clock();
	for (int32_t i = 0; i < 32; i++) {
		F_SYMBOL(aes_crypto_ctr)(&ctx, iv, g_buf, sizeof(g_buf));
		len += sizeof(g_buf);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("aes-ctr enc: %.6f (%.2f MiB/s)\n", time,
		(len / time) / 1024 / 1024);

	len = 0;

	start = clock();
	for (int32_t i = 0; i < 32; i++) {
		F_SYMBOL(aes_crypto_ctr)(&ctx, iv, g_buf, sizeof(g_buf));
		len += sizeof(g_buf);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("aes-ctr dec: %.6f (%.2f MiB/s)\n", time,
		(len / time) / 1024 / 1024);
}

void test_des(void)
{
	clock_t start, end;
	double time;
	uint64_t len;

	DES_NEW(ctx);

	len = 0;
	F_SYMBOL(des_init)(&ctx, g_buf);

	start = clock();
	for (int32_t i = 0; i < (1 << 16); i++) {
		F_SYMBOL(des_crypto)(&ctx, g_buf, 0);
		len += DES_BLOCKSIZE;
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("des enc: %.6f (%.2f MiB/s)\n", time,
		(len / time) / 1024 / 1024);

	len = 0;

	start = clock();
	for (int32_t i = 0; i < (1 << 16); i++) {
		F_SYMBOL(des_crypto)(&ctx, g_buf, 1);
		len += DES_BLOCKSIZE;
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("des dec: %.6f (%.2f MiB/s)\n", time,
		(len / time) / 1024 / 1024);
}

void test_rc4(void)
{
	clock_t start, end;
	double time;
	uint64_t len;

	RC4_NEW(ctx);

	len = 0;
	F_SYMBOL(rc4_init)(&ctx, g_buf, 256);

	start = clock();
	for (int32_t i = 0; i < 128; i++) {
		F_SYMBOL(rc4_crypto)(&ctx, g_buf, sizeof(g_buf));
		len += sizeof(g_buf);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("rc4 enc: %.6f (%.2f MiB/s)\n", time,
		(len / time) / 1024 / 1024);

	len = 0;

	start = clock();
	for (int32_t i = 0; i < 128; i++) {
		F_SYMBOL(rc4_crypto)(&ctx, g_buf, sizeof(g_buf));
		len += sizeof(g_buf);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("rc4 dec: %.6f (%.2f MiB/s)\n", time,
		(len / time) / 1024 / 1024);
}

void test_chacha20(void)
{
	clock_t start, end;
	double time;
	uint64_t len;

	CHACHA20_NEW(ctx);

	len = 0;
	F_SYMBOL(chacha20_init)(&ctx, g_buf, g_buf, g_buf);

	start = clock();
	for (int32_t i = 0; i < 128; i++) {
		F_SYMBOL(chacha20_crypto)(&ctx, g_buf, sizeof(g_buf));
		len += sizeof(g_buf);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("chacha20 enc: %.6f (%.2f MiB/s)\n", time,
		(len / time) / 1024 / 1024);

	len = 0;

	start = clock();
	for (int32_t i = 0; i < 128; i++) {
		F_SYMBOL(chacha20_crypto)(&ctx, g_buf, sizeof(g_buf));
		len += sizeof(g_buf);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("chacha20 dec: %.6f (%.2f MiB/s)\n", time,
		(len / time) / 1024 / 1024);
}

void test_salsa20(void)
{
	clock_t start, end;
	double time;
	uint64_t len;

	SALSA20_NEW(ctx);

	len = 0;
	F_SYMBOL(salsa20_init)(&ctx, g_buf, g_buf, g_buf);

	start = clock();
	for (int32_t i = 0; i < 128; i++) {
		F_SYMBOL(salsa20_crypto)(&ctx, g_buf, sizeof(g_buf));
		len += sizeof(g_buf);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("salsa20 enc: %.6f (%.2f MiB/s)\n", time,
		(len / time) / 1024 / 1024);

	len = 0;

	start = clock();
	for (int32_t i = 0; i < 128; i++) {
		F_SYMBOL(salsa20_crypto)(&ctx, g_buf, sizeof(g_buf));
		len += sizeof(g_buf);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("salsa20 dec: %.6f (%.2f MiB/s)\n", time,
		(len / time) / 1024 / 1024);
}

int main(void)
{
	test_aes_ecb();
	test_aes_cbc();
	test_aes_cfb();
	test_aes_ofb();
	test_aes_ctr();
	test_des();
	test_rc4();
	test_chacha20();
	test_salsa20();

	return 0;
}
