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
 /*--------------------------------------------------------------------------*
 *  File name	: 	App_power_on.c
 *  Function 	: 	power on	 	
 *  First editor	: 	
 *  First date  	: 	
 *  History		:
 *  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

#define HOST_DBG 0
#include "app_com_def.h"
#include "app_com_api.h"

#include "sp5k_global_api.h"
#include "sp5k_disk_api.h"
#include "sp5k_disp_api.h"
#include "sp5k_aud_api.h"
#include "sp5k_ae_api.h"
#include "sp5k_awb_api.h"
#include "sp5k_gfx_api.h"
#include "sp5k_rsvblk_api.h"
#include "sp5k_media_api.h"
#include "sp5k_capture_api.h"
#include "sp5k_sensor_api.h"
#include "sp5k_usb_api.h"
#include "sp5k_utility_api.h"
#include "sp5k_ver.h"
#include "sp5k_dbg_api.h"
#include "middleware/fs_def.h"
#include "app_tvlcd.h"
#include "app_exif.h"
#include "app_osd_api.h"
#include "app_aaa.h"
#include "app_menu_core.h"

#include "customization/dev_init.h"
#include "gpio_custom.h"	/* %REVIEW% for gpioInitialCfgSet() */
#include "app_lens_api.h"	/* %REVIEW% : just for appLensInit() */
#include "app_sound.h"
#include "app_dev_plug.h"
#include "app_battery.h"
#include "app_rtc.h"
#include "app_still.h"
#include "app_jpg_draw.h"
#include "app_calib_api.h"
#include "app_fast_boot.h"
#include "app_view_param.h"
#include "app_video.h"
#include "app_sensor.h"
#include "sp5k_usbhost_api.h"
#include "app_view_osd.h"
#include "app_osdfont.h"
#include "app_awbalg_api.h"
#include "app_aealg_api.h"

#include "../app_ver.h"
#include "../service/misc/app_battery_ctrl.h"
#if SP5K_MOTION_DETECT || SP5K_MOTION_DETECT_REAR
#include "app_md.h"
#endif

#if SP5K_DIQ_FRMWRK
	#include "sp5k_dq_api.h"
#endif

#if SP5K_DIQ_AUTO_WDR
#include "app_auto_alg.h"
#endif

#if SP5K_USB_UVC_SUPPORT
#include "app_video_uvc.h"
#endif
#if APP_STATE_CUSTOMER_VIDEOPV
#include "../ui/_customer/app_test_VideoPv.h"
#endif
#include "app_ui_para.h"

#include "app_watch_dog.h"
#include "app_dead_process.h"

#if APP_LOG_TO_SD_ENABLE
#include "app_sd_log.h"
#endif

#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
#include "app_infohandlerComm.h"
#include "sp5k_dbg_api.h"
#endif

extern void appTzrealRtcCbReg(void *pfunc);
extern void appTzlocalTimeCbReg(void *pfunc);
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define PURE_FW_POWER_ON        0
#define PURE_TESTMODE_POWER_ON  0

typedef enum {
	POWER_ON_PHASE_DISK_DETECT = 0x0000,
	POWER_ON_PHASE_OSD_PRELOAD,
	POWER_ON_PHASE_BTN_INIT
} appPowerOnPhase_e;


/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define READ8(a) (*((volatile UINT8*)(a)))

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
extern appPowerOnInfo_t appPowerOnInfo;
uiParamSetting_t uiParam;
uiParamSetting_t *pUiSetting = &uiParam;

UINT8 isSensorResume = TRUE;

SP5K_EVENT_FLAGS_GROUP			pEvent_Flag;
UINT32 appThread_trig_tmr_POR	= 0xffffffff;
UINT32 timerUnitSet					= 1000;			/*1ms*/
UINT32 hangTimerOut				= 0;
SP5K_THREAD *pthr_por			= NULL;;
extern UINT8 fileTypeSelect;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT32 appVbusInState;
extern void appBtnInit(void);
#if SP5K_DIQ_FRMWRK
extern void appIqPowerOnInit(void);
#else
extern void appIqOffPowerOnInit(void);
#endif
extern void appBackgroundTimerInit(void);
extern void Icatsnap3Acb(void);
extern void usbVndrIcatPreCaptureCallbackSet(void *pfunc);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
extern void appAudioRecFlagSet(BOOL onoff);
extern void appInfoGSensorCompensationSet(void);
#endif

/* paul 2018.04.16 Add for modify compiler warning*/
extern UINT32 cmdOsDeadUrgentCb(void (*fp)(void));
extern void _userSettingDefaultSet(	uiParamSetting_t *pbuf);
extern void appGsensorModeSave(int mode, int level);
extern UINT8 appGsensorSettingGet(void);
extern void appGsensorLevelCtrl(int mode, int level);
#if defined(PRJ_17DVR) ||defined(PRJ_19DVR)
extern void appInfoParkingSettings( void );
extern PowerDetect appInfoPowerDETStateGet(void);
#endif
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void appUiParamInit(void);
void devDiskIoScanStart(void);
void appTimerGetPOR(void);
void appThreadExePOR(void);
void appThreadCreatePOR(void);
void appThreadDeletePOR(void);
void appThreadInitPOR(void);
void appThreadResetPOR(void);

  /*--------------------------------------------------------------------------*
 * Function name	: appCdfsCbInit
 * Function	       	: CDFS callback initial function
 * Return value   	: void
 * Parameter1    	: void
 * History	   :
 *                :  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

#if SP5K_CDFS_OPT
void appCdfsCbInit(void)
{
	HOST_PROF_LOG_ADD(LEVEL_INFO, "pwr: file system-use cdfs");
	sp5kDcfAppCbFuncSet_t cbObj=
	{
		.pfnsp5kDcfCbNextFileNameGet = appCdfsNextFileNameGet,
		.pfnsp5kDcfCbFileNameAdd = appCdfsFileAdd,
		.pfnsp5kDcfCbFileNameSync = appCdfsFileSync,
		.pfnsp5kDcfCbCurFileIdxSet = appCdfsCurFileIdxSet,
		.pfnsp5kDcfCbCurFileIdxGet = appCdfsCurFileIdxGet,
		.pfnsp5kDcfCbFileAttrGet = appCdfsFileAttrGet,
		.pfnsp5kDcfCbFsInfoGet = appCdfsInfoGet,
		.pfnsp5kDcfCbFileOperate = appCdfsFileOperate,
		/*dual cdfs flow*/
		#if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
		.pfnsp5kDcfCbNextSubFileNameGet = appCdfsNextSubFileNameGet,
		.pfnsp5kDcfCbSubFilenameAdd = appCdfsFileAdd,
		.pfnsp5kDcfCbSubFileNameSync = appCdfsFileSync,
		#endif
	};
	sp5kDcfCusSysInit();
	sp5kDcfAppCallbackCfg(&cbObj);
}
#endif

  /*--------------------------------------------------------------------------*
 * Function name	: appProgAllCheck
 * Function	       	: Prog All Check function
 * Return value   	: result  (bool true/false)
 * Parameter1    	: void
 * History	   :
 *                :  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

BOOL appProgAllCheck(void)
{
	UINT32 fd;
	fd = sp5kFsFileOpen(RES_RTC_CHECK_SIG, SP5K_FS_OPEN_RDONLY);
	if (fd==0)
		return 0;
	sp5kFsFileClose(fd);
	HOST_PROF_LOG_ADD(LEVEL_INFO, "pwr: ProgAllCheck ok");
	return 1;
}

  /*--------------------------------------------------------------------------*
 * Function name	: appProgAllClear
 * Function	       	: Prog All Clear function
 * Return value   	: void
 * Parameter1    	: void
 * History	   :
 *                :  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

void appProgAllClear(void)
{
	sp5kFsFileDelete(RES_RTC_CHECK_SIG);
}

#if (((SP5K_SENSOR_ADV7480)&&(!SP5K_SENSOR_DUAL_MODE)))\
	||(((SP5K_SENSOR_AR0230)&&(!SP5K_SENSOR_DUAL_MODE)))\
	||(((SP5K_SENSOR_OV4689)&&(SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE)))\
	||(((SP5K_SENSOR_OV9734)&&(SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE)))\
	||(((SP5K_SENSOR_OV7695)&&(SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE)))
	
  /*--------------------------------------------------------------------------*
 * Function name	: appPriSensorSuspendCb
 * Function	       	: PriSensor Suspend callback
 * Return value   	: result (UINT32 SUCCESS/FAILED)
 * Parameter1    	: void
 * History	   :
 *                :  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

UINT32 appPriSensorSuspendCb(void)
{
	isSensorResume = FALSE;

	sp5kSensorCfgSet(SP5K_SENSOR_CFG_PREVIEW_TIMEOUT_TESTPAT, 2, 0); /*Test pattern: Blue*/
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	sp5kAeModeSet(SP5K_AE_MODE_OFF);    
	sp5kAwbModeSet(SP5K_AWB_MODE_OFF);
#endif
	printf("\n\n $$$$$$ Pri sensor suspend $$$$$$");
	#if 0
	if ((appActiveStateGet()==APP_STATE_STILL_PREVIEW) || (appActiveStateGet()==APP_STATE_VIDEO_PREVIEW)) {
		appViewOsd_SensorDraw(TRUE, isSensorResume);
	}
	#endif

	#if ((SP5K_SENSOR_OV4689)&&(SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE))\
	||(((SP5K_SENSOR_OV9734)&&(SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE)))\
	||(((SP5K_SENSOR_OV7695)&&(SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE)))

	sp5kAeModeSet(SP5K_AE_MODE_OFF);
	#endif

	return SUCCESS;
}

  /*--------------------------------------------------------------------------*
 * Function name	: appPriSensorResumeCb
 * Function	       	: PriSensor Resume callback
 * Return value   	: result (UINT32 SUCCESS/FAILED)
 * Parameter1    	: void
 * History	   :
 *                :  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

UINT32 appPriSensorResumeCb(void)
{
	isSensorResume = TRUE;
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
    	sp5kAeModeSet(SP5K_AE_MODE_INFO_ONLY);    
	sp5kAwbModeSet(SP5K_AWB_MODE_INFO_ONLY); 
#endif
	printf("\n\n $$$$$$ Pri sensor Resume $$$$$$");
	#if 0
	if ((appActiveStateGet()==APP_STATE_STILL_PREVIEW) || (appActiveStateGet()==APP_STATE_VIDEO_PREVIEW)) {
		appViewOsd_SensorDraw(TRUE, isSensorResume);
	}
	#endif

	#if ((SP5K_SENSOR_OV4689)&&(SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE))\
	||(((SP5K_SENSOR_OV9734)&&(SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE)))\
	||(((SP5K_SENSOR_OV7695)&&(SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE)))

	sp5kAeModeSet(SP5K_AE_MODE_INFO_ONLY);
	#endif

	return SUCCESS;
}

  /*--------------------------------------------------------------------------*
 * Function name	: appPriSensorStsChkCb
 * Function	       	: PriSensor status check callback
 * Return value   	: result (UINT32 SUCCESS/FAILED)
 * Parameter1    	: udwInputPara (UINT32)
 * History	   :
 *                :  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

UINT32 appPriSensorStsChkCb(UINT32 a_udwInputPara)
{
	#if ((SP5K_SENSOR_ADV7480)&&(!SP5K_SENSOR_DUAL_MODE))
	extern UINT32 sensorModeGet(void);
	UINT32 modesw;
	UINT8 ucInputNexSenMode = ((a_udwInputPara>>8)&0xff);
	printf("@a_udwInputPara=0x%x, SenModeChk=0x%x, CurSecSenMode=0x%x\n",a_udwInputPara,ucInputNexSenMode,sensorModeGet());

	if (ucInputNexSenMode != sensorModeGet())
	{
		sp5kModeGet(&modesw);
		sp5kSensorModeCfgSet(modesw, ucInputNexSenMode);

    		sp5kModeSet(SP5K_MODE_STANDBY);
   		sp5kModeWait(SP5K_MODE_STANDBY);

    		sp5kModeCfgSet(SP5K_MODE_CFG_PV_DISP_FIT, 1);

    		sp5kModeSet(modesw);
    		sp5kModeWait(modesw);

		printf("#$# Mode 0x%x switch done!!!\n",ucInputNexSenMode);


	}
	#endif

	return SUCCESS;
}

  /*--------------------------------------------------------------------------*
 * Function name	: appModeSensorCbReg
 * Function	       	: Mode sensor callback register
 * Return value   	: result (UINT32 SUCCESS/FAILED)
 * Parameter1    	: void
 * History	   :
 *                :  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

UINT32 appModeSensorCbReg(void)
{
	sp5kSensorCfgSet(SP5K_SENSOR_CFG_PREVIEW_TIMEOUT_INTERVAL, 300, 0); /*Timeout: 300ms, for less pending in MAIN sensor*/
	sp5kSensorCfgSet(SP5K_SENSOR_CFG_PREVIEW_TIMEOUT_PRI_CALLBACK, (UINT32)appPriSensorSuspendCb, (UINT32)appPriSensorResumeCb);
	sp5kSensorCfgSet(SP5K_SENSOR_CFG_PREVIEW_TIMEOUT_STSCHK_PRI_CALLBACK, (UINT32)appPriSensorStsChkCb,0);
	sp5kSensorCfgSet(SP5K_SENSOR_CFG_PREVIEW_TIMEOUT_PRI_ENABLE, 1, 0);
	return SUCCESS;
}
#endif

#if SP5K_SENSOR_DUAL_MODE
  /*--------------------------------------------------------------------------*
 * Function name	: appSensorSuspendCb
 * Function	       	: sensor Suspend callback
 * Return value   	: result (UINT32 SUCCESS/FAILED)
 * Parameter1    	: void
 * History	   :
 *                :  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

UINT32 appSensorSuspendCb(void)
{
	isSensorResume = FALSE;
    printf("\n%s Sensor=%d\n", __FUNCTION__, isSensorResume);

	    sp5kHostMsgSend(APP_UI_MSG_WAIT_MSG_YUV_PLUG_OUT, 0x00);
	sp5kSensorCfgSet(SP5K_SENSOR_CFG_PREVIEW_TIMEOUT_TESTPAT, 2, 0); /*Test pattern: Blue*/
	if ((appActiveStateGet()==APP_STATE_STILL_PREVIEW) || (appActiveStateGet()==APP_STATE_VIDEO_PREVIEW)) {
		appViewOsd_SensorDraw(TRUE, isSensorResume);
	}
	return SUCCESS;
}

  /*--------------------------------------------------------------------------*
 * Function name	: appSensorResumeCb
 * Function	       	: sensor Resume callback
 * Return value   	: result (UINT32 SUCCESS/FAILED)
 * Parameter1    	: void
 * History	   :
 *                :  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

UINT32 appSensorResumeCb(void)
{
    printf("\n%s Sensor=%d\n", __FUNCTION__, isSensorResume);
	if ((appActiveStateGet()==APP_STATE_STILL_PREVIEW) || (appActiveStateGet()==APP_STATE_VIDEO_PREVIEW)) {
		appViewOsd_SensorDraw(TRUE, isSensorResume);
	}
	return SUCCESS;
}


#if (SERDES_4WIRE_UNIDIR_YUV_CASE)

  /*--------------------------------------------------------------------------*
 * Function name	: appSensorStsChkCb
 * Function	       	: Sensor status check callback
 * Return value   	: result (UINT32 SUCCESS/FAILED)
 * Parameter1    	: udwInputPara (UINT32)
 * History	   :
 *                :  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

UINT32 appSensorStsChkCb(UINT32 a_udwInputPara)
{
	extern UINT32 sensorSecModeGet(void);
	isSensorResume = TRUE;
    printf("\n%s sensors=%d  param=0x%X\n", __FUNCTION__, isSensorResume, a_udwInputPara);
    sp5kHostMsgSend(APP_UI_MSG_WAIT_MSG_YUV_PLUG_OUT, 0x01);
	UINT8 ucInputNexSenMode = (a_udwInputPara&0xff);
	profLogPrintf(3,"@a_udwInputPara=0x%x, SenModeChk=0x%x, CurSecSenMode=0x%x",a_udwInputPara,ucInputNexSenMode,sensorSecModeGet());
	if (ucInputNexSenMode != sensorSecModeGet())
	{
		sp5kSensorModeCfgSet(SP5K_MODE_STILL_PREVIEW|SP5K_MODE_SEC_SENSOR_SEL, ucInputNexSenMode);
    	sp5kModeSet(SP5K_MODE_STANDBY);
   		sp5kModeWait(SP5K_MODE_STANDBY);
    	sp5kModeCfgSet(SP5K_MODE_CFG_PV_DISP_FIT, 1);
    	sp5kModeSet(SP5K_MODE_STILL_PREVIEW);
    	sp5kModeWait(SP5K_MODE_STILL_PREVIEW);
		profLogPrintf(3,"#$# Mode switch done!!!");
	}
	return SUCCESS;
}
#endif
  /*--------------------------------------------------------------------------*
 * Function name	: appDualModeSensorCbReg
 * Function	       	: Dual Mode sensor callback register
 * Return value   	: result (UINT32 SUCCESS/FAILED)
 * Parameter1    	: void
 * History	   :
 *                :  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
UINT32 appDualModeSensorCbReg(void)
{
	sp5kSensorCfgSet(SP5K_SENSOR_CFG_PREVIEW_TIMEOUT_INTERVAL, 300, 0); /*Timeout: 300ms, for less pending in MAIN sensor*/
	sp5kSensorCfgSet(SP5K_SENSOR_CFG_PREVIEW_TIMEOUT_CALLBACK, (UINT32)appSensorSuspendCb,(UINT32) appSensorResumeCb);
    sp5kSensorCfgSet(SP5K_SENSOR_CFG_PREVIEW_TIMEOUT_ENABLE, 1, 0);
#if (SERDES_4WIRE_UNIDIR_YUV_CASE)
	sp5kSensorCfgSet(SP5K_SENSOR_CFG_PREVIEW_TIMEOUT_STSCHK_CALLBACK, (UINT32)appSensorStsChkCb,0);
#endif
	return SUCCESS;
}
#endif

  /*--------------------------------------------------------------------------*
 * Function name	: usbPrevResolutionChgCb
 * Function	       	: usb preview resolution change callback
 * Return value   	: void
 * Parameter1    	: width (UINT32)
 * Parameter2    	: height (UINT32)
 * History	   :
 *                :  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
void
usbPrevResolutionChgCb(
	UINT32 width,
	UINT32 height
)
{
	UINT32 w = width, h = height, mode = 0x30;
	UINT32 fac2nd = 0x11;
	#if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
	UINT32 mode2nd=0x30;
	#endif

	sp5kStillCapCfgSet(SP5K_CAPTURE_QV_LENGTH, 0);

	if(width == 0 && height == 0) {
		#if SPCA6330
		sp5kModeSet(SP5K_MODE_STANDBY);
		sp5kModeWait(SP5K_MODE_STANDBY);
		sp5kSystemCfgSet(SP5K_CDSP_2NDPATH_FACTOR_CFG, 0x11);
		#endif
		return;
	}

	/*should make the w and h most relative to
	the sensor,s size.*/
	/*set for 0V5633 sensor*/

/*		#if SP5K_SENSOR_OV5633
	if ((width==1280)&&(height==720)) {
		w = 1280;
		h = 720;
		mode = 0x30;  //1280*960
	}
	else if ((width=640)&&(height==480)) {
		w = 640;
		h = 480;
		mode = 0x31;
	}
	else if ((width=320)&&(height==240)){
		w = 320;
		h = 240;
		mode = 0x31;
	}
	else if ((width=1920)&&(height==1080)){
		w = 1920;
		h = 1080;
		mode = 0x32;
		fac2nd = 0x22;
	}
	else if ((width=1600)&&(height==1200)){
		w = 640;
		h = 480;
		mode = 0x31;
	}
	#elif SP5K_SENSOR_MT9J003
	if (((width=640)&&(height==480))||((width=320)&&(height==240)))
	{
		w = width;
		h = height;
		mode = 0x30;
	}
	else if ((width=1600)&&(height==1200))
	{
		w = 640;
		h = 480;
		mode = 0x30;
	}
	else
	{
		w = 1280;
		h = 720;
		mode = 0x32;
	}
	#elif SP5K_SENSOR_SONYIMX083
	if (((width=640)&&(height==480))||((width=320)&&(height==240)))
	{
		w = width;
		h = height;
		mode = 0x30;
	}
	else if ((width=1600)&&(height==1200))
	{
		w = 640;
		h = 480;
		mode = 0x30;
	}
	else if ((width=1920)&&(height==1080))
	{
		w = 1920;
		h = 1080;
		mode = 0x33;
		fac2nd = 0x22;
	}
	else
	{
		w = 1280;
		h = 720;
		mode = 0x32;
	}
	#elif SP5K_SENSOR_OV2715
	if (((width=640)&&(height==480))||((width=320)&&(height==240)))
	{
		w = width;
		h = height;
		mode = 0x33;
	}
	else if ((width=1600)&&(height==1200))
	{
		w = 640;
		h = 480;
		mode = 0x33;
	}
	else if ((width=1920)&&(height==1080))
	{
		w = width;
		h = height;
		mode = 0x30;
		fac2nd = 0x22;
	}
	else
	{
		w = 1280;
		h = 720;
		mode = 0x31;
	}
	#elif SP5K_SENSOR_OV4689
	if ((width==1280)&&(height==720)) {
		w = 1280;
		h = 720;
		mode = 0x36;  //1280*960
	}
	else if ((width=640)&&(height==480)) {
		w = 640;
		h = 480;
		mode = 0x36;
	}
	else if ((width=320)&&(height==240)){
		w = 320;
		h = 240;
		mode = 0x36;
	}
	else if ((width=1920)&&(height==1080)){
		w = 1920;
		h = 1080;
		mode = 0x36;
	}
	else if ((width=1600)&&(height==1200)){
		w = 640;
		h = 480;
		mode = 0x3d;
	}
	#endif*/
	#if SP5K_SENSOR_OV4689
	if ((width==1280)&&(height==720)) {
		mode = SENSOR_MODE_1080P_30FPS;  //FIX ME
	}
	else if ((width=640)&&(height==480)) {
		mode = 0x36;
	}
	else if ((width=640)&&(height==360)) {
		mode = 0x36;
	}
	else if ((width=320)&&(height==240)){
		mode = 0x36;
	}
	else if ((width=1920)&&(height==1080)){
		mode = 0x36;
	}
	else if ((width=1600)&&(height==1200)){
		mode = 0x3d;
	}
	#else
		HOST_ASSERT(0);
	#endif

#if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
	if (((width=640)&&(height==480)) || ((width=320)&&(height==240)))
		mode2nd = 0x32;
	else if ((width=1920)&&(height==1080))
		mode2nd = 0x30;
	else if ((width=1280)&&(height==720))
		mode2nd = 0x31;
	sp5kModeCfgSet(SP5K_MODE_CFG_PV_DISP_SCALE_FACTOR, 0, 0, 0);
#endif
	#if SPCA6330
	sp5kSystemCfgSet(SP5K_CDSP_2NDPATH_FACTOR_CFG, fac2nd);
	#endif
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_WIDTH, w);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_HEIGHT, h);
	#if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_WIDTH, w);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_HEIGHT, h);
	appVideoFormatSet();
	appVideoSizeSet();
	appVideoSetQuality();
	appVideoFifoSet();
	if (appDualPipModeGet() >= DUALSEN_SINGLE_SEC)
		sp5kUsbCfgSet(SP5K_USB_PCCAM_UVC_DRAM_CLIP_SRC_SET, 1);
	else
		sp5kUsbCfgSet(SP5K_USB_PCCAM_UVC_DRAM_CLIP_SRC_SET, 0);
	sp5kSensorModeCfgSet(SP5K_MODE_SEC_SENSOR_SEL|SP5K_MODE_STILL_PREVIEW, mode2nd);
	#endif

	sp5kStillCapCfgSet(SP5K_CAPTURE_YUV_BUF_NUM, 1);
	sp5kModeSet(SP5K_MODE_STILL_PREVIEW);
	sp5kModeWait(SP5K_MODE_STILL_PREVIEW);

	sp5kSensorModeCfgSet(SP5K_MODE_VIDEO_PREVIEW, mode);
	sp5kModeSet(SP5K_MODE_VIDEO_PREVIEW);
	sp5kModeWait(SP5K_MODE_VIDEO_PREVIEW);
}

  /*--------------------------------------------------------------------------*
 * Function name	: appPowerOnLogoShow
 * Function	       	: Show logo when power on
 * Return value   	: void
 * Parameter1    	: void
 * History	   :
 *                :  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
void
appPowerOnLogoShow(
	void
)
{
    #if SP5K_USB_UVC_SUPPORT
    if(appUvcPwrOnRecGet())
       return;
    #endif
	//check start up image setting is ON
	if(pUiSetting->powerOnLogo == POWERON_LOGO_NONE)
		return;

	profLogAdd(LEVEL_TRACE, "[host]pwr: show logo s");

	sp5kGfxAttrSet(SP5K_GFX_REFRESH_ACTIVE,0, 0, 0, 0);
	appOsd_ColorDrawShapeColorSet(PAL_BLACK, PAL_BLEND_100, PAL_BLACK, PAL_BLEND_100);
	appOsdLib_PFLayerShapeDraw(APP_OSD_SHAPE_RECT,0, 0, 320, 240, 0, 0);
	if ( (pUiSetting->powerOnLogo == POWERON_LOGO_CUSTOM) && appFileExistCheck(RES_JPG_LOGO_CUSTOM) ) {
		appJpgDraw_PipLayer(RES_JPG_LOGO_CUSTOM,0,0,0,0);
	}
	else {
		appJpgDraw_PipLayer(RES_JPG_LOGO_SYS,0,0,0,0);
	}

	sp5kGfxAttrSet(SP5K_GFX_REFRESH_ACTIVE,1, 0, 1, 0);
	LCD_BACKLIGHT_ON;
	LCD_WAKEUP;

	profLogAdd(LEVEL_TRACE, "[host]pwr: show logo e");
}

  /*--------------------------------------------------------------------------*
 * Function name	: appFifInit
 * Function	       	: FIF initial
 * Return value   	: void
 * Parameter1    	: void
 * History	   :
 *                :  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
void
appFifInit(
	void
)
{
	#if 0
	profLogAdd(LEVEL_TRACE, "[host]pwr: uidata fif init s");
	if (fsFifOpen((const char *)FIFB_UI_DATA)==0)
	{
		profLogAdd(LEVEL_TRACE, "[host]pwr: uidata load fail..");
		HOST_PROF_LOG_PRINT(LEVEL_ERROR, "pwr: load %s fail",FIFB_UI_DATA);
	}
	profLogAdd(LEVEL_TRACE, "[host]pwr: uidata fif init -e");
	#endif

	profLogAdd(LEVEL_TRACE, "[host]pwr: colddata fif init -s");
	if (fsFifOpen((const char *)FIF_COLD_DATA)==0)
	{
		profLogAdd(LEVEL_TRACE, "[host]pwr: colddata load fail..");
		HOST_PROF_LOG_PRINT(LEVEL_ERROR, "pwr: load %s fail",FIF_COLD_DATA);
	}
	profLogAdd(LEVEL_TRACE, "[host]pwr: colddata fif init -e");

	#if !SP5K_MULTI_FAST_BOOT
	/*remove to function devAudioEnum*/
	profLogAdd(LEVEL_TRACE, "[host]pwr: audio fif init -s");
	if (fsFifOpen((const char *)FIF_AUDIO_DATA)==0)
	{
		profLogAdd(LEVEL_TRACE, "[host]pwr: audio load fail..");
		HOST_PROF_LOG_PRINT(LEVEL_ERROR, "pwr: load %s fail",FIF_AUDIO_DATA);
	}
	profLogAdd(LEVEL_TRACE, "[host]pwr: audio fif init -e");
	#endif
}

extern UINT8* appCathyWareVersionGet();

  /*--------------------------------------------------------------------------*
 * Function name	: appHostFastBootInit
 * Function	       	: Host fast boot initial
 * Return value   	: void
 * Parameter1    	: void
 * History	   :
 *                :  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
void
appHostFastBootInit(
	void
)
{
	UINT8 verStr[8];

    sp5kBaseFwReleasedVer(verStr);
    printf("+-----------------------------------------------+\n");
    printf("| F/W compiled at %s, %s \n",__TIME__, __DATE__);
    printf("| BSP       : (%s) \n", (char *)appBSPVersionGet());
    printf("| HOST      : (%s) \n", (char *)appSvnGet());
    printf("| BASEFW    : (%s) \n", verStr);
    printf("| JETFW    : 0.06 \n");
    appCathyWareVersionGet();
    printf("+-----------------------------------------------+\n");

	profLogInit(LEVEL_TRACE,0x8000);

	HOST_PROF_LOG_PRINT(LEVEL_INFO, "pwr: %s (%d)", __FUNCTION__, tmrMsTimeGet());
	profLogAdd(LEVEL_TRACE, "[host]pwr: appHostFastBootInit start");

    appPowerOnKeyGet();

    appPowerKeyNormalInit();

    appWatchDogInit_SW();

    #if APP_LOG_TO_SD_ENABLE
    appSDLogMsg();
    #endif

    #if APP_DEAD_MENU_ENABLE
	extern UINT32 cmdOsDeadUrgentCb(void (*fp)(void));
	UINT32 proc_en = _dbg_proc_ps_en(0);
	if (proc_en == 0)
		cmdOsDeadUrgentCb(appHostDeadMunuCb);
	else
		_dbg_proc_ps_en(proc_en);
	#else
	cmdOsDeadUrgentCb(appHostDeadDump);
    #endif

	#if SP5K_CDFS_OPT
	appCdfsCbInit();
	#else
	sp5kDcfStdSysInit();
	#endif

	appTvLcdInit();
	appTvLcdStart();

	#if !SP5K_MULTI_FAST_BOOT
	appExifInit();
	appFifInit();
	appUiParamInit();
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	appInfoGSensorCompensationSet();
#endif
	#endif

	/*appBatteryCheckInit();*/	/*20151118*/
	#if !(POWERON_PB_WITHOUT_SENSOR)
	    #if SP5K_DIQ_FRMWRK
		    appIqPowerOnInit();
	    #else
	        appIqOffPowerOnInit();
	    #endif
	#endif

    #if SP5K_USB_UVC_SUPPORT
    appUvcCamInit();
    #endif
	profLogAdd(LEVEL_TRACE, "[host]pwr: appHostFastBootInit end");
}


  /*--------------------------------------------------------------------------*
 * Function name	: appAeAwbInit
 * Function	       	: AE/AWB initial
 * Return value   	: void
 * Parameter1    	: void
 * History	   :
 *                :  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
void
appAeAwbInit(
	void
)
{
	profLogAdd(LEVEL_TRACE, "[host]pwr: AA init:s");
	/*DynIqTaskInit();*/

	appAeInit();
	appAwbInit();
	profLogAdd(LEVEL_TRACE, "[host]pwr: AA init:e");
}

  /*--------------------------------------------------------------------------*
 * Function name	: appUiParamInit
 * Function	       	: UI Param initial
 * Return value   	: void
 * Parameter1    	: void
 * History	   :
 *                :  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
void
appUiParamInit(
	void
)
{
    UINT32 ret, result;
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	UINT32 recordingTime= 0;
#endif
    profLogAdd(LEVEL_TRACE, "[host]pwr: ui param init:s");

    ret = sp5kDiskInfoGet(SP5K_DRIVE_RSVB, SP5K_DISK_STATE, &result);
    if(!result)
        sp5kRsvBlkFormat(SP5K_DRIVE_RSVB);

    ret = appUserSettingLoad(pUiSetting);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
   if(ret == SUCCESS)
	recordingTime = ((pUiSetting->ParkingModereCordingTimeH<<8)|pUiSetting->ParkingModereCordingTimeL);

    if(recordingTime != 10 && recordingTime !=30 &&recordingTime!=60){
		printf("Recover RSV B... ");
		ret = sp5kRsvBlkFormat(SP5K_DRIVE_RSVB);
		_userSettingDefaultSet(pUiSetting);
		 appUserSettingSave(pUiSetting);
		 appInfoParkingSettings();
    	}
#endif
    if(ret == FAIL)
        appUserSettingSave(pUiSetting);

	appUiParaInit(PARA_INIT_FROM_UDF);

    UiParamViewInit();
    UiParamViewChangeMode(0);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
    pViewParam->videoSize = pUiSetting->videoSize;
    appAudioRecFlagSet(pUiSetting->voiceSet);

    /* On ACC on clean G senosr save */
    appGsensorModeSave(0, 0);
    if (appGsensorSettingGet() &&
		    appInfoPowerDETStateGet() == POWER_DET_ACC_ON) {
        appGsensorLevelCtrl(1, (appGsensorSettingGet() - 1));
    } else
        appGsensorLevelCtrl(0, 0);
#endif

	#if SP5K_USB_UVC_SUPPORT
	if(pViewParam->videoSize>=UI_VIDEO_DUAL_SIZE_FHD_VGA)
		pViewParam->videoSize = UI_VIDEO_DUAL_SIZE_FHD_720P;
	#endif

    appSundVolumeSet(pUiSetting->sound.volume);

    profLogAdd(LEVEL_TRACE, "[host]pwr: ui param init:e");
}

  /*--------------------------------------------------------------------------*
 * Function name	: appAudioInit
 * Function	       	: Audio initial
 * Return value   	: void
 * Parameter1    	: void
 * History	   :
 *                :  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
void
appAudioInit(
	void
)
{
	sp5kAudDevSrcSet(SP5K_AUD_DEV_PLAY, AUD_PLAY_SRC_FOR_LCD);
	sp5kAudDevSrcSet(SP5K_AUD_DEV_REC, AUD_REC_SRC);
    sp5kMediaRecCfgSet(SP5K_MEDIA_REC_DUP_L_TO_R_ENABLE, 1);
}

  /*--------------------------------------------------------------------------*
 * Function name	: appPanelInit
 * Function	       	: panel init
 * Return value   	: void
 * Parameter1    	: void
 * History	   :
 *                :  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
void
appPanelInit(
	void
)
{
	profLogAdd(LEVEL_TRACE, "[host]pwr: panel init");
	appTvLcdStart();
}

  /*--------------------------------------------------------------------------*
 * Function name	: appPowerOnSoundPlay
 * Function	       	: Power On Sound Play
 * Return value   	: void
 * Parameter1    	: void
 * History	   :
 *                :  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
void
appPowerOnSoundPlay(
	void
)
{
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	UINT32 err;
#endif
	/* paul 2018.04.16 Add for modify compiler warning*/
#if !defined (PRJ_17DVR)&&!defined (PRJ_19DVR)/*weikai added fixed gcc warning 180524*/
	char *wavFilename[POWERON_SOUND_MAX] =
	{
		NULL,
		(char *)RES_WAV_POWER_ON1,
	};
#endif

	profLogAdd(LEVEL_TRACE, "[host]pwr: play opening sound :s");

	appAudioDevOn();

	if (pUiSetting->sound.powerOn == POWERON_SOUND_NONE)
		return;
		
    #if SP5K_USB_UVC_SUPPORT
    if(appUvcPwrOnRecGet())
	return;
    #endif

#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	    err = sp5kHostMsgSend(INFO_QUEUEMSG_BUZZERONOFF, BUZZER_CTRL_POWERON);
	    HOST_ASSERT(err == SP5K_SUCCESS);
#else
	sp5kResourceFilePlay(SP5K_RES_TYPE_WAV, (const char *)appRootGet(wavFilename[pUiSetting->sound.powerOn]), NULL);
#endif

	profLogAdd(LEVEL_TRACE, "[host]pwr: play opening sound :e");
}

  /*--------------------------------------------------------------------------*
 * Function name	: _powerOnStateGet
 * Function	       	: Get power on state
 * Return value   	: state (UINT32)
 * Parameter1    	: void
 * History	   :
 *                :  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
UINT32
_powerOnStateGet(
	void
)
{
	UINT32 state = APP_STATE_NULL;

	/*Definition for USB boot which defined in ".\customization\hostfw\makefile"*/
	#if (OPT_USB_BOOT || CMD_GDB)	/*add gdb */
	HOST_PROF_LOG_ADD(LEVEL_INFO, "pwr: Power on with USB boot or GDB");

	#if CMD_GDB
	if (getDgbType() == 1)
	#endif
	{
		appCalibAPITestSettingSet( 1 );
	}
	#endif  /*end gdb*/

	#if POWERON_PB_WITHOUT_SENSOR
	appPowerOnInfo.isPbPowerOn =1 ;
	#endif

	#if COMPEL_POWEROFF_CHARGE
	if(appGetBoot4ChargingFlag())
		appPowerOnInfo.isPowerOnForCharging =1 ;
	#endif

	if(appPowerOnInfo.isPbPowerOn)
	{
		fileTypeSelect = 1; /*ez_menu for pb filetype */
		sp5kModeSet(SP5K_MODE_STANDBY);
		sp5kModeWait(SP5K_MODE_STANDBY);
		state = APP_STATE_MENU;
	}
	#if COMPEL_POWEROFF_CHARGE
	else if(appPowerOnInfo.isPowerOnForCharging)
	{
		state = APP_STATE_USB_BATTERY_CHARGING;
	}
	#endif
	#if 1
	else
	{
		state = APP_STATE_VIDEO_PREVIEW;
	}
	#else
	else if (pUiSetting->capSetting.CurViewMode == VIDEO_MODE)
	{
		state = APP_STATE_VIDEO_PREVIEW;
	}
	else
	{
#if SP5K_USB_UVC_SUPPORT
		state = APP_STATE_VIDEO_PREVIEW;
#else
		state = APP_STATE_STILL_PREVIEW;
#endif
	}
	#endif
	if((pUiSetting->periodicalCap.periodicalSet == PERIODICAL_CAPTURE_OFF) && (pUiSetting->capSetting.CurViewMode == VIDEO_MODE))
		state = APP_STATE_VIDEO_PREVIEW;

	return state;
}

  /*--------------------------------------------------------------------------*
 * Function name	: appPowerOnViewSet
 * Function	       	: power on view set 
 * Return value   	: void
 * Parameter1    	: startupState (UINT32)
 * History	   :
 *                :  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
void
appPowerOnViewSet(
	UINT32 startupState
)
{
	HOST_PROF_LOG_PRINT(LEVEL_INFO,"pwr: appPowerOnViewSet %d", startupState);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	appModeWaitCfg(TRUE);
	HOST_PROF_LOG_PRINT(LEVEL_INFO,"pwr: appPowerOnViewSet C..%d", startupState);
#endif
	/*
	config still pv size to VGA, you nend check "digital flow"
	keyword: digital effect init set
	*/
	sp5kModeCfgSet(SP5K_MODE_CFG_PV_BUF_ALLOC_BOTTOM,1);
	sp5kModeCfgSet(SP5K_MODE_CFG_STILL_PV_SIZE, CFG_WIDTTH_VGA, CFG_HEIGHT_VGA, 8);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
    sp5kMediaRecCfgSet(SP5K_MEDIA_REC_AUD_FRAME_DURATION, 25);
#endif
    #if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
    sp5kModeCfgSet(SP5K_MODE_CFG_PV_DISP_FIT, 1);
    #endif
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	sp5kModeCfgSet(SP5K_MODE_CFG_STANDBY_FRAME_SIZE, 720, 480);
	sp5kModeCfgSet(SP5K_MODE_CFG_STNDBY_FRAME_BUF_SIZE, 720, 480);
#else
	sp5kModeCfgSet(SP5K_MODE_CFG_STANDBY_FRAME_SIZE, 320, 240);
	sp5kModeCfgSet(SP5K_MODE_CFG_STNDBY_FRAME_BUF_SIZE, 320, 240);
#endif
	//please note : capture size / video size MUST be set befor mode set
	if(startupState == APP_STATE_STILL_PREVIEW)
	{
		profLogAdd(LEVEL_TRACE, "[host]pwr: be view set s");

		appStill_SetSp5kStillSize(UI_STILL_SIZE_FULL);
		/*appModeSet(SP5K_MODE_STILL_PREVIEW);*/
		profLogAdd(LEVEL_TRACE, "[host]pwr: af view set e");
	}
	else if(startupState == APP_STATE_VIDEO_PREVIEW)
	{
		profLogAdd(LEVEL_TRACE, "[host]pwr: be video view set s");
		appStill_SetPasmSceneParam(VIDEO_MODE, 0);
		///////////////Mark for set in VideoView/////// appVideoSizeSet();
		profLogAdd(LEVEL_TRACE, "[host]pwr: af video view set e");
	}
    else if((startupState == APP_STATE_MENU)&&(fileTypeSelect == 1))/*ez_menu for pb filetype */
	{
		profLogAdd(LEVEL_TRACE, "[host]pwr: playback mode set s");
        appStill_SetSp5kStillSize(UI_STILL_SIZE_FULL);
		profLogAdd(LEVEL_TRACE, "[host]pwr: playback mode set e");
	}
#if APP_STATE_CUSTOMER_VIDEOPV
	else if(APP_STATE_CUSTOMER_VIDEO_PREVIEW){
		profLogAdd(LEVEL_TRACE, "[host]pwr: customer video preview set s");
		appCustomerVideoPVSetCaptureInfo();
		profLogAdd(LEVEL_TRACE, "[host]pwr: customer video preview set e");
	}
#endif
	/*ae setting*/
	sp5kAeModeSet(SP5K_AE_MODE_INFO_ONLY);
	sp5kAwbModeSet(SP5K_AWB_MODE_INFO_ONLY);

    sp5kSystemCfgSet(SP5K_MEDIA_SYS_CFG, SP5K_MEDIA_SYS_CFG_H264_SEQ_SCALING_MATRIX_PRESENT, 1);
}

  /*--------------------------------------------------------------------------*
 * Function name	: appPwrOnTask
 * Function	       	: power on Task
 * Return value   	: void
 * Parameter1    	: param (UINT32)
 * History	   :
 *                :  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

#if SP5K_MULTI_FAST_BOOT
SP5K_THREAD *pPwrOnThread = NULL;
void
appPwrOnTask(
	UINT32 param
)
{
	while(1)
	{
		profLogAdd(LEVEL_TRACE, "[host]pwr: pPwrOnThread :s");

		profLogAdd(LEVEL_TRACE, "[host]pwr: exif init :s");
		appExifInit();
		profLogAdd(LEVEL_TRACE, "[host]pwr: exif init :e");

		appFifInit();
		appUiParamInit();

#if !SP5K_ULTRA_FAST_BOOT
		profLogAdd(LEVEL_TRACE,"ULTRA_FAST_BOOT:appPowerOnLogoShow s");
		appPowerOnLogoShow();
		profLogAdd(LEVEL_TRACE,"ULTRA_FAST_BOOT:appPowerOnLogoShow e");

		profLogAdd(LEVEL_TRACE,"ULTRA_FAST_BOOT:appPowerOnSoundPlay s");

		appAudioInit();
		appPowerOnSoundPlay();

		profLogAdd(LEVEL_TRACE, "[host]pwr: pre load sound :s");
		appPreloadSoundInit();
		profLogAdd(LEVEL_TRACE, "[host]pwr: pre load sound :e");
		profLogAdd(LEVEL_TRACE,"ULTRA_FAST_BOOT:appAudioInit e");
#endif


		profLogAdd(LEVEL_TRACE, "[host]pwr: pPwrOnThread :e");

		appBatteryCheckInit();	/*20151118*/
		sp5kOsThreadTerminate(pPwrOnThread);
		sp5kOsThreadDelete(pPwrOnThread);
	}
}
#endif

  /*--------------------------------------------------------------------------*
 * Function name	: appPowerOnStateInit
 * Function	       	: power on state initial
 * Return value   	: void
 * Parameter1    	: void
 * History	   :
 *                :  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
void
appPowerOnStateInit(
	void
)
{
	profLogPrintf(LEVEL_TRACE,"host:%s s",__FUNCTION__);

	/*for usb pccam flow*/
    sp5kUsbCfgSet(SP5K_USB_PCCAM_UVC_RESOLUTION_CHANGE_CB, usbPrevResolutionChgCb);
	usbVndrIcatPreCaptureCallbackSet(Icatsnap3Acb);

	appEZ_menuParaChange();/*ez_menu uipara change to cathylite para*/

	#if PURE_FW_POWER_ON
	return;
	#else
	if (appLowBatteryCheck(APP_BATT_CHK_SYS_BOOT))
	{
		return;
	}
	#endif

	appBatteryCharge_Init();
	appBackgroundTimerInit();
	
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	LED_NORMAL_ON;
#else
if(IS_CARD_EXIST)	
	LED_FLICKER_POWER_ON;	 /* to light on the LED_POWER of camera */
#endif
#if SP5K_SENSOR_DUAL_MODE
    appDualModeSensorCbReg();
#else

#if (SP5K_SENSOR_ADV7480)\
	||(SP5K_SENSOR_AR0230)\
	||(((SP5K_SENSOR_OV4689)&&(SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE)))\
	||(((SP5K_SENSOR_OV9734)&&(SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE)))\
	||(((SP5K_SENSOR_OV7695)&&(SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE)))

appModeSensorCbReg();
#endif

#endif
	appTzrealRtcCbReg(appRealRtcSet);
	appTzlocalTimeCbReg(appLocalTimeSet);

	appPanelInit();

	/* RTC check */
	if(appRtcLostCheck())
	{
		appRtcDefaultSet();
		sp5kRtcReliableSet(APP_RTC_RELIABLE_CODE);
	}

	sp5kRtcCurrentSet(4);
	#if COMPEL_POWEROFF_CHARGE
	if(!appGetBoot4ChargingFlag())
	#endif
	{
		appWaitOSDInitReady();

		#if SP5K_MULTI_FAST_BOOT
		pPwrOnThread = sp5kOsThreadCreate("PowerOnThread",
							  appPwrOnTask,
							  0,
							  15,
							  15,
							  0,
							  1);
		#else

#if !SP5K_ULTRA_FAST_BOOT
			profLogAdd(LEVEL_TRACE,"ULTRA_FAST_BOOT:appAudioInit s");
			appAudioInit();
			profLogAdd(LEVEL_TRACE,"ULTRA_FAST_BOOT:appAudioInit e");
			profLogAdd(LEVEL_TRACE, "[host]pwr: show logo :s");
			profLogAdd(LEVEL_TRACE,"ULTRA_FAST_BOOT:appPowerOnLogoShow s");
			appPowerOnLogoShow();
			profLogAdd(LEVEL_TRACE,"ULTRA_FAST_BOOT:appPowerOnLogoShow e");
#endif
			profLogAdd(LEVEL_TRACE,"ULTRA_FAST_BOOT:appPowerOnSoundPlay e");
#if !SP5K_ULTRA_FAST_BOOT
			profLogAdd(LEVEL_TRACE, "[host]pwr: play opening sound :s");
			appPowerOnSoundPlay();
			if (fsFifOpen((const char *)FIF_AUDIO_DATA)==0)
			{
				profLogAdd(LEVEL_TRACE, "[host]pwr: audio load fail..");
				HOST_PROF_LOG_PRINT(LEVEL_ERROR, "pwr: load %s fail",FIF_AUDIO_DATA);
			}

			appPreloadSoundInit();
#endif
			profLogAdd(LEVEL_TRACE,"ULTRA_FAST_BOOT:appPowerOnSoundPlay e");

			//sp5kStillCapCfgSet(SP5K_CAPTURE_SHUTTER_SND_NAME, (pUiSetting->sound.shutter)? (UINT32)RES_WAV_SHUTTER : NULL);
		#endif

		#if !(POWERON_PB_WITHOUT_SENSOR)
			appAeAwbInit();
			appAEALGInit();
		#endif

		#if SP5K_DIQ_AUTO_WDR
			appAutoWDRLibInit();
		#endif

		#if SP5K_MOTION_DETECT
	        appMotionDetectSet(APP_MD_REG, TRUE);
	        appMotionDetectSet(APP_MD_EN, puiPara->MotionDetect?TRUE:FALSE);
	        appMotionDetectSet(APP_MD_CHKLV_SET, APP_MD_LV1);
		#endif

		#if SP5K_MOTION_DETECT_FRONT
	        appMotionDetectSet(APP_MD_EN_FRONT, TRUE);
		#endif

		#if SP5K_MOTION_DETECT_REAR
	        appMotionDetectSet(APP_MD_EN_REAR, TRUE);
		#endif

        appModeWaitCfg(TRUE);
        sp5kMediaRecCfgSet(SP5K_MEDIA_REC_AUD_FRAME_DURATION, 25);
        sp5kModeCfgSet(SP5K_MODE_CFG_STANDBY_FRAME_SIZE, 320, 240);
        sp5kModeCfgSet(SP5K_MODE_CFG_STNDBY_FRAME_BUF_SIZE, 320, 240);

		#if !(POWERON_PB_WITHOUT_SENSOR)
			#if !(PURE_TESTMODE_POWER_ON)
				profLogAdd(LEVEL_TRACE, "[host]pwr: _powerOnStateGet");
				#if APP_STATE_CUSTOMER_VIDEOPV
					appPowerOnViewSet(APP_STATE_CUSTOMER_VIDEO_PREVIEW);
				#else
					appPowerOnViewSet(_powerOnStateGet());
				#endif
				profLogAdd(LEVEL_TRACE, "[host]pwr: appPowerOnViewSet");
			#endif
	    #endif
	}

	#if SP5K_MULTI_FAST_BOOT
	/*load osd res remove to function appViewOsdShow()*/
	sp5kHostMsgSend(APP_UI_MSG_POWER_ON_FLOW_GO_ON, POWER_ON_PHASE_BTN_INIT);
	#else
	sp5kHostMsgSend(APP_UI_MSG_POWER_ON_FLOW_GO_ON, POWER_ON_PHASE_OSD_PRELOAD);
	#endif

	profLogPrintf(LEVEL_TRACE,"host:%s e",__FUNCTION__);
}

  /*--------------------------------------------------------------------------*
 * Function name	: appPowerOnFlowGoOn
 * Function	       	: Power On Flow 
 * Return value   	: void
 * Parameter1    	: phase (UINT32)
 * History	   :
 *                :  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
void
appPowerOnFlowGoOn(
	UINT32 phase
)
{

	if(phase == POWER_ON_PHASE_OSD_PRELOAD)
	{
		profLogAdd(LEVEL_TRACE, "[host]pwr: osd init");
		appOsdResPreload();
		sp5kHostMsgSend(APP_UI_MSG_POWER_ON_FLOW_GO_ON, POWER_ON_PHASE_BTN_INIT);
	}
	else if(phase == POWER_ON_PHASE_BTN_INIT)
	{
		appBtnInit();

		/* Calibration mode power on (W + MENU + PWR)*/
		#if (PURE_TESTMODE_POWER_ON)
		appPowerOnInfo.isTestModePowerOn =TRUE;
		sp5kHostMsgSend(APP_UI_MSG_POWER_ON_ENTER_CALI_MODE, 0);
		#else
		if(appPowerOnInfo.isTestModePowerOn)
		{
			#if SP5K_MULTI_FAST_BOOT
			appOsdLib_PFLayerShapeClear(APP_OSD_SHAPE_RECT, 0, 0, LCD_SIZE_X, LCD_SIZE_Y, 0, 0);
			#endif
			appOsdResPreload();
			sp5kHostMsgSend(APP_UI_MSG_POWER_ON_ENTER_CALI_MODE, 0);
		}
		else
		{
			if(appProgAllCheck())
			{
				sp5kHostMsgSend(APP_UI_MSG_POWER_ON_ENTER_SET_LANGUAGE, 0);
			}
			else
			{
				sp5kHostMsgSend(APP_UI_MSG_POWER_ON_ENTER_UI_MODE, 0);
			}
		}
		#endif
	}
	else
	{
		profLogAdd(LEVEL_TRACE, "[host]pwr: power on go on error!!");
		HOST_PROF_LOG_ADD(LEVEL_ERROR, "pwr: power on go on error");
	}
}

  /*--------------------------------------------------------------------------*
 * Function name	: appPowerOnStateClose
 * Function	       	: state close for power on
 * Return value   	: void
 * Parameter1    	: void
 * History	   :
 *                :  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
void
appPowerOnStateClose(
	void
)
{
	DBG_PRINT("[host]pwr: %s ..\n",__FUNCTION__);

	#if SP5K_MULTI_FAST_BOOT
	/*open img in pv state init avoid green image*/
	#else
	sp5kDispAttrSet(sp5kDispActChnlGet(0), SP5K_DISP_IMG_ACTIVE, 1);
	#endif
}

  /*--------------------------------------------------------------------------*
 * Function name	: appPowerOnEnterUIMode
 * Function	       	: power on enter UI mode
 * Return value   	: void
 * Parameter1    	: void
 * History	   :
 *                :  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
void appPowerOnEnterUIMode(void)
{
	profLogAdd(LEVEL_TRACE, "[host]pwr: msg enter UI mode");
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	if(IS_CARD_EXIST)
#endif
	{
		LED_NORMAL_ON; /* to light on the LED_POWER of camera */
	}
	if (!IS_HDMI_IN)
	{
		LCD_BACKLIGHT_ON;
		LCD_WAKEUP;
		LCD_WAKEUP;
	}

	appProgAllClear();

#if APP_STATE_CUSTOMER_VIDEOPV
    appCustomerVideoPvStateChange();
#else
	appStateChange( _powerOnStateGet(), STATE_PARAM_POWER_ON );
#endif
}

  /*--------------------------------------------------------------------------*
 * Function name	: appPowerOnState
 * Function	       	: power on state process
 * Return value   	: void
 * Parameter1    	: state messages (UINT32)
 * Parameter2    	: messages parameter (UINT32)
 * History	   :
 *                :  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
void
appPowerOnState(
	UINT32 msg,
	UINT32 param
)
{

	DBG_PRINT("[host]pwr: %s:[0x%x] [0x%x]\n",__FUNCTION__,msg,param);

	switch (msg) {
	case APP_STATE_MSG_INIT:
		RPrintf("appPowerOnState");
		profLogAdd(LEVEL_TRACE, "[host]pwr: enter power on state");
		appPowerOnStateInit();
		break;

	case APP_UI_MSG_POWER_ON_FLOW_GO_ON:
		profLogPrintf(LEVEL_TRACE, "[host]pwr: power on flow go on : %d",param);
		appPowerOnFlowGoOn(param);
		break;

	case APP_UI_MSG_POWER_ON_ENTER_SET_LANGUAGE:
		appStateChange(APP_STATE_POWER_ON_SET_LANGUAGE,STATE_PARAM_NORMAL_INIT);
		break;

	case APP_UI_MSG_POWER_ON_ENTER_UI_MODE:
		appPowerOnEnterUIMode();
		break;

	case APP_UI_MSG_POWER_ON_ENTER_CALI_MODE:
		LED_NORMAL_ON; /* to light on the LED_POWER of camera */
		LCD_BACKLIGHT_ON;
		LCD_WAKEUP;
		LCD_WAKEUP;
		appStateChange(APP_STATE_CALIBRATION,STATE_PARAM_POWER_ON);
		break;

	case SP5K_MSG_AE_READY:
#if 0
		appAePreviewAdjust(param, SP5K_MODE_STILL_PREVIEW);
#endif
		break;

	case APP_STATE_MSG_CLOSE:
		appPowerOnStateClose();
		profLogAdd(LEVEL_TRACE, "[host]pwr: leave power on state");
		appStateCloseDone();
		break;

	default:
		break;
	}
}

  /*--------------------------------------------------------------------------*
 * Function name	: appPwerOnInfoGet
 * Function	       	: get power on info
 * Return value   	: void
 * Parameter1    	:  *pbPowerOn (BOOL pointer)
 * Parameter2    	:  *mutePowerOn (BOOL pointer)
 * Parameter3    	:  *testModePowerOn (BOOL pointer)
 * History	   :
 *                :  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
void
appPwerOnInfoGet(
	BOOL *pbPowerOn,
	BOOL *mutePowerOn,
	BOOL *testModePowerOn
)
{
	*pbPowerOn = appPowerOnInfo.isPbPowerOn;
	*mutePowerOn = appPowerOnInfo.isMutePowerOn;
	*testModePowerOn = appPowerOnInfo.isTestModePowerOn;
}

