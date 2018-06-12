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
 * @file lc_kthread.h
 * @brief The brief introduction for this file
 * @author LiJun
 *
 * The detail information about this file.
 */
#ifndef _LINUXCOMPAT_KTHREAD_H_
#define _LINUXCOMPAT_KTHREAD_H_

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include <linux/types.h>
#include "lc_sched.h"

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************
 *                              M A C R O S
 **************************************************************************/
#define LINUX_THREAD_STACK_SIZE	(4096-256)
#define LINUX_THREAD_PRIORITY    (20)
#define LINUX_THREAD_SILICE      (10)

#define kernel_thread(fn, arg, flags)	(int)kthread_run(fn, arg, #fn);
#define schedule()

#define smp_processor_id()	0

/**************************************************************************
 *                            T Y P E S
 **************************************************************************/
typedef int (*kthread_func_t)(void *data);

/**************************************************************************
 *              F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
/* Create thread and run it immediately.
 * Returns a task_struct or ERR_PTR(-ENOMEM).
 */
extern struct task_struct *kthread_run(kthread_func_t func, void *data, const char *name);
extern struct task_struct *kthread_run_priority(kthread_func_t func, void *data, const char *name, unsigned int priority);
extern int kthread_stop(struct task_struct *k);
extern bool kthread_join(struct task_struct *k);
extern bool kthread_should_stop();
extern int kthread_exit(); /* kill thread itself. */
extern unsigned int kthread_identify(void *k);
/* \return see ros_thread_priority_change */
extern int kthread_priority_change(void *k, unsigned int priority);
extern void kthread_schedule();
extern void kthread_wakeup(struct task_struct *k);

#ifdef __cplusplus
}
#endif

#endif
