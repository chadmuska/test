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
 * @file lc_tasklet.h
 * @brief The brief introduction for this file
 * @author LiJun
 *
 * The detail information about this file.
 */
#ifndef _LINUXCOMPAT_TASKLET_H_
#define _LINUXCOMPAT_TASKLET_H_

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "lc_kthread.h"

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************
 *                              M A C R O S
 **************************************************************************/
/*
* note: the tasklet in linux is a softirq hdlr, so its priority is between at
* hw irq and kernel thread. In IcatOS, its priority must high enough!
* suggest: tasklet thread priority higher than kthread priority in linuxcompat
*/
#define TASKLET_THREAD_PRIORITY (LINUX_THREAD_PRIORITY-1)
#define TASKLET_THREAD_STACKDEP (4096-256)


/**************************************************************************
 *                          D A T A   T Y P E S
 **************************************************************************/
typedef void (*tasklet_func_t)(unsigned long);

struct tasklet_struct
{
	tasklet_func_t func;
	unsigned long  data;
	signed int     count;
	unsigned char  disabled;
};

/**************************************************************************
 *              F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
extern void tasklet_global_init(void);
extern void tasklet_init(struct tasklet_struct *t, tasklet_func_t func, unsigned long data);
extern void tasklet_schedule(struct tasklet_struct* t);

extern void tasklet_enable(struct tasklet_struct *t);
extern void tasklet_disable(struct tasklet_struct *t);
extern void tasklet_kill(struct tasklet_struct *t);

#define tasklet_hi_schedule tasklet_schedule
#define MAX_SCHEDULE_TIMEOUT (~0UL)

#ifdef __cplusplus
}
#endif

#endif

