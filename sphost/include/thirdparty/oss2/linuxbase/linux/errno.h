#ifndef _LINUX_ERRNO_H
#define _LINUX_ERRNO_H

#include <asm/errno.h>

/* from uapi/asm-generic/errno.h */
///#define	ENOMEDIUM	123	/* No medium found */
#define	EMEDIUMTYPE	124	/* Wrong medium type */
///#define	ECANCELED	125	/* Operation Canceled */
#define	ENOKEY		126	/* Required key not available */
#define	EKEYEXPIRED	127	/* Key has expired */
#define	EKEYREVOKED	128	/* Key has been revoked */
#define	EKEYREJECTED	129	/* Key was rejected by service */


#define ERESTARTNOINTR  513
#define ERESTARTNOHAND  514     /* restart if no handler.. */
#define ENOIOCTLCMD     515     /* No ioctl command */
#define ERESTART_RESTARTBLOCK 516 /* restart by calling sys_restart_syscall */
			/* Defined for the NFSv3 protocol */
#define EBADHANDLE      521     /* Illegal NFS file handle */
#define ENOTSYNC        522     /* Update synchronization mismatch */
#define EBADCOOKIE      523     /* Cookie is stale */
#define ENOTSUPP        524     /* Operation is not supported */
#define ETOOSMALL       525     /* Buffer or request is too small */
#define ESERVERFAULT    526     /* An untranslatable error occurred */
#define EBADTYPE        527     /* Type not supported by server */
#define EJUKEBOX        528     /* Request initiated, but will not complete before timeout */
#define EIOCBQUEUED     529     /* iocb queued, will get completion event */
#define EIOCBRETRY      530     /* iocb queued, will trigger a retry */

#endif
