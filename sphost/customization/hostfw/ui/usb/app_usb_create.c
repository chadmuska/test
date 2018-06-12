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

#include "sp5k_usb_api.h"
#include "sp5k_disk_api.h"
#include "sp5k_capture_api.h"
#include "sp5k_media_api.h"

#include "app_dev_plug.h"
#include "app_osdfont.h"
#include "app_errorosd.h"
#include "app_osd_api.h"
#include "app_jpg_draw.h"
#include "app_sensor.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

#define JPG_RES_START_X  	0
#define JPG_RES_START_Y  	0
#define JPG_RES_START_W  	320
#define JPG_RES_START_H  	240

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

static UINT32 preState = APP_STATE_NULL; /*usb flow control return pre state*/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern void appPowerOnSoundPlay(void);
extern UINT32 _powerOnStateGet(void);

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
/*-------------------------------------------------------------------------
 *  File Name : appUsbCreateStateInit
 *------------------------------------------------------------------------*/
void
appUsbCreateStateInit(
	void
)

{
    appSensorPowerOff();    //close sensor power

	appModeSet(SP5K_MODE_STANDBY);
	CLEAR_OSD_SRC;

	appJpgDraw_PipLayer(ID_JPG_RES_USB_MODE,JPG_RES_START_X,JPG_RES_START_Y,JPG_RES_START_W,JPG_RES_START_H);

	appOsdMessageBoxShow(TRUE,STRTK_CONNECTING);
	sp5kGfxAttrSet(SP5K_GFX_REFRESH_ACTIVE, 1, 0, 1, 0);
	/*sp5kStillCapAdvCfgSet(SP5K_CAPTURE_CROP_SCALE_IN_JPEG ,0);*/
	if (IS_USB_IN)
	{
		if(pUiSetting->usbMode == USB_MSDC)
		{
			appStateChange(APP_STATE_USB_MSDC,STATE_PARAM_NORMAL_INIT);
		}
		else if(pUiSetting->usbMode == USB_UVCH264 || pUiSetting->usbMode == USB_UVCMJPG){
          	HOST_PROF_LOG_ADD(LEVEL_INFO,"usb crt: appStateChange to APP_STATE_USB_PCCAM");
			appStateChange(APP_STATE_USB_PCCAM,STATE_PARAM_NORMAL_INIT);
		}
	} else {
      	HOST_PROF_LOG_ADD(LEVEL_INFO,"usb crt: usb is not connected");
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appUsbBackwardStateGet
 *------------------------------------------------------------------------*/
UINT32
appUsbBackwardStateGet(
	void
)
{
	return preState;
}

/*-------------------------------------------------------------------------
 *  File Name : appUsbCreateState
 *------------------------------------------------------------------------*/
void
appUsbCreateState(
	UINT32 msg,
	UINT32 param
)
{
	DBG_PRINT("usb crt: %s : [0x%x] [0x%x]\n",__FUNCTION__,msg,param);

	switch (msg) {
	case APP_STATE_MSG_INIT:
		RPrintf("appUsbCreateState");
        if (appPreviousStateGet() == APP_STATE_POWER_ON)
	        preState = _powerOnStateGet();
        else if (appPreviousStateGet() != APP_STATE_USB_MSDC)
			preState = appPreviousStateGet();/*willow modify for hang up if power on while SD insert and USB connect*/

        /*usb flow control return pre state*/
        if (IS_USB_IN)
        {
            /**if(IS_USB_GROUP(preState))
                preState = APP_STATE_POWER_ON;
***/
            appBtnDisable(BTN_ALL);
            appUsbCreateStateInit();
        }
        else
        {   //SHEN: fow MSDC/PCCAM would change state to pre-state, not enter USB_CREATE_STATE again, so below flow should not work, NEED remove in the feature.
            HOST_PROF_LOG_PRINT(LEVEL_INFO,"usb crt: usb plug out return prestate =0x%x", preState);
            LCD_BACKLIGHT_OFF;
            /*tele gpio use in usb*/
            /****sp5kGpioCfgSet( GRP_BUTTON_TELE, 1<<CFG_BUTTON_TELE, INPUT);
	        sp5kButtonGpioCfgSet(BTN_ID_TELE, GRP_BUTTON_TELE, CFG_BUTTON_TELE, POL_BUTTON_TELE, GPIO_DBNC, GPIO_RPT);*******/
            /*sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 1500);*/

            if(preState != APP_STATE_POWER_ON)
            {
                if(pUiSetting->usbMode == USB_MSDC || pUiSetting->usbMode == USB_UVCMJPG || pUiSetting->usbMode == USB_UVCH264)

                appOsdMessageBoxShow(TRUE,STRTK_STU_STARTUP);
                sp5kGfxAttrSet(SP5K_GFX_REFRESH_ACTIVE, 1, 0, 0, 0);
                LCD_BACKLIGHT_ON;
                appPowerOnSoundPlay();
                sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 1500);
            }

            if(IS_MENU_STATE(preState))
                preState &= 0x0f00;

            HOST_PROF_LOG_ADD(LEVEL_INFO,"usb crt: usb plug out state change");

            appStateChange(preState, STATE_PARAM_NORMAL_INIT);
            preState = APP_STATE_NULL;
        }

		break;
	case APP_STATE_MSG_CLOSE:
        appStateCloseDone();
		break;
	case APP_UI_MSG_USB_PLUG:
		break;
	default:
		break;
	}

}

