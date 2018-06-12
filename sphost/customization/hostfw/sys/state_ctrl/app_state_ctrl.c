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
 *  File name	: 	app_state_ctrl.c
 *  Function 	: 	state control process		 	
 *  First editor	: 	
 *  First date  	: 	
 *  History		:
 *  2018.06.04  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

#define HOST_DBG 1
#include "app_com_def.h"
#include "app_com_api.h"

#include "app_state_private.h"
#include "app_dev_plug.h"
#include "app_still.h"
#include "app_state_ctrl.h"
#include "app_view_param_def.h"
#include "app_view_param.h"
#include "app_osd_api.h"
#include "app_dbg_api.h"
#include "sp5k_dbg_api.h"
/*state check time performance*/
#define STATE_CHECK_TIME            0
#define SAME_STATE_GROUP_DIRECT_SWITCH 1
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct appState_s {
    UINT32      appStateGroup;
	pfpSATATE   pfpState;
	UINT32 hotPlugCfg;
	char *name;
} appState_t;
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void appNullState(UINT32 msg,UINT32 param);
void appPowerOnState(UINT32 msg,	UINT32 param);
void appPowerOffState(UINT32 msg,	UINT32 param);
void appPowerHibernateState(UINT32 msg, UINT32 param);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
void appJetPowerOffState(UINT32 msg, UINT32 param);
void appJetSdcertState(UINT32 msg, UINT32 param);
#endif
void appBatteryChargingState(UINT32 msg,UINT32 param);
void appStillPvState(UINT32 msg,UINT32 param);
void appStillCapState(UINT32 msg,UINT32 param);
void appStillBurstState(UINT32 msg,UINT32 param);
void appStillAaaState(UINT32 msg,UINT32 param);
void appStillBusyState(UINT32 msg,UINT32 param);
void appVideoPvState(UINT32 msg, UINT32 param);
void appVideoRecState(UINT32 msg, UINT32 param);
void appPbMainState(UINT32 msg,	UINT32 param);
void appPbImgProtState(UINT32 msg, UINT32 param);
void appPbImgDelState(UINT32 msg, UINT32 param);
void appPbVideoState(UINT32 msg, UINT32 param);

void appMenuState(UINT32 msg,UINT32 param);

void appUsbCreateState(UINT32 msg,	UINT32 param);
void appUsbMsdcState(UINT32 msg,	UINT32 param);
void appUsbPccamState(UINT32 msg,UINT32 param);
void appTvPlugState(UINT32 msg,UINT32 param);
#if (KIT_WITH_HDMI)
void appHDMIPlugState(UINT32 msg,UINT32 param);
#endif
void appCecCapabilityCtrlState(UINT32 msg,UINT32 param);

void appCardPlugState(UINT32 msg,	UINT32 param);
void appCalibState(UINT32 msg,UINT32 param);
void appCalibZoomLensState(UINT32 msg,	UINT32 param);

void appTBMain_State(UINT32 msg,	UINT32 param);
void appCcMain_State(UINT32 msg,	UINT32 param);
void appTbUnitTestState(UINT32 msg,	UINT32 param);
void appTbBat_CurveState(UINT32 msg, UINT32 param);
void appCalibAwbState(UINT32 msg, UINT32 param);

#if (ICAT_WIFI || ICAT_ETHERNET)
void appWiFiConnectionState(UINT32 msg, UINT32 param);
#endif
#if GPS_OPEN
void appGPSGSVINFOState(UINT32 msg, UINT32 param);
#endif

void appFileTypeSetState(UINT32 msg,UINT32 param);

void _stateController(UINT32 msg,UINT32 param);
void _stateInitial(UINT32 nextState,UINT32 param);
void _stateClose(UINT32 nextState,UINT32 param);
UINT32 _stateCheck(UINT32 state);

/*paul 2018.04.16 add for modify compiler warning*/
extern void appInfoiStateToPState(UINT32 iState);

#if defined(PRJ_JETDVR)||defined(PRJ_17DVR)||defined(PRJ_19DVR)
#include "app_infohandlerComm.h"
extern SP5K_EVENT_FLAGS_GROUP qEvent_sig;
#endif

/**************************************************************************
 *                         L O C A L    D A T A                         *
 **************************************************************************/
/* state process table */
static appState_t    appStateTbl[] =
{
    /* sys */
    [APP_STATE_NULL_IDX]                 = {APP_STATE_NULL, appNullState, ALL_PLUG_DO_PROC, "Null"},
    [APP_STATE_POWER_ON_IDX]             = {APP_STATE_POWER_ON, appPowerOnState, PWR_ON_PLUG_CFG, "Pwr On"},
    [APP_STATE_POWER_OFF_IDX]            = {APP_STATE_POWER_OFF, appPowerOffState, PWR_OFF_PLUG_CFG, "Pwr Off"},
    [APP_STATE_POWER_HIBERNATE_IDX]      = {APP_STATE_POWER_HIBERNATE, appPowerHibernateState, PWR_HIBERNATE_PLUG_CFG, "Pwr HIBERNATE"},
#ifdef JET_FEATURE
    [APP_STATE_JET_POWER_OFF_IDX]	 = {APP_STATE_JET_POWER_OFF, appJetPowerOffState, PWR_HIBERNATE_PLUG_CFG, "pseudo Pwr Off"},
    [APP_STATE_JET_SDCERT_IDX]	 = {APP_STATE_JET_SD_CERT, appJetSdcertState, ALL_PLUG_DO_PROC, "SD CERT states"},

#endif

    /* still  */
    [APP_STATE_STILL_PREVIEW_IDX]        = {APP_STATE_STILL_PREVIEW, appStillPvState, STILL_PV_PLUG_CFG, "Still PV"},
    [APP_STATE_STILL_CAPTURE_IDX]        = {APP_STATE_STILL_CAPTURE, appStillCapState, STILL_CAP_PLUG_CFG, "Capture"},
    [APP_STATE_BURST_CAPTURE_IDX]        = {APP_STATE_BURST_CAPTURE, appStillBurstState, STILL_BURST_PLUG_CFG, "Burst"},
    [APP_STATE_STILL_AAA_IDX]            = {APP_STATE_STILL_AAA, appStillAaaState, STILL_AAA_PLUG_CFG, "S1 Hold"},
    [APP_STATE_STILL_BUSY_IDX]           = {APP_STATE_STILL_BUSY, appStillBusyState, STILL_BUSY_PLUG_CFG, "Still Busy"},
	[APP_STATE_MENU_IDX]	= {APP_STATE_MENU, appMenuState, MENU_PLUG_CFG, "Main Menu"},

    /* video */
    [APP_STATE_VIDEO_PREVIEW_IDX]        = {APP_STATE_VIDEO_PREVIEW, appVideoPvState, VIDEO_PV_PLUG_CFG, "Video PV"},
    [APP_STATE_VIDEO_REC_IDX]            = {APP_STATE_VIDEO_REC, appVideoRecState, VIDEO_REC_PLUG_CFG, "Video Rec"},
    [APP_STATE_VIDEO_AAA_IDX]            = {APP_STATE_VIDEO_AAA, appStillAaaState, STILL_AAA_PLUG_CFG, "S1 Hold"},

    #if GPS_OPEN
    [APP_STATE_GPS_GSVINFO_IDX]          = {APP_STATE_GPS_GSVINFO, appGPSGSVINFOState,ALL_PLUG_DO_PROC,"Gpsgsvinfo"},
    #endif

    /* playback */
    [APP_STATE_PB_MAIN_IDX]              = {APP_STATE_PB_MAIN, appPbMainState, PB_MAIN_PLUG_CFG, "PB Main"},
    [APP_STATE_PB_VIDEO_IDX]             = {APP_STATE_PB_VIDEO, appPbVideoState, VIDEO_PLAYING_PLUG_CFG, "PB VIDEO"},
    [APP_STATE_PB_IMG_PROT_IDX]          = {APP_STATE_PB_IMG_PROT, appPbImgProtState, PB_PROT_PLUG_CFG, "PB PROTECT"},
    [APP_STATE_PB_IMG_DEL_IDX]           = {APP_STATE_PB_IMG_DEL, appPbImgDelState, PB_DEL_PLUG_CFG, "PB DEL"},

    /* usb */
    [APP_STATE_USB_CREATE_IDX]           = {APP_STATE_USB_CREATE, appUsbCreateState, USB_MSDC_PLUG_CFG, "Usb Create"},
    [APP_STATE_USB_MSDC_IDX]             = {APP_STATE_USB_MSDC, appUsbMsdcState, USB_MSDC_PLUG_CFG, "Usb MSDC"},
    [APP_STATE_USB_PCCAM_IDX]            = {APP_STATE_USB_PCCAM, appUsbPccamState,USB_MSDC_PLUG_CFG, "Usb PCCAM"},
    [APP_STATE_USB_BATTERY_CHARGING_IDX] = {APP_STATE_USB_BATTERY_CHARGING, appBatteryChargingState, USB_MSDC_PLUG_CFG, "Usb Battery Charging"},

    /* TV plug*/
    [APP_STATE_TV_PLUG_IDX]              = {APP_STATE_TV_PLUG, appTvPlugState, TVPLUG_STATE_PLUG_CFG, "TV plug"},

    /* HDMI plug*/
    #if (KIT_WITH_HDMI)
    [APP_STATE_HDMI_PLUG_IDX]            = {APP_STATE_HDMI_PLUG, appHDMIPlugState, HDMIPLUG_STATE_PLUG_CFG, "HDMI plug"},
    [APP_STATE_CEC_CAPABILITY_CTRL_IDX]  = {APP_STATE_CEC_CAPABILITY_CTRL, appCecCapabilityCtrlState, CEC_CAPABILITY_CTRL_STATE_PLUG_CFG, "Cec Capability Control"},
    #endif

    /* card plug */
    [APP_STATE_CARD_PLUG_IDX]            = {APP_STATE_CARD_PLUG, appCardPlugState, CARDPLUG_STATE_PLUG_CFG, "CARD plug"},

    /* Customer */
    [APP_STATE_CUSTOMER_IDX]             = {APP_STATE_CUSTOMER, appNullState, ALL_PLUG_DO_PROC, "CUSTOMER"},
    [APP_STATE_CUSTOMER_OTHERS]          = {0, NULL, 0, NULL}
  };

static appStateInfo_t stAppStateInfo =
{
	APP_STATE_NULL,             /* previous state */
	APP_STATE_NULL,             /* active state */
	APP_STATE_NULL,             /* next state */
	APP_STATE_PHASE_CLOSE,      /* state phase */
	STATE_PARAM_NORMAL_INIT,    /*state msg Param */
};   /* state param */

static pfpSATATE pActiveState = NULL;
extern UINT8 fileTypeSelect;

/**************************************************************************
 *                                                 *
 **************************************************************************/
 
#if TEST_STATE_CHANG_MSG
static UINT32 new_state_rec = 0xff;

 /*--------------------------------------------------------------------------*
 * Function name	: appMsgStateChange
 * Function	       	: Msg state change process
 * Return value   	: void
 * Parameter1    	: sel (UINT32)
 * Parameter2    	: msg (UINT32)
 * Parameter3    	: param (UINT32)
 * History	   :
 *                :  2018.06.04  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

void
appMsgStateChange(
    UINT32 sel,
	UINT32 newState,
	UINT32 param
)
{

#if defined(PRJ_JETDVR)||defined(PRJ_17DVR)||defined(PRJ_19DVR)
	UINT32 err = 0;
#endif
    switch(sel)
    {
        case STATE_CHANG_REQUEST:
            if(new_state_rec != newState)
                new_state_rec = newState;
            else
            {
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		appInfoiStateToPState(newState);
#endif
                DBG_PRINT("state: new state same but prenew not init finish[0x%x]\n",newState);
                return;
            }
            /*or new state*/
			profLogPrintf(0, "#P#STATE_CHG#N#%x", newState);
			profLogCallStack(0);
            sp5kHostMsgSend((APP_STATE_MSG_CHANG|newState), param);
        break;

        case STATE_CHANG_MSG_FINISH:
            /*state close*/
            	newState &= MAX_STATE_TAG;
       	    	stAppStateInfo.nextState =  newState;
        	stAppStateInfo.statePhase = APP_STATE_PHASE_CLOSE;
    	    	stAppStateInfo.stateMsgParam = param;
        	appBtnDisable(BTN_ALL);

        	#if HOST_DBG
        	appStateCtrlInfoPrint("state: state closing");
        	#endif

		profLogPrintf(0, "#S#STATE_CHG#N#%x", newState);

        	pActiveState(APP_STATE_MSG_CLOSE, STATE_PARAM_NORMAL_INIT);

        	if(STATE_GROUP(stAppStateInfo.activeState) != STATE_GROUP(stAppStateInfo.nextState))
        	{
        		appStateGroupCloseProc();
        	}

            /*state init*/
            pActiveState = appStateTbl[_stateCheck(newState)].pfpState;
            stAppStateInfo.prevState = stAppStateInfo.activeState;
        	stAppStateInfo.activeState = newState;
        	stAppStateInfo.nextState = APP_STATE_NULL;
         	stAppStateInfo.statePhase = APP_STATE_PHASE_READY;

        	#if HOST_DBG
        	appStateCtrlInfoPrint("state: state init");
        	#endif

        	pActiveState(APP_STATE_MSG_INIT,param);

			profLogPrintf(0, "#E#STATE_CHG#N#%x", newState);
        	if (stAppStateInfo.statePhase == APP_STATE_PHASE_READY)
        	{
        		appDevPlugStatusChange();
        	}

        	if(appPreviousStateGet() != APP_STATE_POWER_ON && appActiveStateGet() != APP_STATE_VIDEO_REC){
        	    appBtnEnable(BTN_ALL);
        	}
        break;
    }
#if defined(PRJ_JETDVR)||defined(PRJ_17DVR)||defined(PRJ_19DVR)
	err = sp5kOsEventFlagsSet(&qEvent_sig, INFO_QUEUEMSG_THREAD_STATECHANGE, TX_INT_ENABLE);
	HOST_ASSERT(err == SP5K_SUCCESS);
#endif
}
#endif

  /*--------------------------------------------------------------------------*
 * Function name	: _stateCheck
 * Function	       	: Check state index
 * Return value   	: state index (UINT32)
 * Parameter1    	: state (UINT32)
 * History	   :
 *                :  2018.06.04  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

UINT32
_stateCheck(
    UINT32 state
)
{
    #if STATE_CHECK_TIME
    static UINT32 time_tag = 0;
    time_tag = tmrTimeClockGet();
    #endif
    UINT32 state_idx = 0;

    for(state_idx=0; state_idx<APP_STATE_MAX_IDX; state_idx++)
    {
        if(state == appStateTbl[state_idx].appStateGroup)
        {
            #if STATE_CHECK_TIME
            printf("_stateCheck one time: %d us\n", tmrTimeClockGet() - time_tag);
            #endif
            return state_idx;
        }

    }
    DBG_ASSERT(state_idx < APP_STATE_MAX_IDX);
    return APP_STATE_CUSTOMER_OTHERS;
}

  /*--------------------------------------------------------------------------*
 * Function name	: _stateInitial 
 * Function	       	: state initial
 * Return value   	: void
 * Parameter1    	: next state (UINT32)
 * Parameter2    	: param (UINT32)
 * History	   :
 *                :  2018.06.04  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

void
_stateInitial (
 	UINT32 nextState,
 	UINT32 param
)
{
    DBG_PRINT("state: %s : init state:[0x%x] parm:[0x%x]\n",__FUNCTION__,nextState,param);

 	stAppStateInfo.statePhase = APP_STATE_PHASE_INIT;
    pActiveState = appStateTbl[_stateCheck(nextState)].pfpState;

	stAppStateInfo.activeState = nextState;
	stAppStateInfo.nextState = APP_STATE_NULL;
 	stAppStateInfo.statePhase = APP_STATE_PHASE_READY;
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	if(nextState == APP_STATE_POWER_ON)
		appInfoiStateToPState(nextState);
#endif
	#if HOST_DBG
	appStateCtrlInfoPrint("state: state init");
	#endif

	pActiveState(APP_STATE_MSG_INIT,param);

	/* process device plug change*/
	/* check state phase is ready, state may call appStateChange() right after receive APP_STATE_MSG_INIT*/
	if (stAppStateInfo.statePhase == APP_STATE_PHASE_READY)
	{
		appDevPlugStatusChange();
	}

}

  /*--------------------------------------------------------------------------*
 * Function name	: _stateClose 
 * Function	       	: state close
 * Return value   	: void
 * Parameter1    	: next state (UINT32)
 * Parameter2    	: param (UINT32)
 * History	   :
 *                :  2018.06.04  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
void
_stateClose (
 	UINT32 nextState,
 	UINT32 param
)
{

	stAppStateInfo.nextState =  nextState;
	stAppStateInfo.statePhase = APP_STATE_PHASE_CLOSE;
	stAppStateInfo.stateMsgParam = param;

	appBtnDisable(BTN_ALL);

	#if HOST_DBG
	appStateCtrlInfoPrint("state: state closing");
	#endif

	pActiveState(APP_STATE_MSG_CLOSE,param);

}

  /*--------------------------------------------------------------------------*
 * Function name	: _stateController
 * Function	       	: state controller
 * Return value   	: void
 * Parameter1    	: Msg (UINT32)
 * Parameter2    	: param (UINT32)
 * History	   :
 *                :  2018.06.04  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
void
_stateController(
	UINT32 msg,
 	UINT32 param
)
{
	stAppStateInfo.stateMsgParam = STATE_PARAM_NORMAL_INIT;
    _stateCheck(stAppStateInfo.nextState);

 	if (msg == APP_STATE_MSG_INIT)
 		_stateInitial(stAppStateInfo.nextState,param);
	else
		DBG_ASSERT(1);
}

  /*--------------------------------------------------------------------------*
 * Function name	: _stateChangeRequest
 * Function	       	: Request to state change
 * Return value   	: void
 * Parameter1    	: newState (UINT32)
 * Parameter2    	: param (UINT32)
 * History	   :
 *                :  2018.06.04  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

void
_stateChangeRequest(
 	UINT32 newState,
 	UINT32 param
)
{
 	stAppStateInfo.statePhase = APP_STATE_PHASE_CLOSE;
 	stAppStateInfo.prevState = stAppStateInfo.activeState;
 	stAppStateInfo.activeState = APP_STATE_NULL;
    stAppStateInfo.nextState = newState;

 	pActiveState = _stateController;
 	pActiveState(APP_STATE_MSG_INIT, param);
}

  /*--------------------------------------------------------------------------*
 * Function name	: _stateChange_HotKey
 * Function	       	: state change hot key
 * Return value   	: void
 * Parameter1    	: void
 * History	   :
 *                :  2018.06.04  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

void
_stateChange_HotKey(
	void
)
{
    DBG_PRINT("state: %s : format finish ... \n", __FUNCTION__);
    #if 1
	fileTypeSelect = 1;/*ez_menu for pb filetype */
	sp5kModeSet(SP5K_MODE_STANDBY);
	sp5kModeWait(SP5K_MODE_STANDBY);
    appStateChange(APP_STATE_MENU,STATE_PARAM_NORMAL_INIT);
    #else
	switch(gStillCB.cap.CurViewMode)
	{
		case PASM_PROGRAM:
    		appStateChange(APP_STATE_STILL_PREVIEW,STATE_PARAM_NORMAL_INIT);
			break;

		case VIDEO_MODE:
			appStateChange(APP_STATE_VIDEO_PREVIEW,STATE_PARAM_NORMAL_INIT);
			break;

		default :
			break;
	}
    #endif
}

  /*--------------------------------------------------------------------------*
 * Function name	: appStateCloseDone
 * Function	       	: State Close Done
 * Return value   	: void
 * Parameter1    	: void
 * History	   :
 *                :  2018.06.04  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

void
appStateCloseDone (
 	void
)
{
    #if TEST_STATE_CHANG_MSG
    return;
    #else
	if(STATE_GROUP(stAppStateInfo.activeState) != STATE_GROUP(stAppStateInfo.nextState))
	{
		appStateGroupCloseProc();
	}

 	stAppStateInfo.prevState = stAppStateInfo.activeState;
 	stAppStateInfo.activeState = APP_STATE_NULL;

	#if HOST_DBG
	appStateCtrlInfoPrint("state: state close done");
	#endif

	if(appPreviousStateGet() != APP_STATE_POWER_ON){
	    appBtnEnable(BTN_ALL);
	}
 	pActiveState =  _stateController;
 	pActiveState(APP_STATE_MSG_INIT,stAppStateInfo.stateMsgParam);
    #endif



}

  /*--------------------------------------------------------------------------*
 * Function name	: appPreviousStateGet
 * Function	       	: Get Previous State
 * Return value   	: Previous state (UINT32)
 * Parameter1    	: void
 * History	   :
 *                :  2018.06.04  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

UINT32
appPreviousStateGet (
 	void
)
{
 	return (stAppStateInfo.prevState);
}

  /*--------------------------------------------------------------------------*
 * Function name	: appActiveStateGet
 * Function	       	: Get Active State
 * Return value   	: Active state (UINT32)
 * Parameter1    	: void
 * History	   :
 *                :  2018.06.04  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

UINT32
appActiveStateGet (
 	void
)
{
	return (stAppStateInfo.activeState);
}

  /*--------------------------------------------------------------------------*
 * Function name	: appNextStateGet
 * Function	       	: Get Next State
 * Return value   	: Next state (UINT32)
 * Parameter1    	: void
 * History	   :
 *                :  2018.06.04  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

UINT32
appNextStateGet(
 	void
)
{
	return (stAppStateInfo.nextState);
}

  /*--------------------------------------------------------------------------*
 * Function name	: appActiveStateHotPlugCfgGet
 * Function	       	: Get Hot Plug config
 * Return value   	: config (UINT32)
 * Parameter1    	: void
 * History	   :
 *                :  2018.06.04  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
  
UINT32
appActiveStateHotPlugCfgGet (
 	void
)
{
    return (appStateTbl[_stateCheck(stAppStateInfo.activeState)].hotPlugCfg);
}

  /*--------------------------------------------------------------------------*
 * Function name	: appStateCtrlInitial
 * Function	       	: state controll initial
 * Return value   	: void
 * Parameter1    	: void
 * History	   :
 *                :  2018.06.04  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
  
void
appStateCtrlInitial (
 	void
)
{
	DBG_PRINT("state: appStateCtrlInitial() start...\n");
    DBG_ASSERT(APP_STATE_MAX_IDX==COUNT_OF(appStateTbl));
    #if STATE_CHECK_TIME
    _stateCheck(appStateTbl[APP_STATE_MAX_IDX-1].appStateGroup);
    #endif

	stAppStateInfo.prevState = APP_STATE_NULL;
 	stAppStateInfo.nextState = APP_STATE_POWER_ON;
 	stAppStateInfo.statePhase = APP_STATE_PHASE_INIT;

 	/* initialize active state to state controller */
	pActiveState = _stateController;
	pActiveState(APP_STATE_MSG_INIT,STATE_PARAM_NORMAL_INIT);
}

  /*--------------------------------------------------------------------------*
 * Function name	: appStateOnProc
 * Function	       	: state process
 * Return value   	: void
 * Parameter1    	: msg (UINT32)
 * Parameter2    	: param (UINT32)
 * History	   :
 *                :  2018.06.04  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

void
appStateOnProc(
	UINT32 msg,
 	UINT32 param
)
{
	DBG_ASSERT(pActiveState != NULL);
	pActiveState(msg,param);
}

/*
Please note the state will be changeed immediately after call appStateChange(),
therefor, please make sure no anyother instruction behind state all appStateChange()!!
*/

  /*--------------------------------------------------------------------------*
 * Function name	: appStateChange
 * Function	       	: state change
 * Return value   	: void
 * Parameter1    	: newState (UINT32)
 * Parameter2    	: param (UINT32)
 * History	   :
 *                :  2018.06.04  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

void
appStateChange(
	UINT32 newState,
	UINT32 param
)
{
	printf("appStateChange to 0x%X\n", newState);
	callStack();

#if TEST_STATE_CHANG_MSG /*test for state change msg*/
    appMsgStateChange(STATE_CHANG_REQUEST, newState, param);
#else
    #if SAME_STATE_GROUP_DIRECT_SWITCH
	if ((STATE_GROUP(stAppStateInfo.activeState) == STATE_GROUP(newState)))    /* same state dont change */
        #if 0
		||(IS_TV_PLUG_GROUP(stAppStateInfo.activeState))
		||(IS_HDMI_PLUG_GROUP(stAppStateInfo.activeState))
		|| (IS_CARD_PLUG_GROUP(stAppStateInfo.activeState)) )
		#endif
	{
        DBG_PRINT("state: Same State Group change [0x%x]->[0x%x]\n",stAppStateInfo.activeState,newState);
		_stateChangeRequest(newState,param);
	}
	else
	{
		DBG_PRINT("state: Diff State Group change [0x%x]->[0x%x]\n",STATE_GROUP(stAppStateInfo.activeState),STATE_GROUP(newState));
		_stateClose(newState,param);
	}
    #else
    /* every state change */
    _stateClose(newState,param);
    #endif
#endif
}

  /*--------------------------------------------------------------------------*
 * Function name	: appUiModeChange
 * Function	       	: UI Mode change
 * Return value   	: void
 * Parameter1    	: void
 * History	   :
 *                :  2018.06.04  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

void
appUiModeChange(void)
{

	if(IS_PLAYBACK_GROUP(stAppStateInfo.activeState))
	{
		_stateChange_HotKey();
	}
	else
	{
		appStateChange(APP_STATE_PB_MAIN,STATE_PARAM_NORMAL_INIT);
	}

}
  
  /*--------------------------------------------------------------------------*
 * Function name	: appUiModeSel
 * Function	       	: UI Mode select
 * Return value   	: void
 * Parameter1    	: void
 * History	   :
 *                :  2018.06.04  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

void appUiModeSel(void)
{
	if(IS_PLAYBACK_GROUP(stAppStateInfo.activeState))
	{
		_stateChange_HotKey();
	}
    else
    {
        switch(gStillCB.cap.CurViewMode)
    	{
    		case PASM_PROGRAM:
        		appStateChange(APP_STATE_STILL_PREVIEW,STATE_PARAM_NORMAL_INIT);
    			break;

    		case VIDEO_MODE:
    			appStateChange(APP_STATE_VIDEO_PREVIEW,STATE_PARAM_NORMAL_INIT);
    			break;

    		default :
    			break;
    	}
    }
}

  /*--------------------------------------------------------------------------*
 * Function name	: appStatePhaseGet
 * Function	       	: Get state phase
 * Return value   	: void
 * Parameter1    	: state phase (UINT32)
 * History	   :
 *                :  2018.06.04  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
UINT32 appStatePhaseGet(void)
{
	return stAppStateInfo.statePhase;
}

  /*--------------------------------------------------------------------------*
 * Function name	: appStateCtrlInfoPrint
 * Function	       	: printf for state controller info
 * Return value   	: void
 * Parameter1    	: state title (char pointer)
 * History	   :
 *                :  2018.06.04  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
void
appStateCtrlInfoPrint(
	char *title
)
{
 	printf("\n-------------[%s]-----------------------\n",title);
    printf("        Previous State =0x%x (%s)\n",stAppStateInfo.prevState, appStateTbl[_stateCheck(stAppStateInfo.prevState)].name);
    printf("        Active State   =0x%x (%s)\n",stAppStateInfo.activeState, appStateTbl[_stateCheck(stAppStateInfo.activeState)].name);
    printf("        Next State     =0x%x (%s)\n",stAppStateInfo.nextState, appStateTbl[_stateCheck(stAppStateInfo.nextState)].name);
    printf("        Device Cfg     =0x%x (%s)\n",appStateTbl[_stateCheck(stAppStateInfo.activeState)].hotPlugCfg, appStateTbl[_stateCheck(stAppStateInfo.activeState)].name);
    printf("        State Phase    =%d (0-init, 1-ready, 2: close)\n",stAppStateInfo.statePhase);
 	printf("--------------------------------------------------------\n\n");
}

  /*--------------------------------------------------------------------------*
 * Function name	: appStateCustomerReg
 * Function	       	: state Reg for customer
 * Return value   	: void
 * Parameter1    	: RegState (pointer of pfpSATATE)
 * History	   :
 *                :  2018.06.04  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

void
appStateCustomerReg(
    pfpSATATE RegState
)
{
    appStateTbl[APP_STATE_CUSTOMER_IDX].pfpState = RegState;
}
  /*--------------------------------------------------------------------------*
 * Function name	: appStateReg
 * Function	       	: state Reg 
 * Return value   	: void
 * Parameter1    	: eStateGroupIdx (pointer of appStateGroupIdx_e)
 * Parameter2    	: pfpCallBackFunc (pointer of pfpSATATE)
 * History	   :
 *                :  2018.06.04  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

void appStateReg(appStateGroupIdx_e eStateGroupIdx, pfpSATATE pfpCallBackFunc){
    appStateTbl[eStateGroupIdx].pfpState = pfpCallBackFunc;
}
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
  /*--------------------------------------------------------------------------*
 * Function name	: jet_print_all_app_state
 * Function	       	: printf for jet state info 
 * Return value   	: void
 * Parameter1    	: void
 * History	   :
 *                :  2018.06.04  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

void jet_print_all_app_state(void) /*SC add*/
{
	UINT8 i=0;
	for(i= APP_STATE_NULL_IDX; i<APP_STATE_MAX_IDX; i++){
		if(appStateTbl[i].pfpState != NULL)
			printf("%s = 0x%04x\n", appStateTbl[i].name, appStateTbl[i].appStateGroup);
	}
}
#endif
