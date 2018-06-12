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
 *  File name	: 	app_state_postproc.c
 *  Function 	: 	state post process		 	
 *  First editor	: 	
 *  First date  	: 	
 *  History		:
 *  2018.06.05  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
#define HOST_DBG 1
#include "app_com_def.h"
#include "app_com_api.h"

#include "app_playback_def.h"
#include "app_view_param_def.h"
#include "app_view_param.h"
#include "app_still.h"
#include "app_dpof.h"
#include "app_video.h"
#include "app_view_osd.h"
#include "app_osd_api.h"
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/**************************************************************************
 *                         L O C A L    D A T A                         *
 **************************************************************************/

/*--------------------------------------------------------------------------*
 * Function name	: _CheckSavingUiData
 * Function	       	: check save UI data
 * Return value   	: void
 * Parameter1    	: void
 * History	   :
 *                :  2018.06.05  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

void _CheckSavingUiData(
    void
)
{
#if 0
    UINT32 next_state = appNextStateGet();

    if (!IS_VIDEO_GROUP(next_state)
        && !IS_AUDIO_GROUP(next_state)
        && !IS_VIEW_GROUP(next_state)) {
        /*UiParamViewFlush();*/

        if (!IS_USB_GROUP(next_state)) {
            sp5kAeModeSet(SP5K_AE_MODE_OFF);
            sp5kAwbModeSet(SP5K_AWB_MODE_OFF);
        }
    }

    if (IS_AUDIO_GROUP(next_state)) {
        sp5kAeModeSet(SP5K_AE_MODE_OFF);
        sp5kAwbModeSet(SP5K_AWB_MODE_OFF);
    }
#endif
}

/*--------------------------------------------------------------------------*
 * Function name	: appGroupViewPostProc
 * Function	       	: Group view post process
 * Return value   	: void
 * Parameter1    	: void
 * History	   :
 *                :  2018.06.05  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

void appGroupViewPostProc(
    void
)
{
    DBG_PRINT("state pos: %s enter...\n",__FUNCTION__);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
    if(IS_CARD_EXIST)
    {
    	LED_NORMAL_ON;
    }
    else
    {
	GREEN_LED_OFF;
	RED_LED_ON;
    }
#else
    LED_NORMAL_ON;
#endif
    _CheckSavingUiData();
    appStill_SetFlash(0);
    APP_OSD_REFRESH_OFF;

    appViewOsd_ZoomBarDestroy();
    appViewOsd_HistogramDestroy();
    appStillWaitJobFinish();
}

/*--------------------------------------------------------------------------*
 * Function name	: appGroupVideoPostProc
 * Function	       	: Group Video post process
 * Return value   	: void
 * Parameter1    	: void
 * History	   :
 *                :  2018.06.05  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

void appGroupVideoPostProc(
    void
)
{
    DBG_PRINT("state null: %s enter...\n",__FUNCTION__);
#if !defined(PRJ_17DVR)&&!defined(PRJ_19DVR)
    LED_NORMAL_ON;
#endif
    _CheckSavingUiData();
    if (!gVideoCB.isAutoPowerOffOn) {
        appPowerSavingEnableSet(TRUE);
        gVideoCB.isAutoPowerOffOn=1;
    }

    APP_OSD_REFRESH_OFF;
    appViewOsd_ZoomBarDestroy();
    appViewOsd_HistogramDestroy();
}

/*--------------------------------------------------------------------------*
 * Function name	: appGroupAudioPostProc
 * Function	       	: Group Audio post process
 * Return value   	: void
 * Parameter1    	: void
 * History	   :
 *                :  2018.06.05  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

void appGroupAudioPostProc(
    void
)
{
    DBG_PRINT("state null: %s enter...\n",__FUNCTION__);
    if (appNextStateGet() == APP_STATE_PB_MAIN) {
        sp5kModeSet(SP5K_MODE_STANDBY);
        sp5kModeWait(SP5K_MODE_STANDBY);
        appJpgFileDraw(SP5K_RES_TYPE_JPG, RES_JPG_BLACK, 0, 0, 0, 0);
    }
#if !defined(PRJ_17DVR)&&!defined(PRJ_19DVR)
    LED_NORMAL_ON;
#endif
    _CheckSavingUiData();
}
/*--------------------------------------------------------------------------*
 * Function name	: appGroupPlaybackPostProc
 * Function	       	: Group Playback post process
 * Return value   	: void
 * Parameter1    	: void
 * History	   :
 *                :  2018.06.05  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

void appGroupPlaybackPostProc(
    void
)
{
    pbMainCB_t *ppbMainCb;
    DBG_PRINT("state null: %s enter...\n",__FUNCTION__);

    ppbMainCb = appPb_MainCbPtrGet();
    if (ppbMainCb->closeLensTmrId != TIMER_NULL) {
        DBG_PRINT("state null: %s: Clearing TMR for closeLens \n", __FUNCTION__);
        appTimerClear(&ppbMainCb->closeLensTmrId);
    }
}

/*--------------------------------------------------------------------------*
 * Function name	: appGroupUsbPostProc
 * Function	       	: Group USB post process
 * Return value   	: void
 * Parameter1    	: void
 * History	   :
 *                :  2018.06.05  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
void appGroupUsbPostProc(
    void
)
{
	DBG_PRINT("state null: %s enter...\n",__FUNCTION__);
}
/*--------------------------------------------------------------------------*
 * Function name	: appGroupCalibrationPostProc
 * Function	       	: Group Calibration post process
 * Return value   	: void
 * Parameter1    	: void
 * History	   :
 *                :  2018.06.05  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
void appGroupCalibrationPostProc(
    void
)
{
	DBG_PRINT("state null: %s enter...\n",__FUNCTION__);
}

/*--------------------------------------------------------------------------*
 * Function name	: appStateGroupCloseProc
 * Function	       	: state Group close process
 * Return value   	: void
 * Parameter1    	: void
 * History	   :
 *                :  2018.06.05  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
void appStateGroupCloseProc(
    void
)
{
    switch (STATE_GROUP(appActiveStateGet()))
    {
        case APP_STATE_GROUP_VIEW:
            appGroupViewPostProc();
            break;
        case APP_STATE_GROUP_VIDEO:
            appGroupVideoPostProc();
            break;
        case APP_STATE_GROUP_PLAYBACK:
            appGroupPlaybackPostProc();
            break;
        case APP_STATE_GROUP_USB:
            appGroupUsbPostProc();
            break;
        case APP_STATE_GROUP_CALIB:
            appGroupCalibrationPostProc();
            break;
        default:
            break;
    }
}

