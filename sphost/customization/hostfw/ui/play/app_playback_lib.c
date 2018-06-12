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
#include "sp5k_dcf_api.h"
#include "sp5k_pb_api.h"
#include "sp5k_exif_api.h"
#include "sp5k_utility_api.h"
#include "app_playback_private.h"
#include "app_playback_def.h"
#include "app_playback_osd.h"
#include "app_view_param_def.h"
#include "app_lens_api.h"
#include "app_video.h"
#include "app_error_msg.h"
#include "app_jpg_draw.h"
#include "app_dpof.h"
#include "api/sp5k_pip_api.h"
#include "app_cdfs_api.h"
#include "app_playback_def.h"
#include "app_osdfont.h"
#include "app_watch_dog.h"
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
#include "jet_utility.h"
#endif
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
UINT8 IsInHDMI = 0;

UINT16 gpbSizeGrade[11][3][2] =
{      /* 4:3 */     /* 3:2 */   /* 16:9 */
	{{ 640,  480},{ 640,  480},{ 640,  360}},/*VGA*/
	{{1024,  768},{1024,  672},{1024,  576}},/*1M*/
	{{1600, 1200},{1600, 1064},{1600,  896}},/*2M*/
	{{2048, 1536},{2048, 1360},{2048, 1152}},/*3M*/
	{{2272, 1704},{2272, 1512},{2272, 1280}},/*4M*/
	{{2560, 1920},{2560, 1696},{2560, 1440}},/*5M*/
	{{2816, 2112},{2816, 1872},{2816, 1584}},/*6M*/
	{{3072, 2304},{3072, 2048},{3072, 1728}},/*7M*/
	{{3264, 2448},{3264, 2176},{3264, 1840}},/*8M*/
	{{3456, 2592},{3456, 2304},{3456, 1512}},/*9M*/
	{{3648, 2736},{3648, 2432},{3648, 2048}}/*10M*/
};

BOOL isLastFileIdxFlag = FALSE;
static BOOL pipInitStatus = FALSE;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
UINT32 appVideoSizeGet(UINT8 size_idx, UINT32 *pW, UINT32 *pH, UINT32 *pfrmrate);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
BOOL jet_pb_image_pip_is_in_pip_get(void);
void jet_pb_image_pip_is_in_pip_set(BOOL set);
void jet_pb_image_pip_enter(void);
BOOL jet_pb_image_pip_init(void);
BOOL jet_pb_image_pip_preload(char *filename);
void jet_pb_image_pip_play(void);
void jet_pb_image_pip_exit(void);
#endif
/*-------------------------------------------------------------------------
 *  File Name : appRsvFileScaleDraw
 *------------------------------------------------------------------------*/
UINT32 appRsvFileScaleDraw(UINT32 pageID,UINT32 fileType,UINT8 *fileName,UINT32 X,UINT32 Y,UINT32 scaleW,UINT32 scaleH)
{
	UINT32 loadID = 0, ret;
	UINT8 *pDstYUVAddr;
	sp5kGfxObj_t SrcGfxObj;
	sp5kGfxObj_t DstGfxObj;
	sp5kGfxRoi_t RoiObj;

	// align to 16
	scaleW = (scaleW+15)>>4<<4;
	scaleH = (scaleH+15)>>4<<4;

	pDstYUVAddr = (UINT8*)sp5kYuvBufferAlloc(scaleW,scaleH);

	sp5kRsvPreloadRelease();
	sp5kRsvPreloadDrawCfgSet(SP5K_RES_PRELOAD_NUM,1);
	ret = sp5kRsvPreloadFile(fileType,(char *)fileName,loadID,0,0,0);
	if(ret == FAIL){
		return ret;
	}
	ret = sp5kRsvPreloadGfxObjGet(loadID,&SrcGfxObj);
	if(ret == FAIL){
		return ret;
	}

	DstGfxObj.pbuf = pDstYUVAddr;
	DstGfxObj.bufW = scaleW;
	DstGfxObj.bufH = scaleH;
	DstGfxObj.fmt = SP5K_GFX_FMT_YUV422;
	DstGfxObj.roiX = DstGfxObj.roiY =0;
	DstGfxObj.roiW = scaleW;
	DstGfxObj.roiH = scaleH;
	DstGfxObj.paplaneBuf = NULL;

	RoiObj.roiX = X;
	RoiObj.roiY = Y;
	RoiObj.roiW = DstGfxObj.roiW;
	RoiObj.roiH = DstGfxObj.roiH;

	sp5kGfxObjectScale(&SrcGfxObj,&DstGfxObj);

	ret = sp5kGfxObjectDraw(pageID,&RoiObj,0,&DstGfxObj);
	sp5kYuvBufferFree(pDstYUVAddr);
	sp5kRsvPreloadRelease();
	return ret;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbDiskMount
 *------------------------------------------------------------------------*/
UINT32
appPbDiskMount(
	pbDiskIdDef_t dskId
)
{
	switch (dskId)
	{
	case PB_DISK_NAND:
	case PB_DISK_SD:
	case PB_DISK_USB:
		appDiskMount(dskId);
		return SUCCESS;
		break;
	default:
		PB_DBG(LEVEL_WARN,"pblib: %s: Unknown disk (%d) ",__FUNCTION__, dskId);
		return FAIL;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appPbActiveDiskSet
 *------------------------------------------------------------------------*/
UINT32
appPbActiveDiskSet(
	pbDiskIdDef_t dskId
)
{
	/*-- Configure & activate DCF active drive */
	if (FAIL==appActiveDiskSet((UINT32)dskId))
	{
	PB_DBG(LEVEL_ERROR,"pblib: %s: Setting AciveDisk (%d) failed ",__FUNCTION__, dskId);
		return FAIL;
	}

	/*-- Specify FILETYPE */
	if (FAIL==sp5kDcfFsFileTypeSet(APP_PB_FILETYPE_ALL))
	{
		PB_DBG(LEVEL_ERROR,"pblib: %s: sp5kDcfFsFileTypeSet(%d) failed  ",__FUNCTION__, dskId);
		return FAIL;
	}

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : playMusicDRAMPlayStop
 *------------------------------------------------------------------------*/
void
playMusicDRAMPlayStop(
	void
)
{
	/*-- To Stop Last DRAM Play  */
	sp5kResDRAMControl(SP5K_RES_DRAM_ABORT, 0, 0, 0, 0, 0);
}

/*-------------------------------------------------------------------------
 *  File Name : playIsPrivateQVGA
 *------------------------------------------------------------------------*/
BOOL
playIsPrivateQVGA(
	void
)
{
	return TRUE;
}

/*-------------------------------------------------------------------------
 *  File Name : playJpgImgTypeAutoSelect
 *------------------------------------------------------------------------*/
static
UINT8
playJpgImgTypeAutoSelect(
	pbImgCb_t *pimgCb
)
{
	BOOL  isICatchCam;

	/*-- Query for thumbnail/main/spQVGA */
	sp5kExifImageQuery(&pimgCb->szDcfFileName[0], &pimgCb->exifImgFlag);

	/*-- Check if Camera of iCatch :-P */
	isICatchCam = playIsPrivateQVGA();

	#if KIT_WITH_HDMI
	if (IS_HDMI_IN)  //Ethan
		return SP5K_SPB_IMG_PRIMARY;
	#endif
	/*-- Examine any QV/THUMB/MAIN */
	if (IS_QV_IMG_EXIST(pimgCb->exifImgFlag) && isICatchCam)
	{
		return SP5K_SPB_IMG_QV;
	}
	else if (IS_THUMB_IMG_EXIST(pimgCb->exifImgFlag))
	{
		return SP5K_SPB_IMG_THUMB;
	}
	else if (IS_MAIN_IMG_EXIST(pimgCb->exifImgFlag))
	{
		return SP5K_SPB_IMG_PRIMARY;
	}
	else
	{
		return 0xFF;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : playJpgCheckup
 *------------------------------------------------------------------------*/
UINT32
playJpgCheckup(
	pbImgCb_t *pimgCb
)
{
	UINT32  imgWidth, imgHeight, imgYuvFmt;


	if ( !IS_FILETYPE_JPG(pimgCb->dcfAttr.fileType) )
	{
		return FAIL;
	}

	/*-- Query for thumbnail/main/spQVGA */
	sp5kExifImageQuery(&pimgCb->szDcfFileName[0], &pimgCb->exifImgFlag);
	/*-- Check YUV format : only 422/420 supported */
	sp5kExifImageInfoGet(SP5K_IMG_PRIMARY, SP5K_IMG_ATTR_DATA_FMT, &imgYuvFmt);
	if (imgYuvFmt != 0x21 && imgYuvFmt != 0x22 && imgYuvFmt != 0x11)
	{
		pimgCb->isFileErr = TRUE;
		pimgCb->isMainImgOK = pimgCb->isThumbImgOK = FALSE;
		return FAIL;
	}
	pimgCb->imgYuvFmt = imgYuvFmt;

	/*-- Check image W & H : shall be multiple of 8 */
	sp5kExifImageInfoGet(SP5K_IMG_PRIMARY, SP5K_IMG_ATTR_WIDTH, &imgWidth);
	sp5kExifImageInfoGet(SP5K_IMG_PRIMARY, SP5K_IMG_ATTR_HEIGHT, &imgHeight);
	#if 0 /* remove this constraint */
	if ((imgWidth & 0x07UL) || (imgHeight & 0x07UL))
	{
		pimgCb->isFileErr = TRUE;
		pimgCb->isMainImgOK = pimgCb->isThumbImgOK = FALSE;
		return FAIL;
	}
	#endif
	pimgCb->imgWidth = imgWidth;
	pimgCb->imgHeight = imgHeight;

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appIsInPBMode
 *------------------------------------------------------------------------*/
void appIsInPBMode(void)
{
	appStateChange(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbShowBG
 *------------------------------------------------------------------------*/
void appPbShowBG(void)
{
	#if (KIT_WITH_HDMI)
		/* UINT32 w = 1920, h = 1080; */
		/* UINT32 arr[4] = {0,0, w, h}; */
		if(IS_HDMI_IN)
		{
			UINT32 arr[4] = {0,0, 1920, 1080};
			appPbPIPInit();
			sp5kResourceFilePagePlay(SP5K_RES_TYPE_JPG, (const char *)RES_JPG_PB_BG , SP5K_GFX_PAGE_PIP_MAIN, arr, 0);
			sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE,1,0,1,0);
		}else
	#endif

	PB_BG_SHOW;
}

BOOL appPbYUVPIPStatusGet(void)
{
	return pipInitStatus;
}

void appPbYUVPIPStatusSet(BOOL val)
{
	pipInitStatus = val;
}

/*-------------------------------------------------------------------------
 *  File Name : showJPGInPIP
 *------------------------------------------------------------------------*/
void showJPGInPIP(void)
{
	UINT32 w = 1920, h = 1080;
	UINT32 arr[4] = {0,0, w, h};
	sp5kResourceFilePagePlay(SP5K_RES_TYPE_JPG, (const char *)gpbImgCb.szDcfFileName, SP5K_GFX_PAGE_PIP_BASE+0, arr, 0);
	sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE,1,0,1,0);
}

/*-------------------------------------------------------------------------
 *  File Name : AppPbDestroyshowJPGInPIP
 *------------------------------------------------------------------------*/
void AppPbDestroyshowJPGInPIP(void)
{
	UINT32 ret;
	sp5kRsvPreloadRelease();

	#if KIT_WITH_HDMI
    	if (!IS_HDMI_IN)	/*//PIP size too big to standby mode would too long, so DON'T DUPLICATE IMAGE in HDMI*/
    #endif
	sp5kModeCfgSet(SP5K_MODE_CFG_STANDBY_DUP, 1);
	sp5kModeCfgSet(SP5K_MODE_CFG_STANDBY_FORCE, 1);/*1: enter standby mode even already in, 0: enter standby mode if not in*/
	sp5kModeSet(SP5K_MODE_STANDBY);
	sp5kModeWait(SP5K_MODE_STANDBY);
	ret = sp5kPipTerm();
	appPbYUVPIPStatusSet(FALSE);
	if(ret != SUCCESS)
	{
		PB_DBG(LEVEL_ERROR,"pblib: %s: sp5kPipTerm err",__FUNCTION__);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appPbPIPInit
 *------------------------------------------------------------------------*/
void appPbPIPInit(void)
{
	UINT32 w, h;

	if(IS_HDMI_IN){
		w = 1920;
		h = 1080;
	}
	else{
		w = 320;
		h = 240;
	}
	sp5kPipInitCfgSet(SP5K_PIP_INIT_MAIN_FRAME_SIZE, w, h, 0, 0);
	sp5kPipInit();
	sp5kDispAttrSet(sp5kDispActChnlGet(0), SP5K_DISP_IMG_ACTIVE, 1);
	sp5kRsvPreloadCfg(1);
    appPbYUVPIPStatusSet(TRUE);
}

/*-------------------------------------------------------------------------
 *  File Name : playJpgPlayStart
 *------------------------------------------------------------------------*/
UINT32
playJpgPlayStart(
	void
)
{
	/*-- Validity checkup JPG file */
	if (FAIL==playJpgCheckup(&gpbImgCb))
	{
		gpbImgCb.isFileErr = TRUE;
		gpbImgCb.fileType = PB_FT_UNKNOWN;
		return FAIL;
	}

	/*-- Auto select QV/THUMB/MAIN to play */
	gpbImgCb.sp5kImgType = playJpgImgTypeAutoSelect(&gpbImgCb);
	gpbImgCb.isFileErr = FALSE;

	#if PLAYBACK_ROTATE_BY_EXIF_TAG
	sp5kStillPlayCfgSet(SP5K_SPB_QV_ROTATE_BASE,SP5K_SPB_QV_ROTATE_BY_THUMB);
	sp5kStillPlayCfgSet(SP5K_SPB_THUMB_ROTATE_BASE,SP5K_SPB_THUMB_ROTATE_BY_PRIMARY);
	sp5kStillPlayCfgSet(SP5K_SPB_PRIMARY_ROTATE_BASE,SP5K_SPB_PRIMARY_ROTATE_BY_PRIMARY);
	#endif

	#if KIT_WITH_HDMI
	if (IS_HDMI_IN){
		gpbImgCb.sp5kImgType = SP5K_SPB_IMG_PRIMARY;
	}
	#endif

	if (gpbImgCb.sp5kImgType==SP5K_SPB_IMG_QV)
	{
		gpbMainCb.playStat = PB_PLAY_STAT_JPG_QV_PLAYING;
	}
	else if (gpbImgCb.sp5kImgType==SP5K_SPB_IMG_THUMB)
	{
		gpbMainCb.playStat = PB_PLAY_STAT_JPG_THUMB_PLAYING;
	}
	else if (gpbImgCb.sp5kImgType==SP5K_SPB_IMG_PRIMARY)
	{
		gpbMainCb.playStat = PB_PLAY_STAT_JPG_MAIN_PLAYING;
	}
	else
	{
		gpbImgCb.isFileErr = TRUE;
		gpbImgCb.fileType = PB_FT_UNKNOWN;
		gpbMainCb.playStat = PB_PLAY_STAT_STANDBY;
		gpbMainCb.err = PB_ERR_FILE_ERROR;
		return FAIL;
	}

	/*-- Go playback select imgType */
	gpbImgCb.isMainImgOK = gpbImgCb.isThumbImgOK = FALSE;
	gpbMainCb.isPlayDone = FALSE;

#if (KIT_WITH_HDMI)
	if (IS_HDMI_IN && appPbYUVPIPStatusGet())
	{
//		showJPGInPIP();
	}
#endif

	sp5kStillPlayCfgSet(SP5K_SPB_IMG_TYPE, gpbImgCb.sp5kImgType);

	PB_DBG(LEVEL_INFO,"pblib: %s pb mode set",__FUNCTION__);
	sp5kModeSet(SP5K_MODE_STILL_PLAYBACK);
	sp5kModeWait(SP5K_MODE_STILL_PLAYBACK);
	PB_DBG(LEVEL_INFO,"pblib: %s pb mode set done",__FUNCTION__);

	/*-- Check JPG IMG Size Grade */
	gpbImgCb.imgOrientDim = appPbImageOrientDimGet(gpbImgCb.imgWidth, gpbImgCb.imgHeight);
	if (gpbImgCb.imgOrientDim & PB_IMG_ORIENT_HORIZONTAL)
	{
		gpbImgCb.sizeGrade = appPbImageSizeGradeGet((UINT16)gpbImgCb.imgWidth, (UINT16)gpbImgCb.imgHeight, 0);
	}
	else if (gpbImgCb.imgOrientDim & PB_IMG_ORIENT_VERTICAL)
	{
		gpbImgCb.sizeGrade = appPbImageSizeGradeGet((UINT16)gpbImgCb.imgWidth, (UINT16)gpbImgCb.imgHeight, 0);
	}
	else
	{
		gpbImgCb.sizeGrade = PB_IMG_SIZE_GRADE_UNKNOWN;
	}
	#if defined(PRJ_17DVR)||defined(PRJ_19DVR)

	/*-- Exif Info */
	if (FAIL==appPbImgExifInfoGet(&gpbExifInfo))
	{
		gpbImgCb.pExifInfo = NULL;
	}
	else
	{
		gpbImgCb.pExifInfo = &gpbExifInfo;
	}	
	#else
	/*-- Get Histogram & EXIF info, depends on osdType */
	if (gpbMainCb.osdType==PB_DISP_FULL)
	{
		/*-- Histogram */
		if (FAIL==appPbImgHistogramGet(&gpbHistBuf[0], PB_HISTOGRAM_BAR_NUM, PB_HISTOGRAM_OSD_HEIGHT))
		{
			gpbImgCb.pHistBuf = NULL;
		}
		else
		{
			gpbImgCb.pHistBuf = &gpbHistBuf[0];
		}

		/*-- Exif Info */
		if (FAIL==appPbImgExifInfoGet(&gpbExifInfo))
		{
			gpbImgCb.pExifInfo = NULL;
		}
		else
		{
			gpbImgCb.pExifInfo = &gpbExifInfo;
		}
	}
	#endif

	#if SP5K_CDFS_OPT
	#else
	#endif

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : playVideoTitlePlayStart
 *------------------------------------------------------------------------*/
UINT32
playVideoTitlePlayStart(
	void
)
{
	UINT32 w, h, frm;
	if(IS_HDMI_IN){
		w=1920;
        h=1080;
	}
	else{
		w=320;
        h=240;
	}
/* #if !defined(PRJ_17DVR)
	UINT32 hdmiArr[4] = {100, 310, 576, 324};
   	UINT32 arr[4] = {8,80, 112, 63};
#endif */
	UINT32 activeState = appActiveStateGet();

	//UINT32 selPos[4] = {0,0,w,h};

	gpbMainCb.playStat = PB_PLAY_STAT_VIDEO_TITLE_PLAYING;
	gpbMainCb.isPlayDone = FALSE;
	gpbImgCb.isMainImgOK = gpbImgCb.isThumbImgOK = FALSE;

	/*-- Set MediaPlay filename :
	 * The filename should be set before parsing attribute !!
	 */
	if (FAIL==sp5kMediaPlayAttrSet(SP5K_MEDIA_ATTR_FILE_NAME,(UINT32)gpbImgCb.szDcfFileName))
	{
		PB_DBG(LEVEL_ERROR,"pblib: %s:Media filename set failed",__FUNCTION__);
		return FAIL;
	}
#if !defined(PRJ_17DVR)&&!defined(PRJ_19DVR)
	sp5kMediaPlayCfgSet(SP5K_MEDIA_PLAY_YUV_SCALE_DIV, 0);

    appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_NOCOLOR, PAL_NOCOLOR, PAL_BLEND_100);
    appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 40, 80, 40, 40, 0, 0);
#endif
	if (activeState == APP_STATE_PB_IMG_DEL ||
		activeState == APP_STATE_PB_IMG_PROT ||
		activeState == APP_STATE_MENU ||
		pUiSetting->pbfileType == PB_FILETYPE_IMAGE
		)
	{
	    if (FAIL==sp5kMediaPlayAttrParse())
	    	return FAIL;

        PB_DBG(LEVEL_INFO,"pblib: SP5K_MODE_VIDEO_PLAYBACK %d",pUiSetting->pbfileType);
		sp5kModeSet(SP5K_MODE_VIDEO_PLAYBACK);
		appHostMsgWaitExact(SP5K_MSG_MODE_READY,SP5K_MODE_VIDEO_PLAYBACK,5000);
	}
	else
    {
#if !defined(PRJ_17DVR)&&!defined(PRJ_19DVR)
	sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE,0,0,0,0);
        PB_DBG(LEVEL_DEBUG,"pblib: sp5kResourceFilePagePlay %s",gpbImgCb.szDcfFileName);

		if (IS_HDMI_IN){
            if(FAIL == appRsvFileScaleDraw(SP5K_GFX_PAGE_PIP_MAIN,SP5K_RES_TYPE_JPG, RES_JPG_PB_BG_FRAME, 10, 0, 1920, 1080))
            {
                PB_DBG(LEVEL_ERROR,"pblib: background jpg fail %d",__LINE__);
                return FAIL;
            }
			UINT32 hdmiArr[4] = {100, 310, 576, 324};
            if(FAIL == appRsvFileScaleDraw(
                                SP5K_GFX_PAGE_PIP_MAIN,
                                SP5K_RES_TYPE_AVI,
                                gpbImgCb.szDcfFileName,
                                hdmiArr[0], hdmiArr[1],
                                hdmiArr[2], hdmiArr[3])
               )
        	{
                gpbMainCb.playStat = PB_PLAY_STAT_STANDBY;
                sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE,1,0,1,0);
                PB_DBG(LEVEL_ERROR,"pblib: hdmi sp5kResourceFilePagePlay FAIL %s",gpbImgCb.szDcfFileName);

                gpbMainCb.err = PB_ERR_FILE_ERROR;
                appOsdLib_IconFileDraw(SP5K_GFX_PAGE_OSD_0, 40, 80,SP5K_GFX_ALIGN_TOP_LEFT,(UINT8 *)ID_WARNICON,0x0000);
            }
		}
		else{
		#if !defined(PRJ_17DVR)&&!defined(PRJ_19DVR)
		 UINT32 selPos[4] = {0,0,w,h};
			if(FAIL == sp5kResourceFilePagePlay(SP5K_RES_TYPE_JPG, (const char *)RES_JPG_PB_BG_FRAME, SP5K_GFX_PAGE_PIP_MAIN, selPos, 0)){
                PB_DBG(LEVEL_ERROR,"pblib: background jpg fail %d",__LINE__);
                return FAIL;
			}
			#endif
			UINT32 arr[4] = {8,80, 112, 63};
            if(FAIL == sp5kResourceFilePagePlay(SP5K_RES_TYPE_AVI, (const char *)gpbImgCb.szDcfFileName, SP5K_GFX_PAGE_PIP_MAIN, arr, 0)){
				gpbMainCb.playStat = PB_PLAY_STAT_STANDBY;
				sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE,1,0,1,0);
                PB_DBG(LEVEL_ERROR,"pblib: sp5kResourceFilePagePlay FAIL %s",gpbImgCb.szDcfFileName);

                gpbMainCb.err = PB_ERR_FILE_ERROR;
                appOsdLib_IconFileDraw(SP5K_GFX_PAGE_OSD_0, 40, 80,SP5K_GFX_ALIGN_TOP_LEFT,(UINT8 *)ID_WARNICON,0x0000);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
				retur FAIL;//sc added
#endif
			}
		}
		sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE,1,0,1,0);
		#endif
	}

	/*-- Retrieve media information */
    if(gpbMainCb.err!=PB_ERR_FILE_ERROR)
    {
		sp5kMediaPlayAttrGet(SP5K_MEDIA_ATTR_DURATION, &gpbImgCb.mediaTotalTime);
		sp5kMediaPlayAttrGet(SP5K_MEDIA_ATTR_WIDTH, &gpbImgCb.imgWidth);
		sp5kMediaPlayAttrGet(SP5K_MEDIA_ATTR_HEIGHT, &gpbImgCb.imgHeight);
    }
    else
    {
        gpbImgCb.mediaTotalTime = gpbImgCb.imgWidth = gpbImgCb.imgHeight = 0;
        appPlayOsd_ShowFileTime(TRUE,gpbImgCb.mediaTotalTime);
        appPlayOsd_MainInfoDraw(PB_OSD_DRAW_NORMAL);
    }

	#if KIT_WITH_HDMI
	if (IS_HDMI_IN && appPbYUVPIPStatusGet())
	{
		UINT32 selPos[4] = {0,0,w,h};
    	sp5kRsvPreloadFile(SP5K_RES_TYPE_AVI, (char *)gpbImgCb.szDcfFileName, 0, w, h, 0);
    	sp5kRsvPreloadDraw(0 , SP5K_GFX_PAGE_PIP_MAIN, selPos, 0);
    	sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE,1,0,1,0);
	}
	#endif
	/*-- Determine sizeGrade */
	UINT32 i;
	for (i=0; i<UI_VIDEO_SIZE_MAX; i++)
	{
		appVideoSizeGet(i, &w, &h, &frm);
		if (gpbImgCb.imgWidth==w && gpbImgCb.imgHeight==h)
		{
			gpbImgCb.sizeGrade = PB_VIDEO_SIZE_GRADE_1+i;
			break;
		}
	}
	if (i==UI_VIDEO_SIZE_MAX)
	{
		gpbImgCb.sizeGrade = PB_VIDEO_SIZE_GRADE_UNKNOWN;
	}

	gpbMainCb.playStat = PB_PLAY_STAT_STANDBY;
	gpbMainCb.isPlayDone = TRUE;
	gpbImgCb.isMainImgOK = gpbImgCb.isThumbImgOK = TRUE;

	PB_DBG(LEVEL_INFO,"pblib: video %s, total(%d), size(%d x %d)",
	                 gpbImgCb.szDcfFileName,
	                 gpbImgCb.mediaTotalTime,
	                 gpbImgCb.imgWidth,
	                 gpbImgCb.imgHeight);

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : playAudioTitlePlayStart
 *------------------------------------------------------------------------*/
UINT32
playAudioTitlePlayStart(
	void
)
{
	DBG_PRINT("pblib: %s \n", __FUNCTION__);

	/*-- Set MediaPlay filename :
	 * The filename should be set before parsing attribute !!
	 */
	if (FAIL==sp5kMediaPlayAttrSet(SP5K_MEDIA_ATTR_FILE_NAME,(UINT32)gpbImgCb.szDcfFileName))
	{
		PB_DBG(LEVEL_ERROR,"pblib: %s:Media filename set failed",__FUNCTION__);
		return FAIL;
	}

	/*-- Parse Media Attr before playing :*/
	if (FAIL==sp5kMediaPlayAttrParse())
	{
		PB_DBG(LEVEL_ERROR,"pblib: %s:Media attr parsing failed",__FUNCTION__);
		return FAIL;
	}

	/*-- Retrieve media information, for UI Draw */
	sp5kMediaPlayAttrGet(SP5K_MEDIA_ATTR_DURATION, &gpbImgCb.mediaTotalTime);

	/*-- Draw Audio background JPG*/
	gpbMainCb.playStat = PB_PLAY_STAT_STANDBY;
	gpbImgCb.isMainImgOK = gpbImgCb.isThumbImgOK = FALSE;
	gpbMainCb.isPlayDone = FALSE;
	gpbMainCb.playStat = PB_PLAY_STAT_STANDBY;
	gpbMainCb.isPlayDone = TRUE;
	gpbImgCb.isThumbImgOK = TRUE;

	sp5kHostMsgSend(APP_UI_MSG_PB_AUDIO_TITLE_DONE, 0);

	PB_DBG(LEVEL_INFO,"pblib: audio %s, total(%d) \n",
	                gpbImgCb.szDcfFileName,
	                gpbImgCb.mediaTotalTime);

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbStillPlayDoneParse
 *------------------------------------------------------------------------*/
#define PB_DONE_MSG_GET_IMG_TYPE(p)	(((p) & 0xFFFF0000)>>16)
#define PB_DONE_MSG_GET_ERR_CODE(p)	((p) & 0x000000FF)
#define IS_PB_DONE_QV(p) 	(PB_DONE_MSG_GET_IMG_TYPE(p)==SP5K_SPB_IMG_QV)
#define IS_PB_DONE_MAIN(p) 	(PB_DONE_MSG_GET_IMG_TYPE(p)==SP5K_SPB_IMG_PRIMARY)
#define IS_PB_DONE_THUMB(p) (PB_DONE_MSG_GET_IMG_TYPE(p)==SP5K_SPB_IMG_THUMB)

UINT32
appPbStillPlayDoneParse(
	UINT32 param
)
{
	UINT32 ret, errCode;

	errCode = PB_DONE_MSG_GET_ERR_CODE(param);

	if (IS_PB_DONE_MAIN(param))
	{
		if (errCode==0x00FFUL)
		{
			ret = PB_DONE_MAIN_ABORT;
		}
		else if (errCode==0x00UL)
		{
			ret = PB_DONE_MAIN_FINISH;
		}
		else
		{
			ret = PB_DONE_MAIN_ERROR;
		}
	}
	else if (IS_PB_DONE_QV(param))
	{
		if (errCode==0x00FFUL)
		{
			ret = PB_DONE_QV_ABORT;
		}
		else if (errCode==0x00UL)
		{
			ret = PB_DONE_QV_FINISH;
		}
		else
		{
			ret = PB_DONE_QV_ERROR;
		}
	}
	else if (IS_PB_DONE_THUMB(param))
	{
		if (errCode==0x00FFUL)
		{
			ret = PB_DONE_THUMB_ABORT;
		}
		else if (errCode==0x00UL)
		{
			ret = PB_DONE_THUMB_FINISH;
		}
		else
		{
			ret = PB_DONE_THUMB_ERROR;
		}
	}
	else
	{
		ret = PB_DONE_UNKNOWN_ERROR;
	}

	return ret;
}

/*-------------------------------------------------------------------------
 *  File Name : playImgPlayReset
 *------------------------------------------------------------------------*/
void
playImgPlayReset(
	void
)
{
	DBG_PRINT("pblib: %s \n", __FUNCTION__);

	gpbMainCb.lastFileType = gpbImgCb.fileType;

	memset(&gpbImgCb, 0, sizeof(pbImgCb_t));
}

/*-------------------------------------------------------------------------
 *  File Name : playImgDcfAttrGet
 *------------------------------------------------------------------------*/
UINT32
playImgDcfAttrGet (
	pbImgCb_t *pImgCb
)
{
	sp5kDcfAttrElm_t *pdcfAttr;
	UINT32 fileType;

    appWatchDogRefresh();		/*avoid to host WDT trig. when a lot of file opration*/

	DBG_PRINT("pblib: %s \n", __FUNCTION__);

	pdcfAttr = &pImgCb->dcfAttr;

	DBG_PRINT("pblib: Getting Dcf Attr of index(%d) ...\n", pImgCb->dcfIdx);
	if (FAIL==sp5kDcfFsFileAttrGet(pImgCb->dcfIdx, pdcfAttr, &pImgCb->szDcfFileName[0]))
	{
		PB_DBG(LEVEL_ERROR,"pblib: %s: getting attr failed",__FUNCTION__);
		pImgCb->isFileErr = TRUE;
		pImgCb->fileType = PB_FT_UNKNOWN;
		return FAIL;
	}

	if (pdcfAttr->attribute & SP5K_DCF_FILE_ATTR_LOCK)
	{
		PB_DBG(LEVEL_WARN,"pblib: %s: IMG is locked, %s", __FUNCTION__, &pImgCb->szDcfFileName[0]);
		pImgCb->isProtected = TRUE;
	}
	else
	{
		pImgCb->isProtected = FALSE;
	}

	fileType = pdcfAttr->fileType;
	if (!IS_SUPPORTED_FILETYPE(fileType))
	{
		PB_DBG(LEVEL_WARN,"pblib: %s: unknown filetype(0x%08x)",__FUNCTION__, pdcfAttr->fileType);
		pdcfAttr->fileType = 0;
		pImgCb->isFileErr = TRUE;
		pImgCb->fileType = PB_FT_UNKNOWN;
		return FAIL;
	}
	pImgCb->isFileErr = FALSE;

	if (IS_FILETYPE_JPG(fileType))
	{
		if (pdcfAttr->attribute & SP5K_DCF_FILE_ATTR_MEMO)
		{
			DBG_PRINT("pblib: JPG MEMO \n");
			pdcfAttr->fileType = SP5K_DCF_FILETYPE_JPG_MEMO;
			pImgCb->fileType = PB_FT_JPG_MEMO;
		}
		else
		{
			pImgCb->fileType = PB_FT_JPG;
		}
	}
	else if (IS_FILETYPE_WAV(fileType))
	{
		pImgCb->fileType = PB_FT_WAV;
	}
	else if (IS_FILETYPE_AVI(fileType))
	{
		pImgCb->fileType = PB_FT_AVI;
	}
	else if (IS_FILETYPE_MOV(fileType))
	{
		pImgCb->fileType = PB_FT_MOV;
	}
	else if (IS_FILETYPE_MP4(fileType))
	{
		pImgCb->fileType = PB_FT_MP4;
	}
	else
	{
		pImgCb->fileType = PB_FT_UNKNOWN;
	}

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : playImgPlayStart
 *------------------------------------------------------------------------*/
UINT32
playImgPlayStart(
	void
)
{
	/* sp5kDcfAttrElm_t *pdcfAttr; */

	DBG_PRINT("pblib: %s \n", __FUNCTION__);

	/* pdcfAttr = &gpbImgCb.dcfAttr; */

	sp5kDcfFsCurFileIdxSet(gpbImgCb.dcfIdx);

#if SP5K_CDFS_OPT
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	if (gpbImgCb.fileType == SP5K_DCF_FILETYPE_JPG) {
		PB_DBG(LEVEL_INFO,"pblib: photo %s", &gpbImgCb.szDcfFileName[0]);
		return (playJpgPlayStart());
	} else {
		PB_DBG(LEVEL_INFO,"pblib: video  %s", &gpbImgCb.szDcfFileName[0]);
		return (playVideoTitlePlayStart());
	}
#endif
#if !defined(PRJ_17DVR)&&!defined(PRJ_19DVR)
	if (pUiSetting->pbfileType == PB_FILETYPE_IMAGE)
	{
		PB_DBG(LEVEL_INFO,"pblib: %s", &gpbImgCb.szDcfFileName[0]);
		return (playJpgPlayStart());
	}
	else if (pUiSetting->pbfileType == PB_FILETYPE_VIDEO || pUiSetting->pbfileType == PB_FILETYPE_EVENT)
	{
		PB_DBG(LEVEL_INFO,"pblib: %s", &gpbImgCb.szDcfFileName[0]);
		return (playVideoTitlePlayStart());
	}
	else
	{
		sp5kDcfAttrElm_t *pdcfAttr;
		pdcfAttr = &gpbImgCb.dcfAttr;
		PB_DBG(LEVEL_WARN,"pblib: %s: Unsupported filetype(0x%08x)",__FUNCTION__, pdcfAttr->fileType);
		return FAIL;
	}
#endif
#else
	sp5kDcfAttrElm_t *pdcfAttr;
	pdcfAttr = &gpbImgCb.dcfAttr;
	if (IS_FILETYPE_JPG(pdcfAttr->fileType))
	{
		PB_DBG(LEVEL_INFO,"pblib: %s", &gpbImgCb.szDcfFileName[0]);
		return (playJpgPlayStart());
	}
	else if (IS_SUPPORTED_VIDEO_FILETYPE(pdcfAttr->fileType))
	{
		PB_DBG(LEVEL_INFO,"pblib: %s", &gpbImgCb.szDcfFileName[0]);
		return (playVideoTitlePlayStart());
	}
	else if (IS_SUPPORTED_AUDIO_FILETYPE(pdcfAttr->fileType))
	{
		PB_DBG(LEVEL_INFO,"pblib: %s", &gpbImgCb.szDcfFileName[0]);
		return (playAudioTitlePlayStart());
	}
	else
	{
		PB_DBG(LEVEL_WARN,"pblib: %s: Unsupported filetype(0x%08x)",__FUNCTION__, pdcfAttr->fileType);
		return FAIL;
	}
#endif
}

/*-------------------------------------------------------------------------
 *  File Name : playImageShift
 *------------------------------------------------------------------------*/
static
UINT32
playImageShift(
	UINT8	shiftDir,
	BOOL	isJpgOnly,
	BOOL	isSkipFileErr
)
{
	DBG_PRINT("pblib: %s: (%s) \n", __FUNCTION__, (shiftDir==PB_SHIFT_NEXT ? "Next" : "Pre"));

lbl_image_shift_err_next_scan:
	gpbMainCb.err = PB_ERR_NONE;
	/*-- Go for next image
	 */
	if (isJpgOnly)
	{
		/*-- JPG IMG Only */
		if (FAIL==appPbScanNextJPG(shiftDir, isSkipFileErr))
		{
			PB_DBG(LEVEL_WARN,"pblib: %s: Next JPG Scan failed ",__FUNCTION__);
			if (isSkipFileErr)
			{
				PB_DBG(LEVEL_ERROR,"pblib: FATAL!! @ %s,%d \n\n", __FUNCTION__, __LINE__);
			}
			return FAIL;
		}
	}
	else
	{
		/*-- Any FileType */
		if (shiftDir==PB_SHIFT_NEXT)
		{
			PB_INDEX_INC(gcurrDcfIdx, 1, gtotalImgCount);
		}
		else if (shiftDir==PB_SHIFT_PRE)
		{
			PB_INDEX_DEC(gcurrDcfIdx, 1, gtotalImgCount);
		}
		else
		{
			PB_DBG(LEVEL_WARN,"pblib: %s: illegal shiftDir ",__FUNCTION__);
			return FAIL;
		}
	}

	/*-- Go and Play selected file*/

	appPbFilePageCtrlParaSync();

	gpbMainCb.postponeUI = 0;
	if (FAIL==appPbImagePlay(gcurrDcfIdx))
	{
		if (isSkipFileErr)
		{
			PB_DBG(LEVEL_WARN,"pblib: %s,%d: FileErr, go on next scan", __FUNCTION__, __LINE__);
			goto lbl_image_shift_err_next_scan;
		}
		else
		{
			gpbMainCb.err = PB_ERR_FILE_ERROR;
			gpbImgCb.isFileErr = TRUE;/*willow++ for clear the video"file error" when shift*/
			return FAIL;
		}
	}

	if (IS_FILETYPE_JPG(gpbImgCb.dcfAttr.fileType))
	{
		gpbMainCb.postponeUI=2;
		/*-- File playback has been fired, wait PLAYBACK_DONE
		 * to tell if this is a true playable file !!
		 * So, we have to postponeUI OSD till then !!
		 */
	}

	DBG_PRINT("pblib: %s, exit !!\n", __FUNCTION__);

	return SUCCESS;
}


/*-------------------------------------------------------------------------
 *  File Name : appPbMediaUIStateGet
 *------------------------------------------------------------------------*/
UINT32
appPbMediaUIStateGet(
	void
)
{
	if ((gpbMainCb.mainStat==PB_MAIN_STAT_VIDEO_SUSPEND&&gpbMainCb.subStat==PB_VIDEO_STAT_PLAYING)
		||(gpbMainCb.mainStat==PB_MAIN_STAT_AUDIO_SUSPEND &&gpbMainCb.subStat==PB_AUDIO_STAT_PLAYING))
	{
		return PB_MEDIA_UI_STAT_PLAYING;
	}
	else if ((gpbMainCb.mainStat==PB_MAIN_STAT_VIDEO_SUSPEND&&gpbMainCb.subStat==PB_VIDEO_STAT_PAUSE)
		||(gpbMainCb.mainStat==PB_MAIN_STAT_AUDIO_SUSPEND &&gpbMainCb.subStat==PB_AUDIO_STAT_PAUSE)
		||(gpbMainCb.mainStat==PB_MAIN_STAT_MEMOREC_SUSPEND &&gpbMainCb.subStat==PB_MEMOREC_STAT_PAUSE))
	{
		return PB_MEDIA_UI_STAT_PAUSE;
	}
	else if ((gpbMainCb.mainStat==PB_MAIN_STAT_VIDEO_SUSPEND &&gpbMainCb.subStat==PB_VIDEO_STAT_FF))
	{
		return PB_MEDIA_UI_STAT_FF;
	}
	else if ((gpbMainCb.mainStat==PB_MAIN_STAT_VIDEO_SUSPEND &&gpbMainCb.subStat==PB_VIDEO_STAT_REW))
	{
		return PB_MEDIA_UI_STAT_REW;
	}
	else if ((gpbMainCb.mainStat==PB_MAIN_STAT_MEMOREC_SUSPEND &&gpbMainCb.subStat==PB_MEMOREC_STAT_RECORDING)	)
	{
		return PB_MEDIA_UI_STAT_RECORDING;
	}
	else if (gpbMainCb.subStat==PB_VIDEO_STAT_SLOW_MOTION)
	{
		return PB_MEDIA_UI_STAT_SLOWMOTION;
	}
	else
	{
		return PB_MEDIA_UI_STAT_STANDBY;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appPbMsg_Clear
 *------------------------------------------------------------------------*/
void
appPbMsg_Clear(
	void
)
{
	appErrMsgShow(ERR_MSG_CLEAR, PB_ERR_SHOW_FOREVER);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbMsg_BusyWait
 *------------------------------------------------------------------------*/
void
appPbMsg_BusyWait(
	void
)
{
	PB_OSD_CLR;
	PB_PIP_CLR;
	appErrMsgShow(ERR_MSG_BUSY_WAIT, PB_ERR_SHOW_FOREVER+ERR_MSG_SHOW_1SEC);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbMsg_ImgSaved
 *------------------------------------------------------------------------*/
void
appPbMsg_ImgSaved(
	void
)
{
	appErrMsgShow(ERR_MSG_IMG_SAVED, ERR_MSG_SHOW_FOREVER);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbMsg_Completed
 *------------------------------------------------------------------------*/
void
appPbMsg_Completed(
	void
)
{
	appErrMsgShow(ERR_MSG_COMPLETED, ERR_MSG_SHOW_FOREVER);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbErr_LockedIconBlink
 *------------------------------------------------------------------------*/
static
void
appPbErr_LockedIconBlink(
	void
)
{
	int i;

	for (i=0; i<4; i++) {
		appPlayOsd_ProtectDraw(TRUE); APP_SLEEP_MS(250);
		appPlayOsd_ProtectDraw(FALSE); APP_SLEEP_MS(250);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appPbErr_FileError
 *------------------------------------------------------------------------*/
static
void
appPbErr_FileError(
	void
)
{
	gpbMainCb.err = PB_ERR_FILE_ERROR;
	//sp5kModeSet(SP5K_MODE_STANDBY);
	//PB_BG_SHOW;
	/*-- Draw PB_BG outside this funciton, to prevent from redraw */
	CLEAR_PIP_SRC;
	CLEAR_OSD_SRC;
	PB_BLACK_BKGND;
	appErrMsgShow(ERR_MSG_FILE_ERROR, PB_ERR_SHOW_FOREVER);
	appOsd_GfxStringDraw(430,480,SP5K_GFX_ALIGN_TOP,gpbImgCb.dcfAttr.filename);
}


/*-------------------------------------------------------------------------
 *  File Name : appPbErr_FileTypeError
 *------------------------------------------------------------------------*/
static
void
appPbErr_FileTypeError(
	void
)
{
	gpbMainCb.err = PB_ERR_FILETYPE_ERROR;
	//sp5kModeSet(SP5K_MODE_STANDBY);
	/*-- Dont override the BG !! */
	PB_OSD_CLR;
	CLEAR_PIP_SRC;
	appErrMsgWithSound(ERR_MSG_FILETYPE_ERROR, ERR_MSG_WITH_WARNING_SOUND, ERR_MSG_SHOW_2SEC);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbErr_CopyError
 *------------------------------------------------------------------------*/
static
void
appPbErr_CopyError(
	void
)
{
	gpbMainCb.err = PB_ERR_COPY_ERROR;
	//sp5kModeSet(SP5K_MODE_STANDBY);
	/*-- Dont override the BG !! */
	appErrMsgWithSound(ERR_MSG_COPY_ERROR, ERR_MSG_WITH_WARNING_SOUND, ERR_MSG_SHOW_2SEC);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbErr_FileLocked
 *------------------------------------------------------------------------*/
void
appPbErr_FileLocked(
	void
)
{
	gpbMainCb.err = PB_ERR_IMG_PROTECTED;
	gpbImgCb.isProtected = TRUE;
	appErrMsgWithSound(ERR_MSG_FILE_LOCKED, ERR_MSG_WITH_WARNING_SOUND, ERR_MSG_SHOW_FOREVER);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbErr_NoImage
 *------------------------------------------------------------------------*/
static
void
appPbErr_NoImage(
	void
)
{
	gpbMainCb.err = PB_ERR_NO_IMAGE;
	gpbMainCb.isNoImage = TRUE;

	PB_OSD_CLR;
	CLEAR_PIP_SRC;
	/*-- Host Shall take care of Background IMG to enhance performance !! */
	appErrMsgShow(ERR_MSG_NO_IMG, PB_ERR_SHOW_FOREVER);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbErr_DiskFull
 *------------------------------------------------------------------------*/
void
appPbErr_DiskFull(
	void
)
{
	gpbMainCb.err = PB_ERR_DISK_FULL;
	gpbDiskCb.isDiskFull = TRUE;
	//sp5kModeSet(SP5K_MODE_STANDBY);
	PB_OSD_CLR;
	CLEAR_PIP_SRC;
	appErrMsgWithSound(ERR_MSG_MEMORY_FULL, ERR_MSG_WITH_WARNING_SOUND, ERR_MSG_SHOW_2SEC);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbErr_DcfFull
 *------------------------------------------------------------------------*/
void
appPbErr_DcfFull(
	void
)
{
	gpbMainCb.err = PB_ERR_DCF_KEY_FULL;
	gpbDiskCb.isDcfFull = TRUE;
	PB_OSD_CLR;
	CLEAR_PIP_SRC;
	appErrMsgWithSound(ERR_MSG_MEMORY_FULL, ERR_MSG_WITH_WARNING_SOUND, ERR_MSG_SHOW_2SEC);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbErr_CardLock
 *------------------------------------------------------------------------*/
void
appPbErr_CardLock(
	void
)
{
	gpbMainCb.err = PB_ERR_CARD_LOCKED;
	PB_OSD_CLR;
	CLEAR_PIP_SRC;
	appErrMsgWithSound(ERR_MSG_CARD_LOCKED, ERR_MSG_WITH_WARNING_SOUND, ERR_MSG_SHOW_2SEC);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbErr_CardError
 *------------------------------------------------------------------------*/
void
appPbErr_CardError(
	void
)
{
	gpbMainCb.err = PB_ERR_CARD_ERROR;

	//sp5kModeSet(SP5K_MODE_STANDBY);
	PB_OSD_CLR; //PB_BLACK_BKGND;
	CLEAR_PIP_SRC;
	appErrMsgWithSound(ERR_MSG_CARD_ERROR, ERR_MSG_WITH_WARNING_SOUND, PB_ERR_SHOW_FOREVER);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbErr_CardNotExist
 *------------------------------------------------------------------------*/
void
appPbErr_CardNotExist(
	void
)
{
	gpbMainCb.err = PB_ERR_CARD_NOT_READY;
	PB_OSD_CLR;
	CLEAR_PIP_SRC;
	appErrMsgWithSound(ERR_MSG_NO_MEMORY_CARD, ERR_MSG_WITH_WARNING_SOUND, ERR_MSG_SHOW_2SEC);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbErrorProcess
 *------------------------------------------------------------------------*/
void
appPbErrorProcess(
	UINT32 err
)
{
	DBG_PRINT("pblib: %s: err(%d) \n", __FUNCTION__, err);

	if (err==PB_ERR_DISK_ERROR)
	{
		if (IS_CARD_EXIST)
		{
			err = PB_ERR_CARD_ERROR;
			gpbDiskCb.isCardMounted = FALSE;
		}
		else
		{
			err = PB_ERR_NAND_ERROR;
			gpbDiskCb.isNandMounted = FALSE;
		}
	}

	switch (err)
	{
	default:
		DBG_PRINT("pblib: %s: err(%d) not defined !! \n", __FUNCTION__, err);
		break;
	case PB_ERR_NONE:
		/*appErrMsgShow(ERR_MSG_CLEAR, PB_ERR_SHOW_FOREVER);*/
		break;
	case PB_ERR_CARD_LOCKED:
		PB_DBG(LEVEL_WARN,"pblib: %s: %d",__FUNCTION__,__LINE__);
		appPbErr_CardLock();
		break;
	case PB_ERR_DISK_ERROR:
	case PB_ERR_CARD_ERROR:
	case PB_ERR_NAND_ERROR:
		PB_DBG(LEVEL_ERROR,"pblib: %s: %d",__FUNCTION__,__LINE__);
		appPbShowBG();
		appPbErr_CardError();
		break;
	case PB_ERR_CARD_NOT_READY:
		PB_DBG(LEVEL_WARN,"pblib: %s: %d",__FUNCTION__,__LINE__);
		appPbErr_CardNotExist();
		break;
	case PB_ERR_DISK_FULL:
		PB_DBG(LEVEL_WARN,"pblib: %s: %d",__FUNCTION__,__LINE__);
		appPbErr_DiskFull();
		break;
	case PB_ERR_DCF_KEY_FULL:
		PB_DBG(LEVEL_WARN,"pblib: %s: %d",__FUNCTION__,__LINE__);
		appPbErr_DcfFull();
		break;
	case PB_ERR_NO_IMAGE:
		/*-- For performance sake, Host shall draw PB_BG by before appPbErrorProcess !!
		 */
		PB_DBG(LEVEL_WARN,"pblib: %s: %d",__FUNCTION__,__LINE__);
		appPbShowBG();
		appPbErr_NoImage();
		break;
	case PB_ERR_NO_IMAGE_SPLASH:
		PB_DBG(LEVEL_WARN,"pblib: %s: %d",__FUNCTION__,__LINE__);
		appPbShowBG();
		appPbErr_NoImage();
		APP_SLEEP_MS(2000);
		break;
	case PB_ERR_IMG_PROTECTED:
		PB_DBG(LEVEL_WARN,"pblib: %s: %d",__FUNCTION__,__LINE__);
		PB_OSD_CLR;
		CLEAR_PIP_SRC;
		appPbErr_FileLocked();
		appPbErr_LockedIconBlink();
		appErrMsgShow(ERR_MSG_CLEAR, ERR_MSG_SHOW_FOREVER);
		break;
	case PB_ERR_FILE_ERROR:
		PB_DBG(LEVEL_ERROR,"pblib: %s: %d",__FUNCTION__,__LINE__);
		appPbErr_FileError();
		break;
	case PB_ERR_FILETYPE_ERROR:
		PB_DBG(LEVEL_ERROR,"pblib: %s: %d",__FUNCTION__,__LINE__);
		appPbErr_FileTypeError();
		break;
	case PB_ERR_COPY_ERROR:
		PB_DBG(LEVEL_ERROR,"pblib: %s: %d",__FUNCTION__,__LINE__);
		appPbErr_CopyError();
		break;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appPbBytes2ClusterCalculate
 *------------------------------------------------------------------------*/
UINT32
appPbBytes2ClusterCalculate(
	UINT32 bytes
)
{
	UINT32 clusNum;

	DBG_PRINT("pblib: %s: bytes(%d) \n", __FUNCTION__, bytes);

	clusNum = appBytes2ClusterCalculate(bytes);
	PB_DBG(LEVEL_INFO,"pblib: bytes(%d), clusNum(%d)", bytes, clusNum);

	return clusNum;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbDiskCheckup
 *------------------------------------------------------------------------*/
void
appPbDiskCheckup(
	pbDiskCB_t *pdskCb
)
{
	/*-- Note : DISK shall be ready before calling this function !!
	 */
	UINT32 actDsk;
	appDiskInfo_t *pdskInfo;

	DBG_PRINT("pblib: %s \n", __FUNCTION__);

	/*-- Retrieve current disk info */
	pdskInfo = &pdskCb->dskInfo;
	memcpy(pdskInfo, appCurDiskInfoGet(), sizeof(appDiskInfo_t));

	actDsk = pdskInfo->actDsk;

	/*-- CARD Locked */
	pdskCb->isCardLock = FALSE;
	if (actDsk==SP5K_DRIVE_SD && IS_CARD_LOCK)
	{
		pdskCb->isCardLock = TRUE;
	}

	/*-- DISK Full */
	pdskCb->isDiskFull = FALSE;
	if (pdskInfo->freeSz < (pdskInfo->cluSz >>10)+PB_DISK_DPOF_RESERVED)
	{
		pdskCb->isDiskFull = TRUE;
	}

	/*-- DCF Key Full */
	pdskCb->isDcfFull = FALSE;
	if (IS_DCF_KEY_FULL)
	{
		pdskCb->isDcfFull = TRUE;
		pdskCb->isDiskFull = TRUE; /* regarded DISK_FULL while DCF_FULL */
	}

	/*-- CARD or NAND */
	pdskCb->isNandMounted = IS_DISK_NAND_MOUNT;
	pdskCb->isCardMounted = IS_DISK_SD_MOUNT;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbDiskFreeClusterGet
 *------------------------------------------------------------------------*/
UINT32
appPbDiskFreeClusterGet(
	UINT32 actDsk
)
{
	UINT32 remaining;

	remaining = appDiskFreeClusterGet(actDsk);
	PB_DBG(LEVEL_INFO,"pblib: drive(%d), free clust(%d)", actDsk, remaining);

	return remaining;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbEditSaveDiskCheckup
 *------------------------------------------------------------------------*/
UINT32
appPbEditSaveDiskCheckup(
	pbImgCb_t *pimgCb
)
{
	sp5kDcfAttrElm_t *pdcfAttr;
	UINT32 freeClust, fileClust;

	DBG_PRINT("pblib: %s: checking %s \n", __FUNCTION__, &pimgCb->szDcfFileName[0]);

	pdcfAttr = &pimgCb->dcfAttr;

	/*-- Check remaining disk size for new file !!*/
	/*-- Disk remaining clusters */
	freeClust = appPbDiskFreeClusterGet(gpbDiskCb.dskInfo.actDsk);
	DBG_PRINT("pblib: %s: Disk free clust (%d) \n", __FUNCTION__, freeClust);

	/*-- Calculate clusters of current image*/
	fileClust = appPbBytes2ClusterCalculate(pdcfAttr->filesize);

	if (fileClust > freeClust)
	{
		return FAIL;
	}

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbImageOrientGet
 *------------------------------------------------------------------------*/
UINT32 appPbImageOrientGet(void)
{
	/* UINT32 orientTag = 0; */
	UINT32 orient = SP5K_ROTATE_0;

	if ( (IS_FILETYPE_JPG(gpbImgCb.dcfAttr.fileType)) ) {
		UINT32 orientTag;
		sp5kExifImageInfoGet(SP5K_IMG_PRIMARY, SP5K_IMG_ATTR_ORIENT, (UINT32 *)&(orientTag));

		if ( orientTag == 6 ) {  /* exif 270-degree. */
			orient = SP5K_ROTATE_270;
		}
		else if ( orientTag == 3 ) {  /* exif 180-degree. */
			orient = SP5K_ROTATE_180;
		}
		else if ( orientTag == 8 ) {  /* exif 90-degree. */
			orient = SP5K_ROTATE_90;
		}
		else {  /* exif 0-degree. */
			orient = SP5K_ROTATE_0;
		}
	}

	return orient;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbImageOrientDimGet
 *------------------------------------------------------------------------*/
pbImageOrientDim_t
appPbImageOrientDimGet(
	UINT16 width,
	UINT16 height
)
{
	UINT32 ratio;
	if ( (width==0) || (height==0) )
	{
		return PB_IMG_ORIENT_UNKNOWN;
	}

	ratio=((width*100)/height);

#if PLAYBACK_ROTATE_BY_EXIF_TAG
	if ( (appPbImageOrientGet() == SP5K_ROTATE_0)
		||(appPbImageOrientGet() == SP5K_ROTATE_180) )
	{
		if((ratio>=185) || (ratio<=120))
		{
			return PB_IMG_ORIENT_UNKNOWN;
		}

		if(ratio>160)
		{
			/*-- 1.777778 */
			return PB_IMG_ORIENT_16X9;
		}
		else if((ratio <= 160) && (ratio > 140))
		{
			/*-- 1.50 */
			return PB_IMG_ORIENT_3X2;
		}
		else
		{
			/*-- 1.33333 */
			return PB_IMG_ORIENT_4X3;
		}
	}
	else
	{
		/*-- PORTRAIT (Vertical) image*/
		if((ratio>=185) || (ratio<=120))
		{
			return PB_IMG_ORIENT_UNKNOWN;
		}

		if(ratio>160)
		{
			/*-- 1.777778 */
			return PB_IMG_ORIENT_9X16;
		}
		else if((ratio <= 160) && (ratio > 140))
		{
			/*-- 1.50 */
			return PB_IMG_ORIENT_2X3;
		}
		else
		{
			/*-- 1.33333 */
			return PB_IMG_ORIENT_3X4;
		}
	}
#else
	if (width>=height)
	{
		if((ratio>=185) || (ratio<=120))
		{
			return PB_IMG_ORIENT_UNKNOWN;
		}

		if(ratio>160)
		{
			/*-- 1.777778 */
			return PB_IMG_ORIENT_16X9;
		}
		else if((ratio <= 160) && (ratio > 140))
		{
			/*-- 1.50 */
			return PB_IMG_ORIENT_3X2;
		}
		else
		{
			/*-- 1.33333 */
			return PB_IMG_ORIENT_4X3;
		}
	}
	else
	{
		/*-- PORTRAIT (Vertical) image*/
		if((ratio >= 80) || (ratio <= 45))
		{
			return PB_IMG_ORIENT_UNKNOWN;
		}

		if(ratio < 60)
		{
			/*-- 0.5625 */
			return PB_IMG_ORIENT_9X16;
		}
		else if((ratio >= 60 ) && (ratio < 70))
		{
			/*-- 0.66667 */
			return PB_IMG_ORIENT_2X3;
		}
		else
		{
			/*-- 0.75 */
			return PB_IMG_ORIENT_3X4;
		}
	}
#endif  /*end PLAYBACK_ROTATE_BY_EXIF_TAG*/
}

/*-------------------------------------------------------------------------
 *  File Name : appPbImageOrientDimGet
 *------------------------------------------------------------------------*/
pbImageSizeGrade_t
appPbImageSizeGradeGet(
	UINT16 width,
	UINT16 height,
	UINT8 ratioType
)
{
	UINT32 length;
	if(ratioType)//90 or 270
	{
		length = height;
	}
	else//0 or 180
	{
		 length = width;
	}

	if(length > 4000)
	{
		if(length > 4600)
		{
			return PB_IMG_SIZE_GRADE_16;
		}
		else if(length > 4030)
		{
			return PB_IMG_SIZE_GRADE_12;
		}
	}
	if(length > 3000)
	{
		if(length > 3456)
		{
			return PB_IMG_SIZE_GRADE_10;
		}
		else if(length > 3264)
		{
			return PB_IMG_SIZE_GRADE_9;
		}
		else if(length > 3072)
		{
			return PB_IMG_SIZE_GRADE_8;
		}
		else
		{
			return PB_IMG_SIZE_GRADE_7;
		}
	}
	else if(length > 2000)
	{
		if(length > 2560)
		{
			return PB_IMG_SIZE_GRADE_6;
		}
		else if(length == 2560)
		{
			return PB_IMG_SIZE_GRADE_5;
		}
		else if(length > 2350 || height > 1536)
		{
			return PB_IMG_SIZE_GRADE_4;
		}
		else
		{
			return PB_IMG_SIZE_GRADE_3;
		}
	}
	else
	{
		if(length >= 1024)
		{
			return PB_IMG_SIZE_GRADE_2;
		}
		else if(length > 640)
		{
			return PB_IMG_SIZE_GRADE_1;
		}
		else
		{
			return PB_IMG_SIZE_GRADE_0;
		}
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appPbJpgMainPlayStart
 *------------------------------------------------------------------------*/
UINT32
appPbJpgMainPlayStart(
	void
)
{
	UINT32 ret=FAIL;

	if (!IS_MAIN_IMG_EXIST(gpbImgCb.exifImgFlag))
	{
		return FAIL;
	}

	gpbMainCb.playStat = PB_PLAY_STAT_JPG_MAIN_PLAYING;
	sp5kStillPlayCfgSet(SP5K_SPB_IMG_TYPE, SP5K_SPB_IMG_PRIMARY);
	sp5kModeSet(SP5K_MODE_STILL_PLAYBACK);
	ret=appHostMsgWaitExact(SP5K_MSG_STILLPLAY_DONE,(SP5K_SPB_IMG_PRIMARY&0xff)<<16,5000);
	if (ret ==FAIL)
	{
		gpbImgCb.isMainImgOK = FALSE;
		gpbMainCb.isPlayDone = FALSE;
		gpbMainCb.playStat = PB_PLAY_STAT_STANDBY;
		gpbMainCb.mainStat = PB_MAIN_STAT_STANDBY;
	}
	else
	{
		gpbMainCb.isPlayDone = TRUE;
		gpbImgCb.isMainImgOK = TRUE;
		gpbMainCb.mainStat = PB_MAIN_STAT_STANDBY;
		gpbMainCb.playStat = PB_PLAY_STAT_STANDBY;
	}

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbImagePlay
 *------------------------------------------------------------------------*/
UINT32
appPbImagePlay(
	UINT32 dcfIdx
)
{
	DBG_PRINT("pblib: %s: index(%d) \n", __FUNCTION__, dcfIdx);

	/*-- Reset current IMG info struct, gpbImgCb
	 */
	playImgPlayReset();

	/*-- Retrieve DCF attributes of current IMG
	 */
	gpbImgCb.dcfIdx = gcurrDcfIdx = dcfIdx;
	if (FAIL==playImgDcfAttrGet(&gpbImgCb))
	{
		return FAIL;
	}

	/*-- Start to playback IMG
	 */
	if (FAIL==playImgPlayStart())
	{
		return FAIL;
	}

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbImageShift_PreCheck
 *------------------------------------------------------------------------*/
UINT32
appPbImageShift_PreCheck(
	void
)
{
	if (gtotalImgCount<2)
	{
		return FAIL;
	}

	/*-- Check if Playback on-going
	 */
	if (IS_PB_JPG_PLAYING)
	{
		if (gpbImgCb.isThumbImgOK)
		{
			/*-- Thumb or QV has been played, abort MAIN playback */
			sp5kStillPlayAbort();
		}
		else
		{
			/*-- wait till thumb/QV ready, or MAIN completes */
			gpbMainCb.err = PB_ERR_PLAYING_BUSY;
			return FAIL;
		}
	}
	else if (IS_PB_VIDEO_TITLE_PLAYING || IS_PB_AUDIO_TITLE_PLAYING)
	{
		gpbMainCb.err = PB_ERR_PLAYING_BUSY;
		return FAIL;
	}

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbImageShiftRepeatCheck
 *------------------------------------------------------------------------*/
void
appPbImageShiftRepeatCheck(UINT32 dumbMs)
{
	if (dumbMs)
	{
		PB_SLEEP(dumbMs);
	}

	if(IS_KEY_LEFT_PRESSED)
	{
		sp5kHostMsgSend(APP_UI_MSG_PB_SHIFT_PRE, 0);
	}
	else if(IS_KEY_RIGHT_PRESSED)
	{
		sp5kHostMsgSend(APP_UI_MSG_PB_SHIFT_NEXT, 0);
	}
	else
	{
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appPbImageShift
 *------------------------------------------------------------------------*/
UINT32
appPbImageShift(
	UINT8	shiftDir
)
{
	return playImageShift(shiftDir, FALSE/*isJpgOnly*/, FALSE/*isSkipFileErr*/);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbJpgImageShift
 *------------------------------------------------------------------------*/
UINT32
appPbJpgImageShift(
	UINT8	shiftDir,
	BOOL isSkipFileErr
)
{
	return playImageShift(shiftDir, TRUE/*isJpgOnly*/, isSkipFileErr);
}

#if 0
/*-------------------------------------------------------------------------
 *  File Name : appPbImageDcfSave
 *------------------------------------------------------------------------*/
UINT32
appPbImageDcfSave(
	UINT8 saveOpt
)
{
	if (FAIL==sp5kImageEdit(SP5K_IMGEDIT_SAVE| SP5K_IMGEDIT_ATTR_TIME_SET, 0/*DCF*/, (saveOpt==PB_SAVE_NEW ? 1: 0), 0, 0))
	{
		return FAIL;
	}

	return SUCCESS;
}
#endif

/*-------------------------------------------------------------------------
 *  File Name : appPbImgExifInfoGet
 *------------------------------------------------------------------------*/
void
appPbImgExifDateTimeGet(
	UINT8 *pszDTBuf /* min size : 24 */
)
{
	sp5kExifDecodeTagValueGet(EXIF_FORMAT_EXIF, 0x9003, (UINT8 *)pszDTBuf);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbImgExifInfoGet
 *------------------------------------------------------------------------*/
UINT32
appPbImgExifInfoGet(
	pbImgExifInfo_t *pexifInfo
)
{
	memset(pexifInfo, 0 , sizeof(pbImgExifInfo_t));

	sp5kExifDecodeTagValueGet(EXIF_FORMAT_EXIF, 0xA002, (UINT8 *)&pexifInfo->imgWidth);
	sp5kExifDecodeTagValueGet(EXIF_FORMAT_EXIF, 0xA003, (UINT8 *)&pexifInfo->imgHeight);
	sp5kExifDecodeTagValueGet(EXIF_FORMAT_EXIF, 0x829A, (UINT8 *)&pexifInfo->expTime);
	sp5kExifDecodeTagValueGet(EXIF_FORMAT_EXIF, 0x829D, (UINT8 *)&pexifInfo->fNum);
	sp5kExifDecodeTagValueGet(EXIF_FORMAT_EXIF, 0x8827, (UINT8 *)&pexifInfo->iso);
	sp5kExifDecodeTagValueGet(EXIF_FORMAT_EXIF, 0x9209, (UINT8 *)&pexifInfo->flash);
	sp5kExifDecodeTagValueGet(EXIF_FORMAT_EXIF, 0x9003, (UINT8 *)&pexifInfo->szDate);
	sp5kExifDecodeTagValueGet(EXIF_FORMAT_EXIF, 0x9204, (UINT8 *)&pexifInfo->expBias);
	sp5kExifDecodeTagValueGet(EXIF_FORMAT_EXIF, 0xA403, (UINT8 *)&pexifInfo->wbMode);
	sp5kExifDecodeTagValueGet(EXIF_FORMAT_EXIF, 0x9208, (UINT8 *)&pexifInfo->lightSource);
	sp5kExifDecodeTagValueGet(EXIF_FORMAT_EXIF, 0x9207, (UINT8 *)&pexifInfo->meteringMode);
	sp5kExifDecodeTagValueGet(EXIF_FORMAT_EXIF, 0xA406, (UINT8 *)&pexifInfo->sceneMode);
	sp5kExifDecodeTagValueGet(EXIF_FORMAT_EXIF, 0x8822, (UINT8 *)&pexifInfo->expProgMode);
	#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	UINT8 buf[32], str[32];
	UINT32 dwbuf[6];
	sp5kExifDecodeTagValueGet(SP5K_IFD_TYPE_GPSIFD, SP5K_GPS_TAG_LATITUDEREF, (UINT8 *)&pexifInfo->latitudeRef);
	sp5kExifDecodeTagValueGet(SP5K_IFD_TYPE_GPSIFD, SP5K_GPS_TAG_LONGITUDEREF, (UINT8 *)&pexifInfo->longitudeRef);
	memset(buf,0, sizeof(buf));
	memset(dwbuf,0, sizeof(dwbuf));
	sp5kExifDecodeTagValueGet(SP5K_IFD_TYPE_GPSIFD, SP5K_GPS_TAG_LATITUDE, (UINT8 *)&buf);
	jet_LatiLogi_rational_to_string(buf,(char *)str);
	strcpy((char *)pexifInfo->latitude,(char *)str);
	sp5kExifDecodeTagValueGet(SP5K_IFD_TYPE_GPSIFD, SP5K_GPS_TAG_LONGITUDE, (UINT8 *)&buf);
	jet_LatiLogi_rational_to_string(buf,(char *)str);
	strcpy((char *)pexifInfo->longitude,(char *)str);
	sp5kExifDecodeTagValueGet(SP5K_IFD_TYPE_GPSIFD, SP5K_GPS_TAG_SPEED, (UINT8 *)&buf);
	jet_exif_speed_rational_to_string(buf,(char *)str);
	strcpy((char *)pexifInfo->speed,(char *)str);
	sp5kExifDecodeTagValueGet(SP5K_IFD_TYPE_GPSIFD, SP5K_GPS_TAG_IMGDIR, (UINT8 *)&buf);
	jet_exif_direction_rational_to_string(buf,(char *)str);
	strcpy((char *)pexifInfo->direction,(char *)str);
	#endif
	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbImgHistogramGet
 *------------------------------------------------------------------------*/
UINT32
appPbImgHistogramGet(
	UINT32	*pLumaBuf,	/* the buf ptr to store the calcualted histogram */
	UINT32	barNum,		/* number of statistics to be shown */
	UINT32  maxVal		/* designated max luma value */
)
{
	/*-- Descriptions :
	 * The Histogram table presents the distributions of Y(Luma) of the image.
	 * The X-axis represents the quantized luma spread.
	 * For example, if barNum=64, then X=0 depicts those luma from 0~(256/64).
	 * The Y-axis represnts the pixel numbers spreading in the luma range.
	 */
	//UINT32 ret = SUCCESS;
	UINT32 x,y,idx,barWidth;
	UINT32 imgW,imgH,tmpHistData[PB_HISTOGRAM_MAX_VALUE];
	UINT32 countMax;
	/* UINT64 tmpSum; */
	UINT32 imgYuvAddr;
	UINT8 *pbYuvData = NULL;
	UINT8 value;

	/*-- Calculate the Luma range of a 'bar' at X-axis */
	if (barNum > PB_HISTOGRAM_MAX_VALUE)
	{
		barNum = PB_HISTOGRAM_MAX_VALUE;
	}
	barWidth = PB_HISTOGRAM_MAX_VALUE / barNum;

	DBG_PRINT("pblib: barNum = %d\n", barNum);
	DBG_PRINT("pblib: barWidth  = %d\n", barWidth );

	/*-- Get Image YUV buffer address and dimension */
	sp5kStillPlayImageInfoGet(SP5K_SPB_IMG_QV,&imgYuvAddr,&imgW,&imgH, NULL, NULL, NULL, NULL);

	DBG_PRINT("pblib: PB_HISTO_BUFF_ADDR = 0x%x\n", imgYuvAddr);
	DBG_PRINT("pblib: PB_HISTO_BUFF_W = %d\n",imgW);
	DBG_PRINT("pblib: PB_HISTO_BUFF_H = %d\n",imgH);

	pbYuvData = (UINT8 *)imgYuvAddr;
	if ((pbYuvData == NULL) || (imgW == 0) || (imgH == 0))
	{
		PB_DBG(LEVEL_ERROR,"pblib: PB_HistogramGet() %s: %d",__FUNCTION__,__LINE__);
		return FAIL;
	}

	/*-- Firstly, find out the luma statistics */
	memset(tmpHistData, 0, PB_HISTOGRAM_MAX_VALUE*sizeof(tmpHistData[0]));
	for(y=0; y<imgH; y++)
	{
		UINT32 tmpCount;
		idx = y * imgW * 2;
			/*-- jump to next raw, each raw occupies W*2,
			 * because one pixel takes two bytes
			 */
		tmpCount = 0;
		for (x=0; x< imgW; x++)
		{
			value = *(pbYuvData + idx);
			tmpHistData[value]++;
			tmpCount++;
			/*-- Jump for 8Y4U4V buffer format */
			if ((tmpCount>0)&& ((tmpCount%8) == 0))
			{
				idx = idx + 8;
				tmpCount = 0;
			}
			idx++;
		}
	}

	#if HOST_DBG
	DBG_PRINT("pblib: ======== raw tmpHistData[] data ==========\n");
	for (x=0; x<PB_HISTOGRAM_MAX_VALUE/8; x++) {
		DBG_PRINT("\n[%03d]: ", x*8);
		for (y=0; y<8;  y++) {
			DBG_PRINT("%6d ", tmpHistData[8*x+y]);
		}
	}
	DBG_PRINT("\n");
	#endif

	/*-- Normalize the luma statistics :
	 * The original value in tmpHistData[x] is accumulated from whole image.
	 * There would be a problem that the larger the image size,
	 * the larger accumulation for the same scene the image is shot.
	 * Therefore, we have to normalize the luma statistics.
	 * (here, we take the LCD size as the target).
	 */
	for (x=0; x<PB_HISTOGRAM_MAX_VALUE; x++)
	{
		tmpHistData[x] = (tmpHistData[x] * (320*240)) / (imgW * imgH);
		//printf("tmpHistData[%d]=%d\n",x,tmpHistData[x]);
	}

	#if HOST_DBG
	for (x=0; x<PB_HISTOGRAM_MAX_VALUE/8; x++)
	{
		DBG_PRINT("\n[%03d]: ", x*8);
		for (y=0; y<8;  y++)
		{
			DBG_PRINT("%6d ", tmpHistData[8*x+y]);
		}
	}
	DBG_PRINT("\n");
	#endif


	/*-- Now, Go on and accumulate the pixel number a bar contains */
	idx = countMax = 0;
	for (x=0; x<PB_HISTOGRAM_MAX_VALUE; x++)
	{
		UINT64 tmpSum;
		tmpSum = tmpHistData[x];
		for (y=1; y<barWidth; y++)
		{
			tmpSum += tmpHistData[x+y];
		}
		x += barWidth - 1;
		pLumaBuf[idx] = tmpSum /barWidth;
		if (pLumaBuf[idx] > countMax)
		{
			countMax = pLumaBuf[idx];
		}
		idx ++;
	}
	barNum = idx;
	DBG_PRINT("pblib: countMax = %d\n", countMax);

	/*-- equalization */
	for (x=0; x<barNum; x++)
	{
		pLumaBuf[x] = (pLumaBuf[x] * (maxVal)) / countMax;
	}

	#if HOST_DBG
	DBG_PRINT("pblib: ======== Histgorm data ==========\n");
	for (x=0; x<barNum/8; x++)
	{
		DBG_PRINT("\n[%03d]: ", x*8);
		for (y=0; y<8;  y++)
		{
			DBG_PRINT("%4d ", pLumaBuf[8*x+y]);
		}
	}
	DBG_PRINT("\n");
	#endif

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbTotalFileCountGet
 *------------------------------------------------------------------------*/
UINT32
appPbTotalFileCountGet(
	void
)
{
	UINT32 totalFileCount = 0;

	#if SP5K_CDFS_OPT
	#if JET_PANA_CDFS
	totalFileCount = jet_file_list_filter_num_get();
	#else
	appCdfsFileNumGet(&totalFileCount);
	#endif
	#else

	sp5kDcfFsInfoGet(SP5K_DCF_INFO_TOTAL_FILENUM,&totalFileCount);
	#endif	/*end SP5K_CDFS_OPT */

	return (totalFileCount);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbScanNextJPG
 *------------------------------------------------------------------------*/
UINT32
appPbScanNextJPG(
	pbImageShiftDir_t scanDir,
	BOOL isSkipFileErr
)
{
	UINT32 dcfIdx;
	UINT32 scanCount;
	UINT32 scanResult;
	pbImgCb_t imgCb;

	if (gtotalImgCount<2)
	{
		DBG_PRINT("pblib: %s: !!warning!!, only 1 IMG !! \n", __FUNCTION__);
		isLastFileIdxFlag=TRUE;
		return SUCCESS;
	}

	if (!(scanDir==PB_SHIFT_NEXT || scanDir==PB_SHIFT_PRE))
	{
		PB_DBG(LEVEL_ERROR,"pblib: %s: !!error!!, illegal scanDir(%d)",__FUNCTION__,scanDir);
		return FAIL;
	}

	dcfIdx = gcurrDcfIdx;
	scanCount = gtotalImgCount;
	scanResult = FAIL;
	memset(&imgCb, 0, sizeof(pbImgCb_t));

	do
	{
		if (scanDir==PB_SHIFT_NEXT)
		{
			PB_INDEX_INC(dcfIdx, 1, gtotalImgCount);
		}
		else if (scanDir==PB_SHIFT_PRE)
		{
			PB_INDEX_DEC(dcfIdx, 1, gtotalImgCount);
		}
		DBG_PRINT("pblib: %s: Shifting to DCF index (%d of %d) \n", __FUNCTION__, dcfIdx, gtotalImgCount);

		/*-- Retrieve DCF attributes of current IMG
		 */
		sp5kDcfFsCurFileIdxSet(dcfIdx);
		imgCb.dcfIdx = dcfIdx;
		if (FAIL==playImgDcfAttrGet(&imgCb))
		{
			if (FALSE==isSkipFileErr)
			{
				PB_DBG(LEVEL_ERROR,"pblib: %s: Getting IMG Attr failed",__FUNCTION__);
				scanResult = FAIL;
				break;
			}
			else
			{
				goto lbl_scan_next_jpg_cont;
			}
		}

		if (IS_FILETYPE_JPG(imgCb.dcfAttr.fileType))
		{
			/*-- JPG IMG */
			if (FAIL==playJpgCheckup(&imgCb))
			{
				if (FALSE==isSkipFileErr)
				{
					/*-- JPG IMG w/t ERROR */
					PB_DBG(LEVEL_ERROR,"pblib: %s: Stayed at ERROR IMG as requested",__FUNCTION__);
					scanResult = SUCCESS;
					break;
				}
			}
			else
			{
				/*-- GOOD JPG IMG */
				DBG_PRINT("pblib: %s: JPG (%s) scanned, dcfIdx(%d) \n", __FUNCTION__, &imgCb.szDcfFileName[0], dcfIdx);
				scanResult = SUCCESS;
				break;
			}
		}
lbl_scan_next_jpg_cont:
		scanCount--;
	} while(scanCount>0);

	if (scanResult==FAIL)
	{
		return FAIL;
	}

	memcpy(&gpbImgCb, &imgCb, sizeof(pbImgCb_t));
	gcurrDcfIdx = dcfIdx;

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbBackdoorEventCBF
 *------------------------------------------------------------------------*/
UINT32
appPbBackdoorEventCBF(
	pbAppBackdoorEvent_t appEvent
)
{
	switch (appEvent)
	{
	default:
		return FAIL;
	case PB_APP_EVENT_DISK_FORMAT:
		memset(&gpbMainCb, 0, sizeof(pbMainCB_t));
		memset(&gpbDiskCb, 0, sizeof(pbDiskCB_t));
		/*-- Checkup Disk status */
		appPbDiskCheckup(&gpbDiskCb);
		/*-- check up IMG count */
		gtotalImgCount = gcurrDcfIdx = 0;
		gpbMainCb.isNoImage = TRUE;
		break;
	}

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : PB_HistogramGet
 *------------------------------------------------------------------------*/
#define PB_HISTOGRAM_VALUE_MAX    256
UINT32 appPbHistogramGet( UINT32 *pbHistoData, UINT32 stepX, UINT32 stepY)
{
	UINT32 ret = SUCCESS;
	UINT32 x,y,idx,barWidth,barNum;
	UINT32 bufW,bufH,tmpHistData[PB_HISTOGRAM_VALUE_MAX];
	UINT32 countMax;
	/* UINT64 tmpSum; */
	UINT32 YuvDataAddr;
	UINT8 *pbYuvData = NULL;

	barNum = stepX;
	barWidth = PB_HISTOGRAM_VALUE_MAX / barNum;
	if (barNum > PB_HISTOGRAM_VALUE_MAX)
	{
		barNum = PB_HISTOGRAM_VALUE_MAX;
	}

	sp5kStillPlayImageInfoGet(SP5K_SPB_IMG_QV, &YuvDataAddr, &bufW, &bufH, NULL, NULL, NULL, NULL);/*Edward 20080821*/

	if ((YuvDataAddr == 0) || (bufW == 0) || (bufH == 0))
	{
		return FAIL;
	}

	/* 21ms -> 7ms */
	pbYuvData = (UINT8 *)(YuvDataAddr&~0x20000000); /* cache space */
	memset(tmpHistData, 0, sizeof(tmpHistData));
	//calculating
	x = bufH*bufW*2;
	for (idx=0 ; idx<x ; idx+=16)
	{
		tmpHistData[pbYuvData[idx]]++;
		tmpHistData[pbYuvData[idx+1]]++;
		tmpHistData[pbYuvData[idx+2]]++;
		tmpHistData[pbYuvData[idx+3]]++;
		tmpHistData[pbYuvData[idx+4]]++;
		tmpHistData[pbYuvData[idx+5]]++;
		tmpHistData[pbYuvData[idx+6]]++;
		tmpHistData[pbYuvData[idx+7]]++;
	}

	idx = countMax = 0;
	for (x=0;x<PB_HISTOGRAM_VALUE_MAX;x+=barWidth)
	{
		UINT64 tmpSum;
		tmpSum = tmpHistData[x];
		for (y=1; y<barWidth; y ++)
		{
			tmpSum += tmpHistData[x+y];
		}
		pbHistoData[idx] = tmpSum / barWidth;
		if (pbHistoData[idx] > countMax)
		{
			countMax = pbHistoData[idx];
		}
		idx ++;
	}
	barNum = idx;
	for (x=0;x<barNum;x++)
	{
		pbHistoData[x] = (pbHistoData[x] * (stepY)) / countMax;
	}

	return ret;
}

  /*--------------------------------------------------------------------------*
 * Function name	: jet_pb_mediainfo_retriveve
 * Function	       	: retriveve the playback media information
 * Return value   	: result (UINT32 SUCCESS/FAIL)
 * Parameter1    	: void
 * History	   :  
 *                :  2018.04.12  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
  
UINT32 jet_pb_mediainfo_retriveve(void) // copy this function "playVideoTitlePlayStart"
{
	UINT32 w, h, frm;
	if(IS_HDMI_IN){
		w=1920;
        h=1080;
	}
	else{
		w=320;
        h=240;
	}
	/*-- Set MediaPlay filename :
	 * The filename should be set before parsing attribute !!
	 */
	if (FAIL==sp5kMediaPlayAttrSet(SP5K_MEDIA_ATTR_FILE_NAME,(UINT32)gpbImgCb.szDcfFileName))
	{
		PB_DBG(LEVEL_ERROR,"pblib: %s:Media filename set failed",__FUNCTION__);
		return FAIL;
	}
	sp5kMediaPlayCfgSet(SP5K_MEDIA_PLAY_YUV_SCALE_DIV, 0);
	/*-- Retrieve media information */
    if(gpbMainCb.err!=PB_ERR_FILE_ERROR)
    {
		sp5kMediaPlayAttrGet(SP5K_MEDIA_ATTR_DURATION, &gpbImgCb.mediaTotalTime);
		sp5kMediaPlayAttrGet(SP5K_MEDIA_ATTR_WIDTH, &gpbImgCb.imgWidth);
		sp5kMediaPlayAttrGet(SP5K_MEDIA_ATTR_HEIGHT, &gpbImgCb.imgHeight);
    }
    else
    {
        gpbImgCb.mediaTotalTime = gpbImgCb.imgWidth = gpbImgCb.imgHeight = 0;
        appPlayOsd_ShowFileTime(TRUE,gpbImgCb.mediaTotalTime);
        appPlayOsd_MainInfoDraw(PB_OSD_DRAW_NORMAL);
    }
	/*-- Determine sizeGrade */
	UINT32 i;
	for (i=0; i<UI_VIDEO_SIZE_MAX; i++)
	{
		appVideoSizeGet(i, &w, &h, &frm);
		if (gpbImgCb.imgWidth==w && gpbImgCb.imgHeight==h)
		{
			gpbImgCb.sizeGrade = PB_VIDEO_SIZE_GRADE_1+i;
			break;
		}
	}
	if (i==UI_VIDEO_SIZE_MAX)
	{
		gpbImgCb.sizeGrade = PB_VIDEO_SIZE_GRADE_UNKNOWN;
	}

	gpbMainCb.playStat = PB_PLAY_STAT_STANDBY;
	gpbMainCb.isPlayDone = TRUE;
	gpbImgCb.isMainImgOK = gpbImgCb.isThumbImgOK = TRUE;
	PB_DBG(LEVEL_INFO,"pblib: video %s, total(%d), size(%d x %d)",
	                 gpbImgCb.szDcfFileName,
	                 gpbImgCb.mediaTotalTime,
	                 gpbImgCb.imgWidth,
	                 gpbImgCb.imgHeight);

	return SUCCESS;
}
BOOL isInPIPFlag = FALSE;

  /*--------------------------------------------------------------------------*
 * Function name	: jet_pb_image_pip_is_in_pip_get
 * Function	       	: get flag to check in pip mode or not
 * Return value   	: result (BOOL TRUE/FALSE)
 * Parameter1    	: void
 * History	   :  2017.08.18  scchang	  "First edit"
 *                :  2018.04.12  Paul.Chang     "Modify description"
 *--------------------------------------------------------------------------*/
   
BOOL jet_pb_image_pip_is_in_pip_get(void)
{
	printf("[%s] isInPIPFlag = %d\n", __FUNCTION__,isInPIPFlag);
	return isInPIPFlag;
}

  /*--------------------------------------------------------------------------*
 * Function name	: jet_pb_image_pip_is_in_pip_set
 * Function	       	: set flag to enter pip mode or not
 * Return value   	: result (BOOL TRUE/FALSE)
 * Parameter1    	: void
 * History	   :  2017.08.18  scchang	  "First edit"
 *                :  2018.04.12  Paul.Chang     "Modify description"
 *--------------------------------------------------------------------------*/
   
void jet_pb_image_pip_is_in_pip_set(BOOL set)
{
	isInPIPFlag = set;
	printf("[%s] isInPIPFlag = %d\n", __FUNCTION__,isInPIPFlag);
}

  /*--------------------------------------------------------------------------*
 * Function name	: jet_pb_image_pip_enter
 * Function	       	: set standby mode to enter pip
 * Return value   	: void
 * Parameter1    	: void
 * History	   :  2017.08.18  scchang	  "First edit"
 *                :  2018.04.12  Paul.Chang     "Modify description"
 *--------------------------------------------------------------------------*/
   
void jet_pb_image_pip_enter(void)
{
	printf("[%s]\n", __FUNCTION__);
	sp5kModeCfgSet(SP5K_MODE_CFG_STANDBY_DUP, 1);
	sp5kModeSet(SP5K_MODE_STANDBY);
	sp5kModeWait(SP5K_MODE_STANDBY);
}

  /*--------------------------------------------------------------------------*
 * Function name	: jet_pb_image_pip_init
 * Function	       	: initialize pip
 * Return value   	: result (BOOL TRUE/FALSE)
 * Parameter1    	: void
 * History	   :  2017.08.18  scchang	  "First edit"
 *                :  2018.04.12  Paul.Chang     "Modify description"
 *--------------------------------------------------------------------------*/
   
BOOL jet_pb_image_pip_init(void)
{
    UINT32 pipW=320,pipH=240,ret;
	printf("[%s]\n", __FUNCTION__);
	sp5kRsvPreloadCfg(1);
	sp5kPipInitCfgSet(SP5K_PIP_INIT_MAIN_FRAME_SIZE, pipW, pipH, 0, 0);
	ret = sp5kPipInit();
	if(ret!=0){
		printf("fail sp5kPipInit ret=%u\n", ret);
		return FAIL;
	}
	sp5kGfxRectFill(SP5K_GFX_PAGE_PIP_MAIN, 0, 0, pipW, pipH, 0x800080);
	return SUCCESS;
}

  /*--------------------------------------------------------------------------*
 * Function name	: jet_pb_image_pip_preload
 * Function	       	: preload pip
 * Return value   	: result (BOOL TRUE/FALSE)
 * Parameter1    	: void
 * History	   :  2017.08.18  scchang	  "First edit"
 *                :  2018.04.12  Paul.Chang     "Modify description"
 *--------------------------------------------------------------------------*/
   
BOOL jet_pb_image_pip_preload(char *filename)
{
	UINT32 qvW=960,qvH=540;
	printf("[%s]\n", __FUNCTION__);
	if(filename==NULL){
		printf("[%s] filename is null.\n",__FUNCTION__);
		return FAIL;
	}
	if(sp5kRsvPreloadFile(SP5K_RES_TYPE_JPG|SP5K_RES_SUBTYPE_JPG_MAIN , filename, 0, qvW, qvH, 0x821)){
		printf("sp5kRsvPreloadFile %s FAIL!!\n", filename);
		return FAIL;
	}
	return SUCCESS;
}

  /*--------------------------------------------------------------------------*
 * Function name	: jet_pb_image_pip_play
 * Function	       	: play pip
 * Return value   	: result (BOOL TRUE/FALSE)
 * Parameter1    	: void
 * History	   :  2017.08.18  scchang	  "First edit"
 *                :  2018.04.12  Paul.Chang     "Modify description"
 *--------------------------------------------------------------------------*/
   
void jet_pb_image_pip_play(void)
{
	UINT32 qvW=960,qvH=540,pipW=320,pipH=240;
	//source image load and scale to x=0,y=0,width=160,height=120
	UINT32 src[4]={0,0,qvW,qvH};
	//put image at x=0,y=0 and destination width=160 ,height=120
	UINT32 dst[4]={0,0,pipW,pipH};
	int x=0;
	//Loop to draw RSV preload images to PIP
	if(sp5kRsvPreloadDrawRoi(x, src, SP5K_GFX_PAGE_PIP_MAIN, dst, 0)){
		printf("sp5kRsvPreloadDrawRoi id:%x FAIL!!\n",x);
	}
	//show the pip layer
	sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE, 1, 0, 1, 0); // no wait
}

  /*--------------------------------------------------------------------------*
 * Function name	: jet_pb_image_pip_exit
 * Function	       	: exit pip
 * Return value   	: result (BOOL TRUE/FALSE)
 * Parameter1    	: void
 * History	   :  2017.08.18  scchang	  "First edit"
 *                :  2018.04.12  Paul.Chang     "Modify description"
 *--------------------------------------------------------------------------*/
   
void jet_pb_image_pip_exit(void)
{
	//release the preload buffer
	printf("[%s]\n", __FUNCTION__);
	sp5kRsvPreloadRelease();
	sp5kModeCfgSet(SP5K_MODE_CFG_STANDBY_DUP, 1);
	sp5kModeSet(SP5K_MODE_STANDBY);
	sp5kModeWait(SP5K_MODE_STANDBY);
	//release pip layer
	sp5kPipTerm();			
}

