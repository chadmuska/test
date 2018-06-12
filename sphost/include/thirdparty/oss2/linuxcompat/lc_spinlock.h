/**************************************************************************
 *
 *       Copyright (c) 2013-2013 by iCatch Technology, Inc.
 *
 *  This software is copyrighted by and is the property of iCatch
 *  Technology, Inc.. All rights are reserved by iCatch Technology, Inc..
 *  This software may only be used in accordance with the corresponding
 *  license agreement. Any unauthorized use, duplication, distribution,
 *  or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of iCatch Technology, Inc..
 *
 *  iCatch Technology, Inc. reserves the right to modify this software
 *  without notice.
 *
 *  iCatch Technology, Inc.
 *  19-1, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan.
 *
 **************************************************************************/
/**
 * @file lc_spinlock.h
 * @brief The brief introduction for this file
 * @author LiJun
 *
 * The detail information about this file.
 */
#ifndef _LINUXCOMPAT_SPINLOCK_H_
#define _LINUXCOMPAT_SPINLOCK_H_

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/

/**************************************************************************
 *                          D A T A   T Y P E S
 **************************************************************************/
#define SPIN_LOCK_OPS \
	int  (*sl_lock)(void *plock, int irq); \
	void (*sl_unlock)(void *plock, int irq)

typedef struct spinlock {
	SPIN_LOCK_OPS;
	unsigned int mutex;
	unsigned int interrupt_save; /*use for IRQ disable*/
} spinlock_t;

/**************************************************************************
 *                        COMPATIBLE SPIN-LOCK
 **************************************************************************/
#define SPINLOCK_INITIALIZER(name) { \
	.sl_lock	= (int  (*)(void*, int))compat_spin_lock, \
	.sl_unlock	= (void (*)(void*, int))compat_spin_unlock, \
	.mutex		= 0, \
	.interrupt_save	= 0 \
}

#define DEFINE_SPINLOCK(name)			spinlock_t name = SPINLOCK_INITIALIZER(name)
#define DEFINE_RAW_SPINLOCK(name)		DEFINE_SPINLOCK(name)

extern int compat_spin_lock(spinlock_t *plock, int irq);
extern void compat_spin_unlock(spinlock_t *plock, int irq);
extern void compat_spin_lock_init(spinlock_t *plock);

/**************************************************************************
 *                     SPINLOCK COMMON FUNCTIONS
 **************************************************************************/

#define spin_lock_destroy(plock)		((void)0)

static inline int spin_is_locked(spinlock_t *plock)
{
	return (plock->mutex != 0);
}

#define spin_lock(plock)			((plock)->sl_lock(plock, 0))
#define spin_unlock(plock)			((plock)->sl_unlock(plock, 0))

#define spin_lock_irq(plock)			((plock)->sl_lock(plock, 1))
#define spin_unlock_irq(plock)			((plock)->sl_unlock(plock, 1))

#define spin_lock_bh(lock)			spin_lock_irq(lock)
#define spin_unlock_bh(lock)			spin_unlock_irq(lock)

#define raw_spin_lock_irqsave(lock, flags)	spin_lock_irqsave(lock, flags)
#define raw_spin_unlock_irqrestore(lock, flags)	spin_unlock_irqrestore(lock, flags)

#define spin_lock_irqsave(lock, flags)		\
do {						\
	spin_lock_irq(lock);			\
	(flags) = (1);				\
} while (0)

#define spin_unlock_irqrestore(lock, flags)	\
do {						\
	spin_unlock_irq(lock);			\
	(flags) = (0);				\
} while (0)

static inline void local_bh_disable(void) { }
static inline void local_bh_enable(void) { }

/**************************************************************************
 *                    MUTEX SPIN LOCK
 *             Cannot be used in irq handler.
 **************************************************************************/
/* mutex spin lock */
#include <lc_mutex.h>

typedef struct {
	SPIN_LOCK_OPS;
	mutex_t mutex;
} mutex_spinlock_t;

#define DEFINE_MUTEX_SPINLOCK(name) \
mutex_spinlock_t name = { \
	.sl_lock	= (int (*)(void*, int))mutex_spin_lock, \
	.sl_unlock	= (void (*)(void*, int))mutex_spin_unlock, \
	.mutex		= MUTEX_INITIALIZER(name) \
}

extern int mutex_spin_lock(mutex_spinlock_t *plock, int irq);
extern void mutex_spin_unlock(mutex_spinlock_t *plock, int irq);

/*
 * spin lock init
 */
extern void __spin_lock_init(void *p, size_t size, const char *name);
#define spin_lock_init(p)	__spin_lock_init(p, sizeof(*(p)), __FILE__ ":" __STR_LINE__)

#endif /* _LINUXCOMPAT_SPINLOCK_H_ */
