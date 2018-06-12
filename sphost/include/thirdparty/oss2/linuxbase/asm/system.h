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
#define smp_mb__after_atomic()		smp_mb()

#define smp_store_release(p, v)		do { *(p) = (v); } while(0)
#define smp_load_acquire(p)		(*(p))

#endif /* _ASM_SYSTEM_H */
