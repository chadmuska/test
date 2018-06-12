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
#if (KIT_WITH_HDMI)
#define HOST_DBG 0
#include "app_com_def.h"
#include "app_com_api.h"
#include "gpio_custom.h"

#include "api/sp5k_rsvblk_api.h"
#include "api/sp5k_media_api.h"
#include "api/sp5k_aud_api.h"
#include "api/sp5k_cec_api.h"
#include "api/sp5k_disp_api.h"
#include "middleware/proflog.h"
#if SPCA5310
#include "api/sp5k_pip_api.h"
#endif

#include "app_tvlcd.h"
#include "app_sound.h"
#include "app_aaa.h"
#include "app_osd_api.h"
#include "customization/app_init.h"
#include "customization/dev_init.h"
#include "app_osdfont.h"
#include "app_setup_lib.h"
#include "app_video.h"
#include "app_playback_def.h"
#include "app_dev_plug.h"

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
extern UINT32 halPllClockSwitch(UINT32 cpuKHz,UINT32 dramKHz,UINT16 syncMode,UINT8 *ptmpBuf);
extern void appPwerOnInfoGet(BOOL *pbPowerOn, BOOL *mutePowerOn, BOOL *testModePowerOn);
extern void appCecStop(void);
extern void appCecCtrlLogicalAlloc(void);
void appHDMISwitch2LCD(void);
void appHDMIshowOSD(void);
void appHDMIPlugStateInit(void);
void appHDMIPlugState(UINT32 msg,UINT32 param);

/**************************************************************************
 *                         G L O B A L    D A T A                        *
 **************************************************************************/
UINT32 previousStateGet;
UINT8 resolution=0xFF;

#define WRITE8(a,b)   *(volatile UINT8*)(a) = (b)
#define READ8(a)      (*((volatile UINT8*)(a)))

/**************************************************************************
 *                             F U N C T I O N                         *
 **************************************************************************/

void
appOsdHDMIResPreload(
	void
)
{
	sp5kResourceFileLoad(SP5K_RES_OSD_ICON, (char*)ID_FONT_TK_BIGICON);
	appSetupMenu_SetCurLanguage(pUiSetting->language);
	sp5kResourceFileLoad(SP5K_RES_OSD_ICON_2,(char*)RES_ICON_MUMODE_HDMI);
	sp5kResourceFileLoad(SP5K_RES_OSD_ICON_3,(char*)RES_ICON_MUOK_HDMI);
	sp5kResourceFileLoad(SP5K_RES_OSD_ICON_4,(char*)RES_ICON_MUTAB_HDMI);
	sp5kResourceFileLoad(SP5K_RES_OSD_ICON_5,(char*)RES_ICON_MUBAR_HDMI);
}

void appHDMISwitch2LCD(void)
{
#if (KIT_WITH_HDMI)
	extern void _dispLcdInit(void);

	sp5kAudActDevSet(SP5K_AUD_DEV_PLAY, SP5K_AUD_DEV_PRIMARY);
	sp5kDispAttrSet(sp5kDispActChnlGet(0), SP5K_DISP_EDGE_ENHANCE, 1, 8, 1, 2);
	sp5kDispAttrSet(sp5kDispActChnlGet(0), SP5K_DISP_IMG_WINDOW, 0, 0, 320, 240);
	sp5kDispAttrSet(sp5kDispActChnlGet(0), SP5K_DISP_OSD_WINDOW,0, 0, 320, 240);
	sp5kModeCfgSet(SP5K_MODE_CFG_STANDBY_FRAME_SIZE, 320, 240);
	sp5kModeCfgSet(SP5K_MODE_CFG_STNDBY_FRAME_BUF_SIZE, 320, 240);
appDispGfxInit();
	//LCD_PWR_ON ;
	//LCD_BACKLIGHT_ON;
#endif
}

void appSwitchImage2HDMI(UINT8 resolution)
{
#if (KIT_WITH_HDMI)
	sp5kAudActDevSet(SP5K_AUD_DEV_PLAY, SP5K_AUD_DEV_SECONDARY);
#else
#endif  /* KIT_WITH_HDMI Modify by Shang 11/11/16 */
}

void appHDMIshowOSD()
{
#if (KIT_WITH_HDMI)
	UINT32 dispWidth,dispHeight;

	sp5kGfxPageClear(SP5K_GFX_PAGE_OSD_0,0);
	sp5kDispAttrGet(SP5K_DISP_CHNL_1, SP5K_DISP_DEV_DIMENSION, (SINT32 *)&dispWidth, (SINT32 *)&dispHeight);

	#if SP5K_SENSOR_OV2715 || SP5K_SENSOR_AR0330CS || SP5K_SENSOR_AR0330CS_PLCC  /* for 16:9 */
	sp5kDispAttrSet(SP5K_DISP_CHNL_1, SP5K_DISP_IMG_WINDOW, 0, 0, dispWidth, dispHeight);
	sp5kDispAttrSet(SP5K_DISP_CHNL_1, SP5K_DISP_OSD_WINDOW,0, 0, dispWidth, dispHeight);
	#else  /* For background image size 4:3 */
	sp5kDispAttrSet(SP5K_DISP_CHNL_1, SP5K_DISP_IMG_WINDOW, 0, 0, dispWidth, dispHeight);
	sp5kDispAttrSet(SP5K_DISP_CHNL_1, SP5K_DISP_OSD_WINDOW,0, 0, dispWidth, dispHeight);
	/*sp5kDispAttrSet(SP5K_DISP_CHNL_1, SP5K_DISP_IMG_WINDOW, 40, 30, dispWidth-40, dispHeight-30);
	sp5kDispAttrSet(SP5K_DISP_CHNL_1, SP5K_DISP_OSD_WINDOW, 240, 0, 1440, 1040);*/
	#endif
	sp5kModeCfgSet(SP5K_MODE_CFG_STANDBY_FRAME_SIZE, 1920, 1080);	/* Sync with SP5K_MODE_CFG_STILL_PV_SIZE */
	sp5kModeCfgSet(SP5K_MODE_CFG_STNDBY_FRAME_BUF_SIZE, 1920, 1080);

	sp5kDispAttrSet(SP5K_DISP_CHNL_1, SP5K_DISP_BG_COLOR, 0, 0, 0);

	sp5kGfxInitCfgSet(SP5K_GFX_OSD_PAGE_TOT, 1, 0, 0, 0);
	sp5kGfxInitCfgSet(SP5K_GFX_OSD_FMT, SP5K_GFX_FMT_IDX8, 0, 0, 0);
	sp5kGfxInitCfgSet(SP5K_GFX_OSD_FRAME_SIZE, 640, 360, 0, 0);
	sp5kGfxInitCfgSet(SP5K_GFX_OSD_WINDOW, 0, 0, 640, 360);

	sp5kGfxInitCfgSet(SP5K_GFX_PIP_PAGE_TOT, 1, 0, 0, 0);
	sp5kGfxInitCfgSet(SP5K_GFX_PIP_FMT, SP5K_GFX_FMT_RGB565, 0, 0, 0);
	sp5kGfxInitCfgSet(SP5K_GFX_PIP_FRAME_SIZE,640, 360, 0, 0);  //for audio background
	sp5kGfxInitCfgSet(SP5K_GFX_PIP_WINDOW, 0, 0, 640, 360);

	sp5kGfxInitCfgSet(SP5K_GFX_PF_PAGE_TOT, 1, 0, 0, 0);
	sp5kGfxInitCfgSet(SP5K_GFX_PF_FMT, SP5K_GFX_FMT_RGB565, 0, 0, 0);
	sp5kGfxInitCfgSet(SP5K_GFX_PF_FRAME_SIZE,640, 360, 0, 0);
	sp5kGfxInitCfgSet(SP5K_GFX_PF_WINDOW, 0, 0, 640, 360);

	sp5kGfxInitCfgSet(SP5K_GFX_AB_FRAME_SIZE, 640, 360, 0, 0);

	sp5kGfxInit();
#endif
}

void
appHDMIPlugStateInit(
	void
)
{
printf("%s_ IS_HDMI_IN=%d\n", __FUNCTION__, IS_HDMI_IN);
	#if SPCA5310
    	sp5kDispAttrSet(sp5kDispActChnlGet(0), SP5K_DISP_IMG_ACTIVE, 0);
    	sp5kDispAttrSet(sp5kDispActChnlGet(0), SP5K_DISP_OSD_ACTIVE, 0);
	#endif

	if (appHdmiStateGet() == HDMI_IN_PROC) {
		sp5kModeSet(SP5K_MODE_STANDBY);
		sp5kModeWait(SP5K_MODE_STANDBY);
		appLcdTvSwitch(APP_DISP_HDMI,TRUE);  /* second display for dual screen */
		/* switch display for dual screen & single screen */
        if (appHdmiStateGet()==HDMI_OUT_PROC)
        {
            printf("\n\n SHEN:HDMI is out!!! HDMI=%d State=0x%X\n", IS_HDMI_IN, appHdmiStateGet());
            return;
        }
		sp5kDispAttrSet(sp5kDispActChnlGet(0), SP5K_DISP_EDGE_ENHANCE, 0, 0, 0, 0);
		sp5kAudActDevSet(SP5K_AUD_DEV_PLAY, SP5K_AUD_DEV_SECONDARY);
		appOsdHDMIResPreload();
		appHDMIshowOSD();  /* graphic */
		sp5kModeCfgSet(SP5K_MODE_CFG_STANDBY_DUP, 0);
		sp5kModeCfgSet(SP5K_MODE_CFG_STANDBY_FORCE, 1);  /*1: enter standby mode even already in, 0: enter standby mode if not in*/

		#if DUAL_DISP_ENABLE
		    LCD_BACKLIGHT_ON;
		#else
		    LCD_BACKLIGHT_OFF;  // BL OFF here???
		#endif

		#if IS_CEC_ON
		    appCecCtrlLogicalAlloc();
		#endif
        appHDMIColorMatrix();
	}
	else {
		if ( STATE_GROUP(appPreviousStateGet()) == APP_STATE_GROUP_PLAYBACK ) {
			AppPbDestroyshowJPGInPIP();
		}

        #if DUAL_DISP_ENABLE
    		LCD_BACKLIGHT_OFF;
		#endif

		appCecStop();
		appOsdResPreload();
		appHDMISwitch2LCD();
		appLcdTvSwitch(APP_DISP_LCD,0);
		sp5kModeCfgSet(SP5K_MODE_CFG_STANDBY_DUP, 0);
		sp5kModeCfgSet(SP5K_MODE_CFG_STANDBY_FORCE, 1);
		resolution = 0xff;
        appHdmiStatusSet(HDMI_OUT);
    }
}

void
appHDMIPlugState(
	UINT32 msg,
	UINT32 param
)
{
	BOOL pbPowerOn, mutePowerOn, testModePowerOn;
	DBG_PRINT("hdmi: %s : [0x%x] [0x%x]\n",__FUNCTION__,msg,param);

	switch (msg) {
	case APP_STATE_MSG_INIT:
		RPrintf("appHDMIPlugState init\n");
		appHDMIPlugStateInit();
		break;
	case APP_STATE_MSG_CLOSE:
		appStateCloseDone();
		break;
	case SP5K_MSG_DISP_POWER_ON_READY:
        printf("SP5K_MSG_DISP_POWER_ON_READY DispMode=%d HDMI_PLUG_STATE=0x%X\n", appCurrDispModeGet(), appHdmiStateGet());
        #if 0
			if ( (!IS_HDMI_IN) && (appCurrDispModeGet() == APP_DISP_HDMI) ) {
				LCD_BACKLIGHT_ON;
			}
        #endif

		DBG_PRINT("hdmi: DISP_POWER_ON_READY 0x%X\n",appHdmiStateGet());
        {
			DBG_PRINT("hdmi: appHDMIPlugState: Return state is %x after running HDMI plug\n", previousStateGet);
			appPwerOnInfoGet(&pbPowerOn, &mutePowerOn, &testModePowerOn);
			if ( testModePowerOn == TRUE ) {
				appStateChange(APP_STATE_CALIBRATION, STATE_PARAM_HDMI_PLUG);
			}
            else if ( STATE_GROUP(appPreviousStateGet()) == APP_STATE_GROUP_PLAYBACK ) {
				appStateChange(APP_STATE_PB_MAIN, STATE_PARAM_HDMI_PLUG);
			}
            else{
				appStateChange(previousStateGet, STATE_PARAM_HDMI_PLUG);
			}
		}

		sp5kDispAttrSet(sp5kDispActChnlGet(0), SP5K_DISP_IMG_ACTIVE, 1);
		sp5kDispAttrSet(sp5kDispActChnlGet(0), SP5K_DISP_OSD_ACTIVE, 1);

        #if DUAL_DISP_ENABLE
        if (IS_HDMI_IN) {
    		sp5kDispAttrSet(sp5kDispActChnlGet(1), SP5K_DISP_OSD_ACTIVE, 1);
        }
        #endif
        appHdmiStatusSet(IS_HDMI_IN);
		break;

	case SP5K_MSG_DISP_POWER_OFF_READY:
        printf("SP5K_MSG_DISP_POWER_OFF_READY DispMode=%d HDMI_PLUG_STATE=%d\n", appCurrDispModeGet(), appHdmiStateGet());
        break;
	case APP_UI_MSG_DUMMY:
		appStateChange(appPreviousStateGet(),STATE_PARAM_HDMI_PLUG);
		break;
	default:
		break;
	}
}
#endif

