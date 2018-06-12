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
 *  File name	: 	app_state_preproc.c
 *  Function 	: 	state prepost process		 	
 *  First editor	: 	
 *  First date  	: 	
 *  History		:
 *  2018.06.05  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

#define HOST_DBG 1
#include "app_com_def.h"
#include "app_com_api.h"

#include "sp5k_global_api.h"
#include "app_playback_def.h"
#include "app_tvlcd.h"
#include "app_still.h"
#include "app_view_osd.h"
#include "app_video.h"
#include "app_audio.h"
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
 * Function name	: appPowerSavingDispOff
 * Function	       	: Power  Saving Display off
 * Return value   	: void
 * Parameter1    	: void
 * History	   :
 *                :  2018.06.05  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
void
appPowerSavingDispOff(
	void
)
{
	if(IS_LCD_OUT) 
	{
		LCD_BACKLIGHT_OFF;
		LED_FLICKER_LCD_OFF;
	}		
}

  /*--------------------------------------------------------------------------*
 * Function name	: appGroupViewProc
 * Function	       	: Group View process
 * Return value   	: appState_msg_flow_e (APPSTATE_MSG_DEFAULT/APPSTATE_MSG_IGNORE)
 * Parameter1    	: msg (UINT32)
 * Parameter2    	: param (UINT32)
 * History	   :
 *                :  2018.06.05  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

appState_msg_flow_e
appGroupViewProc(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 ret = APPSTATE_MSG_DEFAULT;

	appStillBackground(msg, param);
		
	switch( msg )
	{
	case APP_UI_MSG_POWER_SAVE_TIME_OUT:
		DBG_PRINT("state pre: APP_UI_MSG_POWER_SAVE_TIME_OUT....\n");			
		appPowerSavingDispOff();
		break;
	case APP_UI_MSG_BATT_LEVEL_CHANGE:
		DBG_PRINT("state pre: APP_UI_MSG_BATT_LEVEL_CHANGE(%d)\n",param);
		if (!IS_MENU_STATE(appActiveStateGet()) 
			&& (gStillCB.osd != OSD_VIEW_NONE) 
			&& (gStillCB.osd != OSD_VIEW_GRID) )
		{
			appViewOsd_BatteryDraw(TRUE,param);
		}
		ret = APPSTATE_MSG_IGNORE;
		break;
	default:
		break;
	}
	
	if (ret == APPSTATE_MSG_IGNORE)
	{
		DBG_PRINT("state pre: Msg[0x%x] is ignored after %s \n",msg,__FUNCTION__);
	}

	return ret;
}
  /*--------------------------------------------------------------------------*
 * Function name	: appGroupVideoProc
 * Function	       	: Group Video process
 * Return value   	: appState_msg_flow_e (APPSTATE_MSG_DEFAULT/APPSTATE_MSG_IGNORE)
 * Parameter1    	: msg (UINT32)
 * Parameter2    	: param (UINT32)
 * History	   :
 *                :  2018.06.05  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
appState_msg_flow_e
appGroupVideoProc(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 ret = APPSTATE_MSG_DEFAULT;

	appStillBackground(msg, param);

	switch( msg )
	{
		case APP_UI_MSG_POWER_SAVE_TIME_OUT:
			appPowerSavingDispOff();
			break;
		case APP_UI_MSG_BATT_LEVEL_CHANGE:
			if (!IS_MENU_STATE(appActiveStateGet()) && (gVideoCB.osd != OSD_VIEW_NONE) )
			{
				appViewOsd_BatteryDraw(TRUE,param);
			}
			ret = APPSTATE_MSG_IGNORE;
			break;
		default:
			break;
	}
	
	if (ret == APPSTATE_MSG_IGNORE)
	{
		DBG_PRINT("state pre: Msg[0x%x] is ignored after %s \n",msg,__FUNCTION__);
	}

	return ret;
}


  /*--------------------------------------------------------------------------*
 * Function name	: appGroupAudioProc
 * Function	       	: Group Audio process
 * Return value   	: appState_msg_flow_e (APPSTATE_MSG_DEFAULT/APPSTATE_MSG_IGNORE)
 * Parameter1    	: msg (UINT32)
 * Parameter2    	: param (UINT32)
 * History	   :
 *                :  2018.06.05  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

appState_msg_flow_e
appGroupAudioProc(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 ret = APPSTATE_MSG_DEFAULT;
	
	switch( msg )
	{
		case APP_UI_MSG_POWER_SAVE_TIME_OUT:
			DBG_PRINT("state pre: APP_UI_MSG_POWER_SAVE_TIME_OUT....\n");
			//do power saving process
			//exception : audio recording
			appPowerSavingDispOff();
			break;
		case APP_UI_MSG_LENS_CLOSE:
			//close lens 
			//exception : during audio recording
			break;
		case APP_UI_MSG_BATT_LEVEL_CHANGE:
			DBG_PRINT("state pre: APP_UI_MSG_BATT_LEVEL_CHANGE(%d)\n",param);
			//update battery icon
			ret = APPSTATE_MSG_IGNORE;
			break;
	}	
	if (ret == APPSTATE_MSG_IGNORE)
	{
		DBG_PRINT("state pre: Msg[0x%x] is ignored after %s \n",msg,__FUNCTION__);
	}

	return ret;
}
  
  /*--------------------------------------------------------------------------*
 * Function name	: appGroupPlaybackProc
 * Function	       	: Group Playback process
 * Return value   	: appState_msg_flow_e (APPSTATE_MSG_DEFAULT/APPSTATE_MSG_IGNORE)
 * Parameter1    	: msg (UINT32)
 * Parameter2    	: param (UINT32)
 * History	   :
 *                :  2018.06.05  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
appState_msg_flow_e
appGroupPlaybackProc(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 ret = APPSTATE_MSG_DEFAULT;
	pbMainCB_t *pmainCb;
	pbImgCb_t *pimgCb;

	pmainCb = appPb_MainCbPtrGet();
	pimgCb = appPb_ImageCbPtrGet();


	switch( msg )
	{
		case SP5K_MSG_STILLPLAY_DONE:
			ret = appPbStillPlayDoneParse(param);
			if (ret & PB_DONE_ERROR) 
			{
                HOST_PROF_LOG_PRINT(LEVEL_ERROR,"state pre: PB_DONE ERROR (0x%x)", ret);
				gpbImgCb.isFileErr = TRUE;
				gpbImgCb.fileType = PB_FT_UNKNOWN;
				gpbImgCb.dcfAttr.fileType=0;
				gpbMainCb.err = PB_ERR_FILE_ERROR;
				gpbMainCb.playStat = PB_PLAY_STAT_STANDBY;
			}
			ret = APPSTATE_MSG_DEFAULT;
			break;

		case SP5K_MSG_AE_READY:
		case SP5K_MSG_AWB_READY:
			//ret = APPSTATE_MSG_IGNORE;
			break;
		case APP_UI_MSG_POWER_SAVE_TIME_OUT:
			appPowerSavingDispOff();
			break;
		case SP5K_MSG_TIMER:
			break;
		case APP_UI_MSG_LENS_CLOSE:
			break;
		case SP5K_MSG_DISK_MOUNT_COMPLETE:
		case SP5K_MSG_DISK_MOUNT_FAIL:
			DBG_PRINT("state pre: Preprocess [SP5K_MSG_DISK_MOUNT_COMPLETE/_FAIL] ....\n");
			if (pmainCb->mainStat!=PB_MAIN_STAT_C2C_SUSPEND) 
			{
				ret = APPSTATE_MSG_IGNORE;
			}
			break;

		case SP5K_MSG_MODE_READY:
			DBG_PRINT("state pre: Preprocess [SP5K_MSG_MODE_READY] ...param=%x \n",param);
			if (param==SP5K_MODE_STANDBY && FALSE==appPb_IsViewIdle()) 
			{
				sp5kHostMsgSend(APP_UI_MSG_PB_INIT, 0);
			} 
			else if (param==SP5K_MODE_VIDEO_PLAYBACK 
				&&pmainCb->playStat==PB_PLAY_STAT_VIDEO_TITLE_PLAYING)
			{   
				printf("state pre: PB_PreProc: Video QV ready !!\n");
				pmainCb->playStat = PB_PLAY_STAT_STANDBY;
				pimgCb->isThumbImgOK = TRUE;
				pmainCb->isPlayDone = TRUE;
				sp5kHostMsgSend(APP_UI_MSG_PB_VIDEO_TITLE_DONE, 0);
				ret = APPSTATE_MSG_IGNORE;
			}
			break;

		case APP_UI_MSG_BATT_LEVEL_CHANGE:
			DBG_PRINT("state pre: APP_UI_MSG_BATT_LEVEL_CHANGE(%d)\n",param);
			ret = APPSTATE_MSG_IGNORE;			
			break;
	}		
	if (ret == APPSTATE_MSG_IGNORE)
	{
		DBG_PRINT("state pre: Msg[0x%x] is ignored after %s \n",msg,__FUNCTION__);
	}

	return ret;
}
  /*--------------------------------------------------------------------------*
 * Function name	: appGroupUsbProc
 * Function	       	: Group USB process
 * Return value   	: appState_msg_flow_e (APPSTATE_MSG_DEFAULT/APPSTATE_MSG_IGNORE)
 * Parameter1    	: msg (UINT32)
 * Parameter2    	: param (UINT32)
 * History	   :
 *                :  2018.06.05  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
appState_msg_flow_e
appGroupUsbProc(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 ret = APPSTATE_MSG_DEFAULT;
		
	switch( msg )
	{
		case APP_UI_MSG_BATT_LEVEL_CHANGE:
			ret = APPSTATE_MSG_IGNORE;
			break;
		default:
			break;
	}

	if (ret == APPSTATE_MSG_IGNORE)
	{
		DBG_PRINT("state pre: Msg[0x%x] is ignored after %s \n",msg,__FUNCTION__);
	}

	return ret;
}
  
  /*--------------------------------------------------------------------------*
 * Function name	: appGroupCalibrationProc
 * Function	       	: Group Calibration process
 * Return value   	: appState_msg_flow_e (APPSTATE_MSG_DEFAULT/APPSTATE_MSG_IGNORE)
 * Parameter1    	: msg (UINT32)
 * Parameter2    	: param (UINT32)
 * History	   :
 *                :  2018.06.05  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
appState_msg_flow_e
appGroupCalibrationProc(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 ret = APPSTATE_MSG_DEFAULT;
		
	switch( msg )
	{
		case APP_UI_MSG_BATT_LEVEL_CHANGE:
			ret = APPSTATE_MSG_IGNORE;
			break;
		default:
			break;
	}

	if (ret == APPSTATE_MSG_IGNORE)
	{
		DBG_PRINT("state pre: Msg[0x%x] is ignored after %s \n",msg,__FUNCTION__);
	}

	return ret;
}		

