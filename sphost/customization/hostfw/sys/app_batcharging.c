/*--------------------------------------------------------------------------*
 *	File name	: App_batcharging.c
 *	Function	: "explanation of this file  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
 *	First editor: 
 *	First date	: 
 *	History:
 *	
 *	
 *	
 *	
 *--------------------------------------------------------------------------*/

/**************************************************************************
 *
 *       Copyright (c) 2012-2018 by iCatch Technology, Inc.
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

#include "sp5k_usb_api.h"
#include "sp5k_gfx_api.h"

#include "app_battery.h"
#include "app_timer.h"
#include "app_icon_def.h"
#include "app_pal.h"
#include "../service/misc/app_battery_ctrl.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static UINT32 ChargingTimer = TIMER_NULL;
static UINT32 count = 0;

#define LCD_SIZE_X					320
#define LCD_SIZE_Y					240
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern BOOL bBatteryIn;
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
/*--------------------------------------------------------------------------*
* Function name : appBatteryChargingInit 
* Function		: function for initial Battery Charging
* Return value	: void  
* Parameter1	: void
* History	   :
*                 :  2018.06.01  Paul.Chang     "Add description"				  
*--------------------------------------------------------------------------*/
void appBatteryChargingInit(void)
{
    BOOL bsrc_PC = FALSE;

    bBatteryIn = appBatteryAdcValueGet() > 100 ? TRUE:FALSE;
    bsrc_PC = appBatteryCharge_LineStateGet();

	if(appBatteryCharge_SetCheckStatusCBF() != BATTERY_STATUS_FULL && bBatteryIn)
	{
	    if(bsrc_PC == BATTERY_DCSRC_ADAPTER || (bsrc_PC == BATTERY_DCSRC_PC && sp5kUsbMaxPowerCheck(SP5K_USB_MAXPOWER_HIGH))){
	        appBatteryCharge_SetStartCBF();
			RED_LED_ON;
		}
	}
	ChargingTimer = appTimerSet(1000, "Charging Timer");
}
/*--------------------------------------------------------------------------*
* Function name : appBatteryChargingStateOnMsg 
* Function		: State MSG function for Battery Charging
* Return value	: void  
* Parameter1	: UINT32 *msg
* Parameter2	: UINT32 *param
* History	   :
*                 :  2018.06.01  Paul.Chang     "Add description"				  
*--------------------------------------------------------------------------*/
void
appBatteryChargingStateOnMsg(
    UINT32 msg,
    UINT32 param
)
{
    switch (msg) {
        case SP5K_MSG_TIMER:
            if (param == ChargingTimer)
            {
                if(count < 3)
                {
                    sp5kGfxRectFill(SP5K_GFX_PAGE_OSD_0, 0, 0, LCD_SIZE_X, LCD_SIZE_Y, PAL_BLACK);
                    sp5kGfxIconDraw(SP5K_GFX_PAGE_OSD_0, LCD_SIZE_X/2, LCD_SIZE_Y/2, SP5K_GFX_ALIGN_TOP, ID_ICON_BATTERY_EMPTY+count);
                }
                count++;
                if(count >= 3)
                    count %= 3;
                if(appBatteryCharge_SetCheckStatusCBF() == BATTERY_STATUS_FULL)
                {
                    appBatteryCharge_SetStopCBF();
			        RED_LED_OFF;
                    appTimerClear(&ChargingTimer);
                    sp5kGfxRectFill(SP5K_GFX_PAGE_OSD_0, 0, 0, LCD_SIZE_X, LCD_SIZE_Y, PAL_BLACK);
                    sp5kGfxIconDraw(SP5K_GFX_PAGE_OSD_0, LCD_SIZE_X/2, LCD_SIZE_Y/2, SP5K_GFX_ALIGN_TOP, ID_ICON_BATTERY_FULL);
                }
            }
            break;
        default:
            break;
    }
}
/*--------------------------------------------------------------------------*
* Function name : appBatteryChargingState
* Function		: State  function for Battery Charging
* Return value	: void  
* Parameter1	: UINT32 *msg
* Parameter2	: UINT32 *param
* History	   :
*                 :  2018.06.01  Paul.Chang     "Add description"	  
*--------------------------------------------------------------------------*/
void
appBatteryChargingState(
	UINT32 msg,
	UINT32 param
)
{
	switch (msg)
	{
		case APP_STATE_MSG_INIT:
			appBatteryChargingInit();
			break;
		case APP_STATE_MSG_CLOSE:
		    appStateCloseDone();
		    break;
		default:
		    appBatteryChargingStateOnMsg(msg,param);
			break;
	}
}

