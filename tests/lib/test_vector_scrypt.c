/* @file: test_vector_scrypt.c
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
#include <demoz/c/stdint.h>
#include <demoz/c/string.h>
#include <demoz/lib/scrypt.h>


void print_hex(const uint8_t *buf, uint32_t n)
{
	printf(" ");
	for (uint32_t i = 0; i < n; i++)
		printf("%02x", buf[i]);
	printf("\n");

	for (uint32_t i = 0; i < n; i++) {
		printf(" 0x%02x", buf[i]);
		if ((i + 1) != n)
			printf(",");
		if (!((i + 1) % 8))
			printf("\n");
	}
	if (n % 8)
		printf("\n");
}

const char *verify_cmp(const uint8_t *a, const uint8_t *b, uint32_t len)
{
	return C_SYMBOL(memcmp)(a, b, len) ? "No" : "Yes";
}

struct test_vector {
	uint8_t in_pass[128];
	uint32_t in_pass_len;
	uint8_t in_salt[128];
	uint32_t in_salt_len;
	uint32_t in_n;
	uint32_t in_r;
	uint32_t in_p;
	uint8_t out_dk[128];
	uint32_t out_dk_len;
};

void print_vector(struct test_vector *v)
{
	printf("vector in_pass: %u\n ", v->in_pass_len);
	print_hex(v->in_pass, v->in_pass_len);
	printf("vector in_salt: %u\n ", v->in_salt_len);
	print_hex(v->in_salt, v->in_salt_len);
	printf("vector in_n: %u\n ", v->in_n);
	printf("vector in_r: %u\n ", v->in_r);
	printf("vector in_p: %u\n ", v->in_p);
	printf("vector out_dk: %u\n ", v->out_dk_len);
	print_hex(v->out_dk, v->out_dk_len);
}

struct test_vector test_scrypt_vector_1 = {
	{
		0
	}, 0,
	{
		0
	}, 0,
	16, 1, 1,
	{
		0x77, 0xd6, 0x57, 0x62, 0x38, 0x65, 0x7b, 0x20,
		0x3b, 0x19, 0xca, 0x42, 0xc1, 0x8a, 0x04, 0x97,
		0xf1, 0x6b, 0x48, 0x44, 0xe3, 0x07, 0x4a, 0xe8,
		0xdf, 0xdf, 0xfa, 0x3f, 0xed, 0xe2, 0x14, 0x42,
		0xfc, 0xd0, 0x06, 0x9d, 0xed, 0x09, 0x48, 0xf8,
		0x32, 0x6a, 0x75, 0x3a, 0x0f, 0xc8, 0x1f, 0x17,
		0xe8, 0xd3, 0xe0, 0xfb, 0x2e, 0x0d, 0x36, 0x28,
		0xcf, 0x35, 0xe2, 0x0c, 0x38, 0xd1, 0x89, 0x06
	}, 64
	};

struct test_vector test_scrypt_vector_2 = {
	{
		0x70, 0x61, 0x73, 0x73, 0x77, 0x6f, 0x72, 0x64
	}, 8,
	{
		0x4e, 0x61, 0x43, 0x6c
	}, 4,
	1024, 8, 16,
	{
		0xfd, 0xba, 0xbe, 0x1c, 0x9d, 0x34, 0x72, 0x00,
		0x78, 0x56, 0xe7, 0x19, 0x0d, 0x01, 0xe9, 0xfe,
		0x7c, 0x6a, 0xd7, 0xcb, 0xc8, 0x23, 0x78, 0x30,
		0xe7, 0x73, 0x76, 0x63, 0x4b, 0x37, 0x31, 0x62,
		0x2e, 0xaf, 0x30, 0xd9, 0x2e, 0x22, 0xa3, 0x88,
		0x6f, 0xf1, 0x09, 0x27, 0x9d, 0x98, 0x30, 0xda,
		0xc7, 0x27, 0xaf, 0xb9, 0x4a, 0x83, 0xee, 0x6d,
		0x83, 0x60, 0xcb, 0xdf, 0xa2, 0xcc, 0x06, 0x40
	}, 64
	};

uint8_t g_tmp[SCRYPT_TMPSIZE(1024, 8, 16)];
uint8_t g_buf[4096];

void _test_scrypt(struct test_vector *t, int32_t n)
{
	printf("case %d vector:\n", n);
	print_vector(t);

	F_SYMBOL(scrypt)(t->in_pass, t->in_pass_len,
		t->in_salt, t->in_salt_len,
		g_tmp,
		t->in_n, t->in_r, t->in_p,
		g_buf, t->out_dk_len);

	printf("out_dk: %u -- %s\n ", t->out_dk_len,
		verify_cmp(g_buf, t->out_dk, t->out_dk_len));
	print_hex(g_buf, t->out_dk_len);

	printf("\n");
}

void test_scrypt(void)
{
	printf("==== Scrypt ====\n\n");

	_test_scrypt(&test_scrypt_vector_1, 1);
	_test_scrypt(&test_scrypt_vector_2, 2);
}

int main(void)
{
	test_scrypt();

	return 0;
}
