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
#include "sp5k_usbhost_api.h"

#include "app_errorosd.h"
#include "app_dev_plug.h"
#include "app_tvlcd.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
/*why need use timer count??*/
/*static UINT32 msdcStartTimer;*/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static UINT32 mountNand = 0;
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT32 appUsbBackwardStateGet(void);

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

void
appUsbMsdcModeSet(
	void
)
{	
	//peter++ 2013/08/22 for judge usb status, if in host mode, need exit it
	UINT32 status;
	sp5kUsbhostCfgGet( SP5K_USBHOST_CFG_PHY, &status );
	if( status != 0 ) { //in host mode
		sp5kUsbhostControl(SP5K_USBHOST_CTRL_EXIT, 0, 0, 0);
	}
	//end
	
	sp5kUsbModeSet(SP5K_USB_MODE_MSDC);
	sp5kUsbCfgSet(SP5K_USB_MSDC_CUSTOM_MOUNT_SET, 1);
	sp5kUsbCfgSet(SP5K_USB_MSDC_LUN_SET, 0xff, 0xff);
	#if (SP5K_DISK_NAND || SP5K_DISK_EMMC || SP5K_DISK_ESD)
	UINT32 lunNo = 0;
	if (IS_DISK_NAND_MOUNT) {
		sp5kUsbCfgSet(SP5K_USB_MSDC_LUN_SET,SP5K_DRIVE_NAND, lunNo++);
	}
	else {
		sp5kDiskMountDirect(SP5K_DRIVE_NAND);
		sp5kUsbCfgSet(SP5K_USB_MSDC_LUN_SET,SP5K_DRIVE_NAND, lunNo++);
		mountNand = 1;
	}
	#endif
	#if SP5K_DISK_SD
	UINT32 lunNo1 = 0;
	sp5kUsbCfgSet(SP5K_USB_MSDC_LUN_SET,SP5K_DRIVE_SD, lunNo1++);
	#endif
}

/*-------------------------------------------------------------------------
 *  File Name : appUsbMsdcStateClose
 *------------------------------------------------------------------------*/
void 
appUsbMsdcStateClose(
	UINT32 type
)
{
	if (mountNand) {
		if (!IS_DISK_NAND_MOUNT) {
			printf("umount nand\n");
			sp5kDiskUnMount(SP5K_DRIVE_REMOVE | SP5K_DRIVE_NAND);
		}
		mountNand = 0;
	}
	if (type)
	{
		if (!IS_USB_IN)
		{
			sp5kUsbSwCon(0);
			appStateChange(appUsbBackwardStateGet(),STATE_PARAM_USB_PLUG);
		}
	}
	else
	{
        LCD_BACKLIGHT_ON;
		sp5kUsbSwCon(0);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appUsbMsdcStateInit
 *------------------------------------------------------------------------*/
void
appUsbMsdcStateInit(
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

    /*why need use timer count??*/
    /*msdcStartTimer= appTimerSet(5000, "MSDC Start Timer");*/

	// wait mount complete here
	if (IS_DISK_MOUNT_READY) 
	{
		sp5kUsbSwCon(0);
		appUsbMsdcModeSet();
		sp5kUsbCfgSet(SP5K_USB_DEV_MAX_POWER_SET,SP5K_USB_MAXPOWER_HIGH);
		sp5kUsbSwCon(1);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appUsbMsdcState
 *------------------------------------------------------------------------*/
void 
appUsbMsdcState(
	UINT32 msg,
	UINT32 param
)
{	
	static UINT8 isBack2PreviousState = 0;
	
	switch (msg) 
	{
	case APP_STATE_MSG_INIT:	
		RPrintf("appUsbMsdcState");
		isBack2PreviousState = 0;		
		appUsbMsdcStateInit();
		break;

	case APP_STATE_MSG_CLOSE:	        
		appUsbMsdcStateClose(FALSE);	
		appStateCloseDone();
		break;

	case APP_UI_MSG_USB_PLUG:	
		isBack2PreviousState = 1;		
		appUsbMsdcStateClose(TRUE);		
		break;

	case SP5K_MSG_TIMER:
        #if 0   /*why need use timer count??*/
        LCD_BACKLIGHT_OFF;
		appLcdTurnOff();
		appTimerClear(&msdcStartTimer);
        #endif
		break;

	case SP5K_MSG_DISK_MOUNT_COMPLETE:
        HOST_PROF_LOG_PRINT(LEVEL_INFO, "usb msdc: SP5K_MSG_DISK_MOUNT_COMPLETE %d", isBack2PreviousState); 
		isBack2PreviousState = 0;
		appStateChange(APP_STATE_USB_CREATE,STATE_PARAM_USB_PLUG);
		break;
	case SP5K_MSG_DISK_MOUNT_FAIL:		
	case SP5K_MSG_DISK_ERROR:		
		if (isBack2PreviousState) 
		{
			isBack2PreviousState = 0;
			appStateChange(appUsbBackwardStateGet(),STATE_PARAM_USB_PLUG);
		}
		break;

	case SP5K_MSG_USB_MSDC_ACCESS: 
	case SP5K_MSG_USB_MSDC_ACCESS_START:
	case SP5K_MSG_USB_MSDC_ACCESS_STOP:	
		if (isBack2PreviousState==0) 
		{
            HOST_PROF_LOG_ADD(LEVEL_WARN, "usb msdc: process start,disable lcd");
			isBack2PreviousState = 2;
		}      
		break;        
	default:		
		break;
	}
	
} 

