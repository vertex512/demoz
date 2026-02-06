/* @file: test_vector_ecdh_p256.c
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
#include <demoz/lib/ecc.h>


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

const char *verify_ieq(int32_t r)
{
	return r ? "No" : "Yes";
}

struct test_vector {
	uint8_t in_a_pri[128];
	uint32_t in_a_pri_len;
	uint8_t in_b_pri[128];
	uint32_t in_b_pri_len;
	uint8_t out_a_pub[128];
	uint32_t out_a_pub_len;
	uint8_t out_b_pub[128];
	uint32_t out_b_pub_len;
	uint8_t out_key[128];
	uint32_t out_key_len;
};

void print_vector(struct test_vector *v)
{
	printf("vector in_a_pri: %u\n ", v->in_a_pri_len);
	print_hex(v->in_a_pri, v->in_a_pri_len);
	printf("vector in_b_pri: %u\n ", v->in_b_pri_len);
	print_hex(v->in_b_pri, v->in_b_pri_len);
	printf("vector out_a_pub: %u\n ", v->out_a_pub_len);
	print_hex(v->out_a_pub, v->out_a_pub_len);
	printf("vector out_b_pub: %u\n ", v->out_b_pub_len);
	print_hex(v->out_b_pub, v->out_b_pub_len);
	printf("vector out_key: %u\n ", v->out_key_len);
	print_hex(v->out_key, v->out_key_len);
}

struct test_vector test_p256_vector_1 = {
	{
		0x33, 0x14, 0x2d, 0x86, 0x57, 0x9c, 0x2a, 0xc6,
		0x49, 0x40, 0xe8, 0xaf, 0xb8, 0xaa, 0xe9, 0x44,
		0xe5, 0x6d, 0x31, 0xa2, 0xda, 0x92, 0xa2, 0x70,
		0x3f, 0xac, 0xd9, 0x10, 0xf5, 0x01, 0x8f, 0xc8
	}, ECDH_P256_PRI_LEN,
	{
		0x53, 0xee, 0x6b, 0x47, 0x46, 0xab, 0x83, 0xb2,
		0xe0, 0x9b, 0xbf, 0x06, 0x8f, 0x5d, 0x68, 0x88,
		0x20, 0xce, 0x97, 0xb3, 0xac, 0x64, 0x11, 0x01,
		0x2a, 0x01, 0xae, 0x78, 0x5d, 0x9c, 0xef, 0xc6
	}, ECDH_P256_PRI_LEN,
	{
		0x80, 0x11, 0x58, 0x72, 0x37, 0x0c, 0x5d, 0x94,
		0xef, 0xb9, 0x90, 0xfc, 0x37, 0xe6, 0xdf, 0x98,
		0xd0, 0x87, 0x57, 0xca, 0xfe, 0xe1, 0x51, 0xb0,
		0xf9, 0x1c, 0x22, 0x94, 0x53, 0xb6, 0xd0, 0xda,
		0xb3, 0x0b, 0x99, 0xb8, 0x77, 0x05, 0x9e, 0x38,
		0x58, 0x5f, 0xcf, 0xac, 0x33, 0x5b, 0xf4, 0xb1,
		0x9a, 0xe5, 0xa3, 0x6f, 0x45, 0x1c, 0x1f, 0xd6,
		0x52, 0x82, 0xdb, 0x1c, 0x46, 0xa0, 0x71, 0x52
	}, ECDH_P256_PUB_LEN,
	{
		0x63, 0xbf, 0x94, 0x44, 0x55, 0xc7, 0x6f, 0x73,
		0x4c, 0xb7, 0xcc, 0x0b, 0x0a, 0x97, 0x96, 0x22,
		0x34, 0x8c, 0x39, 0x70, 0x02, 0xb7, 0x08, 0x12,
		0xf8, 0xd4, 0xc8, 0x89, 0x52, 0xfb, 0x2d, 0xd1,
		0xab, 0x72, 0x98, 0x03, 0x33, 0x4f, 0xe7, 0x53,
		0x83, 0x0f, 0xa3, 0xad, 0x46, 0xf0, 0x23, 0xac,
		0x6a, 0x8d, 0xc5, 0x13, 0x4c, 0x85, 0x57, 0x81,
		0x3e, 0xc2, 0x6c, 0x36, 0xca, 0xf3, 0xfb, 0x56
	}, ECDH_P256_PUB_LEN,
	{
		0xde, 0x42, 0x94, 0x4b, 0xd0, 0x64, 0x24, 0x81,
		0x03, 0xce, 0x7d, 0xce, 0x9e, 0x8e, 0xef, 0x2f,
		0x20, 0x65, 0x25, 0xe1, 0xe0, 0x16, 0x31, 0xd1,
		0xaf, 0xed, 0xf6, 0x42, 0x6b, 0x0f, 0x84, 0xd6
	}, ECDH_P256_KEY_LEN
	};

uint8_t g_buf[4096];
uint8_t g_a_pub[128];
uint8_t g_b_pub[128];

void _test_p256(struct test_vector *t, int32_t n)
{
	printf("case %d vector:\n", n);
	print_vector(t);

	F_SYMBOL(ecdh_p256_public_key)(t->in_a_pri, g_a_pub);
	F_SYMBOL(ecdh_p256_public_key)(t->in_b_pri, g_b_pub);

	printf("out_a_pub: %u -- %s\n ", t->out_a_pub_len,
		verify_cmp(g_a_pub, t->out_a_pub, t->out_a_pub_len));
	print_hex(g_a_pub, t->out_a_pub_len);

	printf("out_b_pub: %u -- %s\n ", t->out_b_pub_len,
		verify_cmp(g_b_pub, t->out_b_pub, t->out_b_pub_len));
	print_hex(g_b_pub, t->out_b_pub_len);

	F_SYMBOL(ecdh_p256_shared_key)(t->in_a_pri, g_b_pub, g_buf);

	printf("out_a_key: %u -- %s\n ", t->out_key_len,
		verify_cmp(g_buf, t->out_key, t->out_key_len));
	print_hex(g_buf, t->out_key_len);

	F_SYMBOL(ecdh_p256_shared_key)(t->in_b_pri, g_a_pub, g_buf);

	printf("out_b_key: %u -- %s\n ", t->out_key_len,
		verify_cmp(g_buf, t->out_key, t->out_key_len));
	print_hex(g_buf, t->out_key_len);

	printf("\n");
}

void test_p256(void)
{
	printf("==== ECDH P-256 ====\n\n");

	_test_p256(&test_p256_vector_1, 1);
}

int main(void)
{
	test_p256();

	return 0;
}
