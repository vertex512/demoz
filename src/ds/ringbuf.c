/* @file: ringbuf.c
 * #desc:
 *    The implementations of ring buffer.
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
#include <demoz/c/stdint.h>
#include <demoz/c/string.h>
#include <demoz/ds/ringbuf.h>


/* @func: ringbuf_write
 * #desc:
 *    write to the ring buffer.
 *
 * #1: ring buffer head
 * #2: input buffer
 * #3: input length
 * #r: length of write
 */
uint32_t F_SYMBOL(ringbuf_write)(struct ringbuf_head *head, const uint8_t *s,
		uint32_t len)
{
	uint32_t l, f;

	/* NOTE: unsigned overflow features */

	l = head->size - (head->write - head->read); /* avail length */
	f = head->write % head->size; /* first offset */

#undef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

	head->write += len = MIN(len, l);
	l = MIN(len, head->size - f); /* first length */

	C_SYMBOL(memcpy)(head->buf + f, s, l);
	C_SYMBOL(memcpy)(head->buf, s + l, len - l);

	return len;
}

/* @func: ringbuf_read
 * #desc:
 *    read from the ring buffer.
 *
 * #1: ring buffer head
 * #2: output buffer
 * #3: buffer length
 * #r: length of read
 */
uint32_t F_SYMBOL(ringbuf_read)(struct ringbuf_head *head, uint8_t *s,
		uint32_t len)
{
	uint32_t l, f;

	/* NOTE: unsigned overflow features */

	l = head->write - head->read; /* use length */
	f = head->read % head->size; /* first offset */

#undef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

	head->read += len = MIN(len, l);
	l = MIN(len, head->size - f); /* first length */

	C_SYMBOL(memcpy)(s, head->buf + f, l);
	C_SYMBOL(memcpy)(s + l, head->buf, len - l);

	return len;
}

/* @func: ringbuf_peek
 * #desc:
 *    peek only the ring buffer.
 *
 * #1: ring buffer head
 * #2: output buffer
 * #3: buffer length
 * #4: offset of read
 * #r: output length
 */
uint32_t F_SYMBOL(ringbuf_peek)(struct ringbuf_head *head, uint8_t *s,
		uint32_t len, uint32_t off)
{
	uint32_t l, f;

	/* NOTE: unsigned overflow features */

	l = head->write - head->read; /* use length */
	if (off > l)
		return 0;

	l -= off;
	f = (head->read + off) % head->size; /* first offset */

#undef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

	len = MIN(len, l);
	l = MIN(len, head->size - f); /* first length */

	C_SYMBOL(memcpy)(s, head->buf + f, l);
	C_SYMBOL(memcpy)(s + l, head->buf, len - l);

	return len;
}
