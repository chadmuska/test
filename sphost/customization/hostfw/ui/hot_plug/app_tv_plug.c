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
#include "app_com_api.h"
#include "gpio_custom.h"
#include "app_tvlcd.h"
#include "app_sound.h"
#include "sp5k_disp_api.h"

void
appTvPlugStateInit(
	void
)
{
	if ((IS_TV_IN && (appCurrDispModeGet() == pUiSetting->TV))
		|| (!IS_TV_IN && (appCurrDispModeGet() == APP_DISP_LCD)))
	{
		HOST_PROF_LOG_PRINT(LEVEL_WARN,"tvplug: cancle TV plug action(%d,%d,%d)",IS_TV_IN,pUiSetting->TV,appCurrDispModeGet());
		sp5kHostMsgSend(APP_UI_MSG_DUMMY, 0x00);
		return;
	}
	DBG_PRINT("tvplug: LCD <-> TV start\n");
	/*s Add by Aries 10/04/08*/
	#if SPCA5310
	sp5kDispAttrSet(sp5kDispActChnlGet(0), SP5K_DISP_IMG_ACTIVE, 0);
	sp5kDispAttrSet(sp5kDispActChnlGet(0), SP5K_DISP_OSD_ACTIVE, OSD_DISPLAY_SUPPORT);
	#endif	
	/*s Add by Aries 10/04/08*/
	if (IS_TV_IN)
	{
		LCD_BACKLIGHT_OFF;  // BL OFF here???
	#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		appLcdTvSwitch(pUiSetting->TV,1);
	#else
		appLcdTvSwitch(pUiSetting->TV,0);
	#endif
	}
	else
	{
		appLcdTvSwitch(APP_DISP_LCD,0);
	}
}


void
appTvPlugState(
	UINT32 msg,
	UINT32 param
)
{
	DBG_PRINT("tvplug: %s : [0x%x] [0x%x]\n",__FUNCTION__,msg,param);

	switch (msg) {
	case APP_STATE_MSG_INIT:
		RPrintf("appTvPlugState");
		appTvPlugStateInit();
		break;
	case APP_STATE_MSG_CLOSE:
		appStateCloseDone();
		break;
	case SP5K_MSG_DISP_POWER_ON_READY:
		DBG_PRINT("tvplug: $$ SP5K_MSG_DISP_POWER_ON_READY : %d\n",IS_TV_IN);
		appDispGfxInit();
		if ( (!IS_TV_IN) && (appCurrDispModeGet() == APP_DISP_LCD) ) {
			LCD_BACKLIGHT_ON;
		}
		appAudioDevOn();
		appStateChange(appPreviousStateGet(),STATE_PARAM_TV_PLUG);

		#if SPCA5310
		sp5kDispAttrSet(sp5kDispActChnlGet(0), SP5K_DISP_OSD_COLOR_KEY, SP5K_DISP_COLOR_KEY_RGB565, 0x000000, 0);
		sp5kDispAttrSet(sp5kDispActChnlGet(0), SP5K_DISP_IMG_ACTIVE, 1);
		sp5kDispAttrSet(sp5kDispActChnlGet(0), SP5K_DISP_OSD_ACTIVE, OSD_DISPLAY_SUPPORT);
		#endif
		break;
	case APP_UI_MSG_DUMMY:
		if( IS_CARD_PLUG_GROUP(appPreviousStateGet() ) )
			appStateChange(APP_STATE_VIDEO_PREVIEW,STATE_PARAM_TV_PLUG);
		else
			appStateChange(appPreviousStateGet(),STATE_PARAM_TV_PLUG);
		break;
	default:
		break;
	}
}

