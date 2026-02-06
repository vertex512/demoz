/* @file: stdlib_exit.c
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
#include <demoz/c/atomic.h>
#include <demoz/c/sys/stdlib.h>


/* @def: _ */
#define ATEXIT_MAX 32

struct atexit_head {
	void (*func[ATEXIT_MAX])(void);
	struct atexit_head *next;
};

static struct atexit_head _at_head, *_at_curr = NULL;
static int32_t _at_count = 0;
static volatile int32_t _lock = 0;
/* end */

/* @func: atexit
 * #desc:
 *    register a exit callback function.
 *
 * #1: callback function
 */
int32_t C_SYMBOL(atexit)(void (*func)(void))
{
	int32_t lock = C_SYMBOL(atomic_cas)(&_lock, 0, 1);
	while (lock && C_SYMBOL(atomic_cas)(&_lock, 0, 1));

	if (!_at_count) {
		_at_curr = &_at_head;
	} else if (!(_at_count % ATEXIT_MAX)) {
		_at_curr->next = C_SYMBOL(malloc)(sizeof(struct atexit_head));
		_at_curr = _at_curr->next;
	}

	_at_curr->func[_at_count++ % ATEXIT_MAX] = func;

	while (!C_SYMBOL(atomic_cas)(&_lock, 1, 0));

	return 0;
}

/* @func: exit
 * #desc:
 *    terminate the calling process.
 *
 * #1: exit status
 */
void C_SYMBOL(exit)(int32_t status)
{
	int32_t lock = C_SYMBOL(atomic_cas)(&_lock, 0, 1);
	while (lock && C_SYMBOL(atomic_cas)(&_lock, 0, 1));

	struct atexit_head *p = &_at_head;
	for (int32_t i = 0; i < _at_count; i++) {
		(p->func[i % ATEXIT_MAX])();

		if (!((i + 1) % ATEXIT_MAX))
			p = p->next;
	}

	C_SYMBOL(_Exit)(status);

	while (!C_SYMBOL(atomic_cas)(&_lock, 1, 0));
}
