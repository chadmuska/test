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
 * @file lc_completion.h
 * @brief The brief introduction for this file
 * @author LiJun
 *
 * The detail information about this file.
 */
#ifndef _LINUXCOMPAT_COMPLETION_H_
#define _LINUXCOMPAT_COMPLETION_H_

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include <asm/errno.h>
#include <lc_wait.h>

struct completion {
	unsigned int done;
	wait_queue_head_t wait;
};
typedef struct completion completion_t;

/**************************************************************************
 *                              M A C R O S
 **************************************************************************/
#define COMPLETION_INITIALIZER(name)			{ 0, WQ_HEAD_INITIALIZER((name).wait) }
#define DECLARE_COMPLETION(name)			struct completion name = COMPLETION_INITIALIZER(name)
#define DECLARE_COMPLETION_ONSTACK(name)		DECLARE_COMPLETION(name)

static inline void init_completion(struct completion *c) {
	c->done = 0;
	init_waitqueue_head(&c->wait);
}

static inline void reinit_completion(struct completion *c) {
	c->done = 0;
	__reinit_waitqueue_head(&c->wait);
}

static inline void reset_completion(struct completion *c) {
	ASSERT(!c->wait.evt_wait.thrd); /* no one wait on this completion */
	c->done = 0;
}

#define destroy_completion(c)				destroy_wait_queue(&(c)->wait)

/*
 * Return: 0 if timed out, and positive (at least 1, or number of milli-seconds left
 * till timeout) if completed.
 */
extern unsigned long __wait_for_completion_timeout(struct completion *c, unsigned long to);
#define wait_for_completion(c)				__wait_for_completion_timeout(c, MAX_SCHEDULE_TIMEOUT)
#define wait_for_completion_timeout(c, to)		__wait_for_completion_timeout(c, to)

static inline int wait_for_completion_interruptible(struct completion *x) {
	unsigned long t = __wait_for_completion_timeout(x, MAX_SCHEDULE_TIMEOUT);
	if (t)
		return 0;
	return -ERESTARTSYS;
}

extern void __complete(struct completion *c, int nr);
static inline void complete(struct completion *c)	{ __complete(c, 1); }
static inline void complete_all(struct completion *c)	{ __complete(c, INT_MAX); }

#endif
