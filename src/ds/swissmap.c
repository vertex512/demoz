/* @file: swissmap.c
 * #desc:
 *    The implementations of swiss high-performance hash table.
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
#include <demoz/ds/swissmap.h>


/* @func: swissmap_empty
 * #desc:
 *    swissmap ctrl group set to empty.
 *
 * #1: swissmap head
 */
void F_SYMBOL(swissmap_empty)(struct swissmap_head *head)
{
	C_SYMBOL(memset)(head->group, SWISSMAP_EMPTY,
		sizeof(union swissmap_group)
		* SWISSMAP_CLIGN(head->total_size));
}

/* @func: swissmap_insert
 * #desc:
 *    swissmap insert function.
 *
 * #1: swissmap head
 * #2: input key
 * #3: key length
 * #r: bucket / NULL pointer
 */
void *F_SYMBOL(swissmap_insert)(struct swissmap_head *head,
		const void *key, size_t len)
{
	uint64_t h = head->call_hash(key, len);
	uint64_t h1 = h >> 7;
	uint8_t h2 = h & 0x7f;

	uint32_t _mask = h2 * 0x01010101;
	uint8_t *_ctrl;

	size_t n = head->total_size;
	size_t m = h1 % n;
	for (size_t i = 0; i < n; i += 4) {
		size_t pos = ((m + i) % n) & ~0x3;
		uint32_t x = head->group[pos >> 2].ctrl;

		size_t j = 0;
		if (x & 0x80808080) {
			_ctrl = head->group[pos >> 2]._ctrl;
			for (; !(_ctrl[j] & 0x80) && j < 4; j++);
			_ctrl[j] = h2;
			head->size++;
			return SWISSMAP_ARRAY(head, pos + j);
		}

		x ^= _mask;
		x = (x - 0x01010101) & ~x & 0x80808080;
		if (!x)
			continue;

		_ctrl = head->group[pos >> 2]._ctrl;
		for (; j < 4; j++) {
			void *p = SWISSMAP_ARRAY(head, pos + j);
			if (_ctrl[j] == h2 && !head->call_cmp(p, key, len))
				return p;
		}
	}

	return NULL;
}

/* @func: swissmap_find
 * #desc:
 *    swissmap find function.
 *
 * #1: swissmap head
 * #2: input key
 * #3: key length
 * #r: bucket / NULL pointer
 */
void *F_SYMBOL(swissmap_find)(struct swissmap_head *head,
		const void *key, size_t len)
{
	uint64_t h = head->call_hash(key, len);
	uint64_t h1 = h >> 7;
	uint8_t h2 = h & 0x7f;

	uint32_t _mask = h2 * 0x01010101;
	uint8_t *_ctrl;

	size_t n = head->total_size;
	size_t m = h1 % n;
	for (size_t i = 0; i < n; i += 4) {
		size_t pos = ((m + i) % n) & ~0x3;
		uint32_t x = head->group[pos >> 2].ctrl;

		x ^= _mask;
		x = (x - 0x01010101) & ~x & 0x80808080;
		if (!x)
			continue;

		_ctrl = head->group[pos >> 2]._ctrl;
		for (size_t j = 0; j < 4; j++) {
			void *p = SWISSMAP_ARRAY(head, pos + j);
			if (_ctrl[j] == h2 && !head->call_cmp(p, key, len))
				return p;
		}
	}

	return NULL;
}

/* @func: swissmap_delete
 * #desc:
 *    swissmap delete function.
 *
 * #1: swissmap head
 * #2: input key
 * #3: key length
 * #r: bucket / NULL pointer
 */
void *F_SYMBOL(swissmap_delete)(struct swissmap_head *head,
		const void *key, size_t len)
{
	uint64_t h = head->call_hash(key, len);
	uint64_t h1 = h >> 7;
	uint8_t h2 = h & 0x7f;

	uint32_t _mask = h2 * 0x01010101;
	uint8_t *_ctrl;

	size_t n = head->total_size;
	size_t m = h1 % n;
	for (size_t i = 0; i < n; i += 4) {
		size_t pos = ((m + i) % n) & ~0x3;
		uint32_t x = head->group[pos >> 2].ctrl;

		x ^= _mask;
		x = (x - 0x01010101) & ~x & 0x80808080;
		if (!x)
			continue;

		_ctrl = head->group[pos >> 2]._ctrl;
		for (size_t j = 0; j < 4; j++) {
			void *p = SWISSMAP_ARRAY(head, pos + j);
			if (_ctrl[j] == h2 && !head->call_cmp(p, key, len)) {
				_ctrl[j] = SWISSMAP_DELETE;
				head->size--;
				return p;
			}
		}
	}

	return NULL;
}
