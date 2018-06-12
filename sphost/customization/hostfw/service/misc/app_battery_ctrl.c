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
 *	File name    : App_battery_ctrl.c
 *	Function     : Provide battery charge flow.
 *	First editor :   
 *	First date   :   
 *	History      :
 *               : 2018.06.04  Edward.Lu  "Add description and modify code"
 **************************************************************************/
#define HOST_DBG 0

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "math.h"
#include "common.h"
#include "sp5k_usb_api.h"
#include "app_battery_ctrl.h"
#include "app_battery.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

#define SHORT_TYPE_ISCONNECTED 0x0f /*USB D+/D- is short. It might indicate that a short-type USB charger is connected.*/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
appBatteryChargeStartCBF_t *gBatteryChargeStartCBF;
appBatteryChargeStopCBF_t *gBatteryChargeStopCBF;
appBatteryCheckStatusCBF_t *gBatteryCheckStatusCBF;

UINT8 gucBatteryStatus=BATTERY_STATUS_NON_FULL;

UINT8 gChargerStatus=FALSE;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/*--------------------------------------------------------------------------*
* Function name    : appBatteryCharge_SetStartCBFRegister
* Function         : 
* Return value	   : 
* Parameter1	   : 
* History          :
*                  : 2018.06.04  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	

void appBatteryCharge_SetStartCBFRegister(
	appBatteryChargeStartCBF_t *aBatteryChargeStartCBF
)
{
    if (!aBatteryChargeStartCBF)
	{
		gBatteryChargeStartCBF = NULL;
	}
	else
	{
		gBatteryChargeStartCBF = aBatteryChargeStartCBF;
	}
}

/*--------------------------------------------------------------------------*
* Function name    : appBatteryCharge_SetStartCBF
* Function         : 
* Return value	   : 
* Parameter1	   : 
* History          :
*                  : 2018.06.04  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	

void appBatteryCharge_SetStartCBF(void)
{

	if (gBatteryChargeStartCBF){
		gBatteryChargeStartCBF();
		gChargerStatus=TRUE;
	}
}

/*--------------------------------------------------------------------------*
* Function name    : appBatteryCharge_SetStopCBFRegister
* Function         : 
* Return value	   : 
* Parameter1	   : 
* History          :
*                  : 2018.06.04  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	

void appBatteryCharge_SetStopCBFRegister(
	appBatteryChargeStopCBF_t *aBatteryChargeStopCBF
)
{
    if (!aBatteryChargeStopCBF)
	{
		gBatteryChargeStopCBF = NULL;
	}
	else
	{
		gBatteryChargeStopCBF = aBatteryChargeStopCBF;
	}
}

/*--------------------------------------------------------------------------*
* Function name    : appBatteryCharge_SetStopCBF
* Function         : 
* Return value	   : 
* Parameter1	   : 
* History          :
*                  : 2018.06.04  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	

void appBatteryCharge_SetStopCBF(void)
{
	if (gBatteryChargeStopCBF){
		gBatteryChargeStopCBF();
		gChargerStatus=FALSE;
	}
}

/*--------------------------------------------------------------------------*
* Function name    : appBatteryCharge_SetCheckStatusCBFRegister
* Function         : 
* Return value	   : 
* Parameter1	   : 
* History          :
*                  : 2018.06.04  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	

void appBatteryCharge_SetCheckStatusCBFRegister(
	appBatteryCheckStatusCBF_t *aBatteryCheckStatusCBF
)
{
    if (!aBatteryCheckStatusCBF)
	{
		gBatteryCheckStatusCBF = NULL;
	}
	else
	{
		gBatteryCheckStatusCBF = aBatteryCheckStatusCBF;
	}
}

/*--------------------------------------------------------------------------*
* Function name    : appBatteryCharge_SetCheckStatusCBF
* Function         : 
* Return value	   : 
* Parameter1	   : 
* History          :
*                  : 2018.06.04  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	

UINT8 appBatteryCharge_SetCheckStatusCBF(void)
{
	if (gBatteryCheckStatusCBF)
		gucBatteryStatus = gBatteryCheckStatusCBF();
	else
	    gucBatteryStatus = BATTERY_STATUS_NON_FULL;

	return gucBatteryStatus;
}

/*--------------------------------------------------------------------------*
* Function name    : appBatteryCharge_CBRegister
* Function         : Register callback function which ctrl battery.
* Return value	   : SUCCESS
* Parameter1	   : void
* History          :
*                  : 2018.06.04  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	

UINT8 appBatteryCharge_CBRegister(void)
{
#if COMPEL_CHARGING
	appBatteryCharge_SetStartCBFRegister(appBatteryCharge_Start);
	appBatteryCharge_SetStopCBFRegister(appBatteryCharge_Stop);
	appBatteryCharge_SetCheckStatusCBFRegister(appBatteryCharge_CheckStatus);
#endif
	return SUCCESS;

}

/*--------------------------------------------------------------------------*
* Function name    : appBatteryCharge_LineStateGet
* Function         : Check USB charger is connected or not.
* Return value	   : BATTERY_DCSRC_ADAPTER/BATTERY_DCSRC_PC (appDCSRC_e)
* Parameter1	   : void
* History          :
*                  : 2018.06.04  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	

appDCSRC_e appBatteryCharge_LineStateGet(void)
{
	if( sp5kUsbLineStateGet()==SHORT_TYPE_ISCONNECTED  ){
		return BATTERY_DCSRC_ADAPTER;
	}else{
		return BATTERY_DCSRC_PC;
	}

}

/*--------------------------------------------------------------------------*
* Function name    : appBatteryCharge_Init
* Function         : Battery charge init
* Return value	   : SUCCESS (UINT8)
* Parameter1	   : void
* History          :
*                  : 2018.06.04  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	

UINT8 appBatteryCharge_Init(void)
{
	appBatteryCharge_CBRegister();

	return SUCCESS;

}

/*--------------------------------------------------------------------------*
* Function name    : appBatteryCharge_ChargerStatusGet
* Function         : get battery charger status
* Return value	   : void
* Parameter1	   : gChargerStatus (UINT8 TRUE/FALSE)
* History          :
*                  : 2018.06.04  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	

UINT8 appBatteryCharge_ChargerStatusGet(void)
{

	return gChargerStatus;

}

