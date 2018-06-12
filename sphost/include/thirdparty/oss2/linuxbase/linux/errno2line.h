#ifndef __ERRNO_TO_LINE_H__
#define __ERRNO_TO_LINE_H__

#include <linux/errno.h>

#ifndef __ERR_LINE__
//#define __ERR_LINE__	({printf(">> error %s:%d <<\n", __func__, __LINE__); __LINE__;})
#define __ERR_LINE__	__LINE__
#endif

#undef ERESTARTSYS
#define ERESTARTSYS __ERR_LINE__
#undef EREMOTEIO
#define EREMOTEIO __ERR_LINE__
#undef ENOSUPP
#define ENOSUPP __ERR_LINE__
#undef ERESTARTNOINTR
#define ERESTARTNOINTR __ERR_LINE__
#undef ERESTARTNOHAND
#define ERESTARTNOHAND __ERR_LINE__
#undef ENOIOCTLCMD
#define ENOIOCTLCMD __ERR_LINE__
#undef ERESTART_RESTARTBLOCK
#define ERESTART_RESTARTBLOCK __ERR_LINE__
#undef EBADHANDLE
#define EBADHANDLE __ERR_LINE__
#undef ENOTSYNC
#define ENOTSYNC __ERR_LINE__
#undef EBADCOOKIE
#define EBADCOOKIE __ERR_LINE__
#undef ENOTSUPP
#define ENOTSUPP __ERR_LINE__
#undef ETOOSMALL
#define ETOOSMALL __ERR_LINE__
#undef ESERVERFAULT
#define ESERVERFAULT __ERR_LINE__
#undef EBADTYPE
#define EBADTYPE __ERR_LINE__
#undef EJUKEBOX
#define EJUKEBOX __ERR_LINE__
#undef EIOCBQUEUED
#define EIOCBQUEUED __ERR_LINE__
#undef EIOCBRETRY
#define EIOCBRETRY __ERR_LINE__

#undef EPERM
///#undef ENOENT
#undef ESRCH
///#undef EINTR
#undef EIO
#undef ENXIO
#undef E2BIG
#undef ENOEXEC
#undef EBADF
#undef ECHILD
#undef ENOMEM
#undef EACCES
#undef EFAULT
#undef ENOTBLK
#undef EBUSY
#undef EEXIST
#undef EXDEV
#undef ENODEV
#undef ENOTDIR
#undef EISDIR
#undef EINVAL
#undef ENFILE
#undef EMFILE
#undef ENOTTY
#undef ETXTBSY
#undef EFBIG
#undef ENOSPC
#undef ESPIPE
#undef EROFS
#undef EMLINK
#undef EPIPE
#undef EDOM
#undef ERANGE
#undef ENOMSG
#undef EIDRM
#undef ECHRNG
#undef EL2NSYNC
#undef EL3HLT
#undef EL3RST
#undef ELNRNG
#undef EUNATCH
#undef ENOCSI
#undef EL2HLT
#undef EDEADLK
#undef ENOLCK
#undef EBADE
#undef EBADR
#undef EXFULL
#undef ENOANO
#undef EBADRQC
#undef EBADSLT
#undef EDEADLOCK
#undef EBFONT
#undef ENOSTR
#undef ENODATA
#undef ETIME
#undef ENOSR
#undef ENONET
#undef ENOPKG
#undef EREMOTE
#undef ENOLINK
#undef EADV
#undef ESRMNT
#undef ECOMM
#undef EPROTO
#undef EMULTIHOP
#undef ELBIN
#undef EDOTDOT
#undef EBADMSG
#undef EFTYPE
#undef ENOTUNIQ
#undef EBADFD
#undef EREMCHG
#undef ELIBACC
#undef ELIBBAD
#undef ELIBSCN
#undef ELIBMAX
#undef ELIBEXEC
#undef ENOSYS
#undef ENMFILE
#undef ENOTEMPTY
#undef ENAMETOOLONG
#undef ELOOP
#undef EOPNOTSUPP
#undef EPFNOSUPPORT
#undef ECONNRESET
#undef ENOBUFS
#undef EAFNOSUPPORT
#undef EPROTOTYPE
#undef ENOTSOCK
#undef ENOPROTOOPT
#undef ESHUTDOWN
#undef ECONNREFUSED
#undef EADDRINUSE
#undef ECONNABORTED
#undef ENETUNREACH
#undef ENETDOWN
#undef ETIMEDOUT
#undef EHOSTDOWN
#undef EHOSTUNREACH
#undef EALREADY
#undef EDESTADDRREQ
#undef EMSGSIZE
#undef EPROTONOSUPPORT
#undef ESOCKTNOSUPPORT
#undef EADDRNOTAVAIL
#undef ENETRESET
#undef EISCONN
#undef ENOTCONN
#undef ETOOMANYREFS
#undef EPROCLIM
#undef EUSERS
#undef EDQUOT
#undef ESTALE
#undef ENOTSUP
#undef ENOMEDIUM
#undef ENOSHARE
#undef ECASECLASH
#undef EILSEQ
#undef EOVERFLOW
#undef ECANCELED
#undef ENOTRECOVERABLE
#undef EOWNERDEAD
#undef ESTRPIPE

#define	EPERM		__ERR_LINE__
///#define	ENOENT		__ERR_LINE__
#define	ESRCH		__ERR_LINE__
///#define	EINTR		__ERR_LINE__
#define	EIO		__ERR_LINE__
#define	ENXIO		__ERR_LINE__
#define	E2BIG		__ERR_LINE__
#define	ENOEXEC 	__ERR_LINE__
#define	EBADF		__ERR_LINE__
#define	ECHILD		__ERR_LINE__
#define	ENOMEM		__ERR_LINE__
#define	EACCES		__ERR_LINE__
#define	EFAULT		__ERR_LINE__
#define	ENOTBLK 	__ERR_LINE__
#define	EBUSY		__ERR_LINE__
#define	EEXIST		__ERR_LINE__
#define	EXDEV		__ERR_LINE__
#define	ENODEV		__ERR_LINE__
#define	ENOTDIR 	__ERR_LINE__
#define	EISDIR		__ERR_LINE__
#define	EINVAL		__ERR_LINE__
#define	ENFILE		__ERR_LINE__
#define	EMFILE		__ERR_LINE__
#define	ENOTTY		__ERR_LINE__
#define	ETXTBSY 	__ERR_LINE__
#define	EFBIG		__ERR_LINE__
#define	ENOSPC		__ERR_LINE__
#define	ESPIPE		__ERR_LINE__
#define	EROFS		__ERR_LINE__
#define	EMLINK		__ERR_LINE__
#define	EPIPE		__ERR_LINE__
#define	EDOM		__ERR_LINE__
#define	ERANGE		__ERR_LINE__
#define	ENOMSG		__ERR_LINE__
#define	EIDRM		__ERR_LINE__
#define	ECHRNG		__ERR_LINE__
#define	EL2NSYNC	__ERR_LINE__
#define	EL3HLT		__ERR_LINE__
#define	EL3RST		__ERR_LINE__
#define	ELNRNG		__ERR_LINE__
#define	EUNATCH 	__ERR_LINE__
#define	ENOCSI		__ERR_LINE__
#define	EL2HLT		__ERR_LINE__
#define	EDEADLK 	__ERR_LINE__
#define	ENOLCK		__ERR_LINE__
#define EBADE		__ERR_LINE__
#define EBADR		__ERR_LINE__
#define EXFULL		__ERR_LINE__
#define ENOANO		__ERR_LINE__
#define EBADRQC 	__ERR_LINE__
#define EBADSLT 	__ERR_LINE__
#define EDEADLOCK	__ERR_LINE__
#define EBFONT		__ERR_LINE__
#define ENOSTR		__ERR_LINE__
#define ENODATA 	__ERR_LINE__
#define ETIME		__ERR_LINE__
#define ENOSR		__ERR_LINE__
#define ENONET		__ERR_LINE__
#define ENOPKG		__ERR_LINE__
#define EREMOTE 	__ERR_LINE__
#define ENOLINK 	__ERR_LINE__
#define EADV		__ERR_LINE__
#define ESRMNT		__ERR_LINE__
#define	ECOMM		__ERR_LINE__
#define EPROTO		__ERR_LINE__
#define	EMULTIHOP	__ERR_LINE__
#define	ELBIN		__ERR_LINE__
#define	EDOTDOT 	__ERR_LINE__
#define EBADMSG 	__ERR_LINE__
#define EFTYPE		__ERR_LINE__
#define ENOTUNIQ	__ERR_LINE__
#define EBADFD		__ERR_LINE__
#define EREMCHG 	__ERR_LINE__
#define ELIBACC 	__ERR_LINE__
#define ELIBBAD 	__ERR_LINE__
#define ELIBSCN 	__ERR_LINE__
#define ELIBMAX 	__ERR_LINE__
#define ELIBEXEC	__ERR_LINE__
#define ENOSYS		__ERR_LINE__
#define ENMFILE 	__ERR_LINE__
#define ENOTEMPTY	__ERR_LINE__
#define ENAMETOOLONG	__ERR_LINE__
#define ELOOP		__ERR_LINE__
#define EOPNOTSUPP	__ERR_LINE__
#define EPFNOSUPPORT	__ERR_LINE__
#define ECONNRESET	__ERR_LINE__
#define ENOBUFS 	__ERR_LINE__
#define EAFNOSUPPORT	__ERR_LINE__
#define EPROTOTYPE	__ERR_LINE__
#define ENOTSOCK	__ERR_LINE__
#define ENOPROTOOPT	__ERR_LINE__
#define ESHUTDOWN	__ERR_LINE__
#define ECONNREFUSED	__ERR_LINE__
#define EADDRINUSE	__ERR_LINE__
#define ECONNABORTED	__ERR_LINE__
#define ENETUNREACH	__ERR_LINE__
#define ENETDOWN	__ERR_LINE__
#define ETIMEDOUT	__ERR_LINE__
#define EHOSTDOWN	__ERR_LINE__
#define EHOSTUNREACH	__ERR_LINE__
#define EALREADY	__ERR_LINE__
#define EDESTADDRREQ	__ERR_LINE__
#define EMSGSIZE	__ERR_LINE__
#define EPROTONOSUPPORT __ERR_LINE__
#define ESOCKTNOSUPPORT __ERR_LINE__
#define EADDRNOTAVAIL	__ERR_LINE__
#define ENETRESET	__ERR_LINE__
#define EISCONN 	__ERR_LINE__
#define ENOTCONN	__ERR_LINE__
#define ETOOMANYREFS	__ERR_LINE__
#define EPROCLIM	__ERR_LINE__
#define EUSERS		__ERR_LINE__
#define EDQUOT		__ERR_LINE__
#define ESTALE		__ERR_LINE__
#define ENOTSUP 	__ERR_LINE__
#define ENOMEDIUM	__ERR_LINE__
#define ENOSHARE	__ERR_LINE__
#define ECASECLASH	__ERR_LINE__
#define EILSEQ		__ERR_LINE__
#define EOVERFLOW	__ERR_LINE__
#define ECANCELED	__ERR_LINE__
#define ENOTRECOVERABLE __ERR_LINE__
#define EOWNERDEAD	__ERR_LINE__
#define ESTRPIPE	__ERR_LINE__

#endif
