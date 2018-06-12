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
 * @file lc_timer.h
 * @brief The brief introduction for this file
 * @author LiJun
 *
 * The detail information about this file.
 */
#ifndef _LINUXCOMPAT_TIMER_H_
#define _LINUXCOMPAT_TIMER_H_

#include "lc_compiler.h"
#include "lc_kthread.h"
#include "lc_workqueue.h"

#define TIMEREVENT_THREAD_PRIORITY (LINUX_THREAD_PRIORITY -1)
#define TIMEREVENT_THREAD_STACKDEP (4096-256)

/**************************************************************************
 *                          D A T A   T Y P E S
 **************************************************************************/
typedef void (*timer_list_func_t)(unsigned long);

struct timer_list {
	struct work_struct work;
	const char        *pname;
	unsigned long      expires;
	timer_list_func_t  function;
	unsigned long      data;
};

unsigned int curCpuTick(void);

/**************************************************************************
 *              F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
void __init_timer(struct timer_list * timer, char *pname);
#define init_timer(t)	__init_timer(t, __FILE__ ":" __STR_LINE__);

int  setup_timer(struct timer_list* timer, timer_list_func_t func, unsigned long data);
void add_timer(struct timer_list *timer);
int  mod_timer(struct timer_list *timer, unsigned long delay);
int  del_timer(struct timer_list * timer);

#define del_timer_sync(t)	del_timer(t)

/***
 * timer_pending - is a timer pending?
 * @timer: the timer in question
 *
 * timer_pending will tell whether a given timer is currently pending,
 * or not. Callers must ensure serialization wrt. other operations done
 * to this timer, eg. interrupt contexts, or other CPUs on SMP.
 *
 * return value: 1 if the timer is pending, 0 if not.
 */

#define timer_pending(t)	work_pending(&(t)->work)

void timer_list_test_main(int argc, char *argv[]);

#endif
