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
#include "sp5k_ae_api.h"
#include "sp5k_capture_api.h"
#include "sp5k_disp_api.h"
#include "sp5k_gfx_api.h"
#include "sp5k_ftrack_api.h"
#include "sp5k_rsvblk_api.h"

#include "app_still.h"
#include "app_view_param.h"
#include "app_strobe.h"
#include "app_osd_api.h"
#include "app_error_msg.h"
#include "app_view_osd.h"
#include "app_osd.h"
#include "app_video.h"
#include "app_flash_api.h"
#include "app_calib_api.h"
#include "app_mmface_detect.h"
#include "app_face_detect_ex.h"
#include "app_aaa.h"

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
extern void appStrobeChargeTimerReset(void);
extern void appAeStableSet(void);
extern void appAeStableReset(void);
extern void _processAfterFastAe(UINT32 ae_result);
extern void _stillUtil_ShutterDelaySet(void);
extern void appShowCurExpAgc(void);

 #if KIT_SHUT_CTRL_TIMER
extern UINT32 CapExpTime=0;
extern UINT32 CapShutColseTime=0;
#endif

extern UINT32 BlinkYUVCallBackSet;
extern UINT32 BlinkGet;
extern UINT32 BlinkSaveSelect;

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
#define apptbCIPA_Log(a)

/*-------------------------------------------------------------------------
 * Function Name : appStillCapFlashStatusGet
 *------------------------------------------------------------------------*/
 UINT32 appStillCapFlashStatusGet(void)
{
/*0 :Not have Flash*/
/*1 :Need Flash*/
/*2 :Fake Flash*/

#ifdef FLASH_ROLLING
	return appAeFlashNeedGet(); /* Eric 110823 determine by AE */
#else
	if(gStillCB.needFlash)
	{
		return 1;
	}
	else
	{
		return 0;
	}
#endif

}

/*-------------------------------------------------------------------------
 * Function Name : appStillCapExpTimeGet
 *------------------------------------------------------------------------*/
 UINT32 appStillCapExpTimeGet(void)
{
	#if KIT_SHUT_CTRL_TIMER
		return CapExpTime;
	#else
		return 0;
	#endif
}

/*-------------------------------------------------------------------------
 * Function Name : appStillCapShutCloseTimeGet
 *------------------------------------------------------------------------*/
 UINT32 appStillCapShutCloseTimeGet(void)
{
	#if KIT_SHUT_CTRL_TIMER
		return CapShutColseTime;
	#else
		return 0;
	#endif
}

void
IcatUsbCBCapStateInit(void)
{
	appAeCaptureSet(0);

	HOST_PROF_LOG_PRINT(LEVEL_INFO, "cap: %s s",__FUNCTION__);

#ifdef FLASH_ROLLING
	appStillCapFlashStatusGet();
	appStrobeChargeTimerReset();
#else
	if(gStillCB.needFlash)
	{
		UINT32 snapPulseW;
		HOST_PROF_LOG_ADD(LEVEL_INFO, "cap: need flash");
		appPreflashConfig(UI_FLASH_OFF);
		appPreflashTrig();

		snapPulseW = appSnapFlashCalculate(appSnapflashTargetGet());
		if (snapPulseW)
		{

			appSnapFlashInstall(0x00, snapPulseW, appSnapflashCfgHPosGet());
			// update the ae information after flash.
			appAeCaptureFlashAeOverrideSet();
			appAeGetCurrentExpIsoUiIdx((UINT16 *)&gStillCB.ae.shutter, (UINT16 *)&gStillCB.ae.iso);
			appStrobeChargeTimerReset();
		}
		else
		{
			appSnapFlashDismiss();
		}
	}
	else
	{
		HOST_PROF_LOG_ADD(LEVEL_INFO, "cap: don't need flash");
		#if DEBUG_PREFH_USE_DATE_STAMP
		{
			appPrefhCb_t* pprefhCb;
			pprefhCb = appPrefhCbGet();
			if (pprefhCb->isStampDbg==TRUE)
			{
				sp5kStillCapCfgSet(SP5K_CAPTURE_STAMP, (UINT32)NULL);
			}
		}
		#endif
	}
#endif

	appShowCurExpAgc();

	appStill_SetSp5kStillSize(pViewParam->stillSize);
	appStill_SetFlash(0xff);

	HOST_PROF_LOG_ADD(LEVEL_DEBUG, "cap: destroy ft s");
	HOST_PROF_LOG_ADD(LEVEL_DEBUG, "cap: destroy ft e");

	gStillCB.isReady2View = 0;

	appShowCurExpAgc();
	HOST_PROF_LOG_ADD(LEVEL_INFO, "cap: mode set s");
	_stillUtil_ShutterDelaySet();
	HOST_PROF_LOG_ADD(LEVEL_INFO, "cap: mode set e");
}

void Icatsnap3Acb(void)
{
	APP_SLEEP_MS(200);
	appAeStableSet();
	_processAfterFastAe(appAeFastAdjust(0));
	IcatUsbCBCapStateInit();
	appAeStableReset();
	appAePreviewRestore();
	APP_SLEEP_MS(200);
}

UINT32
appViewNotify(
	UINT32 addr,
	UINT32 qvAddr,
	void* size
)
{
	profLogAdd(0, "cap: VIEW_SETTING_NOTIFY");
	appAePreviewSet();//expidx ,agcidx setting
	appAePreviewRestore();
	/*appStill_SetSp5kStillSize(pViewParam->size);*/

	#ifdef HW_DIST
	appAeDistStatusGet(&dist);
	if (dist ==TRUE)
	{
		appStill_SetStabilizer(pViewParam->stablizer);
	}
	#endif
	return 0;
}
void
appStillCapStateInit(void)
{
	/*appShowCurExpAgc();*/
	appAeCaptureSet(0);
	appShowCurExpAgc();

#ifdef FLASH_ROLLING
	appStillCapFlashStatusGet();
	appStrobeChargeTimerReset();
#else
#endif

	appStill_SetSp5kStillSize(pViewParam->stillSize);
	appStill_SetFlash(0xff); // This will turn off the flash but keep its setting.

	gStillCB.isReady2View = 0;

	sp5kStillCapCallbackSet(SP5K_CAPTURE_CALLBACK_VIEW_NOTIFY ,appViewNotify);

    #if S2S_IMPROVE && !SP5K_SENSOR_DUAL_MODE
   	    sp5kStillCapCfgSet(SP5K_CAPTURE_VIEW_DISP_CTRL, 0x01010101);
    #else
        #if SP5K_SENSOR_DUAL_MODE
        if(appDualPipModeGet()==DUALSEN_SINGLE_MAIN || appDualPipModeGet()==DUALSEN_SINGLE_SEC)
   	        sp5kStillCapCfgSet(SP5K_CAPTURE_VIEW_DISP_CTRL, 0x01010101);
        else
        #endif
    	    sp5kStillCapCfgSet(SP5K_CAPTURE_VIEW_DISP_CTRL, SP5K_CAPTURE_VIEW_DISP_AUTO);
    #endif

	appStillUtil_CaptureSet();
	HOST_PROF_LOG_ADD(LEVEL_DEBUG, "cap: snap mode set e");
}
/*-------------------------------------------------------------------------
 *  Function Name : appStillCapStateOnMsg
 *  Description :   sk5p still capture mode will disable all display layers
 					than turn on the image layer in qv and other layer after
 					qv. If you want to show or keep something in non-image
 					layers, you can active disp_osd after into sill capture
 					mode.
 *------------------------------------------------------------------------*/

void
appStillCapStateOnMsg(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 next_state,next_param;
	BOOL is_yuv_cb_on=FALSE;
	#ifdef HW_DIST
	UINT8 dist=0;
	#endif

	switch (msg) {
	case SP5K_MSG_STILL_CAPTURE_IMAGE_ID:
		gStillCB.isReady2View = 1;
		gStillCB.remainImg--;
		break;
	case APP_UI_MSG_LENS_MSHUT_OPEN:
		HOST_PROF_LOG_ADD(LEVEL_DEBUG, "cap: LENS_MSHUT_OPEN");
		appSnapFlashDismiss();
		appStill_SetFlash(gStillCB.flashMode);
		break;
	case SP5K_MSG_STILL_QV_TIMEOUT:
		HOST_PROF_LOG_ADD(LEVEL_INFO, "cap: QV_TIMEOUT");
		break;
#if 0
	case SP5K_MSG_STILL_VIEW_SETTING_NOTIFY:
		HOST_PROF_LOG_ADD(LEVEL_INFO, "cap: VIEW_SETTING_NOTIFY");
		appAePreviewSet();//expidx ,agcidx setting
		appAePreviewRestore();
		/*appStill_SetSp5kStillSize(pViewParam->size);*/

		#ifdef HW_DIST
		appAeDistStatusGet(&dist);
		if (dist ==TRUE)
		{
			appStill_SetStabilizer(pViewParam->stablizer);
		}
		#endif
		break;
#endif
	case SP5K_MSG_MODE_READY:
		HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "cap: SP5K_MSG_MODE_READY: remain=%d param=%x",\
		        gStillCB.remainImg ,param);

		if (param==SP5K_MODE_STILL_PREVIEW)
		{
			appStillFlow_GetNextState(&next_state, &next_param);
			appStateChange(next_state, next_param);
		}
		if (param==SP5K_MODE_STILL_SNAP)
		{
			HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "cap: snap mode ready");

			if(gStillCB.needDarkFrameSub || gStillCB.isXDenoiseOn)
			{
				is_yuv_cb_on = TRUE;
			}

			appStillYuvCbSet(is_yuv_cb_on);
			HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "cap: yuvcb set");

			#if !SP5K_PVQV_DISABLE
			appStillQVCbSet(TRUE);
			HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "cap: pvqv set");
			#endif

			sp5kDispAttrSet(sp5kDispActChnlGet(0), SP5K_DISP_OSD_ACTIVE, OSD_DISPLAY_SUPPORT);
			HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "cap: disp set");
		}
		break;

	case APP_UI_MSG_VIEW_YUV_CALLBACK:
		if(gStillCB.needDarkFrameSub || gStillCB.isXDenoiseOn)
		{
			gStillCB.needDarkFrameSub = FALSE;
			gStillCB.isXDenoiseOn = FALSE;
			appErrMsgWithSound(ERR_MSG_CLEAR, 0, 0);
			#if (FIELD_QV_ENABLE	== 1)
				if (pViewParam->stillStamp == UI_DATESTAMP_OFF) {
				}
			#endif
		}
		break;
	case SP5K_MSG_STILL_READOUT_START:
        if(pUiSetting->sound.shutter != SHUTTER_SOUND_OFF) {
            appPreloadSoundPlay(SOUND_ID_SHUTTER);
        }
		break;
	default:
		break;
	}
}

void
appStillCapStateOnKey(
	UINT32 msg,
	UINT32 param
)
{
	switch (msg)
	{
	case APP_KEY_PRESS_S1:
		if(!gStillCB.needDarkFrameSub && !gStillCB.isXDenoiseOn)
		{
			gStillCB.s1AaaTicket = 1;
			sp5kStillCapCfgSet(SP5K_CAPTURE_QV_LENGTH, 1);
		}
		break;
	case APP_KEY_PRESS_S2:
		gStillCB.s2SnapTicket = gStillCB.s1AaaTicket? 1 : 0;
		break;
	default:
		break;
	}
}

void
appStillCapState(
	UINT32 msg,
	UINT32 param
)
{
	switch (msg)
	{
	case APP_STATE_MSG_INIT:
		/*HOST_PROF_LOG_PRINT(LEVEL_INFO, "cap: %s init s",__FUNCTION__);*/
		appStillCapStateInit();
		/*HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "cap: %s init e", __FUNCTION__);*/
		break;
	case APP_STATE_MSG_CLOSE:
		#if 0
		appStillWaitJobFinish();
		#endif
		appStateCloseDone();
		break;
	default:
		if (IS_APP_KEY_MSG(msg))
		{
			appStillCapStateOnKey(msg,param);
		}
		else
		{

			appStillCapStateOnMsg(msg,param);
		}
		break;
	}
}

