/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2009-2014 by iCatch Technology Co., Ltd.         *
 *                                                                        *
 *  This software is copyrighted by and is the property of iCatch         *
 *  Technology Co., Ltd. All rights are reserved by iCatch Technology     *
 *  Co., Ltd. This software may only be used in accordance with the       *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice MUST not be removed or modified without prior   *
 *  written consent of iCatch Technology Co., Ltd.                        *
 *                                                                        *
 *  iCatch Technology Co., Ltd. reserves the right to modify this         *
 *  software without notice.                                              *
 *                                                                        *
 *  iCatch Technology Co., Ltd.                                           *
 *  19-1, Innovation First Road, Science-Based Industrial Park,           *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 *  Author: Timothy Wu                                                    *
 *  Author: Anmin Deng                                                    *
 *  Author: Philip Lin                                                    *
 *                                                                        *
 **************************************************************************/
/* DO NOT EDIT the next 2 lines for auto revision by subversion
$Rev: 12606 $
$Date:: 2014-03-19 21:27:29#$
 */
#ifndef _PTHEAD_TIMER_H_
#define _PTHEAD_TIMER_H_

#ifdef  __cplusplus
extern  "C" {
#endif


#include "common.h"
#include "pthread_def.h"

#if __GNUC__ <= 3
typedef UINT32 timer_t;

typedef enum _clockid_t{
	CLOCK_REALTIME =1,
	CLOCK_MONOTONIC,
	CLOCK_PROCESS_CPUTIME_ID,
	CLOCK_THREAD_CPUTIME_ID,
	CLOCK_REALTIME_HR,
	CLOCK_MONOTONIC_HR,
}clockid_t;
#endif


int timer_create(clockid_t which_clock, struct sigevent* timer_event_spec, timer_t* timer_id);
int timer_settime(timer_t timerid, int flags, const struct itimerspec *value, struct itimerspec *ovalue);
int timer_gettime(timer_t timerid, struct itimerspec* value);
int timer_delete(timer_t timerid);
int timer_getoverrun(timer_t timerid);
int nanosleep(const struct timespec *req, struct timespec *rem);
int clock_settime (clockid_t clock_id, const struct timespec *tp);
int clock_gettime (clockid_t clock_id, struct timespec *tp);
int clock_getres (clockid_t clock_id, struct timespec *res);  



#ifdef  __cplusplus
}
#endif

#endif /*_PTHEAD_TIMER_H_*/
