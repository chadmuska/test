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
#define HOST_DBG 0
#include "sp5k_capture_api.h"
#include "sp5k_disk_api.h"

#include "app_com_def.h"
#include "app_com_api.h"
#include "app_still.h"
#include "app_view_param.h"
#include "app_disk.h"
#include "app_view_osd.h"
#include "app_lens_api.h"
#include "app_zoom_api.h"
#include "app_audio.h"
#include "app_led.h"
#include "app_strobe.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct {
	UINT32 imgId;
	UINT32 svPrg;
	UINT32 cluSzInByte;
	UINT32 dskFree;
	UINT32 estFileSize;
} StillBackgroundCB_t;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static StillBackgroundCB_t gSBG = {
	.imgId = 0,
	.svPrg = 0,
	.cluSzInByte = 16384,
	.dskFree = 0,
	.estFileSize = 0,
};
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT32 CapWithFaceCheck;

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/*-------------------------------------------------------------------------
 *	Function Name:  appStillFlashRecharge()
 *	Description: The flash/strobe power will decrease with time, we need to
 				 recharge it every a period. However, the lens moving and
 				 strobe charging can not work the same time.
 *------------------------------------------------------------------------*/
void
appStillFlashRecharge(
	UINT32 param
)
{
	if(gStillCB.flashChargeMonitorTimer == param)
	{
		appTimerClear(&gStillCB.flashChargeMonitorTimer);
		if(gStillCB.flashMode != UI_FLASH_OFF)
		{
			if(appLensQuery(LENS_QUERY_MOTION_STATE))
			{
				/* when the lens is moving, we retry later */
				gStillCB.flashChargeMonitorTimer = appTimerSet(10, "Flash ReCharge");
			}
			else
			{
				/* start recharge and wait messge "APP_UI_MSG_STROBE_CHARGE_READY" */
				DBG_PRINT("bg: %s: STROBE_RECHARGE\n", __FUNCTION__);
				appStrobeChargeStart();
				gStillCB.flashCharging = 1;
			}
		}
	}
}

/*-------------------------------------------------------------------------
 *	Function Name:  appStillBackground()
 *	Description:	It will effect for all still/video group states.
 *					This will be called in appGroupViewProc().
 *					The storing/saving messages may vary with different
 *					memory card, capture size and zoom ratio. So we put
 *					it in group preproces to catch all informations.
 *
 *					1). Here only "memory card full" will stop the burst capture.
 *					    The other stop case will maintain on burst state.
 *
 *					2). We assume the IMAGE_ID will always advance
 * 						SAVE_PROGRESS.
 *
 *					3). Some other functions will also be also maitained here.
 *						osd (hitogram, zoom-bar), background AE for menu state and
 *						face track disable for scene menu. Lens messags share with
 *						video group.
 *------------------------------------------------------------------------*/
void
appStillBackground(
	UINT32 msg,
	UINT32 param
)
{

	UINT32 realFileSz = 0;

	switch(msg) {
	/* monitor flash/strobe */
	case APP_UI_MSG_STROBE_CHARGE_READY:
		DBG_PRINT("bg: %s: STROBE_CHARGE_READY!\n", __FUNCTION__);
		appStrobeChargeStop();
		gStillCB.flashCharging=0;
		if(gStillCB.flashChargeMonitorTimer!=TIMER_NULL)
		{
			appTimerClear(&gStillCB.flashChargeMonitorTimer);
		}
		gStillCB.flashChargeMonitorTimer = appTimerSet(3*60000, "Flash ReCharge");
		GREEN_LED_ON;
		break;

	/* monitor key press */
	case APP_KEY_PRESS_S1:
		gStillCB.s1press=1;
		break;
	case APP_KEY_PRESS_S2:
		gStillCB.s2press=1;
		break;
	case APP_KEY_RELEASE_S1:
		gStillCB.s1press=0;
		break;
	case APP_KEY_RELEASE_S2:
		gStillCB.s2press=0;
		break;

	/* backgroud store */
	case SP5K_MSG_STILL_CAPTURE_IMAGE_ID:
		/* record the raw image number and check the disk size */
		gSBG.imgId++;
		if ((gSBG.imgId - gSBG.svPrg + 1)*gSBG.estFileSize > gSBG.dskFree )
		{
			sp5kStillCapAbort();
			DBG_PRINT("bg: %s: memory full\n", __FUNCTION__);
		}
		if(gStillCB.diskSpaceInImgId>gStillCB.estFileSize)
		{
			gStillCB.diskSpaceInImgId -= gStillCB.estFileSize;
		}
		DBG_PRINT("bg: %s: imgId=%d, svPrg=%d para:0x%x\n", __FUNCTION__, gSBG.imgId, gSBG.svPrg,param);
		break;
	case SP5K_MSG_STILL_SAVE_PROGRESSIVE:
		LED_FLICKER_IMAGE_SAVING
		/* record the save file number and calculate the disk free size */
		gSBG.svPrg++;
		/* Sync disk free size for each capture complete */
		appStillRemainNumGet();
		/*
		 * All read images have become vlc files, so we can estimate
		 * the rest space and show it on OSD.
		 */
		if((gSBG.imgId == gSBG.svPrg) && (appActiveStateGet()== APP_STATE_STILL_PREVIEW))
		{
			sp5kHostMsgSend(APP_UI_MSG_VIEW_DISKSIZE_READY,0);
		}
		if(gStillCB.estFileSize > realFileSz)
		{
			gStillCB.diskSpaceInImgId += (gStillCB.estFileSize - realFileSz);
		}
		else
		{
			gStillCB.diskSpaceInImgId -= (realFileSz - gStillCB.estFileSize);
		}
		DBG_PRINT("bg: %s: est=%d, real=%d\n", __FUNCTION__, gSBG.estFileSize, realFileSz);
		DBG_PRINT("bg: %s: real/est = [ %d%% ]\n", __FUNCTION__, gSBG.estFileSize>0? realFileSz*100/gSBG.estFileSize:0);
		break;
	case SP5K_MSG_STILL_STORAGE_FULL:
		DBG_PRINT("%s: STORAGE_FULL\n",__FUNCTION__);
		sp5kStillCapAbort();
		break;
	case SP5K_MSG_STILL_CONFIRM_STORE:
		DBG_PRINT("bg: %s: CONFIRM_STORE\n",__FUNCTION__);
		CapWithFaceCheck =FALSE;
		break;
	case SP5K_MSG_STILL_STORE_COMPLETE:
        //MUST return to normal LED condition after snap action.
		LED_NORMAL_ON
		DBG_PRINT("bg: %s: STORE_COMPLETE,para:0x%x\n",__FUNCTION__,param);
        appAePreviewRestore();
		if (appActiveStateGet()== APP_STATE_STILL_PREVIEW)
		{
			sp5kHostMsgSend(APP_UI_MSG_VIEW_DISKSIZE_READY,0);
		}

		#if !defined(SPCA6350_DEFAWB_MOVEOUT)
		SP_AwbPointBufDump();
		#endif

		break;
	/* osd and timer*/
	case SP5K_MSG_TIMER:
		appViewOsd_ZoomBarProcess(param);
		appViewOsd_HistogramProcess(param);
		appStillFlashRecharge(param);
		break;
	/* ae awb message */
	case SP5K_MSG_AE_READY:
		break;
	case SP5K_MSG_AWB_READY:
		#if !defined(SPCA6350_DEFAWB_MOVEOUT)
		SP_AwbGainRead();
		#endif
		break;

	default:
		break;
	}
}

/*-------------------------------------------------------------------------
 *	Function Name:  appStillBackgroundInit()
 *	Description:	Init it while into still view from other state group
 *------------------------------------------------------------------------*/
void
appStillBackgroundInit(
	UINT32 cluSzInByte,
	UINT32 dskFree,
	UINT32 estFileSize
)
{
	int spaceInKb;

	spaceInKb = dskFree - APP_DISK_DPOF_RSVD_SIZE_KB;
	gSBG.imgId = 0;
	gSBG.svPrg = 0;
	gSBG.cluSzInByte = cluSzInByte==0? 16384 : cluSzInByte;
	gSBG.dskFree = spaceInKb>0? spaceInKb : 0;
	gSBG.estFileSize = estFileSize;
}

/*-------------------------------------------------------------------------
 *	Function Name:  appStillBackgroundSet()
 *	Description:	Set it before into capture/burst.
 *------------------------------------------------------------------------*/
void
appStillBackgroundSet(
	UINT32 estFileSize
)
{
	gSBG.estFileSize = estFileSize;
	gSBG.imgId = gSBG.imgId - gSBG.svPrg;
	gSBG.svPrg = 0;
}

/*-------------------------------------------------------------------------
 *	Function Name:  appStillBackgroundGetFree()
 *	Description:	This can let still view state calculate the image number.
 *					It will be faster than "store complete message". The Unit
 *					is KB.
 *------------------------------------------------------------------------*/
UINT32
appStillBackgroundGetFree(UINT32* dskFree)
{
	if(gSBG.imgId == gSBG.svPrg)
	{
		*dskFree = gSBG.dskFree;
		return SUCCESS;
	}
	else
	{
		*dskFree =gSBG.dskFree;
		return FAIL;
	}
}

/*-------------------------------------------------------------------------
 *	Function Name:  appStillBackgroundDiskFreeUpdate()
 *	Description:	update the disk free
 *	param: get from appCurDiskInfoGet(), freeSz
 *------------------------------------------------------------------------*/


void
appStillBackgroundDiskFreeUpdate(UINT32 dskFree)
{
	int spaceInKb;
	spaceInKb = dskFree - APP_DISK_DPOF_RSVD_SIZE_KB;
	gSBG.dskFree = spaceInKb>0? spaceInKb : 0;
}

/*-------------------------------------------------------------------------
 *	Function Name:  appStillRemainNumGet()
 *	Description: get the remain image number to be captured
 *	param: N /A
 *------------------------------------------------------------------------*/


UINT32
appStillRemainNumGet(void)
{

	if(_StillCheckDiskReady() == FALSE)
        return 0;

	gStillCB.estFileSize = appStillEstFileSize(pViewParam->stillSize, pViewParam->stillQuality, gStillCB.diskClusterSize);
	DBG_PRINT("bg: gStillCB.isDiskReady:%d\n",gStillCB.isDiskReady);
	DBG_PRINT("bg: gStillCB.estFileSize:%d\n",gStillCB.estFileSize);
	if (gStillCB.isDiskReady && gStillCB.estFileSize){
		UINT32 dskFree;
		appDiskInfo_t* pDsk;
		pDsk = appCurDiskInfoGet();
		appStillBackgroundDiskFreeUpdate(pDsk->freeSz);
		appStillBackgroundGetFree(&dskFree);
		DBG_PRINT("bg: dskFree[%d], ret[%d]\n", dskFree, ret);
		/* Jira 1605  ,cause by normal capture 16bit raw ,Burst capture 8 bit raw */
		if(pViewParam->stillDriverMode == UI_STILL_DRIVERMODE_OFF)
				return (dskFree / gStillCB.estFileSize);
		else
				return (dskFree / (gStillCB.estFileSize*5/9));
	}
	return 0;
}

