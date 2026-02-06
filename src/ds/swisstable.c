/* @file: swisstable.c
 * #desc:
 *    The implementations of swiss hash table.
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
#include <demoz/ds/swisstable.h>


/* @func: swisstable_insert
 * #desc:
 *    swisstable insert function.
 *
 * #1: swisstable head
 * #2: input key
 * #3: key length
 * #r: bucket / NULL pointer
 */
void *F_SYMBOL(swisstable_insert)(struct swisstable_head *head,
		const void *key, size_t len)
{
	uint64_t h = head->call_hash(key, len);
	uint64_t h1 = h >> 7;
	uint8_t h2 = h & 0x7f;

	size_t n = head->total_size;
	size_t m = h1 % n;
	for (size_t i = 0; i < n; i++) {
		size_t pos = (m + i) % n;

		void *p = (void *)((char *)head->array + (head->wsize * pos));
		if (head->group[pos].ctrl == SWISSTABLE_EMPTY
				|| head->group[pos].ctrl == SWISSTABLE_DELETE) {
			head->group[pos].ctrl = h2;
			head->size++;
			return p;
		}
		if (head->group[pos].ctrl == h2
				&& !head->call_cmp(p, key, len))
			return p;
	}

	return NULL;
}

/* @func: swisstable_find
 * #desc:
 *    swisstable find function.
 *
 * #1: swisstable head
 * #2: input key
 * #3: key length
 * #r: bucket / NULL pointer
 */
void *F_SYMBOL(swisstable_find)(struct swisstable_head *head,
		const void *key, size_t len)
{
	uint64_t h = head->call_hash(key, len);
	uint64_t h1 = h >> 7;
	uint8_t h2 = h & 0x7f;

	size_t n = head->total_size;
	size_t m = h1 % n;
	for (size_t i = 0; i < n; i++) {
		size_t pos = (m + i) % n;
		if (head->group[pos].ctrl != h2)
			continue;
		if (head->group[pos].ctrl == SWISSTABLE_EMPTY)
			break;

		void *p = (void *)((char *)head->array + (head->wsize * pos));
		if (!head->call_cmp(p, key, len))
			return p;
	}

	return NULL;
}

/* @func: swisstable_delete
 * #desc:
 *    swisstable delete function.
 *
 * #1: swisstable head
 * #2: input key
 * #3: key length
 * #r: bucket / NULL pointer
 */
void *F_SYMBOL(swisstable_delete)(struct swisstable_head *head,
		const void *key, size_t len)
{
	uint64_t h = head->call_hash(key, len);
	uint64_t h1 = h >> 7;
	uint8_t h2 = h & 0x7f;

	size_t n = head->total_size;
	size_t m = h1 % n;
	for (size_t i = 0; i < n; i++) {
		size_t pos = (m + i) % n;
		if (head->group[pos].ctrl != h2)
			continue;
		if (head->group[pos].ctrl == SWISSTABLE_EMPTY)
			break;

		void *p = (void *)((char *)head->array + (head->wsize * pos));
		if (!head->call_cmp(p, key, len)) {
			head->group[pos].ctrl = SWISSTABLE_DELETE;
			head->size--;
			return p;
		}
	}

	return NULL;
}
