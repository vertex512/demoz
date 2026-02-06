/* @file: signal_diff.c
 * #desc:
 *    The implementations of signal difference conversion.
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
#include <demoz/c/sys/signal.h>
#include <demoz/c/sys/syscall.h>


/* @func: ___signal_sig
 * #desc:
 *    signal difference conversion.
 *
 * #1: signal number
 * #r: signal number
 */
int32_t C_SYMBOL(___signal_sig)(int32_t sig)
{
#if (DEMOZ_PLATFORM == DEMOZ_PLATFORM_LINUX)

	switch (sig) {
		case X_SIGHUP: sig = SIGHUP; break;
		case X_SIGINT: sig = SIGINT; break;
		case X_SIGQUIT: sig = SIGQUIT; break;
		case X_SIGILL: sig = SIGILL; break;
		case X_SIGTRAP: sig = SIGTRAP; break;
		case X_SIGABRT: sig = SIGABRT; break;
		case X_SIGBUS: sig = SIGBUS; break;
		case X_SIGFPE: sig = SIGFPE; break;
		case X_SIGKILL: sig = SIGKILL; break;
		case X_SIGUSR1: sig = SIGUSR1; break;
		case X_SIGSEGV: sig = SIGSEGV; break;
		case X_SIGUSR2: sig = SIGUSR2; break;
		case X_SIGPIPE: sig = SIGPIPE; break;
		case X_SIGALRM: sig = SIGALRM; break;
		case X_SIGTERM: sig = SIGTERM; break;
		case X_SIGSTKFLT: sig = SIGSTKFLT; break;
		case X_SIGCHLD: sig = SIGCHLD; break;
		case X_SIGCONT: sig = SIGCONT; break;
		case X_SIGSTOP: sig = SIGSTOP; break;
		case X_SIGTSTP: sig = SIGTSTP; break;
		case X_SIGTTIN: sig = SIGTTIN; break;
		case X_SIGTTOU: sig = SIGTTOU; break;
		case X_SIGURG: sig = SIGURG; break;
		case X_SIGXCPU: sig = SIGXCPU; break;
		case X_SIGXFSZ: sig = SIGXFSZ; break;
		case X_SIGVTALRM: sig = SIGVTALRM; break;
		case X_SIGPROF: sig = SIGPROF; break;
		case X_SIGWINCH: sig = SIGWINCH; break;
		case X_SIGPOLL: sig = SIGPOLL; break;
		case X_SIGPWR: sig = SIGPWR; break;
		case X_SIGSYS: sig = SIGSYS; break;
		default:
			break;
	}

	return sig;

#else
#	error "!!!unknown DEMOZ_PLATFORM!!!"
#endif
}

/* @func: ___signal_sig_inv
 * #desc:
 *    signal difference inverse conversion.
 *
 * #1: signal number
 * #r: signal number
 */
int32_t C_SYMBOL(___signal_sig_inv)(int32_t sig)
{
#if (DEMOZ_PLATFORM == DEMOZ_PLATFORM_LINUX)

	switch (sig) {
		case SIGHUP: sig = X_SIGHUP; break;
		case SIGINT: sig = X_SIGINT; break;
		case SIGQUIT: sig = X_SIGQUIT; break;
		case SIGILL: sig = X_SIGILL; break;
		case SIGTRAP: sig = X_SIGTRAP; break;
		case SIGABRT: sig = X_SIGABRT; break;
		case SIGBUS: sig = X_SIGBUS; break;
		case SIGFPE: sig = X_SIGFPE; break;
		case SIGKILL: sig = X_SIGKILL; break;
		case SIGUSR1: sig = X_SIGUSR1; break;
		case SIGSEGV: sig = X_SIGSEGV; break;
		case SIGUSR2: sig = X_SIGUSR2; break;
		case SIGPIPE: sig = X_SIGPIPE; break;
		case SIGALRM: sig = X_SIGALRM; break;
		case SIGTERM: sig = X_SIGTERM; break;
		case SIGSTKFLT: sig = X_SIGSTKFLT; break;
		case SIGCHLD: sig = X_SIGCHLD; break;
		case SIGCONT: sig = X_SIGCONT; break;
		case SIGSTOP: sig = X_SIGSTOP; break;
		case SIGTSTP: sig = X_SIGTSTP; break;
		case SIGTTIN: sig = X_SIGTTIN; break;
		case SIGTTOU: sig = X_SIGTTOU; break;
		case SIGURG: sig = X_SIGURG; break;
		case SIGXCPU: sig = X_SIGXCPU; break;
		case SIGXFSZ: sig = X_SIGXFSZ; break;
		case SIGVTALRM: sig = X_SIGVTALRM; break;
		case SIGPROF: sig = X_SIGPROF; break;
		case SIGWINCH: sig = X_SIGWINCH; break;
		case SIGPOLL: sig = X_SIGPOLL; break;
		case SIGPWR: sig = X_SIGPWR; break;
		case SIGSYS: sig = X_SIGSYS; break;
		default:
			break;
	}

	return sig;

#else
#	error "!!!unknown DEMOZ_PLATFORM!!!"
#endif
}

/* @func: ___signal_set
 * #desc:
 *    signal set difference conversion.
 *
 * #1: signal set
 * #2: signal set
 * #r: 0: no error, -1: error
 */
int32_t C_SYMBOL(___signal_set)(const x_sigset_t *a, void *b)
{
#if (DEMOZ_PLATFORM == DEMOZ_PLATFORM_LINUX)

	C_SYMBOL(memset)(b, 0, sizeof(sigset_t));

	for (uint32_t i = 0; i < sizeof(sigset_t); i++) {
		for (int32_t j = 0; j < 8; j++) {
			int32_t k = (((uint8_t *)a)[i] >> j) & 1;
			if (!k)
				continue;

			int32_t l = C_SYMBOL(___signal_sig)(
				(i * 8) + j + 1);
			if (l <= X_SIGZER || l > X_SIGMAX)
				return -1;

			l--;
			((uint8_t *)b)[l / 8] |= 1U << (l % 8);
		}
	}

	return 0;

#else
#	error "!!!unknown DEMOZ_PLATFORM!!!"
#endif
}

/* @func: ___signal_set_inv
 * #desc:
 *    signal set difference inverse conversion.
 *
 * #1: signal set
 * #2: signal set
 * #r: 0: no error, -1: error
 */
int32_t C_SYMBOL(___signal_set_inv)(const void *a, x_sigset_t *b)
{
#if (DEMOZ_PLATFORM == DEMOZ_PLATFORM_LINUX)

	C_SYMBOL(memset)(b, 0, sizeof(x_sigset_t));

	for (uint32_t i = 0; i < sizeof(sigset_t); i++) {
		for (int32_t j = 0; j < 8; j++) {
			int32_t k = (((uint8_t *)a)[i] >> j) & 1;
			if (!k)
				continue;

			int32_t l = C_SYMBOL(___signal_sig_inv)(
				(i * 8) + j + 1);
			if (l <= X_SIGZER || l > X_SIGMAX)
				return -1;

			l--;
			((uint8_t *)b)[l / 8] |= 1U << (l % 8);
		}
	}

	return 0;

#else
#	error "!!!unknown DEMOZ_PLATFORM!!!"
#endif
}
