/* @file: errno.c
 * #desc:
 *    The implementations of system error numbers.
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
#include <demoz/c/errno.h>


/* @func: ___errno
 * #desc:
 *    internal function for get and set error number.
 *
 * #r: returns errno internal pointer
 */
int32_t *C_SYMBOL(___errno)(void)
{
	static int32_t a;

	/* TODO: not thread safe */

	return &a;
}

/* @func: strerror
 * #desc:
 *    error number to string.
 *
 * #1: error number
 * #r: returns error number string
 */
const char *C_SYMBOL(strerror)(int32_t err)
{
	switch (err) {
		case X_EPERM:
			return X_EPERM_STR;
		case X_ENOENT:
			return X_ENOENT_STR;
		case X_ESRCH:
			return X_ESRCH_STR;
		case X_EINTR:
			return X_EINTR_STR;
		case X_EIO:
			return X_EIO_STR;
		case X_ENXIO:
			return X_ENXIO_STR;
		case X_E2BIG:
			return X_E2BIG_STR;
		case X_ENOEXEC:
			return X_ENOEXEC_STR;
		case X_EBADF:
			return X_EBADF_STR;
		case X_ECHILD:
			return X_ECHILD_STR;
		case X_EAGAIN:
			return X_EAGAIN_STR;
		case X_ENOMEM:
			return X_ENOMEM_STR;
		case X_EACCES:
			return X_EACCES_STR;
		case X_EFAULT:
			return X_EFAULT_STR;
		case X_ENOTBLK:
			return X_ENOTBLK_STR;
		case X_EBUSY:
			return X_EBUSY_STR;
		case X_EEXIST:
			return X_EEXIST_STR;
		case X_EXDEV:
			return X_EXDEV_STR;
		case X_ENODEV:
			return X_ENODEV_STR;
		case X_ENOTDIR:
			return X_ENOTDIR_STR;
		case X_EISDIR:
			return X_EISDIR_STR;
		case X_EINVAL:
			return X_EINVAL_STR;
		case X_ENFILE:
			return X_ENFILE_STR;
		case X_EMFILE:
			return X_EMFILE_STR;
		case X_ENOTTY:
			return X_ENOTTY_STR;
		case X_ETXTBSY:
			return X_ETXTBSY_STR;
		case X_EFBIG:
			return X_EFBIG_STR;
		case X_ENOSPC:
			return X_ENOSPC_STR;
		case X_ESPIPE:
			return X_ESPIPE_STR;
		case X_EROFS:
			return X_EROFS_STR;
		case X_EMLINK:
			return X_EMLINK_STR;
		case X_EPIPE:
			return X_EPIPE_STR;
		case X_EDOM:
			return X_EDOM_STR;
		case X_ERANGE:
			return X_ERANGE_STR;
		case X_EDEADLK:
			return X_EDEADLK_STR;
		case X_ENAMETOOLONG:
			return X_ENAMETOOLONG_STR;
		case X_ENOLCK:
			return X_ENOLCK_STR;
		case X_ENOSYS:
			return X_ENOSYS_STR;
		case X_ENOTEMPTY:
			return X_ENOTEMPTY_STR;
		case X_ELOOP:
			return X_ELOOP_STR;
/*		case X_EWOULDBLOCK:
			return X_EWOULDBLOCK_STR; */
		case X_ENOMSG:
			return X_ENOMSG_STR;
		case X_EIDRM:
			return X_EIDRM_STR;
		case X_ECHRNG:
			return X_ECHRNG_STR;
		case X_EL2NSYNC:
			return X_EL2NSYNC_STR;
		case X_EL3HLT:
			return X_EL3HLT_STR;
		case X_EL3RST:
			return X_EL3RST_STR;
		case X_ELNRNG:
			return X_ELNRNG_STR;
		case X_EUNATCH:
			return X_EUNATCH_STR;
		case X_ENOCSI:
			return X_ENOCSI_STR;
		case X_EL2HLT:
			return X_EL2HLT_STR;
		case X_EBADE:
			return X_EBADE_STR;
		case X_EBADR:
			return X_EBADR_STR;
		case X_EXFULL:
			return X_EXFULL_STR;
		case X_ENOANO:
			return X_ENOANO_STR;
		case X_EBADRQC:
			return X_EBADRQC_STR;
		case X_EBADSLT:
			return X_EBADSLT_STR;
/*		case X_EDEADLOCK:
			return X_EDEADLOCK_STR; */
		case X_EBFONT:
			return X_EBFONT_STR;
		case X_ENOSTR:
			return X_ENOSTR_STR;
		case X_ENODATA:
			return X_ENODATA_STR;
		case X_ETIME:
			return X_ETIME_STR;
		case X_ENOSR:
			return X_ENOSR_STR;
		case X_ENONET:
			return X_ENONET_STR;
		case X_ENOPKG:
			return X_ENOPKG_STR;
		case X_EREMOTE:
			return X_EREMOTE_STR;
		case X_ENOLINK:
			return X_ENOLINK_STR;
		case X_EADV:
			return X_EADV_STR;
		case X_ESRMNT:
			return X_ESRMNT_STR;
		case X_ECOMM:
			return X_ECOMM_STR;
		case X_EPROTO:
			return X_EPROTO_STR;
		case X_EMULTIHOP:
			return X_EMULTIHOP_STR;
		case X_EDOTDOT:
			return X_EDOTDOT_STR;
		case X_EBADMSG:
			return X_EBADMSG_STR;
		case X_EOVERFLOW:
			return X_EOVERFLOW_STR;
		case X_ENOTUNIQ:
			return X_ENOTUNIQ_STR;
		case X_EBADFD:
			return X_EBADFD_STR;
		case X_EREMCHG:
			return X_EREMCHG_STR;
		case X_ELIBACC:
			return X_ELIBACC_STR;
		case X_ELIBBAD:
			return X_ELIBBAD_STR;
		case X_ELIBSCN:
			return X_ELIBSCN_STR;
		case X_ELIBMAX:
			return X_ELIBMAX_STR;
		case X_ELIBEXEC:
			return X_ELIBEXEC_STR;
		case X_EILSEQ:
			return X_EILSEQ_STR;
		case X_ERESTART:
			return X_ERESTART_STR;
		case X_ESTRPIPE:
			return X_ESTRPIPE_STR;
		case X_EUSERS:
			return X_EUSERS_STR;
		case X_ENOTSOCK:
			return X_ENOTSOCK_STR;
		case X_EDESTADDRREQ:
			return X_EDESTADDRREQ_STR;
		case X_EMSGSIZE:
			return X_EMSGSIZE_STR;
		case X_EPROTOTYPE:
			return X_EPROTOTYPE_STR;
		case X_ENOPROTOOPT:
			return X_ENOPROTOOPT_STR;
		case X_EPROTONOSUPPORT:
			return X_EPROTONOSUPPORT_STR;
		case X_ESOCKTNOSUPPORT:
			return X_ESOCKTNOSUPPORT_STR;
		case X_EOPNOTSUPP:
			return X_EOPNOTSUPP_STR;
		case X_EPFNOSUPPORT:
			return X_EPFNOSUPPORT_STR;
		case X_EAFNOSUPPORT:
			return X_EAFNOSUPPORT_STR;
		case X_EADDRINUSE:
			return X_EADDRINUSE_STR;
		case X_EADDRNOTAVAIL:
			return X_EADDRNOTAVAIL_STR;
		case X_ENETDOWN:
			return X_ENETDOWN_STR;
		case X_ENETUNREACH:
			return X_ENETUNREACH_STR;
		case X_ENETRESET:
			return X_ENETRESET_STR;
		case X_ECONNABORTED:
			return X_ECONNABORTED_STR;
		case X_ECONNRESET:
			return X_ECONNRESET_STR;
		case X_ENOBUFS:
			return X_ENOBUFS_STR;
		case X_EISCONN:
			return X_EISCONN_STR;
		case X_ENOTCONN:
			return X_ENOTCONN_STR;
		case X_ESHUTDOWN:
			return X_ESHUTDOWN_STR;
		case X_ETOOMANYREFS:
			return X_ETOOMANYREFS_STR;
		case X_ETIMEDOUT:
			return X_ETIMEDOUT_STR;
		case X_ECONNREFUSED:
			return X_ECONNREFUSED_STR;
		case X_EHOSTDOWN:
			return X_EHOSTDOWN_STR;
		case X_EHOSTUNREACH:
			return X_EHOSTUNREACH_STR;
		case X_EALREADY:
			return X_EALREADY_STR;
		case X_EINPROGRESS:
			return X_EINPROGRESS_STR;
		case X_ESTALE:
			return X_ESTALE_STR;
		case X_EUCLEAN:
			return X_EUCLEAN_STR;
		case X_ENOTNAM:
			return X_ENOTNAM_STR;
		case X_ENAVAIL:
			return X_ENAVAIL_STR;
		case X_EISNAM:
			return X_EISNAM_STR;
		case X_EREMOTEIO:
			return X_EREMOTEIO_STR;
		case X_EDQUOT:
			return X_EDQUOT_STR;
		case X_ENOMEDIUM:
			return X_ENOMEDIUM_STR;
		case X_EMEDIUMTYPE:
			return X_EMEDIUMTYPE_STR;
		case X_ECANCELED:
			return X_ECANCELED_STR;
		case X_ENOKEY:
			return X_ENOKEY_STR;
		case X_EKEYEXPIRED:
			return X_EKEYEXPIRED_STR;
		case X_EKEYREVOKED:
			return X_EKEYREVOKED_STR;
		case X_EKEYREJECTED:
			return X_EKEYREJECTED_STR;
		case X_EOWNERDEAD:
			return X_EOWNERDEAD_STR;
		case X_ENOTRECOVERABLE:
			return X_ENOTRECOVERABLE_STR;
		case X_ERFKILL:
			return X_ERFKILL_STR;
		case X_EHWPOISON:
			return X_EHWPOISON_STR;
/*		case X_ENOTSUP:
			return X_ENOTSUP_STR; */
		default:
			break;
	}

	return X_EUNKNOWN_STR;
}
