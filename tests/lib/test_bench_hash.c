/* @file: test_bench_hash.c
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
#include <demoz/lib/md5.h>
#include <demoz/lib/sha1.h>
#include <demoz/lib/sha2.h>
#include <demoz/lib/sha3.h>
#include <demoz/lib/blake2.h>
#include <demoz/lib/xxhash.h>
#include <demoz/lib/poly1305.h>
#include <demoz/lib/siphash24.h>


static uint8_t g_buf[1 << 20];

void test_md5(void)
{
	clock_t start, end;
	double time;
	uint64_t len;

	MD5_NEW(ctx);

	len = 0;
	F_SYMBOL(md5_init)(&ctx);

	start = clock();
	for (int32_t i = 0; i < 200; i++) {
		F_SYMBOL(md5_process)(&ctx, g_buf, sizeof(g_buf));
		len += sizeof(g_buf);
	}
	F_SYMBOL(md5_finish)(&ctx, len);
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("md5: %.6f (%.2f MiB/s)\n", time,
		(len / time) / 1024 / 1024);
}

void test_sha1(void)
{
	clock_t start, end;
	double time;
	uint64_t len;

	SHA1_NEW(ctx);

	len = 0;
	F_SYMBOL(sha1_init)(&ctx);

	start = clock();
	for (int32_t i = 0; i < 200; i++) {
		F_SYMBOL(sha1_process)(&ctx, g_buf, sizeof(g_buf));
		len += sizeof(g_buf);
	}
	F_SYMBOL(sha1_finish)(&ctx, len);
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("sha1: %.6f (%.2f MiB/s)\n", time,
		(len / time) / 1024 / 1024);
}

void test_sha256(void)
{
	clock_t start, end;
	double time;
	uint64_t len;

	SHA256_NEW(ctx);

	len = 0;
	F_SYMBOL(sha256_init)(&ctx);

	start = clock();
	for (int32_t i = 0; i < 200; i++) {
		F_SYMBOL(sha256_process)(&ctx, g_buf, sizeof(g_buf));
		len += sizeof(g_buf);
	}
	F_SYMBOL(sha256_finish)(&ctx, len);
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("sha256: %.6f (%.2f MiB/s)\n", time,
		(len / time) / 1024 / 1024);
}

void test_sha512(void)
{
	clock_t start, end;
	double time;
	uint64_t len;

	SHA512_NEW(ctx);

	len = 0;
	F_SYMBOL(sha512_init)(&ctx);

	start = clock();
	for (int32_t i = 0; i < 200; i++) {
		F_SYMBOL(sha512_process)(&ctx, g_buf, sizeof(g_buf));
		len += sizeof(g_buf);
	}
	F_SYMBOL(sha512_finish)(&ctx, len);
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("sha512: %.6f (%.2f MiB/s)\n", time,
		(len / time) / 1024 / 1024);
}

void test_sha3(void)
{
	clock_t start, end;
	double time;
	uint64_t len;

	SHA3_NEW(ctx);

	len = 0;
	F_SYMBOL(sha3_init)(&ctx, SHA3_256_TYPE, 0);

	start = clock();
	for (int32_t i = 0; i < 200; i++) {
		F_SYMBOL(sha3_process)(&ctx, g_buf, sizeof(g_buf));
		len += sizeof(g_buf);
	}
	F_SYMBOL(sha3_finish)(&ctx);
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("sha3-256: %.6f (%.2f MiB/s)\n", time,
		(len / time) / 1024 / 1024);

	len = 0;
	F_SYMBOL(sha3_init)(&ctx, SHA3_512_TYPE, 0);

	start = clock();
	for (int32_t i = 0; i < 200; i++) {
		F_SYMBOL(sha3_process)(&ctx, g_buf, sizeof(g_buf));
		len += sizeof(g_buf);
	}
	F_SYMBOL(sha3_finish)(&ctx);
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("sha3-512: %.6f (%.2f MiB/s)\n", time,
		(len / time) / 1024 / 1024);

	len = 0;
	F_SYMBOL(sha3_init)(&ctx, SHA3_SHAKE128_TYPE, 0);

	start = clock();
	for (int32_t i = 0; i < 200; i++) {
		F_SYMBOL(sha3_process)(&ctx, g_buf, sizeof(g_buf));
		len += sizeof(g_buf);
	}
	F_SYMBOL(sha3_finish)(&ctx);
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("sha3-shake128: %.6f (%.2f MiB/s)\n", time,
		(len / time) / 1024 / 1024);

	len = 0;
	F_SYMBOL(sha3_init)(&ctx, SHA3_SHAKE256_TYPE, 0);

	start = clock();
	for (int32_t i = 0; i < 200; i++) {
		F_SYMBOL(sha3_process)(&ctx, g_buf, sizeof(g_buf));
		len += sizeof(g_buf);
	}
	F_SYMBOL(sha3_finish)(&ctx);
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("sha3-shake256: %.6f (%.2f MiB/s)\n", time,
		(len / time) / 1024 / 1024);
}

void test_blake2b(void)
{
	clock_t start, end;
	double time;
	uint64_t len;

	BLAKE2B_NEW(ctx);

	len = 0;
	F_SYMBOL(blake2b_init)(&ctx, BLAKE2B_512_LEN);

	start = clock();
	for (int32_t i = 0; i < 200; i++) {
		F_SYMBOL(blake2b_process)(&ctx, g_buf, sizeof(g_buf));
		len += sizeof(g_buf);
	}
	F_SYMBOL(blake2b_finish)(&ctx);
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("blake2b: %.6f (%.2f MiB/s)\n", time,
		(len / time) / 1024 / 1024);
}

void test_blake2s(void)
{
	clock_t start, end;
	double time;
	uint64_t len;

	BLAKE2S_NEW(ctx);

	len = 0;
	F_SYMBOL(blake2s_init)(&ctx, BLAKE2S_256_LEN);

	start = clock();
	for (int32_t i = 0; i < 200; i++) {
		F_SYMBOL(blake2s_process)(&ctx, g_buf, sizeof(g_buf));
		len += sizeof(g_buf);
	}
	F_SYMBOL(blake2s_finish)(&ctx);
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("blake2s: %.6f (%.2f MiB/s)\n", time,
		(len / time) / 1024 / 1024);
}

void test_xxhash32(void)
{
	clock_t start, end;
	double time;
	uint64_t len;

	XXHASH32_NEW(ctx);

	len = 0;
	F_SYMBOL(xxhash32_init)(&ctx, 0);

	start = clock();
	for (int32_t i = 0; i < 200; i++) {
		F_SYMBOL(xxhash32_process)(&ctx, g_buf, sizeof(g_buf));
		len += sizeof(g_buf);
	}
	F_SYMBOL(xxhash32_finish)(&ctx, len);
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("xxh32: %.6f (%.2f MiB/s)\n", time,
		(len / time) / 1024 / 1024);
}

void test_xxhash64(void)
{
	clock_t start, end;
	double time;
	uint64_t len;

	XXHASH64_NEW(ctx);

	len = 0;
	F_SYMBOL(xxhash64_init)(&ctx, 0);

	start = clock();
	for (int32_t i = 0; i < 200; i++) {
		F_SYMBOL(xxhash64_process)(&ctx, g_buf, sizeof(g_buf));
		len += sizeof(g_buf);
	}
	F_SYMBOL(xxhash64_finish)(&ctx, len);
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("xxh64: %.6f (%.2f MiB/s)\n", time,
		(len / time) / 1024 / 1024);
}

void test_poly1305(void)
{
	clock_t start, end;
	double time;
	uint64_t len;

	POLY1305_NEW(ctx);

	len = 0;
	F_SYMBOL(poly1305_init)(&ctx, g_buf);

	start = clock();
	for (int32_t i = 0; i < 200; i++) {
		F_SYMBOL(poly1305_process)(&ctx, g_buf, sizeof(g_buf));
		len += sizeof(g_buf);
	}
	F_SYMBOL(poly1305_finish)(&ctx);
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("poly1305: %.6f (%.2f MiB/s)\n", time,
		(len / time) / 1024 / 1024);
}

void test_siphash24(void)
{
	clock_t start, end;
	double time;
	uint64_t len;

	len = 0;

	start = clock();
	for (int32_t i = 0; i < 200; i++) {
		F_SYMBOL(siphash24)(g_buf, sizeof(g_buf), g_buf);
		len += sizeof(g_buf);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("siphash24: %.6f (%.2f MiB/s)\n", time,
		(len / time) / 1024 / 1024);
}

int main(void)
{
	test_md5();
	test_sha1();
	test_sha256();
	test_sha512();
	test_sha3();
	test_blake2b();
	test_blake2s();
	test_xxhash32();
	test_xxhash64();
	test_poly1305();
	test_siphash24();

	return 0;
}
