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
 *  File name	: 	App_power_off.c
 *  Function 	: 	power off		 	
 *  First editor	: 	
 *  First date  	: 	
 *  History		:
 *  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

#define HOST_DBG 1
#include "app_com_def.h"
#include "app_com_api.h"

#include "sp5k_global_api.h"
#include "sp5k_utility_api.h"
#include "sp5k_aud_api.h"
#include "sp5k_disp_api.h"
#include "sp5k_gfx_api.h"
#include "sp5k_usb_api.h"
#include "app_timer.h"
#include "app_lens_api.h"
#include "app_tvlcd.h"
#include "app_sensor.h"
#include "app_error_msg.h"
#include "app_sound.h"
#include "app_jpg_draw.h"
#include "app_osd_api.h"
#include "app_still.h"
#include "app_view_param.h"
#include "app_view_param_def.h"
#include "app_ui_para.h"
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
#include "app_rtc.h"
#include "app_infohandlerComm.h"
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

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
UINT32 PeriodicalCaptureTime=0;
static UINT32 powerOffEvent;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern void appResetBtnCtrlSet(UINT8 bNormalOff);
extern void appPowerOffWakeupSet(UINT32 param);
extern BOOL bPccamConn;

#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
/*
*
*
* 16383*(detected voltage)/(SARADC reference voltage) 
*
*/
#define BPLUS_LOW_LEAVEL 16383*2/3
extern PowerDetect appInfoPowerDETStateGet(void);
extern MoveFileState appInfoGetMoveFileState(void);
extern void appinfoMoveFileState(void);
extern void appInfoFinishFileNameset(UINT8 * buf);
extern void appinfoMoveFileStateInit(void);
extern void appGsensorLevelCtrl(int mode, int level);
extern void appGsensorModeSave(int mode, int level);
extern UINT32 appInfoBatteryLvGet(void);
extern void appInfoParkingModeCtrl( UINT32 count ,UINT8 * filename);

/* paul 2018.04.16 Add for modify compiler warning*/
extern UINT32 appInfoParkingSettingGet(void);
extern UINT32 appInfoParkingBatterySettingGet(void);
extern UINT8 appInfoParkingGSensorLevelGet(void);
extern BOOL appGsensorModeRestore();
extern void appInfoSWWDTReset();
extern void appInfoSWWDTWakeup(UINT32);
extern void halPwrcEnable(UINT32 en);
extern void appInfoLastPositionSave(void);

extern BOOL fileReady ;
extern BOOL NewAVNModel;
extern CDFS_FILE_TAB_T *lastNode;

#define READ8(a) (*((volatile UINT8*)(a)))
#define WRITE8(_reg_, _value_)  (*((volatile UINT8  *)(_reg_)) = (_value_))
#endif

#ifdef JET_FEATURE
extern char appPowerOffCommand[];
#endif

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

  /*--------------------------------------------------------------------------*
 * Function name	: _powerOffWarningProc
 * Function	       	: Warning process of power off 
 * Return value   	: void
 * Parameter1    	: void
 * History	   :
 *                :  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

void
_powerOffWarningProc(
	void
)
{
	switch(powerOffEvent){
	case STATE_PARAM_BATT_EMPTY:
		appErrMsgWithSound(ERR_MSG_OUT_OF_BATTERY,ERR_MSG_WITH_WARNING_SOUND,ERR_MSG_SHOW_1SEC);
		break;
	case STATE_PARAM_LENS_ERROR:
	case STATE_PARAM_VIDEO_REC_ERROR:
		appPreloadSoundPlay(SOUND_ID_WARNING);
		appTimeDelayMs( ERR_MSG_SHOW_1SEC);
		break;
	}
}

  /*--------------------------------------------------------------------------*
 * Function name	: appPeriodicalCaptureTimeSet
 * Function	       	: set Periodical Capture Time
 * Return value   	: void
 * Parameter1    	: void
 * History	   :
 *                :  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

void appPeriodicalCaptureTimeSet(void)
{
	switch(pUiSetting->periodicalCap.periodicalSet)
	{
	case PERIODICAL_CAPTURE_5SEC:
		PeriodicalCaptureTime=5;
		break;
	case PERIODICAL_CAPTURE_10SEC:
		PeriodicalCaptureTime = 10;
		break;
	case PERIODICAL_CAPTURE_30SEC:
		PeriodicalCaptureTime = 30;
		break;
	case PERIODICAL_CAPTURE_60SEC:
		PeriodicalCaptureTime = 60;
		break;
	case PERIODICAL_CAPTURE_OFF:
	default:
		PeriodicalCaptureTime=0;
		break;
	}

	if(!PeriodicalCaptureTime)
	{
		return;
	}

}

  /*--------------------------------------------------------------------------*
 * Function name	: appPsuedoPowerOff
 * Function	       	: Psuedo Power off process
 * Return value   	: void
 * Parameter1    	: powerDown (UINT32)
 * History	   :
 *                :  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

void
appPsuedoPowerOff(
	UINT32 powerDown
)
{
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	UINT32 pwrVal;
	UINT32 BP;
	int i = 0 ;
#endif
	if(appPreviousStateGet() == APP_STATE_VIDEO_REC){
		sp5kGfxInitCfgSet(SP5K_GFX_AB_FRAME_SIZE, OSD_DEFAULT_FRAMSIZE_W, OSD_DEFAULT_FRAMSIZE_H, 0, 0);
		sp5kGfxInitCfgSet(SP5K_GFX_PIP_PAGE_TOT, 1, 0, 0, 0);
		sp5kGfxInitCfgSet(SP5K_GFX_PIP_FMT, SP5K_GFX_FMT_RGB565, 0, 0, 0);
		sp5kGfxInitCfgSet(SP5K_GFX_PIP_FRAME_SIZE, OSD_DEFAULT_FRAMSIZE_W, OSD_DEFAULT_FRAMSIZE_H, 0, 0);
		sp5kGfxInitCfgSet(SP5K_GFX_PIP_WINDOW, 0, 0, OSD_DEFAULT_FRAMSIZE_W, OSD_DEFAULT_FRAMSIZE_H);
		sp5kGfxInit();
	}

	#if COMPEL_POWEROFF_CHARGE
	if(appGetBoot4ChargingFlag())
	{
		if(IS_USB_IN){
			sp5kRtcReliableSet(0x5a);
			sp5kRtcDateTimeGet(SP5K_DATE_TIME_OPTION, &rtc);
			rtc.tmx_sec += 1;
			sp5kRtcDateTimeSet(SP5K_WAKEUP_OPTION, &rtc);
		}
		sp5kPowerCtrl(SP5K_POWER_OFF, 0);
		while(1);
		return;
	}
	#endif

	if (bPccamConn){
		/* disconnect pccam,  force basefw to close uvc or uac if necessary */
		sp5kUsbSwCon(0);
	}

	CLEAR_OSD_SRC;
	sp5kGfxAttrSet(SP5K_GFX_REFRESH_ACTIVE,0, 0, 0, 0);
	if (IS_HDMI_IN) {
		appJpgDraw_PipLayer(RES_JPG_GOODBYE,0,0,640,360);
	}
	else {
		appJpgDraw_PipLayer(RES_JPG_GOODBYE,0,0,320,240);
	}
	sp5kDispAttrSet(sp5kDispActChnlGet(0), SP5K_DISP_IMG_ACTIVE, 0);
	sp5kGfxAttrSet(SP5K_GFX_REFRESH_ACTIVE,1, 0, 1, 0);

	sp5kTimeDelay(SP5K_TIME_DELAY_1MS,500);
#if !defined(PRJ_17DVR) && !defined(PRJ_19DVR)/*paul modify 2018.05.29 Bug #3670*/
	LED_FLICKER_POWER_OFF;
#endif
	/* UI param update*/
	/*appUserSettingSave(pUiSetting);*/

	appUIParaSave();

	if(pUiSetting->periodicalCap.periodicalSet > PERIODICAL_CAPTURE_10SEC){
		appPeriodicalCaptureTimeSet();
		appPowerOffWakeupSet(PeriodicalCaptureTime);
	}
	//power off sensor
	appSensorPowerOff();
	APP_SLEEP_MS( 1);

	//LED ON
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
    LCD_PWR_OFF;
    ALL_LED_OFF;
    APP_SLEEP_MS(100);
#endif

#if !defined(PRJ_17DVR)&&!defined(PRJ_19DVR)
	sp5kGpioWrite(SP5K_GPIO_GRP_GEN, 1<<0, 1);
	APP_SLEEP_MS(30);
	LCD_PWR_OFF;
	ALL_LED_OFF;
#endif

	DBG_PRINT(".done.\n");

	if (!powerDown)
	{
		return;
	}
	#if COMPEL_POWEROFF_CHARGE
	if(IS_USB_IN && (appBatteryAdcValueGet() > 100) && !appGetBoot4ChargingFlag())
	{
		sp5kRtcReliableSet(0x5b);
		sp5kRtcDateTimeGet(SP5K_DATE_TIME_OPTION, &rtc);
		rtc.tmx_sec += 1;
		sp5kRtcDateTimeSet(SP5K_WAKEUP_OPTION, &rtc);
	}
	else
		sp5kRtcReliableSet(0x5a);
	#endif
    appResetBtnCtrlSet(TRUE);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)

    sp5kGpioWrite(SP5K_GPIO_GRP_AUD_LINEOUT, 0x01, 0x00);
    sp5kGpioDirSet(SP5K_GPIO_GRP_AUD_LINEOUT, 0x01, 0x01);
    sp5kAudDevVrefSet(5, 4);
    printf("%s @@6454,64b8=%x,%x\n", __FUNCTION__, READ8(0xb0006454),
            READ8(0xb00064B8));

    gpioRead(SP5K_GPIO_GRP_FML, BIT1, &BP);
    printf("%s BP 0x%x check file(%p)!!!!!!\n", __FUNCTION__, BP, lastNode);

    if(lastNode != NULL && appInfoPowerDETStateGet() == POWER_DET_PARKING_WAKEUP) {
	printf("%s: delete parking file : %s\n", __func__, lastNode->FileName);
        appCdfsFileDel(lastNode);
    }

    if ((appInfoParkingSettingGet() && (BP == BIT1) )&& NewAVNModel
            && appInfoBatteryLvGet()
                    >= (appInfoParkingBatterySettingGet() - (UINT32)200)) {
        appGsensorModeSave(2, ((int) appInfoParkingGSensorLevelGet() - 1));
        if (appInfoPowerDETStateGet() == POWER_DET_PARKING_WAKEUP) {
            appGsensorModeRestore();
	    goto skip_deep_suspend;
	}
    } else
	    printf("Parking mode disable\n");

    appGsensorLevelCtrl(3, 4);
skip_deep_suspend:
    /* Check 1 sec for BP */
    gpioRead(SP5K_GPIO_GRP_FML, BIT1, &BP);
    sp5kGpioRead(SP5K_GPIO_GRP_POCU, 0xffffffff, &pwrVal);
    for (i = 0; i < 5; i++) {
        printf("%s ACC:0x%x BP:0x%x\n", __FUNCTION__, pwrVal, BP);
        if (BP == BIT1 && ((pwrVal & SP5K_PWR_SRC_USB) == SP5K_PWR_SRC_USB))
            break;
        sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 100);
        gpioRead(SP5K_GPIO_GRP_FML, BIT1, &BP);
        sp5kGpioRead(SP5K_GPIO_GRP_POCU, 0xffffffff, &pwrVal);

    }

#if DEBUG_ON
    printf("POWER OFF final\n");
#endif
    if ((pwrVal & SP5K_PWR_SRC_USB) == SP5K_PWR_SRC_USB && BP == BIT1) {
        appInfoSWWDTReset();
        appInfoSWWDTWakeup(1);
    } else
        appInfoSWWDTWakeup(0);
    halPwrcEnable(0);
#else
	sp5kPowerCtrl(SP5K_POWER_OFF, 0);
#endif
	while(1);
}

  /*--------------------------------------------------------------------------*
 * Function name	: appPowerOffState
 * Function	       	: Power off state process
 * Return value   	: void
 * Parameter1    	: msg (UINT32)
 * Parameter2    	: param (UINT32)
 * History	   :
 *                :  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

void
appPowerOffState(
	UINT32 msg,
	UINT32 param
)
{
	switch (msg)
	{
	case APP_STATE_MSG_INIT:
		RPrintf("appPowerOffState");
		appModeSet(SP5K_MODE_STANDBY);
#ifdef JET_FEATURE
		appInfoLastPositionSave();		/* UI param update*/		
		appUserSettingSave(pUiSetting);		/*appUIParaSave();*/		
		if(pUiSetting->periodicalCap.periodicalSet > PERIODICAL_CAPTURE_10SEC){		
			appPeriodicalCaptureTimeSet();		
			appPowerOffWakeupSet(PeriodicalCaptureTime);		
		}				
		if (strlen(appPowerOffCommand) > 0) {	        	
			sp5kCmdExec(appPowerOffCommand);	        	
			sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 100);		
		}
#endif
		powerOffEvent = param;
		appPsuedoPowerOff(TRUE);
		break;
	default:
		break;
	}
}

