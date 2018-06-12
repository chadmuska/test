#ifndef _LINUX_TYPES_H
#define _LINUX_TYPES_H

#include <linux/config.h>
#include <asm/types.h>

#if 0 /*we have defined pm_message_t in pm.h*/
typedef u32  pm_message_t;
#endif

typedef unsigned int uint;
typedef u32 phys_addr_t;
typedef unsigned  gfp_t;

/* -1 should not be a valid pointer */
#define IS_ERR(x)			((x) == (void*)-1)
#define PTR_ERR(x)			((long)-1)
#define ERR_PTR(x)			((void*)-1)

#endif /* _LINUX_TYPES_H */
