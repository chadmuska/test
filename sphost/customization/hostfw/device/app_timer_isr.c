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
#include "app_com_def.h"
#include "sp5k_global_api.h"
#include "app_timer.h"

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
/* multi slot timer isr service */
static UINT32 tmrIsrTickInterval=10; /* in ms, alex */

static UINT8 appTimerIsrInited=0;
typedef void (*appTimerIsrCallBack_t)(void);

#define APP_TIMER_ISR_SIZE	4
struct {
	appTimerIsrCallBack_t isr;
	UINT8 counter,max;
	UINT8 trigger;
} appTimerIsrTable[APP_TIMER_ISR_SIZE];


static void
appTimerIsr(
	void
)
{
	int i;
	for (i=0 ; i<APP_TIMER_ISR_SIZE ; i++) {
		if (appTimerIsrTable[i].isr && appTimerIsrTable[i].trigger==0) {
			appTimerIsrTable[i].counter++;
			if (appTimerIsrTable[i].counter >= appTimerIsrTable[i].max) {
				appTimerIsrTable[i].trigger = 1;
				(appTimerIsrTable[i].isr)();
				appTimerIsrTable[i].trigger = 0;
				appTimerIsrTable[i].counter = 0;
			}
		}
	}
}

/* !!! sp5kTimerIsrReg has one slot only, everyone can call this and trash our isr... */
static void
appTimerIsrInit(
	void
)
{
	memset(appTimerIsrTable, 0, sizeof(appTimerIsrTable));
	sp5kTimerIsrReg(0, tmrIsrTickInterval, appTimerIsr);
	appTimerIsrInited = 1;
}

UINT32
appTimerIsrReg(
	void *pfun,
	UINT8 numTick /* refer tmrIsrTickInterval for time interval per tick */
)
{
	int i;
	if (!appTimerIsrInited)
		appTimerIsrInit();
	for (i=0 ; i<APP_TIMER_ISR_SIZE ; i++) {
		if (appTimerIsrTable[i].isr==NULL) {
			appTimerIsrTable[i].isr = pfun;
			appTimerIsrTable[i].counter = 0;
			appTimerIsrTable[i].max = numTick;
			appTimerIsrTable[i].trigger = 0;
			return i;
		}
	}
	return TIMER_NULL;
}

void
appTimerIsrUnreg(
	UINT32 *ptimerid
)
{
	if (ptimerid) 
	{
		if (appTimerIsrTable[*ptimerid].isr) 
		{
			appTimerIsrTable[*ptimerid].isr = NULL;
			appTimerIsrTable[*ptimerid].counter = 0;
			appTimerIsrTable[*ptimerid].max = 0;
			appTimerIsrTable[*ptimerid].trigger = 0;
		}
		*ptimerid = TIMER_NULL;
	}
}

UINT32
appTimerIsrTickIntervalGet(
	void
)
{
	return tmrIsrTickInterval;
}
