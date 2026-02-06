/* @file: errno.c
 * #desc:
 *    The definitions of system error numbers.
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

#ifndef _DEMOZ_C_ERRNO_C
#define _DEMOZ_C_ERRNO_C

#include <demoz/config.h>
#include <demoz/c/stdint.h>


/* @def: _ */
#define X_EPERM 1
#define X_EPERM_STR "Operation not permitted"
#define X_ENOENT 2
#define X_ENOENT_STR "No such file or directory"
#define X_ESRCH 3
#define X_ESRCH_STR "No such process"
#define X_EINTR 4
#define X_EINTR_STR "Interrupted system call"
#define X_EIO 5
#define X_EIO_STR "I/O error"
#define X_ENXIO 6
#define X_ENXIO_STR "No such device or address"
#define X_E2BIG 7
#define X_E2BIG_STR "Argument list too long"
#define X_ENOEXEC 8
#define X_ENOEXEC_STR "Exec format error"
#define X_EBADF 9
#define X_EBADF_STR "Bad file descriptor"
#define X_ECHILD 10
#define X_ECHILD_STR "No child processes"
#define X_EAGAIN 11
#define X_EAGAIN_STR "Try again"
#define X_ENOMEM 12
#define X_ENOMEM_STR "Out of memory"
#define X_EACCES 13
#define X_EACCES_STR "Permission denied"
#define X_EFAULT 14
#define X_EFAULT_STR "Bad address"
#define X_ENOTBLK 15
#define X_ENOTBLK_STR "Block device required"
#define X_EBUSY 16
#define X_EBUSY_STR "Device or resource busy"
#define X_EEXIST 17
#define X_EEXIST_STR "File exists"
#define X_EXDEV 18
#define X_EXDEV_STR "Cross-device link"
#define X_ENODEV 19
#define X_ENODEV_STR "No such device"
#define X_ENOTDIR 20
#define X_ENOTDIR_STR "Not a directory"
#define X_EISDIR 21
#define X_EISDIR_STR "Is a directory"
#define X_EINVAL 22
#define X_EINVAL_STR "Invalid argument"
#define X_ENFILE 23
#define X_ENFILE_STR "File table overflow"
#define X_EMFILE 24
#define X_EMFILE_STR "Too many open files"
#define X_ENOTTY 25
#define X_ENOTTY_STR "Inappropriate ioctl for device"
#define X_ETXTBSY 26
#define X_ETXTBSY_STR "Text file busy"
#define X_EFBIG 27
#define X_EFBIG_STR "File too large"
#define X_ENOSPC 28
#define X_ENOSPC_STR "No space left on device"
#define X_ESPIPE 29
#define X_ESPIPE_STR "Illegal seek"
#define X_EROFS 30
#define X_EROFS_STR "Read-only file system"
#define X_EMLINK 31
#define X_EMLINK_STR "Too many links"
#define X_EPIPE 32
#define X_EPIPE_STR "Broken pipe"
#define X_EDOM 33
#define X_EDOM_STR "Math argument out of domain of func"
#define X_ERANGE 34
#define X_ERANGE_STR "Math result not representable"
#define X_EDEADLK 35
#define X_EDEADLK_STR "Resource deadlock would occur"
#define X_ENAMETOOLONG 36
#define X_ENAMETOOLONG_STR "File name too long"
#define X_ENOLCK 37
#define X_ENOLCK_STR "No record locks available"
#define X_ENOSYS 38
#define X_ENOSYS_STR "Function not implemented"
#define X_ENOTEMPTY 39
#define X_ENOTEMPTY_STR "Directory not empty"
#define X_ELOOP 40
#define X_ELOOP_STR "Too many symbolic links encountered"
#define X_EWOULDBLOCK 11
#define X_EWOULDBLOCK_STR X_EAGAIN_STR
#define X_ENOMSG 42
#define X_ENOMSG_STR "No message of desired type"
#define X_EIDRM 43
#define X_EIDRM_STR "Identifier removed"
#define X_ECHRNG 44
#define X_ECHRNG_STR "Channel number out of range"
#define X_EL2NSYNC 45
#define X_EL2NSYNC_STR "Level 2 not synchronized"
#define X_EL3HLT 46
#define X_EL3HLT_STR "Level 3 halted"
#define X_EL3RST 47
#define X_EL3RST_STR "Level 3 reset"
#define X_ELNRNG 48
#define X_ELNRNG_STR "Link number out of range"
#define X_EUNATCH 49
#define X_EUNATCH_STR "Protocol driver not attached"
#define X_ENOCSI 50
#define X_ENOCSI_STR "No CSI structure available"
#define X_EL2HLT 51
#define X_EL2HLT_STR "Level 2 halted"
#define X_EBADE 52
#define X_EBADE_STR "Invalid exchange"
#define X_EBADR 53
#define X_EBADR_STR "Invalid request descriptor"
#define X_EXFULL 54
#define X_EXFULL_STR "Exchange full"
#define X_ENOANO 55
#define X_ENOANO_STR "No anode"
#define X_EBADRQC 56
#define X_EBADRQC_STR "Invalid request code"
#define X_EBADSLT 57
#define X_EBADSLT_STR "Invalid slot"
#define X_EDEADLOCK 35
#define X_EDEADLOCK_STR X_EDEADLK_STR
#define X_EBFONT 59
#define X_EBFONT_STR "Bad font file format"
#define X_ENOSTR 60
#define X_ENOSTR_STR "Device not a stream"
#define X_ENODATA 61
#define X_ENODATA_STR "No data available"
#define X_ETIME 62
#define X_ETIME_STR "Timer expired"
#define X_ENOSR 63
#define X_ENOSR_STR "Out of streams resources"
#define X_ENONET 64
#define X_ENONET_STR "Machine is not on the network"
#define X_ENOPKG 65
#define X_ENOPKG_STR "Package not installed"
#define X_EREMOTE 66
#define X_EREMOTE_STR "Object is remote"
#define X_ENOLINK 67
#define X_ENOLINK_STR "Link has been severed"
#define X_EADV 68
#define X_EADV_STR "Advertise error"
#define X_ESRMNT 69
#define X_ESRMNT_STR "Srmount error"
#define X_ECOMM 70
#define X_ECOMM_STR "Communication error on send"
#define X_EPROTO 71
#define X_EPROTO_STR "Protocol error"
#define X_EMULTIHOP 72
#define X_EMULTIHOP_STR "Multihop attempted"
#define X_EDOTDOT 73
#define X_EDOTDOT_STR "RFS specific error"
#define X_EBADMSG 74
#define X_EBADMSG_STR "Not a data message"
#define X_EOVERFLOW 75
#define X_EOVERFLOW_STR "Value too large for defined data type"
#define X_ENOTUNIQ 76
#define X_ENOTUNIQ_STR "Name not unique on network"
#define X_EBADFD 77
#define X_EBADFD_STR "File descriptor in bad state"
#define X_EREMCHG 78
#define X_EREMCHG_STR "Remote address changed"
#define X_ELIBACC 79
#define X_ELIBACC_STR "Can not access a needed shared library"
#define X_ELIBBAD 80
#define X_ELIBBAD_STR "Accessing a corrupted shared library"
#define X_ELIBSCN 81
#define X_ELIBSCN_STR ".lib section in a.out corrupted"
#define X_ELIBMAX 82
#define X_ELIBMAX_STR "Attempting to link in too many shared libraries"
#define X_ELIBEXEC 83
#define X_ELIBEXEC_STR "Cannot exec a shared library directly"
#define X_EILSEQ 84
#define X_EILSEQ_STR "Illegal byte sequence"
#define X_ERESTART 85
#define X_ERESTART_STR "Interrupted system call should be restarted"
#define X_ESTRPIPE 86
#define X_ESTRPIPE_STR "Streams pipe error"
#define X_EUSERS 87
#define X_EUSERS_STR "Too many users"
#define X_ENOTSOCK 88
#define X_ENOTSOCK_STR "Socket operation on non-socket"
#define X_EDESTADDRREQ 89
#define X_EDESTADDRREQ_STR "Destination address required"
#define X_EMSGSIZE 90
#define X_EMSGSIZE_STR "Message too long"
#define X_EPROTOTYPE 91
#define X_EPROTOTYPE_STR "Protocol wrong type for socket"
#define X_ENOPROTOOPT 92
#define X_ENOPROTOOPT_STR "Protocol not available"
#define X_EPROTONOSUPPORT 93
#define X_EPROTONOSUPPORT_STR "Protocol not supported"
#define X_ESOCKTNOSUPPORT 94
#define X_ESOCKTNOSUPPORT_STR "Socket type not supported"
#define X_EOPNOTSUPP 95
#define X_EOPNOTSUPP_STR "Operation not supported on transport endpoint"
#define X_EPFNOSUPPORT 96
#define X_EPFNOSUPPORT_STR "Protocol family not supported"
#define X_EAFNOSUPPORT 97
#define X_EAFNOSUPPORT_STR "Address family not supported by protocol"
#define X_EADDRINUSE 98
#define X_EADDRINUSE_STR "Address already in use"
#define X_EADDRNOTAVAIL 99
#define X_EADDRNOTAVAIL_STR "Cannot assign requested address"
#define X_ENETDOWN 100
#define X_ENETDOWN_STR "Network is down"
#define X_ENETUNREACH 101
#define X_ENETUNREACH_STR "Network is unreachable"
#define X_ENETRESET 102
#define X_ENETRESET_STR "Network dropped connection because of reset"
#define X_ECONNABORTED 103
#define X_ECONNABORTED_STR "Software caused connection abort"
#define X_ECONNRESET 104
#define X_ECONNRESET_STR "Connection reset by peer"
#define X_ENOBUFS 105
#define X_ENOBUFS_STR "No buffer space available"
#define X_EISCONN 106
#define X_EISCONN_STR "Transport endpoint is already connected"
#define X_ENOTCONN 107
#define X_ENOTCONN_STR "Transport endpoint is not connected"
#define X_ESHUTDOWN 108
#define X_ESHUTDOWN_STR "Cannot send after transport endpoint shutdown"
#define X_ETOOMANYREFS 109
#define X_ETOOMANYREFS_STR "Too many references: cannot splice"
#define X_ETIMEDOUT 110
#define X_ETIMEDOUT_STR "Connection timed out"
#define X_ECONNREFUSED 111
#define X_ECONNREFUSED_STR "Connection refused"
#define X_EHOSTDOWN 112
#define X_EHOSTDOWN_STR "Host is down"
#define X_EHOSTUNREACH 113
#define X_EHOSTUNREACH_STR "No route to host"
#define X_EALREADY 114
#define X_EALREADY_STR "Operation already in progress"
#define X_EINPROGRESS 115
#define X_EINPROGRESS_STR "Operation now in progress"
#define X_ESTALE 116
#define X_ESTALE_STR "Stale NFS file handle"
#define X_EUCLEAN 117
#define X_EUCLEAN_STR "Structure needs cleaning"
#define X_ENOTNAM 118
#define X_ENOTNAM_STR "Not a XENIX named type file"
#define X_ENAVAIL 119
#define X_ENAVAIL_STR "No XENIX semaphores available"
#define X_EISNAM 120
#define X_EISNAM_STR "Is a named type file"
#define X_EREMOTEIO 121
#define X_EREMOTEIO_STR "Remote I/O error"
#define X_EDQUOT 122
#define X_EDQUOT_STR "Quota exceeded"
#define X_ENOMEDIUM 123
#define X_ENOMEDIUM_STR "No medium found"
#define X_EMEDIUMTYPE 124
#define X_EMEDIUMTYPE_STR "Wrong medium type"
#define X_ECANCELED 125
#define X_ECANCELED_STR "Operation Canceled"
#define X_ENOKEY 126
#define X_ENOKEY_STR "Required key not available"
#define X_EKEYEXPIRED 127
#define X_EKEYEXPIRED_STR "Key has expired"
#define X_EKEYREVOKED 128
#define X_EKEYREVOKED_STR "Key has been revoked"
#define X_EKEYREJECTED 129
#define X_EKEYREJECTED_STR "Key was rejected by service"
#define X_EOWNERDEAD 130
#define X_EOWNERDEAD_STR "Owner died"
#define X_ENOTRECOVERABLE 131
#define X_ENOTRECOVERABLE_STR "State not recoverable"
#define X_ERFKILL 132
#define X_ERFKILL_STR "Operation not possible due to RF-kill"
#define X_EHWPOISON 133
#define X_EHWPOISON_STR "Memory page has hardware error"
#define X_ENOTSUP 95
#define X_ENOTSUP_STR X_EOPNOTSUPP_STR

#define X_EUNKNOWN 0
#define X_EUNKNOWN_STR "Unknown error number"

#define x_errno \
	(*C_SYMBOL(___errno)())
/* end */


#ifdef __cplusplus
extern "C" {
#endif

/* c/errno.c */

extern
int32_t *C_SYMBOL(___errno)(void)
;

extern
const char *C_SYMBOL(strerror)(int32_t err)
;

#ifdef __cplusplus
}
#endif


#endif
