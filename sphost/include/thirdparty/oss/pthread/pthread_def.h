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
#ifndef _PTHEAD_DEF_H_
#define _PTHEAD_DEF_H_

#ifdef  __cplusplus
extern  "C" {
#endif

#include "common.h"
#include <sys/types.h> 
#if defined(SPCA6330) && !defined(SPCA6350)
#include <stdio.h>
#endif
/*   wait options */
#define PTHEAD_NO_WAIT      1
#define PTHEAD_WAIT_FOREVER 0xffffffff


#define PTHREAD_AUTO_START       1


#define PTHREAD_CANCEL_ENABLE       0  
#define PTHREAD_CANCEL_DISABLE      1  

#define PTHREAD_CANCEL_ASYNCHRONOUS 0 
#define PTHREAD_CANCEL_DEFERRED     1 

#define PTHREAD_CANCELED            ((void *) 1)  
#define PTHREAD_BARRIER_SERIAL_THREAD -1

#define PTHREAD_CREATE_JOINABLE 		0/* Default */
#define PTHREAD_CREATE_DETACHED 		1


#define PTHREAD_ONCE_INIT 0

struct sched_param{
  UINT32 sched_priority;
};

typedef struct
{
	int detachstate;
	int schedpolicy;
	struct sched_param param;
	int  inheritsched;
	int scope;
	int guardsize;
	int stackaddr_set;
	void * stackaddr;
	int stacksize;
	char *sz_name;
	UINT32 slice;
	UINT32 autostart;
}pthread_attr_t;

union sigval
{
	int sival_int; 
	void *sival_ptr; 
};

typedef struct sigevent
{
	int sigev_notify; /*notification type*/
	int sigev_signo; /*signal number*/
	union sigval sigev_value; /*signal value*/
	void (*sigev_notify_function)(union sigval);
	pthread_attr_t *sigev_notify_attributes;
}sigevent_t;

typedef enum _signal_e{
	SIGSTOP = 1,
	SIGTSTP = 2,
	SIGTERM = 3,
	SIGQUIT = 4,
	SIGINT  = 5,
	SIGCONT = 6,
}signal_e;

#ifdef  __cplusplus
}
#endif

#endif /* _PTHEAD_DEF_H_ */
