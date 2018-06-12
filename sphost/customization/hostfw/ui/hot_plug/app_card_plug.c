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
#include "app_tvlcd.h"
#include "sp5k_disk_api.h"
#include "app_dev_plug.h"
#include "app_user_setting.h"

static UINT32 isDiskMount = FALSE;

/*-------------------------------------------------------------------------
 *  File Name : appCardPlugStateInit
 *------------------------------------------------------------------------*/
void
appCardPlugStateInit(
	void
)

{
	if ((!IS_CARD_EXIST) && (appActiveDiskGet() == SP5K_DRIVE_NAND)) {
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	      
		if(IS_MENU_STATE(appPreviousStateGet()) || IS_TV_PLUG_GROUP(appPreviousStateGet()))
#else	
        if(IS_MENU_STATE(appPreviousStateGet()) || IS_PLAYBACK_GROUP(appPreviousStateGet()))
#endif
            appStateChange(APP_STATE_VIDEO_PREVIEW, STATE_PARAM_NORMAL_INIT);
 	}
	else {
#if SP5K_DISK_SPI
		if(IS_CARD_EXIST)
		{
			appDiskMount(SP5K_DRIVE_SD);
			isDiskMount = TRUE;
		}
		else
		{
			if(IS_PLAYBACK_GROUP(appPreviousStateGet()) && pUiSetting->pbfileType == PB_FILETYPE_IMAGE)
			{
				sp5kModeSet(SP5K_MODE_STANDBY);
				sp5kModeWait(SP5K_MODE_STANDBY);
			}
			appHotPlugNextStChange();
		}
#else
		if(IS_PLAYBACK_GROUP(appPreviousStateGet()) && pUiSetting->pbfileType == PB_FILETYPE_IMAGE)
		{
			sp5kModeCfgSet(SP5K_MODE_CFG_STANDBY_DUP, 1);
			sp5kModeSet(SP5K_MODE_STANDBY);
			sp5kModeWait(SP5K_MODE_STANDBY);
		}
		if (IS_CARD_EXIST) {
			appDiskUnMount(SP5K_DRIVE_NAND);
			appDiskMount(SP5K_DRIVE_SD);
		}
		else {
			appDiskMount(SP5K_DRIVE_NAND);
		}
		isDiskMount = TRUE;
#endif

 	}

}

/*-------------------------------------------------------------------------
 *  File Name : appCardPlugState
 *------------------------------------------------------------------------*/
void
appCardPlugState(
	UINT32 msg,
	UINT32 param
)
{
	DBG_PRINT("cardplug: %s : [0x%x] [0x%x]\n",__FUNCTION__,msg,param);

	switch (msg) {
	case APP_STATE_MSG_INIT:
		RPrintf("appCardPlugState");
		isDiskMount = FALSE;
		appCardPlugStateInit();
		break;

	case APP_STATE_MSG_CLOSE:
		appStateCloseDone();
		break;
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	case SP5K_MSG_DISK_MOUNT_FAIL:	
	case SP5K_MSG_DISK_ERROR:		
		isDiskMount = FALSE;	
	case SP5K_MSG_DISK_MOUNT_COMPLETE:		
		if(IS_TV_PLUG_GROUP(appPreviousStateGet()) ||IS_PLAYBACK_GROUP(appPreviousStateGet()))			
			appStateChange(APP_STATE_VIDEO_PREVIEW,STATE_PARAM_CARD_PLUG);		
		else			
			appStateChange(appPreviousStateGet(),STATE_PARAM_CARD_PLUG);
		break;
#else
	case SP5K_MSG_DISK_MOUNT_COMPLETE:
	case SP5K_MSG_DISK_MOUNT_FAIL:
	case SP5K_MSG_DISK_ERROR:
		isDiskMount = FALSE;
		appStateChange(appPreviousStateGet(),STATE_PARAM_CARD_PLUG);
		break;
#endif
	default:
		break;
	}
}

