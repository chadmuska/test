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

#include "app_com_def.h"
#include "app_com_api.h"
#include "gpio_custom.h"

#include "sp5k_usb_api.h"
#include "sp5k_rsvblk_api.h"

#include "app_errorosd.h"
#include "app_dev_plug.h"
#include "app_tvlcd.h"
#include "app_still.h"
#include "sp5k_media_api.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
/*static UINT32 pccamStartTimer = 0;*/  /* dual stream mask */
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
extern UINT32 appUsbBackwardStateGet(void);

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

void
appUsbPccamStateClose(
	void
)
{
#if SUPPORT_RUNTIME_SWITCH_DRAM_CLK
	appSysClkSwitch(0);
#endif
	if (!IS_USB_IN)
	{
		sp5kUsbSwCon(0);
	}
}

void
appUsbPccamStateInit(
	void
)
{
    UINT32 ret;

	if (!IS_USB_IN)
	{
		LCD_BACKLIGHT_ON;
		return;
	}
	
#if SUPPORT_RUNTIME_SWITCH_DRAM_CLK
	appSysClkSwitch(1);
#endif

	//SHEN: UVC+UAC bandwidth about 10MBps, so video can set  7MBps for smooth.
	//      UVC only, can set 12MBps.
	sp5kUsbCfgSet(SP5K_USB_PCCAM_UVC_MJPG_BITRATE_SET, 7*1024*1024*8);
	// H264 bitrate
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_AVG_BITRATE, 7*1024*1024);

	sp5kUsbSwCon( 0 );
//	pccamStartTimer= appTimerSet(5000, "PCCAM Start Timer");	/* dual stream mask */
	sp5kUsbModeSet(SP5K_USB_MODE_PCCAM );
//	sp5kUsbCfgSet(SP5K_USB_DEV_MAX_POWER_SET,SP5K_USB_MAXPOWER_HIGH);

	//sp5kUsbCfgSet(SP5K_USB_PCCAM_UVC_VER_SET,1);
	//sp5kResourceFileLoad( SP5K_RES_USB_CONF_BIN, (char *)RES_USB_USBUVC_15);

	sp5kUsbCfgSet(SP5K_USB_PCCAM_UVC_VER_SET,0);

	#if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
	if (appDualPipModeGet() >= DUALSEN_SINGLE_SEC)
	{
		sp5kUsbCfgSet(SP5K_USB_PCCAM_UVC_IF_EXCHANGE_SET, 1);
		ret =sp5kResourceFileLoad(SP5K_RES_USB_CONF_BIN, (char *)RES_USB_USBUVC_DUAL);
	}
	else
	{
		sp5kUsbCfgSet(SP5K_USB_PCCAM_UVC_IF_EXCHANGE_SET, 1);
		ret =sp5kResourceFileLoad(SP5K_RES_USB_CONF_BIN, (char *)RES_USB_USBUVC);
	}
	#else
		sp5kUsbCfgSet(SP5K_USB_PCCAM_UVC_IF_EXCHANGE_SET, 1);
		ret =sp5kResourceFileLoad(SP5K_RES_USB_CONF_BIN, (char *)RES_USB_USBUVC);
	#endif

	if (ret != SUCCESS) {
        HOST_PROF_LOG_ADD(LEVEL_ERROR, "usb pcam: USB UVC Load Resource Fail");
	}else{
        HOST_PROF_LOG_ADD(LEVEL_INFO, "usb pcam: USB UVC Load Resource Success");
	}

	sp5kUsbSwCon(1);
}

void
appUsbPccamState(
	UINT32 msg,
	UINT32 param
)
{


	switch (msg)
	{
	case APP_STATE_MSG_INIT:
		RPrintf("appUsbPccamState");
		appUsbPccamStateInit();
		break;
	case APP_STATE_MSG_CLOSE:
		appUsbPccamStateClose();
		appStateCloseDone();
		break;
	case APP_UI_MSG_USB_PLUG:
		appUsbPccamStateClose();
		appStateChange(appUsbBackwardStateGet(),STATE_PARAM_USB_PLUG);
		break;
	case SP5K_MSG_TIMER:
		appLcdTurnOff();
//		appTimerClear(&pccamStartTimer);	/* dual stream mask */
		break;
	default:
		break;
	}

}

