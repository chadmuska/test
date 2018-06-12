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
#include "sp5k_disk_api.h"

#include "app_dev_plug.h"
#include "app_errorosd.h"
#include "app_usb_dpslib.h"
#include "app_playback_def.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

static UINT32 dpsStartTimer;
dpsImage_t  gDpsImage;
dpsInfo_t  gDpsInfo;

dpsMenuOption_t  gDpsMenuOpt ;
pbImgCb_t gpbDPSImgCb ={0}; 

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

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/*-------------------------------------------------------------------------
 *  File Name : appUsbDpsStateInit
 *------------------------------------------------------------------------*/
void
appUsbDpsStateInit(
	void
)
{

	if (!IS_USB_IN) 
	{
		LCD_BACKLIGHT_ON;
		return;
	}

	#if POWER_OFF_AFTER_USB_OUT
	appSensorPowerOff();
	#endif

	sp5kDcfFsFileTypeSet(SP5K_DCF_FILETYPE_JPG);
	gDpsImage.totalImg=appPbTotalFileCountGet();
	

	if (gDpsImage.totalImg == 0)
	{
		appOsdMessageBoxShow(TRUE,STRTK_NO_JPEG);
		return;
	}
	else
	{
		gDpsImage.curImgIdx =1;
	}

	 dpsStartTimer= appTimerSet(20000, "DPS Start Timer");

	gDpsInfo.SelPic=0;
	gDpsInfo.PicBuf=NULL;
	gDpsInfo.dpsModePara =DPS_STANDBYMODE;
	gDpsInfo.IndexPrintCap=0;
	gDpsInfo.DpofPrintCap=0;

/*peter temp mark*/
#if 0
	sp5kUsbPtpIpClose();
#endif
	sp5kUsbModeSet(SP5K_USB_MODE_SIDC);
	sp5kUsbCfgSet(SP5K_USB_DEV_MAX_POWER_SET,SP5K_USB_MAXPOWER_HIGH);
	sp5kUsbSwCon(1);

}

/*-------------------------------------------------------------------------
 *  File Name : appUsbDps_PrintCapabilityGet
 *------------------------------------------------------------------------*/
void appUsbDps_PrintCapabilityGet(void)
{
	gDpsMenuOpt.Print.OsdEnOption =0x00000001;
	appUsbDps_PaperSizeCapabilityGet(&gDpsMenuOpt);
	appUsbDps_PaperLayoutCapabilityGet(&gDpsMenuOpt);
	appUsbDps_PaperTypeCapabilityGet(&gDpsMenuOpt);
	appUsbDps_PaperQualityCapabilityGet(&gDpsMenuOpt);
	appUsbDps_DateTimePrintCapabilityGet(&gDpsMenuOpt);
	appUsbDps_FileNamePrintCapabilityGet(&gDpsMenuOpt);
	appUsbDps_IndexPrintCapabilityGet(&gDpsMenuOpt);
	appUsbDps_DPOFPrintCapabilityGet(&gDpsMenuOpt);
}

/*-------------------------------------------------------------------------
 *  File Name : appUsbDps_PrintDiscover
 *------------------------------------------------------------------------*/
void appUsbDps_PrintDiscover(
	void
)
{
	if (gDpsInfo.PicBuf ==	NULL){	
		gDpsInfo.PicBuf = sp5kMalloc(gDpsImage.totalImg);
		if (gDpsInfo.PicBuf ==NULL){
			DBG_ASSERT(gDpsInfo.PicBuf!=NULL);
		}else{
			memset(gDpsInfo.PicBuf,0,gDpsImage.totalImg);
		}
	}

}

/*-------------------------------------------------------------------------
 *  File Name : appUsbDps_DpsStateOnKey
 *------------------------------------------------------------------------*/
void 
appUsbDps_DpsStateOnKey(
	UINT32 msg,
	UINT32 param)
{
	switch (msg) {
	case APP_KEY_PRESS_S1:
	case APP_KEY_PRESS_S2:
	case APP_KEY_PRESS_TELE:
	case APP_KEY_PRESS_WIDE:
	case APP_KEY_RELEASE_WIDE:
	case APP_KEY_RELEASE_TELE:
	case APP_KEY_PRESS_MENU:
	case APP_KEY_PRESS_MODE:	
	case APP_KEY_PRESS_SET:	
	case APP_KEY_PRESS_RIGHT:
	case APP_KEY_PRESS_LEFT:
	case APP_KEY_PRESS_UP:
	case APP_KEY_PRESS_DOWN:
		break;	
	default:
		break;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appUsbDps_DpsStateOnMsg
 *------------------------------------------------------------------------*/
void 
appUsbDps_DpsStateOnMsg(
	UINT32 msg,
	UINT32 param
)
{
	switch ( msg) {
	case SP5K_MSG_USB_DPS_DISCOVER:	
		appTimerClear(&dpsStartTimer);	
		appUsbDps_PrintCapabilityGet();
		appUsbDps_PrintDiscover();
		appStateChange(APP_STATE_USB_PRINTMENU,STATE_PARAM_NORMAL_INIT);
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
 *  File Name : appUsbDpsState
 *------------------------------------------------------------------------*/
void 
appUsbDpsState(
	UINT32 msg,
	UINT32 param
)
{	

	switch (msg) 
	{
	case APP_STATE_MSG_INIT:	
		RPrintf("appUsbDpsState");
		appUsbDpsStateInit();
		break;
	case APP_STATE_MSG_CLOSE:		
		appStateCloseDone();
		break;
	case APP_UI_MSG_USB_PLUG:	
		appTimerClear(&dpsStartTimer);
		sp5kFree(gDpsInfo.PicBuf);
		appStateCloseDone();
		break;

	case SP5K_MSG_TIMER: 
		if (param==dpsStartTimer) 
		{
			appTimerClear(&dpsStartTimer);
			appOsdMessageBoxShow(TRUE,STRTK_USB_ERR);
		}
		break;
	default:	
		if (IS_APP_KEY_MSG(msg)) 
		{
			appUsbDps_DpsStateOnKey(msg,param);
		}
		else
		{
			appUsbDps_DpsStateOnMsg(msg,param);
		}
		break;
	}

} 

