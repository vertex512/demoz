/* @file: signal_old.c
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
#include <demoz/c/sys/signal.h>


/* @func: signal
 * #desc:
 *    set signal callback.
 *
 * #1: signal number
 * #2: callback handler
 * #r: old handler (X_SIG_ERR: errno)
 */
void (*C_SYMBOL(signal)(int32_t sig, void (*handler)(int32_t)))(int32_t)
{
	struct x_sigaction act, old;
	act.u._sa_handler = handler;
	act.sa_flags = X_SA_RESTART;
	C_SYMBOL(sigemptyset)(&act.sa_mask);

	if (C_SYMBOL(sigaction)(sig, &act, &old))
		return X_SIG_ERR;

	return old.u._sa_handler;
}
