#ifndef _ASM_ERRNO_H
#define _ASM_ERRNO_H

#ifndef __LINUX_ERRNO_EXTENSIONS__
#define __LINUX_ERRNO_EXTENSIONS__
#endif
#include <errno.h>

#define ENMFILE 		89    /* No such host or network path */
#define ENOSHARE 		136    /* No such host or network path */
#define ECASECLASH 	137  /* Filename exists with different case */

#define ENOERR			0
#define ERESTARTSYS	512
#define EREMOTEIO		513
#define ENOSUPP		514

#endif /*_ASM_ERRNO_H */
