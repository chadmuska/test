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

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************
 *                          D A T A   T Y P E S
 **************************************************************************/
typedef struct completion {
	unsigned int events;
	unsigned int wait_cnt;
} wait_queue_head_t;



/**************************************************************************
 *              F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
/* The wait queue is not initialize. Must add codes to initialize it. */
#define DECLARE_WAIT_QUEUE_HEAD(name) wait_queue_head_t name = { .events = 0 }

extern void __init_wq_head(wait_queue_head_t *wq, const char* name);
extern int  __wait_event_timeout(wait_queue_head_t* wq, int to);
extern int  __wait_event(wait_queue_head_t* wq);

extern void wake_up(wait_queue_head_t *wq);
#define wake_up_interruptible			wake_up

extern void destroy_wait_queue(wait_queue_head_t *wq);

#define init_waitqueue_head(wq) 		__init_wq_head(wq, __FILE__ ":" __STR_LINE__)

#define wait_event(wq, cond) \
do {\
	if(!(cond)) \
		__wait_event(&(wq));\
} while(0)

#define wait_event_timeout(wq, cond, to) \
do {\
	if(!(cond))\
		__wait_event_timeout(&(wq), to);\
}while(0)

#define wait_event_interruptible		wait_event
#define wait_event_interruptible_timeout	wait_event_timeout

#ifdef __cplusplus
}
#endif

#endif


