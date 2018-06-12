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
#include "sp5k_ae_api.h"
#include "sp5k_awb_api.h"
#include "sp5k_disp_api.h"
#include "sp5k_dcf_api.h"
#if SPCA5310
#include "api/sp5k_pip_api.h"
#endif
#include "app_view_param.h"
#include "app_playback_private.h"
#include "app_playback_def.h"
#include "app_playback_osd.h"
#include "app_lens_api.h"
#include "app_dpof.h"
#include "app_still.h"
#include "app_cdfs_api.h"
#include "app_menu_frameosd.h"
#include "app_state_ctrl.h"
#include "app_sensor.h"
#include "app_playback_uiparam.h"
#include "app_osd.h"
#include "app_video.h"
#include "app_view_osd.h"
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
#include "jet_utility.h"
#include "jet_cdfs.h"
#include "app_infohandler_feedback.h"
#include "app_infohandler_func.h"
#include "app_infohandler.h"
#endif
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define PB_LENS_CLOSE_TIME		15*1000 /* unit: ms */
#define WRITE8(a,b)	*(volatile UINT8*)(a) = (b)

#if SP5K_CDFS_OPT
#define CDFSINIT    1
#endif
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

typedef enum {
	PB_MAIN_SXN_START,
	PB_MAIN_SXN_END,
} pbMainKeySessionDef_t;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

#if (PB_PRIVATE_DBG_MASTER)
UINT8 szOsdStr[PB_TMP_OSD_STR_SIZE];
#endif

pbUIParam_t gpbUiParam = {0};       /* Use appPb_UiParamPtrGet() for extern reference */
pbMainCB_t	gpbMainCb = {0};	    /* Use appPb_MainCbPtrGet() for extern reference */
pbImgCb_t	gpbImgCb = {0};         /* Use appPb_ImageCbPtrGet() for extern reference */
pbDiskCB_t	gpbDiskCb;
pbImgExifInfo_t gpbExifInfo;        /* Use appPb_ExifInfoPtrGet() for extern reference */

static BOOL gpbIsViewIdle = TRUE;   /* Use appPb_IsViewIdle() for extern reference */
UINT32 gpbHistBuf[PB_HISTOGRAM_BAR_NUM]; /* the Histogram buffer */
BOOL gProtectAllFinish = TRUE;
static UINT32 previousViewState = 0;

extern appMenuInfo_t gPlayMainMenu;
extern UINT8 fileTypeSelect;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
extern BOOL jet_pb_image_pip_is_in_pip_get(void);
extern void jet_pb_image_pip_is_in_pip_set(BOOL set);
extern void jet_pb_image_pip_exit(void);
#endif
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
static UINT32 appPbMainEnter(UINT32 playImgIdx);

/*-------------------------------------------------------------------------
 *  File Name : appPb_IsViewIdle
 *------------------------------------------------------------------------*/
BOOL appPb_IsViewIdle(void)
{
	return gpbIsViewIdle;
}

/*-------------------------------------------------------------------------
 *  File Name : appMediaFileUpLoad
 *------------------------------------------------------------------------*/
UINT32
appMediaFileUpLoad(
	UINT8 *filename
)
{
	return 0;
}

/*-------------------------------------------------------------------------
 *  File Name : appPb_ExifInfoPtrGet
 *------------------------------------------------------------------------*/
pbImgExifInfo_t *appPb_ExifInfoPtrGet(void)
{
	return &gpbExifInfo;
}

/*-------------------------------------------------------------------------
 *  File Name : appPb_ImageCbPtrGet
 *------------------------------------------------------------------------*/
pbImgCb_t *appPb_ImageCbPtrGet(void)
{
	return &gpbImgCb;
}

/*-------------------------------------------------------------------------
 *  File Name : appPb_ImageCbPtrGet
 *------------------------------------------------------------------------*/
pbUIParam_t *appPb_UiParamPtrGet(void)
{
	return &gpbUiParam;
}

/*-------------------------------------------------------------------------
 *  File Name : appPb_ImageCbPtrGet
 *------------------------------------------------------------------------*/
pbMainCB_t *appPb_MainCbPtrGet(void)
{
	return &gpbMainCb;
}

/*-------------------------------------------------------------------------
 *  File Name : appPb_ImageCbPtrGet
 *------------------------------------------------------------------------*/
pbDiskCB_t *appPb_DiskCbPtrGet(void)
{
	return &gpbDiskCb;
}

/*-------------------------------------------------------------------------
 *  File Name : pbmainUIKeyControl
 *------------------------------------------------------------------------*/
static
void
pbmainUIKeyControl(
	UINT8 ctrl
)
{
	switch (ctrl){
	case PB_MAIN_SXN_START:
		appBtnDisable(BTN_LEFT|BTN_RIGHT);
		break;
	case PB_MAIN_SXN_END:
		appBtnEnable(BTN_LEFT|BTN_RIGHT);
		break;
	default:
        PB_DBG(LEVEL_WARN,"pbmain: %s: undefined key session",__FUNCTION__);
		break;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : pbmainKeyPreCheck
 *------------------------------------------------------------------------*/
static
UINT32
pbmainKeyPreCheck(
	UINT32 keyMsg
)
{
	DBG_PRINT("pbmain:%s: keyMsg(0x%08x) \n", __FUNCTION__, keyMsg);

	if (gpbIsViewIdle==FALSE || gpbMainCb.isIniting==TRUE)
	{
        PB_DBG(LEVEL_WARN,"pbmain: %s: %d ignore key (0x%08x) ", __FUNCTION__, __LINE__, keyMsg);
		return FAIL;
	}

	if (gpbImgCb.isFileErr)
	{
		if (keyMsg==APP_KEY_PRESS_TELE && keyMsg==APP_KEY_PRESS_WIDE)
		{
            PB_DBG(LEVEL_WARN,"pbmain: %s: %d ignore key (0x%08x)", __FUNCTION__, __LINE__, keyMsg);
			return FAIL;
		}
	}
	return SUCCESS;

}

/*-------------------------------------------------------------------------
 *  File Name : playInitCreate
 *------------------------------------------------------------------------*/
static
void
playInitCreate(
	void
)
{

	DBG_PRINT("pbmain: %s \n", __FUNCTION__);

	//pbUIParam_t *ppbUiParam = appPb_UiParamPtrGet();
    UINT32 temp = gpbMainCb.lastImgIdx;

	memset(&gpbMainCb, 0, sizeof(pbMainCB_t));
	memset(&gpbDiskCb, 0, sizeof(pbDiskCB_t));

	if(!((appPreviousStateGet() == APP_STATE_MENU)&&(fileTypeSelect == 1)))/*ez_menu for pb filetype */
            gpbMainCb.lastImgIdx = temp;

	gpbImgCb.fileType = gpbMainCb.lastFileType = PB_FT_UNKNOWN;
	gpbMainCb.closeLensTmrId = TIMER_NULL;
	gpbMainCb.asyncOpTmrId = TIMER_NULL;
	gpbMainCb.isIniting = TRUE;

	sp5kExifCfgSet(SP5K_EXIF_CFG_MPF_SUPPORT, 1); /* support mpf big thumb */
	sp5kDcfFsFileTypeSet(APP_PB_FILETYPE_ALL);
	sp5kZoomFactorSet(SP5K_MODE_STILL_PLAYBACK, 1000);

 	#if (KIT_WITH_HDMI)
	if (IS_HDMI_IN)
	{
		
		sp5kStillPlayCfgSet(SP5K_SPB_RING_BUFFER_MAX_NUM,6);
		#if SP5K_CDFS_OPT
		pbUIParam_t *ppbUiParam = appPb_UiParamPtrGet();
		if(ppbUiParam->selCdfs == CDFS_FILE_FOLDER_VIDEO){
			sp5kModeSet(SP5K_MODE_STANDBY);
			sp5kModeWait(SP5K_MODE_STANDBY);
			appPbPIPInit();
		}
		if((ppbUiParam->selCdfs == CDFS_FILE_FOLDER_JPG) && (appPbYUVPIPStatusGet() == TRUE)){
			AppPbDestroyshowJPGInPIP();
		}
		#else
		sp5kModeSet(SP5K_MODE_STANDBY);
		sp5kModeWait(SP5K_MODE_STANDBY);
		appPbPIPInit();
		#endif
	}
	else
	{
		sp5kStillPlayCfgSet(SP5K_SPB_RING_BUFFER_MAX_NUM,8);

	}
	#endif
 }

/*-------------------------------------------------------------------------
 *  File Name : appPbMainStIamgeShift
 *------------------------------------------------------------------------*/
static
void
appPbMainStImageShift(
	UINT8 shiftCtrl
)
{
	DBG_PRINT("pbmain: Shift (%s) \n", (shiftCtrl==PB_SHIFT_NEXT ? "NEXT" : "PRE"));

	pbmainUIKeyControl(PB_MAIN_SXN_START);

	switch (shiftCtrl) {
	case PB_SHIFT_NEXT:
	case PB_SHIFT_PRE:
		/*-- Clear center msg before shifting image */
		if (gpbImgCb.isFileErr)
		{
			appPbMsg_Clear();
		}
        /*
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeClear(APP_OSD_SHAPE_RECT,
									0, 40,
									320, 200, 0, 0);
        */

		if (FAIL==appPbImageShift(shiftCtrl))
		{
			/*-- For PbMain, the only shift_error shall be FILE_ERROR !!
			 * otherwise, it should be a BUG !?
			 */
			appPbErrorProcess(PB_ERR_FILE_ERROR);
		}

		/*-- UPDATE UI
		if((gpbImgCb.fileType == PB_FT_AVI)||(gpbImgCb.fileType == PB_FT_WAV))
		{
			if(gpbMainCb.osdType==PB_DISP_FULL)
			{
				gpbMainCb.osdType = PB_DISP_BASIC;
			}
		}*/

		if (gpbMainCb.postponeUI==0)
		{
#if !defined(PRJ_17DVR)&&!defined(PRJ_19DVR)
			appPlayOsd_MainInfoDraw(PB_OSD_DRAW_NORMAL);
#endif
		}
		else
		{
			gpbMainCb.postponeUI=2; /* PB_OSD_DRAW_NORMAL */
		}
		/*-- Emulate Key Repeat */
		if((gpbImgCb.fileType == PB_FT_AVI)||(gpbImgCb.fileType == PB_FT_WAV)||(gpbImgCb.fileType == PB_FT_MOV)||(gpbImgCb.fileType == PB_FT_MP4))
		{
			appPbImageShiftRepeatCheck(0);
		}
		break;
	default:
		DBG_PRINT("pbmain: undefined shiftCtrl(%d) \n", shiftCtrl);
		break;
	}

	pbmainUIKeyControl(PB_MAIN_SXN_END);

}

/*-------------------------------------------------------------------------
 *  File Name : appPbMainEnter
 *------------------------------------------------------------------------*/
static
UINT32
appPbMainEnter(
	UINT32 playImgIdx
)
{
	/*-- argument playImgIdx is used to override the
	 * DCF image index to play at creation.
	 * If playImgIdx==0, use the last image for play,
	 * otherwise, use designated index.
	 */

	DBG_PRINT("pbmain: %s: playImgIdx(%d) \n", __FUNCTION__, playImgIdx);

	/*-- while calling appPbMainEnter(),
	 * DISK shall have been mounted !!
	 */
	/*-- check up Disk mounted successfully !! */
	if (gpbDiskCb.isDiskErr)
	{
		gpbMainCb.err = PB_ERR_DISK_ERROR;
		appPbErrorProcess(PB_ERR_DISK_ERROR);
		return FAIL;
			/*-- Return FAIL to indicate that appPbMainEnter()
			 * did not play any image.
			 */
	}

	/*-- Checkup & Renew Disk status */
	appPbDiskCheckup(&gpbDiskCb);

	/*-- check up IMG count */
	gtotalImgCount = appPbTotalFileCountGet();
	if (!gtotalImgCount)
	{
		gpbMainCb.err = PB_ERR_NO_IMAGE;
		appPbErrorProcess(PB_ERR_NO_IMAGE);
		return FAIL;
			/*-- Return FAIL to indicate that appPbMainEnter()
			 * did not play any image.
			 */
	}
	gpbMainCb.isNoImage = FALSE;

	/*-- Go on & playback image */
	playImgIdx = (playImgIdx==0 ? gtotalImgCount : playImgIdx);

	PB_DBG(LEVEL_INFO,"pbmain: Playing %d of %d ...", playImgIdx, gtotalImgCount);

	gpbImgCb.isFileErr = FALSE;
	
	if (FAIL==appPbImagePlay(playImgIdx))
	{
		gpbMainCb.err = PB_ERR_FILE_ERROR;
		gpbImgCb.isFileErr = TRUE;/*willow++ for clear the video"file error" when shift*/
		appPbErrorProcess(PB_ERR_FILE_ERROR);
		return FAIL;
			/*-- Return FAIL to indicate that appPbMainEnter()
			 * did not play any image.
			 */
	}
	#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	extern BOOL jet_pb_video_file_udata_info_get(void);
	if(gpbImgCb.fileType != PB_FT_JPG){
		if(jet_pb_video_file_udata_info_get()==FAIL){
			return FAIL;
		}
	}
	#endif

	gpbMainCb.postponeUI=0;
	if (IS_FILETYPE_JPG(gpbImgCb.dcfAttr.fileType))
	{
		/*-- Playing JPG image, shall postpone UI OSD
		 * till the PLAYBACK_DONE message.
		 */
		gpbMainCb.postponeUI=1;
	}
	gpbMainCb.err = PB_ERR_NONE;
	return SUCCESS;
			/*-- Return SUCCESS to indicate that appPbMainEnter()
			 * has fired the image playback.
			 */

}

/*-------------------------------------------------------------------------
 *  File Name : appPbMainInit
 *------------------------------------------------------------------------*/
static
UINT32
appPbMainInit(
	void
)
{

	PB_DBG(LEVEL_INFO,"\n\npbmain: %s: mainStat(%d), Idx=%d,%d  pre=0x%X\n", __FUNCTION__, gpbMainCb.mainStat, gpbMainCb.curImgIdx, gpbMainCb.lastImgIdx, appPreviousStateGet());
    PB_CLR_ALL;

	if(pUiSetting->pbfileType == PB_FILETYPE_IMAGE)
	{
		gpbMainCb.osdType = PB_DISP_BASIC;
		appPlayOsd_MainInfoDraw(PB_OSD_DRAW_NORMAL);
	}

	gpbMainCb.postponeUI=0;
		/* Do need to wait playback_done msg for UI update by default */

	switch (gpbMainCb.mainStat)
	{
	case PB_MAIN_STAT_NULL:
		DBG_PRINT("pbmain: PB_MAIN_STAT_NULL\n");
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
			if(!IS_CARD_EXIST)
			break;
#else
		APP_OSD_REFRESH_OFF;
#endif
#if JET_PANA_CDFS
		if(jetFolderFilterTotal() != 0 &&
				appPbMainEnter(jet_cdfs_pb_dcf_index_get())==FAIL){
			appReportErrorStatusFB(1, 3);
			appFilePBStartFB(0);
			gpbMainCb.err = PB_ERR_FILE_ERROR;
			appStateChange(APP_STATE_VIDEO_PREVIEW,STATE_PARAM_NORMAL_INIT);
			break;
		}
		sp5kHostMsgSend(APP_KEY_PRESS_SET,0);
#else
	if(!((appPreviousStateGet() == APP_STATE_MENU)&&(fileTypeSelect == 1)))/*ez_menu for pb filetype */
            appPbMainEnter(gpbMainCb.lastImgIdx);
        else
       		appPbMainEnter(0);

		fileTypeSelect=0;

		if(IS_CARD_EXIST && (gtotalImgCount!=0) &&
			(gpbMainCb.err != PB_ERR_DISK_ERROR) &&
			pUiSetting->pbfileType != PB_FILETYPE_IMAGE)
		{
				#if 1
#if !defined(PRJ_17DVR)&&!defined(PRJ_19DVR)
				appPlayOsd_VideoInfoDraw();
#endif
				#else
                DBG_PRINT("pbmain: %d %d %d %d\n",gpbMainCb.lastImgIdx,gpbMainCb.curImgIdx,gPlayVideoMenu.curVTab,gPlayVideoMenu.lastVTab);
                gPlayVideoMenu.curVTab = (gpbMainCb.curImgIdx==gtotalImgCount ? 0 : gtotalImgCount - gpbMainCb.curImgIdx);
                DBG_PRINT("pbmain: %d %d %d %d\n",gpbMainCb.lastImgIdx,gpbMainCb.curImgIdx,gPlayVideoMenu.curVTab,gPlayVideoMenu.lastVTab);

                appPlayMenu_VideoMainMenuCreate();
    			if (gPlayVideoMenu.curVTab == 0 )
    			{
    				appPlayMenu_DrawVideoMainMenu();
    			}
    			else
    			{
    				appMenu_TKMenuPBVideoMenuSelectFrameDraw(TRUE);
    				appMenu_TKPlayOsdVideoMenuMainMenuSelChange(1);
    			}
				#endif
		}
#endif
#if !defined(PRJ_17DVR)&&!defined(PRJ_19DVR)
		APP_OSD_REFRESH_ON;
#endif
		break;

	case PB_MAIN_STAT_MENU_SUSPEND:
		DBG_PRINT("pbmain: PB_MAIN_STAT_MENU_SUSPEND\n");
		if (gtotalImgCount != 0)
		{
			/*-- PB Menu is returning from different image.
			 * We shall replay the last image.
			 */
#if !defined(PRJ_17DVR)&&!defined(PRJ_19DVR)
			APP_OSD_REFRESH_OFF;
			appPbMainEnter(gcurrDcfIdx);
			if(pUiSetting->pbfileType != PB_FILETYPE_IMAGE){
				appPlayOsd_VideoInfoDraw();
			}
			APP_OSD_REFRESH_ON;
#endif
			break;
		}
		else
		{
			/*-- The MENU return to same IMG.
			 * Do not need to replay image.
			 */
			if (gpbDiskCb.isDiskErr)
			{
				//appPbErrorProcess(PB_ERR_DISK_ERROR);
				gpbMainCb.err = PB_ERR_CARD_ERROR;
				appErrMsgWithSound(ERR_MSG_CARD_ERROR, ERR_MSG_WITH_WARNING_SOUND, PB_ERR_SHOW_FOREVER);
				break;
			}
			else if (gtotalImgCount==0)
			{
				//appPbErrorProcess(PB_ERR_NO_IMAGE);
				gpbMainCb.err = PB_ERR_NO_IMAGE;
				gpbMainCb.isNoImage = TRUE;
				appErrMsgShow(ERR_MSG_NO_IMG, PB_ERR_SHOW_FOREVER);
				break;
			}
			else if (gpbImgCb.isFileErr)
			{
				//appPbErrorProcess(PB_ERR_FILE_ERROR);
				gpbMainCb.err = PB_ERR_FILE_ERROR;
				appErrMsgShow(ERR_MSG_FILE_ERROR, PB_ERR_SHOW_FOREVER);
				break;
			}
		}
		break;

	case PB_MAIN_STAT_ZPAN_SUSPEND:
		DBG_PRINT("pbmain: PB_MAIN_STAT_ZPAN_SUSPEND\n");
		appPbMainEnter(gcurrDcfIdx);
		break;

	case PB_MAIN_STAT_MPLAY_SUSPEND:
		DBG_PRINT("pbmain: PB_MAIN_STAT_MPLAY_SUSPEND\n");
		if (gtotalImgCount==0)
		{
			appPbErrorProcess(PB_ERR_NO_IMAGE);
		}
		else
		{
			appPbImagePlay(gcurrDcfIdx);
			if (IS_FILETYPE_JPG(gpbImgCb.dcfAttr.fileType))
			{
				gpbMainCb.postponeUI = 1;
			}
		}
		break;

	case PB_MAIN_STAT_IMG_PROT_SUSPEND:
		DBG_PRINT("pbmain: PB_MAIN_STAT_IMG_PROT_SUSPEND\n");
		break;

	case PB_MAIN_STAT_IMG_DEL_SUSPEND:
		DBG_PRINT("pbmain: PB_MAIN_STAT_IMG_DEL_SUSPEND\n");
		appPbMainEnter(gpbMainCb.lastImgIdx);
		break;

	case PB_MAIN_STAT_C2C_SUSPEND:
		DBG_PRINT("pbmain: PB_MAIN_STAT_C2C_SUSPEND\n");
		appPbMainEnter(gpbMainCb.lastImgIdx);
		break;

	case PB_MAIN_STAT_SLIDE_SUSPEND:
		DBG_PRINT("pbmain: PB_MAIN_STAT_SLIDE_SUSPEND\n");
		appPbMainEnter(gpbMainCb.curImgIdx);
		break;

	case PB_MAIN_STAT_JPG_ROTATE:
		DBG_PRINT("pbmain: PB_MAIN_STAT_JPG_ROTATE(0x%0x)\n", gpbMainCb.mainStat);
		appPbMainEnter(gpbMainCb.lastImgIdx);
		break;

	case PB_MAIN_STAT_JPG_EFFECT:
	case PB_MAIN_STAT_JPG_PHOTOFRAME:
	case PB_MAIN_STAT_JPG_RESIZE:
		DBG_PRINT("pbmain: PB_MAIN_STAT_JPG_XXX(0x%0X)\n", gpbMainCb.mainStat);
		appPbMainEnter(gpbMainCb.lastImgIdx);
		break;

	case PB_MAIN_STAT_VIDEO_SUSPEND:
		DBG_PRINT("pbmain: PB_MAIN_STAT_VIDEO_SUSPEND(0x%0x)\n", gpbMainCb.mainStat);
        appVideoRecStampExit();
#if !defined(PRJ_17DVR)&&!defined(PRJ_19DVR)
		appOsd_GfxFontIDSet(SP5K_RES_OSD_FONT);
		APP_OSD_REFRESH_OFF;
		appPbMainEnter(gcurrDcfIdx);

		appPlayOsd_VideoInfoDraw();

		APP_OSD_REFRESH_ON;
#endif
		break;

	case PB_MAIN_STAT_DPOF_SUSPEND:
	case PB_MAIN_STAT_MEMOREC_SUSPEND:
		DBG_PRINT("pbmain: PB_MAIN_STAT_MEMOREC_SUSPEND(0x%0x)\n", gpbMainCb.mainStat);
		appPbMainEnter(gpbMainCb.lastImgIdx);
		break;

	default:
		DBG_PRINT("pbmain: unknown mainStat(0x%x)\n", gpbMainCb.mainStat);
	}

	if (gpbMainCb.postponeUI==0 &&
		IS_CARD_EXIST && gtotalImgCount!=0 &&
		(gpbMainCb.err != PB_ERR_DISK_ERROR))
	{
		appPlayOsd_MainInfoDraw(PB_OSD_DRAW_NORMAL);
	}

	gpbMainCb.mainStat = PB_MAIN_STAT_STANDBY;

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbMainState_Close
 *------------------------------------------------------------------------*/
static
void
appPbMainState_Close(
	void
)
{
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	if(jet_pb_image_pip_is_in_pip_get()==TRUE){
		jet_pb_image_pip_exit();
	}
	PlayPosCountStop();
	//appInfoPanaStateSet(PANA_STATE_PLAY_IDLE);
#endif
	gpbMainCb.mainStat = PB_MAIN_STAT_NULL;
	gpbIsViewIdle = TRUE;
    gpbMainCb.isIniting = FALSE;
    if ((STATE_GROUP(appNextStateGet())!=APP_STATE_GROUP_PLAYBACK) && (STATE_GROUP(appNextStateGet())!=APP_STATE_GROUP_MENU))
        gpbMainCb.lastImgIdx = 0;

	/* If host flow is PowerOn¡÷Still¡÷PB¡÷Still view, modeSet will 0x11¡÷0x88¡÷0x11, user will see the black screen immediately and
	    disappear, so call the API wiil avoid this phenomenon */

	#if (KIT_WITH_HDMI)
	//AppPbDestroyshowJPGInPIP();
	#endif
	if (appLensQuery(LENS_QUERY_MOTION_STATE))
	{
		//appLensStopWait();
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appPbMainState_Init
 *------------------------------------------------------------------------*/
static
void
appPbMainState_Init(
	void
)
{
	DBG_PRINT("pbmain: %s: \n", __FUNCTION__);
	/*-- Check if DISK READY */
	if (!IS_DISK_MOUNT_READY )
	{
        PB_DBG(LEVEL_WARN,"pbmain: %s: DISK Not Ready",__FUNCTION__);
		gpbMainCb.err = PB_ERR_DISK_NOT_READY;
		gpbDiskCb.isDiskReady = FALSE;
		sp5kHostMsgSend(APP_UI_MSG_PB_WAIT_DISK, 0 /* timeout counter */);
		return;
	}
	gpbDiskCb.isDiskReady = TRUE;

	sp5kHostMsgSend(APP_UI_MSG_PB_CHECK_DISK, 0);
	return;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbMainState_OnMsg
 *------------------------------------------------------------------------*/
static
void
appPbMainState_OnMsg(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 tmpU32;

	DBG_PRINT("pbmain: PB_OnMsg: 0x%08x, 0x%08x ... \n", msg, param);

	switch (msg) {
	case APP_UI_MSG_PB_WAIT_DISK:
        PB_DBG(LEVEL_DEBUG,"pbmain: %s pb wait disk(0x%08x)",__FUNCTION__, param);
		if (!IS_DISK_MOUNT_READY)
		{
			if (param>(10000/PB_WAIT_DISK_INTERVAL))
			{
				PB_DBG(LEVEL_ERROR,"pbmain: disk mount timed out");
			}
			APP_SLEEP_MS(PB_WAIT_DISK_INTERVAL);
			sp5kHostMsgSend(APP_UI_MSG_PB_WAIT_DISK, param+1);
			return;
		}

		gpbDiskCb.isDiskReady = TRUE;
		sp5kHostMsgSend(APP_UI_MSG_PB_CHECK_DISK, 0);
		break;

	case APP_UI_MSG_PB_CHECK_DISK:
        PB_DBG(LEVEL_DEBUG,"pbmain: %s pb check disk(0x%08x)",__FUNCTION__, param);
		/*-- DISK mount completed, go & and checkup disk */
		gpbDiskCb.isDiskErr = FALSE;
		if (IS_DISK_MOUNT_FAIL)
		{
			PB_DBG(LEVEL_ERROR,"pbmain: %s: disk mount failed", __FUNCTION__);
			gpbMainCb.err = PB_ERR_DISK_ERROR;
			gpbDiskCb.isDiskErr = TRUE;
		}
		else
		{
			gtotalImgCount = appPbTotalFileCountGet();
			gpbMainCb.isNoImage = FALSE;
			if (gtotalImgCount==0)
			{

				gpbMainCb.err = PB_ERR_NO_IMAGE;
				gpbMainCb.isNoImage = TRUE;
			}
			else
			{ //mantis 0024588: dpof init only when image coount > 0
				/*-- Init DPOF */
                #if SP5K_CDFS_OPT
#if !defined(PRJ_17DVR)&&!defined(PRJ_19DVR)
				appCdfsFolderInit(appCdfsFileFolderGet());

				appPbFilePageCtrlParaInit();
#endif
                #else
				appDpofInit();
                #endif
			}
		}

		sp5kHostMsgSend(APP_UI_MSG_PB_CREATE, 0);
		break;

	case APP_UI_MSG_PB_CREATE:
        PB_DBG(LEVEL_DEBUG,"pbmain: %s pb create(0x%08x)",__FUNCTION__, param);
		gpbIsViewIdle = TRUE;
		tmpU32 = appPreviousStateGet();
		if (STATE_GROUP(tmpU32)==APP_STATE_GROUP_VIEW
			||STATE_GROUP(tmpU32)==APP_STATE_GROUP_VIDEO  )
		{
			/*-- Shutdown SP5K Live View, if necessary */
			if (gpbMainCb.isNoImage==TRUE || gpbDiskCb.isDiskErr==TRUE)
			{
				PB_DBG(LEVEL_INFO,"pbmain: IDLE to shutdown preview %d %d",gpbMainCb.isNoImage,gpbDiskCb.isDiskErr);
				appModeSet(SP5K_MODE_STANDBY);
				gpbIsViewIdle = FALSE;
					/*-- INIT Flow is suspended here to wait for IDLE ready.
					 * appGroupPlaybackProc() will intercept MODE_RDY(IDLE)
					 * and send APP_UI_MSG_PB_INIT to resume the initialization
					 * flow !!
					 */
				return;
			}
		}

		if (gpbIsViewIdle==TRUE)
		{
			sp5kHostMsgSend(APP_UI_MSG_PB_INIT, 1 /* CREATE Triggered */);
		}
		break;

	case APP_UI_MSG_PB_INIT:
        PB_DBG(LEVEL_INFO,"pbmain: %s pb init(0x%08x)",__FUNCTION__, param);
		gpbIsViewIdle = TRUE;
		gpbMainCb.isIniting = FALSE;
		appPbMainInit();
		break;

	case SP5K_MSG_STILLPLAY_DONE:
		/*-- NOTE :
		 *   Common Error Process has been done in
		 *   appGroupPlaybackProc() @ app_state_preproc.c
		 */
        PB_DBG(LEVEL_DEBUG,"pbmain: %s pb play done(0x%08x)",__FUNCTION__, param);
		tmpU32 = appPbStillPlayDoneParse(param);
		if (tmpU32 & PB_DONE_ERROR)
		{
            PB_DBG(LEVEL_DEBUG,"pbmain: %s play done error(0x%08x)",__FUNCTION__, gpbMainCb.err);
			appPbErrorProcess(gpbMainCb.err);
			break;
		}

		if ( (IS_PB_QV_PLAYING && tmpU32==PB_DONE_QV_FINISH)
			|| (IS_PB_THUMB_PLAYING && tmpU32==PB_DONE_THUMB_FINISH) )
		{
			DBG_PRINT("pbmain: THUMB/QV Done !!\n");
			appPlayOsd_MainInfoDraw(PB_OSD_DRAW_NORMAL);
			gpbImgCb.isThumbImgOK = TRUE;
			if (IS_PB_THUMB_PLAYING
				&& tmpU32==PB_DONE_THUMB_FINISH)
			{
				appPbJpgMainPlayStart();
			}
			else
			{
				gpbMainCb.isPlayDone = TRUE;
				gpbImgCb.isMainImgOK = TRUE;
				gpbMainCb.mainStat = PB_MAIN_STAT_STANDBY;
				gpbMainCb.playStat = PB_PLAY_STAT_STANDBY;
			}

		}
		else if (IS_PB_MAIN_PLAYING && tmpU32==PB_DONE_MAIN_FINISH)
		{
			DBG_PRINT("pbmain: MAIN Done !!\n");
			/*-- Update OSD ASAP !! */
			if (gpbImgCb.isThumbImgOK==FALSE)
			{
				appPlayOsd_MainInfoDraw(PB_OSD_DRAW_NORMAL);
			}
			/*-- Update flags */
			gpbMainCb.playStat = PB_PLAY_STAT_STANDBY;
			gpbMainCb.isPlayDone = TRUE;
			gpbImgCb.isMainImgOK = TRUE;
			gpbMainCb.mainStat = PB_MAIN_STAT_STANDBY;
		}

		appPbImageShiftRepeatCheck(0);
		break;
	case APP_UI_MSG_PB_AUDIO_TITLE_DONE:
        appPlayOsd_AudioBGDraw(TRUE);
		break;
	case APP_UI_MSG_PB_SHIFT_NEXT:
	case APP_UI_MSG_PB_SHIFT_PRE:
		#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		if(param == 0){
			jet_file_list_filter_current_play_set(jet_cdfs_pb_image_next_index_get());
		}
		if(appPbMainEnter(jet_cdfs_pb_dcf_index_get())==FAIL){
			appReportErrorStatusFB(1, 3);
			appFilePBStartFB(1);
			gpbMainCb.err = PB_ERR_FILE_ERROR;
			break;
		}
		appReportErrorStatusFB(1, 0);
		appFilePBStartFB(1);
		break;
		#else
		if (gpbMainCb.playStat != PB_PLAY_STAT_STANDBY)
		{
			break;
		}

		if (gpbMainCb.isNoImage == TRUE)
		{
			break;
		}
		appPbMainStImageShift((msg==APP_UI_MSG_PB_SHIFT_NEXT ?
									PB_SHIFT_NEXT :
									PB_SHIFT_PRE));
		#endif
		break;
	case SP5K_MSG_DCF_FILEOP_COMPLETE:
		gProtectAllFinish = TRUE;
		break;
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	case JET_APP_UI_MSG_PB_STOP:
		printf("[JET_APP_UI_MSG_PB_STOP] \n");
		if(jet_pb_image_pip_is_in_pip_get()==TRUE){
			jet_pb_image_pip_exit();
		}
		PlayPosCountStop();
		//appInfoPanaStateSet(PANA_STATE_PLAY_IDLE);
		appFilePBStopFB(1, jet_avn_pb_para_get(), jet_file_list_filter_current_play_get()-1);	
		break;
#endif
	default :
		break;
	}
}


/*-------------------------------------------------------------------------
 *  File Name : appPbMainState_OnKey
 *------------------------------------------------------------------------*/
static
void
appPbMainState_OnKey(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 tmpU32;

	DBG_PRINT("pbmain: %s : [0x%x] [0x%x]\n",__FUNCTION__,msg,param);

	if (FAIL==pbmainKeyPreCheck(msg))
	{
		DBG_PRINT("pbmain: %s: Key(0x%x) ignored ...\n", __FUNCTION__, msg);
		return;
	}

	if(!gProtectAllFinish && appPreviousStateGet() == APP_STATE_PB_IMG_PROT)
		return;

	switch (msg) {
	case APP_KEY_PRESS_S2:
	case APP_KEY_PRESS_DEL:
		break;

    case APP_KEY_PRESS_S1:
        HOST_PROF_LOG_ADD(LEVEL_INFO, "pbmain: s1 key press");
    case APP_KEY_PRESS_MODE:
        HOST_PROF_LOG_ADD(LEVEL_INFO, "pbmain: mode key press");
	case APP_KEY_PRESS_PB:
		#if (KIT_WITH_HDMI)
		if(IS_HDMI_IN)
		{
			AppPbDestroyshowJPGInPIP();
		}
		#endif
		#if 0
        memset(&gPlayVideoMenu, 0, sizeof(appMenuVideoInfo_t));
		#endif
		appUiModeChange();
		break;

	case APP_KEY_PRESS_MENU:
#if defined(PRJ_PDVR) //Isaac add to back play back menu
		appUiModeChange();
		printf("PB_MAIN APP_KEY_PRESS_MENU\n");
#else
	DBG_PRINT("pbmain: PB_MAIN: APP_KEY_PRESS_MENU\n");
		
        if(gtotalImgCount == 0 || gpbMainCb.err == PB_ERR_DISK_ERROR)
			break;

		CLEAR_OSD_SRC;

        if(pUiSetting->pbfileType != PB_FILETYPE_IMAGE){
			AppPbDestroyshowJPGInPIP();
		}

		if(gtotalImgCount!=0)
		{
			gpbMainCb.mainStat = PB_MAIN_STAT_MENU_SUSPEND;
			gpbMainCb.lastImgIdx = gcurrDcfIdx;
			sp5kModeSet(SP5K_MODE_STANDBY);
			sp5kModeWait(SP5K_MODE_STANDBY);
			appStateChange(APP_STATE_MENU, STATE_PARAM_NORMAL_INIT);
		}
#endif
		break;

	case APP_KEY_PRESS_SET:
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	tmpU32 = gpbImgCb.dcfAttr.fileType;
#else
        appBeep(TRUE, FALSE);
#endif
		if (gpbMainCb.err == PB_ERR_FILE_ERROR)
		{
			printf("FILE ERROR !!\n");
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
			appStateChange(APP_STATE_VIDEO_PREVIEW, STATE_PARAM_NORMAL_INIT);
#endif
			break;
		}
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		if(IS_FILETYPE_JPG(tmpU32)){
			appReportErrorStatusFB(1, 0);
			appFilePBStartFB(1);
			PlayPosCountStart();
			appInfoPanaStateSet(PANA_STATE_PLAY_IDLE);
		}
#endif
		if (!gpbMainCb.isPlayDone)
		{
			printf("Playing busy!!, Ignore ... \n");
			break;
		}
#if !defined(PRJ_17DVR)&&!defined(PRJ_19DVR)
		tmpU32 = gpbImgCb.dcfAttr.fileType;
#endif
		if (IS_FILETYPE_VIDEO(tmpU32))
		{
			gpbMainCb.lastImgIdx = gcurrDcfIdx;
			gpbMainCb.mainStat = PB_MAIN_STAT_VIDEO_SUSPEND;
			appStateChange(APP_STATE_PB_VIDEO, STATE_PARAM_NORMAL_INIT);
		}
		break;

	case APP_KEY_PRESS_UP:
    case APP_KEY_REPEAT_UP:
		if (gpbMainCb.playStat != PB_PLAY_STAT_STANDBY && pUiSetting->pbfileType == PB_FILETYPE_VIDEO)
		{
			break;
		}
		if (gpbMainCb.isNoImage == TRUE || gpbMainCb.err == PB_ERR_CARD_ERROR)
		{
			break;
		}
#if !defined(PRJ_17DVR)&&!defined(PRJ_19DVR)
		APP_OSD_REFRESH_OFF;
		if(pUiSetting->pbfileType != PB_FILETYPE_IMAGE)
		{
			appViewOsd_AllClean();	  /*fix bug ICOM-2391*/
		}
#endif
		#if 0
	    appPbMenu_MainMenuInfoDecreaseSet(&gPlayVideoMenu);
	    #endif
		appPbMainStImageShift(PB_SHIFT_NEXT);
		if(pUiSetting->pbfileType != PB_FILETYPE_IMAGE)
		{
			#if 1
#if !defined(PRJ_17DVR)&&!defined(PRJ_19DVR)
			appPlayOsd_VideoInfoDraw();
#endif
			#else
		if (gPlayVideoMenu.curVTab == 0 )
		{
			appPlayMenu_DrawVideoMainMenu();
		}
		else
		{
			appMenu_TKMenuPBVideoMenuSelectFrameDraw(TRUE);
			appMenu_TKPlayOsdVideoMenuMainMenuSelChange(1);
		}
			#endif
		}
		else
		{
			//appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 6,45,36, 32, 0, 0); /*useless?*/
		}
#if !defined(PRJ_17DVR)&&!defined(PRJ_19DVR)
		APP_OSD_REFRESH_ON;
#endif
		break;

	case APP_KEY_PRESS_DOWN:
	case APP_KEY_REPEAT_DOWN:
		if (gpbMainCb.playStat != PB_PLAY_STAT_STANDBY && pUiSetting->pbfileType == PB_FILETYPE_VIDEO)
		{
			break;
		}
		if (gpbMainCb.isNoImage == TRUE || gpbMainCb.err == PB_ERR_CARD_ERROR)
		{
			break;
		}
		APP_OSD_REFRESH_OFF;
		if(pUiSetting->pbfileType != PB_FILETYPE_IMAGE)
		{
#if !defined(PRJ_17DVR)&&!defined(PRJ_19DVR)
			appViewOsd_AllClean();
#endif
		}
		#if 0
		appPbMenu_MainMenuInfoIncreaseSet(&gPlayVideoMenu);
		#endif
		appPbMainStImageShift(PB_SHIFT_PRE);
		if(pUiSetting->pbfileType != PB_FILETYPE_IMAGE)
		{
			#if 1
#if !defined(PRJ_17DVR)&&!defined(PRJ_19DVR)
			appPlayOsd_VideoInfoDraw();
#endif
			#else
		if (gPlayVideoMenu.curVTab == 0 )
		{
			appPlayMenu_DrawVideoMainMenu();
		}
		else
		{
			appMenu_TKMenuPBVideoMenuSelectFrameDraw(TRUE);
			appMenu_TKPlayOsdVideoMenuMainMenuSelChange(0);
		}
			#endif
		}
		else
		{
			//appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,6,45,36, 32, 0, 0); /*useless?*/
		}
		APP_OSD_REFRESH_ON;
		break;
	case APP_KEY_PRESS_LEFT:
	case APP_KEY_PRESS_RIGHT:
	case APP_KEY_PRESS_TELE:
	case APP_KEY_PRESS_WIDE:
		break;

	}

	switch (msg)
	{
		case APP_KEY_PRESS_UP:
		case APP_KEY_REPEAT_UP:
		case APP_KEY_PRESS_DOWN:
		case APP_KEY_REPEAT_DOWN:
        case APP_KEY_PRESS_S1:
		case APP_KEY_PRESS_MODE:
		case APP_KEY_PRESS_PB:
		case APP_KEY_PRESS_MENU:
		case APP_KEY_PRESS_DEL:
		#if !defined(PRJ_17DVR)&&!defined(PRJ_19DVR)
		appBeep(TRUE, FALSE);
		#endif
		break;
		default:
			  break;
	}

}

void
appPreviousViewStateSet(
	UINT32 state
)
{
	previousViewState = state;

}

UINT32
appPreviousViewStateGet(
	void
)
{
	return previousViewState;
}


#if SP5K_CDFS_OPT
/*-------------------------------------------------------------------------
 *  File Name : appPbCdfsFileScan
 *------------------------------------------------------------------------*/
UINT8 appPbCdfsFileScan(void)
{
    CDFS_FILE_TAB_T *pObj;
    UINT8 i=0;
    UINT8 video_file[32],video_size=0;
	UINT8 jpg_file[32],jpg_size=0;
	//char lastname[64];
	//UINT32 len;

	pbUIParam_t *ppbUiParam = appPb_UiParamPtrGet();

    for(i=CDFS_FILE_FOLDER_VIDEO; i<=CDFS_FILE_FOLDER_JPG; i++)
    {
        pObj= appCdfsFileListHeadGet(i);

    	while(pObj)
    	{
    		if(pObj->next)
    			pObj=pObj->next;
    		else
    			break;
    	}

        if(i==CDFS_FILE_FOLDER_VIDEO && pObj!=NULL)
        {
            sprintf((char *)video_file, "%s", (char *)pObj->FileName);
            video_size = strlen((const char *)video_file);
        }

        if(i==CDFS_FILE_FOLDER_JPG && pObj!=NULL)
        {
            sprintf((char *)jpg_file, "%s", (char *)pObj->FileName);
            jpg_size = strlen((const char *)jpg_file);
        }
    }

    if(video_size==0 && jpg_size==0)
    {
		UINT32 len;
		char lastname[64];
        sprintf((char *)lastname, "%s", (char *)appCdfsLastFileNameGet());
        len = strlen(lastname);

        if(len!=0)
        {
            printf("Cdfs fileget mode lastname %s len=%u -\n",lastname ,len);

    	    if((lastname[len-3] == 'J'|| lastname[len-3] == 'j') && \
           	   (lastname[len-2] == 'P'|| lastname[len-2] == 'p') && \
           	   (lastname[len-1] == 'G'|| lastname[len-1] == 'g') \
              )
    	    {
                jpg_size = len;
                return CDFS_FILE_FOLDER_JPG;
    	    }
            else
            {
                video_size = len;
                return CDFS_FILE_FOLDER_VIDEO;
            }
            HOST_PROF_LOG_PRINT(LEVEL_INFO, "pbmain: %s pb lastname %s", __FUNCTION__,lastname);
        }
        else
        {
            HOST_PROF_LOG_PRINT(LEVEL_INFO, "pbmain: %s CDFS_FILE_FOLDER_FAIL", __FUNCTION__);
            return CDFS_FILE_FOLDER_FAIL;
        }
    }

    HOST_PROF_LOG_PRINT(LEVEL_INFO, "pbmain: %s vidsize %d imgsize%d", __FUNCTION__,video_size ,jpg_size);

    if(video_size!=jpg_size)
    {
        if(!video_size && jpg_size!=0)      //just only have image file
            return CDFS_FILE_FOLDER_JPG;
        else if(!jpg_size)
            return CDFS_FILE_FOLDER_VIDEO;  //just only have video file
        else{	//For burst captrue
			if(ppbUiParam->selCdfs == CDFS_FILE_FOLDER_VIDEO)
            	return CDFS_FILE_FOLDER_VIDEO;
            else
            	return CDFS_FILE_FOLDER_JPG;
        }
    }
    else
    {
        /*file count eq*/
        DBG_PRINT("pbmain: filencmp %s %s %d\n",video_file, jpg_file, strncmp(video_file, jpg_file, jpg_size-4));

		#if SP5K_CDFS_OPT	/* Mantis 49658 */
		if(ppbUiParam->selCdfs == CDFS_FILE_FOLDER_VIDEO)
		#else
        if(strncmp(video_file, jpg_file, jpg_size-4)>=0)  //-4 == .ext name
		#endif
        	return CDFS_FILE_FOLDER_VIDEO;
        else
        	return CDFS_FILE_FOLDER_JPG;
    }
}

/*-------------------------------------------------------------------------
 *  File Name : appPbCdfsFolderSel
 *------------------------------------------------------------------------*/
UINT8 appPbCdfsFolderSel(UINT8 sel)
{
    //UINT8 ret = FAIL;
	UINT8 ret;
	appCdfsFileFolderSet(sel);
	ret = appCdfsFolderInit(sel);
    return ret;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbCdfsInit
 *------------------------------------------------------------------------*/
UINT8 appPbCdfsInit(UINT8 sel)
{
    UINT8 ret = FAIL;
    //UINT8 type;

    if(!IS_CARD_EXIST)
    {
        HOST_PROF_LOG_PRINT(LEVEL_ERROR, "pbmain: %s card no exist", __FUNCTION__);
        return ret;
    }

    if(sel==CDFSINIT)
    {
		UINT8 type;
        type = appPbCdfsFileScan();

        if(type!=CDFS_FILE_FOLDER_FAIL)
            ret = appPbCdfsFolderSel(type);

        HOST_PROF_LOG_PRINT(LEVEL_INFO, "pbmain: Cdfs scan mode------pb type=%d ret=%d",type,ret);
    }
    else
    {
        if(gpbMainCb.mainStat ==PB_MAIN_STAT_CDFS_IMAGE)
        {
            //type = CDFS_FILE_FOLDER_JPG;
            ret = appPbCdfsFolderSel(CDFS_FILE_FOLDER_JPG);
        }
        else if(gpbMainCb.mainStat ==PB_MAIN_STAT_CDFS_VIDEO)
        {
            //type = CDFS_FILE_FOLDER_VIDEO;
            ret = appPbCdfsFolderSel(CDFS_FILE_FOLDER_VIDEO);
        }

        HOST_PROF_LOG_PRINT(LEVEL_INFO, "pbmain: Cdfs menu mode------pb stat=%d ret=%d",gpbMainCb.mainStat,ret);
    }

    return ret;
}
#endif

/*-------------------------------------------------------------------------
 *  File Name : appPbMainState
 *------------------------------------------------------------------------*/
void
appPbMainState(
	UINT32 msg,
	UINT32 param
)
{
    static UINT32 diskstat = 0xff;

    if(diskstat==0xff)
        diskstat= appActiveDiskGet();

	switch (msg)
	{
	case APP_STATE_MSG_INIT:
		printf("appPbMainState\n");
	/*	#if JET_PANA_CDFS
		UINT32 filetype;
		#endif
		*/
		#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		jet_pb_image_pip_is_in_pip_set(FALSE);
		#endif
		/*Set sensor to enter sensor powerSave Mode */
        appsensorAutoPwrSaveSet(0);

        if (!IS_PLAYBACK_GROUP(appPreviousStateGet()))
		{
			appPreviousViewStateSet(appPreviousStateGet());
			playInitCreate();
			gpbMainCb.mainStat = PB_MAIN_STAT_NULL;
		}
		else
		{
            if(pUiSetting->pbfileType != PB_FILETYPE_IMAGE)
            {
                playInitCreate();
                gpbMainCb.mainStat = PB_MAIN_STAT_NULL;
            }
#if !defined(PRJ_17DVR)&&!defined(PRJ_19DVR)
			if (appPreviousStateGet() == APP_STATE_PB_IMG_PROT)
                gpbMainCb.mainStat = PB_MAIN_STAT_NULL;
#endif
		}

        HOST_PROF_LOG_PRINT(LEVEL_INFO, "pbmain: filetype %d", pUiSetting->pbfileType);
		#if SP5K_CDFS_OPT
		if(pUiSetting->pbfileType == PB_FILETYPE_VIDEO)
		{
			__FUNC_TRACK__;
#if !JET_PANA_CDFS
			appCdfsFileFolderSet(CDFS_FILE_FOLDER_VIDEO);
			appCdfsFolderInit(CDFS_FILE_FOLDER_VIDEO);
#endif
		}
		else if(pUiSetting->pbfileType == PB_FILETYPE_IMAGE)
		{
			__FUNC_TRACK__;
			appCdfsFileFolderSet(CDFS_FILE_FOLDER_JPG);
			appCdfsFolderInit(CDFS_FILE_FOLDER_JPG);
		}
		else if(pUiSetting->pbfileType == PB_FILETYPE_EVENT)
		{
			__FUNC_TRACK__;
            #if SP5K_USB_UVC_SUPPORT
            UINT8 path[20];
            appCdfsFolderInit(CDFS_FILE_FOLDER_EVENT);
        	appCdfsFolder2Path(CDFS_FILE_FOLDER_EVENT,path);
        	sp5kFsDirCacheSet(path, 1);/*speed up*/
        	appCdfsCurrFolderFileListCreate(CDFS_FILE_FOLDER_EVENT, path);
        	sp5kFsDirCacheSet(path, 0);
            #else
			appCdfsFileFolderSet(CDFS_FILE_FOLDER_EVENT);
			appCdfsFolderInit(CDFS_FILE_FOLDER_EVENT);
            #endif
		}
        #endif

		if (!IS_PLAYBACK_GROUP(appPreviousStateGet()))
		{
			if(fileTypeSelect)
			{
				appPbFilePageCtrlParaInit();
			}
		}
		else
		{
			appPbFilePageCtrlParaSync();
		}

		if (gpbMainCb.mainStat==PB_MAIN_STAT_NULL)
		{
			sp5kModeCfgSet(SP5K_MODE_CFG_STANDBY_DUP, 0);
			sp5kModeCfgSet(SP5K_MODE_CFG_STANDBY_FORCE, 1);
    		sp5kModeSet(SP5K_MODE_STANDBY);
   			sp5kModeWait(SP5K_MODE_STANDBY);
            if(pUiSetting->pbfileType != PB_FILETYPE_IMAGE)
            {
				appPbPIPInit();
            }
			appPbMainState_Init();
		}
		else
		{
			sp5kHostMsgSend(APP_UI_MSG_PB_INIT, 2 /* INIT Triggered */);
		}
		gpbMainCb.isIniting = TRUE;
	    /*-- To mark the period from here to APP_UI_MSG_PB_INIT.
		 * To ignore all buttons to guarantee
	     *   the completion of PLAY INIT period.
	     * The UI buttons will be handled after APP_UI_MSG_PB_INIT.
	     */
		/* do nothing */
		break;

	case APP_STATE_MSG_CLOSE:
		if(appPbYUVPIPStatusGet()){
			AppPbDestroyshowJPGInPIP();
		}
		appPbMainState_Close();
		appStateCloseDone();
		break;
	default:
		if (IS_APP_KEY_MSG(msg))
		{
			appPbMainState_OnKey(msg,param);
		}
		else
		{
			appPbMainState_OnMsg(msg,param);
		}
		break;
	}

}

