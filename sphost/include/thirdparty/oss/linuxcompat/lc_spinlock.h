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
#include "debug_mod/dbg_api.h"
#include "lc_interrupt.h"

/**************************************************************************
 *                          D A T A   T Y P E S
 **************************************************************************/
typedef struct spinlock_s{
	unsigned int mutex;
	unsigned int interrupt_save; /*use for IRQ disable*/
}spinlock_t;

/**************************************************************************
 *                              M A C R O S
 **************************************************************************/
#if 1
static inline unsigned int load_linked(unsigned int addr)
{
	unsigned int res;

	__asm__ __volatile__("ll\t%0,(%1)"
	                     : "=r" (res)
	                     : "r" (addr));
	return res;
}

static inline unsigned int store_conditional(unsigned int addr, unsigned int value)
{
	unsigned int res;

	__asm__ __volatile__("sc\t%0,(%2)"
	                     : "=r" (res)
	                     : "0" (value), "r" (addr));
	return res;
}

static inline int spin_lock(spinlock_t *plock)
{
	unsigned int mw;

	do {
		mw = load_linked((unsigned int)&plock->mutex);
		if (mw) {
			return -1;
		}
	} while (!store_conditional((unsigned int)&plock->mutex, 1));

	asm volatile("":::"memory");

	return 0;
}

static inline void spin_unlock(spinlock_t *plock)
{
	asm volatile("":::"memory");
	plock->mutex = 0;
}

static inline void spin_lock_init(spinlock_t *plock)
{
	plock->mutex = 0;
}

static inline void spin_lock_destroy(spinlock_t *plock)
{
	return;
}

static inline int spin_is_locked(spinlock_t *plock)
{
	return (plock->mutex != 0);
}

#define spin_lock_irqsave(lock, flags)	\
do {						\
	local_irq_save((lock)->interrupt_save);	\
	spin_lock(lock); \
	(flags) = (flags);				\
} while (0)

#define spin_lock_irq(lock)			\
do {						\
	local_irq_save((lock)->interrupt_save); \
	spin_lock(lock); \
} while (0)

#define spin_unlock_irqrestore(lock, flags)	\
do {						\
	spin_unlock(lock); \
	(flags) = (flags);				\
	local_irq_restore((lock)->interrupt_save); \
} while (0)

#define spin_unlock_irq(lock)			\
do {						\
	spin_unlock(lock); \
	local_irq_restore((lock)->interrupt_save);		\
} while (0)

#else
#define spin_lock_init(lock) \
do {\
	(lock)->mutex = ros_mutex_create("usbhost-spin", 0);\
	ASSERT((lock)->mutex);\
}while(0)

#define spin_lock_destroy(lock)\
do {\
	if ((lock)->mutex) {\
		ros_mutex_destroy((lock)->mutex);\
		(lock)->mutex = 0;\
	}\
} while(0)

#define spin_lock(lock)\
do {\
	ASSERT((lock)->mutex);\
	ros_mutex_get((lock)->mutex, ROS_WAIT_FOREVER);\
}while(0)

#define spin_unlock(lock) \
do {\
	ASSERT((lock)->mutex);\
	ros_mutex_put((lock)->mutex);\
}while(0)

#define spin_lock_irqsave(lock, flags)	\
do {\
	ASSERT((lock)->mutex);\
	ros_mutex_get((lock)->mutex, ROS_WAIT_FOREVER);\
	local_irq_save((lock)->interrupt_save);\
	(flags) = (flags);\
} while (0)

#define spin_lock_irq(lock)			\
do {\
	ASSERT((lock)->mutex);\
	ros_mutex_get((lock)->mutex, ROS_WAIT_FOREVER);\
	local_irq_save((lock)->interrupt_save); \
} while (0)

#define spin_unlock_irqrestore(lock, flags)	\
do {\
	local_irq_restore((lock)->interrupt_save); \
	ASSERT((lock)->mutex);\
	ros_mutex_put((lock)->mutex);\
	(flags) = (flags);\
} while (0)

#define spin_unlock_irq(lock)			\
do {						\
	local_irq_restore((lock)->interrupt_save); \
	ASSERT((lock)->mutex);\
	ros_mutex_put((lock)->mutex);\
} while (0)

#endif
#define spin_lock_bh spin_lock_irq
#define spin_unlock_bh spin_unlock_irq


#endif /* _LINUXCOMPAT_SPINLOCK_H_ */
