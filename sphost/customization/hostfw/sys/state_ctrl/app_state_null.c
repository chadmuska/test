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

 /*--------------------------------------------------------------------------*
 *  File name	: 	app_state_null.c
 *  Function 	: 	null state process		 	
 *  First editor	: 	
 *  First date  	: 	
 *  History		:
 *  2018.06.04  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
#define HOST_DBG 1
#include "app_com_def.h"
#include "app_com_api.h"

#include "sp5k_disp_api.h"
#include "sp5k_capture_api.h"
#include "sp5k_cdsp_api.h"
#include "sp5k_ae_api.h"
#include "sp5k_awb_api.h"

#include "app_aaa.h"

 /*--------------------------------------------------------------------------*
 * Function name	: appNullStateClose
 * Function	       	: Null state Close
 * Return value   	: void
 * Parameter1    	: void
 * History	   :
 *                :  2018.06.04  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
void 
appNullStateClose(
	void
)

{
	//do nothing
}

 /*--------------------------------------------------------------------------*
 * Function name	: appNullStateInit
 * Function	       	: Null state Initial
 * Return value   	: void
 * Parameter1    	: void
 * History	   :
 *                :  2018.06.04  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

void
appNullStateInit(
	void
)

{
	//sp5kModeSet(SP5K_MODE_STANDBY);
}

 /*--------------------------------------------------------------------------*
 * Function name	: appNullStateOnMsg
 * Function	       	: MSG null state process
 * Return value   	: void
 * Parameter1    	: msg (UINT32)
 * Parameter2    	: param (UINT32)
 * History	   :
 *                :  2018.06.04  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

void
appNullStateOnMsg(
	UINT32 msg,
	UINT32 param
)

{
	//do nothing
}
 /*--------------------------------------------------------------------------*
 * Function name	: appNullStateOnKey
 * Function	       	: null state for key
 * Return value   	: void
 * Parameter1    	: msg (UINT32)
 * Parameter2    	: param (UINT32)
 * History	   :
 *                :  2018.06.04  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

void 
appNullStateOnKey(
	UINT32 msg,
	UINT32 param
)

{
	DBG_PRINT("state null: %s : [0x%x] [0x%x]\n",__FUNCTION__,msg,param);

	switch (msg) {
	case APP_KEY_PRESS_LEFT: 
	case APP_KEY_PRESS_RIGHT:
	case APP_KEY_PRESS_S2:
		break;
	}
}

 /*--------------------------------------------------------------------------*
 * Function name	: appNullState
 * Function	       	: null state process
 * Return value   	: void
 * Parameter1    	: msg (UINT32)
 * Parameter2    	: param (UINT32)
 * History	   :
 *                :  2018.06.04  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

void 
appNullState(
	UINT32 msg,
	UINT32 param
)
{	
	DBG_PRINT("state null: %s : [0x%x] [0x%x]\n",__FUNCTION__,msg,param);
	
	switch (msg) {
	case APP_STATE_MSG_INIT:		
		RPrintf("appNullState");
		appNullStateInit();
		/* do nothing */
		break;
	case APP_STATE_MSG_CLOSE:
		appStateCloseDone();
		break;	
	default:
		if (IS_APP_KEY_MSG(msg)) {
			appNullStateOnKey(msg,param);
		}
		else{
			appNullStateOnMsg(msg,param);
		}
		break;
	}

} 


