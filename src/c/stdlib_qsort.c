/* @file: stdlib_qsort.c
 * #desc:
 *    The implementations of standard library.
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
#include <demoz/c/string.h>
#include <demoz/c/stdlib.h>


/* @func: _qsort_swap (static)
 * #desc:
 *    memory swap.
 *
 * #1: a [in/out] member pointer
 * #2: b [in/out] member pointer
 * #3: n [in]     memory swap size
 */
static void _qsort_swap(void *a, void *b, uint64_t n)
{
	char buf[32];
	for (; n > 32; n -= 32) {
		C_SYMBOL(memcpy)(buf, a, 32);
		C_SYMBOL(memcpy)(a, b, 32);
		C_SYMBOL(memcpy)(b, buf, 32);
		a = (char *)a + 32;
		b = (char *)b + 32;
	}

	if (n > 0) {
		C_SYMBOL(memcpy)(buf, a, n);
		C_SYMBOL(memcpy)(a, b, n);
		C_SYMBOL(memcpy)(b, buf, n);
	}
}

/* @func: _qsort (static)
 * #desc:
 *    heapsort siftdown.
 *
 * #1: b   [in/out] base array
 * #2: n   [in]     number of array members
 * #3: w   [in]     members size
 * #4: cmp [in]     callback cmp
 * #5: k   [in]     heapsort binary size
 */
static void _qsort(void *b, uint64_t n, uint64_t w,
		int32_t (*cmp)(const void *, const void *), uint64_t k)
{
	for (uint64_t j = 0; ((k * 2) + 1) <= n; ) {
		j = (2 * k) + 1;
		if (j < n && cmp((char *)b + (j * w),
				(char *)b + ((j + 1) * w)) < 0)
			j++;
		if (j == k || cmp((char *)b + (k * w),
				(char *)b + (j * w)) >= 0)
			break;
		_qsort_swap((char *)b + (w * j), (char *)b + (k * w), w);
		k = j;
	}
}

/* @func: qsort
 * #desc:
 *    sort a table of data (heapsort).
 *
 * #1: b   [in/out] base array
 * #2: n   [in]     number of array members
 * #3: w   [in]     members size
 * #4: cmp [in]     callback cmp
 */
void C_SYMBOL(qsort)(void *b, uint64_t n, uint64_t w,
		int32_t (*cmp)(const void *, const void *))
{
	if (n-- <= 1)
		return;

	/* build binary heap */
	for (int64_t k = n / 2; k >= 0; k--)
		_qsort(b, n, w, cmp, k);

	while (1) {
		_qsort_swap(b, (char *)b + (n * w), w);
		if (!--n)
			break;
		_qsort(b, n, w, cmp, 0);
	}
}

/* @func: bsearch
 * #desc:
 *    binary search a sorted table.
 *
 * #1: k   [in]  key value
 * #2: b   [in]  base sorted array
 * #3: n   [in]  number of array members
 * #4: w   [in]  members size
 * #5: cmp [in]  callback cmp
 * #r:     [ret] return key members / NULL pointer
 */
void *C_SYMBOL(bsearch)(const void *k, const void *b, uint64_t n, uint64_t w,
		int32_t (*cmp)(const void *, const void *))
{
	while (n > 0) {
		char *p = (char *)b + (w * (n / 2));
		int32_t m = cmp(k, p);
		if (m < 0) {
			n /= 2;
		} else if (m > 0) {
			b = p + w;
			n -= (n / 2) + 1;
		} else {
			return p;
		}
	}

	return NULL;
}
