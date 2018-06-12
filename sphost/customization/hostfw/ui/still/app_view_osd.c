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
#include "common.h"
#include <stdio.h>
#include "app_com_api.h"
#include "app_com_def.h"

#include "sp5k_disp_api.h"
#include "sp5k_gfx_api.h"
#include "sp5k_disk_api.h"
#include "sp5k_rsvblk_api.h"
#include "sp5k_cdsp_api.h"
#include "sp5k_global_api.h"
#include "sp5k_modesw_api.h"

#include "app_osd.h"
#include "app_still.h"
#include "app_view_param.h"
#include "app_osd_api.h"
#include "app_osdfont.h"
#include "app_battery.h"
#include "app_view_osd.h"
#include "app_jpg_draw.h"
#include "app_error_msg.h"
#include "app_stringid.h"
#include "app_audio.h"
#include "app_face_detect_ex.h"
#include "app_video.h"
#include "app_tvlcd.h"
#include "app_audio.h"
#include "app_playback_def.h"
#include "app_playback_osd.h"

#if SP5K_USB_UVC_SUPPORT
#include "app_video_uvc.h"
#endif

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
static UINT32 DlgConfirmYesNo[2] = {STRTK_YES, STRTK_NO};

UINT16 QualityIcon[] = {
	ID_ICON_QUALITY_SUPER_FINE,
	ID_ICON_QUALITY_FINE,
	ID_ICON_QUALITY
};

UINT16 PasmIcon[] = {
	ID_ICON_PASM_PROGRAM,
	ID_ICON_PASM_APERTURE,
	ID_ICON_PASM_TV,
	ID_ICON_PASM_MANUAL,
};

UINT8* aperturevValueStr[7][2] = {
	{(UINT8 *)"F2.8", (UINT8 *)"F5.6"},
	{(UINT8 *)"F3.1", (UINT8 *)"F6.3"},
	{(UINT8 *)"F3.4", (UINT8 *)"F6.9"},
	{(UINT8 *)"F3.8", (UINT8 *)"F7.6"},
	{(UINT8 *)"F4.2", (UINT8 *)"F8.5"},
	{(UINT8 *)"F4.7", (UINT8 *)"F9.5"},
	{(UINT8 *)"F5.2", (UINT8 *)"F10.4"}
};

UINT8* shutterSpeedValueStr[] = {
	(UINT8 *)"AUTO",
	(UINT8 *)"16.0",
	(UINT8 *)"8.0",
	(UINT8 *)"4.0",
	(UINT8 *)"2.0",
	(UINT8 *)"1.0",
	(UINT8 *)"1/2",
	(UINT8 *)"1/4",
	(UINT8 *)"1/8",
	(UINT8 *)"1/15",
	(UINT8 *)"1/30",
	(UINT8 *)"1/60",
	(UINT8 *)"1/125",
	(UINT8 *)"1/250",
	(UINT8 *)"1/500",
	(UINT8 *)"1/1000",
	(UINT8 *)"1/2000",
	(UINT8 *)"1/4000",
	(UINT8 *)"1/8000",
	(UINT8 *)"MAX",
};

UINT8* isoValueStr[UI_ISO_MAX] = {
	[UI_ISO_50] 	= (UINT8 *)"ISO 50",
	[UI_ISO_100] 	= (UINT8 *)"ISO 100",
	[UI_ISO_200] 	= (UINT8 *)"ISO 200",
	[UI_ISO_400] 	= (UINT8 *)"ISO 400",
	[UI_ISO_800] 	= (UINT8 *)"ISO 800",
};

UINT32 isoValue[UI_ISO_MAX] = {
	[UI_ISO_50] 	= 50,
	[UI_ISO_100] 	= 100,
	[UI_ISO_200] 	= 200,
	[UI_ISO_400] 	= 400,
	[UI_ISO_800] 	= 800,
	[UI_ISO_1600]	= 1600,
};


typedef struct {
	UINT32 status;
	UINT32 timer;
} viewosdobj_t;

viewosdobj_t gHistogram = {
	.status=OSD_OBJ_DESTROYED,
	.timer=TIMER_NULL,
};

UINT32 HistogramBuf[256];
UINT32 ZoomBarCounter = TIMER_NULL;
UINT32 ViewOsdPresentStatus = 0;

#if !SP5K_PVQV_DISABLE
static UINT8 PvQV_SHOW_FLAG = 0xff;
static UINT8 PvQV_ENABLE_FLAG = 0xff;
#endif

#define ID_ICON_DIGITAL_ZOOM	0x0000
#define ID_ICON_OPTICAL_ZOOM	0x0001

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern appAudioCB_t gAudioCB;
extern UINT8 isSensorResume;

void appPlayOsd_DlgBoxConfig(UINT32 titleStrId,UINT32 numOptions,UINT32 *poptionStrID,UINT32 actIndex,SINT32 yOffset);

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void appViewOsd_ASMChangeDraw(void);

void
appViewOsdValidAreaShow(
	UINT16 imgw,
	UINT16 imgh
)
{
	#if KIT_SENSOR_FIT_IMAGE

	#else
	// use pf layer to cover unused area instead of sp5k size change
	static UINT32 h0=0;
	UINT32 h, pfw=LCD_SIZE_X, pfh=LCD_SIZE_Y;

	DBG_PRINT("sosd: %s: iw=%d, ih=%d, h0=%d\n\n", __func__, imgw, imgh, h0);

	if(imgw==0) imgw=LCD_SIZE_X;
	if(imgh==0) imgh=LCD_SIZE_Y;

	if(imgw*300 < imgh*400*1.1 && imgw*300 > imgh*400*0.9 && h0==0)
	{
		return;
	}

	switch(appCurrDispModeGet())
	{
		case APP_DISP_TV_NTSC:
			pfw=OSD_DEFAULT_FRAMSIZE_W;
			pfh=OSD_DEFAULT_FRAMSIZE_H;
			break;
		case APP_DISP_TV_PAL:
			pfw=OSD_DEFAULT_FRAMSIZE_W;
			pfh=OSD_DEFAULT_FRAMSIZE_H;
			break;
		case APP_DISP_LCD:

			break;
		default:
			break;
	}

	h = (pfh - pfh * (4 * imgh) / (3 * imgw) ) / 2;
	h = h>0? h+1 : h; //only compensate the differ from capture.

	sp5kGfxAttrSet(SP5K_GFX_REFRESH_ACTIVE, 0, 0, 0, 0);
	if (h0>h)
	{
		// clean some area
		appOsdLib_PFLayerShapeClear(APP_OSD_SHAPE_RECT, 0, h, pfw, h0, 0, 0);
		appOsdLib_PFLayerShapeClear(APP_OSD_SHAPE_RECT, 0, pfh-h0, pfw, pfh-h, 0, 0);
	}
	else if (h != 0)
	{
		// draw area
		appOsd_ColorDrawShapeColorSet(PAL_BLACK, PAL_BLEND_100, PAL_BLACK, PAL_BLEND_100);
		appOsdLib_PFLayerShapeDraw(APP_OSD_SHAPE_RECT, 0, 0, 	 pfw, h, 0, 0);
		appOsdLib_PFLayerShapeDraw(APP_OSD_SHAPE_RECT, 0, pfh-h, pfw, pfh, 0, 0);
	}
	sp5kGfxAttrSet(SP5K_GFX_REFRESH_ACTIVE, 1, 0, 0, 0);
	h0 = h;

	#endif
	/*e Modify by Aries 10/06/22*/
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_SensorDraw
 *  Description :
 *------------------------------------------------------------------------*/
void
appViewOsd_SensorDraw(
	UINT8 bShow,
	UINT8 isResume
)
{
	/*UINT32 pmode;*/
    UINT32 osdchk;
 	/*
	sp5kModeGet(&pmode);
    	osdchk = (pmode==SP5K_MODE_STILL_PREVIEW) ? gStillCB.osd : gVideoCB.osd;
	*/
	/*Change to use active state to replace the sp5kModeGet.
	The sp5kModeGet will be halt because the 5kmode switch failed(because serdes power on failed at this time).*/
    osdchk = (appActiveStateGet()==APP_STATE_STILL_PREVIEW) ? gStillCB.osd : gVideoCB.osd;

	if (bShow) {
		if (isResume && (osdchk <= OSD_VIEW_VERBOSE)) {
			appOsdLib_OsdLayIconDraw(APPVIEW_SENSOR_X, APPVIEW_SENSOR_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_SLIDESHOW_TRANSITION3);
			appBtnEnable(BTN_ALL);
		}
		else if(!isResume){
			appOsdLib_OsdLayIconDraw(APPVIEW_SENSOR_X, APPVIEW_SENSOR_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_SLIDESHOW_TRANSITION1);
		}
	}
    else
    {
        appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
    	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, APPVIEW_SENSOR_X, APPVIEW_SENSOR_Y, APPVIEW_ICON_SIZE_W, APPVIEW_ICON_SIZE_H, 0, 0);
    }
}

#if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_DualModeDraw
 *  Description :
 *------------------------------------------------------------------------*/
void
appViewOsd_DualModeDraw()
{
	UINT32 pmode;
    UINT32 osdchk;
    #if SP5K_USB_UVC_SUPPORT
    sp5kModeGet(&pmode);
    if((pmode != SP5K_MODE_VIDEO_PREVIEW)
        || IS_MENU_STATE(appActiveStateGet())
       )
    {
        return;
    }
    if(appActiveStateGet() == APP_STATE_POWER_OFF || appActiveStateGet() == APP_STATE_VIDEO_REC)
    {
        APP_OSD_REFRESH_OFF;
        CLEAR_OSD_SRC;
        APP_OSD_REFRESH_ON;
        return;
    }
    #endif
    appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
    appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, APPVIEW_DUAL_X, APPVIEW_DUAL_Y, (FONT_W*5)+5, FONT_H, 0, 0);
    appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
#if SP5K_USB_UVC_SUPPORT
        if (appUvcCarModeGet())
        {
            appOsdLib_TextDraw(APPVIEW_DUAL_X, APPVIEW_DUAL_Y, SP5K_GFX_ALIGN_TOP_LEFT, (UINT8 *)"Back");
            return;
        }
    #endif
	sp5kModeGet(&pmode);
    osdchk = (pmode==SP5K_MODE_STILL_PREVIEW) ? gStillCB.osd : gVideoCB.osd;
    if(osdchk <= OSD_VIEW_VERBOSE)
    {
        if((pmode == SP5K_MODE_VIDEO_PREVIEW)||(pmode == SP5K_MODE_STILL_PREVIEW)||(pmode == SP5K_MODE_STANDBY))
        {
            if (appDualPipModeGet()== DUALSEN_SINGLE_MAIN)
            {
                appOsdLib_TextDraw(APPVIEW_DUAL_X, APPVIEW_DUAL_Y, SP5K_GFX_ALIGN_TOP_LEFT, (UINT8 *)"Front");
            }
            else if (appDualPipModeGet() == DUALSEN_SINGLE_SEC)
            {
                appOsdLib_TextDraw(APPVIEW_DUAL_X, APPVIEW_DUAL_Y, SP5K_GFX_ALIGN_TOP_LEFT, (UINT8 *)"Rear");
            }
            else
            {
                appOsdLib_TextDraw(APPVIEW_DUAL_X, APPVIEW_DUAL_Y, SP5K_GFX_ALIGN_TOP_LEFT, (UINT8 *)"Dual");
            }
        }
    }
}
#endif
/*-------------------------------------------------------------------------
 *  File Name : appViewOsdDiskShow
 *------------------------------------------------------------------------*/
void
appViewOsdDiskShow(
	void
)
{
	UINT32 n, dskFree;

	if(appStillBackgroundGetFree(&dskFree)==SUCCESS && gStillCB.isDiskReady)
	{
		/*
		This will ready at last save progress message received. In this moment
		we will also sync the diskSpaceInImgId will dskFree.
		*/

		gStillCB.diskSpaceInImgId = dskFree;
		if(gStillCB.estFileSize>0)
		{
			n = dskFree/gStillCB.estFileSize;
			appViewOsd_RemainNumDraw(1, 0, n);
		}
		else
		{
            HOST_PROF_LOG_ADD(LEVEL_ERROR, "sosd: est file size0 =0");
		}
	}
	else if(gStillCB.isDiskReady)
	{
		/*
		This is for fast update the disk space in image number. The diskSpaceInImgId
		will update in every raw image id ready message.
		*/

		if(gStillCB.estFileSize>0)
		{
			n = gStillCB.diskSpaceInImgId/gStillCB.estFileSize;
			appViewOsd_RemainNumDraw(1, 0, n);
		}
		else
		{
            HOST_PROF_LOG_ADD(LEVEL_ERROR, "sosd: est file size1 =0");
		}
	}
	else
	{
        HOST_PROF_LOG_ADD(LEVEL_WARN, "sosd:can not get disk info for OSD");
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appViewOsd_PicSizeDraw
 *------------------------------------------------------------------------*/
void
appViewOsd_PicSizeDraw(
	UINT8 bShow,
	UINT8 nPicSize
)
{
	//static UINT8 lastRecord=0;
	UINT32 id;

	if (bShow)
	{
		//check
		static UINT8 lastRecord=0;
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_SIZE && lastRecord==nPicSize)
		{
			return;
		}
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_SIZE && lastRecord!=nPicSize)
		{
			appViewOsd_PicSizeDraw(FALSE, 0);
		}
		//record
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_SIZE;
		lastRecord = nPicSize;

		//draw
		appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_1);
        appStill_GetCurSizeIcon(&id);

        #if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
        if ((appDualPipModeGet() == DUALSEN_SINGLE_SEC)||(appDualPipModeGet() == DUALSEN_PIP_SEC))
        {
            if(id >= ID_ICON_RESOLUTION_3M){
                    appOsdLib_OsdLayIconDraw(APPVIEW_PICSIZE_X,APPVIEW_PICSIZE_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_RESOLUTION_2M);
            }
            else{
                    appOsdLib_OsdLayIconDraw(APPVIEW_PICSIZE_X,APPVIEW_PICSIZE_Y,SP5K_GFX_ALIGN_TOP_LEFT,id);
            }
        }
        else{/*DUALSEN_SINGLE_MAIN or else*/
		appOsdLib_OsdLayIconDraw(APPVIEW_PICSIZE_X,APPVIEW_PICSIZE_Y,SP5K_GFX_ALIGN_TOP_LEFT,id);
	}
        #else
        appOsdLib_OsdLayIconDraw(APPVIEW_PICSIZE_X,APPVIEW_PICSIZE_Y,SP5K_GFX_ALIGN_TOP_LEFT,id);
        #endif
    }
	else if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_SIZE)
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_SIZE;
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,APPVIEW_PICSIZE_X, APPVIEW_PICSIZE_Y,APPVIEW_ICON_SIZE_W,APPVIEW_ICON_SIZE_H,0,0);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appViewOsd_FlashModeDraw
 *------------------------------------------------------------------------*/
void
appViewOsd_FlashModeDraw(
	UINT8 bShow,
	UINT8 nFlashMode
)
{
	//static UINT8 lastRecord;

	if (bShow)
	{
		//check
		static UINT8 lastRecord;
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_FLASH&& lastRecord==nFlashMode)
		{
			return;
		}
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_FLASH&& lastRecord!=nFlashMode)
		{
			appViewOsd_FlashModeDraw(FALSE, 0);
		}
		//record
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_FLASH;
		lastRecord = nFlashMode;
		appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_1);
		switch(nFlashMode)
		{
			case UI_FLASH_AUTO:
				appOsdLib_OsdLayIconDraw(APPVIEW_FLASH_X,APPVIEW_FLASH_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_FLASH_AUTO);
				break;
			case UI_FLASH_OFF:
				appOsdLib_OsdLayIconDraw(APPVIEW_FLASH_X,APPVIEW_FLASH_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_FLASH_OFF);
				break;
			case UI_FLASH_ON:
				appOsdLib_OsdLayIconDraw(APPVIEW_FLASH_X,APPVIEW_FLASH_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_FLASH_ON);
				break;
			case UI_FLASH_RED_EYE:
				appOsdLib_OsdLayIconDraw(APPVIEW_FLASH_X,APPVIEW_FLASH_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_FLASH_REDEYE_ON);
				break;
			case UI_FLASH_SLOW_SYNC:
				appOsdLib_OsdLayIconDraw(APPVIEW_FLASH_X,APPVIEW_FLASH_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_FLASH_SLOW_SYNC);
				break;
			default:
				break;
		}
	}
	else if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_FLASH)
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_FLASH;
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,APPVIEW_FLASH_X, APPVIEW_FLASH_Y,APPVIEW_ICON_SIZE_W,APPVIEW_ICON_SIZE_H,0,0);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appViewOsd_WBModeDraw
 *------------------------------------------------------------------------*/
void
appViewOsd_WBModeDraw(
	UINT8 bShow,
	UINT8 nWBMode
)
{
	//static UINT8 lastRecord;

	if (bShow)
	{
		//check
		static UINT8 lastRecord;
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_WB&& lastRecord==nWBMode)
		{
			return;
		}
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_WB&& lastRecord!=nWBMode)
		{
			appViewOsd_WBModeDraw(FALSE, 0);
		}
		//record
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_WB;
		lastRecord = nWBMode;

		appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_1);
		switch(nWBMode)
		{
			case UI_WB_AUTO:
				appOsdLib_OsdLayIconDraw(APPVIEW_WB_X,APPVIEW_WB_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_WB_AUTO);
				break;
			case UI_WB_CLOUDY:
				appOsdLib_OsdLayIconDraw(APPVIEW_WB_X,APPVIEW_WB_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_WB_CLOUDY);
				break;
			case UI_WB_FLUORESCENT_H:
				appOsdLib_OsdLayIconDraw(APPVIEW_WB_X,APPVIEW_WB_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_WB_FLUORESCENT_H);
				break;
			case UI_WB_FLUORESCENT_L:
				appOsdLib_OsdLayIconDraw(APPVIEW_WB_X,APPVIEW_WB_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_WB_FLUORESCENT_L);
				break;
			case UI_WB_FLUORESCENT:
				appOsdLib_OsdLayIconDraw(APPVIEW_WB_X,APPVIEW_WB_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_WB_FLUORESCENT);
				break;
			case UI_WB_TUNGSTEN:
				appOsdLib_OsdLayIconDraw(APPVIEW_WB_X,APPVIEW_WB_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_WB_TUNGSTEN);
				break;
			case UI_WB_DAYLIGHT:
				appOsdLib_OsdLayIconDraw(APPVIEW_WB_X,APPVIEW_WB_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_WB_DAYLIGHT);
				break;
			case UI_WB_CUSTOM:
				appOsdLib_OsdLayIconDraw(APPVIEW_WB_X,APPVIEW_WB_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_WB_CUSTOM);
				break;
			default:
				DBG_PRINT("sosd: _unkonw type: %d", nWBMode);
				break;
		}
	}
	else if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_WB)
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_WB;
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,APPVIEW_WB_X, APPVIEW_WB_Y,APPVIEW_ICON_SIZE_W,APPVIEW_ICON_SIZE_H,0,0);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appViewOsd_FocusModeDraw
 *------------------------------------------------------------------------*/
void
appViewOsd_FocusModeDraw(
	UINT8 bShow,
	UINT8 nFocusMode
)
{
	//static UINT8 lastRecord;

	if (bShow)
	{
		//check
		static UINT8 lastRecord;
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_AF&& lastRecord==nFocusMode)
		{
			return;
		}
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_AF&& lastRecord!=nFocusMode)
		{
			appViewOsd_FocusModeDraw(FALSE, 0);
		}
		//record
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_AF;
		lastRecord = nFocusMode;

		appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_1);
		switch(nFocusMode)
		{
			case UI_FOCUS_NORMAL:
				appOsdLib_OsdLayIconDraw(APPVIEW_FOCUSMODE_X,APPVIEW_FOCUSMODE_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_AUTO_FOCUS);
				break;
			case UI_FOCUS_MACRO:
				appOsdLib_OsdLayIconDraw(APPVIEW_FOCUSMODE_X,APPVIEW_FOCUSMODE_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_SCENE_MICROSPUR);
				break;
			case UI_FOCUS_INFINITY:
				appOsdLib_OsdLayIconDraw(APPVIEW_FOCUSMODE_X,APPVIEW_FOCUSMODE_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_INFINITY_FOCUS);
				break;
			default:
				break;
		}
	}
	else if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_AF)
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_AF;
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,APPVIEW_FOCUSMODE_X, APPVIEW_FOCUSMODE_Y,APPVIEW_ICON_SIZE_W,APPVIEW_ICON_SIZE_H,0,0);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appViewOsd_ShakeWarnDraw
 *------------------------------------------------------------------------*/
void
appViewOsd_ShakeWarnDraw(
	UINT8 bShow
)
{
	if (bShow)
	{
		//check
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_HAND)
		{
			return;
		}
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_HAND;

		appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_1);
		appOsdLib_OsdLayIconDraw(APPVIEW_SHAKE_X,APPVIEW_SHAKE_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_SCENE_SHAKEFREE_OFF);
	}
	else if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_HAND)
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_HAND;
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,APPVIEW_SHAKE_X, APPVIEW_SHAKE_Y,APPVIEW_ICON_SIZE_W,APPVIEW_ICON_SIZE_H,0,0);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appViewOsd_ISOModeDraw
 *------------------------------------------------------------------------*/
void
appViewOsd_ISOModeDraw(
	UINT8 bShow,
	UINT8 enISO
)
{
	//static UINT8 lastRecord;

	if (bShow)
	{
		//check
		static UINT8 lastRecord;
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_ISO&& lastRecord==enISO)
		{
			return;
		}
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_ISO&& lastRecord!=enISO)
		{
			appViewOsd_ISOModeDraw(FALSE, 0);
		}
		//record
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_ISO;
		lastRecord = enISO;

		appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_1);
		switch(enISO)
		{
			case UI_ISO_AUTO:
				appOsdLib_OsdLayIconDraw(APPVIEW_ISO_X,APPVIEW_ISO_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_ISO_AUTO);
				break;
			case UI_ISO_50:
				appOsdLib_OsdLayIconDraw(APPVIEW_ISO_X,APPVIEW_ISO_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_ISO_50);
				break;
			case UI_ISO_100:
				appOsdLib_OsdLayIconDraw(APPVIEW_ISO_X,APPVIEW_ISO_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_ISO_100);
				break;
			case UI_ISO_200:
				appOsdLib_OsdLayIconDraw(APPVIEW_ISO_X,APPVIEW_ISO_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_ISO_200);
				break;
			case UI_ISO_400:
				appOsdLib_OsdLayIconDraw(APPVIEW_ISO_X,APPVIEW_ISO_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_ISO_400);
				break;
			case UI_ISO_800:
				appOsdLib_OsdLayIconDraw(APPVIEW_ISO_X,APPVIEW_ISO_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_ISO_800);
				break;
			case UI_ISO_1600:
				appOsdLib_OsdLayIconDraw(APPVIEW_ISO_X,APPVIEW_ISO_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_ISO_1600);
				break;
			default:
				break;
		}
	}
	else if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_ISO)
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_ISO;
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,APPVIEW_ISO_X, APPVIEW_ISO_Y,APPVIEW_ICON_SIZE_W,APPVIEW_ICON_SIZE_H,0,0);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appViewOsd_DriverModeDraw
 *------------------------------------------------------------------------*/
void
appViewOsd_DriverModeDraw
(
    UINT8 bShow,
    UINT8 nDriverMode
)
{
	//static UINT8 lastRecord;

	if (bShow)
	{
		//check
		static UINT8 lastRecord;
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_DRIVE&& lastRecord==nDriverMode)
		{
			return;
		}
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_DRIVE&& lastRecord!=nDriverMode)
		{
			appViewOsd_DriverModeDraw(FALSE, 0);
		}
		//record
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_DRIVE;
		lastRecord = nDriverMode;

		appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_1);
		switch(nDriverMode)
		{
			case UI_STILL_DRIVERMODE_OFF:
				break;
			case UI_STILL_DRIVERMODE_BURST_H:
            case UI_STILL_DRIVERMODE_BURST_L:
			case UI_STILL_DRIVERMODE_AEB:
				appOsdLib_OsdLayIconDraw(APPVIEW_DRIVERMODE_X,APPVIEW_DRIVERMODE_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_BURST);
				break;
			default:
				break;
		}
	}
	else if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_DRIVE)
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_DRIVE;
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,APPVIEW_DRIVERMODE_X, APPVIEW_DRIVERMODE_Y,APPVIEW_ICON_SIZE_W,APPVIEW_ICON_SIZE_H,0,0);
	}
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_ViewModeDraw
 *  Description :
 *------------------------------------------------------------------------*/
void
appViewOsd_ViewModeDraw(
	UINT8 bShow,
	UINT8 Pasm,
	UINT8 Scene
)
{
	//static UINT8 lastRecord1, lastRecord2;

	if(bShow)
	{
		//check
		static UINT8 lastRecord1, lastRecord2;
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_VIEWSCENE_MODE
			&& lastRecord1==Pasm && lastRecord2==Scene)
		{
			return;
		}

		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_VIEWSCENE_MODE
			&& (lastRecord1!=Pasm || lastRecord2!=Scene))
		{
			appViewOsd_ViewModeDraw(FALSE, 0, 0);
		}
		//record
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_VIEWSCENE_MODE;
		lastRecord1 = Pasm;
		lastRecord2 = Scene;

        HOST_PROF_LOG_ADD(LEVEL_DEBUG, "sosd: load font s");
		appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_1);
        HOST_PROF_LOG_ADD(LEVEL_DEBUG, "sosd: load font e");

        HOST_PROF_LOG_ADD(LEVEL_DEBUG, "sosd: mode draw s");
		switch(Pasm)
		{
			case VIDEO_MODE:
                #if (KIT_WITH_HDMI)
                if (IS_HDMI_IN) {
                    appOsdLib_OsdLayIconDraw(APPVIEW_MODE_X+10, APPVIEW_MODE_Y,
                        SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_MODE_VIDEO);
                }
                else
                #endif
                {
                    appOsdLib_OsdLayIconDraw(APPVIEW_MODE_X, APPVIEW_MODE_Y,
                        SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_MODE_VIDEO);
                }
				break;

			default:
                #if (KIT_WITH_HDMI)
                if (IS_HDMI_IN) {
                    appOsdLib_OsdLayIconDraw(APPVIEW_MODE_X+10, APPVIEW_MODE_Y,
                        SP5K_GFX_ALIGN_TOP_LEFT, PasmIcon[Pasm]);
                }
                else
                #endif
                {
                    appOsdLib_OsdLayIconDraw(APPVIEW_MODE_X, APPVIEW_MODE_Y,
                        SP5K_GFX_ALIGN_TOP_LEFT, PasmIcon[Pasm]);
                }
				break;
		}
        HOST_PROF_LOG_ADD(LEVEL_DEBUG, "sosd: mode draw e");
	}
	else if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_VIEWSCENE_MODE)
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_VIEWSCENE_MODE;
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,APPVIEW_MODE_X, APPVIEW_MODE_Y,APPVIEW_ICON_SIZE_W,APPVIEW_ICON_SIZE_H,0,0);
	}
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_MeteringDraw
 *  Description :
 *------------------------------------------------------------------------*/
void
appViewOsd_MeteringDraw(
	UINT8 bShow,
	UINT8 idx
)
{
	//static UINT8 lastRecord;

	if (bShow)
	{
		//check
		static UINT8 lastRecord;
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_METER && lastRecord==idx)
		{
			return;
		}
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_METER && lastRecord!=idx)
		{
			appViewOsd_MeteringDraw(FALSE, 0);
		}
		//record
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_METER;
		lastRecord = idx;

		appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_1);
		switch (idx)
		{
			case UI_METERING_CENTER:
				appOsdLib_OsdLayIconDraw(APPVIEW_METER_X,APPVIEW_METER_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_METERING_CENTER);
				break;
			case UI_METERING_MULTI:
				appOsdLib_OsdLayIconDraw(APPVIEW_METER_X,APPVIEW_METER_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_METERING_MULTI);
				break;
			case UI_METERING_SPOT:
				appOsdLib_OsdLayIconDraw(APPVIEW_METER_X,APPVIEW_METER_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_METERING_SPOT);
				break;
			default:
				break;
		}
	}
	else if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_METER)
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_METER;
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,APPVIEW_METER_X, APPVIEW_METER_Y,APPVIEW_ICON_SIZE_W,APPVIEW_ICON_SIZE_H,0,0);
	}
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_ExposureBiaseDraw
 *  Description :
 *------------------------------------------------------------------------*/
void
appViewOsd_ExposureBiaseDraw(
	UINT8 bShow,
	UINT8 idx
)
{
	//static UINT8 lastRecord;

	if (bShow)
	{
		//check
		static UINT8 lastRecord;
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_EV && lastRecord==idx)
		{
			return;
		}
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_EV && lastRecord!=idx)
		{
			appViewOsd_ExposureBiaseDraw(FALSE, 0);
		}
		//record
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_EV;
		lastRecord = idx;

		appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_1);
		switch(idx)
		{
			case UI_EXP_BIAS_P200:
				appOsdLib_OsdLayIconDraw(APPVIEW_EV_X,APPVIEW_EV_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_EV_P20);
				break;
			case UI_EXP_BIAS_P166:
				appOsdLib_OsdLayIconDraw(APPVIEW_EV_X,APPVIEW_EV_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_EV_P17);
				break;
			case UI_EXP_BIAS_P133:
				appOsdLib_OsdLayIconDraw(APPVIEW_EV_X,APPVIEW_EV_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_EV_P13);
				break;
			case UI_EXP_BIAS_P100:
				appOsdLib_OsdLayIconDraw(APPVIEW_EV_X,APPVIEW_EV_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_EV_P10);
				break;
			case UI_EXP_BIAS_P066:
				appOsdLib_OsdLayIconDraw(APPVIEW_EV_X,APPVIEW_EV_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_EV_P07);
				break;
			case UI_EXP_BIAS_P033:
				appOsdLib_OsdLayIconDraw(APPVIEW_EV_X,APPVIEW_EV_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_EV_P03);
				break;
			case UI_EXP_BIAS_ZERO:
				appOsdLib_OsdLayIconDraw(APPVIEW_EV_X,APPVIEW_EV_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_EV_00);
				break;
			case UI_EXP_BIAS_N033:
				appOsdLib_OsdLayIconDraw(APPVIEW_EV_X,APPVIEW_EV_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_EV_N03);
				break;
			case UI_EXP_BIAS_N066:
				appOsdLib_OsdLayIconDraw(APPVIEW_EV_X,APPVIEW_EV_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_EV_N07);
				break;
			case UI_EXP_BIAS_N100:
				appOsdLib_OsdLayIconDraw(APPVIEW_EV_X,APPVIEW_EV_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_EV_N10);
				break;
			case UI_EXP_BIAS_N133:
				appOsdLib_OsdLayIconDraw(APPVIEW_EV_X,APPVIEW_EV_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_EV_N13);
				break;
			case UI_EXP_BIAS_N166:
				appOsdLib_OsdLayIconDraw(APPVIEW_EV_X,APPVIEW_EV_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_EV_N17);
				break;
			case UI_EXP_BIAS_N200:
				appOsdLib_OsdLayIconDraw(APPVIEW_EV_X,APPVIEW_EV_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_EV_N20);
				break;
			default:
				break;
		}
	}
	else if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_EV)
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_EV;
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,APPVIEW_EV_X, APPVIEW_EV_Y,APPVIEW_ICON_SIZE_W,APPVIEW_ICON_SIZE_H,0,0);
	}
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_BatteryDraw
 *  Description :
 *------------------------------------------------------------------------*/
void
appViewOsd_BatteryDraw(
	UINT8 bShow,
	UINT8 idx
)
{
	//static UINT8 lastRecord;

	if (bShow)
	{
		//check
		static UINT8 lastRecord;
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_BATTERY && lastRecord==idx)
		{
			return;
		}
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_BATTERY && lastRecord!=idx)
		{
			appViewOsd_BatteryDraw(FALSE, 0);
		}
		//record
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_BATTERY;
		lastRecord = idx;

		appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_1);
		switch(idx)
		{
			case APP_BATT_LEVEL_0:
                #if (KIT_WITH_HDMI)
                if (IS_HDMI_IN) {
                    appOsdLib_OsdLayIconDraw(APPVIEW_BATTERY_X+10, APPVIEW_BATTERY_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_BATTERY_LOW);
                }
                else
                #endif
                {
                    appOsdLib_OsdLayIconDraw(APPVIEW_BATTERY_X, APPVIEW_BATTERY_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_BATTERY_LOW);
                }
				break;
			case APP_BATT_LEVEL_1:
                #if (KIT_WITH_HDMI)
                if (IS_HDMI_IN) {
                    appOsdLib_OsdLayIconDraw(APPVIEW_BATTERY_X+10, APPVIEW_BATTERY_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_BATTERY_HALF);
                }
                else
                #endif
                {
                    appOsdLib_OsdLayIconDraw(APPVIEW_BATTERY_X, APPVIEW_BATTERY_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_BATTERY_HALF);
                }
				break;
			case APP_BATT_LEVEL_2:
                #if (KIT_WITH_HDMI)
                if (IS_HDMI_IN) {
                    appOsdLib_OsdLayIconDraw(APPVIEW_BATTERY_X+10, APPVIEW_BATTERY_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_BATTERY_FULL);
                }
                else
                #endif
                {
                    appOsdLib_OsdLayIconDraw(APPVIEW_BATTERY_X, APPVIEW_BATTERY_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_BATTERY_FULL);
                }
				break;
			case APP_BATT_LEVEL_3:
                HOST_PROF_LOG_PRINT(LEVEL_WARN, "sosd: APP_BATT_LEVEL_3 is not used. %s (%d)",\
                        __FUNCTION__,__LINE__);
				break;
			default:
				break;
		}
	}
	else if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_BATTERY)
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_BATTERY;
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,APPVIEW_BATTERY_X, APPVIEW_BATTERY_Y,APPVIEW_ICON_SIZE_W,APPVIEW_ICON_SIZE_H,0,0);
	}
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_HistogramDraw
 *  Description :
 *------------------------------------------------------------------------*/
void
appViewOsd_HistogramDraw(
	BOOL showType,
	UINT32 *pLuma
)
{
	//UINT32 *pBar, tempMax;
	//int bar_x, bar_y, bar_w, bar_h, bar_num, sample_interval;
	//int i, j;

	if(showType == OSD_OBJ_DESTROYED)
	{
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, APPVIEW_HISTOGRAM_X,
			APPVIEW_HISTOGRAM_Y,APPVIEW_HISTOGRAM_W,APPVIEW_HISTOGRAM_H,0,0);
		appOsdLib_TransparentLayerShapeClear(APP_OSD_SHAPE_RECT, APPVIEW_HISTOGRAM_BG_X,
			APPVIEW_HISTOGRAM_BG_Y,APPVIEW_HISTOGRAM_BG_W,APPVIEW_HISTOGRAM_BG_H,0,0);
	}
	else
	{
		int bar_x, bar_y, bar_w, bar_h, bar_num, sample_interval;
		int i, j;
		UINT32 *pBar, tempMax;
		if(!pLuma)
		{
			return;
		}

		bar_w = 1;
		bar_num = APPVIEW_HISTOGRAM_W;
		sample_interval = 256/APPVIEW_HISTOGRAM_W;
		pBar = pLuma;
		tempMax = 0;

		/* down sample */
		for(i=0, j=0; i<256; i+=sample_interval, j++)
		{
			pBar[j] = pLuma[i];
			if(pBar[j]>tempMax)
			{
				tempMax = pBar[j];
			}
		}

		/* equlization */
		if(tempMax >0)
		{
			for(i=0; i<bar_num; i++)
			{
				pBar[i] = (pBar[i]*APPVIEW_HISTOGRAM_H)/tempMax;
			}
		}
		else
		{
            HOST_PROF_LOG_PRINT(LEVEL_WARN, "sosd: %s: tempMax is %d",__FUNCTION__, tempMax);
		}

		sp5kGfxAttrSet(SP5K_GFX_REFRESH_ACTIVE, 0, 0, 0, 0);
		/* draw the gray backgroud */
		if(showType != OSD_OBJ_RUNNING)
		{

			appOsd_ColorDrawShapeColorSet(PAL_GRAY_1, PAL_BLEND_50, PAL_GRAY_1, PAL_BLEND_50);
			appOsd_ColorTransparentShapeColorSet(PAL_GRAY_1, PAL_GRAY_1, PAL_BLEND_50);
			appOsdLib_TransparentLayerShapeDraw(APP_OSD_SHAPE_RECT,	APPVIEW_HISTOGRAM_BG_X,
				APPVIEW_HISTOGRAM_BG_Y,APPVIEW_HISTOGRAM_BG_W,APPVIEW_HISTOGRAM_BG_H,0,0);
		}

		/* clean the bar chart */
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, APPVIEW_HISTOGRAM_X,
			APPVIEW_HISTOGRAM_Y,APPVIEW_HISTOGRAM_W,APPVIEW_HISTOGRAM_H,0,0);

		/* draw the new bar chart */
		appOsd_ColorDrawShapeColorSet(PAL_WHITE, PAL_BLEND_100, PAL_WHITE, PAL_BLEND_100);
		for(i = 0, bar_x = APPVIEW_HISTOGRAM_X; i < bar_num; i++, bar_x += bar_w)
		{
			if(pBar[i]>APPVIEW_HISTOGRAM_H)
			{
                HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "sosd: [WARN] %s: pBar[i]> his_h",\
                        __FUNCTION__);
				bar_h =APPVIEW_HISTOGRAM_H;
			}
			else
			{
				bar_h = pBar[i];
			}

			if (bar_h !=0)
			{
				bar_y = APPVIEW_HISTOGRAM_Y + APPVIEW_HISTOGRAM_H - bar_h;
				appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, bar_x, bar_y, bar_w, bar_h, 0, 0);
			}
		}
		sp5kGfxAttrSet(SP5K_GFX_REFRESH_ACTIVE, 1, 0, 0, 0);
	}
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_RealFnumberDraw
 *  Description :
 *------------------------------------------------------------------------*/
void
appViewOsd_RealFnumberDraw(
	UINT8 bShow,
	UINT8* str
)
{
	DBG_PRINT("sosd: %s\n", __FUNCTION__);
	if (bShow)
	{
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_REAL_FNUM;
		appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_TextDraw(APPVIEW_REAL_AV_X, APPVIEW_REAL_AV_Y, SP5K_GFX_ALIGN_TOP_RIGHT, (UINT8 *)str);
	}
	else if (ViewOsdPresentStatus&VIEW_OSD_PRESENT_REAL_FNUM)
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_REAL_FNUM;
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,APPVIEW_REAL_AV_X-(APPVIEW_ICON_SIZEX*2+APPVIEW_ICON_Y_GAP+6), APPVIEW_REAL_AV_Y, APPVIEW_ICON_SIZEX*2+APPVIEW_ICON_Y_GAP+6, APPVIEW_ICON_SIZEY, 0, 0);
	}
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_RealShutterSpeedDraw
 *  Description :
 *------------------------------------------------------------------------*/
void
appViewOsd_RealShutterSpeedDraw(
	UINT8 bShow,
	UINT8* str
)
{
	DBG_PRINT("sosd: %s\n", __FUNCTION__);

	if (bShow)
	{
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_REAL_SHUTTER;
		appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_TextDraw(APPVIEW_REAL_TV_X, APPVIEW_REAL_TV_Y, SP5K_GFX_ALIGN_TOP_RIGHT, (UINT8 *)str);
	}
	else if (ViewOsdPresentStatus&VIEW_OSD_PRESENT_REAL_SHUTTER)
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_REAL_SHUTTER;
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,APPVIEW_REAL_TV_X-(APPVIEW_ICON_SIZEX*2+APPVIEW_ICON_Y_GAP+6), APPVIEW_REAL_TV_Y, APPVIEW_ICON_SIZEX*2+APPVIEW_ICON_Y_GAP+6, APPVIEW_ICON_SIZEY, 0, 0);
	}
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_FocusWindowDraw
 *  Description :
 *------------------------------------------------------------------------*/
void
appViewOsd_FocusWindowDraw(
	UINT8 bShow,
	UINT8 nFocusStatus
)
{

}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_AmodeAvDraw
 *  Description :	In PASM-A mode (aperture piority), to draw the aperture
 *					value(F number).
 *------------------------------------------------------------------------*/
void
appViewOsd_AmodeAvDraw(
	BOOL bShow,
	UINT8 Select,
	UINT8 Index,
	UINT32 ZoomPoint
)
{
	//static UINT8 lastRecord1, lastRecord2, lastRecord3;

	SINT32 x=235;
	SINT32 y=240-30-48-4-6;
	SINT32 w=100;
	SINT32 h=24;
	//UINT32 checkZoomPoint, checkIrisIdx;

	if (bShow)
	{
		//check
		UINT32 checkZoomPoint, checkIrisIdx;
		static UINT8 lastRecord1, lastRecord2, lastRecord3;
		if(gStillCB.err > VIEW_ERR_NO_ERROR )
		{
			return;
		}
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_AMODE_FNUM
			&& lastRecord1==Select && lastRecord2==Index && lastRecord3==ZoomPoint)
		{
				return;
		}
		//record
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_AMODE_FNUM;
		lastRecord1=Select;
		lastRecord2=Index;
		lastRecord3=ZoomPoint;

		if (Select ==TRUE)
		{
			appOsd_ColorDrawShapeColorSet(PAL_BLACK, PAL_BLEND_100, PAL_BLACK, PAL_BLEND_100);
			appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,x, y, w, h, 0, 0);
			appOsd_ColorDrawTextColorSet(PAL_YELLOW, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);

			appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_1);
			appOsdLib_OsdLayIconDraw(x,y+h+12,SP5K_GFX_ALIGN_BOTTOM_LEFT,ID_ICON_MEDIA_UP_ARROW);/* replace ICON */
		}
		else
		{
			appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
			appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,x, y, w, h, 0, 0);
			// for guid line overlay
			if(gStillCB.osd==OSD_VIEW_GRID)
			{
				appOsd_ColorDrawShapeColorSet(PAL_WHITE, PAL_BLEND_100, PAL_WHITE, PAL_BLEND_100);
				appOsdLib_OsdLayerLineDraw(0, LCD_SIZE_Y-LCD_SIZE_Y/3, LCD_SIZE_X, LCD_SIZE_Y-LCD_SIZE_Y/3);
			}
			appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		}
		checkIrisIdx = (Index==APERTURE_2_8)? 0: 1;
		checkZoomPoint= (ZoomPoint>6)? 6: ZoomPoint;
		appOsdLib_TextDraw(LCD_SIZE_X, y+h, SP5K_GFX_ALIGN_BOTTOM_RIGHT, (UINT8 *)aperturevValueStr[checkZoomPoint][checkIrisIdx]);
	}
	else if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_AMODE_FNUM)
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_AMODE_FNUM;
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,x, y, w, h, 0, 0);
		// for guid line overlay
		if(gStillCB.osd==OSD_VIEW_GRID)
		{
			appOsd_ColorDrawShapeColorSet(PAL_WHITE, PAL_BLEND_100, PAL_WHITE, PAL_BLEND_100);
			appOsdLib_OsdLayerLineDraw(0, LCD_SIZE_Y-LCD_SIZE_Y/3, LCD_SIZE_X, LCD_SIZE_Y-LCD_SIZE_Y/3);
		}
	}
}


/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_SmodeTvDraw
 *  Description :	In PASM-S mode (shutter piority), to draw the shutter
 *					speed (T value).
 *------------------------------------------------------------------------*/
void
appViewOsd_SmodeTvDraw(
	BOOL bShow,
	UINT8 Select,
	UINT8 Index
)
{
	//static UINT8 lastRecord1, lastRecord2;

	SINT32 x=235;
	SINT32 y=240-30-24-6-2;
	SINT32 w=100;
	SINT32 h=24;

	if (bShow) {
		//check
		static UINT8 lastRecord1, lastRecord2;
		if(gStillCB.err > VIEW_ERR_NO_ERROR )
		{
			return;
		}
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_SMODE_SHUTTER
			&& lastRecord1==Select && lastRecord2==Index)
		{
			return;
		}
		//record
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_SMODE_SHUTTER;
		lastRecord1=Select;
		lastRecord2=Index;

		if (Select ==TRUE)
		{
			appOsd_ColorDrawShapeColorSet(PAL_BLACK, PAL_BLEND_100, PAL_BLACK, PAL_BLEND_100);
			appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,x, y, w, h, 0, 0);
			appOsd_ColorDrawTextColorSet(PAL_YELLOW, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);

			appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_1);
			appOsdLib_OsdLayIconDraw(x,y+h+12,SP5K_GFX_ALIGN_BOTTOM_LEFT,ID_ICON_MEDIA_UP_ARROW); /* replace ICON */
		}
		else
		{
			appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
			appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,x, y, w, h, 0, 0);
			appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		}
		appOsdLib_TextDraw(LCD_SIZE_X, y+h, SP5K_GFX_ALIGN_BOTTOM_RIGHT, (UINT8 *)shutterSpeedValueStr[Index]);
	}
	else if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_SMODE_SHUTTER)
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_SMODE_SHUTTER;
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,x, y, w, h, 0, 0);
	}
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_GuideLineDraw
 *  Description :
 *------------------------------------------------------------------------*/
void
appViewOsd_GuideLineDraw(
	BOOL bShow
)
{
	UINT32 int_x = LCD_SIZE_X/3;
	UINT32 int_y = LCD_SIZE_Y/3;
	UINT32 hLine2_end_x, vLine_start_y, vLine_end_y, img_w, img_h, fr, vLine_h;

	if (bShow)
	{
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_GUID_LINE)
		{
			return;
		}
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_GUID_LINE;
		appOsd_ColorDrawShapeColorSet(PAL_WHITE, PAL_BLEND_100, PAL_WHITE, PAL_BLEND_100);
	}
	else
	{
		if(!(ViewOsdPresentStatus&VIEW_OSD_PRESENT_GUID_LINE))
		{
			return;
		}
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_GUID_LINE;
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	}

	if(IS_VIDEO_MODE)
	{
		appVideoSizeGet(pViewParam->videoSize, &img_w, &img_h, &fr);
	}
	else
	{
		appStill_GetCurSize(&img_w, &img_h);
	}
	vLine_h = LCD_SIZE_X * img_h /img_w;
	vLine_start_y = (LCD_SIZE_Y - vLine_h)/2;
	vLine_end_y = vLine_start_y + vLine_h;
	hLine2_end_x = LCD_SIZE_X;

#if (KIT_WITH_HDMI)
	if(IS_HDMI_IN && IS_VIDEO_MODE){
		appOsdLib_OsdLayerLineDraw(int_x			,0		,int_x				,LCD_SIZE_Y);
		appOsdLib_OsdLayerLineDraw(LCD_SIZE_X-int_x	,0		,LCD_SIZE_X-int_x	,LCD_SIZE_Y);
	}
	else{
#else
	{
#endif
		appOsdLib_OsdLayerLineDraw(int_x			,vLine_start_y		,int_x				,vLine_end_y);
		appOsdLib_OsdLayerLineDraw(LCD_SIZE_X-int_x	,vLine_start_y		,LCD_SIZE_X-int_x	,vLine_end_y);
	}
	appOsdLib_OsdLayerLineDraw(0				,int_y				,LCD_SIZE_X			,int_y);
	appOsdLib_OsdLayerLineDraw(0				,LCD_SIZE_Y-int_y	,hLine2_end_x		,LCD_SIZE_Y-int_y);
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_PvQV_DiskDraw
 *  Description :
 *------------------------------------------------------------------------*/
#if !SP5K_PVQV_DISABLE
void appViewOsd_PvQV_DiskDraw(void)
{
    sp5kDcfAttrElm_t DcfAttr;
	UINT32 FileIdx;
	static UINT8 Filename[32];
    static UINT32 ActDisk;
    UINT32 roiX, roiY, roiW, roiH, x, y, x1, y1, imgWidth, imgHeight;
    //UINT32 w_default = 160;

    #if S2STEST || S2S_IMPROVE
        return;
    #endif

    sp5kGfxPageClear(SP5K_GFX_PAGE_PF_0,0);
	return;

    if(PvQV_ENABLE_FLAG == PVQV_SHOW_OFF)
    {
        DBG_PRINT("sosd: DISK PvQV_ENABLE_FLAG OFF\n");
        return;
    }

    if(PvQV_SHOW_FLAG == PVQV_SHOW_ON)
    {
        DBG_PRINT("sosd: appViewOsd_PvQV_DiskDraw %d %d\n",ActDisk,appActiveDiskGet());
        if(ActDisk == appActiveDiskGet())
            return;
        else
            appViewOsd_PvQVInfoFree();
    }

    ActDisk = appActiveDiskGet();

    #if SP5K_CDFS_OPT
    if(appCdfsFileNumGet(&FileIdx)==SUCCESS)
    #else
    if(sp5kDcfFsCurFileIdxGet(&FileIdx)==SUCCESS)
    #endif
    {
		UINT32 w_default = 160;
        #if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
        if(FileIdx==0)
        {
            HOST_PROF_LOG_ADD(LEVEL_DEBUG, "sosd: PvQV no file");
            sp5kGfxPageClear(SP5K_GFX_PAGE_PF_0,0);
            return;
        }
        #endif

        sp5kDcfFsFileAttrGet(FileIdx , &DcfAttr, Filename);

        if(IS_FILETYPE_VIDEO(DcfAttr.fileType))
        {
            if (FAIL==sp5kMediaPlayAttrSet(SP5K_MEDIA_ATTR_FILE_NAME,(UINT32)Filename))
        	{
                HOST_PROF_LOG_PRINT(LEVEL_ERROR, "sosd: %s Media filename set failed",\
                                    __FUNCTION__);

                sp5kGfxPageClear(SP5K_GFX_PAGE_PF_0,0);
        		return;
        	}

            if (FAIL==sp5kMediaPlayAttrParse())
        	{
                HOST_PROF_LOG_PRINT(LEVEL_ERROR, "sosd: %s Media Attr Parse failed",\
                        __FUNCTION__);

                sp5kGfxPageClear(SP5K_GFX_PAGE_PF_0,0);
		        return;
        	}

            sp5kMediaPlayAttrGet(SP5K_MEDIA_ATTR_WIDTH, &imgWidth);
	        sp5kMediaPlayAttrGet(SP5K_MEDIA_ATTR_HEIGHT, &imgHeight);

            if(imgWidth==0 || imgHeight==0)
            {
                HOST_PROF_LOG_PRINT(LEVEL_ERROR, "sosd: %s %d %dwidth or height failed",\
                        __FUNCTION__, imgWidth, imgWidth);

                sp5kGfxPageClear(SP5K_GFX_PAGE_PF_0,0);
		        return;
            }

            roiW = (w_default*APPVIEW_QV_RATION/100)&(~0x0001);

            if (imgWidth * 3 == imgHeight * 4)  /*4:3*/
            {
                HOST_PROF_LOG_ADD(LEVEL_DEBUG, "sosd: appViewOsd_PvQV_DiskDraw 4:3 w:160 h:128");
                roiH = (w_default*8*APPVIEW_QV_RATION/1000)&(~0x0001);
            }
        	else if (imgWidth * 9 == imgHeight * 16)  /*16:9*/
        	{
                HOST_PROF_LOG_ADD(LEVEL_DEBUG, "sosd: appViewOsd_PvQV_DiskDraw 16:9 w:160 h:96");
                roiH = (w_default*6*APPVIEW_QV_RATION/1000)&(~0x0001);
        	}
        }
        else if(IS_FILETYPE_JPG(DcfAttr.fileType))
        {
            roiW = (w_default*APPVIEW_QV_RATION/100)&(~0x0001);
            roiH = (w_default*8*APPVIEW_QV_RATION/1000)&(~0x0001);
        }
        else
        {
            HOST_PROF_LOG_ADD(LEVEL_WARN, "sosd: PvQV not support format");
            sp5kGfxPageClear(SP5K_GFX_PAGE_PF_0,0);
            return;
        }
    }
    else
    {
        HOST_PROF_LOG_ADD(LEVEL_DEBUG, "sosd: PvQV no file");
        sp5kGfxPageClear(SP5K_GFX_PAGE_PF_0,0);
        return;
    }

    HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "sosd: appViewOsd_PvQV_DiskDraw %s %x ",\
                        Filename, DcfAttr.fileType);

    roiX = APPVIEW_QV_W-roiW;
    roiY = APPVIEW_QV_H;
    x = roiX-1;
    y = roiY-1;
    x1= x+roiW+1;
    y1= y+roiH+1;

    appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_LIGHTBLUE, PAL_BLEND_100);
	sp5kGfxPageClear(SP5K_GFX_PAGE_PF_0, 0); /* clear again before draw rec */
    sp5kGfxRectDraw(SP5K_GFX_PAGE_PF_0, x, y, x1, y1);
    //sp5kGfxRectFill(SP5K_GFX_PAGE_PF_0, x, y, x1, y1, PAL_BLACK);
    if(IS_FILETYPE_JPG(DcfAttr.fileType))
        appJpgFileDraw(JPG_DRAW_PF_LAYER, Filename, roiX, roiY, roiW, roiH);
    else
        appAviFileDraw(AVI_DRAW_PF_LAYER, Filename, roiX, roiY, roiW, roiH);

    PvQV_SHOW_FLAG = PVQV_SHOW_DISK_ON;
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_PvQVDraw
 *  Description :
 *------------------------------------------------------------------------*/
UINT32 appViewOsd_PvQVDraw(
	BOOL en,
	UINT8 qvSec
)
{
    //UINT32 roiX, roiY, roiW, roiH;
    //UINT32 x, y, x1, y1;
    sp5kGfxPicture_t* pic = NULL;

    #if S2STEST || S2S_IMPROVE
        return;
    #endif

    sp5kGfxPageClear(SP5K_GFX_PAGE_PF_0,0);
    return;

    if(en)  // && (qvSec!=QV_TIME_OFF))
    {
        if(appViewOsd_PvQVInfoGet(&pic)==FAIL)
        {
            if(PvQV_SHOW_FLAG == PVQV_SHOW_ON)
            {
                HOST_PROF_LOG_ADD(LEVEL_ERROR, "sosd: appViewOsd_PvQVDraw Fail");
                PvQV_SHOW_FLAG = PVQV_SHOW_FAIL;
                sp5kGfxPageClear(SP5K_GFX_PAGE_PF_0,0);
                 return PVQV_SHOW_FAIL;
            }
            else
            {
                HOST_PROF_LOG_ADD(LEVEL_WARN, "sosd: appViewOsd_PvQVDraw Disk");
                PvQV_SHOW_FLAG = PVQV_SHOW_DISK_ON;
                return PVQV_SHOW_DISK_ON;
            }
        }
        else
        {
            if(PvQV_ENABLE_FLAG == PVQV_SHOW_OFF)
            {
                DBG_PRINT("PvQV_ENABLE_FLAG OFF\n");
                PvQV_SHOW_FLAG = PVQV_SHOW_OFF;
                return PVQV_SHOW_DISABLE;
            }
            else
            {
				UINT32 x, y, x1, y1;
				UINT32 roiX, roiY, roiW, roiH;
                roiW = (pic->roiW*APPVIEW_QV_RATION/100)&(~0x0001);
                roiH = (pic->roiH*APPVIEW_QV_RATION/100)&(~0x0001);
                roiX = APPVIEW_QV_W-roiW;
                roiY = APPVIEW_QV_H;

                x = roiX-1;
                y = roiY-1;
                x1= x+roiW+1;
                y1= y+roiH+1;
                sp5kGfxAttrSet( SP5K_GFX_REFRESH_ACTIVE, 0, 0, 0, 0 );
                sp5kGfxPageClear(SP5K_GFX_PAGE_PF_0, 0);
                appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_LIGHTBLUE, PAL_BLEND_100);
                sp5kGfxRectDraw(SP5K_GFX_PAGE_PF_0, x, y, x1, y1);
                //sp5kGfxRectFill(SP5K_GFX_PAGE_PF_0, x, y, x1, y1, PAL_BLACK);
                sp5kGfxPictureScale(SP5K_GFX_PAGE_PF_0, roiX, roiY, roiW, roiH, pic);
                sp5kGfxAttrSet(SP5K_GFX_REFRESH_ACTIVE, 1, 0, 0, 0);

                PvQV_SHOW_FLAG = PVQV_SHOW_ON;
                return PVQV_SHOW_ON;
            }
        }
    }
    else
    {
        sp5kGfxPageClear(SP5K_GFX_PAGE_PF_0,0);
        PvQV_SHOW_FLAG = PVQV_SHOW_OFF;
        return PVQV_SHOW_OFF;
    }
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_PvQV_En
 *  Description :
 *------------------------------------------------------------------------*/
void appViewOsd_PvQV_En(void)
{
    //UINT32 ret;
    if(PvQV_SHOW_FLAG == 0xff || PvQV_SHOW_FLAG == PVQV_SHOW_FAIL)
    {
        PvQV_ENABLE_FLAG = 0xff;
        return;
    }
    else
    {
		UINT32 ret;
        if(PvQV_ENABLE_FLAG==PVQV_SHOW_DISK_ON)
            PvQV_ENABLE_FLAG = PVQV_SHOW_ON;

        if(PvQV_ENABLE_FLAG!=0xff)
            PvQV_ENABLE_FLAG = !PvQV_ENABLE_FLAG;

        ret = appViewOsd_PvQVDraw(!PvQV_SHOW_FLAG, gStillCB.cap.qvSec);

        if(ret==PVQV_SHOW_ON)
	        PvQV_ENABLE_FLAG = ret;
        else if(ret==PVQV_SHOW_DISK_ON)
        {
            sp5kGfxAttrSet( SP5K_GFX_REFRESH_ACTIVE, 0, 0, 0, 0 );
            appViewOsd_PvQV_DiskDraw();
            sp5kGfxAttrSet( SP5K_GFX_REFRESH_ACTIVE, 1, 0, 0, 0 );
        }
        else
            PvQV_ENABLE_FLAG = PVQV_SHOW_OFF;
    }
}
#endif

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_WarmingMsgDraw
 *  Description :
 *------------------------------------------------------------------------*/
void
appViewOsd_WarmingMsgDraw(
	BOOL bShow,
	UINT32 viewError,
	BOOL bSound
)
{


	if(bShow)
	{
		UINT32 needSound, osdtype;
		UINT32 err[VIEW_ERR_MAX]=
		{
			[VIEW_ERR_NULL] = ERR_MSG_NO_IMG,
			[VIEW_ERR_NO_ERROR] = ERR_MSG_NO_IMG,
			[VIEW_ERR_CARD_ERROR] = ERR_MSG_CARD_ERROR,
			[VIEW_ERR_CARD_LOCKED] = ERR_MSG_CARD_LOCKED,
			[VIEW_ERR_MEMORY_FULL] = ERR_MSG_MEMORY_FULL,
			[VIEW_ERR_TO_POWER_OFF] = ERR_MSG_NO_IMG,
			[VIEW_ERR_DISK_SPEED_TOO_SLOW] = ERR_MSG_DISK_SPEED_TOO_SLOW,
		};
		DBG_PRINT("sosd: viewError = %d", viewError);
		if(err[viewError] == ERR_MSG_NO_IMG)
		{
			return;
		}
		osdtype = IS_VIDEO_MODE? gVideoCB.osd : gStillCB.osd;
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_ERROR
			&& osdtype != OSD_VIEW_GRID
			&& osdtype != OSD_VIEW_SIMPLE
			&& bSound == FALSE ) {
			return;
		}
		// 1st with sound (also can force on in 2nd)
		needSound = (ViewOsdPresentStatus&VIEW_OSD_PRESENT_ERROR)? ERR_MSG_ONLY: ERR_MSG_WITH_WARNING_SOUND;
		needSound = bSound? ERR_MSG_WITH_WARNING_SOUND: needSound;

		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_ERROR;

		//clean
		appViewOsd_FocusWindowDraw(FALSE,0);
		appViewOsd_HistogramDestroy();

		//draw
		if(err[viewError] != ERR_MSG_NO_IMG)
		{
			appErrMsgWithSound(err[viewError] ,needSound , ERR_MSG_SHOW_FOREVER);
		}
		// if the message play with sound, we may want to flick the remain-time/remain-image number.
		// Here will issue the other message let the current status decide if need to goto still-busy state.
		if(viewError == VIEW_ERR_MEMORY_FULL
			&& needSound == ERR_MSG_WITH_WARNING_SOUND)
		{
			sp5kHostMsgSend(APP_UI_MSG_VIEW_MEMORY_FULL, 0);
		}
	}
	else if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_ERROR)
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_ERROR;
		appErrMsgWithSound(ERR_MSG_NO_IMG, ERR_MSG_ONLY, ERR_MSG_SHOW_FOREVER);

	}
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_PanoramaDraw
 *  Description :
 *------------------------------------------------------------------------*/
void
appViewOsd_PanoramaDraw(
	UINT8 bShow,
	UINT8 idx
)
{
	appOsd_ColorDrawShapeColorSet(PAL_BLACK, PAL_BLEND_75, PAL_BLACK, PAL_BLEND_75);
	appOsd_GfxRectShapeDraw(SP5K_GFX_PAGE_OSD_0,65, 50,254, 80);
	appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_1);
	appOsdLib_OsdLayIconDraw(90,52,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_BUTTON_RIGHT);
	appOsdLib_OsdLayIconDraw(130,52,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_BUTTON_LEFT);
	appOsdLib_OsdLayIconDraw(170,52,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_BUTTON_DOWN);
	appOsdLib_OsdLayIconDraw(210,52,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_BUTTON_UP);
	appOsd_ColorIconColorSet(PAL_LIGHTBLUE_1,PAL_RED);
	switch(idx)
	{
		case 0:
			appOsdLib_OsdLayIconDraw(90,52,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_BUTTON_RIGHT);
			break;
		case 1:
			appOsdLib_OsdLayIconDraw(130,52,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_BUTTON_LEFT);
			break;
		case 2:
			appOsdLib_OsdLayIconDraw(170,52,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_BUTTON_DOWN);
			break;
		case 3:
			appOsdLib_OsdLayIconDraw(210,52,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_BUTTON_UP);
			break;
		default:
			break;
	}
	appOsd_ColorIconColorSet(PAL_LIGHTBLUE_1,PAL_LIGHTBLUE_1);
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsdShow
 *  Description :   This is the only function will be called in view state.
 *------------------------------------------------------------------------*/
void
appViewOsdShow(
	UINT8 clean,
	UINT8 outInOsdFreeze,
	UINT8 showType
)
{
	UINT32 current_osd_type;
    HOST_PROF_LOG_ADD(LEVEL_DEBUG, "sosd: osdtype s");

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

#if	SP5K_MULTI_FAST_BOOT
	if (!IS_CARD_EXIST)
        appErrMsgWithSound(ERR_MSG_NO_MEMORY_CARD, ERR_MSG_WITH_WARNING_SOUND, ERR_MSG_SHOW_FOREVER);
#endif
    }
    #endif

	APP_OSD_REFRESH_OFF;
	appViewOsd_ZoomBarDestroy();
	
	#if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
	appViewOsd_SensorDraw(TRUE, isSensorResume);
    appViewOsd_DualModeDraw();
	#endif
	switch(showType)
	{
	case OSD_VIEW_VERBOSE:
		appViewOsd_ISOModeDraw(TRUE, pViewParam->iso);
		appViewOsd_WBModeDraw(TRUE, pViewParam->wb);
		appViewOsd_MeteringDraw(TRUE, pViewParam->metering);
		appViewOsd_ExposureBiaseDraw(TRUE, pViewParam->ev);
	default:
	case OSD_VIEW_SIMPLE:
		appViewOsd_GuideLineDraw(FALSE);
		appViewOsd_FocusWindowDraw(TRUE, gStillCB.focusStatus);

		appViewOsd_ViewModeDraw(TRUE, gStillCB.cap.CurViewMode, gStillCB.cap.CurScene);
		appViewOsdDiskShow();
		appViewOsd_PicSizeDraw(TRUE, pViewParam->stillSize);
		appViewOsd_PicQtyDraw(TRUE, pViewParam->stillQuality);
		appViewOsd_FlashModeDraw(TRUE, gStillCB.flashMode);
		appViewOsd_DriverModeDraw(TRUE, pViewParam->stillDriverMode);
		appViewOsd_StabilizerModeDraw(pViewParam->stablizer);
		appViewOsd_BatteryDraw(TRUE, appBatteryLevelGet());

        #if !SP5K_PVQV_DISABLE
        appViewOsd_PvQVDraw(TRUE, gStillCB.cap.qvSec);
        #endif
		break;
	case OSD_VIEW_NONE:
		appViewOsd_FocusWindowDraw(TRUE, gStillCB.focusStatus);
		appViewOsd_ISOModeDraw(FALSE, pViewParam->iso);
		appViewOsd_WBModeDraw(FALSE, pViewParam->wb);
		appViewOsd_MeteringDraw(FALSE, pViewParam->metering);
		appViewOsd_ExposureBiaseDraw(FALSE, pViewParam->ev);
		appViewOsd_ViewModeDraw(FALSE, gStillCB.cap.CurViewMode, gStillCB.cap.CurScene);
		appViewOsd_RemainNumDraw(FALSE, 0, 0);
		appViewOsd_PicSizeDraw(FALSE, pViewParam->stillSize);
		appViewOsd_PicQtyDraw(FALSE, pViewParam->stillQuality);
		appViewOsd_FlashModeDraw(FALSE, gStillCB.flashMode);
		appViewOsd_DriverModeDraw(FALSE, pViewParam->stillDriverMode);
		appViewOsd_StabilizerModeDraw(FALSE);
		appViewOsd_BatteryDraw(FALSE, appBatteryLevelGet());
        #if !SP5K_PVQV_DISABLE
        appViewOsd_PvQVDraw(FALSE, gStillCB.cap.qvSec);
        #endif
		break;
	case OSD_VIEW_GRID:
		appViewOsd_GuideLineDraw(TRUE);
        #if !SP5K_PVQV_DISABLE
        appViewOsd_PvQVDraw(FALSE, gStillCB.cap.qvSec);
        #endif
		if(gStillCB.err <= VIEW_ERR_NO_ERROR )
		{
			appViewOsd_FocusWindowDraw(TRUE, gStillCB.focusStatus);
		}
		break;
	case OSD_VIEW_CLEAN_ALL:
        #if !SP5K_PVQV_DISABLE
        appViewOsd_PvQVDraw(FALSE, gStillCB.cap.qvSec);
        #endif
		appViewOsd_AllClean();
		break;
	case OSD_VIEW_S_ONE_HOLD:
		current_osd_type = (IS_VIDEO_MODE)? gVideoCB.osd : gStillCB.osd;
		switch(current_osd_type)
		{
			case OSD_VIEW_NONE:
			case OSD_VIEW_VERBOSE:
			case OSD_VIEW_SIMPLE:
				appViewOsd_AllClean();
				break;
			case OSD_VIEW_GRID:
				appViewOsd_AllClean();
				appViewOsd_GuideLineDraw(TRUE);
				break;
		}
		//draw info
		if(!IS_VIDEO_MODE)
		{
			appViewOsd_ShakeWarnDraw(TRUE);
			if(gStillCB.lensZoomSeg>6 || (gStillCB.ae.iris-1)>1 || (gStillCB.ae.iris-1)<0)
			{
                HOST_PROF_LOG_PRINT(LEVEL_WARN, "sosd: fnum-idx exceed rang(zseg=%d, iris=%d)",gStillCB.lensZoomSeg, gStillCB.ae.iris-1);
			}
			appViewOsd_RealFnumberDraw(TRUE, aperturevValueStr[gStillCB.lensZoomSeg][gStillCB.ae.iris-1]);
			appViewOsd_RealShutterSpeedDraw(TRUE, shutterSpeedValueStr[(gStillCB.ae.shutter-47)/16+SHUTTER_1S]);
			appViewOsd_RealIsoValueDraw(TRUE, isoValueStr[gStillCB.ae.iso]);
		}
		{
			#ifdef AF_CURVE_SHOW
			if(pViewParam->af != UI_FOCUS_INFINITY)
			{
				AfValueCurveDraw(gStillCB.focusStatus);
			}
			else
			{
				appViewOsd_FocusWindowDraw(TRUE, gStillCB.focusStatus);
			}
			#else
			appViewOsd_FocusWindowDraw(TRUE, gStillCB.focusStatus);
			#endif
		}
		break;
	}

	if(showType!=OSD_VIEW_S_ONE_HOLD)
	{
		appViewOsd_ASMChangeDraw();
	}

	if(gStillCB.err > VIEW_ERR_NO_ERROR)
	{
		APP_OSD_REFRESH_ON;
		appViewOsd_WarmingMsgDraw(TRUE, gStillCB.err, FALSE);
	}

    if(showType<=OSD_VIEW_VERBOSE && gStillCB.err<=VIEW_ERR_NO_ERROR )
    {
        #if !SP5K_PVQV_DISABLE
        appViewOsd_PvQV_DiskDraw();
        #endif

        if(showType==OSD_VIEW_VERBOSE)
            appViewOsd_HistogramCreat();
        else
        {
            appViewOsd_HistogramDestroy();
        }
    }
    else
        appViewOsd_HistogramDestroy();

	if(!outInOsdFreeze)
	{
		APP_OSD_REFRESH_ON;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : app_StillOsd_PicSizeVideoDraw
 *------------------------------------------------------------------------*/
void
appViewOsd_PicSizeVideoDraw(
	BOOL bShow,
	UINT8 nPicSize
)
{
	//static UINT8 lastRecord;
    //UINT8 w = 70;
    //UINT8 h = 32;
	if (bShow)
	{
		static UINT8 lastRecord;
		UINT8 w = 70;
		UINT8 h = 32;
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_VIDEO_SIZE && lastRecord==nPicSize)
		{
			return ;
		}
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_VIDEO_SIZE && lastRecord!=nPicSize)
		{
			appViewOsd_PicSizeVideoDraw(FALSE, 0);
		}
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_VIDEO_SIZE;

		appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_1);
		switch(nPicSize)
		{
			#if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
			case UI_VIDEO_DUAL_SIZE_FHD:
			case UI_VIDEO_DUAL_SIZE_FHD_720P:
			case UI_VIDEO_DUAL_SIZE_FHD_VGA:
			#endif	/* SP5K_USB_UVC_SUPPORT / SP5K_SENSOR_DUAL_MODE*/

            case UI_VIDEO_SIZE_4K2K:
            case UI_VIDEO_SIZE_27K:
				appOsdLib_OsdLayerShapeClear(APP_OSD_SHAPE_RECT, APPVIEW_FRAMERATE_X, APPVIEW_FRAMERATE_Y, w, h, 0, 0);
				appOsdLib_OsdLayIconDraw(APPVIEW_PICSIZE_X, APPVIEW_PICSIZE_Y, SP5K_GFX_ALIGN_TOP_LEFT , ID_ICON_MOVIE_SIZE_1080P);
				break;
				
			case UI_VIDEO_SIZE_FHD_60FPS:
				appOsdLib_OsdLayerShapeClear(APP_OSD_SHAPE_RECT, APPVIEW_FRAMERATE_X, APPVIEW_FRAMERATE_Y, w, h, 0, 0);
				appOsdLib_OsdLayIconDraw(APPVIEW_PICSIZE_X, APPVIEW_PICSIZE_Y, SP5K_GFX_ALIGN_TOP_LEFT , ID_ICON_MOVIE_SIZE_1080P);
				appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_IRON, PAL_NOCOLOR, PAL_BLEND_50);
                appOsdLib_TextDraw(APPVIEW_FRAMERATE_X, APPVIEW_FRAMERATE_Y, SP5K_GFX_ALIGN_TOP_LEFT, (UINT8 *)"60fps");
				break;
				
			case UI_VIDEO_SIZE_FHD_30FPS:
			#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
			case UI_VIDEO_SIZE_FHD_PARK:
			#endif
				appOsdLib_OsdLayerShapeClear(APP_OSD_SHAPE_RECT, APPVIEW_FRAMERATE_X, APPVIEW_FRAMERATE_Y, w, h, 0, 0);
				appOsdLib_OsdLayIconDraw(APPVIEW_PICSIZE_X, APPVIEW_PICSIZE_Y, SP5K_GFX_ALIGN_TOP_LEFT , ID_ICON_MOVIE_SIZE_1080P);
				appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_IRON, PAL_NOCOLOR, PAL_BLEND_50);

				#if defined(PRJ_PDVR)
	     		       appOsdLib_TextDraw(APPVIEW_FRAMERATE_X, APPVIEW_FRAMERATE_Y, SP5K_GFX_ALIGN_TOP_LEFT, (UINT8 *)"27.5fps");
				#else
				appOsdLib_TextDraw(APPVIEW_FRAMERATE_X, APPVIEW_FRAMERATE_Y, SP5K_GFX_ALIGN_TOP_LEFT, (UINT8 *)"30fps");
				#endif
				break;

			case UI_VIDEO_SIZE_HD_30FPS:
				appOsdLib_OsdLayerShapeClear(APP_OSD_SHAPE_RECT, APPVIEW_FRAMERATE_X, APPVIEW_FRAMERATE_Y, w, h, 0, 0);
				appOsdLib_OsdLayIconDraw(APPVIEW_PICSIZE_X, APPVIEW_PICSIZE_Y, SP5K_GFX_ALIGN_TOP_LEFT , ID_ICON_MOVIE_SIZE_720P);
				appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_IRON, PAL_NOCOLOR, PAL_BLEND_50);
				#if defined(PRJ_PDVR)
	     		       appOsdLib_TextDraw(APPVIEW_FRAMERATE_X, APPVIEW_FRAMERATE_Y, SP5K_GFX_ALIGN_TOP_LEFT, (UINT8 *)"27.5fps");
				#else
				appOsdLib_TextDraw(APPVIEW_FRAMERATE_X, APPVIEW_FRAMERATE_Y, SP5K_GFX_ALIGN_TOP_LEFT, (UINT8 *)"30fps");
				#endif
				break;

			default:
				break;
		}
		lastRecord=nPicSize;

	}
	else if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_VIDEO_SIZE)
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_VIDEO_SIZE;
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, APPVIEW_PICSIZE_X, APPVIEW_PICSIZE_Y,APPVIEW_ICON_SIZE_W, APPVIEW_ICON_SIZE_H, 0, 0);
	}
}
/*-------------------------------------------------------------------------
 *  File Name : appViewOsd_PicQtyDraw
 *------------------------------------------------------------------------*/
void
appViewOsd_PicQtyDraw(
	BOOL bShow,
	UINT8 nPicQty
)
{
	//static UINT8 lastRecord;

	if (bShow)
	{
		//check
		static UINT8 lastRecord;
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_QUALITY && lastRecord==nPicQty)
		{
			return;
		}
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_QUALITY && lastRecord!=nPicQty)
		{
			appViewOsd_PicQtyDraw(FALSE, 0);
		}
		//record
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_QUALITY;
		lastRecord = nPicQty;

		appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_1);
		switch(nPicQty)
		{
			default:
			case UI_QUALITY_NORMAL:
				appOsdLib_OsdLayIconDraw(APPVIEW_PICQTY_X, APPVIEW_PICQTY_Y, SP5K_GFX_ALIGN_TOP_LEFT,  ID_ICON_QUALITY);
				break;
			case UI_QUALITY_FINE:
				appOsdLib_OsdLayIconDraw(APPVIEW_PICQTY_X, APPVIEW_PICQTY_Y, SP5K_GFX_ALIGN_TOP_LEFT,  ID_ICON_QUALITY_FINE);
				break;
			case UI_QUALITY_SUPERFINE:
				appOsdLib_OsdLayIconDraw(APPVIEW_PICQTY_X, APPVIEW_PICQTY_Y, SP5K_GFX_ALIGN_TOP_LEFT,  ID_ICON_QUALITY_SUPER_FINE);
				break;
		}
	}
	else if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_QUALITY)
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_QUALITY;
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, APPVIEW_PICQTY_X, APPVIEW_PICQTY_Y,APPVIEW_ICON_SIZE_W, APPVIEW_ICON_SIZE_H, 0, 0);
	}
}
/*-------------------------------------------------------------------------
 *  File Name : appViewOsd_PicQtyDraw
 *------------------------------------------------------------------------*/
void
appViewOsd_VideoQtyDraw(
	BOOL bShow,
	UINT8 nPicQty
)
{
	//static UINT8 lastRecord;

	if (bShow)
	{
		//check
		static UINT8 lastRecord;
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_QUALITY && lastRecord==nPicQty)
		{
			return;
		}
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_QUALITY && lastRecord!=nPicQty)
		{
			appViewOsd_VideoQtyDraw(FALSE, 0);
		}
		//record
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_QUALITY;
		lastRecord = nPicQty;

		appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_1);
		switch(nPicQty)
		{
			default:
			case UI_QUALITY_NORMAL:
				appOsdLib_OsdLayIconDraw(APPVIEW_PICQTY_X-172, APPVIEW_PICQTY_Y+2, SP5K_GFX_ALIGN_TOP_LEFT,  ID_ICON_QUALITY);
				break;
			case UI_QUALITY_FINE:
				appOsdLib_OsdLayIconDraw(APPVIEW_PICQTY_X-172, APPVIEW_PICQTY_Y+2, SP5K_GFX_ALIGN_TOP_LEFT,  ID_ICON_QUALITY_FINE);
				break;
			case UI_QUALITY_SUPERFINE:
				appOsdLib_OsdLayIconDraw(APPVIEW_PICQTY_X-172, APPVIEW_PICQTY_Y+2, SP5K_GFX_ALIGN_TOP_LEFT,  ID_ICON_QUALITY_SUPER_FINE);
				break;
		}
	}
	else if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_QUALITY)
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_QUALITY;
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, APPVIEW_PICQTY_X-172, APPVIEW_PICQTY_Y+2,APPVIEW_ICON_SIZE_W+32, APPVIEW_ICON_SIZE_H, 0, 0);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : app_StillOsd_RemainNumDraw
 *------------------------------------------------------------------------*/
void
appViewOsd_RemainNumDraw(
	BOOL bShow,
	unsigned char bHighLight,
	unsigned int nRemainNum
)
{


	if (nRemainNum>9999)
	{
		nRemainNum=9999;
	}

	if (bShow)
	{
		//check
		static unsigned int lastRecord;
		char szNum[8];
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_REMAIN_IMAGE_NUMBER
			&& lastRecord==nRemainNum)
		{
			return;
		}
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_REMAIN_IMAGE_NUMBER
			&& lastRecord!=nRemainNum)
		{
			appViewOsd_RemainNumDraw(FALSE, 0, 0);
		}
		//record
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_REMAIN_IMAGE_NUMBER;
		lastRecord = nRemainNum;

		appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_IRON, PAL_NOCOLOR, PAL_BLEND_50);
		sprintf(szNum, "%x", nRemainNum);
        #if (KIT_WITH_HDMI)
        if (IS_HDMI_IN) {
            appOsdLib_TextDraw(APPVIEW_REMAINNUM_RIGHT_X-10, APPVIEW_REMAINNUM_RIGHT_Y, SP5K_GFX_ALIGN_TOP_RIGHT, (UINT8 *)szNum);
        }
        else
        #endif
        {
            appOsdLib_TextDraw(APPVIEW_REMAINNUM_RIGHT_X, APPVIEW_REMAINNUM_RIGHT_Y, SP5K_GFX_ALIGN_TOP_RIGHT, (UINT8 *)szNum);
        }
	}
	else if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_REMAIN_IMAGE_NUMBER)
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_REMAIN_IMAGE_NUMBER;
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, APPVIEW_REMAINNUM_RIGHT_X-(APPVIEW_CHR_WIDTH*4), APPVIEW_REMAINNUM_RIGHT_Y,APPVIEW_CHR_WIDTH*4, APPVIEW_CHR_HIEGHT, 0, 0);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : app_StillOsd_AudioTotalFreeTimeDraw
 *------------------------------------------------------------------------*/
void
appViewOsd_TotalFreeTimeDraw(
	BOOL bShow,
	unsigned char bHighLight,
	UINT32 nSec
)
{
	char szTime[16];
	unsigned short 	nHourX=0;

    /*
       *  There is an limit: nHourX + FontEngNumSize_X * 6 <= LCD_SIZE_X,
       *  so the APPVIEW_AUDIO_FREETIME_OFFSET should not exceed the value of 99
       */
	nHourX=APPVIEW_AUDIO_FREETIME_X + APPVIEW_AUDIO_FREETIME_OFFSET;


	if (!bShow)
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_VIDEO_REMAIN_TIME;
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, nHourX, APPVIEW_AUDIO_FREETIME_Y,FontEngNumSize_X*6, FontEngNumSize_Y, 0, 0);
		return ;

	}
	ViewOsdPresentStatus |= VIEW_OSD_PRESENT_VIDEO_REMAIN_TIME;

	memset(szTime, 0, 16);

	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, nHourX, APPVIEW_AUDIO_FREETIME_Y,FontEngNumSize_X*6, FontEngNumSize_Y, 0, 0);
	sprintf(szTime, "%02u:%02u:%02u", nSec/3600, (nSec%3600)/60, (nSec%3600)%60);

	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	if(nSec/3600>99)
	{
		appOsdLib_TextDraw(LCD_SIZE_X-93, APPVIEW_AUDIO_FREETIME_Y, SP5K_GFX_ALIGN_TOP_LEFT, (UINT8 *)szTime);
	}
	else
	{
		appOsdLib_TextDraw(LCD_SIZE_X-89, APPVIEW_AUDIO_FREETIME_Y, SP5K_GFX_ALIGN_TOP_LEFT, (UINT8 *)szTime);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : app_StillOsd_RealIsoValueDraw
 *------------------------------------------------------------------------*/
void
appViewOsd_RealIsoValueDraw(
	BOOL bShow,
	UINT8* str
)
{
	DBG_PRINT("sosd: %s\n", __FUNCTION__);

	if (bShow)
	{
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_REAL_ISO)
		{
			return;
		}
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_REAL_ISO;
		appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_TextDraw(APPVIEW_REAL_ISO_ICON_X, APPVIEW_REAL_ISO_ICON_Y, SP5K_GFX_ALIGN_TOP_RIGHT, (UINT8 *)str);
	}
	else if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_REAL_ISO)
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_REAL_ISO;
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,APPVIEW_REAL_ISO_ICON_X-(APPVIEW_ICON_SIZEX*2+APPVIEW_ICON_Y_GAP+6), APPVIEW_REAL_ISO_ICON_Y, APPVIEW_ICON_SIZEX*2+APPVIEW_ICON_Y_GAP+6, APPVIEW_ICON_SIZEY, 0, 0);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appViewOsd_StabilizerModeDraw
 *------------------------------------------------------------------------*/
void
appViewOsd_FDModeDraw(
	BOOL bShow
)
{
	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, APPVIEW_FD_X, APPVIEW_FD_Y,APPVIEW_ICON_SIZEX, APPVIEW_ICON_SIZEY, 0, 0);
}

/*-------------------------------------------------------------------------
 *  File Name : appViewOsd_StabilizerModeDraw
 *------------------------------------------------------------------------*/
void
appViewOsd_StabilizerModeDraw(
	BOOL bShow
)
{
	if (bShow)
	{
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_STABILIZER)
		{
			return;
		}
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_STABILIZER;
		appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_1);
		appOsdLib_OsdLayIconDraw(APPVIEW_STABILIZER_X, APPVIEW_STABILIZER_Y,  SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_STABILIZER);
	}
	else if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_STABILIZER)
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_STABILIZER;
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, APPVIEW_STABILIZER_X, APPVIEW_STABILIZER_Y,APPVIEW_ICON_SIZEX, APPVIEW_ICON_SIZEY, 0, 0);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appViewOsd_SelfTimeDraw
 *------------------------------------------------------------------------*/
void appViewOsd_SelfTimeDraw(
	UINT8 bShow
)
{
	if(bShow)
	{
		if(ViewOsdPresentStatus&VIEW_OSD_PRESENT_SELFTIME)
		{
			return;
		}
		ViewOsdPresentStatus |= VIEW_OSD_PRESENT_SELFTIME;

		appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_1);
		appOsdLib_OsdLayIconDraw(APPVIEW_SELFTIME_X,APPVIEW_SELFTIME_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_SELFTIMER_2SEC);
	}
	else if (ViewOsdPresentStatus&VIEW_OSD_PRESENT_SELFTIME)
	{
		ViewOsdPresentStatus &= ~VIEW_OSD_PRESENT_SELFTIME;
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,APPVIEW_SELFTIME_X, APPVIEW_SELFTIME_Y,APPVIEW_ICON_SIZE_W,APPVIEW_ICON_SIZE_H,0,0);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appViewOsd_ZoomFrameDraw
 *------------------------------------------------------------------------*/

void appViewOsd_ZoomFrameDraw(
	UINT8 bShow
)
{
	#if 0
	if (pViewParam->dzoom)
	{
		appViewOsd_DigZoomFrameDraw(bShow);
	}else
	{
		appViewOsd_OptZoomFrameDraw(bShow);
	}
	#endif
}
/*-------------------------------------------------------------------------
 *  File Name : appViewOsd_DigZoomFrameDraw
 *------------------------------------------------------------------------*/

void appViewOsd_DigZoomFrameDraw(
	UINT8 bShow
)
{
	if (bShow)
	{
		appOsd_ColorDrawShapeColorSet(PAL_GRAY_1, PAL_BLEND_50, PAL_GRAY_1, PAL_BLEND_50);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, OPTICAL_ZOOM_BAR_X, OPTICAL_ZOOM_BAR_Y, OPTICAL_ZOOM_BAR_SIZEX + DIGITAL_ZOOM_BAR_SIZEX + 6, OPTICAL_ZOOM_BAR_SIZEY, 0, 0);
		appOsd_FontIconFontSet(ID_ICON_ZOOM);
		appOsdLib_OsdLayIconDraw(VIEW_ZOOM_FRAME_X, VIEW_ZOOM_FRAME_Y, SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_DIGITAL_ZOOM);
		#if (KIT_WITH_HDMI)
		if(IS_HDMI_IN){
			appOsd_FontIconFontSet(ID_FONT_TK_BIGICON);
		}else
		#endif
		{
		appOsd_FontIconFontSet(ID_FONT_TK_ICON);
	}
	}
	else if (!bShow)
	{
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, VIEW_ZOOM_FRAME_X, VIEW_ZOOM_FRAME_Y, OPTICAL_DIGITAL_ZOOM_ICON_SIZEX, OPTICAL_DIGITAL_ZOOM_ICON_SIZEY, 0, 0);
	}

}
/*-------------------------------------------------------------------------
 *  File Name : appViewOsd_OptZoomFrameDraw
 *------------------------------------------------------------------------*/

void appViewOsd_OptZoomFrameDraw(
	UINT8 bShow
)
{
	if (bShow)
	{
		appOsd_ColorDrawShapeColorSet(PAL_GRAY_1, PAL_BLEND_50, PAL_GRAY_1, PAL_BLEND_50);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, OPTICAL_ZOOM_BAR_X, OPTICAL_ZOOM_BAR_Y, OPTICAL_ZOOM_BAR_SIZEX, OPTICAL_ZOOM_BAR_SIZEY, 0, 0);
		appOsd_FontIconFontSet(ID_ICON_ZOOM);
		appOsdLib_OsdLayIconDraw(VIEW_ZOOM_FRAME_X, VIEW_ZOOM_FRAME_Y, SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_OPTICAL_ZOOM);
		#if (KIT_WITH_HDMI)
		if(IS_HDMI_IN){
			appOsd_FontIconFontSet(ID_FONT_TK_BIGICON);
		}else
		#endif
		{
		appOsd_FontIconFontSet(ID_FONT_TK_ICON);
		}

	}
	else if (!bShow)
	{
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, VIEW_ZOOM_FRAME_X, VIEW_ZOOM_FRAME_Y, OPTICAL_DIGITAL_ZOOM_ICON_SIZEX, OPTICAL_DIGITAL_ZOOM_ICON_SIZEY, 0, 0);
	}

}

/*-------------------------------------------------------------------------
 *  File Name : appViewOsd_OptZoomPosDraw
 *------------------------------------------------------------------------*/

void appViewOsd_OptZoomPosDraw(
	UINT8 Position
)
{
	if (Position > 100 )
	{
		Position = 100;
	}
	if (Position !=0)
	{
		float BarPosition = Position;
		BarPosition = (BarPosition / 100.0) * ((float)OPTICAL_ZOOM_BAR_SIZEX);
		appOsd_ColorDrawShapeColorSet(PAL_GREEN, PAL_BLEND_100, PAL_GREEN, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, OPTICAL_ZOOM_BAR_X, OPTICAL_ZOOM_BAR_Y, BarPosition, OPTICAL_ZOOM_BAR_SIZEY, 0, 0);
	}
}


/*-------------------------------------------------------------------------
 *  File Name : appViewOsd_DigZoomPosDraw
 *------------------------------------------------------------------------*/

void appViewOsd_DigZoomPosDraw(
	UINT8 Position
)
{
	if (Position > 100 )
	{
		Position = 100;
	}
	appViewOsd_OptZoomPosDraw(100);
	if (Position !=0)
	{
		float BarPosition = Position;
		BarPosition = (BarPosition / 100.0) * ((float)DIGITAL_ZOOM_BAR_SIZEX);
		appOsd_ColorDrawShapeColorSet(PAL_GREEN_2, PAL_BLEND_100, PAL_GREEN_2, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, DIGITAL_ZOOM_BAR_X, DIGITAL_ZOOM_BAR_Y, BarPosition, DIGITAL_ZOOM_BAR_SIZEY, 0, 0);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appViewOsd_ASMChangeDraw
 *------------------------------------------------------------------------*/
void appViewOsd_ASMChangeDraw(
)
{
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_HistogramCreat
 *  Description :	To creat the hitogram and assist timer.
 *					The hitogram object has creat/running/destroy status.
 *------------------------------------------------------------------------*/
void
appViewOsd_HistogramCreat(void)
{
	if(gHistogram.status == OSD_OBJ_RUNNING)
	{
		return;
	}
	/*peter temp mark*/

	appViewOsd_HistogramDraw(OSD_OBJ_CREATED, HistogramBuf);
	gHistogram.timer = appTimerSet(500, "hitogram");
	gHistogram.status = OSD_OBJ_RUNNING;
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_HistogramDestroy
 *  Description :	To cancle the timer and clean hitogram
 *------------------------------------------------------------------------*/
void
appViewOsd_HistogramDestroy(void)
{
	if(gHistogram.status == OSD_OBJ_DESTROYED)
	{
		return;
	}

	if(gHistogram.status == OSD_OBJ_RUNNING)
	{
		appTimerClear(&gHistogram.timer);
	}

	appViewOsd_HistogramDraw(OSD_OBJ_DESTROYED, NULL);
	gHistogram.status = OSD_OBJ_DESTROYED;
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_HistogramUpdateStop
 *  Description :	only stop the timer
 *------------------------------------------------------------------------*/
void
appViewOsd_HistogramUpdateStop(void)
{
	if(gHistogram.status == OSD_OBJ_RUNNING)
	{
		appTimerClear(&gHistogram.timer);
		gHistogram.status = OSD_OBJ_CREATED;
	}
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_HistogramUpdateStart
 *  Description :	Re-start timer and init every thing if we didn't creat
 *					before.
 *------------------------------------------------------------------------*/
void
appViewOsd_HistogramUpdateStart(void)
{
	if(gHistogram.status == OSD_OBJ_DESTROYED)
	{
		appViewOsd_HistogramCreat();
		gHistogram.timer = appTimerSet(250, "hitogram");
	}
/* 	else if(gHistogram.status == OSD_OBJ_DESTROYED)
	{
		gHistogram.timer = appTimerSet(250, "hitogram");
	} */
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_HistogramProcess
 *  Description :	This will be put on background.	It is responsible to
 *					receive the timer msg and draw new histogram data.
 *------------------------------------------------------------------------*/
void
appViewOsd_HistogramProcess(
	UINT32 param
)
{
	//UINT32 curState;

	if (param == gHistogram.timer)
	{
		UINT32 curState;
		if(gHistogram.status != OSD_OBJ_RUNNING)
		{
			return;
		}

		/* We will let histogram stop/destroy itself if it leave preview states */
		curState = appActiveStateGet();
		if(curState != APP_STATE_STILL_PREVIEW && curState != APP_STATE_VIDEO_PREVIEW)
		{
			appViewOsd_HistogramUpdateStop();
			return;
		}
		else if(!IS_VIEW_GROUP(curState) && !IS_VIDEO_GROUP(curState))
		{
			DBG_PRINT("sosd: %s: not group\n", __FUNCTION__);
			appViewOsd_HistogramDestroy();
			return;
		}

		/*peter temp mark*/
        #if 0
		sp5kImgHistogramGet(HistogramBuf);
        #endif
		appViewOsd_HistogramDraw(OSD_OBJ_RUNNING, HistogramBuf);
	}
}


/*-------------------------------------------------------------------------
*  File Name : appViewOsd_VideoTotalFreeTimeDraw
*------------------------------------------------------------------------*/
void
appViewOsd_VideoTotalFreeTimeDraw(
	BOOL bShow,
	UINT32 nSec
)
{
	unsigned char 	szTime[16];
	unsigned short 	nHourX=0;

	nHourX=APPVIEW_AUDIO_FREETIME_X+103;

    if(appActiveStateGet()==APP_STATE_VIDEO_REC)
        return;

	if (!bShow)
	{
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, nHourX, APPVIEW_AUDIO_FREETIME_Y,FontEngNumSize_X*6, FontEngNumSize_Y, 0, 0);
		return ;

	}
	memset(szTime, 0, 16);

	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, nHourX, APPVIEW_AUDIO_FREETIME_Y,FontEngNumSize_X*6, FontEngNumSize_Y, 0, 0);
	sprintf((char *)szTime, "%02u:%02u:%02u", nSec/3600, (nSec%3600)/60, (nSec%3600)%60);

	appOsd_ColorDrawTextColorSet(PAL_RED, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	if(nSec/3600>99)
	{
		appOsdLib_TextDraw(LCD_SIZE_X-93, APPVIEW_AUDIO_FREETIME_Y, SP5K_GFX_ALIGN_TOP_LEFT, (UINT8 *)szTime);
	}
	else
	{
		appOsdLib_TextDraw(LCD_SIZE_X-87, APPVIEW_AUDIO_FREETIME_Y, SP5K_GFX_ALIGN_TOP_LEFT, (UINT8 *)szTime);
	}
}

/*-------------------------------------------------------------------------
*  File Name : appViewOsd_VideoRecIconDraw
*------------------------------------------------------------------------*/
void
appViewOsd_VideoRecIconDraw(
	BOOL bShow
)
{
	if(bShow)
	{
		appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_1);
			appOsdLib_OsdLayIconDraw(APPVIEW_REAL_ISO_ICON_X, 0,  SP5K_GFX_ALIGN_TOP_RIGHT, ID_ICON_VIDEO_RECORD);
		}
	else
	{
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
			appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 270, 0,APPVIEW_ICON_SIZEX+5, APPVIEW_ICON_SIZEY, 0, 0);
		}
}

/*-------------------------------------------------------------------------
*  File Name : appViewOsd_VideoPauseIconDraw
*------------------------------------------------------------------------*/
void
appViewOsd_VideoPauseIconDraw(
	BOOL bShow
)
{
	if(bShow)
	{
		appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_1);
			appOsdLib_OsdLayIconDraw(APPVIEW_REAL_ISO_ICON_X, 0,SP5K_GFX_ALIGN_TOP_RIGHT, ID_ICON_VIDEO_RECORD_PAUSE);
	}
	else
	{
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
			appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 270, 0,APPVIEW_ICON_SIZEX+5, APPVIEW_ICON_SIZEY, 0, 0);
	}
}

/*-------------------------------------------------------------------------
*  File Name : appViewOsd_VideoTotalFreeTimeDraw
*------------------------------------------------------------------------*/
void
appViewOsd_VideoRecControlDraw(
	UINT8 Recstatus
)
{
	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,LCD_SIZE_X/2-APPVIEW_ICON_SIZEX, VIDEO_REC_CONTROL_Y,APPVIEW_ICON_SIZEX+FONT_W*5, FONT_H, 0, 0);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, VIDEO_CLEAN_RECICON_X, 0, VIDEO_CLEAN_RECICON_WIDTH_X, VIDEO_CLEAN_RECICON_HEIGHT_Y, 0, 0);	/* Clean REC pause/REC status icon */

	appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_1);
	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayIconDraw(LCD_SIZE_X/2-APPVIEW_ICON_SIZEX+8, VIDEO_REC_CONTROL_Y+8, SP5K_GFX_ALIGN_CENTER, ID_ICON_MEDIA_RIGHT_ARROW);	;/*willow modify@20100611*/

	if(pViewParam->videoStamp){
		appOsd_GfxFontIDSet(SP5K_GFX_FONT_ID_1);
	}
	switch(Recstatus)
	{
		case VIDEO_REC_RECORDING:
			sp5kGfxPageClear(SP5K_GFX_PAGE_PIP_0,0); /*ICOM-3403*/
			sp5kGfxPageClear(SP5K_GFX_PAGE_OSD_0,0); /*ICOM-3403*/
			appOsdLib_TextIDDraw(VIDEO_RECPAUSE_STR_PROMPT_X, VIDEO_RECPAUSE_STR_PROMPT_Y,SP5K_GFX_ALIGN_TOP_LEFT,STRTK_REC);
			appViewOsd_VideoPauseIconDraw(TRUE);
			break;
		case VIDEO_REC_PAUSE:
			sp5kGfxPageClear(SP5K_GFX_PAGE_PIP_0,0); /*ICOM-3403*/
			sp5kGfxPageClear(SP5K_GFX_PAGE_OSD_0,0); /*ICOM-3403*/
			appOsdLib_TextIDDraw(VIDEO_RECPAUSE_STR_PROMPT_X, VIDEO_RECPAUSE_STR_PROMPT_Y,SP5K_GFX_ALIGN_TOP_LEFT,STRTK_PAUSE);
			appViewOsd_VideoRecIconDraw(TRUE);
			break;
		default:
		case VIDEO_REC_STOP:
			break;
	}
	if(pViewParam->videoStamp){
		appOsd_GfxFontIDSet(SP5K_GFX_STAMP);
	}
}

/*-------------------------------------------------------------------------
*  File Name : appViewOsd_ZoomBarProcess
*------------------------------------------------------------------------*/
void
appViewOsd_ZoomBarProcess(
	UINT32 param
)
{
	//UINT32 curState;

	if (param == ZoomBarCounter)
	{
		UINT32 curState;
		curState = appActiveStateGet();
		if(curState != APP_STATE_STILL_PREVIEW
			&& curState != APP_STATE_VIDEO_PREVIEW
			&& curState != APP_STATE_VIDEO_REC)
		{
			appTimerClear(&ZoomBarCounter);
			return;
		}
		else if(!IS_VIEW_GROUP(curState) && !IS_VIDEO_GROUP(curState))
		{
			DBG_PRINT("sosd: %s: not group\n", __FUNCTION__);
			appTimerClear(&ZoomBarCounter);
			return;
		}

		appViewOsd_ZoomFrameDraw(FALSE);
		appTimerClear(&ZoomBarCounter);
		//for overlay with the guid line
		if((IS_PASM_PMODE && gStillCB.osd==OSD_VIEW_GRID)
			||(IS_VIDEO_MODE && gVideoCB.osd==OSD_VIEW_GRID))
		{
			appOsd_ColorDrawShapeColorSet(PAL_WHITE, PAL_BLEND_100, PAL_WHITE, PAL_BLEND_100);
			appOsdLib_OsdLayerLineDraw(LCD_SIZE_X-LCD_SIZE_X/3	,0	,LCD_SIZE_X-LCD_SIZE_X/3 ,LCD_SIZE_Y/4);
		}
	}
}

/*-------------------------------------------------------------------------
*  File Name : appViewOsd_ZoomBarProcess
*------------------------------------------------------------------------*/
void
appViewOsd_ZoomBarProcessStart()
{
	if(ZoomBarCounter != TIMER_NULL)
	{
		appTimerClear(&ZoomBarCounter);
	}
	ZoomBarCounter=appTimerSet(2000, "ZOOMBAR");
}

/*-------------------------------------------------------------------------
*  Function Name : appViewOsd_ZoomBarDestroy
*  Description :
*------------------------------------------------------------------------*/
void
appViewOsd_ZoomBarDestroy(void)
{
	if(ZoomBarCounter != TIMER_NULL)
	{
		appTimerClear(&ZoomBarCounter);
		appViewOsd_ZoomFrameDraw(FALSE);
	}
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_AllClean
 *  Description :
*------------------------------------------------------------------------*/
void
appViewOsd_AllClean(void)
{

	CLEAR_OSD_SRC;
	CLEAR_PIP_SRC;
	ViewOsdPresentStatus=0;
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_RefreshEffectTest
 *  Description : snap osdt2
*------------------------------------------------------------------------*/
void
appViewOsd_RefreshEffectTest(void)
{
	int i, j;
	//UINT32 t1, t2;

	APP_OSD_REFRESH_ON;

	for(j=0; j<10; j++)
	{
		//t1=appMsTimeGet();
		for(i=0; i<1000; i++)
		{
			appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_1);
			appOsdLib_OsdLayIconDraw(APPVIEW_SHAKE_X,APPVIEW_SHAKE_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_SCENE_SHAKEFREE_OFF);
		}
		//t2=appMsTimeGet();
		DBG_PRINT("sosd: normal draw 1000 icons (%d) = %dms\n", j, t2-t1);
	}

	APP_OSD_REFRESH_OFF;

	for(j=0; j<10; j++)
	{
		//t1=appMsTimeGet();
		for(i=0; i<1000; i++)
		{
			appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_1);
			appOsdLib_OsdLayIconDraw(APPVIEW_SHAKE_X,APPVIEW_SHAKE_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_SCENE_SHAKEFREE_OFF);
		}
		//t2=appMsTimeGet();
		DBG_PRINT("sosd: no-refresh 1000 draw icon (%d) = %dms\n", j, t2-t1);
	}
	APP_OSD_REFRESH_ON;
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_ModeIconTest
 *  Description : snap osdt2
*------------------------------------------------------------------------*/
void
appViewOsd_ModeIconTest(void)
{
	int i, j;
	//UINT32 t1, t2;

	APP_OSD_REFRESH_OFF;
	for(j=0; j<10; j++)
	{
		appViewOsd_AllClean();
		//t1=appMsTimeGet();
		for(i=0; i<100; i++)
		{
			appViewOsdShow(0,0, OSD_VIEW_SIMPLE);
		}
		//t2=appMsTimeGet();
		DBG_PRINT("sosd: refres off x100 viewosdinit (%d) = %dms\n", j, t2-t1);
	}
	APP_OSD_REFRESH_ON;
	for(j=0; j<10; j++)
	{
		appViewOsd_AllClean();
		//t1=appMsTimeGet();
		for(i=0; i<1000; i++)
		{
			appViewOsdShow(0,0, OSD_VIEW_SIMPLE);
		}
		//t2=appMsTimeGet();
		DBG_PRINT("sosd: refres on x100 viewosdinit (%d) = %dms\n", j, t2-t1);
	}
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_DriveModeCountDownDraw
 *  Description :
*------------------------------------------------------------------------*/

void appViewOsd_DriveModeCountDownDraw(UINT8 Second)
{
	if(Second>11)
	{
		return;
	}
	else if(!Second)
	{
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_CIRCLE,112,72,0,0,96,96);
		appOsd_ColorTransparentShapeColorSet(PAL_NOCOLOR,PAL_NOCOLOR,PAL_BLEND_100);
		sp5kGfxCircleDraw(0+1,160,120,46);
		return;
	}

	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 130, 99, 60, 42, 0, 0);
	appOsd_ColorBlendingSet(PAL_GRAY_1,PAL_BLEND_50);
	#if (KIT_WITH_HDMI)
	if(IS_HDMI_IN){
		appOsd_FontIconFontSet(ID_FONT_TIME_HDMI);
	}else
	#endif
	{
	appOsd_FontIconFontSet(ID_FONT_TK_TIME);
	}
	appOsd_ColorTransparentShapeColorSet(PAL_GRAY_1,PAL_GRAY_1,PAL_BLEND_50);
	sp5kGfxCircleDraw(0+1,160,120,46);
	appOsdLib_OsdLayIconDraw(112,72,SP5K_GFX_ALIGN_TOP_LEFT,Second);
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_BlackScreenDraw
 *  Description :
*------------------------------------------------------------------------*/
void
appViewOsd_BlackOsdLayerDraw(void)
{
	printf("%s\n", __func__);
	appOsd_ColorDrawShapeColorSet(PAL_BLACK, PAL_BLEND_100, PAL_BLACK, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 0, 0, LCD_SIZE_X, LCD_SIZE_Y, 0, 0);
	APP_SLEEP_MS(20);
	//appJpgFileDraw(SP5K_RES_TYPE_JPG, RES_JPG_BLACK, 0, 0, 0, 0);
}

/*-------------------------------------------------------------------------
*	 Func. Name : appViewOsd_DlgSetandDraw()
*------------------------------------------------------------------------*/
void appViewOsd_DlgSetandDraw( unsigned short wTitle,  unsigned short SelectIndex)
{
     // wTitle= wTitle; /*avoid warning*/

	appPlayOsd_DlgBoxConfig(STRTK_IMG_SAVED, 2, DlgConfirmYesNo, 0, 50);
	appPlayOsd_DlgBoxShow();

}

void appViewOsd_WiFiIconDraw(COLOR_DEF color)
{
	appOsd_ColorDrawTextColorSet(color, PAL_NOCOLOR, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_TextDraw(APPVIEW_WIFI_STRING_X, APPVIEW_WIFI_STRING_Y, SP5K_GFX_ALIGN_TOP_LEFT,(UINT8 *)"WiFi");
}

