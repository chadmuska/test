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
 *  File name    : App_video_view.c
 *  Function     : about video preview state (pv mode)		   
 *  First editor :   
 *  First date   :   
 *  History      :
 *               : 2018.05.29  Edward.Lu  "Add description and modify code"
 **************************************************************************/

/*

 The suggestion video fllow.
 1. set video size.
 2. sp5k mode set to "video_preview." (need to allocate buffer)
 3. sp5k mode set to "video_record" as recording.
 4. set media abort to stop. (to stop and add file name to DCF)
 5. set sp5k mode set to "video_preview."

 */

#include "sp5k_global_api.h"
#include "sp5k_ae_api.h"
#include "sp5k_capture_api.h"
#include "sp5k_disp_api.h"
#include "sp5k_disk_api.h"
#include "sp5k_rsvblk_api.h"
#include "sp5k_gfx_api.h"
#include "sp5k_aud_api.h"
#include "sp5k_dcf_api.h"
#include "sp5k_media_api.h"
#include "sp5k_dzoom_api.h"
#include "sp5k_sensor_api.h"
#include "sp5k_dbg_api.h"

#include "app_com_def.h"
#include "app_com_api.h"
#include "app_osd.h"
#include "app_pal.h"
#include "app_icon_def.h"
#include "app_face_detect_ex.h"
#include "app_cdfs_api.h"
#include "app_zoom_api.h"
#include "app_still.h"
#include "app_video.h"
#include "app_sensor.h"
#include "app_user_setting.h"
#include "app_sound.h"
#include "app_aaa.h"
#include "app_view_param.h"
#include "app_error_msg.h"
#include "app_view_osd.h"
#include "gpio_custom.h"
#include "app_audio.h"
#include "app_osd_api.h"
#include "app_flash_api.h"
#include "app_cdfs_api.h"
#include "app_playback_def.h"
#include "app_playback_uiparam.h"
#include "app_awbalg_api.h"
#include "app_key_def.h"
#include "app_setup_lib.h"
#include "customization/app_init.h"

#include "app_ui_para.h"

#if SP5K_MOTION_DETECT
#include "app_md.h"
#endif

#if SP5K_USB_UVC_SUPPORT
#include "sp5k_usbhost_api.h"
#include "app_video_uvc.h"
#endif
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
#include "jet_cdfs.h"
#include "app_infohandler_func.h"
#include "app_infohandlerComm.h"
#endif
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
appVideoCB_t gVideoCB=
{
	.CapTimer = SP5K_TIMER_ID_1,
	.CapSec = 0,
	.osd = OSD_VIDEO_DEFAULT,
	.fCapture = 0,
	.fPause = 0,
	.isDiskReady=0,
	.isAutoPowerOffOn=1,
	.err=0
};

UINT32 SeamLessTime[UI_VIDEO_SEAMLESS_MAX] = {0 , 60, 180, 300};
UINT8 autoRecFlag = ENABLE;
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
UINT8 dropFrameRecFlag = 0;
#endif
#if SP5K_MOTION_DETECT
UINT32 mdTime=TIMER_NULL;
UINT32 mdRecCount;
UINT8 mdStop;
#endif
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
#if GPS_BY_UART0
extern void appGpsModeSwitch(void);
#endif
#if GPS_OPEN
UINT32 RTCTimer=TIMER_NULL;
#endif
extern UINT8 fileTypeSelect;

UINT32 StationModeFirst = 0;

#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
extern TemperatureState TEMPstate;
extern PowerDetect appInfoPowerDETStateGet(void);
extern UINT32 appInfoParkingReCordingTimeGet(void);
extern BOOL appInfoDiskSizeCheck(void);
extern TemperatureState jetTempGet(UINT8 *temp);
extern void appInfoHighTempProtectedFB(UINT8 state, UINT8 temp);
extern void appGsensorLevelCtrl(int mode, int level);
extern UINT32 GetPowerBacklight(void);
extern void SetPowerBacklight(UINT32 en);
extern  BOOL appBuzzerReady(void);
extern UINT32 appBuzzerReadyWaitTime(void);
#endif

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/*--------------------------------------------------------------------------*
* Function name    : appVideoAutoRecCB
* Function         : Video Auto Record callback
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.29  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appVideoAutoRecCB(void)
{
	/*auto record */
	BPrintf("Button Enable");
	appBtnControl(BTN_ALL, ENABLE);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	UINT8 temp= 0;
	/*TemperatureState state= TEMP_NONE;*/
	/*state = jetTempGet(&temp);*/
	/*TEMPstate =state;*/
	TEMPstate = jetTempGet(&temp);/*weikai modify 180504*/
	if(TEMPstate ==TEMP_HIGH)
	{
		appInfoHighTempProtectedFB(TEMPstate, temp);
		jetErrStorageState(ERROR_SAVE_TEMP_HIGH);
		autoRecFlag = DISABLE;
		printf("%s High temperatur \n" , __FUNCTION__);
		return;
	}
#endif
	if( (appActiveStateGet() == APP_STATE_VIDEO_PREVIEW) && (autoRecFlag == ENABLE)){
		autoRecFlag = DISABLE;
		sp5kHostMsgSend(APP_KEY_PRESS_S2, DISABLE);
		return;
	}
	if((autoRecFlag == ENABLE) &&(appActiveStateGet()== APP_STATE_VIDEO_AAA)&&
		(appPreviousStateGet()==APP_STATE_VIDEO_PREVIEW)){
		autoRecFlag = DISABLE;
		sp5kHostMsgSend(APP_KEY_PRESS_S2, DISABLE);
		return;
	}
}

/*--------------------------------------------------------------------------*
* Function name    : appVideoSeamLessSecGet
* Function         : For more safty delete file used(seamless), return (seamless video file time)*4 add 1 minute.
* Return value	   : seamlessSec
* Parameter1	   : APP_STATE_VIDEO_REC used(UINT32 state)
* History          :
*                  : 2018.05.29  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32 appVideoSeamLessSecGet(UINT32 state)
{
    UINT32 seamlessSec;
    seamlessSec = (SeamLessTime[pViewParam->videoSeamless]*4+60);
    return seamlessSec;
}

/*--------------------------------------------------------------------------*
* Function name    : appVideoSeamLessSecCriticalGet
* Function         : For safty delete file used(seamless), return seamless video file time add 1 minute.
* Return value	   : seamlessSec
* Parameter1	   : APP_STATE_VIDEO_REC used(UINT32 state)
* History          :
*                  : 2018.05.29  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32 appVideoSeamLessSecCriticalGet(UINT32 state)
{
    UINT32 seamlessSec;
    seamlessSec = (SeamLessTime[pViewParam->videoSeamless]+60);
    #if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
        seamlessSec = seamlessSec<<1;
    #endif
    return seamlessSec;
}

/*--------------------------------------------------------------------------*
* Function name    : _VideoCheckDiskReady
* Function         : Check Disk Ready
* Return value	   : TRUE/FALSE
* Parameter1	   : void
* History          :
*                  : 2018.05.29  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32
_VideoCheckDiskReady(void)
{
	if (!IS_DISK_MOUNT_READY)
	{
		HOST_PROF_LOG_PRINT(LEVEL_WARN, "vpv: %s %d mount not rdy",__FUNCTION__ ,__LINE__);
		return FALSE;
	}

	gVideoCB.isDiskReady = TRUE;
	if(IS_DISK_MOUNT_FAIL)
	{
		gVideoCB.err = VIEW_ERR_CARD_ERROR;
	}
	else
	{
		gVideoCB.err = VIEW_ERR_NO_ERROR;
	}
	return TRUE;
}

/*--------------------------------------------------------------------------*
* Function name    : appVideoCheckReady
* Function         : Check Video Ready
* Return value	   : VIDEO_STATE_YES/VIDEO_STATE_NO (UINT32)
* Parameter1	   : CAN_I_START_RECORD/CAN_I_LEAVE_VIDEO (UINT32)
* History          :
*                  : 2018.05.29  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32
appVideoCheckReady(UINT32 question)
{
	UINT32 mode;
	UINT32 speed, uV_BitRate;

	if(gVideoCB.err == VIEW_ERR_CARD_ERROR)
	{
		HOST_PROF_LOG_PRINT(LEVEL_WARN, "vpv: %s %d card error",__FUNCTION__ ,__LINE__);
		return VIDEO_STATE_NO;
	}

	if(_VideoCheckDiskReady()==FALSE)
	{
		HOST_PROF_LOG_ADD(LEVEL_INFO, "vpv: wait for disk ready");
		sp5kHostMsgSend(APP_UI_MSG_VIEW_WAIT_DISK, DISABLE);
        return VIDEO_STATE_NO;
	}

	switch (question)
	{
	default:
	case CAN_I_START_RECORD:
		if (pViewParam->videoSeamless) {
			#if SP5K_CDFS_OPT
			if(appCdfsFreeSpaceCheck(CDFS_FILE_FOLDER_VIDEO) == FAIL)
			{
				gVideoCB.err = VIEW_ERR_INSUFFICIENT_DISK_SPACE;
				HOST_PROF_LOG_PRINT(LEVEL_WARN, "vpv: %s %d check space fail",__FUNCTION__ ,__LINE__);
				return VIDEO_STATE_NO;
			}
			#else
			if (appVideoRemainSecGet() < (SeamLessTime[pViewParam->videoSeamless]<<1)) {
				gVideoCB.err = VIEW_ERR_INSUFFICIENT_DISK_SPACE;
				HOST_PROF_LOG_PRINT(LEVEL_WARN, "vpv: %s %d check space fail",__FUNCTION__ ,__LINE__);
				return VIDEO_STATE_NO;
			}
			#endif
		}

		if(!gVideoCB.isDiskReady)
		{
			gVideoCB.err = VIEW_ERR_CARD_ERROR;
			HOST_PROF_LOG_PRINT(LEVEL_WARN, "vpv: %s %d disk not ready",__FUNCTION__ ,__LINE__);
			return VIDEO_STATE_NO;
		}

		if(IS_DISK_SD_MOUNT && IS_CARD_LOCK)
		{
			gVideoCB.err = VIEW_ERR_CARD_LOCKED;
			return VIDEO_STATE_NO;
		}

		if(appVideoRemainSecGet()<1)
		{
			HOST_PROF_LOG_PRINT(LEVEL_WARN, "vpv: %s %d check space fail",__FUNCTION__ ,__LINE__);
			gVideoCB.err = VIEW_ERR_INSUFFICIENT_DISK_SPACE;
			return VIDEO_STATE_NO;
		}

		if(appLensQuery(LENS_QUERY_MOTION_STATE))
		{
			HOST_PROF_LOG_PRINT(LEVEL_WARN, "vpv: %s %d lens is moving, can not do aaa",__FUNCTION__ ,__LINE__);
			return VIDEO_STATE_NO;
		}

        sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_VIDEO_AVG_BITRATE, &uV_BitRate);
		sp5kDiskInfoGet(SP5K_DRIVE_SD, SP5K_DISK_CLASS_SPEED_QUERY, &speed);
	    if((speed < 10/*CLASS_10*/) && (uV_BitRate>14000000)) {/*Mapping to original status, FHD30fps is pass, 14Mbps*/
			gVideoCB.err = VIEW_ERR_DISK_SPEED_TOO_SLOW;
			return VIDEO_STATE_NO;
        }
		break;

	case CAN_I_LEAVE_VIDEO:
		sp5kModeGet(&mode);
		if (mode!= SP5K_MODE_VIDEO_PREVIEW)
		{
			return VIDEO_STATE_NO;
		}
		break;
	}

	return VIDEO_STATE_YES;
}

/*--------------------------------------------------------------------------*
* Function name    : appVideoSettingInit
* Function         : do video setting
* Return value	   : void
* Parameter1	   : flag 
(UINT32 VIDEO_SET_CAPTURE_MODE:set video format
        VIDEO_SET_CAPTURE_SIZE:set video size
        VIDEO_SET_ALL)
* History          :
*                  : 2018.05.29  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appVideoSettingInit(
	UINT32 flg
)
{
	/*seamless cdfs control*/
	printf("%s :s  0x%x\n",__FUNCTION__ , flg);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	if(appInfoPowerDETStateGet()== POWER_DET_PARKING_WAKEUP&& appInfoParkingReCordingTimeGet() <60){
		appVideoSeamLessTimeSet(appInfoParkingReCordingTimeGet());
	}
	else
#endif
	appVideoSeamLessTimeSet(SeamLessTime[pViewParam->videoSeamless]);

	if (flg & VIDEO_SET_CAPTURE_MODE)
	{
		appVideoFormatSet();
	}

	/* Avoid modeSet 0x21 twice in condition */
	if ((flg & VIDEO_SET_CAPTURE_SIZE)/* && appPreviousStateGet() != APP_STATE_POWER_ON*/)////Move these setting from power on state
	{
		appVideoSizeSet();
        appVideoSetQuality();
	}


	#if SUPPORT_RUNTIME_SWITCH_DRAM_CLK	/* 4k video test */
    if(pViewParam->videoSize == UI_VIDEO_SIZE_4K2K||pViewParam->videoSize == UI_VIDEO_SIZE_27K)
    {
    	UINT32 maxclock=1;
        appSysClkSwitch(maxclock);
    }
	#endif

	appStill_SetMetering(pViewParam->metering);
	appStill_SetWB(pViewParam->wb);
	appStill_SetISO(pViewParam->iso);
	appStill_SetExposure(pViewParam->ev);
	//appStill_SetAF(pViewParam->af);
	appStill_SetShutterSpeed(SHUTTER_AUTO);
	appStill_SetApertureValue(APERTURE_AUTO);

	/* Ghost image setting */
	/** SP5K_MEDIA_SYS_CFG_h264_MV_CTRL
	 * \brief Adjust the mv control register
	 * \param[in] mode 0:4MV(16x16~8x8), 1:1MV(16x16 only)(UINT32)
	 * \param[in] term 1: enable B direct termination. 0: full mode search.(UINT32)
	 * \param[in] shift early termination threshold.(UINT32)
	 */
	UINT32 mode=0, term=1, shiftthreshold=2;
	sp5kSystemCfgSet(SP5K_MEDIA_SYS_CFG, SP5K_MEDIA_SYS_CFG_h264_MV_CTRL, mode, term, shiftthreshold);

	/* Block effect setting */
	/** SP5K_MEDIA_SYS_CFG_H264_MB_QP_DIFF
	 * \brief adjust the macroblock qp diff value 
	 * \param[in] id Stream id. 0 - main, 1 - second.(UINT32)
	 * \param[in] parr pointer to qp diff array.(UINT8)
	 * \param[in] type 0: I, 1: P/B.(UINT32)
	 */
	UINT32 strid=0, type=0;
	UINT8 iarr[6] = { 0x06, 0x04, 0x02, 0x18, 0x1c, 0x1e };
	UINT8 pbarr[6] = { 0x06, 0x04, 0x02, 0x1a, 0x1c, 0x1e };
	sp5kSystemCfgSet(SP5K_MEDIA_SYS_CFG, SP5K_MEDIA_SYS_CFG_H264_MB_QP_DIFF, strid, iarr, type);
	type=1;
	sp5kSystemCfgSet(SP5K_MEDIA_SYS_CFG, SP5K_MEDIA_SYS_CFG_H264_MB_QP_DIFF, strid, pbarr, type);

	/** SP5K_MEDIA_SYS_CFG_H264_QT_AC_SKIP
	 * \brief Adjust the qt ac skip enable register
	 * \param[in] en 0: disable skip ac, 1: enable (default).
	 */
	sp5kSystemCfgSet(SP5K_MEDIA_SYS_CFG, SP5K_MEDIA_SYS_CFG_H264_QT_AC_SKIP, DISABLE);
	#if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
	strid=1; type=0;
	sp5kSystemCfgSet(SP5K_MEDIA_SYS_CFG, SP5K_MEDIA_SYS_CFG_H264_MB_QP_DIFF, strid, iarr, type);
	type=1;
	sp5kSystemCfgSet(SP5K_MEDIA_SYS_CFG, SP5K_MEDIA_SYS_CFG_H264_MB_QP_DIFF, strid, pbarr, type);
	#endif

    applightFrequencySetting(pUiSetting->lightFrequency);
		printf("%s :e\n",__FUNCTION__);
}

/*--------------------------------------------------------------------------*
* Function name    : appVideoPvStateInit
* Function         : Video preview state init on the basis of previous state.
* Return value	   : void
* Parameter1	   : Previous State (UINT32)
* History          :
*                  : 2018.05.29  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appVideoPvStateInit(
	UINT32 preState
)
{
	
	UINT32 mode;
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
//	UINT32 err=0;
#endif
	#if SP5K_CDFS_OPT
	pbUIParam_t *ppbUiParam = appPb_UiParamPtrGet();
	#endif
	#if SP5K_MOTION_DETECT
	uiPara_t* puiPara = appUiParaGet();
    #endif

	HOST_PROF_LOG_PRINT(LEVEL_INFO, "vpv: %s s", __FUNCTION__);
    #if SP5K_SENSOR_DUAL_MODE
		UINT32 vd_watchdog=DISABLE;
	    sp5kSensorCfgSet(SP5K_SENSOR_CFG_PREVIEW_TIMEOUT_ENABLE, vd_watchdog, NULL);
		vd_watchdog=ENABLE;
	    sp5kSensorCfgSet(SP5K_SENSOR_CFG_PREVIEW_TIMEOUT_ENABLE, vd_watchdog, NULL);
	#endif
	
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	/*paul 2018.04.19 modify bug #3421 remove else mapping from 17dvr*/
	/*paul 2018.0426 modfiy bug  #3445 add SD Free size check*/
	UINT32 FreeSize= 0;
	sp5kDiskInfoGet(SP5K_DRIVE_SD, SP5K_FREE_SIZE_BY_KBYTE, &FreeSize);
	if(IS_CARD_EXIST && appDiskSDMountReady() && \
	(appInfoPowerDETStateGet() == POWER_DET_ACC_ON) \
	&&appInfoDiskSizeCheck()&& (FreeSize >=102400))
	{		
		RED_LED_OFF;
		GREEN_LED_FLICKER(1000);/*FLICKER_MS*/

	}
#else
	LED_NORMAL_ON;
#endif
	gVideoCB.fCapture = 0;
	gVideoCB.CapSec = 0;
    profLogPrintf(0, "<lester> gVideoCB.CapSec case1:\n");
	#if GPS_BY_UART0
	appGpsModeSwitch();
	#endif
	#if GPS_OPEN
	if((RTCTimer == NULL) || (RTCTimer == TIMER_NULL)){
		appTimerClear(&RTCTimer);
			RTCTimer = appTimerSet(1000/*ms*/,"RTCTimer");
	}
	#endif
    if(pViewParam->videoSize == UI_VIDEO_SIZE_4K2K||pViewParam->videoSize == UI_VIDEO_SIZE_27K)
        sp5kAudRecPreSet();
#if defined(PRJ_PDVR) /*Isaac modified with font address error*/
	printf("pViewParam->videoStamp   0x%x" , pViewParam->videoStamp);
	if(pViewParam->videoStamp){
	    appOsd_GfxFontIDSet(SP5K_GFX_FONT_ID_1);
	}
	sp5kTimerEnable(gVideoCB.CapTimer, DISABLE);
#endif
	HOST_PROF_LOG_PRINT(LEVEL_INFO, "vpv: seamless:%x/ stamp:%x", \
						pViewParam->videoSeamless, pViewParam->videoStamp);

	#if (SP5K_USB_UVC_SUPPORT==0) || (SP5K_SENSOR_DUAL_MODE==0)
	if (IS_HDMI_IN && (pViewParam->videoSize==UI_VIDEO_SIZE_FHD_60FPS || pViewParam->videoSize==UI_VIDEO_SIZE_4K2K || pViewParam->videoSize==UI_VIDEO_SIZE_27K)) {
		pViewParam->videoSize = UI_VIDEO_SIZE_FHD_30FPS;
	}
    #else
	#if SINGLE_DUAL_SWITCH_SAMPLE
	    if (appDualPipModeGet() == DUALSEN_SINGLE_MAIN)
	        appSwitchSingleInDual(TRUE);
	    else
	        appSwitchSingleInDual(FALSE);
	#endif
	#endif
    appResetCmdSetting();

#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	
	/*ae setting*/
	sp5kAeModeSet(SP5K_AE_MODE_INFO_ONLY);
	appAePreviewSet();
	sp5kAwbModeSet(SP5K_AWB_MODE_INFO_ONLY);
	appAWBALG_WBParamSet ( AWB_PARA_UPDTREFE_FRAME_NUM, 255 );
#endif

	switch(preState)
	{
	case APP_STATE_VIDEO_REC:
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		sp5kAwbModeSet(SP5K_AWB_MODE_INFO_ONLY);
#endif
		appPowerSavingEnableSet(TRUE);
		gVideoCB.isAutoPowerOffOn=1;
	case APP_STATE_STILL_AAA:
		appViewOsd_AllClean();
	case APP_STATE_STILL_BUSY:
		appVideoOsdShow(DISABLE,DISABLE,gVideoCB.osd);
		break;
	case APP_STATE_VIDEO_AAA:
		gStillCB.s1press=0;
		break;
	default:
	    gStillCB.s1press=0;
        gStillCB.s2press=0;
        gStillCB.s1AaaTicket=0;
        gStillCB.s2SnapTicket=0;
        gStillCB.err = VIEW_ERR_NO_ERROR;

        if(IS_VIEW_MODE_CHANGE)
        {
            gVideoCB.osd = OSD_VIEW_SIMPLE;
        }

        gStillCB.cap.CurViewMode = VIDEO_MODE;
#if !defined(PRJ_17DVR)&&!defined(PRJ_19DVR)
        if(preState != APP_STATE_POWER_ON && !IS_VIDEO_GROUP(preState))
            appUserSettingSave(pUiSetting);
#endif
        #if 0//EZ_MENU  /*mark for using up more time when enter menu*/
        appUIParaSave(); /*lester test*/
        #endif

	appStill_SetPasmSceneParam(gStillCB.cap.CurViewMode, gStillCB.cap.CurScene);

        #if SP5K_USB_UVC_SUPPORT
            if(appUvcPwrOnRecGet())
                pUiSetting->powerOnRecord = POWER_ON_RECORD_ON;
        #endif
        if(pUiSetting->usbMode == USB_UVCH264 && pUiSetting->usbMode == USB_UVCMJPG){
            appStillRawCbSet();
        }

        gVideoCB.isDiskReady = FALSE;
        gVideoCB.err = VIEW_ERR_NO_ERROR;
        gVideoCB.CapTimer = SP5K_TIMER_ID_1;
        gVideoCB.isAutoPowerOffOn=1;

        /*pre flash setting*/
        appPreflashInit();
		appVideoSettingInit(VIDEO_SET_ALL);

		#if SP5K_DISK_SPI
		if(IS_CARD_EXIST)
		#endif
		{
			if(_VideoCheckDiskReady()==FALSE)
			{
				HOST_PROF_LOG_ADD(LEVEL_INFO, "vpv: wait for disk ready");
				sp5kHostMsgSend(APP_UI_MSG_VIEW_WAIT_DISK, DISABLE);
			}
			else
			{
				#if SP5K_CDFS_OPT
				appCdfsFileFolderSet(CDFS_FILE_FOLDER_VIDEO);
				appCdfsFolderInit(CDFS_FILE_FOLDER_VIDEO);
				ppbUiParam->selCdfs = CDFS_FILE_FOLDER_VIDEO;
				#endif
				HOST_PROF_LOG_ADD(LEVEL_INFO, "vpv: disk ready");
			}
		}

		APP_OSD_REFRESH_OFF;
		if(preState == APP_STATE_POWER_ON)
			appVideoOsdShow(DISABLE,DISABLE,gVideoCB.osd);
		else
			appVideoOsdShow(ENABLE,ENABLE,gVideoCB.osd);

        #if SP5K_MULTI_FAST_BOOT
		if(preState == APP_STATE_POWER_ON)
		{
			/*make sure sensor img ready*/
			sp5kDispAttrSet(sp5kDispActChnlGet(0), SP5K_DISP_IMG_ACTIVE, ENABLE);
		}
        #endif
		#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		if(preState == APP_STATE_POWER_ON){
			sp5kStillCapCallbackSet( SP5K_CAPTURE_CALLBACK_EXIF_CREATE ,appExifEncodeCb);
		}
		#endif
		if((!IS_VIDEO_GROUP(preState)) ||
			(IS_VIDEO_GROUP(preState) && IS_MENU_STATE(preState) && (preState != APP_STATE_POWER_ON)))
		{
			sp5kModeGet(&mode);
			if (mode!= SP5K_MODE_VIDEO_PREVIEW)
			{
				// vieo preview mode change take 30ms
				appModeSet(SP5K_MODE_VIDEO_PREVIEW);
			}
			else
			{
				HOST_PROF_LOG_ADD(LEVEL_INFO, "vpv: video preview");
			}
		}

		sp5kModeWait(SP5K_MODE_VIDEO_PREVIEW);

		UINT32 start_x=0, start_y=0, raduis_x=0, raduis_y=0;
		appOsdLib_PFLayerShapeClear(APP_OSD_SHAPE_RECT, start_x, start_y, LCD_SIZE_X, LCD_SIZE_Y, raduis_x, raduis_y);
		appOsdLib_PIPLayerClearDraw();
		APP_OSD_REFRESH_ON;

		appAudioRecVolumeConfig();

		#if SP5K_MOTION_DETECT
			if(puiPara->MotionDetect == ENABLE)
			{
				appMotionDetectSet(APP_MD_EN, ENABLE);
			}
		#endif

		break;
	}

	appBtnControl(BTN_ALL, ENABLE);

	#if SP5K_DISK_SPI
	if (!IS_CARD_EXIST)	{
		appErrMsgWithSound(ERR_MSG_NO_MEMORY_CARD, ERR_MSG_WITH_WARNING_SOUND, ERR_MSG_SHOW_FOREVER);
	#if !defined(PRJ_17DVR)&&!defined(PRJ_19DVR)
		appBtnDisable(BTN_ALL);
	#endif
	}
	else
	#endif
	{
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		if (pUiSetting->powerOnRecord == POWER_ON_RECORD_ON && preState == APP_STATE_TV_PLUG  && appInfoDiskSizeCheck() )
		{
			UINT8 temp = 0 ;
			/*TemperatureState state= TEMP_NONE;*/
			TEMPstate = jetTempGet(&temp);
			/*TEMPstate =state;*/
			if(TEMPstate <TEMP_HIGH){/*weikai modify 180504*/
				autoRecFlag = ENABLE;
				sp5kTimerIsrOneShotReg(0, 1000/*ms*/, appVideoAutoRecCB);
			}
			else{
				printf("%s High temperatur \n" , __FUNCTION__);

				appInfoHighTempProtectedFB(TEMPstate, temp);
				jetErrStorageState(ERROR_SAVE_TEMP_HIGH);

				if(appInfoPowerDETStateGet() == POWER_DET_PARKING_WAKEUP)
				{
					sp5kHostMsgSend(INFO_QUEUEMSG_POWER, POWER_DET_ACC_OFF);
					/*err = sp5kHostMsgSend(INFO_QUEUEMSG_POWER, POWER_DET_ACC_OFF);*//*weikai modify 180504*/
				}
			}

		}
#else
		if (pUiSetting->powerOnRecord == POWER_ON_RECORD_ON && preState == APP_STATE_POWER_ON) 

		{
			autoRecFlag = ENABLE;
			sp5kTimerIsrOneShotReg(DISABLE, 1000/*ms*/, appVideoAutoRecCB);
		}
#endif
	}

	#if (SP5K_SENSOR_DUAL_MODE && SINGLE_DUAL_SWITCH_SAMPLE)
	    appAutoSwitchUI();
	#endif

    #if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
    if(autoRecFlag==DISABLE)
    	appViewOsd_DualModeDraw();
    #endif

	#if	SP5K_MULTI_FAST_BOOT
	if (!IS_CARD_EXIST)
	    appErrMsgWithSound(ERR_MSG_NO_MEMORY_CARD, ERR_MSG_WITH_WARNING_SOUND, ERR_MSG_SHOW_FOREVER);
	#endif

#if SP5K_SENSOR_DUAL_MODE
        appStillDualModePIP(appDualPipModeGet());
#endif

	HOST_PROF_LOG_PRINT(LEVEL_INFO, "vpv: %s e", __FUNCTION__);
}

/*--------------------------------------------------------------------------*
* Function name    : appVideoPvStateClose
* Function         : Video preview state close
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.29  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appVideoPvStateClose(void)
{
#if SP5K_SENSOR_DUAL_MODE
    UINT32 retVal;
    sp5kSensorModeCfgSet(SP5K_MODE_SEC_SENSOR_SEL|SP5K_MODE_STILL_PREVIEW, DEFAULT_PREVIEW_MODE);
    sp5kMediaRecCfgGet(SP5K_MEDIA_REC_MULTI_STREAM_EN, &retVal);
    if(retVal)
        sp5kMediaRecCfgSet(SP5K_MEDIA_REC_MULTI_STREAM_EN, DISABLE);
#endif

#if GPS_OPEN
    appTimerClear(&RTCTimer);
#endif

}

/*--------------------------------------------------------------------------*
* Function name    : appVideoPvStateOnMsg
* Function         : Video preview state management for message use.
* Return value	   : void
* Parameter1	   : message (UINT32 app_state_ctrl.h/sysg_msg_def.h)
* Parameter2	   : parameter (UINT32)
* History          :
*                  : 2018.05.29  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appVideoPvStateOnMsg(
	UINT32 msg,
	UINT32 param
)
{
	#if SP5K_MOTION_DETECT
	uiPara_t* puiPara = appUiParaGet();
    #endif

	switch ( msg) {
	case SP5K_MSG_MODE_READY:
		break;

	case APP_UI_MSG_VIEW_WAIT_DISK:
		if(_VideoCheckDiskReady()==FALSE)
		{
			APP_SLEEP_MS(200);
			sp5kHostMsgSend(APP_UI_MSG_VIEW_WAIT_DISK, DISABLE);
			break;
		}
		else {
			#if SP5K_CDFS_OPT
			appCdfsFileFolderSet(CDFS_FILE_FOLDER_VIDEO);
			appCdfsFolderInit(CDFS_FILE_FOLDER_VIDEO);
			appPb_UiParamPtrGet()->selCdfs = CDFS_FILE_FOLDER_VIDEO;
			#endif
		}
		appVideoOsdShow(DISABLE,DISABLE,gVideoCB.osd);
		break;

	case SP5K_MSG_AE_READY:
            appAeReady();

#if SP5K_MOTION_DETECT
            if(puiPara->MotionDetect == ENABLE)
            {
                appMotionDetectHdr(DISABLE);
            }
#endif
		break;

	case SP5K_MSG_AWB_READY:
		break;

	case APP_UI_MSG_LENS_DZOOM_TELE:
		appVideoDZoomTele();
		break;

	case APP_UI_MSG_LENS_DZOOM_WIDE:
		appVideoDZoomWide();
		break;

	case APP_UI_MSG_VIEW_MEMORY_FULL:
		break;
#if (SP5K_SENSOR_DUAL_MODE && SINGLE_DUAL_SWITCH_SAMPLE)
    case APP_UI_MSG_WAIT_MSG_YUV_PLUG_OUT:
        appAutoSwitchUI();
        break;
#endif
	case SP5K_MSG_TIMER:
		#if GPS_OPEN
		if(param == RTCTimer){
			HL_GPS_STATUS_FLAG_Proc();
		}
		#endif
		break;
#if SP5K_MOTION_DETECT
	case APP_UI_MSG_MD_START:
		mdStop = 0;
		if(mdTime == TIMER_NULL)
		{
			/* Because videorec may don't sync with MD timer, which will bring about issue 50797 */
			/* So seamless on/off will use different MD record solution -GX */
			if(puiPara->Seamless == UI_VIDEO_SEAMLESS_OFF)
			{
				mdTime = appTimerSet(1000/*ms*/, "MDRec");
				mdRecCount = 60;
			}
			else
			{
				mdTime = appTimerSet(300/*ms*/, "MDRec");
				mdRecCount = appVideoSeamLessSecGet(appActiveStateGet());
			}
		}
		sp5kHostMsgSend(APP_KEY_PRESS_S1 , DISABLE);
		sp5kHostMsgSend(APP_KEY_PRESS_S2 , DISABLE);
		break;
#endif
	default:
		break;
	}
}

/*--------------------------------------------------------------------------*
* Function name    : appVideoPvStateOnKey
* Function         : Video preview state management for key use.
* Return value	   : void
* Parameter1	   : message (UINT32 app_state_ctrl.h/sysg_msg_def.h)
* Parameter2	   : parameter (UINT32)
* History          :
*                  : 2018.05.29  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appVideoPvStateOnKey(
	UINT32 msg,
	UINT32 param
)
{
	uiPara_t* puiPara = appUiParaGet();

	switch (msg)
	{
	case APP_KEY_PRESS_S2:
		gStillCB.s2SnapTicket=1;
	/*case APP_KEY_PRESS_S1:*/
		if(appVideoCheckReady(CAN_I_START_RECORD)==VIDEO_STATE_YES)
		{
			#if GPS_OPEN
			appTimerClear(&RTCTimer);
			#endif
			/*-- shall wait Lens completes before going focus */
			appStateChange(APP_STATE_VIDEO_AAA, STATE_PARAM_FREE_CARRY);
		}
		else
		{
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
			if (appInfoPowerDETStateGet() == POWER_DET_PARKING_WAKEUP) {

				if(appInfoBatteryLvGet() < appInfoParkingBatterySettingGet()){
					int mode,level;
					if((appInfoParkingBatterySettingGet() - appInfoBatteryLvGet() ) <= 200)
					{
						mode=2;/*Parking mode*/
						level=(appInfoParkingGSensorLevelGet() - 1);
						appGsensorLevelCtrl(mode, level); /*Parking mode enable*/
					}
					else
					{
						mode=3;/*Power mode*/
						level=4;
						appGsensorLevelCtrl(mode, level);
					 	appInfoParkingModeTriStateSet(ParkingModeTigger_LOWPOWER);
					}
				}
				appInfoSWWDTWakeup(DISABLE);
				// Fix Redmine#2794
				if(appBuzzerReady())
				{
					sp5kHostMsgSend(INFO_QUEUEMSG_POWER, POWER_DET_ACC_OFF);
				}
				else
					sp5kTimerIsrOneShotReg(DISABLE, appBuzzerReadyWaitTime(), appInfoParkingLowPowerCB);
			}
#endif
			appViewOsd_WarmingMsgDraw(TRUE, gVideoCB.err, TRUE);
			UINT32 showTimeInMs=1000;
			if((gVideoCB.err == VIEW_ERR_INSUFFICIENT_DISK_SPACE) && (msg == APP_KEY_PRESS_S1))
			{
				appErrMsgShow(ERR_MSG_INSUFFICIENT_DISK_SPACE, showTimeInMs);
			}
			if((gVideoCB.err == VIEW_ERR_DISK_SPEED_TOO_SLOW) && (msg == APP_KEY_PRESS_S1))
			{
				appErrMsgShow(ERR_MSG_DISK_SPEED_TOO_SLOW, showTimeInMs);
			}
		}
		break;

	case APP_KEY_PRESS_MENU:
		#if GPS_OPEN
		appTimerClear(&RTCTimer);
		#endif
		appStateChange(APP_STATE_MENU,STATE_PARAM_NORMAL_INIT);
		break;

	case APP_KEY_PRESS_SET:
		gVideoCB.osd++;
		if (gVideoCB.osd >= OSD_VIEW_CLEAN_ALL)
		{
			gVideoCB.osd = OSD_VIEW_SIMPLE;
		}
		appVideoOsdShow(DISABLE,DISABLE,gVideoCB.osd);
		break;

	case APP_KEY_PRESS_PB:
		appBeep(TRUE, FALSE);
		appAAAOff();
		fileTypeSelect = 1;/*ez_menu for pb filetype */
		sp5kModeSet(SP5K_MODE_STANDBY);
		sp5kModeWait(SP5K_MODE_STANDBY);
		appStateChange(APP_STATE_MENU,STATE_PARAM_NORMAL_INIT); /*pb mode*/
		break;

	case APP_KEY_PRESS_MODE:
		appBeep(TRUE, FALSE);
		appAAAOff();
		#if SP5K_USB_UVC_SUPPORT
		appsensorAutoPwrSaveSet(DISABLE);
		fileTypeSelect = 1;/*ez_menu for pb filetype */
		sp5kModeSet(SP5K_MODE_STANDBY);
		sp5kModeWait(SP5K_MODE_STANDBY);
		appStateChange(APP_STATE_MENU,STATE_PARAM_NORMAL_INIT); /*pb mode*/
		#else
		appStateChange(APP_STATE_STILL_PREVIEW,STATE_PARAM_NORMAL_INIT); /*pb mode*/
		#endif
		break;

	case APP_KEY_PRESS_TELE:
		break;

	case APP_KEY_PRESS_WIDE:
		break;

	case APP_KEY_RELEASE_WIDE:
	case APP_KEY_RELEASE_TELE:
		break;

	case APP_KEY_PRESS_UP:
        #if SP5K_USB_UVC_SUPPORT
        appUvcEventSet(EVENT_TRIG_INIT);
		#elif ICAT_WIFI || ICAT_ETHERNET
		if(pViewParam->stillDriverMode!= UI_STILL_DRIVERMODE_OFF){
			pViewParam->stillDriverMode = UI_STILL_DRIVERMODE_OFF;
			appStill_SetDriveMode(pViewParam->stillDriverMode);
		}
		HOST_PROF_LOG_ADD(LEVEL_INFO, "vpv: WiFi connection, entering WiFi state");
		StationModeFirst = 0;
		#endif
		break;

	case APP_KEY_PRESS_LEFT:
		#if SP5K_USB_UVC_SUPPORT
        appUvcFlipModeSet();
		#elif SP5K_SENSOR_DUAL_MODE
		#else
		#if SP5K_ICAT_EIS
            if (pViewParam->VideoEISMode != UI_EIS_OFF)
                pViewParam->videoSize = appView_ItemDecrease(UI_VIDEO_SIZE_4K2K, pViewParam->videoSize);
            else
        #endif
		pViewParam->videoSize = appView_ItemDecrease(UI_VIDEO_SIZE_MAX, pViewParam->videoSize);
		/* ICOM-3412 */
		puiPara->VideoSize = pViewParam->videoSize;
		appVideoSettingInit(VIDEO_SET_ALL);
		appViewOsd_PicSizeVideoDraw(TRUE, pViewParam->videoSize);
		appVideoOsdDiskShow(TRUE);
		appModeSet(SP5K_MODE_VIDEO_PREVIEW);
		#endif
		UiParamViewFlush();
		appUIParaSave();
		break;

	case APP_KEY_PRESS_RIGHT:
        #if SP5K_USB_UVC_SUPPORT
        appUvcCapModeSet();
        #endif
		break;
	case APP_KEY_PRESS_DOWN:
        #if SP5K_USB_UVC_SUPPORT
        if(!appUvcCarModeGet())
    		appUvcPipMode();
		#elif SP5K_SENSOR_DUAL_MODE
		appStillDualModePIP(appView_ItemDecrease(DUAL_SEN_MAX, appDualPipModeGet()));
            appViewOsd_DualModeDraw();
		#endif
		break;
	case APP_KEY_PRESS_DEL:
		break;

	default:
		break;
	}

	switch (msg)
	{
	case APP_KEY_PRESS_DEL:
	case APP_KEY_PRESS_MENU:
	case APP_KEY_PRESS_SET:
	case APP_KEY_PRESS_LEFT:
	case APP_KEY_PRESS_RIGHT:
    case APP_KEY_PRESS_DOWN:
		appBeep(TRUE, FALSE);
		break;
	default:
		break;
	}
}

/*--------------------------------------------------------------------------*
* Function name    : appVideoPvState
* Function         : Video preview state management in common use.
* Return value	   : void
* Parameter1	   : message (UINT32 app_state_ctrl.h/sysg_msg_def.h)
* Parameter2	   : parameter (UINT32)
* History          :
*                  : 2018.05.29  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appVideoPvState(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 mode;

	switch (msg) {
	case APP_STATE_MSG_INIT: // video preview state init case
		RPrintf("appVideoPvState");
		// Set standby mode, before do video preview init.
		sp5kModeGet(&mode);
		if (mode != SP5K_MODE_STANDBY) {
			UINT32 backup_last_frame=1;
			sp5kModeCfgSet(SP5K_MODE_CFG_STANDBY_DUP, backup_last_frame);
			sp5kModeSet(SP5K_MODE_STANDBY);
			sp5kModeWait(SP5K_MODE_STANDBY);
		}
		// Do video preview init.
		appVideoPvStateInit(appPreviousStateGet());
		// double check
		sp5kModeGet(&mode);
		if (mode != SP5K_MODE_VIDEO_PREVIEW) {
			sp5kModeSet(SP5K_MODE_VIDEO_PREVIEW);
			sp5kModeWait(SP5K_MODE_VIDEO_PREVIEW);
		}
		#if SP5K_SENSOR_DUAL_MODE
		appStillDualModePIP(appDualPipModeGet());
	    appVideoDualStreamCfgSet(appDualPipModeGet());
		#endif

		#if API_TEST_INIT
		sp5kHostMsgSend(APP_UI_MSG_EMULATION, DISABLE);
		#endif

		break;
	case APP_STATE_MSG_CLOSE: // video preview state close case
        if ((appNextStateGet()&0xFF00) != APP_STATE_GROUP_VIDEO)
        {
			appResetCmdSetting();
        }
        appVideoPvStateClose();
		appStateCloseDone();
		break;
	default:
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		if (dropFrameRecFlag) {
			sp5kHostMsgSend(APP_KEY_PRESS_S2, DISABLE);
			dropFrameRecFlag = 0;
		}
#endif
		if (IS_APP_KEY_MSG(msg))
		{
			appVideoPvStateOnKey(msg,param);
		}
		else
		{
			appVideoPvStateOnMsg(msg,param);
		}
		break;
	}
}
