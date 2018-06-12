/**************************************************************************
 *
 *       Copyright (c) 2012-2018 by iCatch Technology, Inc.
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
 *  File name    : App_audio_osd.c
 *  Function     : About audio information display on the osd.
                   This feature is a simple sample code, you can expand it.
 *  First editor :   
 *  First date   :   
 *  History      :
 *               : 2018.06.01  Edward.Lu  "Add description and modify code"
 **************************************************************************/

#include "app_com_def.h"
#include "app_com_api.h"

#include "sp5k_global_api.h"
#include "sp5k_ae_api.h"
#include "sp5k_capture_api.h"
#include "sp5k_disp_api.h"
#include "sp5k_disk_api.h"

/* for display osd */
#include "sp5k_gfx_api.h"
#include "app_pal.h"
#include "app_osd.h"

#include "sp5k_aud_api.h"
#include "sp5k_dcf_api.h"
#include "sp5k_media_api.h"
#include "sp5k_dzoom_api.h"
#include "gpio_custom.h"
#include "app_audio.h"
#include "app_still.h"
#include "app_view_osd.h"
#include "app_osdfont.h"
#include "app_osd_api.h"
#include "app_stringid.h"
#include "app_jpg_draw.h"
#include "app_battery.h"
//#include "app_user_setting.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

#define HOST_DBG 1

#define VOICE_MEMO_BAR_X					VOICE_MEMO_PROG_FRAME_CENT_X - (VOICE_MEMO_PROG_FRAME_SIZE_X / 2) + 37
#define VOICE_MEMO_BAR_Y					VOICE_MEMO_PROG_FRAME_CENT_Y - (VOICE_MEMO_PROG_FRAME_SIZE_Y / 2) + 107
#define VOICE_MEMO_BAR_SIZE_X				165
#define HDMI_VOICE_MEMO_BAR_SIZE_X			86
#define VOICE_MEMO_BAR_SIZE_Y				19

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
UINT16 LastProgBarLength;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern appAudioCB_t gAudioCB;
extern UINT16 AudioRemainTime;

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/*--------------------------------------------------------------------------*
* Function name    : appAudioOsdElsTimeShow
* Function         : show audio elapsed time(ElsTime)
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.06.01  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appAudioOsdElsTimeShow(
	void
)
{
	unsigned char 	szTime[16];

	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	#if (KIT_WITH_HDMI)
	if(IS_HDMI_IN){
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, APPVIEW_AUDIO_RECORDTIME_X-FontEngNumSize_X*6/2, 104,FontEngNumSize_X*6, FontEngNumSize_Y, DISABLE, DISABLE);
	}else
	#endif
	{
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, APPVIEW_AUDIO_RECORDTIME_X-FontEngNumSize_X*6/2, APPVIEW_AUDIO_RECORDTIME_Y,FontEngNumSize_X*6, FontEngNumSize_Y, DISABLE, DISABLE);
	}

	memset(szTime, 0, 16);
	sprintf(szTime, "%02u:%02u:%02u", gAudioCB.recSec/3600, (gAudioCB.recSec%3600)/60, (gAudioCB.recSec%3600)%60);

	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	if(gAudioCB.recSec/3600>99)
	{
		#if (KIT_WITH_HDMI)
		if(IS_HDMI_IN){
			appOsdLib_TextDraw(APPVIEW_AUDIO_RECORDTIME_X-30, 105, SP5K_GFX_ALIGN_TOP_LEFT, (UINT8 *)szTime);
		}else
		#endif
		{
		appOsdLib_TextDraw(APPVIEW_AUDIO_RECORDTIME_X-40-FontEngNumSize_X, APPVIEW_AUDIO_RECORDTIME_Y, SP5K_GFX_ALIGN_TOP_LEFT, (UINT8 *)szTime);
		}
	}
	else
	{
		#if (KIT_WITH_HDMI)
		if(IS_HDMI_IN){
			appOsdLib_TextDraw(APPVIEW_AUDIO_RECORDTIME_X-30, 105, SP5K_GFX_ALIGN_TOP_LEFT, (UINT8 *)szTime);
		}else
		#endif
		{
		appOsdLib_TextDraw(APPVIEW_AUDIO_RECORDTIME_X-40, APPVIEW_AUDIO_RECORDTIME_Y, SP5K_GFX_ALIGN_TOP_LEFT, (UINT8 *)szTime);
		}
	}
}

/*--------------------------------------------------------------------------*
* Function name    : appAudioOsdRecRemainingShow
* Function         : show remain time for audio use
* Return value	   : void
* Parameter1	   : remaining time (UINT32 unit second)
* History          :
*                  : 2018.06.01  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appAudioOsdRecRemainingShow(
	UINT32 remainingSec
)
{
	appViewOsd_TotalFreeTimeDraw(ENABLE, DISABLE, remainingSec);
}

/*--------------------------------------------------------------------------*
* Function name    : appAudioOsdDiskShow
* Function         : show disk on the osd for audio, you can expand it.
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.06.01  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appAudioOsdDiskShow(
	void
)
{

}

/*--------------------------------------------------------------------------*
* Function name    : appAudioOsd_BatteryDraw
* Function         : show battery on the osd for audio
* Return value	   : void
* Parameter1	   : bShow (UINT8 ENABLE/DISABLE)
* Parameter1	   : idx, battery level (UINT8 appBatteryLevelGet())
* History          :
*                  : 2018.06.01  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appAudioOsd_BatteryDraw(
	UINT8 bShow,
	UINT8 idx
)
{
	if(!bShow){
		appOsd_ColorDrawShapeColorSet(PAL_GRAY_1, PAL_BLEND_50, PAL_GRAY_1, PAL_BLEND_50);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, APPVIEW_BATTERY_X , APPVIEW_BATTERY_Y, APPVIEW_ICON_SIZE_W, APPVIEW_ICON_SIZE_H, 0, 0);
		return;
	}
	if (bShow) {
		appOsd_FontIconFontSet(ID_FONT_TK_ICON);
		switch(idx)	{
		case APP_BATT_LEVEL_0:
			appOsdLib_OsdLayIconDraw(APPVIEW_BATTERY_X,APPVIEW_BATTERY_Y,SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_BATTERY_LOW );
			break;
		case APP_BATT_LEVEL_1:
			appOsdLib_OsdLayIconDraw(APPVIEW_BATTERY_X,APPVIEW_BATTERY_Y,SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_BATTERY_HALF );
			break;
		case APP_BATT_LEVEL_2:
			appOsdLib_OsdLayIconDraw(APPVIEW_BATTERY_X,APPVIEW_BATTERY_Y,SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_BATTERY_FULL );
			break;
		case APP_BATT_LEVEL_3:
			//appOsdLib_OsdLayIconDraw(APPVIEW_BATTERY_X,APPVIEW_BATTERY_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_SNAPSHOTPREVIEW_BATTERY_3);
            HOST_PROF_LOG_PRINT(LEVEL_WARN, "adoosd: APP_BATT_LEVEL_3 is not used. %s (%d)",__FUNCTION__,__LINE__);
			break;
		default:
            HOST_PROF_LOG_PRINT(LEVEL_WARN, "adoosd: Illegal BATT Level%d %s (%d)",idx,__FUNCTION__,__LINE__);
			break;
		}
	}
}

/*--------------------------------------------------------------------------*
* Function name    : appAudioOsdProgressBarDraw
* Function         : show progress bar(CurrentTime_msec/TotalTime_msec) on the osd for audio
* Return value	   : void
* Parameter1	   : bShow (UINT8 ENABLE/DISABLE)
* Parameter1	   : CurrentTime_msec (UINT32)
* Parameter1	   : TotalTime_msec (UINT32)
* History          :
*                  : 2018.06.01  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appAudioOsdProgressBarDraw(
		UINT8 bShow,
		UINT32 CurrentTime_msec,
		UINT32 TotalTime_msec
)
{
	UINT32 CurrentProgBarLength;

	if(!bShow)
	{
		appOsd_ColorDrawShapeColorSet(PAL_GRAY_1, PAL_BLEND_50, PAL_GRAY_1, PAL_BLEND_50);
		#if (KIT_WITH_HDMI)
		if(IS_HDMI_IN){
			appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 121, 141, 84, 14, 0, 0);
		}else
		#endif
		{
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, VOICE_MEMO_BAR_X , VOICE_MEMO_BAR_Y, VOICE_MEMO_BAR_SIZE_X, VOICE_MEMO_BAR_SIZE_Y, 0, 0);
		}
		return;
	}

	if (CurrentTime_msec > TotalTime_msec)
		CurrentTime_msec = TotalTime_msec;

	if (CurrentTime_msec ==TotalTime_msec)
		CurrentTime_msec = TotalTime_msec;

	#if (KIT_WITH_HDMI)
	if(IS_HDMI_IN){
		CurrentProgBarLength = ((float)CurrentTime_msec / (float)TotalTime_msec) * (float)HDMI_VOICE_MEMO_BAR_SIZE_X;
	}else
	#endif
	{
		CurrentProgBarLength = ((float)CurrentTime_msec / (float)TotalTime_msec) * (float)VOICE_MEMO_BAR_SIZE_X;
	}

	appOsd_ColorDrawShapeColorSet(PAL_YELLOW, PAL_BLEND_100, PAL_YELLOW, PAL_BLEND_100);
	#if (KIT_WITH_HDMI)
	if(IS_HDMI_IN){
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 121 + LastProgBarLength, 141, CurrentProgBarLength - LastProgBarLength, 14, 0, 0);
	}else
	#endif
	{
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, VOICE_MEMO_BAR_X + LastProgBarLength, VOICE_MEMO_BAR_Y, CurrentProgBarLength - LastProgBarLength, VOICE_MEMO_BAR_SIZE_Y, 0, 0);
	}
	LastProgBarLength = CurrentProgBarLength;

}

/*--------------------------------------------------------------------------*
* Function name    : appAudioOsdStatusDraw
* Function         : draw audio status by video record status
* Return value	   : void
* Parameter1	   : Audiostatus (appAudioRecStatus_t)
* History          :
*                  : 2018.06.01  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appAudioOsdStatusDraw(UINT8 Audiostatus)
{
	/*Els time update*/
	appOsd_FontIconFontSet(ID_FONT_TK_ICON);
	appAudioOsdDiskShow();
	appAudioOsdElsTimeShow();

	appAudioOsd_BatteryDraw(TRUE, appBatteryLevelGet());
	appOsdLib_OsdLayIconDraw(APPVIEW_MODE_X, APPVIEW_MODE_Y,SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_MODE_VOICE );
	/*-- Center Background */
	appOsd_FontIconFontSet( RES_ICON_WARNM );
	appOsdLib_OsdLayIconDraw(VOICE_MEMO_PROG_FRAME_CENT_X,VOICE_MEMO_PROG_FRAME_CENT_Y,SP5K_GFX_ALIGN_CENTER,0x0000);
	#if (KIT_WITH_HDMI)
	if(IS_HDMI_IN){
		appOsd_FontIconFontSet(ID_FONT_TK_BIGICON);
	}else
	#endif
	{
	appOsd_FontIconFontSet(ID_FONT_TK_ICON);
	}

    {
	    /*RECORD TIME*/
	    appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		#if (KIT_WITH_HDMI)
		if(IS_HDMI_IN){
			UINT32 startx=104,starty=75,width=117,high=FONT_H;
			appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, startx, starty, width, high, DISABLE, DISABLE);
		}else
		#endif
		{
	        appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, APPVIEW_AUDIO_STRING_X-(APPVIEW_AUDIO_STRING_W/2), APPVIEW_AUDIO_STRING_Y, APPVIEW_AUDIO_STRING_W, FONT_H, DISABLE, DISABLE);
		}
	    appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		if(Audiostatus == AUDIO_REC_STATUS_STOP)
		{
			#if (KIT_WITH_HDMI)
			if(IS_HDMI_IN){
				UINT32 x=162,y=75;
				appOsdLib_TextIDDraw(x,y,SP5K_GFX_ALIGN_TOP,STRTK_RDY_TO_REC);
			}else
			#endif
			{
		        appOsdLib_TextIDDraw(APPVIEW_AUDIO_STRING_X,APPVIEW_AUDIO_STRING_Y,SP5K_GFX_ALIGN_TOP,STRTK_RDY_TO_REC);
			}
		}
		else if(Audiostatus== AUDIO_REC_STATUS_RECORDING)
		{
			#if (KIT_WITH_HDMI)
			if(IS_HDMI_IN){
				UINT32 x=161,y=75;
				appOsdLib_TextIDDraw(x,y,SP5K_GFX_ALIGN_TOP,STRTK_RECORDING_MEMO);
			}else
			#endif
			{
		        appOsdLib_TextIDDraw(APPVIEW_AUDIO_STRING_X,APPVIEW_AUDIO_STRING_Y,SP5K_GFX_ALIGN_TOP,STRTK_RECORDING_MEMO);
	        }
        }
	}
}

/*--------------------------------------------------------------------------*
* Function name    : appAudioOsdShow
* Function         : Show audio status on the osd by clr control.
* Return value	   : void
* Parameter1	   : clr control flag
  (UINT32 ENABLE:must show / DISABLE:no show on AUDIO_REC_STATUS_STOP)
* History          :
*                  : 2018.06.01  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appAudioOsdShow(
	UINT32 clr
)
{
	if (clr)
	{
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		#if (KIT_WITH_HDMI)
		if(IS_HDMI_IN){
			UINT32 startx=100,starty=75,width=117,high=FONT_H;
			appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, startx, starty, width, high, DISABLE, DISABLE);
		}else
		#endif
		{
			appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, APPVIEW_AUDIO_STRING_X-(APPVIEW_AUDIO_STRING_W/2), APPVIEW_AUDIO_STRING_Y, APPVIEW_AUDIO_STRING_W, FONT_H, 0, 0);
		}
		appAudioOsdStatusDraw(gAudioCB.recStatus);
	}

	if (gAudioCB.recStatus == AUDIO_REC_STATUS_STOP)
	{
		appAudioOsdDiskShow();
	//	appViewOsd_WarmingMsgDraw(1, gAudioCB.err, TRUE);
	}
	else
	{
		appAudioOsdStatusDraw(gAudioCB.recStatus);
		appAudioOsdProgressBarDraw(TRUE,gAudioCB.recSec,AudioRemainTime);
	}
}

/*--------------------------------------------------------------------------*
* Function name    : appAudioOsdInit
* Function         : get FuncTbl by selector
                     If pfuncTbl is null, will malloc.
* Return value	   : void
* Parameter1	   : elapsed time (UINT32 5s/10s/20s (unit second))
* History          :
*                  : 2018.06.01  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appAudioOsdInit(
	void
)
{
	/*-- Background JPG */
	UINT32 x=0,y=0,w=320,h=240;
	appJpgDraw_ImgLayer(RES_JPG_AUDIO_BG, x, y, w, h);
}
