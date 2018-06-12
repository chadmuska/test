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
#include "lc_jiffies.h"
#include "lc_workqueue.h"

#define TIMEREVENT_THREAD_PRIORITY (LINUX_THREAD_PRIORITY -1)
#define TIMEREVENT_THREAD_STACKDEP (4096-256)

#define NSEC_PER_USEC	1000L

#define CLOCK_MONOTONIC			1

/**************************************************************************
 *                          D A T A   T Y P E S
 **************************************************************************/
typedef void (*timer_list_func_t)(unsigned long);

typedef struct timer_list {
	struct work_struct work;
	const char        *pname;
	s64                tmr_expires; /* => expires */
	timer_list_func_t  function;
	unsigned long      data;
} timer_list_t;

/**************************************************************************
 *              F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
unsigned int curCpuTick(void);

void __init_timer(struct timer_list * timer, const char *pname);
#define init_timer(t)	__init_timer(t, __FILE__ ":" __STR_LINE__);

int  __setup_timer(struct timer_list* timer, timer_list_func_t func, unsigned long data, const char *pname);
#define setup_timer(timer, func, data)	__setup_timer(timer, func, data, __FILE__ ":" __STR_LINE__)

void add_timer_rel(struct timer_list *timer);
void add_timer_abs(struct timer_list *timer);

/* expires is a relative time to the current time. unit: milli-seconds */
int  mod_timer_rel(struct timer_list *timer, unsigned long expires);
/* expires is an absolute time. unit: milli-seconds */
int  mod_timer_abs(struct timer_list *timer, s64 expires);
int  del_timer(struct timer_list * timer);

#ifdef ABS_MOD_TIMER
#define mod_timer(t, e)			mod_timer_abs(t, e)
#define add_timer(t)			add_timer_abs(t)
#else
#define mod_timer(t, e)			mod_timer_rel(t, e)
#define add_timer(t)			add_timer_rel(t)
#endif

#define destroy_timer(t)		del_timer(t)
#define del_timer_sync(t)		del_timer(t)

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

#define timer_pending(t)		work_pending(&(t)->work)

/* hrtimer */
enum hrtimer_mode {
	HRTIMER_MODE_ABS = 0x0,		/* Time value is absolute */
	HRTIMER_MODE_REL = 0x1,		/* Time value is relative to now */
	HRTIMER_MODE_PINNED = 0x02,	/* Timer is bound to CPU */
	HRTIMER_MODE_ABS_PINNED = 0x02,
	HRTIMER_MODE_REL_PINNED = 0x03,
};

enum hrtimer_restart {
	HRTIMER_NORESTART,	/* Timer is not restarted */
	HRTIMER_RESTART,	/* Timer must be restarted */
};

struct hrtimer;

typedef enum hrtimer_restart (*hrtimer_function)(struct hrtimer *);

struct hrtimer {
	struct timer_list tl;
	hrtimer_function  func;
};

#define hrtimer_init(htimer, clock_id, mode)	init_timer(&(htimer)->tl)
#define hrtimer_active(htimer)			timer_pending(&(htimer)->tl)
#define hrtimer_cancel(htimer)			del_timer(&(htimer)->tl)

void hrtimer_setup(struct hrtimer *htimer, hrtimer_function func);
int  hrtimer_start(struct hrtimer *timer, ktime_t tim, const enum hrtimer_mode mode);

#endif
