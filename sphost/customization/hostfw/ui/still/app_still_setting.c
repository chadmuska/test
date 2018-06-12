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

#include "sp5k_cdsp_api.h"
#include "sp5k_capture_api.h"
#include "sp5k_af_api.h"
#include "sp5k_rsvblk_api.h"
#include "sp5k_awb_api.h"
#include "sp5k_cdsp_api.h"
#include "sp5k_utility_api.h"
#include "sp5k_global_api.h"
#include "sp5k_dbg_api.h"
#include "middleware/sysg_resource_def.h"

#include "app_com_api.h"
#include "app_com_def.h"
#include "customization/app_init.h"
#include "app_osd.h"
#include "app_user_setting.h"
#include "app_view_param.h"
#include "app_zoom_api.h"
#include "app_util.h"
#include "app_rtc.h"
#include "app_still.h"
#include "app_video.h"
#include "app_sensor.h"
#include "app_menu.h"
#include "app_strobe.h"
#include "app_led.h"
#include "app_battery.h"
#include "app_aaa.h"
#include "app_exif.h"
#include "app_view_osd.h"
#include "app_calib_api.h"
#include "app_awbalg_api.h"
#include "app_flash_api.h"
#include "app_callback_setting.h"

#if SP5K_DIQ_FRMWRK
#include "api/sp5k_dq_api.h"
#endif

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
/* JFIF header + EXIF header(thumbnail included) + QV*/
#define IMAGE_FILE_HEADER 64*1024

#if SNAP_CAPTURE_MODE == 1
#define CAPTURE_MODE_SINGLE		SP5K_CAPTURE_MODE_SINGLE
#define CAPTURE_MODE_BURST		SP5K_CAPTURE_MODE_BURST
#else
#define CAPTURE_MODE_SINGLE		SP5K_CAPTURE_MODE_SINGLE
#define CAPTURE_MODE_BURST		SP5K_CAPTURE_MODE_BURST
#endif

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
extern UINT32 sp5kStillCapCallbackSet(UINT32 selector,void * pfunc);
extern void * sp5kYuvBufferAlloc(UINT32 bufW,UINT32 bufH);
extern void sp5kYuvBufferFree(void * ptr);

sp5kCaptureStamp_t gStampInfo =
{
	.dateFmtId = SP5K_CAPTURE_DATE_YYYYMMDD,
	.dateLen = 0,
	.pdate = NULL,
	.scaleRatio = 100,
	.x=100,
	.y=100,
	.backgndColor = 0,
	.fontColor = 19,
	.OutlineColor = 6,
};

 typedef struct {
	SINT16 roffset;
	SINT16 groffset;
	SINT16 boffset;
	SINT16 gboffset;
}WbOffset_t;

/*you can set size as follow list
{ 3648, 2736, ID_ICON_SNAPSHOTMENU_RESOLUTION_10M},
{ 3456, 2592, ID_ICON_SNAPSHOTMENU_RESOLUTION_9M },
{ 3264, 2448, ID_ICON_SNAPSHOTMENU_RESOLUTION_8M },
{ 3072, 2304, ID_ICON_SNAPSHOTMENU_RESOLUTION_7M},
{ 2816, 2112, ID_ICON_SNAPSHOTMENU_RESOLUTION_6M },
{ 2560, 1920, ID_ICON_SNAPSHOTMENU_RESOLUTION_5M },
{ 2272, 1704, ID_ICON_SNAPSHOTMENU_RESOLUTION_4M },
{ 2048, 1536, ID_ICON_SNAPSHOTMENU_RESOLUTION_3M },
{ 1600,  1200, ID_ICON_SNAPSHOTMENU_RESOLUTION_2M },
{ 1024,  768, ID_ICON_SNAPSHOTMENU_RESOLUTION_1M },

{ 3264, 2448, ID_ICON_SNAPSHOTMENU_RESOLUTION_8M },
{ 3264, 2176, ID_ICON_SNAPSHOTMENU_RESOLUTION_32 },
{ 3264, 1840, ID_ICON_SNAPSHOTMENU_RESOLUTION_169},

{ 3072, 2304, ID_ICON_SNAPSHOTMENU_RESOLUTION_7M },
{ 3072, 2048, ID_ICON_SNAPSHOTMENU_RESOLUTION_32 },
{ 3072, 1728, ID_ICON_SNAPSHOTMENU_RESOLUTION_169},

{ 3648, 2736, ID_ICON_SNAPSHOTMENU_RESOLUTION_10M},
{ 3648, 2432, ID_ICON_SNAPSHOTMENU_RESOLUTION_32 },
{ 3648, 2048, ID_ICON_SNAPSHOTMENU_RESOLUTION_169},

{ 2560, 1920, ID_ICON_SNAPSHOTMENU_RESOLUTION_5M },
{ 2560, 1706, ID_ICON_SNAPSHOTMENU_RESOLUTION_32 },
{ 2560, 1440, ID_ICON_SNAPSHOTMENU_RESOLUTION_169},
*/

static struct
{
	UINT16 w,h;
	UINT32 icon;
} ImgSize[UI_STILL_SIZE_MAX]=
{
	{ DEFAULT_CAPTURE_W, DEFAULT_CAPTURE_H, DEFAULT_CAPTURE_ICON },
};

static UINT8 compressRatio[UI_QUALITY_MAX]={ 5, 10, 12 };

UINT32 focus_idx=0;
UINT32 flash_idx=0;
UINT32 dateStampFmtId = 0;

/* 1. The first item is default setting.
** 2. When mode chang the idx will reset to default.
*/
typedef struct buttonParam_s
{
	UINT32 id;
	UINT8 focus[6];
	UINT8 flash[6];
	SINT32 shutterBoundary[2];
	UINT8 metering[4];
	UINT32 sharpness;
	UINT32 stablizer;
	UINT32 facetracking;
} buttonParam_t;

static buttonParam_t* pCurBtnPar=0;
static buttonParam_t buttonParam[PASM_MAX] =
{
	{
		PASM_PROGRAM,
		{UI_FOCUS_NORMAL, UI_FOCUS_MACRO, UI_FOCUS_INFINITY, UI_FOCUS_MAX},
		{UI_FLASH_OFF, UI_FLASH_RED_EYE, UI_FLASH_ON, UI_FLASH_SLOW_SYNC, UI_FLASH_OFF, UI_FLASH_MAX},
		{SHUTTER_1S, SHUTTER_1_16000S}
	},
	{
		VIDEO_MODE,
		{UI_FOCUS_NORMAL, UI_FOCUS_MACRO, UI_FOCUS_INFINITY, UI_FOCUS_MAX},
		{UI_FLASH_OFF, UI_FLASH_MAX},
		{SHUTTER_1_15S, SHUTTER_1_2000S}
	},
};

#ifdef HW_DIST
BOOL gDistEnTag=FALSE;
#endif

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/*-------------------------------------------------------------------------
 *  Function Name : appStill_ViewMenuSelFunctionCB
 *  Description :   This callback will response and record the user setting
 *					in menu. It will take charge in still/video menu. Some
 *					items need immeditly response to the user selection.
 *------------------------------------------------------------------------*/

void appStill_ViewMenuSelFunctionCB(
	UINT8 SelPara,
	PappMenuInfo_t CurTab,
	PappSubMenuInfo_t CurItem
)
{
	if(SelPara == MENU_ACTION_SELECT)
	{
		switch(CurTab->tab[CurTab->curTab - 1])
		{
		case VIEWMENU_STILL_RESOLUTION:
			appStill_SetResolution(CurItem->curItem);
			break;
		case VIEWMENU_VIDEO_SIZE:
			break;
		default:
			break;
		}
	}

	if(SelPara == MENU_ACTION_RETURN)
	{
		switch(CurTab->tab[CurTab->curTab - 1])
		{
		case VIEWMENU_STILL_RESOLUTION:
			appStill_SetResolution(pViewParam->stillSize);
			break;
		case VIEWMENU_VIDEO_SIZE:
            appVideoSizeSet();
			break;
		default:
			break;
		}
	}

	if(SelPara == MENU_ACTION_SET)
	{
		switch(CurTab->tab[CurTab->curTab - 1])
		{
    		case VIEWMENU_CHANGE:
    			break;

    		case VIEWMENU_STILL_RESOLUTION:
    			pViewParam->stillSize = CurItem->curItem;
    			appStill_SetResolution(pViewParam->stillSize);
    			appStill_SetDateStamp(pViewParam->stillStamp, pUiSetting->dateTime);
    			appStill_SetQuality(pViewParam->stillQuality);
    			break;

			case VIEWMENU_STILL_QUALITY:
				pViewParam->stillQuality = CurItem->curItem;
				appStill_SetQuality(pViewParam->stillQuality);
				break;

    		case VIEWMENU_STILL_DRIVERMODE:
    			pViewParam->stillDriverMode = CurItem->curItem;
    			break;

    		case VIEWMENU_DATESTAMP:
    			pViewParam->stillStamp = CurItem->curItem;
    			appStill_SetDateStamp(pViewParam->stillStamp, pUiSetting->dateTime);
    			break;

    		case VIEWMENU_VIDEO_SIZE:
    			pViewParam->videoSize = CurItem->curItem;
    			appVideoSizeSet();
    			break;

			case VIEWMENU_VIDEO_QUALITY:
    			pViewParam->videoQuality = CurItem->curItem;
    			appVideoSetQuality();
    			break;

			case VIEWMENU_VIDEO_SEAMLESS:
    			pViewParam->videoSeamless = CurItem->curItem;
    			break;

    		case VIEWMENU_VIDEO_DATESTAMP:
    			pViewParam->videoStamp = CurItem->curItem;
    			break;

    		default:
    			break;
    	}

		UiParamViewFlush();
	}

}

/*-------------------------------------------------------------------------
 *  Function Name : appStillEstFileSize
 *  Description :
 *------------------------------------------------------------------------*/
UINT32
appStillEstFileSize(
	UINT32 imgSizeIdx,
	UINT32 qualityIdx,
	UINT32 clustersize //in k byte
)
{
	UINT32 w, h, ratio;
	UINT32 predictVLCSize, predictFileSize;

	w = ImgSize[imgSizeIdx].w;
	h = ImgSize[imgSizeIdx].h;

	ratio = compressRatio[qualityIdx];
	if(clustersize==0)
	{
		clustersize = 1024;
	}

	predictVLCSize = w * h * 2 / ratio;
	predictVLCSize = predictVLCSize * 120 / 100; /* BRC also has 20% tolerance but normally it is not that big */

	predictFileSize = (predictVLCSize + IMAGE_FILE_HEADER)>>10;

	if (predictFileSize % clustersize)
	{
		predictFileSize += clustersize;
	}
	predictFileSize = (predictFileSize / clustersize) * clustersize;
	return predictFileSize;//in k byte
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetResolution
 *  Description :   This is for normal user seting. It only change the view
 					size by drawing black strips. The real sp5k setting will
 					be set by appStill_SetSp5kStillSize() before capture.
 *------------------------------------------------------------------------*/
void
appStill_SetResolution(
	UINT8 idx
)
{
    #if SP5K_SENSOR_DUAL_MODE
    return; /*avoid video preview to still view flick*/
    #else
	appViewOsdValidAreaShow(ImgSize[idx].w, ImgSize[idx].h);
    #endif
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetSp5kStillSize
 *  Description :   This function will be called in capture/burst init for
 					real size. And it is set to biggest size when return to
 					view. Sp5k still/viedo view/capture/record mode change
 					all need to config the size before you change. In still,
 					we will always use max size for view and real size for
 					capture.
 *------------------------------------------------------------------------*/
void
appStill_SetSp5kStillSize(
	UINT8 idx
)
{
	UINT32 ret;
	sp5kImgSize_t capSize;

	capSize.width = ImgSize[idx].w;
	capSize.height= ImgSize[idx].h;
    HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "set: %s %d",__FUNCTION__,idx);
	ret = sp5kStillCapCfgSet(SP5K_CAPTURE_MAIN_IMAGESIZE, (UINT32)&capSize);

    if(ret!=SUCCESS)
       HOST_PROF_LOG_PRINT(LEVEL_ERROR, "set: %s %d",__FUNCTION__,__LINE__);
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_GetCurSize()
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_GetCurSize(
	UINT32* w,
	UINT32* h
)
{
	*w = ImgSize[pViewParam->stillSize].w;
	*h = ImgSize[pViewParam->stillSize].h;
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_GetCurSizeIcon()
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_GetCurSizeIcon(
	UINT32* icon
)
{
	*icon = ImgSize[pViewParam->stillSize].icon;
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetQuality
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_SetQuality(
	UINT8 idx
)
{
	UINT32 cfg, ret;

	if( pViewParam->stillSize == UI_STILL_SIZE_VGA ) {
      	/*VGA size should set compression ratio to 200 */
		cfg = 2;
	}
    else
	{
		cfg = compressRatio[idx];
	}
	DBG_PRINT("set: snap quality: idx:%d cfg:%d\n",idx ,cfg );
	/* Configure compression ration must do modeSet again */
	ret = sp5kStillCapCfgSet(SP5K_CAPTURE_MAIN_COMPRESSION_RATIO, cfg*100);

    if(ret!=SUCCESS)
       HOST_PROF_LOG_PRINT(LEVEL_ERROR, "set: %s %d",__FUNCTION__,__LINE__);
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetMetering
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_SetMetering(
	UINT8 idx
)
{
	appAeMeterSet(idx);
	appExif_SetMeterMode(idx);
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetWB
 *  Description :
 *------------------------------------------------------------------------*/
extern UINT16 AwbCT_Rgain[3];
extern UINT16 AwbCT_Bgain[3];
//extern _AWBALG_MODE_SEL _gvMWBSwitch;
void
appStill_SetWB(
	UINT8 idx
)
{
	UINT16 one=0x400;
	UINT16 exifWbMode;
	appWbGain_t wb;
    wb.rgain = wb.bgain = 0;
	wb.gbgain = wb.grgain = one;

	switch (idx)
	{
		case UI_WB_AUTO:
			appAWBALG_WBModeSet(AWB_MODE_Auto);/*WB_MODE_AUTO*/
			break;
		case UI_WB_DAYLIGHT:
			appAWBALG_WBModeSet(AWB_MODE_DayLight);/*WB_MODE_DAYLIGHT*/
			break;
		case UI_WB_CLOUDY:
			appAWBALG_WBModeSet(AWB_MODE_Cloudy);/*WB_MODE_CLOUDY*/
			break;
		case UI_WB_TUNGSTEN:
			appAWBALG_WBModeSet(AWB_MODE_Tungsten);/*WB_MODE_TUNGSTEN*/
			break;
		case UI_WB_FLUORESCENT_H:
            appAWBALG_WBModeSet(AWB_MODE_Fluorescent_H);/*WB_MODE_FLUORESCENT_H*/
			break;
		case UI_WB_FLUORESCENT_L:
            appAWBALG_WBModeSet(AWB_MODE_Fluorescent_L);/*WB_MODE_FLUORESCENT_H*/
			break;
		case UI_WB_FLUORESCENT:
			appAWBALG_WBModeSet(AWB_MODE_Fluorescent);/*WB_MODE_FLUORESCENT*/
			break;
		case UI_WB_CUSTOM:
			break;
		case UI_WB_UNDERWATER:
			appAWBALG_WBModeSet(AWB_MODE_Auto_Water);/*WB_MODE_UNDERWATER*/

			break;
	}
	if (idx == UI_WB_CUSTOM)
	{
		exifWbMode = SP5K_EXIF_TAGDATA_WB_MANUAL;
	}
	else
	{
		exifWbMode = SP5K_EXIF_TAGDATA_WB_AUTO;
	}
	appExifSetWBMode((UINT8*)&exifWbMode);

	gStillCB.awb.wbMode = idx;
	gStillCB.awb.rGain = wb.rgain;
	gStillCB.awb.bGain = wb.bgain;

}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetISO
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_SetISO(
	UINT8 idx
)
{
	appAeCapIsoSet(idx);
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetExposure
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_SetExposure(
	UINT8 idx
)
{
	DBG_PRINT("set: %s: idx=%d\n", __FUNCTION__, idx);
	appAeEvBiasSet(idx);
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetDriveModeBuffer
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_SetDriveModeBuffer(
	UINT8 idx
)
{
	UINT32 mode;
	sp5kModeGet(&mode);
	if(mode != SP5K_MODE_STANDBY)
	{
		sp5kModeCfgSet(SP5K_MODE_CFG_STANDBY_DUP, 1);
        appModeSet(SP5K_MODE_STANDBY);
	}

#if SP5K_DIQ_FRMWRK
    sp5kDynIqCapAnrFlow_t *anrFlow;

	switch(idx)
	{
    	case UI_STILL_DRIVERMODE_OFF:
            sp5kStillCapCfgSet(SP5K_CAPTURE_RAW_BUF_NUM, DEF_RAW_BUFFER);
            sp5kDynIqCfgGet(SP5K_DYN_IQ_CFG_CAP_ANR_FLOW, &anrFlow);
            if(anrFlow->anrEn)
                sp5kStillCapCfgSet(SP5K_CAPTURE_YUV_BUF_NUM, ANR_YUV_BUFFER);
            else{
                if(anrFlow->anrLiteEn){
                    sp5kStillCapCfgSet(SP5K_CAPTURE_YUV_BUF_NUM, ANRL_YUV_BUFFER);
                }
                else{
                    sp5kStillCapCfgSet(SP5K_CAPTURE_YUV_BUF_NUM, DEF_YUV_BUFFER);
                }
            }
            sp5kStillCapCfgSet(SP5K_CAPTURE_VLC_BUF_NUM, DEF_VLC_BUFFER);
    		break;

        case UI_STILL_DRIVERMODE_AEB:
            sp5kStillCapCfgSet(SP5K_CAPTURE_RAW_BUF_NUM, AEB_RAW_BUFFER);
            sp5kDynIqCfgGet(SP5K_DYN_IQ_CFG_CAP_ANR_FLOW, &anrFlow);
            if(anrFlow->anrEn)
                sp5kStillCapCfgSet(SP5K_CAPTURE_YUV_BUF_NUM, ANR_YUV_BUFFER);
            else{
                if(anrFlow->anrLiteEn){
                    sp5kStillCapCfgSet(SP5K_CAPTURE_YUV_BUF_NUM, ANRL_YUV_BUFFER);
                }
                else{
                    sp5kStillCapCfgSet(SP5K_CAPTURE_YUV_BUF_NUM, AEB_YUV_BUFFER);
                }
            }
            sp5kStillCapCfgSet(SP5K_CAPTURE_VLC_BUF_NUM, AEB_VLC_BUFFER);
            break;

        case UI_STILL_DRIVERMODE_HDR:
        	sp5kStillCapCfgSet(SP5K_CAPTURE_RAW_BUF_NUM, HDR_RAW_BUFFER);
            sp5kStillCapCfgSet(SP5K_CAPTURE_YUV_BUF_NUM, HDR_RAW_BUFFER);
        	sp5kStillCapCfgSet(SP5K_CAPTURE_VLC_BUF_NUM, HDR_VLC_BUFFER);
            break;

        case UI_STILL_DRIVERMODE_BURST_H:
    #if defined(SP5K_SENSOR_SONYIMX206)
        #if SP5K_HW_DDR3_PARM_2G
           	sp5kStillCapCfgSet(SP5K_CAPTURE_RAW_BUF_NUM, 5);
            sp5kStillCapCfgSet(SP5K_CAPTURE_YUV_BUF_NUM, 4);
            sp5kStillCapCfgSet(SP5K_CAPTURE_VLC_BUF_NUM, 4);
        #else
           	sp5kStillCapCfgSet(SP5K_CAPTURE_RAW_BUF_NUM, 3);
            sp5kDynIqCfgGet(SP5K_DYN_IQ_CFG_CAP_ANR_FLOW, &anrFlow);
            if(anrFlow->anrEn)
                sp5kStillCapCfgSet(SP5K_CAPTURE_YUV_BUF_NUM, ANR_YUV_BUFFER);
            else{
                if(anrFlow->anrLiteEn){
                    sp5kStillCapCfgSet(SP5K_CAPTURE_YUV_BUF_NUM, ANRL_YUV_BUFFER);
                }
                else{
                    sp5kStillCapCfgSet(SP5K_CAPTURE_YUV_BUF_NUM, BURST_H_YUV_BUFFER);
                }
            }
            sp5kStillCapCfgSet(SP5K_CAPTURE_VLC_BUF_NUM, 1);
        #endif	/* SP5K_HW_DDR3_PARM_2G */
    #else
        	sp5kStillCapCfgSet(SP5K_CAPTURE_RAW_BUF_NUM, BURST_H_RAW_BUFFER);
            sp5kDynIqCfgGet(SP5K_DYN_IQ_CFG_CAP_ANR_FLOW, &anrFlow);
            if(anrFlow->anrEn)
                sp5kStillCapCfgSet(SP5K_CAPTURE_YUV_BUF_NUM, ANR_YUV_BUFFER);
            else{
                if(anrFlow->anrLiteEn){
                    sp5kStillCapCfgSet(SP5K_CAPTURE_YUV_BUF_NUM, ANRL_YUV_BUFFER);
                }
                else{
                    sp5kStillCapCfgSet(SP5K_CAPTURE_YUV_BUF_NUM, BURST_H_YUV_BUFFER);
                }
            }
            sp5kStillCapCfgSet(SP5K_CAPTURE_VLC_BUF_NUM, BURST_H_VLC_BUFFER);
    #endif
            break;

        case UI_STILL_DRIVERMODE_BURST_L:
    #if defined(SP5K_SENSOR_SONYIMX206)
        #if SP5K_HW_DDR3_PARM_2G
           	sp5kStillCapCfgSet(SP5K_CAPTURE_RAW_BUF_NUM, 5);
            sp5kStillCapCfgSet(SP5K_CAPTURE_YUV_BUF_NUM, 4);
            sp5kStillCapCfgSet(SP5K_CAPTURE_VLC_BUF_NUM, 4);
        #else
           	sp5kStillCapCfgSet(SP5K_CAPTURE_RAW_BUF_NUM, 3);
            sp5kDynIqCfgGet(SP5K_DYN_IQ_CFG_CAP_ANR_FLOW, &anrFlow);
            if(anrFlow->anrEn)
                sp5kStillCapCfgSet(SP5K_CAPTURE_YUV_BUF_NUM, ANR_YUV_BUFFER);
            else{
                if(anrFlow->anrLiteEn){
                    sp5kStillCapCfgSet(SP5K_CAPTURE_YUV_BUF_NUM, ANRL_YUV_BUFFER);
                }
                else{
                    sp5kStillCapCfgSet(SP5K_CAPTURE_YUV_BUF_NUM, BURST_L_YUV_BUFFER);
                }
            }
            sp5kStillCapCfgSet(SP5K_CAPTURE_VLC_BUF_NUM, 3);
        #endif	/* SP5K_HW_DDR3_PARM_2G */
    #else
        	sp5kStillCapCfgSet(SP5K_CAPTURE_RAW_BUF_NUM, BURST_L_RAW_BUFFER);
            sp5kDynIqCfgGet(SP5K_DYN_IQ_CFG_CAP_ANR_FLOW, &anrFlow);
            if(anrFlow->anrEn)
                sp5kStillCapCfgSet(SP5K_CAPTURE_YUV_BUF_NUM, ANR_YUV_BUFFER);
            else{
                if(anrFlow->anrLiteEn){
                    sp5kStillCapCfgSet(SP5K_CAPTURE_YUV_BUF_NUM, ANRL_YUV_BUFFER);
                }
                else{
                    sp5kStillCapCfgSet(SP5K_CAPTURE_YUV_BUF_NUM, BURST_L_YUV_BUFFER);
                }
            }
            sp5kStillCapCfgSet(SP5K_CAPTURE_VLC_BUF_NUM, BURST_L_VLC_BUFFER);
    #endif
            break;
	}

#else
	switch(idx)
	{
    	case UI_STILL_DRIVERMODE_OFF:
            sp5kStillCapCfgSet(SP5K_CAPTURE_RAW_BUF_NUM, DEF_RAW_BUFFER);
            sp5kStillCapCfgSet(SP5K_CAPTURE_YUV_BUF_NUM, DEF_YUV_BUFFER);
            sp5kStillCapCfgSet(SP5K_CAPTURE_VLC_BUF_NUM, DEF_VLC_BUFFER);
    		break;

        case UI_STILL_DRIVERMODE_AEB:
            sp5kStillCapCfgSet(SP5K_CAPTURE_RAW_BUF_NUM, AEB_RAW_BUFFER);
            sp5kStillCapCfgSet(SP5K_CAPTURE_YUV_BUF_NUM, AEB_YUV_BUFFER);
            sp5kStillCapCfgSet(SP5K_CAPTURE_VLC_BUF_NUM, AEB_VLC_BUFFER);
            break;

        case UI_STILL_DRIVERMODE_HDR:
        	sp5kStillCapCfgSet(SP5K_CAPTURE_RAW_BUF_NUM, HDR_RAW_BUFFER);
            sp5kStillCapCfgSet(SP5K_CAPTURE_YUV_BUF_NUM, HDR_RAW_BUFFER);
        	sp5kStillCapCfgSet(SP5K_CAPTURE_VLC_BUF_NUM, HDR_VLC_BUFFER);
            break;

        case UI_STILL_DRIVERMODE_BURST_H:
    #if defined(SP5K_SENSOR_SONYIMX206)
        #if SP5K_HW_DDR3_PARM_2G
           	sp5kStillCapCfgSet(SP5K_CAPTURE_RAW_BUF_NUM, 5);
            sp5kStillCapCfgSet(SP5K_CAPTURE_YUV_BUF_NUM, 4);
            sp5kStillCapCfgSet(SP5K_CAPTURE_VLC_BUF_NUM, 4);
        #else
           	sp5kStillCapCfgSet(SP5K_CAPTURE_RAW_BUF_NUM, 3);
            sp5kStillCapCfgSet(SP5K_CAPTURE_YUV_BUF_NUM, 1);
            sp5kStillCapCfgSet(SP5K_CAPTURE_VLC_BUF_NUM, 1);
        #endif	/* SP5K_HW_DDR3_PARM_2G */
    #else
        	sp5kStillCapCfgSet(SP5K_CAPTURE_RAW_BUF_NUM, BURST_H_RAW_BUFFER);
            sp5kStillCapCfgSet(SP5K_CAPTURE_YUV_BUF_NUM, BURST_H_YUV_BUFFER);
            sp5kStillCapCfgSet(SP5K_CAPTURE_VLC_BUF_NUM, BURST_H_VLC_BUFFER);
    #endif
            break;

        case UI_STILL_DRIVERMODE_BURST_L:
    #if defined(SP5K_SENSOR_SONYIMX206)
        #if SP5K_HW_DDR3_PARM_2G
           	sp5kStillCapCfgSet(SP5K_CAPTURE_RAW_BUF_NUM, 5);
            sp5kStillCapCfgSet(SP5K_CAPTURE_YUV_BUF_NUM, 4);
            sp5kStillCapCfgSet(SP5K_CAPTURE_VLC_BUF_NUM, 4);
        #else
           	sp5kStillCapCfgSet(SP5K_CAPTURE_RAW_BUF_NUM, 3);
            sp5kStillCapCfgSet(SP5K_CAPTURE_YUV_BUF_NUM, 1);
            sp5kStillCapCfgSet(SP5K_CAPTURE_VLC_BUF_NUM, 3);
        #endif	/* SP5K_HW_DDR3_PARM_2G */
    #else
        	sp5kStillCapCfgSet(SP5K_CAPTURE_RAW_BUF_NUM, BURST_L_RAW_BUFFER);
            sp5kStillCapCfgSet(SP5K_CAPTURE_YUV_BUF_NUM, BURST_L_YUV_BUFFER);
            sp5kStillCapCfgSet(SP5K_CAPTURE_VLC_BUF_NUM, BURST_L_VLC_BUFFER);
    #endif
            break;
	}
#endif
	appModeSet(SP5K_MODE_STILL_PREVIEW);
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetDriveMode
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_SetDriveMode(
	UINT8 idx
)
{
	UINT32 cap_mode, ret;
    cap_mode=CAPTURE_MODE_SINGLE;
    ret =  SUCCESS;
    static UINT8 preidx = 0xff;
	UINT32 mode;

    appAeAebSet(UI_STILL_DRIVERMODE_OFF);
    sp5kStillCapCfgSet(SP5K_CAPTURE_SR_ENABLE,0);
    sp5kStillCapCfgSet(SP5K_CAPTURE_IMG_MERGE_MODE, SP5K_CAPTURE_MERGE_MODE_OFF);

    if((appPreviousStateGet()== APP_STATE_STILL_CAPTURE) || (appPreviousStateGet()== APP_STATE_BURST_CAPTURE))
        sp5kModeCfgSet(SP5K_MODE_CFG_STANDBY_DUP, 1);

    if(preidx==idx)
    {
        HOST_PROF_LOG_ADD(LEVEL_DEBUG, "set: appStill_SetDriveMode return");
        if(pViewParam->stillSize!=UI_STILL_SIZE_FULL)
        {
        	sp5kModeGet(&mode);
			if(mode != SP5K_MODE_STANDBY)
        		appModeSet(SP5K_MODE_STANDBY);
            appStill_SetSp5kStillSize(UI_STILL_SIZE_FULL);
            appModeSet(SP5K_MODE_STILL_PREVIEW);
        }
        return;
    }else{
        preidx = idx;
    }
	/* Delete return because of host configure compression ratio need to do modeSet again, fix mantis 50671 */
    /*HOST_PROF_LOG_PRINT(LEVEL_INFO, "set: driver mode idx =%d", idx);*/

    sp5kStillCapCfgSet(SP5K_CAPTURE_RAW_DATA_TYPE, SP5K_CAPTURE_RAW_FMT_16_BIT);
    sp5kStillCapCfgSet(SP5K_CAPTURE_MAX_COMPRESSION_RATIO, DEF_COMPRESSION_RATIO);

	switch(idx)
	{
    	case UI_STILL_DRIVERMODE_OFF:
            if(appPreviousStateGet()!= APP_STATE_POWER_ON)
            {
                appStill_SetSp5kStillSize(UI_STILL_SIZE_FULL);
#if SP5K_DIQ_FRMWRK
                sp5kDynIqCfgSet(SP5K_DYN_IQ_CFG_CAP_ANR_FLOW_OFF_SET, SP5K_CDSP_CAP_ANR_FLOW,FALSE);
#endif
            }
            else
                preidx = 0xff;

    		gStillCB.remainImg = 1;
    		gStillCB.estJobFilesSize = gStillCB.estFileSize;
    		cap_mode = CAPTURE_MODE_SINGLE;
    		break;

        case UI_STILL_DRIVERMODE_AEB:
            appStill_SetSp5kStillSize(UI_STILL_SIZE_FULL);
#if SP5K_DIQ_FRMWRK
            sp5kDynIqCfgSet(SP5K_DYN_IQ_CFG_CAP_ANR_FLOW_OFF_SET, SP5K_CDSP_CAP_ANR_FLOW, FALSE);
#endif
            appAeAebSet(UI_STILL_DRIVERMODE_AEB);
    		gStillCB.remainImg = AEB_BURST_LENGTH;
    		gStillCB.estJobFilesSize = gStillCB.estFileSize*AEB_BURST_LENGTH;
            cap_mode = CAPTURE_MODE_BURST;
            break;

        case UI_STILL_DRIVERMODE_HDR:
            appStill_SetSp5kStillSize(UI_STILL_SIZE_FULL);
#if SP5K_DIQ_FRMWRK
            sp5kDynIqCfgSet(SP5K_DYN_IQ_CFG_CAP_ANR_FLOW_OFF_SET, SP5K_CDSP_CAP_ANR_FLOW, TRUE);
#endif
    		gStillCB.remainImg = 1;
    		gStillCB.estJobFilesSize = gStillCB.estFileSize;
    		cap_mode = CAPTURE_MODE_BURST;
            break;

        case UI_STILL_DRIVERMODE_BURST_H:
            appStill_SetSp5kStillSize(UI_STILL_SIZE_FULL);
#if SP5K_DIQ_FRMWRK
            sp5kDynIqCfgSet(SP5K_DYN_IQ_CFG_CAP_ANR_FLOW_OFF_SET, SP5K_CDSP_CAP_ANR_FLOW, FALSE);
#endif
            sp5kStillCapCfgSet(SP5K_CAPTURE_MAX_COMPRESSION_RATIO, BURST_COMPRESSION_RATIO);
            sp5kStillCapCfgSet(SP5K_CAPTURE_RAW_DATA_TYPE, SP5K_CAPTURE_RAW_FMT_8_BIT);
    		gStillCB.remainImg = 0xffff;
    		gStillCB.estJobFilesSize = gStillCB.estFileSize*HDR_BURST_LENGTH;
            cap_mode = CAPTURE_MODE_BURST;
            break;

        case UI_STILL_DRIVERMODE_BURST_L:
            appStill_SetSp5kStillSize(UI_STILL_SIZE_FULL);
#if SP5K_DIQ_FRMWRK
            sp5kDynIqCfgSet(SP5K_DYN_IQ_CFG_CAP_ANR_FLOW_OFF_SET, SP5K_CDSP_CAP_ANR_FLOW, FALSE);
#endif
            sp5kStillCapCfgSet(SP5K_CAPTURE_MAX_COMPRESSION_RATIO, BURST_COMPRESSION_RATIO);
            sp5kStillCapCfgSet(SP5K_CAPTURE_RAW_DATA_TYPE, SP5K_CAPTURE_RAW_FMT_8_BIT);
    		gStillCB.remainImg = 0xffff;
    		gStillCB.estJobFilesSize = gStillCB.estFileSize*HDR_BURST_LENGTH;
            cap_mode = CAPTURE_MODE_BURST;
            break;
	}
    appStill_SetDriveModeBuffer(idx);

	gStillCB.flashMode = UI_FLASH_OFF;
	appStill_SetFlash(gStillCB.flashMode);
	ret = sp5kStillCapCfgSet(SP5K_CAPTURE_MODE, cap_mode);

    if(ret!=SUCCESS)
       HOST_PROF_LOG_PRINT(LEVEL_ERROR, "set: %s %d",__FUNCTION__,__LINE__);
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetSharpness
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_SetSharpness(
	UINT8 idx
)
{
	appExif_SetSharpness(idx);
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_CustumStamp
 *  Description :
 *------------------------------------------------------------------------*/
UINT32 appStill_CustumStamp(
	UINT32 addr,
	UINT32 qvAddr,
	void* size
)
{
	UINT32 dataStampRoiW, dataStampRoiH, dataStampScaleW, dataStampScaleH;
	UINT8 *stillStampDateBuf = NULL; /* for string buffer */
	UINT8 *ConvertBuf = NULL;  /*for string scale buffer*/
	tmx_t rtc;

	UINT32 dataStampScaleRoiW, dataStampScaleRoiH;
	UINT32 roiX, roiY, ret=0;
	
	UINT32 _ret1,_ret2;//for cppcheck clean warning

	sp5kYuvCbImgSize_t* psSizeInfo = (sp5kYuvCbImgSize_t*)size;
	sp5kGfxPicture_t srcPic, dstPic;
	sp5kGfxObj_t gfxObjSrc,gfxObjDst;
	sp5kGfxPageCapabDesc_t cap;

	if (pViewParam->stillStamp == UI_DATESTAMP_OFF) {
		return ret;
	}

	if (dateStampFmtId == 0xFF) {
		return ret;
	}

	sp5kGfxAttrSet( SP5K_GFX_REFRESH_ACTIVE, 0, 0, 0, 0 );
	sp5kGfxPageClear( SP5K_GFX_PAGE_PIP_0, PAL_NOCOLOR );
	appOsd_ColorDrawTextColorSet( PAL_RED, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100 );

	/* Buffer allocation */
	stillStampDateBuf = sp5kMalloc( 100);
    if (stillStampDateBuf == NULL) {
        HOST_PROF_LOG_PRINT(LEVEL_ERROR, "set: Stamp Date buf init Fail");
        HOST_ASSERT(0);
    }
	/* Get RTC string */
	sp5kRtcDateTimeGet( SP5K_DATE_TIME_OPTION, &rtc );

	/*Set data stamp fmt*/
    DBG_PRINT("set: dateStampFmtId = %x\n",dateStampFmtId);

	switch ( dateStampFmtId & 0x00FF ) {
	case SP5K_CAPTURE_DATE_YYYYMMDD:
		sprintf( ( char * )stillStampDateBuf, "%d/%02d/%02d", rtc.tmx_year+1900, rtc.tmx_mon,rtc.tmx_mday );
		break;

	case SP5K_CAPTURE_DATE_MMDDYYYY:
		sprintf( ( char * )stillStampDateBuf, "%d/%02d/%02d", rtc.tmx_mon, rtc.tmx_mday, rtc.tmx_year + 1900 );
		break;

	case SP5K_CAPTURE_DATE_DDMMYYYY:
		sprintf( ( char * )stillStampDateBuf, "%d/%02d/%02d", rtc.tmx_mday, rtc.tmx_mon, rtc.tmx_year + 1900 );
		break;

	case SP5K_CAPTURE_DATE_CUSTOM:
		break;

	default:
		/* do nothing */
		return 0;
		break;
	}

	if (( dateStampFmtId & 0x0f00 ) == SP5K_CAPTURE_DATE_TIME ) {
		sprintf( (char *)stillStampDateBuf + strlen( (char *)stillStampDateBuf ), " %02d:%02d:%02d", rtc.tmx_hour, rtc.tmx_min, rtc.tmx_sec );
	}
	else if ( ( dateStampFmtId & 0x0f00 ) == SP5K_CAPTURE_DATE_TIME_NO_SECOND ) {
		sprintf( (char *)stillStampDateBuf + strlen( (char *)stillStampDateBuf ), " %02d:%02d", rtc.tmx_hour, rtc.tmx_min );
	}

	sp5kGfxPageCapabilityGet( SP5K_GFX_PAGE_PIP_0, &cap );

	sp5kGfxStringSizeGet(cap.type, stillStampDateBuf, &dataStampRoiW, &dataStampRoiH);

	dataStampScaleRoiW = (psSizeInfo->roiWidth / 4 + 1) & (~0x1);
	if (dataStampScaleRoiW < dataStampRoiW) {
		dataStampScaleRoiW = dataStampRoiW;
		dataStampScaleRoiH = dataStampRoiH;
	}
	else {
		dataStampScaleRoiH = dataStampRoiH * dataStampScaleRoiW / dataStampRoiW;
	}
	printf("stamp %ux%u -> %ux%u\n", dataStampRoiW, dataStampRoiH, dataStampScaleRoiW, dataStampScaleRoiH);

	dataStampScaleW = (dataStampScaleRoiW + 15) & (~0xf);
	dataStampScaleH = (dataStampScaleRoiH + 15) & (~0xf);


	ConvertBuf = sp5kYuvBufferAlloc( dataStampScaleW, dataStampScaleH );
    if (ConvertBuf == NULL) {
        HOST_PROF_LOG_PRINT(LEVEL_ERROR, "set: Convert buf init Fail");
        HOST_ASSERT(0);
    }


	/*Set data stamp position*/
    roiX = psSizeInfo->roiWidth-dataStampScaleRoiW - 32;	//SHEN: stamp width should be check completer by RD,roiX is design to right side align.
	roiY = psSizeInfo->roiHeight-dataStampScaleRoiH - 32;

	/*Set string color & Draw string on OSD page */
    #if 0
	sp5kGfxAttrSet( SP5K_GFX_REFRESH_ACTIVE, 0, 0, 0, 0 );
	sp5kGfxPageClear( SP5K_GFX_PAGE_PIP_0, PAL_NOCOLOR );
	appOsd_ColorDrawTextColorSet( PAL_RED, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100 );
    #endif
	sp5kGfxStringDraw( SP5K_GFX_PAGE_PIP_0, 0, 0, SP5K_GFX_ALIGN_TOP_LEFT, STR_ENCODE_ASCII, stillStampDateBuf );
    #if 0
    printf("stillStampDateBuf %s\n",stillStampDateBuf);
    printf("dataStampRoiW %d %d\n",dataStampRoiW,dataStampRoiH);
    printf("psSizeInfo %d %d\n",psSizeInfo->cropWidth,psSizeInfo->cropHeight);
    printf("ImgSize %d %d\n",ImgSize[pViewParam->stillSize].w,ImgSize[pViewParam->stillSize].h);
    printf("cap.frm %d %d\n",cap.frmW,cap.frmH);
    printf("cropWidth %d %d\n",psSizeInfo->cropWidth,psSizeInfo->cropHeight);
    #endif

	#if 0
	fsSimpleWrite( "D:\\SRCGFX.RGB", cap.pbuf, cap.frmW * cap.frmH * 2 );
	#endif

	/*OSD SCALE*/
	gfxObjSrc.pbuf = cap.pbuf;
	gfxObjSrc.bufW = cap.frmW;
	gfxObjSrc.bufH = cap.frmH;
	gfxObjSrc.roiX = 0;
	gfxObjSrc.roiY = 0;
	gfxObjSrc.roiW = dataStampRoiW;
	gfxObjSrc.roiH = dataStampRoiH;
	gfxObjSrc.fmt = SP5K_GFX_FMT_RGB565;
	gfxObjDst.pbuf = ConvertBuf;
	gfxObjDst.bufW = dataStampScaleW;
	gfxObjDst.bufH = dataStampScaleH;
	gfxObjDst.roiX = 0;
	gfxObjDst.roiY = 0;
	gfxObjDst.roiW = dataStampScaleRoiW;
	gfxObjDst.roiH = dataStampScaleRoiH;
	gfxObjDst.fmt = SP5K_GFX_FMT_YUV422;
	_ret1 = sp5kGfxObjectScale( &gfxObjSrc, &gfxObjDst );
	#if 0
	fsSimpleWrite( "D:\\DST_SCALE.RGB", cap.pbuf, cap.frmW * cap.frmH * 2 );
	fsSimpleWrite( "D:\\DST_SCALE.YUV", ConvertBuf, dataStampScaleW * dataStampScaleH * 2 );
	#endif

	/* YUV & OSD blend */
	srcPic.pbuf = ConvertBuf;
	srcPic.frmW = dataStampScaleW;
	srcPic.frmH = dataStampScaleH;
	srcPic.roiX = 0;
	srcPic.roiY = 0;
 	srcPic.roiW = dataStampScaleRoiW;	/* it must be multiple of 16x and multiple of roiW of src and dst can't over 12x */
 	srcPic.roiH = dataStampScaleRoiH;	/* it must be multiple of 16x and multiple of roiH of src and dst can't over 12x */
	srcPic.fmt	= SP5K_GFX_FMT_YUV422;
	dstPic.pbuf = ( UINT8* )addr;
	dstPic.frmW = psSizeInfo->roiWidth;
	dstPic.frmH = psSizeInfo->roiHeight;
	dstPic.roiX = roiX;
	dstPic.roiY = roiY;
	dstPic.roiW = dataStampScaleRoiW;
	dstPic.roiH = dataStampScaleRoiH;
	dstPic.fmt	= SP5K_GFX_FMT_YUV422;
	_ret2 = sp5kGfxPictureBlend( &dstPic, 128, 1, 0, &srcPic );

	#if 0
 	fsSimpleWrite( "D:\\DST_01.YUV", srcPic.pbuf, srcPic.frmW * srcPic.frmH * 2 );
	fsSimpleWrite( "D:\\DST_02.YUV", dstPic.pbuf, dstPic.frmW * dstPic.frmH * 2 );
	#endif
	sp5kGfxPageClear( SP5K_GFX_PAGE_PIP_0, PAL_NOCOLOR );
	sp5kGfxAttrSet( SP5K_GFX_REFRESH_ACTIVE, 1, 0, 0, 0 );
	sp5kFree( stillStampDateBuf );
	sp5kYuvBufferFree( ConvertBuf );
	
	//for cppcheck clean warning
	if(_ret1!=ret)
	{
		ret = _ret1;
	}
	if(_ret2!=ret)
	{
		ret = _ret2;
	}
	return ret;
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetDateStamp
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_SetDateStamp(
	UINT32 stamp_in,
	UINT32 dateformat_in
)
{
	UINT32 stamp;
	
	DBG_PRINT("set: %s\n",__FUNCTION__);

	// check the input is correct.
	stamp = ( stamp_in==0 )? pViewParam->stillStamp : stamp_in;

	dateStampFmtId = ( dateformat_in==0 )? SP5K_CAPTURE_DATE_YYYYMMDD : ( dateformat_in + 1 );

	switch( stamp )
	{
		default:
		case UI_DATESTAMP_OFF:
			dateStampFmtId = 0xFF;
			break;
		case UI_DATESTAMP_DATE:
			dateStampFmtId &= ( ~SP5K_CAPTURE_DATE_TIME_NO_SECOND );
			break;
		case UI_DATESTAMP_DATETIME:
			dateStampFmtId |= SP5K_CAPTURE_DATE_TIME_NO_SECOND;
			break;
	}
	if( dateStampFmtId != 0xFF ){
		void* pfunc;
		appCallbackRegisterwithIdx( SP5K_CAPTURE_CALLBACK_YUV_WITH_JPEG, appStill_CustumStamp, 1 );
		pfunc = appCallbackFuncGet( SP5K_CAPTURE_CALLBACK_YUV_WITH_JPEG );
		sp5kStillCapCallbackSet( SP5K_CAPTURE_CALLBACK_YUV_WITH_JPEG, pfunc );
	}else{
		appCallbackUnregister( SP5K_CAPTURE_CALLBACK_YUV_WITH_JPEG, appStill_CustumStamp );
		sp5kStillCapCallbackSet( SP5K_CAPTURE_CALLBACK_YUV_WITH_JPEG, NULL );
	}
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetDist
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_SetDist(
	UINT8 idx
)
{
	#ifdef HW_DIST
	if((idx == UI_STABILIZER_ON)&&(gDistEnTag ==FALSE))
	{
		gDistEnTag=TRUE;
		sp5kStillCapDistStart();
	}
	else if ((idx == UI_STABILIZER_OFF)&&(gDistEnTag ==TRUE))
	{
		gDistEnTag =FALSE;
		sp5kStillCapDistStop();
	}
	#endif
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetStabilizer
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_SetStabilizer(
	UINT8 idx
)
{

	#ifdef HW_DIST
	appStill_SetDist(idx);
	#else
#if 0
	UINT32 shH, shL;

	shH = pCurBtnPar->shutterBoundary[1];
	if(idx == UI_STABILIZER_ON)
	{
		shL = (SHUTTER_1_60S>pCurBtnPar->shutterBoundary[0]? SHUTTER_1_60S : pCurBtnPar->shutterBoundary[0]);
	}
	else
	{
		shL = pCurBtnPar->shutterBoundary[0];
	}
	appAeCapShutBoundarySet(gStillCB.cap.CurScene, shH, shL);
#endif
	#endif
	appAeStabilizerSet(idx);

}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetDZoom
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_SetDZoom(
	UINT8 idx
)
{
	DBG_PRINT("set: %s: idx=%d",__FUNCTION__, idx);
	if(idx==0)
	{
		appDZoomReset(APP_DZOOM_UI_MODE_STILL_VIEW);
		appDZoomReset(APP_DZOOM_UI_MODE_VIDEO_VIEW);
	}
	appDZoomEnableCtrl(idx);
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetFlash
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_SetFlash(
	UINT8 idx
)
{
	BOOL isStorbeCharging = appStrobeQueryChargeEnable();

	DBG_PRINT("set: appStrobeQueryChargeEnable() = %d\n",isStorbeCharging);
	if(idx==0xFF)
	{
		appStrobeChargeStop();
		GREEN_LED_ON
	}
	else if(idx==0 && isStorbeCharging)
	{
		DBG_PRINT("set: stop strobe charge and power off\n");
		appStrobeChargeStop();
		appStrobePowerOff();
		GREEN_LED_ON
		if(gStillCB.flashChargeMonitorTimer)
		{
			appTimerClear(&gStillCB.flashChargeMonitorTimer);
		}
	}
	else if(idx!=0 && !isStorbeCharging)
	{
		DBG_PRINT("set: start strobe charge\n");
		appStrobePowerOn();
		appStrobeChargeStart();
		LED_FLICKER_STROBE_CHARGE
	}
	appSnapFlashUiModeSet(idx);
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetShutterSpeed
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_SetShutterSpeed(
	UINT8 idx
)
{
	appAeCapShutterSet(idx);
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetApertureValue
 *  Description :   change the f number.
 *------------------------------------------------------------------------*/
void
appStill_SetApertureValue(
	UINT8 idx
)
{
	appAeCapIrisSet(idx);
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetQV
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_SetQV(
	UINT32 qv
)
{
	sp5kStillCapCfgSet(SP5K_CAPTURE_QV_LENGTH, qv);

}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetConfirmTimeOut
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_SetConfirmTimeOut(
	UINT32 time
)
{
	sp5kStillCapCfgSet(SP5K_CAPTURE_STORE_CONFIRM_TIMEOUT, time);
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetCaptureConfirmStore
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_SetCaptureConfirmStore(
	UINT32 storeflag
)
{
	sp5kStillCapStoreConfirm(storeflag);
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_CfgCaptureConfirmStore
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_CfgCaptureConfirmStore(
	UINT32 enable
)
{
	/*0: auto store ,1:manually confirm*/
	sp5kStillCapCfgSet(SP5K_CAPTURE_CONFIRM_STORE, enable);
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_FindCurBtnParam
 *  Description :
 *------------------------------------------------------------------------*/
void appStill_FindCurBtnParam(	UINT8 view_idx,UINT8 scene_idx)
{
	UINT32 i;

	for(i=0; i<PASM_MAX; i++)
	{
		if(view_idx == buttonParam[i].id)
		{
			pCurBtnPar = &(buttonParam[i]);
			break;
		}
	}
	DBG_PRINT("set: %s: view=%d, scene=%d, pCurBtnPar->id = %d",__FUNCTION__, view_idx, scene_idx, pCurBtnPar->id);
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_FlashChange
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_ChangeFlash(void)
{
	#if 0
	if(pCurBtnPar==0)
	{
		appStill_FindCurBtnParam(gStillCB.cap.CurViewMode, gStillCB.cap.CurScene);
	}

	flash_idx++;
	if(pCurBtnPar->flash[flash_idx] == UI_FLASH_MAX)
	{
		flash_idx = 0;
	}
	pViewParam->flash= pCurBtnPar->flash[flash_idx];
	gStillCB.flashMode = pViewParam->flash;
	#endif
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_ChangeAf
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_ChangeAf(void)
{
	UINT32 viewMode = gStillCB.cap.CurViewMode;

	if(pCurBtnPar==0)
	{
		appStill_FindCurBtnParam(viewMode, gStillCB.cap.CurScene);
	}

	#if 0
    UINT32 idx,
	idx = pViewParam->af+1;
	pViewParam->af = (idx >= UI_FOCUS_MAX)? 0: idx;
	#endif
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_ChangeShutterSpeed
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_ChangeShutterSpeed(
	UINT32 updownkey
)
{
	#if 0
	DBG_PRINT("set: _%s: in\n", __FUNCTION__);
	if(updownkey)
	{
		pViewParam->shutter--;
	}
	else
	{
		pViewParam->shutter++;
	}

	if(pViewParam->shutter>SHUTTER_1_2000S)
	{
		pViewParam->shutter=SHUTTER_8S;
	}
	else if(pViewParam->shutter<SHUTTER_8S)
	{
		pViewParam->shutter=SHUTTER_1_2000S;
	}
	appStill_SetShutterSpeed(pViewParam->shutter);
	#endif
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_ChangeApertureValue
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_ChangeApertureValue(
	UINT32 updownkey
)
{
	#if 0
	pViewParam->aperture = (pViewParam->aperture == APERTURE_2_8)?
		APERTURE_5_6 : APERTURE_2_8;
	appStill_SetApertureValue(pViewParam->aperture);
	#endif
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetExpFlickerMoe
 *  Description :   change to 50HZz/60Hz
 *------------------------------------------------------------------------*/
void
appStill_SetExpFlickerMode(
	UINT8 idx
)
{
	appAeExpFlickerModeSet(idx);
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_GetViewParam
 *  Description :
 *------------------------------------------------------------------------*/
UINT32
appStill_GetViewParam(
	UINT8 view_idx,
	UINT8 scene_idx
)
{
	UINT32 viewParamIdx=0;

	switch(view_idx)
	{
	case PASM_PROGRAM:
		viewParamIdx = PRM_MODE_P;
		break;
	case VIDEO_MODE:
		viewParamIdx = PRM_MODE_VIDEO;
		break;
	}
	DBG_PRINT("set: viewParamIdx= %d\n", viewParamIdx);

	return viewParamIdx;
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_SetPasmSceneParam
 *  Description :
 *------------------------------------------------------------------------*/
void
appStill_SetPasmSceneParam(
	UINT8 view_idx,
	UINT8 scene_idx
)
{
	static UINT32 lastView=0xffff;
	static UINT32 lastScene=0xffff;

	if(lastView == view_idx && lastScene == scene_idx)
	{
		return;
	}
	lastView = view_idx;
	lastScene = scene_idx;

	/* init mode value */
	appStill_FindCurBtnParam(view_idx, scene_idx);
	DBG_PRINT("set: Scene SET(%u)(%u)\n",view_idx,lastScene);

	if(view_idx != PASM_PROGRAM)
	{
		flash_idx=0;
	}
	/*UiParamViewChangeMode(appStill_GetViewParam(view_idx, scene_idx));*/
	gStillCB.flashMode = UI_FLASH_OFF;

	appStill_SetExposure(pViewParam->ev);

	/* ae relative set */
              appStill_SetISO(UI_ISO_AUTO);
		appStill_SetShutterSpeed(SHUTTER_AUTO);
		appStill_SetApertureValue(APERTURE_AUTO);


}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_ResetAllParam()
 *  Description :   This routine is responsible to reset the parameters in
 *					pViewParam, gStillCB and gVideoCB.
 *------------------------------------------------------------------------*/
void
appStill_ResetAllParam(void)
{
	UiParamViewReset();

	focus_idx = 0;
	flash_idx = 0;

	if(IS_VIDEO_MODE && (appActiveStateGet()!=APP_STATE_MENU))
	{
		appVideoSettingInit(VIDEO_SET_ALL);
		appVideoSizeSet();
	}
	else if(IS_PASM_PMODE)
	{
		appStillSettingInit(VIEW_SET_ALL);
		appStill_SetResolution(pViewParam->stillSize);
	}
}

/*-------------------------------------------------------------------------
 *  Function Name : cmdSnap
 *  Description :   command line test function
 *------------------------------------------------------------------------*/
void
cmdSnap(
	UINT32 argc,
	UINT8 *arg[],
	UINT32 v[]
)
{
	UINT32 temp;
	WbOffset_t wb;

	if (argc==1)
	{
		printf("Snap usage:\n"
			" snapp stillDriverMode [0|1|2|3|4] 		: ??\n"
			" snapp stillQuality [0|1|2] 			: normal/fine/best\n"
			" snapp stillSize [7|169|32|5|3|1|0]	: 7M~VGA\n"
			" snapp stillStamp [0|1|2] 			: off/date/date&time\n"
			" snapp aeb [0|1] 				: off/on\n"
			" snapp edge [0|1|2] 			: hard/middle/soft\n"
			" snapp effect []				: ???\n"
			" snapp qv []                    : quick view time in sec\n"
			" snapp bg                       : show background info\n"
			" snapp dzoom []                 : enable dzoom\n"
			" snapp param                    : show pViewParam members\n"
			" snapp mode [view] [scene]      : p/a/s/m/sc = 0~4 \n"
			" snapp strobe [width] [position] [cycle]\n"
		);
	}
	else if (strcmp((const char *)arg[1], "stillDriverMode")==0)
	{
		pViewParam->stillDriverMode = v[2];
	}
	else if (strcmp((const char *)arg[1], "stillQuality")==0)
	{
		pViewParam->stillQuality = v[2];
	}
	else if (strcmp((const char *)arg[1], "stillSize")==0)
	{
		switch (v[2])
		{
			default:
			case 7: temp=0; break;
			case 169: temp=1; break;
			case 32: temp=2; break;
			case 5: temp=3; break;
			case 3:	temp=4; break;
			case 1: temp=5; break;
			case 0:	temp=6; break;
		}
		pViewParam->stillSize = temp;
	}
	else if (strcmp((const char *)arg[1], "stillStamp")==0)
	{
		pViewParam->stillStamp = v[2];
	}
	else if (strcmp((const char *)arg[1], "edge")==0)
	{
		pViewParam->edge = v[2];
	}
	else if (strcmp((const char *)arg[1], "qv")==0)
	{
		gStillCB.cap.qvSec = v[2];
	}
	else if (strcmp((const char *)arg[1], "bg")==0)
	{
		printf("not support \n");
	}
	else if (strcmp((const char *)arg[1], "dzoom")==0)
	{
		appStill_SetDZoom(v[2]);
	}
	else if (strcmp((const char *)arg[1], "param")==0)
	{
		printf("pViewParam->stillSize=%d\n",pViewParam->stillSize);
		printf("pViewParam->stillQuality=%d\n",pViewParam->stillQuality);
		printf("pViewParam->stillDriverMode=%d\n",pViewParam->stillDriverMode);
		printf("pViewParam->videoSize=%d\n",pViewParam->videoSize);
		printf("pViewParam->videoQuality=%d\n",pViewParam->videoQuality);
		printf("pViewParam->videoSeamless=%d\n",pViewParam->videoSeamless);
		printf("pViewParam->videoStamp=%d\n",pViewParam->videoStamp);
		printf("pViewParam->metering=%d\n",pViewParam->metering);






		printf("pViewParam->wb=%d\n",pViewParam->wb);
		printf("pViewParam->iso=%d\n",pViewParam->iso);
		printf("pViewParam->ev=%d\n",pViewParam->ev);
		printf("pViewParam->edge=%d\n",pViewParam->edge);
		printf("pViewParam->stillStamp=%d\n",pViewParam->stillStamp);
		printf("pViewParam->stablizer=%d\n",pViewParam->stablizer);
	}
	else if (strcmp((const char *)arg[1], "mode")==0)
	{
		appStill_SetPasmSceneParam(v[2], v[3]);
	}
	else if (strcmp((const char *)arg[1], "osdt2")==0)
	{
		appViewOsd_ModeIconTest();
		return;
	}
	else if (strcmp((const char *)arg[1], "wboffset")==0)
	{

		wb.roffset = -CALIB_AE_DEFAULT_OB - v[2]*600;
		wb.groffset = -CALIB_AE_DEFAULT_OB - v[3]*600;
		wb.boffset = -CALIB_AE_DEFAULT_OB - v[4]*600;
		wb.gboffset = -CALIB_AE_DEFAULT_OB - v[5]*600;
		sp5kIqCfgSet(SP5K_IQ_CFG_WB_OFS_R ,wb.roffset);
		sp5kIqCfgSet(SP5K_IQ_CFG_WB_OFS_GR, wb.groffset);
		sp5kIqCfgSet(SP5K_IQ_CFG_WB_OFS_B , wb.boffset);
		sp5kIqCfgSet(SP5K_IQ_CFG_WB_OFS_GB, wb.gboffset);
		return;
	}
	else if (strcmp((const char *)arg[1], "showflash")==0)
	{
		printf("gStillCB.flashMode=%d\n", gStillCB.flashMode);
		return;
	}
	else if (strcmp((const char *)arg[1], "afwin")==0)
	{
		sp5kAfCfgSet(SP5K_AF_WIN_POS_TEST,v[2]);
		return;
	}
	else if (strcmp((const char *)arg[1], "flash_ch")==0)
	{
		IO_STROBE_CHARGE_ON; sp5kTimeDelay(SP5K_TIME_DELAY_1US, 36);//1
		IO_STROBE_CHARGE_OFF; sp5kTimeDelay(SP5K_TIME_DELAY_1US, 5);
		IO_STROBE_CHARGE_ON; sp5kTimeDelay(SP5K_TIME_DELAY_1US, 5);	//2
		IO_STROBE_CHARGE_OFF; sp5kTimeDelay(SP5K_TIME_DELAY_1US, 5);
		IO_STROBE_CHARGE_ON; sp5kTimeDelay(SP5K_TIME_DELAY_1US, 5);	//3
		IO_STROBE_CHARGE_OFF; sp5kTimeDelay(SP5K_TIME_DELAY_1US, 5);
		IO_STROBE_CHARGE_ON; sp5kTimeDelay(SP5K_TIME_DELAY_1US, 5);	//4
		return;
	}
	else if (strcmp((const char *)arg[1], "image")==0)
	{
		sp5kHostMsgSend(APP_KEY_PRESS_S1,0);
		sp5kHostMsgSend(APP_KEY_PRESS_S2,0);
		return;
	}

	appStillSettingInit(VIEW_SET_ALL);
}

