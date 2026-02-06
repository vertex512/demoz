/* @file: test_bench_ecc.c
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
#include <demoz/lib/ecc.h>


void test_curve25519(void)
{
	clock_t start, end;
	double time;

	static const uint32_t pri[8] = {
		0x3017653f, 0x859a95e2, 0xf3f43fc6, 0xb9c8e0c4,
		0x5a577841, 0x43c64177, 0xc84563e1, 0xe529eb36
		};

	uint8_t sign[EDDSA_ED25519_SIGN_LEN];
	uint8_t pub[EDDSA_ED25519_PUB_LEN], key[ECDH_X25519_KEY_LEN];

	printf("x25519 ecdh\n");

	start = clock();
	for (int32_t i = 0; i < 500; i++) {
		F_SYMBOL(ecdh_x25519_public_key)((uint8_t *)pri, pub);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("pub time: %.6f (%.2f/s)\n", time / 500, 500 / time);

	start = clock();
	for (int32_t i = 0; i < 500; i++) {
		F_SYMBOL(ecdh_x25519_shared_key)((uint8_t *)pri, pub, key);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("key time: %.6f (%.2f/s)\n", time / 500, 500 / time);

	printf("ed25519 eddsa\n");

	start = clock();
	for (int32_t i = 0; i < 500; i++) {
		F_SYMBOL(eddsa_ed25519_public_key)((uint8_t *)pri, pub);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("pub time: %.6f (%.2f/s)\n", time / 500, 500 / time);

	start = clock();
	for (int32_t i = 0; i < 500; i++) {
		F_SYMBOL(eddsa_ed25519_sign)((uint8_t *)pri,
			sign, EDDSA_ED25519_SIGN_LEN, sign);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("sign time: %.6f (%.2f/s)\n", time / 500, 500 / time);

	start = clock();
	for (int32_t i = 0; i < 500; i++) {
		F_SYMBOL(eddsa_ed25519_verify)((uint8_t *)pub, sign,
			sign, EDDSA_ED25519_SIGN_LEN);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("verify time: %.6f (%.2f/s)\n", time / 500, 500 / time);
}

void test_curve448(void)
{
	clock_t start, end;
	double time;

	static const uint32_t pri[14] = {
		0x3017653f, 0x859a95e2, 0xf3f43fc6, 0xb9c8e0c4,
		0x5a577841, 0x43c64177, 0xc84563e1, 0xe529eb36,
		0, 0, 0, 0, 0, 0
		};

	uint8_t sign[EDDSA_ED448_SIGN_LEN];
	uint8_t pub[EDDSA_ED448_PUB_LEN], key[ECDH_X448_KEY_LEN];

	printf("x448 ecdh\n");

	start = clock();
	for (int32_t i = 0; i < 500; i++) {
		F_SYMBOL(ecdh_x448_public_key)((uint8_t *)pri, pub);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("pub time: %.6f (%.2f/s)\n", time / 500, 500 / time);

	start = clock();
	for (int32_t i = 0; i < 500; i++) {
		F_SYMBOL(ecdh_x448_shared_key)((uint8_t *)pri, pub, key);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("key time: %.6f (%.2f/s)\n", time / 500, 500 / time);

	printf("ed448 eddsa\n");

	start = clock();
	for (int32_t i = 0; i < 500; i++) {
		F_SYMBOL(eddsa_ed448_public_key)((uint8_t *)pri, pub);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("pub time: %.6f (%.2f/s)\n", time / 500, 500 / time);

	start = clock();
	for (int32_t i = 0; i < 500; i++) {
		F_SYMBOL(eddsa_ed448_sign)((uint8_t *)pri,
			sign, EDDSA_ED448_SIGN_LEN, sign);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("sign time: %.6f (%.2f/s)\n", time / 500, 500 / time);

	start = clock();
	for (int32_t i = 0; i < 500; i++) {
		F_SYMBOL(eddsa_ed448_verify)((uint8_t *)pub, sign,
			sign, EDDSA_ED448_SIGN_LEN);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("verify time: %.6f (%.2f/s)\n", time / 500, 500 / time);
}

void test_p256(void)
{
	clock_t start, end;
	double time;

	static const uint32_t pri[8] = {
		0x3017653f, 0x859a95e2, 0xf3f43fc6, 0xb9c8e0c4,
		0x5a577841, 0x43c64177, 0xc84563e1, 0xe529eb36
		};
	static const uint32_t ran[8] = {
		0xc51362ab, 0xfd1377c7, 0xefb87dd4, 0x2c9e6e78,
		0x88e9bd9d, 0x6aeb1a78, 0x2fe02e84, 0xbb0814c2
		};

	uint8_t sign[ECDSA_P256_SIGN_LEN];
	uint8_t pub[ECDSA_P256_PUB_LEN], key[ECDH_P256_KEY_LEN];

	printf("nist p-256 ecdh\n");

	start = clock();
	for (int32_t i = 0; i < 500; i++) {
		F_SYMBOL(ecdh_p256_public_key)((uint8_t *)pri, pub);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("pub time: %.6f (%.2f/s)\n", time / 500, 500 / time);

	start = clock();
	for (int32_t i = 0; i < 500; i++) {
		F_SYMBOL(ecdh_p256_shared_key)((uint8_t *)pri, pub, key);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("key time: %.6f (%.2f/s)\n", time / 500, 500 / time);

	printf("nist p-256 ecdsa\n");

	start = clock();
	for (int32_t i = 0; i < 500; i++) {
		F_SYMBOL(ecdsa_p256_public_key)((uint8_t *)pri, pub);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("pub time: %.6f (%.2f/s)\n", time / 500, 500 / time);

	start = clock();
	for (int32_t i = 0; i < 500; i++) {
		F_SYMBOL(ecdsa_p256_sign)((uint8_t *)pri, (uint8_t *)ran,
			sign, ECDSA_P256_SIGN_LEN, sign);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("sign time: %.6f (%.2f/s)\n", time / 500, 500 / time);

	start = clock();
	for (int32_t i = 0; i < 500; i++) {
		F_SYMBOL(ecdsa_p256_verify)((uint8_t *)pub, sign,
			sign, ECDSA_P256_SIGN_LEN);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("verify time: %.6f (%.2f/s)\n", time / 500, 500 / time);
}

void test_p384(void)
{
	clock_t start, end;
	double time;

	static const uint32_t pri[12] = {
		0x3017653f, 0x859a95e2, 0xf3f43fc6, 0xb9c8e0c4,
		0x5a577841, 0x43c64177, 0xc84563e1, 0xe529eb36,
		0, 0, 0, 0
		};
	static const uint32_t ran[12] = {
		0xc51362ab, 0xfd1377c7, 0xefb87dd4, 0x2c9e6e78,
		0x88e9bd9d, 0x6aeb1a78, 0x2fe02e84, 0xbb0814c2,
		0, 0, 0, 0
		};

	uint8_t sign[ECDSA_P384_SIGN_LEN];
	uint8_t pub[ECDSA_P384_PUB_LEN], key[ECDH_P384_KEY_LEN];

	printf("nist p-384 ecdh\n");

	start = clock();
	for (int32_t i = 0; i < 500; i++) {
		F_SYMBOL(ecdh_p384_public_key)((uint8_t *)pri, pub);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("pub time: %.6f (%.2f/s)\n", time / 500, 500 / time);

	start = clock();
	for (int32_t i = 0; i < 500; i++) {
		F_SYMBOL(ecdh_p384_shared_key)((uint8_t *)pri, pub, key);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("key time: %.6f (%.2f/s)\n", time / 500, 500 / time);

	printf("nist p-384 ecdsa\n");

	start = clock();
	for (int32_t i = 0; i < 500; i++) {
		F_SYMBOL(ecdsa_p384_public_key)((uint8_t *)pri, pub);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("pub time: %.6f (%.2f/s)\n", time / 500, 500 / time);

	start = clock();
	for (int32_t i = 0; i < 500; i++) {
		F_SYMBOL(ecdsa_p384_sign)((uint8_t *)pri, (uint8_t *)ran,
			sign, ECDSA_P384_SIGN_LEN, sign);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("sign time: %.6f (%.2f/s)\n", time / 500, 500 / time);

	start = clock();
	for (int32_t i = 0; i < 500; i++) {
		F_SYMBOL(ecdsa_p384_verify)((uint8_t *)pub, sign,
			sign, ECDSA_P384_SIGN_LEN);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("verify time: %.6f (%.2f/s)\n", time / 500, 500 / time);
}

void test_p521(void)
{
	clock_t start, end;
	double time;

	static const uint32_t pri[17] = {
		0x3017653f, 0x859a95e2, 0xf3f43fc6, 0xb9c8e0c4,
		0x5a577841, 0x43c64177, 0xc84563e1, 0xe529eb36,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		};
	static const uint32_t ran[17] = {
		0xc51362ab, 0xfd1377c7, 0xefb87dd4, 0x2c9e6e78,
		0x88e9bd9d, 0x6aeb1a78, 0x2fe02e84, 0xbb0814c2,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		};

	uint8_t sign[ECDSA_P521_SIGN_LEN];
	uint8_t pub[ECDSA_P521_PUB_LEN], key[ECDH_P521_KEY_LEN];

	printf("nist p-521 ecdh\n");

	start = clock();
	for (int32_t i = 0; i < 500; i++) {
		F_SYMBOL(ecdh_p521_public_key)((uint8_t *)pri, pub);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("pub time: %.6f (%.2f/s)\n", time / 500, 500 / time);

	start = clock();
	for (int32_t i = 0; i < 500; i++) {
		F_SYMBOL(ecdh_p521_shared_key)((uint8_t *)pri, pub, key);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("key time: %.6f (%.2f/s)\n", time / 500, 500 / time);

	printf("nist p-521 ecdsa\n");

	start = clock();
	for (int32_t i = 0; i < 500; i++) {
		F_SYMBOL(ecdsa_p521_public_key)((uint8_t *)pri, pub);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("pub time: %.6f (%.2f/s)\n", time / 500, 500 / time);

	start = clock();
	for (int32_t i = 0; i < 500; i++) {
		F_SYMBOL(ecdsa_p521_sign)((uint8_t *)pri, (uint8_t *)ran,
			sign, ECDSA_P521_SIGN_LEN, sign);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("sign time: %.6f (%.2f/s)\n", time / 500, 500 / time);

	start = clock();
	for (int32_t i = 0; i < 500; i++) {
		F_SYMBOL(ecdsa_p521_verify)((uint8_t *)pub, sign,
			sign, ECDSA_P521_SIGN_LEN);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("verify time: %.6f (%.2f/s)\n", time / 500, 500 / time);
}

int main(void)
{
	test_curve25519();
	test_curve448();
	test_p256();
	test_p384();
	test_p521();

	return 0;
}
