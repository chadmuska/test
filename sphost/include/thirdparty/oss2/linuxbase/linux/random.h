
#ifndef __LINUX_RANDOM_H__
#define __LINUX_RANDOM_H__

#include <linux/types.h>

void get_random_bytes(void *buf, int nbytes);
u32 lnx_random32(void);
#define prandom_u32	lnx_random32

#endif

