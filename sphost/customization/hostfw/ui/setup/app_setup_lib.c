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
#include "gpio_custom.h"

#include "sp5k_capture_api.h"

#include "app_menu.h"
#include "app_user_setting.h"
#include "app_disk.h"
#include "app_dcf.h"
#include "app_battery.h"
#include "app_error_msg.h"
#include "app_errorosd.h"
#include "app_still.h"
#include "app_rtc.h"
#include "app_sound.h"
#include "app_setup_lib.h"
#include "app_osd.h"
#include "app_osd_api.h"
#include "app_osdfont.h"
#include "app_playback_def.h"
#include "app_view_param_def.h"
#include "app_view_param.h"
#include "app_ui_para.h"

 /**************************************************************************
  * 						  C O N S T A N T S 						   *
  **************************************************************************/
extern void appPeriodicalCaptureTimeSet(void);
extern view_param_t *pViewParam;

void appSoundControl(beepSound_e soundmode);

struct {
	UINT8 *fntname, *strname;
} LangTbl[]=
{
#if defined(PRJ_PDVR)
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_ENG },
	{ (UINT8 *)ID_FONT_JAPAN28x28,(UINT8 *)ID_STRTK_TBL_JAPAN},
	{ (UINT8 *)ID_FONT_SCH28X28, (UINT8 *)ID_STRTK_TBL_SCH },
	{ (UINT8 *)ID_FONT_TCH28X28, (UINT8 *)ID_STRTK_TBL_TCH },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_SPANISH },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_CZECH },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_DUTCH },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_FRENCH },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_GREEK },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_HUNGARIAN },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_ITALIAN },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_INDONESIAN },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_MALAY },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_POLISH },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_PORTUGUESE },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_RUSSIAN },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_ROMANIAN },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_GERMAN },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_SWEDISH },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_TURKISH },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_CROATION },
	{ (UINT8 *)ID_FONT_ARABIC28X28, (UINT8 *)ID_STRTK_TBL_ARABIC },
	{ (UINT8 *)ID_FONT_THAI28X28, (UINT8 *)ID_STRTK_TBL_THAI },
	{ (UINT8 *)ID_FONT_KOREAN28X28,(UINT8 *)ID_STRTK_TBL_KOREAN},
#else
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_ENG },
	{ (UINT8 *)ID_FONT_SCH28X28, (UINT8 *)ID_STRTK_TBL_SCH },
	{ (UINT8 *)ID_FONT_TCH28X28, (UINT8 *)ID_STRTK_TBL_TCH },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_SPANISH },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_CZECH },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_DUTCH },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_FRENCH },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_GREEK },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_HUNGARIAN },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_ITALIAN },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_INDONESIAN },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_MALAY },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_POLISH },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_PORTUGUESE },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_RUSSIAN },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_ROMANIAN },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_GERMAN },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_SWEDISH },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_TURKISH },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_CROATION },
	{ (UINT8 *)ID_FONT_ARABIC28X28, (UINT8 *)ID_STRTK_TBL_ARABIC },
	{ (UINT8 *)ID_FONT_THAI28X28, (UINT8 *)ID_STRTK_TBL_THAI },
	{ (UINT8 *)ID_FONT_JAPAN28x28,(UINT8 *)ID_STRTK_TBL_JAPAN},
	{ (UINT8 *)ID_FONT_KOREAN28X28,(UINT8 *)ID_STRTK_TBL_KOREAN},
#endif
};

struct {
	UINT8 *fntname, *strname;
} LangHDMITbl[]=
{
	{ (UINT8 *)ID_FONT_ENG42, (UINT8 *)ID_STRTK_TBL_BENG },
	{ (UINT8 *)ID_FONT_SCH42, (UINT8 *)ID_STRTK_TBL_BSCH },
	{ (UINT8 *)ID_FONT_TCH42, (UINT8 *)ID_STRTK_TBL_BTCH },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_SPANISH },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_CZECH },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_DUTCH },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_FRENCH },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_GREEK },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_HUNGARIAN },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_ITALIAN },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_INDONESIAN },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_MALAY },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_POLISH },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_PORTUGUESE },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_RUSSIAN },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_ROMANIAN },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_GERMAN },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_SWEDISH },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_TURKISH },
	{ (UINT8 *)ID_FONT_ENG22X28, (UINT8 *)ID_STRTK_TBL_CROATION },
	{ (UINT8 *)ID_FONT_ARABIC28X28, (UINT8 *)ID_STRTK_TBL_ARABIC },
	{ (UINT8 *)ID_FONT_THAI28X28, (UINT8 *)ID_STRTK_TBL_THAI },
	{ (UINT8 *)ID_FONT_JAPAN28x28,(UINT8 *)ID_STRTK_TBL_JAPAN},
	{ (UINT8 *)ID_FONT_KOREAN28X28,(UINT8 *)ID_STRTK_TBL_KOREAN},
};

#define RTC_VALUE_CHECK(var, min, max)      { \
				(var) = ((var) < (min)) ? (max) : (((var) > (max)) ? (min) : (var)); }

#define VAR_LIMIT(var, min, max)  { \
				(var) = ((var) < (min)) ? (min) : (((var) > (max)) ? (max) : (var)); }

 /**************************************************************************
  * 						G L O B A L    D A T A						   *
  **************************************************************************/
static UINT32 month[2][12] =
{
	{ 31,28,31,30,31,30,31,31,30,31,30,31 },
	{ 31,29,31,30,31,30,31,31,30,31,30,31 }
};

appRTC_t rtcTime;

 /**************************************************************************
  * 			  F U N C T I O N	 D E C L A R A T I O N S			   *
  **************************************************************************/
BOOL
_setupMenuFunctionFormat(
	void
)
{
	UINT32 ret;
	//UINT32 time_t0 = 0, time_t1 = 0;

	appBtnDisable(BTN_ALL);

	CLEAR_OSD_SRC;
	CLEAR_PIP_SRC;
	PB_BLACK_BKGND;
	if (IS_CARD_EXIST && IS_CARD_LOCK)
	{
		HOST_PROF_LOG_ADD(LEVEL_WARN, "set lib: Card Lock, Fail to format");
		appErrMsgWithSound(ERR_MSG_CARD_LOCKED,ERR_MSG_WITH_WARNING_SOUND,ERR_MSG_SHOW_2SEC);
		ret= FALSE;
	}
	else
	{
		UINT32 time_t0=0;
		UINT32 time_t1=0;
		LED_FLICKER_FOMATE;
		appOsdMessageBoxShow(TRUE,STRTK_BUSY_PLS_WAIT);
		time_t0 = appMsTimeGet();
		DBG_PRINT("set lib: Format disk%d start ..\n",appActiveDiskGet());
		ret = appDiskFormat(appActiveDiskGet());
		DBG_PRINT("set lib: Format disk%d done ..\n",appActiveDiskGet());

		time_t1 = appMsTimeGet() - time_t0;
		if (time_t1 < 500)
		{
			sp5kTimeDelay( SP5K_TIME_DELAY_1MS , (500 - time_t1));
		}
		appOsdMessageBoxShow(FALSE,STRTK_BUSY_PLS_WAIT);
		//appPbShowBG();
		if (ret == SUCCESS)
		{
			appErrMsgWithSound(ERR_MSG_COMPLETED,ERR_MSG_ONLY,ERR_MSG_SHOW_1SEC);
		}
		else
		{
			appErrMsgWithSound(ERR_MSG_FORMAT_ERRPR,ERR_MSG_WITH_WARNING_SOUND,ERR_MSG_SHOW_1SEC);
		}
		LED_NORMAL_ON;
		ret = TRUE;
	}

	appBtnEnable(BTN_ALL);

	return ret;

}

void
_setupMenuFunctionResetAll(
	void
)
{
	appBtnDisable(BTN_ALL);

	appFactorySettingApply();
	CLEAR_OSD_SRC;
	CLEAR_PIP_SRC;
	appErrMsgWithSound(ERR_MSG_COMPLETED,ERR_MSG_ONLY,ERR_MSG_SHOW_2SEC);
	appBtnEnable(BTN_ALL);
}

void
_setupMenuKeyPressDo_Execute(
	appMenuSetupMenu_e menuCurTab,
	UINT32 subMenuItem
)
{
	DBG_PRINT("set lib: %s (%d,%d)\n",__FUNCTION__, menuCurTab,subMenuItem);
	switch(menuCurTab)
	{
	case SETUPMENU_SETUP_SOUNDS:
		if(pUiSetting->sound.shutter != SHUTTER_SOUND_OFF)
		{
			//sp5kStillCapCfgSet(SP5K_CAPTURE_SHUTTER_SND_NAME, (UINT32)RES_WAV_SHUTTER);
		}
		else
		{
			sp5kStillCapCfgSet(SP5K_CAPTURE_SHUTTER_SND_NAME, (UINT32)NULL);
		}
		break;
	case SETUPMENU_SETUP_LIGHT_FREQ:
		pUiSetting->lightFrequency = subMenuItem;
		applightFrequencySetting(pUiSetting->lightFrequency); // to be finished ,Leo
		DBG_PRINT("set lib: SETUPMENU_SETUP_LIGHT_FREQ = %d\n", pUiSetting->lightFrequency);
		break;
	case SETUPMENU_SETUP_POWER_SAVE:
		pUiSetting->powerSave = subMenuItem;
		DBG_PRINT("set lib: SETUPMENU_SETUP_POWER_SAVE = %d\n", pUiSetting->powerSave);
		break;
	case SETUPMENU_SETUP_POWER_ON_RECORD:
		pUiSetting->powerOnRecord = subMenuItem;
		DBG_PRINT("set lib: SETUPMENU_SETUP_POWER_ON_RECORD = %d\n", pUiSetting->powerOnRecord);
		break;
	case SETUPMENU_SETUP_PERIODICAL_CAPTURE:
		pUiSetting->periodicalCap.periodicalSet = subMenuItem;
		DBG_PRINT("set lib: SETUPMENU_SETUP_PERIODICAL_CAPTURE = %d\n", pUiSetting->periodicalCap.periodicalSet);
		appPeriodicalCaptureTimeSet();
		break;
	case SETUPMENU_SETUP_DATE_TIME:
		appRtcSet(&rtcTime);
		DBG_PRINT("set lib: SETUPMENU_SETUP_DATE_TIME = %d\n", pUiSetting->dateTime);
		break;
	case SETUPMENU_SETUP_LANGUAGE:
		pUiSetting->language = subMenuItem;
		appSetupMenu_SetCurLanguage(pUiSetting->language);
		DBG_PRINT("set lib: SETUPMENU_SETUP_LANGUAGE = %d\n", pUiSetting->language);
		break;
	case SETUPMENU_SETUP_TV_OUT:
		pUiSetting->TV = subMenuItem;
		DBG_PRINT("set lib: SETUPMENU_SETUP_TV_OUT = %d\n", pUiSetting->TV );
		break;
	case SETUPMENU_SETUP_USB:
		pUiSetting->usbMode = subMenuItem;
		DBG_PRINT("set lib: SETUPMENU_SETUP_USB = %d\n", pUiSetting->usbMode);
		break;
	case SETUPMENU_SETUP_FORMAT:
		DBG_PRINT("set lib: SETUPMENU_SETUP_FORMAT = %d\n", subMenuItem);
		if (subMenuItem == FORMAT_YES) {
			if (_setupMenuFunctionFormat()) {
				appPbBackdoorEventCBF(PB_APP_EVENT_DISK_FORMAT);
				#if !SP5K_PVQV_DISABLE
				appViewOsd_PvQVInfoFree();
				#endif
			}
		}
		break;
	case SETUPMENU_SETUP_RESET_ALL:
		DBG_PRINT("set lib: SETUPMENU_SETUP_RESET_ALL = %d\n", subMenuItem);
		if (subMenuItem == RESET_YES) {
			_setupMenuFunctionResetAll();
		}
		break;
	default :
		break;
	}

	appUserSettingSave(pUiSetting);
}

void
_setupMenuKeyPressDo_Select(
	appMenuSetupMenu_e menuCurTab,
	UINT32 subMenuItem
)
{
	;
}

void
_setupMenuKeyPressDo_Return(
	appMenuSetupMenu_e menuCurTab,
	UINT32 subMenuItem
)
{
	;
}
/*-------------------------------------------------------------------------
 *  Function Name : appSetupMenuFunctionSetCB
 *------------------------------------------------------------------------*/
void
appSetupMenuFunctionSetCB(
	appMenuAction_e action,
	appMenuInfo_t *pmenuCurTab,
	appSubMenuInfo_t *psubMenuItem
)
{
	DBG_PRINT("set lib: %s(%d)\n",__FUNCTION__,action);
	DBG_PRINT("set lib: pmenuCurTab->curTab  = %d\n",pmenuCurTab->curTab);
	DBG_PRINT("set lib: pmenuCurTab->tab[%d] = %d\n",pmenuCurTab->curTab,pmenuCurTab->tab[pmenuCurTab->curTab]);
	DBG_PRINT("set lib: psubMenuItem->curItem  = %d\n",psubMenuItem->curItem);

	if (action == MENU_ACTION_SET)
	{
		_setupMenuKeyPressDo_Execute(pmenuCurTab->tab[pmenuCurTab->curTab-1],psubMenuItem->curItem);
		_setupMenuKeyPressDo_Return(pmenuCurTab->tab[pmenuCurTab->curTab],psubMenuItem->curItem);
	}
	else if (action == MENU_ACTION_RETURN)
	{
		_setupMenuKeyPressDo_Return(pmenuCurTab->tab[pmenuCurTab->curTab],psubMenuItem->curItem);
	}
	else
	{
		_setupMenuKeyPressDo_Select(pmenuCurTab->tab[pmenuCurTab->curTab],psubMenuItem->curItem);
	}
}

/*-------------------------------------------------------------------------
*	File Name : appSetupMenu_RTC_Get
*------------------------------------------------------------------------*/
appRTC_t *
appSetupMenu_RTC_Get(
	void
)
{
	appRtcGet(&rtcTime);
	return (&rtcTime);
}

/*-------------------------------------------------------------------------
*	File Name : appSetupMenu_RTC_Adjust
*------------------------------------------------------------------------*/
void
appSetupMenu_RTC_Adjust(
	set_RTC_Item_e rtcItem,
	UINT32 dir
)
{
	DBG_PRINT("set lib: %s (%d,%d)\n",__FUNCTION__,rtcItem,dir);
	switch(rtcItem)
	{
	case SET_RTC_YEAR:
		if (dir)
		{
			rtcTime.year--;
		}
		else
		{
			rtcTime.year++;
			if (rtcTime.year < RTC_YEAR_MIN) {
				rtcTime.year = RTC_YEAR_MIN;
			}
		}
		RTC_VALUE_CHECK(rtcTime.year,RTC_YEAR_MIN,RTC_YEAR_MAX);
		VAR_LIMIT( rtcTime.day, 1, month[ appLeapYearCheck(rtcTime.year) ][ rtcTime.mon -1 ] );
		break;
	case SET_RTC_MONTH:
		if (dir)
		{
			rtcTime.mon--;
		}
		else
		{
			rtcTime.mon++;
		}
		RTC_VALUE_CHECK(rtcTime.mon,1,12);
		VAR_LIMIT( rtcTime.day, 1, month[ appLeapYearCheck(rtcTime.year) ][ rtcTime.mon -1 ] );
		break;
	case SET_RTC_DAY:
		if (dir)
		{
			rtcTime.day--;
		}
		else
		{
			rtcTime.day++;
		}
		RTC_VALUE_CHECK(rtcTime.day,1,month[ appLeapYearCheck(rtcTime.year) ][ rtcTime.mon -1 ] );
		VAR_LIMIT( rtcTime.day, 1, month[ appLeapYearCheck(rtcTime.year) ][ rtcTime.mon -1 ] );
		break;
	case SET_RTC_HOUR:
		if (dir)
		{
			if (rtcTime.hour == 0)
			{
				rtcTime.hour = 23;
			}
			else
			{
				rtcTime.hour--;
			}
		}
		else
		{
			if (rtcTime.hour == 23)
			{
				rtcTime.hour = 0;
			}
			else
			{
				rtcTime.hour++;
			}
		}
		break;
	case SET_RTC_MINUTE:
		if (dir)
		{
			if (rtcTime.min == 0)
			{
				rtcTime.min = 59;
			}
			else
			{
				rtcTime.min--;
			}

		}
		else
		{
			if (rtcTime.min == 59)
			{
				rtcTime.min = 0;
			}
			else
			{
				rtcTime.min++;
			}
		}
		break;
	case SET_DATE_STYLE:
		if (dir)
		{
			pUiSetting->dateTime++;
			if (pUiSetting->dateTime == DATE_TIME_MAX)
			{
				pUiSetting->dateTime = DATE_TIME_YYMMDD;
			}
		}
		else
		{
			if (pUiSetting->dateTime == DATE_TIME_YYMMDD)
			{
				pUiSetting->dateTime = DATE_TIME_MAX-1;
			}
			else
			{
				pUiSetting->dateTime--;
			}
		}
		break;
	default:
		break;
	}
}
/*-------------------------------------------------------------------------
*	File Name : appSetupMenu_Sounn_Adjust
*------------------------------------------------------------------------*/
void
appSetupMenu_Sound_Adjust(
	soundsMode_e soundItem,
	UINT32 dir
)
{
	/*UINT8 *powerOnSound[POWERON_SOUND_MAX] =
	{
		NULL,
		(UINT8 *)RES_WAV_POWER_ON1,
	};*/

	uiPara_t* puiPara = appUiParaGet();

	switch(soundItem)
	{
	case SOUNDS_SHTTUER:
		 if (dir)
		 {
			 puiPara->ShutterSound++;
			if ( puiPara->ShutterSound == SHUTTER_SOUND_MAX)
			{
				puiPara->ShutterSound = SHUTTER_SOUND_OFF;
			}
		}
		else
		{
			if (puiPara->ShutterSound == SHUTTER_SOUND_OFF)
			{
				puiPara->ShutterSound = SHUTTER_SOUND_MAX -1;
			}
			else
			{
				puiPara->ShutterSound--;
			}
		}

		if(puiPara->ShutterSound != SHUTTER_SOUND_OFF)
		{
			if(appSoundLoad(RES_WAV_SHUTTER) == SUCCESS)
			{
				appSoundPlay();
			}
		}
		break;
	case SOUNDS_STARTUP:
		 if (dir)
		 {
			 puiPara->StartupSound++;
			if ( puiPara->StartupSound == POWERON_SOUND_MAX)
			{
				puiPara->StartupSound = POWERON_SOUND_NONE;
			}
		}
		else
		{
			if (puiPara->StartupSound == POWERON_SOUND_NONE)
			{
				puiPara->StartupSound = POWERON_SOUND_MAX -1;
			}
			else
			{
				puiPara->StartupSound--;
			}
		}
		if(puiPara->StartupSound != POWERON_SOUND_NONE)
		{
			UINT8 *powerOnSound[POWERON_SOUND_MAX] =
			{
				NULL,
				(UINT8 *)RES_WAV_POWER_ON1,
				
			};
			
			if(appSoundLoad((UINT8*)appRootGet((char *)powerOnSound[puiPara->StartupSound])) == SUCCESS)
			{
				appSoundPlay();
			}
		}
		break;
	case SOUNDS_BEEP:
		 if (dir)
		 {
			 puiPara->BeepSound++;
			if ( puiPara->BeepSound == BEEP_MAX)
			{
				puiPara->BeepSound = BEEP_OFF;
			}
		}
		else
		{
			if (puiPara->BeepSound == BEEP_OFF)
			{
				puiPara->BeepSound = BEEP_MAX -1;
			}
			else
			{
				puiPara->BeepSound--;
			}
		}
		break;
	case SOUNDS_VOULUME:
		 if (dir)
		 {
			puiPara->Volume++;
			if ( puiPara->Volume == SOUND_VOLUME_MAX)
			{
				puiPara->Volume = SOUND_VOLUME_OFF;
			}
		}
		else
		{
			if (puiPara->Volume == SOUND_VOLUME_OFF)
			{
				puiPara->Volume = SOUND_VOLUME_MAX -1;
			}
			else
			{
				puiPara->Volume--;
			}
		}
		appSundVolumeSet(puiPara->Volume);
		appPreloadSoundPlay(SOUND_ID_BEEP);
		break;
	default:
		break;
	}
}
/*-------------------------------------------------------------------------
*	File Name : appSetupMenu_SetCurLanguage
*------------------------------------------------------------------------*/
void appSetupMenu_SetCurLanguage(UINT8 nCurLanguage)
{
	if (nCurLanguage>=LANG_MAX)
	{
		return;
	}

	#if (KIT_WITH_HDMI)
	if(IS_HDMI_IN && (appActiveStateGet() != APP_STATE_POWER_ON)){
		appOsd_FontTextFontSet(appRootGet((char *)LangHDMITbl[nCurLanguage].fntname));
		appOsd_FontTextStringSet(appRootGet((char *)LangHDMITbl[nCurLanguage].strname));
	}else
	#endif
	{
		appOsd_FontTextFontSet(appRootGet((char *)LangTbl[nCurLanguage].fntname));
		appOsd_FontTextStringSet(appRootGet((char *)LangTbl[nCurLanguage].strname));
	}
}

/*-------------------------------------------------------------------------
*	File Name : applightFrequencySetting
*------------------------------------------------------------------------*/
void applightFrequencySetting(lightFreq_e lightfreq)
{
	switch(lightfreq)
	{
	case LIGHT_FREQ_50HZ:
		appStill_SetExpFlickerMode(0);
		break;
	case LIGHT_FREQ_60HZ:
		appStill_SetExpFlickerMode(1);
		break;
	case LIGHT_FREQ_AUTO:
		sp5kModeCfgSet(SP5K_MODE_CFG_BAND_DETECT_ENABLE, 1);
		return;
	default:
		break;
	}
	appAePvReset();
}
