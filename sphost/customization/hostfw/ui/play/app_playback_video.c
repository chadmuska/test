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
#include "app_com_def.h"
#include "app_com_api.h"
#include "sp5k_global_api.h"
#include "sp5k_disp_api.h"
#include "sp5k_dcf_api.h"
#include "app_timer.h"
#include "app_playback_private.h"
#include "app_playback_def.h"
#include "app_playback_osd.h"
#include "app_video.h"
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
#include "jet_utility.h"
#include "app_infohandler_feedback.h"
#include "app_infohandler_func.h"
#include <math.h>
#include "sp5k_dbg_api.h"
#include "jet_cdfs.h"
#endif
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

#define PB_VIDEO_ELAPSED_TIMER_MS 500
#define video_affine 0

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

#define VIDEO_CRITICAL_OP_POST_SLEEP	APP_SLEEP_MS(500)
#define WRITE8(a,b)	*(volatile UINT8*)(a) = (b)

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef enum {
	VDO_SXN_SNAPSHOT_START,
	VDO_SXN_SNAPSHOT_END,
} videoUIKeyCtrl_t;

typedef enum {
	VIDEO_ERR_OUT_OF_ELAPSE_TMR,
} videoErrorCode_t;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

static UINT32 gvideoElapTimer = TIMER_NULL;

/*seamless video playback*/
static BOOL seamlessPlayEn = FALSE;
extern pbMainCB_t	gpbMainCb;
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
UINT16 prePlayStatus=PB_VIDEO_STAT_STANDBY;
static BOOL seamlessPlayLoadFail = FALSE;
BOOL gResetElapseTimeFlag=FALSE;
#endif
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

 #if defined(PRJ_17DVR)||defined(PRJ_19DVR)
extern DiskStatus appInfoDiskStatus;
extern void appReportErrorStatusFB(UINT8 state, UINT8 error);
extern void appInfoDiskErrorFB(DiskStatus state);
extern void appInfoPanaStateSet(PanaState state);


extern UINT32 jet_pb_mediainfo_retriveve(void);
extern BOOL jet_pb_video_file_udata_info_get(void);
extern void appInfoPlayPOSHandler(void);
UINT32 jet_pb_next_video_index_get(BOOL seamlessNotify);
#endif
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
static void appPbVideoStExit(UINT32 nextSt,	UINT32 stMsg);

/*-------------------------------------------------------------------------
 *  File Name : appPbVideoSeamlessOsd
 *------------------------------------------------------------------------*/
/*seamless video playback*/
void appPbVideoSeamlessOsd(void)
{
    /* UINT8 FileNumString[64]; */

	APP_OSD_REFRESH_OFF;
	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 100, 0, 110, 36, 0, 0);

    if(seamlessPlayEn)
    {
		UINT8 FileNumString[64];
    	memset(FileNumString, 0, 10);
    	sprintf((char*)FileNumString, "%d/%d ", gpbImgCb.dcfIdx,gpbMainCb.totalImg);
    	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
    	appOsdLib_TextDraw(PB_VIDEO_SEAMLESS_X, PB_VIDEO_SEAMLESS_Y, SP5K_GFX_ALIGN_TOP_RIGHT, FileNumString);
    }
    else
    {
		appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
    }
	APP_OSD_REFRESH_ON;
}

/*-------------------------------------------------------------------------
 *  File Name : videoUIKeyControl
 *------------------------------------------------------------------------*/
static
void
videoUIKeyControl(
	UINT8 ctrl
)
{
	DBG_PRINT("pbv: %s: ctrl(%d)\n", __FUNCTION__, ctrl);
	switch (ctrl){
	case VDO_SXN_SNAPSHOT_START:
		appBtnDisable(BTN_ALL);
		break;
	case VDO_SXN_SNAPSHOT_END:
		appBtnEnable(BTN_ALL);
		break;
	default:
		PB_DBG(LEVEL_WARN,"pbv: undefined key session");
		break;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appPbVideoValidityCheckup
 *------------------------------------------------------------------------*/
#if 0
static
UINT32
videoValidityCheckup(
	void
)
{
	/*-- Minimum video length !? */
	/*-- File Error, perhaps, we shall check this in PB_MAIN UP key */

	return SUCCESS;
}
#endif

/*-------------------------------------------------------------------------
 *  File Name : videoSnapValidityCheckup
 *------------------------------------------------------------------------*/
static
UINT32
videoSnapValidityCheckup(
	void
)
{
	UINT32 imgCluz, freeCluz;

	if (gpbDiskCb.isCardLock)
	{
		gpbMainCb.err = PB_ERR_CARD_LOCKED;
		return FAIL;
	}
	else if (gpbDiskCb.isDcfFull)
	{
		gpbMainCb.err = PB_ERR_DCF_KEY_FULL;
		return FAIL;
	}

	imgCluz = appPbBytes2ClusterCalculate(gpbImgCb.imgWidth*gpbImgCb.imgHeight);
		/*-- maximun estimation, W*H*2/2 */
	freeCluz = appPbDiskFreeClusterGet(gpbDiskCb.dskInfo.actDsk);

	PB_DBG(LEVEL_INFO,"pbv: imgCluz(%d), freeCluz(%d)", imgCluz, freeCluz);

	if (imgCluz > freeCluz)
	{
		gpbMainCb.err = PB_ERR_DISK_FULL;
		return FAIL;
	}

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : videoFlyingPlayExecute
 *------------------------------------------------------------------------*/
#if !defined(PRJ_17DVR)&&!defined(PRJ_19DVR)
static
void
videoFlyingPlayExecute(
	pbMediaFlyingDir_t flyingDir
)
{
	UINT32 flyingCtrl, flyingSpeed;
	BOOL isInverseFlying = FALSE;

	DBG_PRINT("pbv: %s: flyingDir(%s)\n", __FUNCTION__, (flyingDir==PB_MEDIA_FLYING_FF ? "FF" : "REW") );

	/*-- flying speed control */
	flyingSpeed = gpbImgCb.mediaPlaySpeed;
	if (flyingDir==PB_MEDIA_FLYING_FF)
	{
		if (gpbMainCb.subStat==PB_VIDEO_STAT_REW)
		{
			/* Inverse flying !! */
			isInverseFlying = TRUE;
		}
		flyingCtrl = SP5K_MEDIA_PLAY_FORWARD;
		gpbMainCb.subStat = PB_VIDEO_STAT_FF;
	}
	else if (flyingDir==PB_MEDIA_FLYING_REW)
	{
		if (gpbMainCb.subStat==PB_VIDEO_STAT_FF)
		{
			/* Inverse flying !! */
			isInverseFlying = TRUE;
		}
		flyingCtrl = SP5K_MEDIA_PLAY_REWIND;
		gpbMainCb.subStat = PB_VIDEO_STAT_REW;
	}
	else if (flyingDir==PB_MEDIA_FLYING_SLOW_MOTION)
	{
		if (gpbMainCb.subStat==PB_VIDEO_STAT_SLOW_MOTION)
		{
			/* Inverse flying !! */
			isInverseFlying = TRUE;
		}
		flyingCtrl = SP5K_MEDIA_PLAY_SLOW_MOTION;
		gpbMainCb.subStat = PB_VIDEO_STAT_SLOW_MOTION;
	}
	else
	{
		return;
	}

	if (isInverseFlying)
	{
		/*-- Reset to ACC_1 speed for inverse flying */
		gpbImgCb.mediaPlaySpeed = flyingSpeed = PB_PLAY_SPEED_ACCELERAT_1;
	}
	else
	{
		PB_INDEX_INC(flyingSpeed, PB_PLAY_SPEED_ACCELERAT_1, PB_PLAY_SPEED_ACCELERAT_MAX);
		gpbImgCb.mediaPlaySpeed = flyingSpeed;
	}

	/*-- go on media control for flying play */
	if (flyingSpeed==PB_PLAY_SPEED_NORMAL)
	{
		sp5kMediaPlayControl(SP5K_MEDIA_PLAY_RESUME, 0, 0);
		gpbMainCb.subStat = PB_VIDEO_STAT_PLAYING;
		PB_DBG(LEVEL_INFO,"pbv: %s: Resume to NORMAL Spped", __FUNCTION__);
	}
	else
	{

		flyingSpeed *= 2; /* mapping enum to real multiple */
		if (flyingDir==PB_MEDIA_FLYING_SLOW_MOTION)
		{
			   flyingSpeed *= 2;
		}
		sp5kMediaPlayControl(flyingCtrl, flyingSpeed, 0);
		PB_DBG(LEVEL_INFO,"pbv: %s: flyingCtrl(%d), speedX(%d)", __FUNCTION__, flyingCtrl, flyingSpeed);
	}

	if (flyingDir==PB_MEDIA_FLYING_FF && gpbMainCb.subStat==PB_VIDEO_STAT_FF)
	{
		; /* do noting */
	}
	else if (flyingDir==PB_MEDIA_FLYING_REW && gpbMainCb.subStat==PB_VIDEO_STAT_REW)
	{
		; /* do noting */
	}
	else if (flyingDir==PB_MEDIA_FLYING_SLOW_MOTION && gpbMainCb.subStat==PB_MEDIA_FLYING_SLOW_MOTION)
	{
		; /* do noting */
	}
	else
	{
		/*- SP5K does not allow any operation during this period */
		VIDEO_CRITICAL_OP_POST_SLEEP;
	}

	return;
}

/*-------------------------------------------------------------------------
 *  File Name : videoFlyingConstraintCheckup
 *------------------------------------------------------------------------*/
static
UINT32
videoFlyingConstraintCheckup(
	UINT8 opCmd
)
{
	UINT32 videoStat;
	UINT32 elapT, totalT, critT;

	DBG_PRINT("%s: opCmd(%d)\n", __FUNCTION__, opCmd);

	videoStat = gpbMainCb.subStat;
	sp5kMediaPlayAttrGet(SP5K_MEDIA_ATTR_ELAPSED_TIME, &elapT);
	gpbImgCb.mediaElapTime = elapT;
	totalT = gpbImgCb.mediaTotalTime;
	critT = PB_VIDEO_CRITICAL_TIME;
	PB_DBG(LEVEL_INFO,"pbv: %s: videoStat(%d), elapT(%d), critT(%d), totalT(%d)", __FUNCTION__, videoStat, elapT, critT, totalT);

	switch (opCmd)
	{
	default:
		return FAIL;
	case PB_MEDIA_FLYING_FF:
		/* The constraints :
		 * A. The end of 2 sec while PLAYING, NO FF !!
		 * B. The end of 2 sec while REWINDING, NO FF !!
		 * C. The begining 2 sec while REWINDING, NO FF !!
		 * D. 500ms of critical constraining time is needed for reverse operation,
		 *    --> e.g., Ask to FF while MEDIA is rewinding.
		 */
		if (videoStat==PB_VIDEO_STAT_PLAYING)
		{
			/*-- Media is playing */
			if (IS_MEDIA_ENDING_CRITICAL_TIME(elapT, critT, totalT))
			{
				PB_DBG(LEVEL_WARN,"pbv: FF - Rule (A), ignored");
				return FAIL;
			}
		}
		else if (videoStat==PB_VIDEO_STAT_REW)
		{
			/*-- Media is rewinding */
			if (IS_MEDIA_ENDING_CRITICAL_TIME(elapT, critT, totalT))
			{
				return FAIL;
			}
			else if (IS_MEDIA_BEGINNING_CRITICAL_TIME(elapT, critT))
			{
				return FAIL;
			}
		}
		break;
	case PB_MEDIA_FLYING_REW:
		/* The constraints :
		 * A. The beginning of 2 sec while PLAYING, NO REW !!
		 * B. The endinf 2 sec while PLAYING, NO REW !!
		 * C. The begining 2 sec while F.FORWARDING, NO REW !!
		 * D. The ending 2 sec while F.FORWARDING, NO REW !!
		 * D. 500ms of critical constraining time is needed for reverse operation,
		 *    --> e.g., Ask to FF while MEDIA is rewinding.
		 */
		if (videoStat==PB_VIDEO_STAT_PLAYING)
		{
			/*-- Media is playing */
			if (IS_MEDIA_BEGINNING_CRITICAL_TIME(elapT, critT))
			{
				PB_DBG(LEVEL_WARN,"pbv: REW - Rule (A), ignored");
				return FAIL;
			}
			else if (IS_MEDIA_ENDING_CRITICAL_TIME(elapT, critT, totalT))
			{
				PB_DBG(LEVEL_WARN,"pbv: REW - Rule (B), ignored");
				return FAIL;
			}
		}
		else if (videoStat==PB_VIDEO_STAT_FF)
		{
			if (IS_MEDIA_BEGINNING_CRITICAL_TIME(elapT, critT))
			{
				PB_DBG(LEVEL_WARN,"pbv: REW - Rule (C), ignored");
				return FAIL;
			}
			else if (IS_MEDIA_ENDING_CRITICAL_TIME(elapT, critT, totalT))
			{
				PB_DBG(LEVEL_WARN,"pbv: REW - Rule (D), ignored");
				return FAIL;
			}
		}
		break;
	case PB_MEDIA_FLYING_SLOW_MOTION:
		break;
	}

	return SUCCESS;
}
#endif
/*-------------------------------------------------------------------------
 *  File Name : appPbVideoResume
 *------------------------------------------------------------------------*/
void
appPbVideoResume(
	void
)
{

	/*-- Retrieve elapsed time */
	sp5kMediaPlayAttrGet(SP5K_MEDIA_ATTR_ELAPSED_TIME, &gpbImgCb.mediaElapTime);

	/*-- Star ELAP_TMR */
	if (gvideoElapTimer==TIMER_NULL)
	{
		/*-- If resume from FF || REW, the ElapTimer is still working !! */
		gvideoElapTimer=appTimerSet(PB_VIDEO_ELAPSED_TIMER_MS, "ELAP_TMR");
	}
	if (TIMER_NULL==gvideoElapTimer)
	{
		sp5kHostMsgSend(APP_UI_MSG_PB_VIDEO_ERROR, VIDEO_ERR_OUT_OF_ELAPSE_TMR);
		return;
	}

	/*-- Go on Resume */
	sp5kMediaPlayControl(SP5K_MEDIA_PLAY_RESUME, 0, 0);
	gpbImgCb.mediaPlaySpeed = PB_PLAY_SPEED_NORMAL;
	gpbMainCb.subStat = PB_VIDEO_STAT_PLAYING;

}

/*-------------------------------------------------------------------------
 *  File Name : appPbVideoPause
 *------------------------------------------------------------------------*/
UINT32
appPbVideoPause(
	void
)
{
	UINT32 rval;
	/*-- Constraint :
	 * A. The very beginning and ending 2 sec, NO Pause.
	 */
#if !defined(PRJ_17DVR)&&!defined(PRJ_19DVR)
	sp5kMediaPlayAttrGet(SP5K_MEDIA_ATTR_ELAPSED_TIME, &gpbImgCb.mediaElapTime);
	if ( IS_MEDIA_BEGINNING_CRITICAL_TIME(gpbImgCb.mediaElapTime, PB_VIDEO_CRITICAL_TIME)
		 || IS_MEDIA_ENDING_CRITICAL_TIME(gpbImgCb.mediaElapTime, PB_VIDEO_CRITICAL_TIME, gpbImgCb.mediaTotalTime) )
	{
		return FAIL;
	}
#endif

	/*-- Go On Puase operation */
	rval = sp5kMediaPlayControl(SP5K_MEDIA_PLAY_PAUSE, 0, 0);
	gpbImgCb.mediaPlaySpeed = PB_PLAY_SPEED_NORMAL;
	gpbMainCb.subStat = PB_VIDEO_STAT_PAUSE;

	/*-- Stop ELAP_TMR */
	//PB_DBG(LEVEL_ERROR,"PbVideo: TMR(%d) clearing \n", gvideoElapTimer);
	appTimerClear(&gvideoElapTimer);

	return rval;

}

/*-------------------------------------------------------------------------
 *  File Name : appPbVideoSnap
 *------------------------------------------------------------------------*/
static
UINT32
appPbVideoSnap(
	void
)
{
	videoUIKeyControl(VDO_SXN_SNAPSHOT_START);

	if (FAIL==videoSnapValidityCheckup())
	{
		videoUIKeyControl(VDO_SXN_SNAPSHOT_END);
		return FAIL;
	}

	appOsdMessageBoxShow(TRUE, STRTK_IMG_SAVED);
	PB_SLEEP(1000);
	appOsdMessageBoxShow(FALSE, STRTK_IMG_SAVED);
	appPlayOsd_MediaPlayStatusDraw( TRUE, PAUSING);

	if (FAIL==sp5kMediaPlayControl(SP5K_MEDIA_PLAY_FRAME_CAPTURE, 0, 0))
	{
		gpbMainCb.err = PB_ERR_DISK_ERROR;
		videoUIKeyControl(VDO_SXN_SNAPSHOT_END);
		return FAIL;
	}

	videoUIKeyControl(VDO_SXN_SNAPSHOT_END);

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbVideoTerminate
 *------------------------------------------------------------------------*/
static
UINT32
appPbVideoTerminate(
	void
)
{
	/*-- Shall stop timer */
	appTimerClear(&gvideoElapTimer);

	/*-- Terminate video play */
	sp5kMediaPlayControl(SP5K_MEDIA_PLAY_ABORT, 0, 0);
	gpbMainCb.subStat = PB_VIDEO_STAT_STANDBY;
//paul modify 2018/03/28
//#if defined(PRJ_17DVr)
#if defined (PRJ_17DVR)||defined(PRJ_19DVR)
	sp5kMediaPlayCfgSet(SP5K_MEDIA_PLAY_DISP_FRAME_CB,(UINT32)NULL);
#endif
	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbVideoFlyingPlay
 *------------------------------------------------------------------------*/
#if !defined(PRJ_17DVR)&&!defined(PRJ_19DVR)
static
void
appPbVideoFlyingPlay(
	pbMediaFlyingDir_t flyingDir
)
{
	if (FAIL==videoFlyingConstraintCheckup(flyingDir))
	{
		return;
	}

	videoFlyingPlayExecute(flyingDir);
	return;
}
#endif
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
UINT32 gElapTime=0;
void appPbVideoElapTimeClean()
{
	gElapTime=0;
}
UINT32 appPbVideoElapTimeGet(void)
{
	return gElapTime;
}
void appPbVideoElapTimeCB(void)
{
	if(gResetElapseTimeFlag){
		if(gpbMainCb.subStat == PB_VIDEO_STAT_FF ||
			gpbMainCb.subStat == PB_VIDEO_STAT_PLAYING){
			gElapTime = 0;
		}else{
			gElapTime = 59000;
		}
		gResetElapseTimeFlag = FALSE;
		printf("@%u (1)\n",gElapTime);
		return;
	}
    sp5kMediaPlayAttrGet(SP5K_MEDIA_ATTR_ELAPSED_TIME, &gElapTime);
    //printf("@%u\n",gElapTime);
}
#endif
/*-------------------------------------------------------------------------
 *  File Name : appPbVideoPlayStart
 *------------------------------------------------------------------------*/
static
UINT32
appPbVideoPlayStart(
	void
)
{
	if (FAIL==sp5kMediaPlayControl(SP5K_MEDIA_PLAY_START, 0, 0))
	{
		gpbMainCb.err = PB_ERR_MEDIA_ERROR;
		return FAIL;
	}
	gpbMainCb.subStat = PB_VIDEO_STAT_PLAY_STARTING;
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	sp5kMediaPlayCfgSet(SP5K_MEDIA_PLAY_DISP_FRAME_CB,(UINT32)appPbVideoElapTimeCB);
#endif
	/*-- SP5KFW will send SP5K_MSG_MEDIA_PLAY_OK or
	 * SP5K_MSG_MEDIA_PLAY_FAIL to notify the result of PLAY_START cmd.
	 * Go and wait for the message !!
	 */
	return SUCCESS;
}
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)

  /*--------------------------------------------------------------------------*
 * Function name	: jet_pbvideo_stclose
 * Function	       	: Affine the playback video
 * Return value   	: void
 * Parameter1    	: void
 * History	   : 
 *                :  2018.04.12  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/   
void jet_pb_video_affine(void)
{	
#if video_affine
	SINT32 deg = 0, i;
//	return;
	double rad = deg*3.1415926535/180;
	SINT32 affine[4] = { cos(rad)*65536, -sin(rad)*65536, sin(rad)*65536, cos(rad)*65536 };
	/* width of standby buffer is 320(16-alignment), height of standby buffer is 240 */
	/* the value should change if standby buffer size is different */
	for (i=0 ; i<4 ; i++) {
		if( i == 0)
			affine[i] = affine[i]*1;
		else
			affine[i] = affine[i]*320/240;
	}
	
	sp5kMediaPlayCfgSet(SP5K_MEDIA_PLAY_AFFINE, (UINT32)affine);
#endif
}
#endif
/*-------------------------------------------------------------------------
 *  File Name : appPbVideoStCreate
 *------------------------------------------------------------------------*/
static
UINT32
appPbVideoStCreate(
	void
)
{
    UINT32 frameRate, width, height;

	if(0==gpbImgCb.mediaTotalTime)
	{
		return FAIL;
	}

	/*-- Register TIMER for elapsed time process */
	gvideoElapTimer = appTimerSet(PB_VIDEO_ELAPSED_TIMER_MS, "VDO_ELAP");
	if (gvideoElapTimer==TIMER_NULL)
	{
		return FAIL;
	}

	gpbImgCb.mediaPlaySpeed = PB_PLAY_SPEED_NORMAL;
	gpbImgCb.mediaElapTime = 0;

	sp5kMediaPlayAttrGet(SP5K_MEDIA_ATTR_VIDEO_FRAME_RATE, &frameRate);
	sp5kMediaPlayAttrGet(SP5K_MEDIA_ATTR_WIDTH, &width);
	sp5kMediaPlayAttrGet(SP5K_MEDIA_ATTR_HEIGHT, &height);
	printf("%s W = %u H = %u \n" , __FUNCTION__ , width , height);
    #if !SP5K_USB_UVC_SUPPORT
        #if SUPPORT_RUNTIME_SWITCH_DRAM_CLK
        if (width * height * frameRate >= 1920 * 1080 * 31){
            appSysClkSwitch(1);
        }
        else{
    		appSysClkSwitch(0);
    	}
    	#endif
    #endif
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	jet_pb_video_affine();
#endif
	sp5kModeSet(SP5K_MODE_VIDEO_PLAYBACK);
	appHostMsgWaitExact(SP5K_MSG_MODE_READY,SP5K_MODE_VIDEO_PLAYBACK,5000);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	jet_pb_video_affine();
#endif

	appPowerSavingEnableSet(FALSE);/*++ for not power saving while playing video*/

	return SUCCESS;

}

/*-------------------------------------------------------------------------
 *  File Name : appPbVideoStClose
 *------------------------------------------------------------------------*/
static
void
appPbVideoStClose(
	void
)
{
    sp5kMediaPlayCfgSet(SP5K_MEDIA_PLAY_DISP_FRAME_RATE, 0);

	if (gpbMainCb.subStat != PB_VIDEO_STAT_STANDBY)
	{
		appPbVideoTerminate();
	}

	gpbMainCb.subStat = PB_VIDEO_STAT_STANDBY;

}
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)

  /*--------------------------------------------------------------------------*
 * Function name	: jet_pbvideo_stclose
 * Function	       	: Playback video state close
 * Return value   	: void
 * Parameter1    	: void
 * History	   : 
 *                :  2018.04.12  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
    
void jet_pbvideo_stclose(void)
{
	printf("[%s]\n",__FUNCTION__);
	appPbVideoStClose();
	/* FIXME : check after single link list */
	jet_file_list_filter_current_play_set(0);
	PlayPosCountStop();
	//appInfoPanaStateSet(PANA_STATE_PLAY_IDLE);
}
#endif
/*-------------------------------------------------------------------------
 *  File Name : appPbVideoStExit
 *------------------------------------------------------------------------*/
static
void
appPbVideoStExit(
	UINT32 nextSt,
	UINT32 stMsg
)
{
	appPbVideoStClose();
	appStateChange(nextSt, stMsg);
}
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
void seamlessFileNameCB(UINT8 *fileName)
{
	printf("*********** %s (%d) start\n", __func__, __LINE__);
	appInfoPlayPOSHandler();
	PlayPosCountStop();
	jet_file_list_filter_current_play_set(jet_pb_next_video_index_get(TRUE));
	gpbImgCb.dcfIdx = gcurrDcfIdx = jet_cdfs_pb_dcf_index_get();
	sp5kDcfFsCurFileIdxSet(gpbImgCb.dcfIdx);
	playImgDcfAttrGet(&gpbImgCb);
	jet_pb_mediainfo_retriveve();
	memcpy((char *)fileName, &gpbImgCb.szDcfFileName[0], sizeof(gpbImgCb.szDcfFileName));
	printf("filename %s\n", &gpbImgCb.szDcfFileName[0]);
	if(jet_pb_video_file_udata_info_get()==FAIL){
		seamlessPlayEn = FALSE;
		gpbMainCb.err = PB_ERR_FILE_ERROR;
		sp5kMediaPlayControl(SP5K_MEDIA_PLAY_SEAMLESS_EN, 0, 0);
		appReportErrorStatusFB(1, 3);
		appFilePBStartFB(1);
		appInfoPanaStateSet(PANA_STATE_PLAY_IDLE);
		return;
	}
	if(gpbMainCb.subStat == PB_VIDEO_STAT_FF || gpbMainCb.subStat == PB_VIDEO_STAT_REW
		|| gpbMainCb.subStat == PB_VIDEO_STAT_PLAYING){
		gResetElapseTimeFlag = TRUE;
	}else{
		gResetElapseTimeFlag = FALSE;
	}
	if(gpbMainCb.subStat == PB_VIDEO_STAT_FF ||
		gpbMainCb.subStat == PB_VIDEO_STAT_PLAYING){
		gElapTime = 0;
	}else{
		gElapTime = 59000;
	}
	sp5kTimerIsrOneShotReg( 0 , 200, PlayPosCountStart);
	printf("*********** %s (%d) end\n", __func__, __LINE__);
}
#endif
/*-------------------------------------------------------------------------
 *  File Name : appPbVideoStInit
 *------------------------------------------------------------------------*/
static
UINT32
appPbVideoStInit(
	void
)
{
	appOsdLib_OsdLayerShapeClear(APP_OSD_SHAPE_RECT,0, 0, 320, 240, 0, 0);	/*ICOM-3269*/
	gpbMainCb.subStat = PB_VIDEO_STAT_STANDBY;

	//paul modify 2018/0328 for check appPbVideoStCreate()
	if (FAIL==appPbVideoStCreate())
	{
		return FAIL;
	}
#if !defined(PRJ_17DVR)&&!defined(PRJ_19DVR)
	if (FAIL==appPbVideoStCreate())
	{
		appPbVideoStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
		return FAIL;
	}

    appPlayOsd_VideoClrOsd();
#endif

	if (FAIL==appPbVideoPlayStart())
	{
#if !defined(PRJ_17DVR)&&!defined(PRJ_19DVR)
		appPbVideoStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
#endif
		return FAIL;
	}
	#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	seamlessPlayEn = TRUE;
	sp5kMediaPlayControl(SP5K_MEDIA_PLAY_SEAMLESS_EN, 1, 0);
	sp5kMediaPlayCfgSet(SP5K_MEDIA_PLAY_SEAMLESS_FILENAME_CB, (UINT32)seamlessFileNameCB);
	appReportErrorStatusFB(1, 0);
	appFilePBStartFB(1);
	PlayPosCountStart();
	appInfoPanaStateSet(PANA_STATE_PLAYING);
	appPbVideoElapTimeCB();
	appInfoPlayPOSHandler();
	#endif

   	APP_OSD_REFRESH_ON;
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	sp5kDispAttrSet(sp5kDispActChnlGet(0), SP5K_DISP_OSD_ACTIVE, 0);
#endif

    //sp5kHostMsgSend(APP_UI_MSG_PB_VIDEO_CREATE, 0);
	return SUCCESS;
}
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)

  /*--------------------------------------------------------------------------*
 * Function name	: jet_pb_video_dir_speed_set
 * Function	       	: Set playback video speed
 * Return value   	: result (BOOL TRUE/FALSE)
 * Parameter1    	: dir -speed branch (UINT8 - 0: Fast forward, 1: Rewind)
 * Parameter2    	: speed - setting video play speed(UINT8 - 0: 1X, 1: 4X)
 * History	   : 
 *                :  2018.04.12  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
    
BOOL jet_pb_video_dir_speed_set(UINT8 dir, UINT8 speed)
{
	//speed branch(dir)	0: Fast forward, 1: Rewind
	//speed	0: 1X, 1: 4X
	UINT8 flyingDir,i;
	UINT32 flyingCtrl, flyingSpeed, rval;
	printf("[%s] dir = %d, speed = %d\n",__FUNCTION__, dir, speed);
	if(dir == AVN_PB_PARA_SPEED_BRANCH_FF){
		flyingDir = PB_MEDIA_FLYING_FF;
	}else{
		flyingDir = PB_MEDIA_FLYING_REW;
	}
	if(speed == AVN_PB_PARA_SPEED_X1){
		flyingSpeed = PB_PLAY_SPEED_NORMAL;
	}else{
		flyingSpeed = PB_PLAY_SPEED_ACCELERAT_1*4;// 4X speed
	}
	#if 0// Don't need to constraint for v35
	if (FAIL==videoFlyingConstraintCheckup(flyingDir))
	{
		printf("failed videoFlyingConstraintCheckup\n");
		return FAIL;
	}
	#endif
	DBG_PRINT("pbv: %s: flyingDir(%s)\n", __FUNCTION__, (flyingDir==PB_MEDIA_FLYING_FF ? "FF" : "REW") );
	/*-- flying speed control */
	if (flyingDir==PB_MEDIA_FLYING_FF)
	{
		flyingCtrl = SP5K_MEDIA_PLAY_FORWARD;
		gpbMainCb.subStat = PB_VIDEO_STAT_FF;
	}
	else if (flyingDir==PB_MEDIA_FLYING_REW)
	{
		flyingCtrl = SP5K_MEDIA_PLAY_REWIND;
		gpbMainCb.subStat = PB_VIDEO_STAT_REW;
	}
	else if (flyingDir==PB_MEDIA_FLYING_SLOW_MOTION)
	{
		flyingCtrl = SP5K_MEDIA_PLAY_SLOW_MOTION;
		gpbMainCb.subStat = PB_VIDEO_STAT_SLOW_MOTION;
	}
	else
	{
		return FAIL;
	}
	gpbImgCb.mediaPlaySpeed = flyingSpeed;
	for(i=0; i<20; i++){
	/*-- go on media control for flying play */
		if (flyingSpeed==PB_PLAY_SPEED_NORMAL)
		{
			rval = sp5kMediaPlayControl(SP5K_MEDIA_PLAY_RESUME, 0, 0);
			gpbMainCb.subStat = PB_VIDEO_STAT_PLAYING;
			PB_DBG(LEVEL_INFO,"pbv: %s: Resume to NORMAL Spped", __FUNCTION__);
		}
		else
		{
			rval = sp5kMediaPlayControl(flyingCtrl, flyingSpeed, 0);
			PB_DBG(LEVEL_INFO,"pbv: %s: flyingCtrl(%d), speedX(%d)", __FUNCTION__, flyingCtrl, flyingSpeed);
		}
		if(rval!=SUCCESS){
			printf("[SC] *** fail to control\n");
			if(i<9){
				appTimeDelayMs(100);
				printf("[SC] *** retry again\n");
				continue;
			}else{
				break;
			}			
		}else{
			printf("[SC] *** success to control\n");
			break;
		}
	}
	if (flyingDir==PB_MEDIA_FLYING_FF && gpbMainCb.subStat==PB_VIDEO_STAT_FF)
	{
		; /* do noting */
	}
	else if (flyingDir==PB_MEDIA_FLYING_REW && gpbMainCb.subStat==PB_VIDEO_STAT_REW)
	{
		; /* do noting */
	}
	else if (flyingDir==PB_MEDIA_FLYING_SLOW_MOTION && gpbMainCb.subStat==PB_MEDIA_FLYING_SLOW_MOTION)
	{
		; /* do noting */
	}
	else
	{
		/*- SP5K does not allow any operation during this period */
		VIDEO_CRITICAL_OP_POST_SLEEP;
	}
	return SUCCESS;
}


  /*--------------------------------------------------------------------------*
 * Function name	: jet_pb_next_video_index_get
 * Function	       	: Get next playback video index
 * Return value   	: Next video Index (UINT32)
 * Parameter1    	: seamless Notify (BOOL TRUE/FALSE)
 * History	   : 
 *                :  2018.04.12  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
    
UINT32 jet_pb_next_video_index_get(BOOL seamlessNotify)
{
	UINT16 index=0, total=0;
	total = jet_file_list_filter_num_get();

	printf("%s(%d) %d\n", __func__, __LINE__, seamlessNotify);
	index = jet_file_list_filter_current_play_get();
	if(jet_avn_pb_para_get()->mode_branch == AVN_PB_PARA_MODE_BRANCH_NORMAL){
		if(jet_avn_pb_para_get()->mode == AVN_PB_PARA_MODE_CONTINUE_ALL){
			if(jet_avn_pb_para_get()->speed_branch == AVN_PB_PARA_SPEED_BRANCH_FF){
				index++;
			}else if(jet_avn_pb_para_get()->speed_branch == AVN_PB_PARA_SPEED_BRANCH_RW){
				index = (index==1)? total : index-1;
			}
		}	
	}else if(jet_avn_pb_para_get()->mode_branch == AVN_PB_PARA_MODE_BRANCH_SKIP_NEXT){
		index++;
	}else if(jet_avn_pb_para_get()->mode_branch == AVN_PB_PARA_MODE_BRANCH_SKIP_PREVIOUS){
		if(seamlessNotify == TRUE){
			index++;
		}else{
			if(gpbImgCb.mediaElapTime <= 3000){
				index = (index==1)? total : index-1;
			}
		}
	}
	if(index > total){
		index = 1;
	}
	return index;
}
 
  /*--------------------------------------------------------------------------*
 * Function name	: jet_pb_video_play
 * Function	       	: Play video for playback
 * Return value   	: result (BOOL SUCCESS/FAIL)
 * Parameter1    	: void
 * History	   : 
 *                :  2018.04.12  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
    
UINT32 jet_pb_video_play(void)
{
	UINT32 time=0;

	printf("[%s]\n", __FUNCTION__);
	gpbImgCb.dcfIdx = gcurrDcfIdx = jet_cdfs_pb_dcf_index_get();
	sp5kDcfFsCurFileIdxSet(gpbImgCb.dcfIdx);
	if (FAIL==playImgDcfAttrGet(&gpbImgCb)){
		return FAIL;
	}
	if(jet_pb_video_file_udata_info_get()==FAIL){
		return FAIL;
	}
	jet_pb_mediainfo_retriveve();
	#if 0
	if (FAIL==appPbVideoStCreate()){
		return FAIL;
	}
	#endif
	jet_pb_video_affine();
	#if 0
	if(prePlayStatus == PB_VIDEO_STAT_PAUSE){
		sp5kModeSet(SP5K_MODE_VIDEO_PLAYBACK);
		sp5kModeWait(SP5K_MODE_VIDEO_PLAYBACK);
	}
	#endif
	jet_lineout_out(FALSE);
	if (FAIL==appPbVideoPlayStart()){
		jet_lineout_out(TRUE);
		return FAIL;
	}
	appInfoPanaStateSet(PANA_STATE_PLAYING);
	sp5kMediaPlayAttrGet(SP5K_MEDIA_ATTR_DURATION, &time);
	if(jet_avn_pb_para_get()->mode_branch == AVN_PB_PARA_MODE_BRANCH_NORMAL
		&& jet_avn_pb_para_get()->speed == AVN_PB_PARA_SPEED_X4)
	{
		printf("\n AVN_PB_PARA_SPEED_X4 \n");
		if(jet_avn_pb_para_get()->speed_branch == AVN_PB_PARA_SPEED_BRANCH_RW){
			printf("\n AVN_PB_PARA_SPEED_BRANCH_RW time = %u\n", time);
			sp5kMediaPlayControl(SP5K_MEDIA_PLAY_SEEK, time, 0);
		}else{
			printf("\n AVN_PB_PARA_SPEED_BRANCH_FF \n");
			sp5kMediaPlayControl(SP5K_MEDIA_PLAY_SEEK, 0, 0);
		}
		jet_pb_video_dir_speed_set(jet_avn_pb_para_get()->speed_branch, jet_avn_pb_para_get()->speed);

	}else if(jet_avn_pb_para_get()->mode_branch == AVN_PB_PARA_MODE_BRANCH_SKIP_NEXT
		||jet_avn_pb_para_get()->mode_branch == AVN_PB_PARA_MODE_BRANCH_SKIP_PREVIOUS){
		if(prePlayStatus == PB_VIDEO_STAT_PAUSE){
			appPbVideoPause();
			appInfoPanaStateSet(PANA_STATE_PLAY_IDLE);
		}
	}
	seamlessPlayEn = TRUE;
	printf("\n SP5K_MEDIA_PLAY_SEAMLESS_EN \n");
	sp5kMediaPlayControl(SP5K_MEDIA_PLAY_SEAMLESS_EN, 1, 0);
	sp5kMediaPlayCfgSet(SP5K_MEDIA_PLAY_SEAMLESS_FILENAME_CB, (UINT32)seamlessFileNameCB);
	jet_lineout_out(TRUE);
	return SUCCESS;
}

  /*--------------------------------------------------------------------------*
 * Function name	: jet_pb_video_play_next
 * Function	       	: Play next video for playback
 * Return value   	: void
 * Parameter1    	: void
 * History	   : 
 *                :  2018.04.12  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
    
void jet_pb_video_play_next(void)
{
	sp5kMediaPlayAttrGet(SP5K_MEDIA_ATTR_ELAPSED_TIME, &gpbImgCb.mediaElapTime);
	PlayPosCountStop(); 	
	prePlayStatus = gpbMainCb.subStat;
	appPbVideoTerminate();
	if(seamlessPlayLoadFail == TRUE){
		//seamlessPlayLoadFail = FALSE;
	}else{
		jet_file_list_filter_current_play_set(jet_pb_next_video_index_get(FALSE));
	}
	if(jet_pb_video_play() == FAIL){
		seamlessPlayLoadFail = FALSE;
		gpbMainCb.err = PB_ERR_FILE_ERROR;
		appReportErrorStatusFB(1, 3);
		appFilePBStartFB(1);
		appInfoPanaStateSet(PANA_STATE_PLAY_IDLE);
		return;
	}
	appPbVideoElapTimeClean();
	appInfoPlayPOSHandler();	
	appReportErrorStatusFB(1, 0);
	appFilePBStartFB(1);
	if(seamlessPlayLoadFail == TRUE){
		seamlessPlayLoadFail = FALSE;
		//sp5kTimerIsrOneShotReg( 0 , 200, PlayPosCountStart);
		PlayPosCountStart();
	}else{
		PlayPosCountStart();
	}

	
}
#endif

/*-------------------------------------------------------------------------
 *  File Name : appPbVideoStOnMsg
 *------------------------------------------------------------------------*/
void
appPbVideoStOnMsg(
	UINT32 msg,
	UINT32 param
)
{
    PB_DBG(LEVEL_DEBUG,"PbVideo: appPbVideoStOnMsg %x %x",msg,param);
	switch (msg)
	{
	case SP5K_MSG_TIMER:
		if (gvideoElapTimer!=param)
		{
			break;
		}

		if (gpbMainCb.subStat==PB_VIDEO_STAT_PLAYING
			|| gpbMainCb.subStat==PB_VIDEO_STAT_SLOW_MOTION
		 	|| gpbMainCb.subStat==PB_VIDEO_STAT_FF
		 	|| gpbMainCb.subStat==PB_VIDEO_STAT_REW)
		{
			sp5kMediaPlayAttrGet(SP5K_MEDIA_ATTR_ELAPSED_TIME, &gpbImgCb.mediaElapTime);
		}
#if !defined(PRJ_17DVR)&&!defined(PRJ_19DVR)
		appPlayOsd_VideoTimeDraw(TRUE,gpbImgCb.mediaElapTime);
#endif
		break;
	case SP5K_MSG_MEDIA_PLAY_START:
		if (param==SP5K_MSG_MEDIA_PARAM_OKAY)
		{
			if (gpbMainCb.subStat != PB_VIDEO_STAT_PLAY_STARTING)
			{
				break;
			}
			gpbMainCb.err = PB_ERR_NONE;
			gpbMainCb.subStat = PB_VIDEO_STAT_PLAYING;
		}
		else
		{

			if (gpbMainCb.subStat != PB_VIDEO_STAT_PLAY_STARTING)
			{
				break;
			}
			gpbMainCb.err = PB_ERR_FILETYPE_ERROR;
			/*appPbErrorProcess(gpbMainCb.err);*/
			appPbVideoTerminate();
			appPbVideoStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
		}
		break;
	case SP5K_MSG_MEDIA_PLAY_STOP:
		break;
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)/*weikai modify 180423*/
	case JET_APP_UI_MSG_PB_STOP:
		printf("[JET_APP_UI_MSG_PB_STOP]\n");
		appPbVideoStClose();
		PlayPosCountStop();
		appInfoPanaStateSet(PANA_STATE_PLAY_IDLE);		
		appFilePBStopFB(1, jet_avn_pb_para_get(), jet_file_list_filter_current_play_get()-1);
		jet_file_list_filter_current_play_set(0);
		break;
#endif
	case SP5K_MSG_MEDIA_PLAY_FILE_ERROR:
		gpbMainCb.err = PB_ERR_FILE_ERROR;
		appPbErrorProcess(gpbMainCb.err);
		appPbVideoTerminate();
		#if JET_PANA_CDFS
		appReportErrorStatusFB(1, 3);
		appFilePBStartFB(1);
		jet_pbvideo_stclose();
		//appPbVideoStExit(APP_STATE_VIDEO_PREVIEW, STATE_PARAM_NORMAL_INIT);
		#else
		appPbVideoStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
		#endif

		break;
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	case JET_APP_UI_MSG_PB_VIDEO_NEXT:
		printf("******** JET_APP_UI_MSG_PB_VIDEO_NEXT **************\n");
		jet_pb_video_play_next();
		break;
#endif
	case SP5K_MSG_MEDIA_PLAY_END:
	case SP5K_MSG_MEDIA_FF_TO_END:
	case SP5K_MSG_MEDIA_RW_TO_END:
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		printf("******** SP5K_MSG_MEDIA_PLAY_END **************\n");
		jet_pb_video_play_next();
#else
		sp5kMediaPlayAttrGet(SP5K_MEDIA_ATTR_ELAPSED_TIME, &gpbImgCb.mediaElapTime);
		appPlayOsd_VideoTimeDraw(TRUE,gpbImgCb.mediaElapTime);

		appPbVideoTerminate();
		appPbVideoStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
#endif
        break;
	case APP_UI_MSG_PB_VIDEO_CREATE:
		break;
	/*----- Video ERROR Handling */
	case APP_UI_MSG_PB_VIDEO_ERROR:
		if (param==VIDEO_ERR_OUT_OF_ELAPSE_TMR)
		{
			appPbVideoTerminate();
			gpbMainCb.err = PB_ERR_VIDEO_ERROR;
			appPbVideoStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
		}
		break;
    /*seamless video playback*/
	case SP5K_MSG_MEDIA_PLAY_SEAMLESS_LOAD_NOTIFY:
		if (seamlessPlayEn)
        {
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
        	PlayPosCountStop();
			jet_file_list_filter_current_play_set(jet_pb_next_video_index_get(TRUE));
			gpbImgCb.dcfIdx = gcurrDcfIdx = jet_cdfs_pb_dcf_index_get();
#else
            while(1)
            {
			gpbImgCb.dcfIdx++;
			if(gpbImgCb.dcfIdx > gtotalImgCount){
				gpbImgCb.dcfIdx = 1;
			}
#endif

			sp5kDcfFsCurFileIdxSet(gpbImgCb.dcfIdx);
			playImgDcfAttrGet(&gpbImgCb);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
			jet_pb_mediainfo_retriveve();
#else
                if (IS_FILETYPE_JPG(gpbImgCb.dcfAttr.fileType))
                {
                    PB_DBG(LEVEL_WARN,"PbVideo: seamless load Jpg %s", &gpbImgCb.szDcfFileName[0]);
                    continue;
                }
                else
#endif
                {
			sp5kMediaPlayAttrSet(SP5K_MEDIA_ATTR_FILE_NAME, (UINT32)&gpbImgCb.szDcfFileName[0]);
			sp5kMediaPlayControl(SP5K_MEDIA_PLAY_SEAMLESS_LOAD, (UINT32)&gpbImgCb.szDcfFileName[0], 0);
            PB_DBG(LEVEL_INFO,"PbVideo: seamless load notify %s", &gpbImgCb.szDcfFileName[0]);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
			PlayPosCountStart();
#else
                    break;
                }
#endif

            }
	}
		break;
	case SP5K_MSG_MEDIA_PLAY_SEAMLESS_LOAD_FINISH:
		printf("***** SP5K_MSG_MEDIA_PLAY_SEAMLESS_LOAD_FINISH *****\n");
#if !defined(PRJ_17DVR)&&!defined(PRJ_19DVR)
		if (seamlessPlayEn) {
            appPbVideoSeamlessOsd();
            PB_DBG(LEVEL_INFO,"PbVideo: seamless load finish");
		}
#endif
		break;
	case SP5K_MSG_MEDIA_PLAY_SEAMLESS_LOAD_FAILED:
		printf("***** SP5K_MSG_MEDIA_PLAY_SEAMLESS_LOAD_FAILED *****\n");
        PB_DBG(LEVEL_ERROR,"PbVideo: seamless load fail");

		seamlessPlayEn = FALSE;
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)/*weikai modify 180426*/
	/*Paul modify Bug #3431 add flag for 'jet_pbvideo_stclose()' function 
		    and retest the #3441 issue is work fine*/
		if(jet_pb_video_file_udata_info_get()==FAIL)
		jet_pbvideo_stclose();
		seamlessPlayLoadFail = TRUE;		
#endif
		break;
	default :
		break;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appPbVideoStOnKey
 *------------------------------------------------------------------------*/
void
appPbVideoStOnKey(
	UINT32 msg,
	UINT32 param
)
{
/* #if defined(PRJ_17DVR)	
UINT8 i;
#endif */
	switch (msg) {
	case APP_KEY_PRESS_S2:
		if (gpbMainCb.subStat != PB_VIDEO_STAT_PAUSE)
		{
			break;
		}
		if (FAIL==appPbVideoSnap())
		{
			appPbErrorProcess(gpbMainCb.err);
			appPbVideoStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
			break;
		}
		break;
	case APP_KEY_PRESS_MODE:
		break;

	case APP_KEY_PRESS_UP:
        PB_DBG(LEVEL_INFO,"PbVideo: APP_KEY_PRESS_UP %x",gpbMainCb.subStat);
		if (gpbMainCb.subStat==PB_VIDEO_STAT_FF
			|| gpbMainCb.subStat==PB_VIDEO_STAT_REW
			|| gpbMainCb.subStat==PB_VIDEO_STAT_SLOW_MOTION
		)
		{
			appPbVideoResume();
#if !defined(PRJ_17DVR)&&!defined(PRJ_19DVR)
			appPlayOsd_VideoTimeDraw(TRUE,gpbImgCb.mediaElapTime);
			appPlayOsd_VideoCntrlDraw(TRUE,FALSE,TRUE,TRUE);
			appPlayOsd_MediaPlayStatusDraw( FALSE, PAUSING);
#endif
			VIDEO_CRITICAL_OP_POST_SLEEP;
			#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
			appFilePBStartFB(1);
			appInfoPanaStateSet(PANA_STATE_PLAYING);
			#endif
		}
		else if (gpbMainCb.subStat==PB_VIDEO_STAT_PLAYING)
		{
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
			UINT8 i;
			for(i=0; i<10; i++){
				if(SUCCESS!=appPbVideoPause()){
					printf("[SC] *** fail to pause\n");
					if(i<9){
						appTimeDelayMs(100);
						printf("[SC] *** retry pause again\n");
						continue;
					}else{
						appFilePBPauseFB(0, jet_avn_pb_para_get());
						break;
					}
				}else{
					printf("[SC] *** success to pause\n");
					appInfoPanaStateSet(PANA_STATE_PLAY_IDLE);
					appFilePBPauseFB(1, jet_avn_pb_para_get());				
					break;
				}
			}
#else
			if (FAIL==appPbVideoPause())
			{
				return;
			}
			appPlayOsd_VideoTimeDraw(TRUE,gpbImgCb.mediaElapTime);
			appPlayOsd_VideoCntrlDraw(TRUE,TRUE,TRUE,TRUE);
			//appPlayOsd_MediaPlayStatusDraw( TRUE, PAUSING); /*ICOM-3721*/
#endif
		}
		else if (gpbMainCb.subStat==PB_VIDEO_STAT_PAUSE)
		{
			appPbVideoResume();
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
			appFilePBStartFB(1);
			appInfoPanaStateSet(PANA_STATE_PLAYING);
#else
			appPlayOsd_VideoTimeDraw(TRUE,gpbImgCb.mediaElapTime);
			appPlayOsd_VideoCntrlDraw(TRUE,FALSE,TRUE,TRUE);
			appPlayOsd_MediaPlayStatusDraw( FALSE, PAUSING);
#endif
		}
		else
		{
            PB_DBG(LEVEL_WARN,"PbVideo: APP_KEY_PRESS_UP return");
			#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
			appFilePBStartFB(1);
			#endif
			return;
		}
		break;
	case APP_KEY_PRESS_DOWN:
		appPbVideoStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
		break;
	case APP_KEY_PRESS_SET:
		break;
	case APP_KEY_PRESS_LEFT:
	case APP_KEY_PRESS_RIGHT:
		#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		printf("[SC] APP_KEY_PRESS_RIGHT\n");
		PlayPosCountStop();
		if(gpbMainCb.err == PB_ERR_FILE_ERROR){
			appFilePBStartFB(0);
			break;
		}
		if(jet_avn_pb_para_get()->speed == AVN_PB_PARA_SPEED_X4
			&& gpbMainCb.subStat==PB_VIDEO_STAT_PAUSE){
			printf("[SC] pb status is pause, resume first.\n");
			jet_pb_video_dir_speed_set(jet_avn_pb_para_get()->speed_branch, AVN_PB_PARA_SPEED_X1);
		}
		if(FAIL==jet_pb_video_dir_speed_set(jet_avn_pb_para_get()->speed_branch, jet_avn_pb_para_get()->speed)){
			sp5kHostMsgSend(JET_APP_UI_MSG_PB_VIDEO_NEXT,0);
			break;
		}
		appFilePBStartFB(1);
		PlayPosCountStart();
		break;
		#endif
	case APP_KEY_PRESS_DEL:
		#if !defined(PRJ_17DVR)&&!defined(PRJ_19DVR)
		if (gpbMainCb.subStat != PB_VIDEO_STAT_PLAYING
			&&gpbMainCb.subStat != PB_VIDEO_STAT_FF
			&&gpbMainCb.subStat != PB_VIDEO_STAT_REW)
		{
			break;
		}

		if (msg==APP_KEY_PRESS_LEFT)
		{
			appPbVideoFlyingPlay(PB_MEDIA_FLYING_REW);
		}
		else if(msg==APP_KEY_PRESS_RIGHT)
		{
			appPbVideoFlyingPlay(PB_MEDIA_FLYING_FF);
		}
		else if(msg==APP_KEY_PRESS_DEL)
		{
			appPbVideoFlyingPlay(PB_MEDIA_FLYING_SLOW_MOTION);
		}

		appPlayOsd_VideoTimeDraw(TRUE,gpbImgCb.mediaElapTime);

		if(appPbMediaUIStateGet()==PB_MEDIA_UI_STAT_FF)
		{
			appPlayOsd_MediaPlayStatusDraw( TRUE, FWDING);
		}
		else if(appPbMediaUIStateGet()==PB_MEDIA_UI_STAT_REW)
		{
			appPlayOsd_MediaPlayStatusDraw( TRUE, RWDING);
		}
		else if(appPbMediaUIStateGet()==PB_MEDIA_UI_STAT_SLOWMOTION)
		{
			appPlayOsd_MediaPlayStatusDraw( TRUE, SLOW);
		}
		break;
		#endif
	case APP_KEY_PRESS_MENU:
		appPbVideoTerminate();
		appPbVideoStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
		break;
		/*seamless video playback*/
	case APP_KEY_PRESS_TELE:
		seamlessPlayEn = TRUE;
		sp5kMediaPlayControl(SP5K_MEDIA_PLAY_SEAMLESS_EN, 1, 0);
		break;
	case APP_KEY_PRESS_WIDE:
		seamlessPlayEn = FALSE;
		sp5kMediaPlayControl(SP5K_MEDIA_PLAY_SEAMLESS_EN, 0, 0);
		break;
	default:
		break;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appPbVideoState
 *------------------------------------------------------------------------*/
void
appPbVideoState(
	UINT32 msg,
	UINT32 param
)
{
/* #if defined(PRJ_17DVR)
	UINT32 err;
#endif */
	switch (msg)
	{
	case APP_STATE_MSG_INIT:
		RPrintf("appPbVideoState");
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		if(appPbVideoStInit()==FAIL){
			if(!IS_CARD_EXIST){
				UINT32 err;
				err = sp5kHostMsgSend(INFO_QUEUEMSG_DISKSTATE, 0);
				HOST_ASSERT(err == SP5K_SUCCESS);
			}
		}
#else
		appPbVideoStInit();
#endif
		/* do nothing */
		break;
	case APP_STATE_MSG_CLOSE:
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		jet_pbvideo_stclose();
#endif
		appPbVideoStClose();
		appStateCloseDone();
		break;
	case APP_STATE_MSG_ABORT:
		appPbVideoTerminate();
		appPbVideoStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
	break;
	default:
		if (IS_APP_KEY_MSG(msg))
		{
			appPbVideoStOnKey(msg,param);
		}
		else
		{
			appPbVideoStOnMsg(msg,param);
		}
		break;
	}

}
