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

#include "app_still.h"
#include "app_state_ctrl.h"
#include "app_view_param_def.h"
#include "app_view_param.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
//The carry message mask 0xFF
#define ST_SELF_TWO_SECOND 2
#define ST_SELF_TEN_SECOND 10
#define ST_AAA_NORMAL 0
#define ST_AAA_FORCE_CAPTURE 1

//The flow control mask 0xF00
#define ST_CTL_RETURN_TO_LAST_STATE 0x100
#define ST_CTL_MAX  4
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct
{
	UINT32 state;
	UINT32 param;
} state_flow_unit_t;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
state_flow_unit_t flowSingle[] =
{
	{APP_STATE_STILL_PREVIEW, 		0},
	{APP_STATE_STILL_AAA, 			0},
	{APP_STATE_STILL_CAPTURE, 		0},
	{APP_STATE_STILL_PREVIEW, 		0},
};

state_flow_unit_t flowBurst[] =
{
	{APP_STATE_STILL_PREVIEW, 		0},
	{APP_STATE_STILL_AAA, 			0},
	{APP_STATE_BURST_CAPTURE, 		0},
	{APP_STATE_STILL_PREVIEW, 		0},
};

state_flow_unit_t* stillFlowList[] =
{
	[UI_STILL_DRIVERMODE_OFF] 		= flowSingle,
	[UI_STILL_DRIVERMODE_BURST_H]	= flowBurst,
	[UI_STILL_DRIVERMODE_BURST_L]	= flowBurst,
};


state_flow_unit_t* pCurFlow = flowSingle;
UINT32 CurStateIdx = 0;
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/


/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void
appStillFlow_SetDriveFlow(
	UINT32 driveMode,
	UINT32 viewMode,
	UINT32 sceneMode
)
{
	/*DBG_PRINT("sflow: %s: driver mode:%d\n", __FUNCTION__, driveMode);*/
	pCurFlow = stillFlowList[driveMode];
	CurStateIdx = 0;
}
void
appStillFlow_GetNextState(
	UINT32* pState,
	UINT32* pParam
)
{
	// check cur state param
	if(FLOW_EXTRCT_CTRL_PAR(pCurFlow[CurStateIdx].param) == ST_CTL_RETURN_TO_LAST_STATE)
	{
		CurStateIdx--;
	}
	else
	{
    	CurStateIdx++;
	}

    if(CurStateIdx>=ST_CTL_MAX)
        CurStateIdx = ST_CTL_MAX-1;
        
	*pState = pCurFlow[CurStateIdx].state;
	*pParam = pCurFlow[CurStateIdx].param + STATE_PARAM_FREE_CARRY;
	/*DBG_PRINT("sflow: %s: %d st= 0x%x, par+0xF000= 0x%x \n", __FUNCTION__, CurStateIdx, *pState, *pParam);*/
}

