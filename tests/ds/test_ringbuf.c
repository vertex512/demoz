/* @file: test_ringbuf.c
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
#include <demoz/ds/ringbuf.h>


void test_ringbuf(void)
{
	uint8_t buf[4096], out[4096];
	uint32_t r;
	RINGBUF_NEW(head, buf, 4096);

	r = F_SYMBOL(ringbuf_write)(&head, (uint8_t *)"Hello, world!", 13);
	printf("write: %d:%u\n", 13, r);

	r = F_SYMBOL(ringbuf_peek)(&head, out, 4096, 2);
	printf("peek: %d-%d:%u\n", 4096, 2, r);
	out[r] = '\0';
	printf(": %s\n", out);

	r = F_SYMBOL(ringbuf_peek)(&head, out, 10, 4096);
	printf("peek: %d-%d:%u\n", 10, 4096, r);
	out[r] = '\0';
	printf(": %s\n", out);

	r = F_SYMBOL(ringbuf_read)(&head, out, 13 - 2);
	printf("read: %d:%u\n", 13 - 2, r);
	out[r] = '\0';
	printf(": %s\n", out);

	r = F_SYMBOL(ringbuf_read)(&head, out, 4096);
	printf("read: %d:%u\n", 4096, r);
	out[r] = '\0';
	printf(": %s\n", out);
}

int main(void)
{
	test_ringbuf();

	return 0;
}
