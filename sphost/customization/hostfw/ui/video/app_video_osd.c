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
 *  File name    : App_video_osd.c
 *  Function     : about video osd draw 		   
 *  First editor :   
 *  First date   :   
 *  History      :
 *               : 2018.05.28  Edward.Lu  "Add description and modify code"
 **************************************************************************/

#include "app_com_def.h"
#include "app_com_api.h"

#include "sp5k_global_api.h"
#include "sp5k_ae_api.h"
#include "sp5k_capture_api.h"
#include "sp5k_disp_api.h"
#include "sp5k_disk_api.h"
#include "sp5k_gfx_api.h"
#include "sp5k_aud_api.h"
#include "sp5k_dcf_api.h"
#include "sp5k_media_api.h"
#include "sp5k_dzoom_api.h"

#include "app_pal.h"
#include "app_osd.h"
#include "app_still.h"
#include "app_video.h"
#include "app_user_setting.h"
#include "app_view_param.h"
#include "app_osd_api.h"
#include "app_view_osd.h"
#include "app_battery.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define HOST_DBG 0

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT8 isSensorResume;
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
extern BOOL appInfoDiskSizeCheck(void);
#endif
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/*--------------------------------------------------------------------------*
* Function name    : appVideoOsdElsTimeShow
* Function         : Show ElsTime unit second
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	

void
appVideoOsdElsTimeShow(
	void
)
{
	appViewOsd_VideoTotalFreeTimeDraw(TRUE,gVideoCB.CapSec);
}

/*--------------------------------------------------------------------------*
* Function name    : appVideoOsdDiskShow
* Function         : Show Disk FreeTime
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	

void
appVideoOsdDiskShow(
	BOOL bShow
)
{
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	if(bShow && gVideoCB.isDiskReady&& (gVideoCB.err==VIEW_ERR_NO_ERROR)&&appInfoDiskSizeCheck()) 
#else
	if(bShow && gVideoCB.isDiskReady&& (gVideoCB.err==VIEW_ERR_NO_ERROR)) 
#endif
	{
		appViewOsd_TotalFreeTimeDraw(TRUE, FALSE, appVideoRemainSecGet());
	}
	else {
		appViewOsd_TotalFreeTimeDraw(FALSE, FALSE, FALSE);
	}

}

/*--------------------------------------------------------------------------*
* Function name    : appVideoOsdShow
* Function         : Video Osd Show about osd draw
* Return value	   : void
* Parameter1	   : clean (UINT8 clean all osd)
* Parameter2	   : outInOsdFreeze (UINT8 1:Freeze osd/0:Refresh osd)
* Parameter3	   : switch osd-type (UINT8 0~5)
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	

void
appVideoOsdShow(
	UINT8 clean,
	UINT8 outInOsdFreeze,
	UINT8 osd
)
{
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	return;
#endif
	if (gVideoCB.fCapture==0)
	{
		APP_OSD_REFRESH_OFF;
		if(clean)
		{
			appViewOsd_AllClean();
		}

        #if SP5K_MULTI_FAST_BOOT
        static UINT8 MULTI_FAST_BOOT = TRUE;
        if(MULTI_FAST_BOOT)
        {
            MULTI_FAST_BOOT = FALSE;
            appOsdResPreload();
        }
        #endif

		appViewOsd_ZoomBarDestroy();

		#if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
        #if !SP5K_USB_UVC_SUPPORT
		appViewOsd_SensorDraw(TRUE, isSensorResume);
        #endif
        appViewOsd_DualModeDraw();
		#endif
		//osd-type
		switch(osd)
		{
			case OSD_VIEW_S_ONE_HOLD:
				// this is not effect, the app_still_aaa will call appViewOsdShow();
				// Then appViewOsdShow() will check if this is video mode.
				break;
			case OSD_VIEW_VERBOSE:
				appViewOsd_WBModeDraw(TRUE, pViewParam->wb);
				appViewOsd_MeteringDraw(TRUE, pViewParam->metering);
				break;
			case OSD_VIEW_SIMPLE:
				
				appViewOsd_GuideLineDraw(FALSE);
				appViewOsd_ViewModeDraw(TRUE, gStillCB.cap.CurViewMode, gStillCB.cap.CurScene);
				appViewOsd_PicSizeVideoDraw(TRUE,pViewParam->videoSize);
				appViewOsd_VideoQtyDraw(TRUE, pViewParam->videoQuality);
			/*  Isaac change 20161209*/
			#if defined(PRJ_PDVR)
				UINT32 text_x=10,text_y=210;
				appOsdLib_TextDraw(text_x, text_y, SP5K_GFX_ALIGN_TOP_LEFT, (UINT8 *)CUSTOMERVER);
				appViewOsd_BatteryDraw(FALSE, appBatteryLevelGet());
			#else
				appViewOsd_BatteryDraw(TRUE, appBatteryLevelGet());
			#endif
				appVideoOsdDiskShow(TRUE);
				break;
			case OSD_VIEW_NONE:
				appViewOsd_WBModeDraw(FALSE,FALSE);
				appViewOsd_MeteringDraw(FALSE,FALSE);
				appViewOsd_GuideLineDraw(FALSE);
				appViewOsd_ViewModeDraw(FALSE, FALSE, FALSE);
				appViewOsd_PicSizeVideoDraw(FALSE,FALSE);
				appViewOsd_VideoQtyDraw(TRUE, pViewParam->videoQuality);
				appViewOsd_FocusModeDraw(FALSE,FALSE);
				appViewOsd_BatteryDraw(FALSE,FALSE);
				appVideoOsdDiskShow(FALSE);
				appViewOsd_AllClean();
				break;
			case OSD_VIEW_GRID:
				appViewOsd_GuideLineDraw(TRUE);
				break;
			default:
				break;
		}
		//post-osd
		appViewOsd_FocusWindowDraw(TRUE, FOCUS_STATUS_VIEW);
		if(gVideoCB.err > VIEW_ERR_NO_ERROR)
		{
			APP_OSD_REFRESH_ON;
			appViewOsd_WarmingMsgDraw(TRUE, gVideoCB.err, FALSE);
		}

		if(osd==OSD_VIEW_VERBOSE && gVideoCB.err <= VIEW_ERR_NO_ERROR)
		{
			appViewOsd_HistogramCreat();
		}
		else
		{
			appViewOsd_HistogramDestroy();
		}

        HOST_PROF_LOG_ADD(LEVEL_DEBUG, "vosd: refresh s");
		if(!outInOsdFreeze)
		{
			APP_OSD_REFRESH_ON;
		}
        HOST_PROF_LOG_ADD(LEVEL_DEBUG, "vosd: refresh e");
	}
	else
	{
		//appOsd_GfxStringDraw(20, 200, SP5K_GFX_ALIGN_BOTTOM_LEFT,gVideoCB.fPause? "SH1: Stop, UP: Record":"SH1: Stop, UP: Pause");
		appVideoOsdElsTimeShow();
	}
}

