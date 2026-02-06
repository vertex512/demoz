/* @file: test_bench_swisstable.c
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
#include <stdlib.h>
#include <time.h>
#include <demoz/c/stddef.h>
#include <demoz/c/stdint.h>
#include <demoz/c/stdlib.h>
#include <demoz/c/string.h>
#include <demoz/ds/swisstable.h>


#define TSIZE (10000000)
#define SIZE ((int32_t)(TSIZE * 0.95))
#define A_SIZE ((int32_t)(SIZE * 0.1))

struct T {
	int32_t key;
};

uint64_t hash(const void *a, size_t len)
{
	unsigned long hash = 5381;
	while (len--)
		hash = ((hash << 5) + hash) + ((char *)a)[len];

	return hash | (hash ^ 0x123456789) << 32;
}

int32_t cmp(void *a, const void *b, size_t len)
{
	int32_t key_a = ((struct T *)a)->key;
	int32_t key_b = *((int32_t *)b);
	(void)len;

	return !(key_a == key_b);
}
	
void test_swisstable(void)
{
	clock_t start, end;
	double time;
	RANDOM_TYPE0_NEW(ran, 123456);

	struct swisstable_group *ctrl = malloc(SWISSTABLE_CTRLSIZE(TSIZE));
	C_SYMBOL(memset)(ctrl, SWISSTABLE_EMPTY, SWISSTABLE_CTRLSIZE(TSIZE));
	struct T *array = malloc(sizeof(struct T) * TSIZE);
	struct T *p;
	SWISSTABLE_NEW(head, ctrl, array, sizeof(struct T), TSIZE, hash, cmp);

	int32_t *array_key = malloc(sizeof(int32_t) * SIZE);
	for (int32_t i = 0; i < SIZE; i++)
		C_SYMBOL(random_r)(&ran, &array_key[i]);

	/* insert */
	start = clock();
	for (int32_t i = 0; i < SIZE; i++) {
		p = F_SYMBOL(swisstable_insert)(&head,
			&array_key[i], sizeof(int32_t));
		if (!p) {
			printf("no speac: i:%d\n", i);
		} else {
			p->key = array_key[i];
		}
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("insert: %d (%.1f) -- %.6fs (%.2f/s) %.2f ns/op\n",
		SIZE, (double)SWISSTABLE_FACTOR(&head) / 10,
		time,
		(double)SIZE / time,
		(double)(time * 1000000000) / SIZE);

	/* find */
	start = clock();
	for (int32_t i = 0; i < SIZE; i++) {
		p = F_SYMBOL(swisstable_find)(&head,
				&array_key[i], sizeof(int32_t));
		if (!p) {
			printf("not found: i:%d\n", i);
		} else if (p->key != array_key[i]) {
			printf("key error: i:%d k:%d -- k:%d\n",
				i, array_key[i], p->key);
		}
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("find: %d (%.1f) -- %.6fs (%.2f/s) %.2f ns/op\n",
		SIZE, (double)SWISSTABLE_FACTOR(&head) / 10,
		time,
		(double)SIZE / time,
		(double)(time * 1000000000) / SIZE);

	/* access */
	start = clock();
	for (int32_t i = 0; i < A_SIZE; i++) {
		p = F_SYMBOL(swisstable_find)(&head,
				&array_key[i], sizeof(int32_t));
		if (!p) {
			printf("not found: i:%d\n", i);
		} else if (p->key != array_key[i]) {
			printf("key error: i:%d k:%d -- k:%d\n",
				i, array_key[i], p->key);
		}
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("access: %d (%.1f) -- %.6fs (%.2f/s) %.2f ns/op\n",
		A_SIZE, (double)SWISSTABLE_FACTOR(&head) / 10,
		time,
		(double)A_SIZE / time,
		(double)(time * 1000000000) / A_SIZE);

	/* delete access */
	start = clock();
	for (int32_t i = 0; i < A_SIZE; i++) {
		p = F_SYMBOL(swisstable_delete)(&head,
				&array_key[i], sizeof(int32_t));
		if (!p) {
			printf("not found: i:%d\n", i);
		} else if (p->key != array_key[i]) {
			printf("key error: i:%d k:%d -- k:%d\n",
				i, array_key[i], p->key);
		}
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("delete access: %d (%.1f) -- %.6fs (%.2f/s) %.2f ns/op\n",
		A_SIZE, (double)SWISSTABLE_FACTOR(&head) / 10,
		time,
		(double)A_SIZE / time,
		(double)(time * 1000000000) / A_SIZE);

	/* delete */
	start = clock();
	for (int32_t i = A_SIZE; i < SIZE; i++) {
		p = F_SYMBOL(swisstable_delete)(&head,
				&array_key[i], sizeof(int32_t));
		if (!p) {
			printf("not found: i:%d\n", i);
		} else if (p->key != array_key[i]) {
			printf("key error: i:%d k:%d -- k:%d\n",
				i, array_key[i], p->key);
		}
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("delete: %d (%.1f) -- %.6fs (%.2f/s) %.2f ns/op\n",
		SIZE - A_SIZE, (double)SWISSTABLE_FACTOR(&head) / 10,
		time,
		(double)(SIZE - A_SIZE) / time,
		(double)(time * 1000000000) / (SIZE - A_SIZE));
}

int main(void)
{
	test_swisstable();

	return 0;
}
