/* @file: test_vector_pbkdf_sha1.c
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
#include <demoz/lib/sha1.h>
#include <demoz/lib/pbkdf2.h>


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
	uint32_t in_iter;
	uint8_t out_odk[128];
	uint32_t out_odk_len;
};

void print_vector(struct test_vector *v)
{
	printf("vector in_pass: %u\n ", v->in_pass_len);
	print_hex(v->in_pass, v->in_pass_len);
	printf("vector in_salt: %u\n ", v->in_salt_len);
	print_hex(v->in_salt, v->in_salt_len);
	printf("vector in_iter: %u\n ", v->in_iter);
	printf("vector out_odk: %u\n ", v->out_odk_len);
	print_hex(v->out_odk, v->out_odk_len);
}

struct test_vector test_sha1_vector_1 = {
	{
		0x70, 0x61, 0x73, 0x73, 0x77, 0x6f, 0x72, 0x64
	}, 8,
	{
		0x73, 0x61, 0x6c, 0x74
	}, 4,
	1,
	{
		0x0c, 0x60, 0xc8, 0x0f, 0x96, 0x1f, 0x0e, 0x71,
		0xf3, 0xa9, 0xb5, 0x24, 0xaf, 0x60, 0x12, 0x06,
		0x2f, 0xe0, 0x37, 0xa6
	}, SHA1_LEN
	};

struct test_vector test_sha1_vector_2 = {
	{
		0x70, 0x61, 0x73, 0x73, 0x77, 0x6f, 0x72, 0x64
	}, 8,
	{
		0x73, 0x61, 0x6c, 0x74
	}, 4,
	2,
	{
		0xea, 0x6c, 0x01, 0x4d, 0xc7, 0x2d, 0x6f, 0x8c,
		0xcd, 0x1e, 0xd9, 0x2a, 0xce, 0x1d, 0x41, 0xf0,
		0xd8, 0xde, 0x89, 0x57
	}, SHA1_LEN
	};

struct test_vector test_sha1_vector_3 = {
	{
		0x70, 0x61, 0x73, 0x73, 0x77, 0x6f, 0x72, 0x64
	}, 8,
	{
		0x73, 0x61, 0x6c, 0x74
	}, 4,
	4096,
	{
		0x4b, 0x00, 0x79, 0x01, 0xb7, 0x65, 0x48, 0x9a,
		0xbe, 0xad, 0x49, 0xd9, 0x26, 0xf7, 0x21, 0xd0,
		0x65, 0xa4, 0x29, 0xc1
	}, SHA1_LEN
	};

struct test_vector test_sha1_vector_4 = {
	{
		0x70, 0x61, 0x73, 0x73, 0x77, 0x6F, 0x72, 0x64,
		0x50, 0x41, 0x53, 0x53, 0x57, 0x4F, 0x52, 0x44,
		0x70, 0x61, 0x73, 0x73, 0x77, 0x6F, 0x72, 0x64
	}, 24,
	{
		0x73, 0x61, 0x6C, 0x74, 0x53, 0x41, 0x4C, 0x54,
		0x73, 0x61, 0x6C, 0x74, 0x53, 0x41, 0x4C, 0x54,
		0x73, 0x61, 0x6C, 0x74, 0x53, 0x41, 0x4C, 0x54,
		0x73, 0x61, 0x6C, 0x74, 0x53, 0x41, 0x4C, 0x54,
		0x73, 0x61, 0x6C, 0x74
	}, 36,
	4096,
	{
		0x3d, 0x2e, 0xec, 0x4f, 0xe4, 0x1c, 0x84, 0x9b,
		0x80, 0xc8, 0xd8, 0x36, 0x62, 0xc0, 0xe4, 0x4a,
		0x8b, 0x29, 0x1a, 0x96, 0x4c, 0xf2, 0xf0, 0x70,
		0x38
	}, SHA1_LEN + 5
	};

uint8_t g_buf[4096];

void _test_pbkdf2_sha1(struct test_vector *t, int32_t n)
{
	printf("case %d vector:\n", n);
	print_vector(t);

	F_SYMBOL(pbkdf2_sha1)(
		t->in_pass, t->in_pass_len,
		t->in_salt, t->in_salt_len,
		g_buf, t->out_odk_len,
		t->in_iter);

	printf("out_odk: %u -- %s\n ", t->out_odk_len,
		verify_cmp(g_buf, t->out_odk, t->out_odk_len));
	print_hex(g_buf, t->out_odk_len);

	printf("\n");
}

void test_pbkdf2_sha1(void)
{
	printf("==== PBKDF2 SHA1 ====\n\n");

	_test_pbkdf2_sha1(&test_sha1_vector_1, 1);
	_test_pbkdf2_sha1(&test_sha1_vector_2, 2);
	_test_pbkdf2_sha1(&test_sha1_vector_3, 3);
	_test_pbkdf2_sha1(&test_sha1_vector_4, 4);
}

int main(void)
{
	test_pbkdf2_sha1();

	return 0;
}
