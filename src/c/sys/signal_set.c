/* @file: signal_set.c
 * #desc:
 *    The implementations of signal functions.
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
#include <demoz/c/errno.h>
#include <demoz/c/sys/signal.h>
#include <demoz/c/sys/syscall.h>


/* @func: sigaddset
 * #desc:
 *    added a signal to signal set.
 *
 * #1: signal set
 * #2: signal number
 * #r: 0: no error, -1: errno
 */
int32_t C_SYMBOL(sigaddset)(x_sigset_t *set, int32_t sig)
{
	if (sig <= X_SIGZER || sig > X_SIGMAX) {
		/* errno */
		x_errno = X_EINVAL;
		return -1;
	}

	sig--;
	((uint8_t *)set)[sig / 8] |= 1U << (sig % 8);

	return 0;
}

/* @func: sigdelset
 * #desc:
 *    delete a signal from a signal set.
 *
 * #1: signal set
 * #2: signal number
 * #r: 0: no error, -1: errno
 */
int32_t C_SYMBOL(sigdelset)(x_sigset_t *set, int32_t sig)
{
	if (sig <= X_SIGZER || sig > X_SIGMAX) {
		/* errno */
		x_errno = X_EINVAL;
		return -1;
	}

	sig--;
	((uint8_t *)set)[sig / 8] &= ~(1U << (sig % 8));

	return 0;
}

/* @func: sigismember
 * #desc:
 *    check the signal in the signal set.
 *
 * #1: signal set
 * #2: signal number
 * #r: 0: no error, -1: errno
 */
int32_t C_SYMBOL(sigismember)(const x_sigset_t *set, int32_t sig)
{
	if (sig <= X_SIGZER || sig > X_SIGMAX) {
		/* errno */
		x_errno = X_EINVAL;
		return -1;
	}

	sig--;
	if (((uint8_t *)set)[sig / 8] & (1U << (sig % 8)))
		return 1;

	return 0;
}

/* @func: sigemptyset
 * #desc:
 *    init signal set.
 *
 * #1: signal set
 * #r: 0: no error
 */
int32_t C_SYMBOL(sigemptyset)(x_sigset_t *set)
{
	C_SYMBOL(memset)(set, 0, sizeof(x_sigset_t));

	return 0;
}

/* @func: sigfillset
 * #desc:
 *    filling signal set.
 *
 * #1: signal set
 * #r: 0: no error, -1: errno
 */
int32_t C_SYMBOL(sigfillset)(x_sigset_t *set)
{
	C_SYMBOL(memset)(set, 0xff, sizeof(x_sigset_t));

	if (C_SYMBOL(sigdelset)(set, X_SIGHUP))
		return -1;
	if (C_SYMBOL(sigdelset)(set, X_SIGINT))
		return -1;

	return 0;
}
