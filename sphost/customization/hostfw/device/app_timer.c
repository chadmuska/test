/**************************************************************************
 *
 *       Copyright (c) 2006-2018 by iCatch Technology, Inc.
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
#define HOST_DBG 0
#include "app_com_def.h"
#include "app_com_api.h"
#include "sp5k_global_api.h"


/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/* ========================================================= */
/* Extended timer service */

static UINT32 appTimerMask=0;

#if HOST_DBG
static char *appTimerOwner[SP5K_TIMER_MAX];
#define SET_TIMER_OWNER(timer,str)	(appTimerOwner[(timer)] = (str))
#define GET_TIMER_OWNER(timer)		appTimerOwner[(timer)]
#else
#define SET_TIMER_OWNER(timer,str)
#define GET_TIMER_OWNER(timer)		""
#endif

UINT32 
appTimerSet(
	UINT32 delayMs,
	char *owner
)
{
	UINT32 timer,delay,enable;
    /*SP5K_TIMER_ID_0 reserve for video date stamp*/
	for (timer=SP5K_TIMER_ID_1 ; timer<=SP5K_TIMER_MAX; timer++) 
	{
		sp5kTimerCfgGet(timer, &delay, &enable);
		if (!enable) 
		{
			sp5kTimerCfgSet(timer, delayMs);
			sp5kTimerEnable(timer, 1);
			appTimerMask |= (1<<timer);
			SET_TIMER_OWNER(timer, owner);
			DBG_PRINT("timer: set %d %s(%d)\n", timer, owner, delayMs);
			return timer;
		}
	}
    HOST_PROF_LOG_PRINT(LEVEL_ERROR,"timer: No avaiable timer");
	return TIMER_NULL; /* no available timer */
}

void 
appTimerClear(
	UINT32 *ptimer
)
{
	DBG_ASSERT(ptimer!=NULL);
	if (*ptimer<SP5K_TIMER_MAX) 
	{
		if ((appTimerMask & (1<<(*ptimer))) ) 
		{
			sp5kTimerEnable(*ptimer, 0);
			DBG_PRINT("timer: appTimer:clear %d\n", *ptimer);
			appTimerMask &= ~(1<<(*ptimer));
		}
		*ptimer = TIMER_NULL;
	}
}

void
appTimerDump(
	void
)
{
	UINT32 i,mask;
	HOST_PROF_LOG_PRINT(LEVEL_INFO,"timer: Active appTimer");
	for (i=0,mask=1 ; i<SP5K_TIMER_MAX ; i++,mask<<=1) 
	{
		if (appTimerMask & mask)
		{
			HOST_PROF_LOG_PRINT(LEVEL_INFO,"timer: %d owner=%s", i, GET_TIMER_OWNER(i));
	}
}
}

void 
appTimeDelay(
	UINT32 delayMs,
	UINT32 fCleanMsg
)
{

sp5kTimeDelay(SP5K_TIME_DELAY_1MS, delayMs);

#if 0
	if (fCleanMsg)
	{
		sp5kTimerCfgSet(0, delayMs);
		sp5kTimerEnable(0, 1);
		appHostMsgWaitExact(SP5K_MSG_TIMER, 0, 2000);
		sp5kTimerEnable(0, 0);
	} 
	else 
	{
		sp5kTimeDelay(SP5K_TIME_DELAY_1MS, delayMs);
	}
#endif
}

