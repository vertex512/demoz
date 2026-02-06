/* @file: signal_new.c
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
#include <demoz/c/stddef.h>
#include <demoz/c/stdint.h>
#include <demoz/c/errno.h>
#include <demoz/c/sys/signal.h>
#include <demoz/c/sys/syscall.h>


/* @func: sigaction
 * #desc:
 *    examine and change a signal action.
 *
 * #1: signal number
 * #2: new action / NULL
 * #3: old action / NULL
 * #r: 0: no error, -1: errno
 */
int32_t C_SYMBOL(sigaction)(int32_t sig, const struct x_sigaction *act,
		struct x_sigaction *old)
{
#if (DEMOZ_PLATFORM == DEMOZ_PLATFORM_LINUX)

	int32_t ret = 0;
	struct sigaction _act, _old;

	if (sig <= X_SIGZER || sig > X_SIGMAX) {
		/* errno */
		x_errno = X_EINVAL;
		return -1;
	}

	sig = C_SYMBOL(___signal_sig)(sig);

	if (act) {
		_act.sa_handler = act->u._sa_handler;
		_act.sa_flags = 0;

		if (act->sa_flags & X_SA_NOCLDSTOP)
			_act.sa_flags |= SA_NOCLDSTOP;
		if (act->sa_flags & X_SA_NOCLDWAIT)
			_act.sa_flags |= SA_NOCLDWAIT;
		if (act->sa_flags & X_SA_SIGINFO)
			_act.sa_flags |= SA_SIGINFO;
		if (act->sa_flags & X_SA_ONSTACK)
			_act.sa_flags |= SA_ONSTACK;
		if (act->sa_flags & X_SA_RESTART)
			_act.sa_flags |= SA_RESTART;
		if (act->sa_flags & X_SA_NODEFER)
			_act.sa_flags |= SA_NODEFER;
		if (act->sa_flags & X_SA_RESETHAND)
			_act.sa_flags |= SA_RESETHAND;

		if (C_SYMBOL(___signal_set)(&act->sa_mask,
				&_act.sa_mask)) {
			/* errno */
			x_errno = X_EINVAL;
			return -1;
		}
	}

	ret = C_SYMBOL(syscall_linux)(
		XSYSCALL_rt_sigaction,
		sig,
		act ? &_act : NULL,
		old ? &_old : NULL,
		sizeof(sigset_t));

	if (ret) {
		/* errno */
		switch (ret) {
			case -EFAULT:
				x_errno = X_EFAULT;
				break;
			case -EINVAL:
				x_errno = X_EINVAL;
				break;
			default:
				x_errno = X_EUNKNOWN;
				break;
		}
		return -1;
	}

	if (old) {
		old->u._sa_handler = _old.sa_handler;
		old->sa_flags = 0;

		if (_old.sa_flags & SA_NOCLDSTOP)
			old->sa_flags |= X_SA_NOCLDSTOP;
		if (_old.sa_flags & SA_NOCLDWAIT)
			old->sa_flags |= X_SA_NOCLDWAIT;
		if (_old.sa_flags & SA_SIGINFO)
			old->sa_flags |= X_SA_SIGINFO;
		if (_old.sa_flags & SA_ONSTACK)
			old->sa_flags |= X_SA_ONSTACK;
		if (_old.sa_flags & SA_RESTART)
			old->sa_flags |= X_SA_RESTART;
		if (_old.sa_flags & SA_NODEFER)
			old->sa_flags |= X_SA_NODEFER;
		if (_old.sa_flags & SA_RESETHAND)
			old->sa_flags |= X_SA_RESETHAND;

		if (C_SYMBOL(___signal_set_inv)(&_old.sa_mask,
				&old->sa_mask)) {
			/* errno */
			x_errno = X_EINVAL;
			return -1;
		}
	}

	return ret;

#else
#	error "!!!unknown DEMOZ_PLATFORM!!!"
#endif
}

/* @func: sigprocmask
 * #desc:
 *    examine and change blocked signals.
 *
 * #1: behavior type
 * #2: new signal set / NULL
 * #3: old signal set / NULL
 * #r: 0: no error, -1: errno
 */
int32_t C_SYMBOL(sigprocmask)(int32_t how, const x_sigset_t *set,
		x_sigset_t *old)
{
#if (DEMOZ_PLATFORM == DEMOZ_PLATFORM_LINUX)

	int32_t ret = 0;
	sigset_t _set, _old;

	switch (how) {
		case X_SIG_BLOCK:
			how = SIG_BLOCK;
			break;
		case X_SIG_UNBLOCK:
			how = SIG_UNBLOCK;
			break;
		case X_SIG_SETMASK:
			how = SIG_SETMASK;
			break;
		default:
			/* errno */
			x_errno = X_EINVAL;
			return -1;
	}

	if (set) {
		if (C_SYMBOL(___signal_set)(set, &_set)) {
			/* errno */
			x_errno = X_EINVAL;
			return -1;
		}
	}

	ret = C_SYMBOL(syscall_linux)(
		XSYSCALL_rt_sigprocmask,
		how,
		set ? &_set : NULL,
		old ? &_old : NULL,
		sizeof(sigset_t));

	if (ret) {
		/* errno */
		switch (ret) {
			case -EFAULT:
				x_errno = X_EFAULT;
				break;
			case -EINVAL:
				x_errno = X_EINVAL;
				break;
			default:
				x_errno = X_EUNKNOWN;
				break;
		}
		return -1;
	}

	if (old) {
		if (C_SYMBOL(___signal_set_inv)(&_old, old)) {
			/* errno */
			x_errno = X_EINVAL;
			return -1;
		}
	}

	return ret;

#else
#	error "!!!unknown DEMOZ_PLATFORM!!!"
#endif
}

/* @func: sigpending
 * #desc:
 *    get the set of signals waiting.
 *
 * #1: signal set
 * #r: 0: no error, -1: errno
 */
int32_t C_SYMBOL(sigpending)(x_sigset_t *set)
{
#if (DEMOZ_PLATFORM == DEMOZ_PLATFORM_LINUX)

	int32_t ret = 0;
	sigset_t _set;

	ret = C_SYMBOL(syscall_linux)(
		XSYSCALL_rt_sigpending,
		&_set,
		sizeof(sigset_t));

	if (ret) {
		/* errno */
		switch (ret) {
			case -EFAULT:
				x_errno = X_EFAULT;
				break;
			case -EINVAL:
				x_errno = X_EINVAL;
				break;
			default:
				x_errno = X_EUNKNOWN;
				break;
		}
		return -1;
	}

	if (C_SYMBOL(___signal_set_inv)(&_set, set)) {
		/* errno */
		x_errno = X_EINVAL;
		return -1;
	}

	return ret;

#else
#	error "!!!unknown DEMOZ_PLATFORM!!!"
#endif
}

/* @func: sigsuspend
 * #desc:
 *    replace the signal set and wait for signal (atomic safe).
 *
 * #1: signal set
 * #r: 0: no error, -1: errno
 */
int32_t C_SYMBOL(sigsuspend)(const x_sigset_t *set)
{
#if (DEMOZ_PLATFORM == DEMOZ_PLATFORM_LINUX)

	int32_t ret = 0;
	sigset_t _set;

	if (C_SYMBOL(___signal_set)(set, &_set)) {
		/* errno */
		x_errno = X_EINVAL;
		return -1;
	}

	ret = C_SYMBOL(syscall_linux)(
		XSYSCALL_rt_sigsuspend,
		&_set,
		sizeof(sigset_t));

	if (ret) {
		/* errno */
		switch (ret) {
			case -EFAULT:
				x_errno = X_EFAULT;
				break;
			case -EINVAL:
				x_errno = X_EINVAL;
				break;
			default:
				x_errno = X_EUNKNOWN;
				break;
		}
		return -1;
	}

	return ret;

#else
#	error "!!!unknown DEMOZ_PLATFORM!!!"
#endif
}
