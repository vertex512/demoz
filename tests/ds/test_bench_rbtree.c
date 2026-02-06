/* @file: test_bench_rbtree.c
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
#include <demoz/c/stdint.h>
#include <demoz/c/stdlib.h>
#include <demoz/ds/rbtree.h>


#define SIZE 1000000
#define A_SIZE 10000

struct T {
	int32_t key;
	struct rb_node node;
};

int32_t cmp(void *n, void *a)
{
	int32_t key_a = container_of(a, struct T, node)->key;
	int32_t key_b = container_of(n, struct T, node)->key;

	return (key_a > key_b) ? 1 : ((key_a < key_b) ? -1 : 0);
}

void test_rbtree(void)
{
	clock_t start, end;
	double time;
	RANDOM_TYPE0_NEW(ran, 123456);

	struct T *node = malloc(sizeof(struct T) * SIZE);
	struct rb_node *p;
	RB_ROOT_NEW(root);

	for (int32_t i = 0; i < SIZE; i++)
		C_SYMBOL(random_r)(&ran, &node[i].key);

	/* insert */
	start = clock();
	for (int32_t i = 0; i < SIZE; i++) {
		if (!F_SYMBOL(rb_wrap_insert)(&root, &node[i].node, cmp))
			printf("collision i:%d k:%d\n", i, node[i].key);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("insert: %d -- %.6fs (%.2f/s) %.2f ns/op\n",
		SIZE, time,
		(double)SIZE / time,
		(double)(time * 1000000000) / SIZE);

	/* first and last */
	p = F_SYMBOL(rb_wrap_first)(&root);
	printf("first: k:%d %p\n", container_of(p, struct T, node)->key, p);

	p = F_SYMBOL(rb_wrap_last)(&root);
	printf("last: k:%d %p\n", container_of(p, struct T, node)->key, p);

	/* search */
	start = clock();
	for (int32_t i = 0; i < SIZE; i++) {
		if (!F_SYMBOL(rb_wrap_search)(&root, &node[i].node, cmp))
			printf("not found: i:%d k:%d\n", i, node[i].key);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("search: %d -- %.6fs (%.2f/s) %.2f ns/op\n",
		SIZE, time,
		(double)SIZE / time,
		(double)(time * 1000000000) / SIZE);

	/* erase access */
	start = clock();
	for (int32_t i = 0; i < A_SIZE; i++) {
		if (!F_SYMBOL(rb_wrap_erase2)(&root, &node[i].node, cmp))
			printf("not found: i:%d k:%d\n", i, node[i].key);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("erase access: %d -- %.6fs (%.2f/s) %.2f ns/op\n",
		A_SIZE, time,
		(double)A_SIZE / time,
		(double)(time * 1000000000) / A_SIZE);

	/* erase */
	start = clock();
	for (int32_t i = A_SIZE; i < SIZE; i++) {
		if (!F_SYMBOL(rb_wrap_erase2)(&root, &node[i].node, cmp))
			printf("not found: i:%d k:%d\n", i, node[i].key);
	}
	end = clock();
	time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("erase: %d -- %.6fs (%.2f/s) %.2f ns/op\n",
		SIZE - A_SIZE, time,
		(double)(SIZE - A_SIZE) / time,
		(double)(time * 1000000000) / (SIZE - A_SIZE));
}

int main(void)
{
	test_rbtree();

	return 0;
}
