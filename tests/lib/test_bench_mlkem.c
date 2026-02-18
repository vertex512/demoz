/* @file: test_bench_mlkem.c
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
#include <demoz/lib/mlkem.h>


void test_mlkem512(void)
{
	clock_t start, end;
	double time;

	uint8_t ran[MLKEM_RAN_LEN], msg[MLKEM_SYM_LEN];
	uint8_t ek[MLKEM_512_EK_LEN], dk[MLKEM_512_DK_LEN],
		ct[MLKEM_512_CT_LEN];
	uint8_t sk[MLKEM_KEY_LEN];

	printf("mlkem-512\n");

	start = clock();
	for (int32_t i = 0; i < 1000; i++) {
		F_SYMBOL(mlkem512_genkey)(ran, msg, ek, dk);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("genkey time: %.6f (%.2f/s)\n", time / 1000, 1000 / time);

	start = clock();
	for (int32_t i = 0; i < 1000; i++) {
		F_SYMBOL(mlkem512_encaps)(msg, ek, ct, sk);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("encaps time: %.6f (%.2f/s)\n", time / 1000, 1000 / time);

	start = clock();
	for (int32_t i = 0; i < 1000; i++) {
		F_SYMBOL(mlkem512_decaps)(dk, ct, sk);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("decaps time: %.6f (%.2f/s)\n", time / 1000, 1000 / time);
}

void test_mlkem768(void)
{
	clock_t start, end;
	double time;

	uint8_t ran[MLKEM_RAN_LEN], msg[MLKEM_SYM_LEN];
	uint8_t ek[MLKEM_768_EK_LEN], dk[MLKEM_768_DK_LEN],
		ct[MLKEM_768_CT_LEN];
	uint8_t sk[MLKEM_KEY_LEN];

	printf("mlkem-768\n");

	start = clock();
	for (int32_t i = 0; i < 1000; i++) {
		F_SYMBOL(mlkem768_genkey)(ran, msg, ek, dk);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("genkey time: %.6f (%.2f/s)\n", time / 1000, 1000 / time);

	start = clock();
	for (int32_t i = 0; i < 1000; i++) {
		F_SYMBOL(mlkem768_encaps)(msg, ek, ct, sk);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("encaps time: %.6f (%.2f/s)\n", time / 1000, 1000 / time);

	start = clock();
	for (int32_t i = 0; i < 1000; i++) {
		F_SYMBOL(mlkem768_decaps)(dk, ct, sk);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("decaps time: %.6f (%.2f/s)\n", time / 1000, 1000 / time);
}

void test_mlkem1024(void)
{
	clock_t start, end;
	double time;

	uint8_t ran[MLKEM_RAN_LEN], msg[MLKEM_SYM_LEN];
	uint8_t ek[MLKEM_1024_EK_LEN], dk[MLKEM_1024_DK_LEN],
		ct[MLKEM_1024_CT_LEN];
	uint8_t sk[MLKEM_KEY_LEN];

	printf("mlkem-1024\n");

	start = clock();
	for (int32_t i = 0; i < 1000; i++) {
		F_SYMBOL(mlkem1024_genkey)(ran, msg, ek, dk);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("genkey time: %.6f (%.2f/s)\n", time / 1000, 1000 / time);

	start = clock();
	for (int32_t i = 0; i < 1000; i++) {
		F_SYMBOL(mlkem1024_encaps)(msg, ek, ct, sk);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("encaps time: %.6f (%.2f/s)\n", time / 1000, 1000 / time);

	start = clock();
	for (int32_t i = 0; i < 1000; i++) {
		F_SYMBOL(mlkem1024_decaps)(dk, ct, sk);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("decaps time: %.6f (%.2f/s)\n", time / 1000, 1000 / time);
}

int main(void)
{
	test_mlkem512();
	test_mlkem768();
	test_mlkem1024();

	return 0;
}
