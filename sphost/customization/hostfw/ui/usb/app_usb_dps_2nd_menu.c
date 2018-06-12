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
#include "app_dev_plug.h"
#include "sp5k_usb_api.h"
#include "app_osdfont.h"
#include "app_osd_api.h"
#include "app_menu_frameosd.h"
#include "app_usb_dpslib.h"
#include "app_menu.h"
#include "app_usb_dps_osd.h"
#include "app_usb_dps_menu.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
UINT8 DpsModePara = 0; 
UINT16 selPic;
#define DPS_PICS_NUM 	30

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
extern dpsInfo_t gDpsInfo;
extern appSubMenuInfo_t gDpsSubMenu;

extern dpsImage_t gDpsImage;

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/*-------------------------------------------------------------------------
*  File Name : appUsbDps2ndMenuStateInit
*------------------------------------------------------------------------*/
void
appUsbDps2ndMenuStateInit(
	void
)
{
	APP_OSD_REFRESH_OFF;
	CLEAR_OSD_SRC;
	appDPSMenu_DPS2ndMenuImageFrameDraw(DpsModePara,selPic);
	APP_OSD_REFRESH_ON;
}


/*-------------------------------------------------------------------------
*	File Name : appUsbDps_Dps2ndMenuStateOnKey
*------------------------------------------------------------------------*/
void 
appUsbDps_Dps2ndMenuStateOnKey(
	UINT32 msg,
	UINT32 param)
{
	switch(msg){
		case APP_KEY_PRESS_LEFT:
		case APP_KEY_PRESS_RIGHT:
			if(DpsModePara==DPS_MENU_2ND_SELECT0)
			{
				appUsbDps2ndMenuLeftRightCangeSet((msg == APP_KEY_PRESS_LEFT ) ?TRUE :FALSE);
			}
			break;
             case APP_KEY_PRESS_SET:
			if(DpsModePara==DPS_MENU_2ND_SELECT0)
			{
				if(gDpsSubMenu.curItem==DPS_IMAGE_SIGNAL)
				{
					appStateChange(APP_STATE_USB_PRINTSUBMENU,STATE_PARAM_NORMAL_INIT);
					if(gDpsInfo.PicBuf[gDpsImage.curImgIdx-1])
					{
						appDPSMenu_DpsOSDIconDraw(TRUE);
					}
					else
					{
						appDPSMenu_DpsOSDIconDraw(FALSE);
					}
				}
				else
				{
					appStateChange(APP_STATE_USB_PRINTSUBMENU,STATE_PARAM_NORMAL_INIT);
				}
				_DpsSettingMaintain();
			}
			else if(DpsModePara==DPS_MENU_2ND_SELECT1)
			{
				appStateChange(APP_STATE_USB_PRINTSUBMENU,STATE_PARAM_NORMAL_INIT);
			}
			break;
		case APP_KEY_PRESS_UP:
		case APP_KEY_PRESS_DOWN:
			DpsModePara ^= 1;
			appDPSMenu_DPS2ndMenuImageUpDownChangeDraw(DpsModePara,selPic);
			break;	
		default:
			break;
		}
}

/*-------------------------------------------------------------------------
 *	File Name : appUsbDps_Dps2ndMenuStateOnMsg
 *------------------------------------------------------------------------*/
void 
appUsbDps_Dps2ndMenuStateOnMsg(
	UINT32 msg,
	UINT32 param
)
{
	switch ( msg) {
	case SP5K_MSG_USB_DPS_DISCOVER: 
		break;

	case SP5K_MSG_USB_DPS_NOTIFY_JOB_STATUS:	
		break;

	case SP5K_MSG_USB_DPS_NOTIFY_DEVICE_STATUS: 
		break;

	default:
		break;
	}	
}

/*-------------------------------------------------------------------------
 *	File Name : appUsbDps2ndMenuState
 *------------------------------------------------------------------------*/
void 
appUsbDps2ndMenuState(
	UINT32 msg,
	UINT32 param
)
{	
	DBG_PRINT("usb dps menu2: %s : [0x%x] [0x%x]\n",__FUNCTION__,msg,param);
	
	switch (msg) {
	case APP_STATE_MSG_INIT:	
		RPrintf("appUsbDps2ndMenuState");
		appUsbDps2ndMenuStateInit();
		break;
	case APP_STATE_MSG_CLOSE:		
		appStateCloseDone();
		break;
	case APP_UI_MSG_USB_PLUG:	
		sp5kFree(gDpsInfo.PicBuf);
		appStateCloseDone();
		break;
	default:	
		if (IS_APP_KEY_MSG(msg)) {
			appUsbDps_Dps2ndMenuStateOnKey(msg,param);
		}
		else{
			appUsbDps_Dps2ndMenuStateOnMsg(msg,param);
		}
		break;
	}
} 

void appUsbDps2ndMenuLeftRightCangeSet(
	unsigned char bMenuLeft)
{
	if(bMenuLeft){
		if(selPic == 0){
			selPic = DPS_PICS_NUM;
		}else{
			selPic--;
		}
	}else{
		if(selPic == DPS_PICS_NUM){
			selPic = 0;
		}else{
			selPic++;
		}
	}
	appDPSMenu_DPS2ndMenuImageSelChangeDraw(DpsModePara,selPic);
}

