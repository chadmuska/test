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
#include "sp5k_disp_api.h"
#include "sp5k_gfx_api.h"
#include "sp5k_capture_api.h"

#include "app_timer.h"
#include "app_still.h"
#include "app_view_param.h"
#include "app_osd_api.h"
#include "app_view_osd.h"
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
static UINT32 StillBusyStateTimer = TIMER_NULL;
static UINT32 StillBusyStateCourter = 0;
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void
appStillBusyStateInit(void)
{	
	StillBusyStateTimer = appTimerSet(250, "Still Busy Timer");		
	StillBusyStateCourter = 0;
}

void
appStillBusyStateClose(void)
{	
	appTimerClear(&StillBusyStateTimer);
}

void 
appStillBusyStateOnMsg(
	UINT32 msg,
	UINT32 param
)
{
	switch (msg) {
	case SP5K_MSG_TIMER: 		
		if (param == StillBusyStateTimer) 
		{
			StillBusyStateCourter++;
			if(IS_PASM_PMODE) 
			{
				appViewOsd_RemainNumDraw(ViewOsdPresentStatus&VIEW_OSD_PRESENT_REMAIN_IMAGE_NUMBER? FALSE : TRUE , 0, 0);	
			}
			else if(IS_VIDEO_MODE) 
			{
				appViewOsd_TotalFreeTimeDraw(ViewOsdPresentStatus&VIEW_OSD_PRESENT_VIDEO_REMAIN_TIME? FALSE : TRUE , 0, 0);	
			}	
			
			appStillBusyStateClose();
			appStateChange(appPreviousStateGet(), APP_STATE_MSG_INIT);
		}
		break;	
	default:
		break;
	}	
}

void 
appStillBusyStateOnKey(
	UINT32 msg,
	UINT32 param
)
{	
}

void 
appStillBusyState(
	UINT32 msg,
	UINT32 param
)
{
	switch (msg) 
	{
	case APP_STATE_MSG_INIT:
		RPrintf("appStillBusyState\n");
		appStillBusyStateInit();
		break;		
	case APP_STATE_MSG_CLOSE:
		appStillBusyStateClose();
		appStateCloseDone();
		break;			
	default:
		if (IS_APP_KEY_MSG(msg)) 
		{
			appStillBusyStateOnKey(msg,param);
		}
		else
		{
			appStillBusyStateOnMsg(msg,param);
		}
		break;
	}
}

