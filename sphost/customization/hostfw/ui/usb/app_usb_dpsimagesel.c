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
#include "sp5k_usb_api.h"

#include "app_dev_plug.h"
#include "app_osdfont.h"
#include "app_osd_api.h"
#include "app_menu_frameosd.h"
#include "app_usb_dpslib.h"
#include "app_playback_def.h"
#include "app_playback_osd.h"
#include "app_menu.h"
#include "app_usb_dps_osd.h"
#include "app_usb_dps_menu.h"

//temp hsinahu 04/22

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
extern dpsImage_t gDpsImage;
extern dpsInfo_t gDpsInfo;
extern appSubMenuInfo_t gDpsSubMenu;
extern appMenuInfo_t gDpsMainMenu;

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
enum{
	SET_THIS_PHOTO_YES = 0x00,
	SET_THIS_PHOTO_NO,
};

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/


/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/*-------------------------------------------------------------------------
 *  File Name : appUsbDpsImageSelState
 *------------------------------------------------------------------------*/

void
appUsbDpsImageSelStateInit(
	void
)
{
	APP_OSD_REFRESH_OFF;
	CLEAR_OSD_SRC;
	appDPSMenu_DPS2ndMenuItenSetandDraw(gDpsMainMenu.curTab,gDpsSubMenu.curItem);
	APP_OSD_REFRESH_ON;
}

/*-------------------------------------------------------------------------
 *  File Name : appUsbDps_DpsImageSelShift
 *------------------------------------------------------------------------*/
void appUsbDps_DpsImageSelShift(
	UINT8 shiftCtrl
)
{
	switch (shiftCtrl)
	{
	case PB_SHIFT_NEXT:
		PB_INDEX_INC(gDpsImage.curImgIdx, 1, gDpsImage.totalImg);
		break;
	case PB_SHIFT_PRE:
		PB_INDEX_DEC(gDpsImage.curImgIdx, 1, gDpsImage.totalImg);
		break;
	default:
		break;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appUsbDps_DpsImageShiftCreate
 *------------------------------------------------------------------------*/
void appUsbDps_DpsImageShiftCreate(
	UINT8 shiftCtrl
)
{
	unsigned int ShiftCount = 0;
	while(1){

		if (FAIL==appPbImagePlay(gDpsImage.curImgIdx)) {
			appUsbDps_DpsImageSelShift(shiftCtrl);
			ShiftCount++;
			if (ShiftCount>gDpsImage.totalImg){
				return;
			}
		}else{
			return;
		}
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appUsbDps_DpsImageSelStateOnKey
 *------------------------------------------------------------------------*/
void 
appUsbDps_DpsImageSelStateOnKey(
	UINT32 msg,
	UINT32 param)
{
	switch (msg) {
	case APP_KEY_PRESS_S1:
	case APP_KEY_PRESS_TELE:
	case APP_KEY_PRESS_WIDE:
	case APP_KEY_RELEASE_WIDE:
	case APP_KEY_RELEASE_TELE:
	case APP_KEY_PRESS_MODE:	
		break;
	case APP_KEY_PRESS_MENU:
	case APP_KEY_PRESS_SET:
		if(SET_THIS_PHOTO_NO==appPlayOsd_DlgBoxSelectGet())
		{
			CLEAR_OSD_SRC;
			appStateChange(APP_STATE_USB_PRINTSUBMENU,STATE_PARAM_NORMAL_INIT);
		}
		else
		{
			CLEAR_OSD_SRC;
			appStateChange(APP_STATE_USB_PRINT2NDMENU,STATE_PARAM_NORMAL_INIT);
		}
		break;
	case APP_KEY_PRESS_RIGHT:
		appUsbDps_DpsImageSelShift(PB_SHIFT_NEXT);
		appUsbDps_DpsImageShiftCreate(PB_SHIFT_NEXT);
		appDPSMenu_DPS2ndMenuHelpItemDraw(TRUE);
		if(gDpsInfo.PicBuf[gDpsImage.curImgIdx-1])
		{
			appDPSMenu_DpsOSDIconDraw(TRUE);
		}
		else
		{
			appDPSMenu_DpsOSDIconDraw(FALSE);
		}
		break;
	case APP_KEY_PRESS_LEFT:
		appUsbDps_DpsImageSelShift(PB_SHIFT_PRE);
		appUsbDps_DpsImageShiftCreate(PB_SHIFT_PRE);
		appDPSMenu_DPS2ndMenuHelpItemDraw(TRUE);
		if(gDpsInfo.PicBuf[gDpsImage.curImgIdx-1])
		{
			appDPSMenu_DpsOSDIconDraw(TRUE);
		}
		else
		{
			appDPSMenu_DpsOSDIconDraw(FALSE);
		}
		break;
	case APP_KEY_PRESS_UP:
	case APP_KEY_PRESS_DOWN:
		appPlayOsd_DlgBoxScroll((msg == APP_KEY_PRESS_DOWN ) ?TRUE :FALSE);
		break;	
	default:
		break;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appUsbDps_DpsImageSelStateOnMsg
 *------------------------------------------------------------------------*/
void 
appUsbDps_DpsImageSelStateOnMsg(
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
 *  File Name : appUsbDpsImageSelState
 *------------------------------------------------------------------------*/
void 
appUsbDpsImageSelState(
	UINT32 msg,
	UINT32 param
)
{	
	DBG_PRINT("usb dps img: %s : [0x%x] [0x%x]\n",__FUNCTION__,msg,param);

	switch (msg) {
	case APP_STATE_MSG_INIT:	
		RPrintf("appUsbDpsImageSelState");
		appUsbDpsImageSelStateInit();
		break;
	case APP_STATE_MSG_CLOSE:		
		appStateCloseDone();
		break;
	case APP_UI_MSG_USB_PLUG:	
		break;
	default:	
		if (IS_APP_KEY_MSG(msg)) {
			appUsbDps_DpsImageSelStateOnKey(msg,param);
		}
		else{
			appUsbDps_DpsImageSelStateOnMsg(msg,param);
		}
		break;
	}
} 

