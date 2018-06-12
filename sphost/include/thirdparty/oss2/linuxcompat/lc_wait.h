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
 * @file lc_wait.h
 * @brief The brief introduction for this file
 * @author LiJun
 *
 * The detail information about this file.
 */
#ifndef _LINUXCOMPAT_WAIT_H_
#define _LINUXCOMPAT_WAIT_H_

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "lc_compiler.h"
#include <lc_sched.h>
#include <linux/list.h>

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************
 *                          D A T A   T Y P E S
 **************************************************************************/
typedef struct {
	struct list_head list;
	u32_t thrd;
	s8_t in_queue : 1;
	s8_t waked_up : 1;
} wait_queue_t;

typedef struct {
	struct list_head list;
	wait_queue_t evt_wait; /* for wait_event and wait_event_timeout */
	s8_t locked : 1;
} wait_queue_head_t;

#define WAIT_IS_QUEUING(wait)			((wait)->in_queue)

/**************************************************************************
 *              F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
/*
 * Initialize
 */
#define WAIT_QUEUE_INITIALIZER(name)		{ { NULL, NULL }, 0, 0, 0 }
#define DEFINE_WAIT(name)			wait_queue_t name = WAIT_QUEUE_INITIALIZER(name)
#define DEFINE_WAIT_FUNC(name, func)		DEFINE_WAIT(name)
#define DECLARE_WAITQUEUE(name, current)	DEFINE_WAIT(name)

#define WQ_HEAD_INITIALIZER(name) \
	{ LIST_HEAD_INIT((name).list), WAIT_QUEUE_INITIALIZER((name).evt_wait), 0 }

#define DECLARE_WAIT_QUEUE_HEAD(name)		wait_queue_head_t name = WQ_HEAD_INITIALIZER(name)

void __init_waitqueue_head(wait_queue_head_t *wq, const char* name);
void __reinit_waitqueue_head(wait_queue_head_t *wq);
void destroy_wait_queue(wait_queue_head_t *wq);
int  waitqueue_active(wait_queue_head_t *wq);

void init_waitqueue(wait_queue_t *wait);
#define init_waitqueue_entry(wait, task)	init_waitqueue(wait)

/*
 * Wait
 */
void __add_wait_queue(wait_queue_head_t *wq, wait_queue_t *wait);
void __remove_wait_queue(wait_queue_head_t *wq, wait_queue_t *wait);
long __wait_schedule_timeout(wait_queue_t *wait, long to);

/*
 * return 0: timeout; > 0: milliseconds left till timeout
 */
void __wait_do_schedule(wait_queue_t *wait, long to);

#define ___wait_cond_timeout(condition)					\
({									\
	BOOL __cond = (condition);					\
	if (__cond && !__ret)						\
		__ret = 1;						\
	__cond || !__ret;						\
})

#define __wait_event_timeout(wq, condition, timeout)			\
({									\
	s64_t __exptime = getmstimeofday() + timeout;			\
	long __ret = timeout, __to = 10;				\
	while (!___wait_cond_timeout(condition)) {			\
		__wait_do_schedule(&((wq)->evt_wait), __to);		\
		__ret = (long)(__exptime - getmstimeofday());		\
		if (__ret <= 0) break;					\
		__to = __ret > 500 ? 500 : __ret;			\
	}								\
	if (__ret < 0) __ret = 0;					\
	__ret;								\
})

#define wait_event(wq, condition)					\
do {									\
	if (condition)							\
		break;							\
	__wait_event_timeout(&(wq), condition, MAX_SCHEDULE_TIMEOUT);	\
} while (0)

#define wait_event_timeout(wq, condition, timeout)			\
({									\
	long __ret = timeout;						\
	if (!___wait_cond_timeout(condition))				\
		__ret = __wait_event_timeout(&(wq), condition, timeout);\
	__ret;								\
})

/* replace schedule */
#define wait_schedule(wait)			__wait_schedule_timeout(wait, MAX_SCHEDULE_TIMEOUT)
#define wait_schedule_timeout(wait, to)		__wait_schedule_timeout(wait, to)
#define wait_woken(wait, mode, to)		wait_schedule_timeout(wait, to)

/*
 * Wake Up
 */
void __wake_up(wait_queue_head_t *wq, int nr);

static inline void wake_up(wait_queue_head_t *wq)	{ __wake_up(wq, 1); }
static inline void wake_up_all(wait_queue_head_t *wq)	{ __wake_up(wq, 0); }

#define wake_up_interruptible			wake_up
#define wake_up_interruptible_poll(a,b)		wake_up(a)
#define wake_up_interruptible_all		wake_up_all

#define init_waitqueue_head(wq) 		__init_waitqueue_head(wq, __FILE__ ":" __STR_LINE__)

#define add_wait_queue(wq, wait)		__add_wait_queue(wq, wait)
#define remove_wait_queue(wq, wait)		__remove_wait_queue(wq, wait);
#define prepare_to_wait(wq, wait, flags)	do{ if (!WAIT_IS_QUEUING(wait)) __add_wait_queue(wq, wait); } while(0)
#define finish_wait(wq, wait)			((void)0)

#define wait_event_interruptible		wait_event
#define wait_event_interruptible_timeout	wait_event_timeout
#define wait_event_interruptible_exclusive	wait_event

#ifdef __cplusplus
}
#endif

#endif


