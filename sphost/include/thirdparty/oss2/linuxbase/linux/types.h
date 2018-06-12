#ifndef _LINUX_TYPES_H
#define _LINUX_TYPES_H

#include <linux/config.h>
#include <asm/types.h>
#include <limits.h>

#if 0 /*we have defined pm_message_t in pm.h*/
typedef u32  pm_message_t;
#endif

typedef unsigned int uint;
typedef u32 phys_addr_t;
typedef unsigned  gfp_t;

#ifndef IFNAMSIZ
# define IFNAMSIZ			16
#endif

#ifndef IFALIASZ
# define IFALIASZ			256
#endif

#ifndef IF_NAMESIZE
# define IF_NAMESIZE			IFNAMSIZ
#endif

#ifndef IF_HWADDRSIZE
# define IF_HWADDRSIZE			14
#endif

#endif /* _LINUX_TYPES_H */
