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
#define HOST_DBG 1
#include "app_com_def.h"
#include "app_dcf.h"
#include "app_still.h"
#include "app_playback_def.h"
#include "app_battery.h"
#include "app_fast_boot.h"
#include "app_com_api.h"
#include "app_cdfs_api.h"
#include "app_rtc.h"
#include "app_res_def.h"
#include "app_ui_para.h"
#include "app_view_param.h"
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
#include "app_infohandlerComm.h"
#endif
#if GPS_OPEN
extern UINT8 gps_update_RtcTime_flag;
#endif

extern void appSetupMenu_SetCurLanguage(UINT8 nCurLanguage);

#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
extern BuzzerSettingState BuzzerActiveState;
extern void appInfoParkingSettings( void );
extern void JetPowerCanOff(int en);
extern void appUIParaReset(void);
#endif


void
_userSettingDefaultSet(
	uiParamSetting_t *pbuf
)
{
	/* set default to zero */
	memset(pbuf, 0, sizeof(uiParamSetting_t));

	pbuf->capSetting.CurViewMode = PASM_DEFAULT;
	pbuf->periodicalCap.periodicalSet= PERIODICAL_CAPTURE_DEFAULT;
	pbuf->sound.shutter = SHUTTER_SOUND_DEFAULT;
	pbuf->sound.powerOn = POWERON_SOUND_DEFAULT;
	pbuf->sound.beep = BEEP_DEFAULT;
	pbuf->sound.volume = SOUND_VOLUME_DEFAULT;
	pbuf->sound.VoiceRec = 1;
    pbuf->pbfileType = PB_FILETYPE_DEFAULT;
	
	#if defined(HW_PDVR)
	pbuf->GSENSOR = GSENSOR_STATUS_DEFAULT;
	#else
	pbuf->lightFrequency= LIGHT_FREQ_DEFAULT;
	#endif
	
	pbuf->powerSave = POWER_SAVE_DEFAULT;
	pbuf->powerOnRecord = POWER_ON_RECORD_DEFAULT;
	pbuf->dateTime = DATE_TIME_DEFAULT;
	pbuf->language = LANG_DEFAULT;
	#if defined(PRJ_PDVR)
	pbuf->HDR= HDR_STATUS_DEFAULT;
	#else
	pbuf->TV = TV_DEFAULT;
	#endif
	pbuf->usbMode= USB_DEFAULT;
	pbuf->batteryType = BATTERY_TYPE_DEFAULT;
	pbuf->powerOnLogo = POWERON_LOGO_DEFAULT;
	pbuf->videoFormat = VIDEO_FORMAT_DEFAULT;
	pbuf->mediaFormat = MEDIA_FORMAT_DEFAULT;
	#ifdef APP_WIFI
	pbuf->wifiConnectStatus = WIFI_CONNECTION_DEFAULT;
	#endif
	#if GPS_OPEN
	pbuf->TimezoneSTATUS = GPS_TIMEZONE_DEFAULT;
	pbuf->GPSSTATUS = GPS_STATUS_DEFAULT;
	gps_update_RtcTime_flag = 0;
	#endif
	pbuf->videoSize = UI_VIDEO_SIZE_FHD_30FPS ; // 1920x1080
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	pbuf->buzzerSet = BUZZER_SETTING_BOOT_ONOFF|BUZZER_SETTING_WARNING_ONOFF|BUZZER_SETTING_OTHER_ONOFF;/*All buzzer setting is on*/
	//pbuf->voiceSet = 1;/* 1 is on , 0 is off*/
	pbuf->ParkingModesetting = 0 ;
	pbuf->ParkingModereCordingTimeH = 0;
	pbuf->ParkingModereCordingTimeL = 30; // 30sec
	pbuf->ParkingModebatteryVoltageS = 112;// 11.2V
	pbuf->ParkingModegSensorLevel = 3;
	pbuf->ParkingModeparkingEvntToAVN = 0;
	pbuf->ParkingModebyPassTimeH = 0;
	pbuf->ParkingModebyPassTimeL = 30;
	pbuf->ParkingTrigger = ParkingModeTigger_NONE;
	pbuf->Gsensor = PANA_GSensor_Lv3;
	pbuf->GSensorComX = 0;
	pbuf->GSensorComY = 0 ;
	pbuf->GSensorComZ = 0;
#endif
	printf("_userSettingDefaultSet     %d\n" ,pbuf->videoSize );
}


UINT32
appUserSettingLoad(
	uiParamSetting_t *pbuf
)
{
	if(pbuf == NULL)
	{
        HOST_PROF_LOG_PRINT(LEVEL_ERROR, "userset: load user setting file error!!! : %s(%d)",\
                       __FUNCTION__, __LINE__);
		return FAIL;
	}
	if(ReadFile((UINT8 *)RESB_USER_SET, (UINT8*)(pbuf), sizeof(uiParamSetting_t))!= SUCCESS)
	{
		DBG_PRINT("[ERR] read file[%s] fail\n", RESB_USER_SET);
		/* set default */
		_userSettingDefaultSet(pbuf);
		return FAIL;
	}
	gStillCB.cap.CurScene = pUiSetting->capSetting.CurScene;
	gStillCB.lastcap = gStillCB.cap;
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	BuzzerActiveState = pbuf->buzzerSet;
	appInfoParkingSettings();
	
#endif
	return SUCCESS;
}


UINT32
appUserSettingSave(
	uiParamSetting_t *pbuf
)
{
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	static BOOL locker = FALSE;
	UINT32 ret = FAIL;
	uiParamSetting_t t;
	
	if (locker == TRUE)
		return ret;
	locker = TRUE;
#endif
	if(pbuf == NULL)
	{
        HOST_PROF_LOG_PRINT(LEVEL_ERROR, "userset: save user setting file error!!! : %s(%d)",\
                       __FUNCTION__, __LINE__);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		goto out;
#else
		return FAIL;
#endif
	}
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	if(ReadFile((UINT8 *)RESB_USER_SET, (UINT8*)(&t), sizeof(uiParamSetting_t)) == SUCCESS)
	{
		if (memcmp(&t, pbuf, sizeof(uiParamSetting_t)) == 0) {
			printf("%s: same as orignal content\n", RESB_USER_SET);
			ret = SUCCESS;
			goto out;
		}
	}

	JetPowerCanOff(0);
	printf("%s: update setting\n", RESB_USER_SET);
#endif
	pUiSetting->capSetting.CurViewMode = gStillCB.cap.CurViewMode;
	pUiSetting->capSetting.CurScene = gStillCB.cap.CurScene;

	if(WriteFile((UINT8 *)RESB_USER_SET, (UINT8*)(pbuf), sizeof(uiParamSetting_t))!=SUCCESS)
	{
		DBG_PRINT("[ERR] write file[%s] fail\n", RESB_USER_SET);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		goto out;
#else
		return FAIL;
#endif
	}
	appFastBootLensModeSet(pUiSetting->capSetting.CurViewMode);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	appUIParaSave();
	out:
	JetPowerCanOff(1);
	locker = FALSE;
	return ret;
#else
	//appFastBootLensBattSet(pUiSetting->batteryType,appLensBootBatteryLevelGet());
	return SUCCESS;
#endif
}

/*
void
appDcfUserSettingGet(
	fileNum_e *fnumRule,
	UINT16 *dirKey,
	UINT16 *fileKey
)
{
	*fnumRule = pUiSetting->fileNumber;
	*dirKey = pUiSetting->dcfDirKey;
	*fileKey = pUiSetting->dcfFileKey;
}
*/


void
appFactorySettingApply(
	void
)
{
    appStill_ResetAllParam();
    //reset slide show param
    appPbBackdoorEventCBF(PB_APP_EVENT_SETUP_RESET);
    appRtcDefaultSet();

	appUIParaReset();

}

void appUIParaReset(void)
{
	uiPara_t* puiPara = appUiParaGet();
	printf("%s ",__FUNCTION__);
	/*still_menu*/
	puiPara->ImageSize = 0;
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	puiPara->ImageQuality = UI_QUALITY_SUPERFINE;
#else
	puiPara->ImageQuality = UI_QUALITY_FINE;
#endif
	#if SP5K_SENSOR_DUAL_MODE
	#else
	puiPara->PhotoBurst = 0;
	#endif
	puiPara->StillStampMode = 0;
	/*video_menu*/
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	puiPara->VideoSize = pUiSetting->videoSize=0;
	puiPara->VideoQuality = UI_QUALITY_SUPERFINE;
	puiPara->Seamless = 1;
	puiPara->VideoStampMode = UI_DATESTAMP_DATETIME;/*Isaac change just for pana debug*/
#else
	puiPara->VideoSize = 0;
	puiPara->VideoQuality = UI_QUALITY_FINE;
	puiPara->Seamless = 0;
	puiPara->VideoStampMode = 0;
#endif
	#if SP5K_ICAT_EIS
    puiPara->VideoEISMode = UI_EIS_OFF;
	#endif
	#if SP5K_SENSOR_DUAL_MODE
	#else
	puiPara->WBMode = 0;
	#endif
	#if SP5K_MOTION_DETECT
	puiPara->MotionDetect = 0;
	#endif
	/*add*/
	puiPara->AEMode = UI_EXP_BIAS_ZERO;
    //reset setup param
    puiPara->SceneMode = pUiSetting->capSetting.CurViewMode = PASM_DEFAULT;
#if SP5K_SENSOR_DUAL_MODE
#else
    puiPara->PeriodicalCap = pUiSetting->periodicalCap.periodicalSet= PERIODICAL_CAPTURE_DEFAULT;
#endif
    puiPara->ShutterSound = pUiSetting->sound.shutter = SHUTTER_SOUND_DEFAULT;
    puiPara->StartupSound = pUiSetting->sound.powerOn = POWERON_SOUND_DEFAULT;
    puiPara->BeepSound = pUiSetting->sound.beep = BEEP_DEFAULT;
    puiPara->Volume = pUiSetting->sound.volume = SOUND_VOLUME_DEFAULT;
    puiPara->VoiceRec = pUiSetting->sound.VoiceRec = pUiSetting->voiceSet= 1;
    /*pUiSetting->pbfileType = PB_FILETYPE_DEFAULT;*/
    #if defined(HW_PDVR) 
     puiPara->G_Sensor= pUiSetting->GSENSOR= GSENSOR_STATUS_DEFAULT;
    #else
    puiPara->LightFreq = pUiSetting->lightFrequency= LIGHT_FREQ_DEFAULT;
    #endif
    puiPara->SleepTime = pUiSetting->powerSave = POWER_SAVE_DEFAULT;
    puiPara->powerOnRecord = pUiSetting->powerOnRecord = POWER_ON_RECORD_DEFAULT;
    puiPara->Language = pUiSetting->language = LANG_DEFAULT;
    appSetupMenu_SetCurLanguage(pUiSetting->language);
    #if defined(PRJ_PDVR) 
     puiPara->HDR= pUiSetting->HDR= HDR_STATUS_DEFAULT;
    #else
   puiPara->TVMode = pUiSetting->TV = TV_DEFAULT;
    #endif
    puiPara->USBMode = pUiSetting->usbMode= USB_DEFAULT;
    puiPara->batteryType = pUiSetting->batteryType = BATTERY_TYPE_DEFAULT;
    puiPara->VideoFormat = pUiSetting->videoFormat = VIDEO_FORMAT_DEFAULT;
    puiPara->MediaFormat = pUiSetting->mediaFormat = MEDIA_FORMAT_DEFAULT;
    pUiSetting->powerOnLogo = POWERON_LOGO_DEFAULT;
	#ifdef APP_WIFI
    pUiSetting->wifiConnectStatus = WIFI_CONNECTION_DEFAULT;
	#endif
	#if GPS_OPEN
    puiPara->TimeZones = pUiSetting->TimezoneSTATUS = GPS_TIMEZONE_DEFAULT;
    puiPara->GPS = pUiSetting->GPSSTATUS = GPS_STATUS_DEFAULT;
    gps_update_RtcTime_flag = 0;
	#endif
}

#if defined(PRJ_17DVR)||defined(PRJ_19DVR)

UINT8 appGsensorSettingGet(void)
{
	return 	pUiSetting->Gsensor;
	
}
#endif
