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
 *  Author:                                                               *
 *                                                                        *
 **************************************************************************/
/* DO NOT EDIT the next 2 lines for auto revision by subversion
$Rev: 12606 $
$Date:: 2014-03-19 21:27:29#$
 */
#ifndef _PTHREAD_DEBUG_H_
#define _PTHREAD_DEBUG_H_

#ifdef  __cplusplus
extern  "C" {
#endif

#if defined(SPCA6330)
#if defined(SPCA6350)
#include "debug_mod/proflog.h"
#else
#include "middleware/proflog.h"
#endif
#endif

#define PTHREAD_DEBUG		0
#define PTHREAD_PROFID		3

#define FUNC_DEBUG		(0x01<<0)
#define MUTEX_DEBUG		(0x01<<1)
#define SEM_DEBUG			(0x01<<2)
#define THREAD_DEBUG	(0x01<<3)
#define BAR_DEBUG			(0x01<<4)
#define RWLOCK_DEBUG	(0x01<<5)
#define TIMER_DEBUG		(0x01<<6)
#define MQ_DEBUG			(0x01<<7)
#define ERROR_DEBUG		(0x01<<8)


#define PTHREAD_DBG_FLAGS (FUNC_DEBUG | MUTEX_DEBUG | SEM_DEBUG | MQ_DEBUG )


#if PTHREAD_DEBUG
	#define PTHREAD_PROFLOGPRINTF(args...) do { \
			profLogPrintf(PTHREAD_PROFID,args); \
	} while(0)
	#define PTHD_DBG(debug, message) do { \
		if ((debug) & PTHREAD_DBG_FLAGS) { \
			profLogPrintf(PTHREAD_PROFID,"[PTHD-%s:%d] ", __FUNCTION__, __LINE__); PTHREAD_PROFLOGPRINTF message ; } \
	} while(0)
#else
	#define PTHD_DBG(debug, message)
#endif /* PTHREAD_PROFLOG */



#ifdef  __cplusplus
}
#endif

#endif /* _PTHREAD_DEBUG_H_ */
