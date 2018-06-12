#ifndef _ASM_SYSTEM_H
#define _ASM_SYSTEM_H

#include <linux/config.h>
#include <linux/types.h>
#include <linux/compiler.h>

#include <asm/wbflush.h>
#include <asm/interrupt.h>

#define wmb()		barrier(); wbflush()
#define rmb()		barrier(); wbflush()
#define mb()		barrier(); wbflush()
#define iob()		barrier(); wbflush()

#define smp_mb()	barrier()
#define smp_rmb()	barrier()
#define smp_wmb()	barrier()
#define smp_read_barrier_depends()	do { } while(0)

#endif /* _ASM_SYSTEM_H */
