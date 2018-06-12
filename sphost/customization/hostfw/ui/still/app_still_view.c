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
#include "sp5k_capture_api.h"
#include "sp5k_disp_api.h"
#include "sp5k_otrack_api.h"
#include "sp5k_usb_api.h"
#include "sp5k_usbhost_api.h"
#include "sp5k_rsvblk_api.h"
#include "sp5k_disk_api.h"
#include "sp5k_cdsp_api.h"
#include "api/sp5k_moe_api.h"
#include "sp5k_gfx_api.h"
#include "sp5k_af_api.h"
#include "sp5k_media_api.h"

#include "app_pal.h"
#include "app_icon_def.h"
#include "app_zoom_api.h"
#include "app_lens_api.h"
#include "app_still.h"
#include "app_sound.h"
#include "app_exif.h"
#include "app_view_param.h"
#include "app_strobe.h"
#include "app_led.h"
#include "app_view_osd.h"
#include "app_error_msg.h"
#include "app_osd.h"
#include "app_res_def.h"
#include "app_osd_api.h"
#include "app_osdfont.h"
#include "app_face_detect_ex.h"
#include "app_video.h"
#include "app_calib_api.h"
#include "app_errorosd.h"
#include "app_3aalg.h"
#include "app_pal.h"
#include "app_setup_lib.h"
#include "app_awbalg_api.h"
#include "app_cdfs_api.h"
#include "app_script.h"
#include "app_user_setting.h"
#include "app_playback_def.h"
#include "app_playback_uiparam.h"
#include "app_flash_api.h"
#include "app_simple_menu.h"
#include "app_btn_def.h"
#include "app_key_def.h"
#include "app_aaa.h"
#include "customization/app_init.h"
#include "app_sensor.h"

#include "app_disk.h"
#include "sp5k_dbg_api.h"
#if SP5K_DIQ_FRMWRK
	#include "sp5k_dq_api.h"
#endif
#include "app_ui_para.h"

#if SP5K_MOTION_DETECT || SP5K_MOTION_DETECT_FRONT || SP5K_MOTION_DETECT_REAR
#include "app_md.h"
#endif
#if SP5K_MOTION_DETECT_FRONT || SP5K_MOTION_DETECT_REAR
#include <stdint.h>
static UINT32 g_nFrameId = 0;
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
#if S2STEST //test for s2s
static UINT8 S2S=1;
#endif

appStillCB_t gStillCB = {
	.viewParam = NULL,
	.cap = {
		.CurViewMode = PASM_DEFAULT,
		.CurScene = 0,
		.qvSec = 3
	},
	.ae = {
		.iris = APERTURE_2_8,
		.shutter = SHUTTER_1_2000S,
		.iso = ISO_100,
		.ev = 0.0,
		.av = APERTURE_2_8,
		.tv = SHUTTER_1_2000S
	},
	.diskClusterSize = 16,
	.estFileSize=3000,
	.estJobFilesSize=3000,
	.remainImg=0,
	.s1press=0,
	.s2press=0,
	.s1AaaTicket=0,
	.s2SnapTicket=0,
	.tvAvSel=0,
	.flashCharging=0,
	.lensZoomSeg=0,
	.amodeApertureIdx=0,
	.smodeShutterIdx=SHUTTER_1_2000S,
	.needFlash=0,
	.focusStatus=FOCUS_STATUS_VIEW,
	.needDarkFrameSub=0,
	.isDiskReady=0,
	.ftStatus= VIEW_FT_STATUS_DESTROY,
	.isLensReady=FALSE,
	.curPhotoFrameIdx = 0,
	.flashChargeMonitorTimer = TIMER_NULL,
	.flashMode =  UI_FLASH_AUTO,
	.awbSpeed = SP5K_AWB_SPEED_NORMAL,
	.err=VIEW_ERR_NULL,
	.osd = OSD_VIEW_DEFAULT
};
extern UINT32 flash_idx;
extern UINT32 isBatteryCheckEnable ;
UINT8 StillCapModeBak[2]={PASM_DEFAULT, 0};

extern UINT32 PeriodicalCaptureTime;
UINT32 PeriodicalCapTimer = TIMER_NULL;
extern UINT8 fileTypeSelect;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern void appAeFaceCfg(void);
extern void Icatsnap3Acb(void);
extern void appPeriodicalCaptureTimeSet(void);
extern UINT8 isSensorResume;
extern UINT32 StationModeFirst;

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void appStillPvStateOnKey(UINT32 msg,UINT32 param);
#if SP5K_SENSOR_DUAL_MODE
#define DUAL_RAW_SEN_MODE        1
#define DUAL_YUV_SEN_MODE        2
#define DUAL_RAW_YUV_SEN_MODE    3
#define STREAM_MAIN_PATH         0
#define STREAM_SEC_PATH          1
#define MAX_SENSOR_NUM           2
#define ROUND_DOWN_TO_DIVISIBLE(num,div) ( (UINT32)(num) & -(UINT32)(div) )
#define ROUND_UP_TO_DIVISIBLE(num,div) ROUND_DOWN_TO_DIVISIBLE( (UINT32)(num) + (div) - 1, div )
#define DUAL_PIP_RATION 30
static UINT8 dual_pip_mode = DUALSEN_SINGLE_MAIN;
static frameBufInfo_t pvmain;
static frameBufInfo_t pv2nd;
static sp5kGfxObj_t imgSrc, imgDst;
static sp5kGfxMapProjectParam_t  proPrm;
static UINT32 ImeCbInited = 0;
static dualModeStatus_e old_DualMode;
#define appTbCIPA_CapNumShow(a)

/*-------------------------------------------------------------------------
 *  Function Name : appDualPipModeGet
 *  Description :
 *------------------------------------------------------------------------*/
UINT8 appDualPipModeGet(void)
{
    return dual_pip_mode;
}

/*-------------------------------------------------------------------------
 *  Function Name : appRawYuvImeTrigCb
 *  Description :
 *------------------------------------------------------------------------*/
UINT32 appRawYuvImeTrigCb(
    UINT32 streamId,        /* nth stream, from 0 */
    UINT32 fid,             /* frame id */
    frameBufInfo_t *pframe, /* buffer information */
    UINT8 *pbufdup          /* duplicated buffer address */
)
{
    if(dual_pip_mode == DUALSEN_PIP_MAIN || dual_pip_mode == DUALSEN_PIP_SEC){
        UINT32 srcRoiw, srcRoih, srcRoix, srcRoiy;
        UINT32 dstRoiw, dstRoih, dstRoix, dstRoiy;
        UINT32 offsetx, offsety;
        if (ImeCbInited) {
            sp5kDispUrgentCallbackYuvGet(&pvmain, 0, 0);
            sp5kDispUrgentCallbackYuvGet(&pv2nd, 1, 0);
        }
        else {
            return FAIL;
        }
        if( !pvmain.pbuf || !pv2nd.pbuf || !pframe->pbuf  ) {
                printf("\n%u %u 0x%x PbFrm=%d %d x=%d y=%d w=%d h=%d\n",streamId,fid,(UINT32)pframe->pbuf,
                    pframe->width, pframe->height,
                    pframe->roiX, pframe->roiY,
                    pframe->roiW, pframe->roiH);

                printf("0x%x mainFrm=%d %d x=%d y=%d w=%d h=%d\n",(UINT32)pvmain.pbuf,
                    pvmain.width, pvmain.height,
                    pvmain.roiX, pvmain.roiY,
                    pvmain.roiW, pvmain.roiH);

                printf("0x%x 2ndFrm=%d %d x=%d y=%d w=%d h=%d\n",(UINT32)pv2nd.pbuf,
                    pv2nd.width, pv2nd.height,
                    pv2nd.roiX, pv2nd.roiY,
                    pv2nd.roiW, pv2nd.roiH);
                return FAIL;
        }

        if (streamId == 1) {
            dstRoiw = pframe->roiW;
            dstRoih = pframe->roiH;
            dstRoix = pframe->roiX;
            dstRoiy = pframe->roiY;
            imgSrc.pbuf = pvmain.pbuf;
            imgSrc.bufW = pvmain.width;
            imgSrc.bufH = pvmain.height;
            srcRoiw = imgSrc.roiW = pvmain.roiW;
            srcRoih = imgSrc.roiH = pvmain.roiH;
            srcRoix = imgSrc.roiX = pvmain.roiX;
            srcRoiy = imgSrc.roiY = pvmain.roiY;
            offsetx = dstRoiw;
            offsety = dstRoih;
        }
        else {
            dstRoiw = pframe->roiW;
            dstRoih = pframe->roiH;
            dstRoix = pframe->roiX;
            dstRoiy = pframe->roiY;
            imgSrc.pbuf = pv2nd.pbuf;
            imgSrc.bufW = pv2nd.width;
            imgSrc.bufH = pv2nd.height;
            if((pv2nd.roiW * 3/4) == pv2nd.roiH){    //4:3
                srcRoiw = imgSrc.roiW = pv2nd.roiW;
                srcRoih = imgSrc.roiH = (pv2nd.roiH * 3 ) >> 2;
                srcRoix = imgSrc.roiX = pv2nd.roiX;
                srcRoiy = imgSrc.roiY = pv2nd.roiH >> 3;
            }
            else{
                srcRoiw = imgSrc.roiW = pv2nd.roiW;
                srcRoih = imgSrc.roiH = pv2nd.roiH;
                srcRoix = imgSrc.roiX = pv2nd.roiX;
                srcRoiy = imgSrc.roiY = pv2nd.roiY;
            }
            offsetx = dstRoiw;
            offsety = dstRoih;
        }
        dstRoiw = ROUND_DOWN_TO_DIVISIBLE((dstRoiw*DUAL_PIP_RATION/100)&(~0x0001), 16);
        dstRoih = ROUND_DOWN_TO_DIVISIBLE((dstRoih*DUAL_PIP_RATION/100)&(~0x0001), 16);
        dstRoix = offsetx - dstRoiw;
        dstRoiy = offsety - dstRoih;
        imgSrc.fmt = imgDst.fmt = SP5K_GFX_FMT_YUV422;
        imgDst.pbuf = pframe->pbuf;
        imgDst.bufW = pframe->width;
        imgDst.bufH = pframe->height;
        imgDst.roiW = dstRoiw;
        imgDst.roiH = dstRoih;
        imgDst.roiX = dstRoix;
        imgDst.roiY = dstRoiy;
        proPrm.mapAttr.itpAlg = SP5K_GFX_MAP_ITP_BILINEAR; /*IME_ITP_BILINEAR*/
        proPrm.mapAttr.bgClrY = 0x00;
        proPrm.mapAttr.bgClrU = 0x80;
        proPrm.mapAttr.bgClrV = 0x80;
        proPrm.mapAttr.ctMode = SP5K_GFX_MAP_COLOR_TRANS_BYPASS; /*IME_COLOR_TRANS_BYPASS*/
        proPrm.mapAttr.ctLut  = 0;
        proPrm.mapAttr.pctTblY= NULL;
        proPrm.mapAttr.pctTblU= NULL;
        proPrm.mapAttr.pctTblV= NULL;
        proPrm.mapAttr.waitMode= SP5K_GFX_MAP_MODE_DIRECT_WAIT;
        proPrm.vec.src[0].x = srcRoix;
        proPrm.vec.src[0].y = srcRoiy;
        proPrm.vec.src[1].x = srcRoix+srcRoiw-1;
        proPrm.vec.src[1].y = srcRoiy;
        proPrm.vec.src[2].x = srcRoix+srcRoiw-1;
        proPrm.vec.src[2].y = srcRoiy+srcRoih-1;
        proPrm.vec.src[3].x = srcRoix;
        proPrm.vec.src[3].y = srcRoiy+srcRoih-1;
        proPrm.vec.dst[0].x = dstRoix;
        proPrm.vec.dst[0].y = dstRoiy;
        proPrm.vec.dst[1].x = dstRoix+dstRoiw-1;
        proPrm.vec.dst[1].y = dstRoiy;
        proPrm.vec.dst[2].x = dstRoix+dstRoiw-1;
        proPrm.vec.dst[2].y = dstRoiy+dstRoih-1;
        proPrm.vec.dst[3].x = dstRoix;
        proPrm.vec.dst[3].y = dstRoiy+dstRoih-1;
            sp5kGfxObjectMapping(
            &imgSrc,
            &imgDst,
            SP5K_GFX_MAP_MODE_STILL,
            SP5K_GFX_MAPPING_ID_PROJECT,
            (UINT32)&proPrm
        );
    }
#if SP5K_MOTION_DETECT_FRONT || SP5K_MOTION_DETECT_REAR
    if(UINT32_MAX == g_nFrameId){
        g_nFrameId = 0;
    }
    ++g_nFrameId;
#if SP5K_MOTION_DETECT_FRONT
    appMotionDetectHdr(&g_nFrameId, CAM_FRONT_YUV);
#endif
#if SP5K_MOTION_DETECT_REAR
    appMotionDetectHdr(&g_nFrameId, CAM_REAR_YUV);
#endif
#endif
    return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  Function Name : appDualModeImeCbUnReg
 *  Description :
 *------------------------------------------------------------------------*/
UINT32 appDualModeImeCbUnReg(UINT32 mode)
{
    if ( ImeCbInited ) {
        HOST_PROF_LOG_ADD(LEVEL_INFO, "spv: ime disp cb Unreg");
        appDispUrgentSet(DISP_URGENT_PIP, 0);
    }
    else {
        HOST_PROF_LOG_PRINT(LEVEL_ERROR, "spv: sp5k mode error");
        return FAIL;
    }
    ImeCbInited = 0;
    return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  Function Name : appDualModeImeCbReg
 *  Description :
 *------------------------------------------------------------------------*/
UINT32 appDualModeImeCbReg(UINT32 mode)
{
    if ( !ImeCbInited ) {
        appDispUrgentSet(DISP_URGENT_PIP, 1);
    }
    else {
        HOST_PROF_LOG_PRINT(LEVEL_ERROR, "spv: already register callback");
        return FAIL;
    }
    ImeCbInited = 1;
    return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  Function Name : appStillDualModePIP
 *  Description :
 *------------------------------------------------------------------------*/
void appStillDualModePIP(dualModeStatus_e DualMode)
{
    UINT32 pmode;
    dual_pip_mode = DualMode;
    sp5kModeGet(&pmode);
#if SINGLE_DUAL_SWITCH_SAMPLE
    if (isSensorResume == 0)
        dual_pip_mode = DUALSEN_SINGLE_MAIN;
#endif
#if SUPPORT_RUNTIME_SWITCH_DRAM_CLK
    appSysClkSwitch(1);
#endif
    UINT32 nFitCustSizeCfg = 2;     //Crop the Source of PbPv to meet cust size
    sp5kModeCfgSet(SP5K_MODE_CFG_PV_DISP_SCALE_FACTOR, 320 , 180, (nFitCustSizeCfg << 16) | 1);
#if SINGLE_DUAL_SWITCH_SAMPLE
    appSwitchSingleInDual(FALSE);
#endif
	if (pmode != SP5K_MODE_STANDBY) {
	    sp5kModeCfgSet(SP5K_MODE_CFG_STANDBY_DUP, 1);
	    sp5kModeSet(SP5K_MODE_STANDBY);
	    sp5kModeWait(SP5K_MODE_STANDBY);
	}
#if SINGLE_DUAL_SWITCH_SAMPLE
    appVideoSettingInit(VIDEO_SET_ALL);
#endif
    if( ImeCbInited ){
        appDualModeImeCbUnReg(pmode);
    }
    appDualModeCfgSet(dual_pip_mode);
    appVideoDualStreamCfgSet(dual_pip_mode);///////should be mark, appVideoSizeSet() already call.
    if (dual_pip_mode >= DUALSEN_SINGLE_MAIN){
        appDualModeImeCbReg(pmode);
    }
    sp5kModeSet(pmode);
    sp5kModeWait(pmode);
}
void appSwitchSingleInDual(
    BOOL bSingle
)
{
	if(bSingle)
	{
        #if SP5K_DIQ_HDR
        if (HDR_En)
			pViewParam->videoSize = UI_VIDEO_SIZE_FHD_HDR;
        else
        #endif
            pViewParam->videoSize = UI_VIDEO_SIZE_FHD_60FPS;
	}
	else
	{
		pViewParam->videoSize = UI_VIDEO_DUAL_SIZE_FHD;
	}
    printf("\n%s_(#%d) Index switch to %d\n",__FUNCTION__, __LINE__, pViewParam->videoSize);
}
UINT8 appSingleModeCheck(
    void
)
{
	UINT8 ret;
    if (appDualPipModeGet() > DUALSEN_SINGLE_MAIN)
        ret = APP_VIDEO_MODE_DUAL;
    else
        ret = APP_VIDEO_MODE_SINGLE;
    return ret;
}
void appAutoSwitchUI(
    void
)
{
    if (isSensorResume == 0)
    {
        if (appDualPipModeGet() != DUALSEN_SINGLE_MAIN)
        {
            old_DualMode = appDualPipModeGet();
            appStillDualModePIP(DUALSEN_SINGLE_MAIN);
            appViewOsd_DualModeDraw();
        }
    }
    else if (isSensorResume == 1)
    {
        if (old_DualMode != DUALSEN_SINGLE_MAIN)
        {
            appStillDualModePIP(old_DualMode);
            old_DualMode = DUALSEN_SINGLE_MAIN;
            appViewOsd_DualModeDraw();
        }
    }
}
#endif

/*-------------------------------------------------------------------------
 *  Function Name : _StillCheckDiskReady
 *  Description :
 *------------------------------------------------------------------------*/
UINT32
_StillCheckDiskReady(void)
{
	if (!IS_DISK_MOUNT_READY)
	{
		return FALSE;
	}

	if(IS_CARD_EXIST && appActiveDiskGet() == SP5K_DRIVE_SD)
		gStillCB.isDiskReady = TRUE;
	else if(!IS_CARD_EXIST && appActiveDiskGet() == SP5K_DRIVE_NAND)
		gStillCB.isDiskReady = TRUE;
	else if(appActiveDiskGet() == SP5K_DRIVE_USB)
		gStillCB.isDiskReady = TRUE;
	else if(STATE_GROUP(appPreviousStateGet())==APP_STATE_GROUP_PLAYBACK)
		gStillCB.isDiskReady = TRUE;
	else
		return FALSE;

	if(IS_DISK_MOUNT_FAIL)
	{
		gStillCB.err = VIEW_ERR_CARD_ERROR;
	}
	else
	{
		gStillCB.err = VIEW_ERR_NO_ERROR;
		appStillSettingInit(VIEW_SET_DISK);
	}
	return TRUE;
}

/*-------------------------------------------------------------------------
 *  Function Name : appStillWaitJobFinish
 *  Description :
 *------------------------------------------------------------------------*/
UINT32
appStillWaitJobFinish(void)
{
	UINT32 waitJob, availabe;

	sp5kStillCapJobInfo(&waitJob, &availabe);

	while(waitJob)
	{
		APP_SLEEP_MS(20);
		sp5kStillCapJobInfo(&waitJob, &availabe);
	}
	return 0;
}

/*-------------------------------------------------------------------------
 *  Function Name : appStillCheckReady
 *  Description :
 *------------------------------------------------------------------------*/
UINT32
appStillCheckReady(UINT32 question)
{
	UINT32 dskfree, ret, remain_number;

	if(gStillCB.err == VIEW_ERR_CARD_ERROR)
	{
		return STILL_STATE_NO;
	}

	switch (question)
	{
	default:
	case CAN_I_START_SNAP:

		if(!gStillCB.isDiskReady)
		{
			HOST_PROF_LOG_PRINT(LEVEL_WARN, "spv: %s: Fail (disk not ready)", __FUNCTION__);
			return STILL_STATE_NO;
		}
		if(IS_CARD_EXIST && IS_CARD_LOCK)
		{
			HOST_PROF_LOG_PRINT(LEVEL_WARN, "spv: %s: Fail (card lock)", __FUNCTION__);
			gStillCB.err = VIEW_ERR_CARD_LOCKED;
			return STILL_STATE_NO;
		}

		ret = appStillBackgroundGetFree(&dskfree);
		remain_number = dskfree/gStillCB.estJobFilesSize;
		if(remain_number==0 && ret==SUCCESS)
		{
			HOST_PROF_LOG_PRINT(LEVEL_ERROR, "spv: %s Fail (disk space not enough)", __FUNCTION__);
			HOST_PROF_LOG_PRINT(LEVEL_WARN, "spv: %s: disk-free = %d kbyte, need size = %d kbyte"\
								,__FUNCTION__ ,dskfree, gStillCB.estJobFilesSize);
			gStillCB.err = VIEW_ERR_MEMORY_FULL;
			return STILL_STATE_MEMORY_FULL;
		}
		else if(remain_number<=2 && ret==FAIL)
		{
			HOST_PROF_LOG_PRINT(LEVEL_ERROR, "spv: %s Fail (disk space close to empty)", __FUNCTION__);
			HOST_PROF_LOG_PRINT(LEVEL_WARN, "spv: %s: disk-free = %d kbyte, need size = %d kbyte"\
								,__FUNCTION__ ,dskfree, gStillCB.estJobFilesSize);
			gStillCB.err = VIEW_ERR_NO_ERROR;
			return STILL_STATE_NO;
		}
		else
		{
			gStillCB.err = VIEW_ERR_NO_ERROR;
		}

		if(!appStrobeQueryReady() && gStillCB.flashMode!=UI_FLASH_OFF)
		{
			HOST_PROF_LOG_PRINT(LEVEL_ERROR, "spv: %s Fail (flash is charging)", __FUNCTION__);
			HOST_PROF_LOG_PRINT(LEVEL_WARN, "spv: %s: flashready=%d"\
								,__FUNCTION__ , appStrobeQueryReady());
			return STILL_STATE_FLASH_CHARGIN;
		}

		if(appLensQuery(LENS_QUERY_MOTION_STATE))
		{
			HOST_PROF_LOG_PRINT(LEVEL_WARN, "spv: lens is moving, can not do aaa");
			return STILL_STATE_NO;
		}
		break;
	case CAN_I_LEAVE_STILL:
		break;
	}

	return STILL_STATE_YES;
}

/*-------------------------------------------------------------------------
 *  Function Name : appStillSettingInit
 *  Description :
 *------------------------------------------------------------------------*/
void
appStillSettingInit(
	UINT32 flg
)
{
	appDiskInfo_t* pDsk;
	UINT32 cfg;

	if(flg & VIEW_SET_STATE_START)
	{
		appStill_SetQuality(pViewParam->stillQuality);
		/*appModeSet(SP5K_MODE_STILL_PREVIEW);*/
		appStill_SetMetering(pViewParam->metering);
		appStill_SetWB(pViewParam->wb);
		appStill_SetISO(pViewParam->iso);
		appStill_SetExposure(pViewParam->ev);

		#if !SP5K_PVQV_DISABLE
		appStillQVCbSet(FALSE);
		#endif

		appStill_SetDriveMode(pViewParam->stillDriverMode);

		appStill_SetPasmSceneParam(gStillCB.cap.CurViewMode, gStillCB.cap.CurScene);
		appExif_SetExpProgram(gStillCB.cap.CurViewMode, gStillCB.cap.CurScene);
		appExif_SetSceneCaptureType(gStillCB.cap.CurViewMode, gStillCB.cap.CurScene);
		appStill_SetSharpness(pViewParam->edge);
		appStill_SetStabilizer(pViewParam->stablizer);
		appStill_SetFlash(gStillCB.flashMode);
		appStill_SetApertureValue(APERTURE_AUTO);

		appAeFrameRateSet(60);

		applightFrequencySetting(pUiSetting->lightFrequency);
	}

	if(flg == VIEW_SET_ALL)
	{
		StillCapModeBak[0] = gStillCB.cap.CurViewMode = PASM_DEFAULT;
		StillCapModeBak[1] = gStillCB.cap.CurScene = 0;

		appStill_SetPasmSceneParam(PASM_DEFAULT,0);
		appExif_SetExpProgram(PASM_DEFAULT,0);
		appExif_SetSceneCaptureType(PASM_DEFAULT, 0);
	}

	if(flg & VIEW_SET_DISK)
	{
		pDsk = appCurDiskInfoGet();
		if(pDsk->actDsk)
		{
			if(pDsk->cluSz > 1024)//large sector
			{
				gStillCB.diskClusterSize = pDsk->cluSz >>10;//unit in Kbyte
			}
			else//small sector
			{
				gStillCB.diskClusterSize = 1;//unit in Kbyte
			}
			appStillBackgroundInit(pDsk->cluSz, pDsk->freeSz, gStillCB.estFileSize);
			appStillCheckReady(CAN_I_START_SNAP);
		}
	}

	gStillCB.estFileSize = appStillEstFileSize(pViewParam->stillSize, pViewParam->stillQuality, gStillCB.diskClusterSize);
	cfg = (pViewParam->stillDriverMode != UI_STILL_DRIVERMODE_OFF)? 3 : 1;
	gStillCB.estJobFilesSize = gStillCB.estFileSize*cfg;
	appStillBackgroundSet(gStillCB.estFileSize);
	appStill_SetDateStamp(pViewParam->stillStamp, pUiSetting->dateTime);

	// This may be set to zero in caputure state to interrupt the QV.
	cfg = gStillCB.cap.qvSec = 1;
	sp5kStillCapCfgSet(SP5K_CAPTURE_QV_LENGTH, cfg);

	// This may be changed in setup menu state. (this cost ~30msec, so we move behid osd)
	gStillCB.focusStatus = FOCUS_STATUS_VIEW;
	flash_idx = UI_FLASH_OFF;
}

/*-------------------------------------------------------------------------
 *  Function Name : appStillPvPowerOnInit
 *  Description :
 *------------------------------------------------------------------------*/
void
appStillPvPowerOnInit(void)
{
	DBG_PRINT("spv: appStillPowerOnInit\n");

	appStillRawCbSet();
	appStillYuvCbSet(FALSE);

	#if !SP5K_PVQV_DISABLE
	appStillQVCbSet(FALSE);
	#endif
#if !defined(PRJ_17DVR)&&!defined(PRJ_19DVR)
	sp5kStillCapCallbackSet( SP5K_CAPTURE_CALLBACK_EXIF_CREATE ,appExifEncodeCb);
#endif

	if(IS_CARD_EXIST && _StillCheckDiskReady()==FALSE)
	{
		appBtnDisable(BTN_ALL);
		#if !SP5K_MULTI_FAST_BOOT
		appOsdMessageBoxShow(TRUE,STRTK_WAITING_FOR_CARD);
		#endif
	}

	/*-- to register callback functions for MShutter */
	appLensMShutCallbackRegister();
}


/*-------------------------------------------------------------------------
 *  Function Name : appStillCapStatusSave
 *  Description :
 *------------------------------------------------------------------------*/
void appStillCapStatusSave(void)
{
	/*save current view and scene*/
	StillCapModeBak[0]=gStillCB.cap.CurViewMode;
	StillCapModeBak[1]=gStillCB.cap.CurScene;
}

/*-------------------------------------------------------------------------
 *  Function Name : appStillCapStatusRecover
 *  Description :
 *------------------------------------------------------------------------*/
void appStillCapStatusRecover(void)
{
	gStillCB.cap.CurViewMode=StillCapModeBak[0];
	gStillCB.cap.CurScene=StillCapModeBak[1];
}

/*-------------------------------------------------------------------------
 *  Function Name : appStillPvStateInit
 *  Description :
 *------------------------------------------------------------------------*/
void
appStillPvStateInit(
	UINT32 preState
)
{
	static UINT32 doFirstTime = 1;
	UINT32 mode;
	#if SP5K_CDFS_OPT
	pbUIParam_t *ppbUiParam = appPb_UiParamPtrGet();
	#endif

	/*HOST_PROF_LOG_PRINT(LEVEL_INFO, "spv: %s s", __FUNCTION__);*/
	appPowerSavingEnableSet(TRUE);

	/* Do one time after power on. */
	if(doFirstTime)
	{
		HOST_PROF_LOG_ADD(LEVEL_INFO, "spv: init poweron s");
		doFirstTime = 0;
		appBtnControl(BTN_ALL, ENABLE);
		appStillPvPowerOnInit();
		HOST_PROF_LOG_ADD(LEVEL_INFO, "spv: init poweron e");
	}
	else
	{
		/*usb flow control return pre state*/
		if(appPreviousStateGet()!=APP_STATE_USB_CREATE)
			appBtnEnable(BTN_ALL);
	}

	#if SP5K_CDFS_OPT
	appCdfsFileFolderSet(CDFS_FILE_FOLDER_JPG);
	appCdfsFolderInit(CDFS_FILE_FOLDER_JPG);
	ppbUiParam->selCdfs = CDFS_FILE_FOLDER_JPG;
	#endif

	sp5kSensorModeCfgSet(SP5K_MODE_STILL_PREVIEW, DEFAULT_PREVIEW_MODE);
#if defined(PRJ_PDVR)
sp5kSensorCfgSet(SP5K_SENSOR_CFG_FRAMERATE_MAX,DEFAULT_PREVIEW_MODE,27500);
#elif defined(PRJ_17DVR)||defined(PRJ_19DVR)
sp5kSensorCfgSet(SP5K_SENSOR_CFG_FRAMERATE_MAX,DEFAULT_PREVIEW_MODE,28000);
#else
    sp5kSensorCfgSet(SP5K_SENSOR_CFG_FRAMERATE_MAX,DEFAULT_PREVIEW_MODE, 30*1000);
#endif
	/* Do one time if you come from other state group or view re-entry */
	switch (preState)
	{
	case APP_STATE_BURST_CAPTURE:
		sp5kStillCapCfgSet(SP5K_CAPTURE_BURST_FAST_SENSOR_TRIG, 0);
        appViewOsd_AllClean();

	case APP_STATE_STILL_CAPTURE:
		#if S2STEST //test for s2s
		if(S2S)
		{
			sp5kHostMsgSend(APP_KEY_PRESS_S1,0);
			sp5kHostMsgSend(APP_KEY_PRESS_S2,0);
			S2S = 0;
			break;
		}
		else if(S2S==0)
		{
			break;
		}
		#else
		if(appPeriodicalCapturePowerOffMsgSend() == FAIL){
			appPeriodicalCaptureTimerSet();
		}
		#endif

	case APP_STATE_STILL_BUSY:
	case APP_STATE_STILL_AAA:
		appViewOsdShow(0,0,gStillCB.osd);
		break;

    case APP_STATE_MENU:
        HOST_PROF_LOG_ADD(LEVEL_DEBUG, "spv: pre-state is APP_STATE_STILL menu");
		sp5kModeGet(&mode);
		if(mode!=SP5K_MODE_STILL_PREVIEW)
		{
			appModeSet(SP5K_MODE_STILL_PREVIEW);
		}
		appViewOsd_AllClean();

		appStillSettingInit(VIEW_SET_DISK);
		appViewOsdShow(0,0,gStillCB.osd);
		appPeriodicalCaptureTimerSet();
		HOST_PROF_LOG_ADD(LEVEL_DEBUG, "spv: pre-state is APP_STATE_STILLSETUP_MAINMENU_2");
		break;

	case APP_STATE_POWER_ON:
		StillCapModeBak[0]=pUiSetting->capSetting.CurViewMode;
		StillCapModeBak[1]=pUiSetting->capSetting.CurScene;

	default:
		/* flow&osd control */
		gStillCB.s1press = 0;
		gStillCB.s2press = 0;
		gStillCB.s1AaaTicket=0;
		gStillCB.s2SnapTicket=0;
		gStillCB.tvAvSel = PASM_MMODE_SEL_TV;
		gStillCB.isLensReady = FALSE;
		gStillCB.isXDenoiseOn = 0;
		gStillCB.cap.CurViewMode = PASM_PROGRAM;

		if(preState != APP_STATE_POWER_ON && !IS_VIEW_GROUP(preState))
			appUserSettingSave(pUiSetting);

			#if 0//EZ_MENU  /*mark for using up more time when enter menu*/
			appUIParaSave();
        	#endif


		#if 0
        if(pViewParam->videoSize == UI_VIDEO_SIZE_4K2K||pViewParam->videoSize == UI_VIDEO_SIZE_27K){
            sp5kModeSet(SP5K_MODE_STANDBY);
            sp5kModeWait(SP5K_MODE_STANDBY);
            appModeCfgPvRawSet(SP5K_MODE_PV_RAW_DO_DISABLE);
            sp5kSystemCfgSet(SP5K_CDSP_2NDPATH_FACTOR_CFG, 0x11);   /* default */
        }
	#endif

		if(IS_VIEW_MODE_CHANGE)
			gStillCB.osd = OSD_VIEW_SIMPLE;

        /*param1: 0: default(spv rgb, vpv yuv), 1:rgb scaling, 2: yuv scaling */
        sp5kModeCfgSet( SP5K_MODE_CFG_PV_CDSP_SCALING_OPTION, 2);
        /**Relative to appVideoSizeSet() for 4K2K/2.7K setting */
            appReset4k2kSetting();
		appStill_SetPasmSceneParam(gStillCB.cap.CurViewMode, gStillCB.cap.CurScene);

		if(appPreviousStateGet() != APP_STATE_POWER_ON){
			/* If host flow is PowerOn¡÷Video¡÷Video view, modeSet will 0x21¡÷0x88¡÷0x21, user will see the black screen immediately and
				disappear, so call the API wiil avoid this phenomenon */
			HOST_PROF_LOG_ADD(LEVEL_INFO, "spv init default top: appStillSettingInit s");
            appStillSettingInit(VIEW_SET_STATE_START);  /*HERE HAS APP_MODE_SET !!!!!!*/
		}

		/* The size has to be set early than sp5k mode set */
		appStill_SetResolution(pViewParam->stillSize);

		appPreflashInit();

		/* check disk */
		gStillCB.isDiskReady = 0;
		gStillCB.err = VIEW_ERR_NO_ERROR;

#if SP5K_DISK_SPI
 	    if(IS_CARD_EXIST)
#endif
		{
			if(_StillCheckDiskReady()==FALSE)
				sp5kHostMsgSend(APP_UI_MSG_VIEW_WAIT_DISK, 0);
			else
			{
				sp5kHostMsgSend(APP_UI_MSG_VIEW_WAIT_DISK, 0);
				HOST_PROF_LOG_ADD(LEVEL_INFO, "spv: still pv disk ready");
			}
		}
		if(gStillCB.err == VIEW_ERR_MEMORY_FULL)
		{
			sp5kHostMsgSend(APP_UI_MSG_VIEW_MEMORY_FULL, 0);
		}

		appStillCheckReady(CAN_I_START_SNAP);

		if(gStillCB.err > VIEW_ERR_NO_ERROR && gStillCB.osd == OSD_VIEW_VERBOSE)
		{
			gStillCB.osd = OSD_VIEW_SIMPLE;
		}

		/* osd control */
		if(preState == APP_STATE_POWER_ON)
		{
			profLogAdd(0, "spv: power pv osd s");
			#if SP5K_MULTI_FAST_BOOT
			/*don't show osd improve performance*/
			#else
			appViewOsdShow(0,1,gStillCB.osd);
			#endif
			profLogAdd(0, "spv: power pv osd e");
		}
		else
			appViewOsdShow(1,1,gStillCB.osd);

		if(appPreviousStateGet() == APP_STATE_POWER_ON){
			HOST_PROF_LOG_ADD(LEVEL_INFO, "spv init default 2nd: appStillSettingInit s");
			appStillSettingInit(VIEW_SET_STATE_START);
		}
		else{
			/* check preview mode */
			sp5kModeGet(&mode);
			if(mode!= SP5K_MODE_STILL_PREVIEW)
			{
				appModeSet(SP5K_MODE_STILL_PREVIEW);
			}
		}

		if(preState == APP_STATE_POWER_ON)
		{
			#if SP5K_MULTI_FAST_BOOT
			/*make sure sensor img ready*/
			sp5kDispAttrSet(sp5kDispActChnlGet(0), SP5K_DISP_IMG_ACTIVE, 1);
			#endif
			appOsdLib_PFLayerShapeClear(APP_OSD_SHAPE_RECT, 0, 0, LCD_SIZE_X, LCD_SIZE_Y, 0, 0);
		}

		profLogAdd(0, "spv: pip clear s");
		appOsdLib_PIPLayerShapeClear(APP_OSD_SHAPE_RECT, 0, 0, LCD_SIZE_X, LCD_SIZE_Y, 0, 0);
		APP_OSD_REFRESH_ON;
		profLogAdd(0, "spv: pip clear e");

		break;
	}
	#if SP5K_MOTION_DETECT
	if(appUiParaGet()->MotionDetect == 1)
	{
		appMotionDetectSet(APP_MD_EN, 1);
	}
	#endif

	/* To reset the first state of flow */
	if(pViewParam->videoStamp){
		HOST_PROF_LOG_ADD(LEVEL_DEBUG, "spv: video rec stamp s");
		sp5kGfxAttrSet(SP5K_GFX_RGB565_PIC_SCALE_FACTOR, 1000, 1000, 0, 0);
		HOST_PROF_LOG_ADD(LEVEL_DEBUG, "spv: video rec stamp e");
	}

	/*HOST_PROF_LOG_ADD(LEVEL_INFO, "spv: set driver flow s");*/
	/*appStillFlow_SetDriveFlow(pViewParam->stillDriverMode, gStillCB.cap.CurViewMode, gStillCB.cap.CurScene);*/

	#if SP5K_DISK_SPI
	if(!IS_CARD_EXIST)
	{
		appErrMsgWithSound(ERR_MSG_NO_MEMORY_CARD, ERR_MSG_WITH_WARNING_SOUND, ERR_MSG_SHOW_FOREVER);
		appBtnDisable(BTN_ALL);
	}
	#endif

	#if SP5K_PVQV_DISABLE
	sp5kGfxPageClear(SP5K_GFX_PAGE_PF_0,0);
	#endif
	/*HOST_PROF_LOG_PRINT(LEVEL_INFO, "spv: %s e", __FUNCTION__);*/
}

/*-------------------------------------------------------------------------
 *  Function Name : appStillPvStateOnMsg
 *  Description :
 *------------------------------------------------------------------------*/
void
appStillPvStateOnMsg(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 tmpU32;

	switch ( msg)
	{
	/* storage check */
	case APP_UI_MSG_VIEW_WAIT_DISK:
		HOST_PROF_LOG_ADD(LEVEL_WARN, "spv: APP_UI_MSG_VIEW_WAIT_DISK");

		if(_StillCheckDiskReady()==FALSE)
		{
			APP_SLEEP_MS(200);
			sp5kHostMsgSend(APP_UI_MSG_VIEW_WAIT_DISK, 0);
			break;
		}
		else
		{
			appBtnEnable(BTN_ALL);
			HOST_PROF_LOG_ADD(LEVEL_INFO, "spv: disk ready");
		}

		appPeriodicalCaptureS2Check();

		appStillCheckReady(CAN_I_START_SNAP);
		appViewOsdShow(0,0,gStillCB.osd);
		break;

	case APP_UI_MSG_VIEW_DISKSIZE_READY:
		appStillCheckReady(CAN_I_START_SNAP);
		appViewOsdShow(0,0,gStillCB.osd);
		break;

	case SP5K_MSG_AE_READY:
            appAeReady();

	#if SP5K_MOTION_DETECT
		#if EZ_MENU
		if(appUiParaGet()->MotionDetect == 1)
		#endif
		{
	        appMotionDetectHdr(NULL);
		}
	#endif
		break;
	case SP5K_MSG_AWB_READY:
		break;

	case SP5K_MSG_AF_READY:
		HOST_PROF_LOG_ADD(LEVEL_DEBUG, "spv: AF_READY");
		break;

	/* DZOOM Position bar update */
	case APP_UI_MSG_DZOOM_POS_UPDATE:
		tmpU32 = 0xFFUL & param;
		APP_OSD_REFRESH_OFF;
		appViewOsd_ZoomFrameDraw(TRUE);
		appViewOsd_DigZoomPosDraw((UINT8)tmpU32);
		APP_OSD_REFRESH_ON;
		break;

	/* TELE/WIDE Self Repeat */
	case APP_UI_MSG_LENS_DZOOM_TELE:
		appStillDZoomTele();
		break;

	case APP_UI_MSG_LENS_DZOOM_WIDE:
		appStillDZoomWide();
		break;

	case APP_UI_MSG_VIEW_MEMORY_FULL:
		appStateChange(APP_STATE_STILL_BUSY, APP_STATE_MSG_INIT);
		break;
	case SP5K_MSG_TIMER :
		appPeriodicalCaptureTimerProc(param);
		break;
	case SP5K_MSG_MODE_READY:
		break;
#if SP5K_MOTION_DETECT
	case APP_UI_MSG_MD_START:
		sp5kHostMsgSend(APP_KEY_PRESS_S1 , 0);
		sp5kHostMsgSend(APP_KEY_PRESS_S2 , 0);
		break;
	case APP_UI_MSG_MD_STOP:
		break;
#endif
	default:
		DBG_PRINT(".");
		break;
	}
}

/*-------------------------------------------------------------------------
 *  Function Name : appStillPvStateOnKey
 *  Description :
 *------------------------------------------------------------------------*/
void
appStillPvStateOnKey(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 next_state,next_param;

	switch (msg)
	{
	case APP_KEY_PRESS_S2:
		/*HOST_PROF_LOG_ADD(LEVEL_INFO, "spv: S2 press s");*/
		gStillCB.s2SnapTicket=1;
	case APP_KEY_PRESS_S1:
		/*HOST_PROF_LOG_ADD(LEVEL_INFO, "spv: S1 press s");*/
		if(appStillCheckReady(CAN_I_START_SNAP)==STILL_STATE_YES)
		{
			appBtnDisable(BTN_PB|BTN_MODE);
			HOST_PROF_LOG_ADD(LEVEL_DEBUG, "spv: s1 disable button e");

            #if TEST_STATE_CHANG_MSG
    		appStillFlow_SetDriveFlow(pViewParam->stillDriverMode, gStillCB.cap.CurViewMode, gStillCB.cap.CurScene);
            #endif

			appStillFlow_GetNextState(&next_state, &next_param);
			appStateChange(next_state,next_param);
		}
		else
		{
			HOST_PROF_LOG_ADD(LEVEL_ERROR, "spv: S1 press Error");
			gStillCB.s2SnapTicket = 0;
			if(gStillCB.err != VIEW_ERR_NO_ERROR)
			{
				appViewOsd_WarmingMsgDraw(1, gStillCB.err, TRUE);
			}
		}
		HOST_PROF_LOG_ADD(LEVEL_DEBUG, "spv: S1 press e");

		#if S2STEST
		if(S2S)
		{
			sp5kHostMsgSend(APP_KEY_PRESS_S2,0);
		}
		#endif
		break;

	case APP_KEY_PRESS_MENU:
		GREEN_LED_ON;
#if 1
		appBtnDisable(BTN_MODE);
		appPeriodicalCaptureTimerCheck(pUiSetting->videoFormat);
        appStateChange(APP_STATE_MENU,STATE_PARAM_NORMAL_INIT);
#else
        appTestStateChange();
#endif
		break;

	case APP_KEY_PRESS_DEL:
		break;

	case APP_KEY_PRESS_SET:
		gStillCB.osd++;
		if (gStillCB.osd >= OSD_VIEW_CLEAN_ALL)
		{
			gStillCB.osd = OSD_VIEW_SIMPLE;
		}
		appViewOsdShow(0,0,gStillCB.osd);
		break;

	case APP_KEY_PRESS_MODE:
		appStillCapStatusSave();
		appPeriodicalCaptureTimerCheck(pUiSetting->videoFormat);
	/*Set sensor to enter sensor powerSave Mode */
		appsensorAutoPwrSaveSet(0);
		fileTypeSelect = 1;/*ez_menu for pb filetype */
		sp5kModeSet(SP5K_MODE_STANDBY);
		sp5kModeWait(SP5K_MODE_STANDBY);
        appStateChange(APP_STATE_MENU,STATE_PARAM_NORMAL_INIT); /*pb mode*/
		break;

	case APP_KEY_PRESS_PB:
		appBeep(TRUE, FALSE);
		appAAAOff();
		appStillCapStatusSave();
		appPeriodicalCaptureTimerCheck(pUiSetting->videoFormat);
		fileTypeSelect = 1;/*ez_menu for pb filetype */
		sp5kModeSet(SP5K_MODE_STANDBY);
		sp5kModeWait(SP5K_MODE_STANDBY);
        appStateChange(APP_STATE_MENU,STATE_PARAM_NORMAL_INIT); /*pb mode*/
		break;

	case APP_KEY_PRESS_UP:
		break;
	case APP_KEY_PRESS_DOWN:
		break;
	case APP_KEY_PRESS_LEFT:

		#ifdef HW_RDK_6330
		CLEAR_OSD_SRC;
		appStillCapStatusSave();
		appStateChange(APP_STATE_VIDEO_PREVIEW,STATE_PARAM_NORMAL_INIT);
		#else

		#if !SP5K_PVQV_DISABLE
		if(gStillCB.osd <= OSD_VIEW_VERBOSE)
			appViewOsd_PvQV_En();
		#endif
		#endif

		break;
	case APP_KEY_PRESS_RIGHT:
		appStillFlow_SetDriveFlow(pViewParam->stillDriverMode, gStillCB.cap.CurViewMode, gStillCB.cap.CurScene);
		break;

	case APP_KEY_PRESS_TELE:
		if(appStrobeQueryChargeEnable() && !appStrobeQueryReady())
		{
			break;
		}
		break;

	case APP_KEY_PRESS_WIDE:
		if(appStrobeQueryChargeEnable() && !appStrobeQueryReady())
		{
			break;
		}
		break;

	case APP_KEY_RELEASE_WIDE:
	case APP_KEY_RELEASE_TELE:
		break;

	#if 0
	case APP_KEY_PRESS_FLASH:
		if((pViewParam->stillDriverMode == UI_DRIVE_BURST_H) || (pViewParam->stillDriverMode == UI_DRIVE_BURST_L))
		{
			break;
		}
		temp = pViewParam->flash;
		appStill_ChangeFlash();
		if(gStillCB.osd == OSD_VIEW_NONE ||gStillCB.osd == OSD_VIEW_GRID)
		{
			gStillCB.osd = OSD_VIEW_SIMPLE;
			appViewOsdShow(0,0,gStillCB.osd);
		}
		else
		{
			appViewOsd_FlashModeDraw(TRUE, gStillCB.flashMode);
		}
		if(temp == pViewParam->flash)
		{
			return;
		}
		appStill_SetFlash(gStillCB.flashMode);
		break;
	#endif

	default:
		break;
	}

	switch (msg)
	{
	case APP_KEY_PRESS_DEL:
	case APP_KEY_PRESS_MODE:
	case APP_KEY_PRESS_MENU:
	case APP_KEY_PRESS_SET:
	case APP_KEY_PRESS_LEFT:
	case APP_KEY_PRESS_RIGHT:
    case APP_KEY_PRESS_DOWN:
		appBeep(TRUE, FALSE);
	break;
	default:
	break;
	}
}

/*-------------------------------------------------------------------------
 *  Function Name : CurrAFRDYParamGet
 *  Description :
 *------------------------------------------------------------------------*/
/*
The following variable and function are used for AF testing and tuning.
Brian.
*/
static UINT32 CurrAFRDYParam = 0;
UINT32 CurrAFRDYParamGet(void)
{
	return CurrAFRDYParam;
}

/*-------------------------------------------------------------------------
 *  Function Name : appStillPvState
 *  Description :
 *------------------------------------------------------------------------*/
void
appStillPvState(
	UINT32 msg,
	UINT32 param
)
{
	//For AF testing and tuning.
	if(msg == SP5K_MSG_AF_READY)
	{
		CurrAFRDYParam = param;
	}

	switch (msg)
	{
	case APP_STATE_MSG_INIT:
		RPrintf("appStillPvState\n");
		appStillPvStateInit(appPreviousStateGet());

		#if API_TEST_INIT
		sp5kHostMsgSend(APP_UI_MSG_EMULATION, 0);
		#endif
		break;

	case APP_STATE_MSG_CLOSE:
		appStateCloseDone();
		break;

	default:
		if (IS_APP_KEY_MSG(msg))
		{
			appStillPvStateOnKey(msg,param);
		}
		else
		{
			appStillPvStateOnMsg(msg,param);
		}
		break;
	}
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewInit
 *  Description : This function can make sure you are in sitll/vieo preview
				  status.
 *------------------------------------------------------------------------*/
void appViewInit(void)
{
	UINT32 mode;

	sp5kModeGet(&mode);
	if(IS_PASM_PMODE && mode!=SP5K_MODE_STILL_PREVIEW)
	{
		appStill_SetResolution(pViewParam->stillSize);
		appModeSet(SP5K_MODE_STILL_PREVIEW);
		appStillSettingInit(VIEW_SET_STATE_START);
	}
	if(IS_VIDEO_MODE && mode!=SP5K_MODE_VIDEO_PREVIEW)
	{
		appVideoSizeSet();
	}
}

/*-------------------------------------------------------------------------
 *  Function Name : appAAAOff
 *  Description :
 *------------------------------------------------------------------------*/
void
appAAAOff(
	void
)
{
#if 0
	/* Mantis 46692, 46771, 46755 */
	sp5kAeModeSet(SP5K_AE_MODE_OFF);
	sp5kAwbModeSet(SP5K_AWB_MODE_OFF);
#endif
}

/*-------------------------------------------------------------------------
 *  Function Name : appBtnEnableDelay
 *  Description :
 *------------------------------------------------------------------------*/
void
appBtnEnableDelay(
	void
)
{
	/* Mantis 45321 */
	BPrintf("Button Enable");
	appBtnControl(BTN_ALL, ENABLE);
}

void
appPeriodicalCaptureTimerCheck(
	UINT8 videoFormat
)
{
	/* Call by out from still view state */

	switch(videoFormat)
	{
		case VIDEO_FORMAT_MJPG:
		case VIDEO_FORMAT_H264:
			if(PeriodicalCapTimer != TIMER_NULL){
				appTimerClear(&PeriodicalCapTimer);
			}
			break;
		default:
			DBG_PRINT("[%s]Wrong vidoe format[%d]\n", __FUNCTION__, videoFormat);
			break;

	}

}

void
appPeriodicalCaptureTimerSet(
	void
)
{
	/* Call by prestate is still setup main menu or still capture mode to set timer for non-reboot periodical capture */

	appPeriodicalCaptureTimeSet();

	switch(pUiSetting->periodicalCap.periodicalSet)
	{
		case PERIODICAL_CAPTURE_OFF:
			break;
		case PERIODICAL_CAPTURE_5SEC:
		case PERIODICAL_CAPTURE_10SEC:
			if(PeriodicalCapTimer == TIMER_NULL){
				PeriodicalCapTimer= appTimerSet(PeriodicalCaptureTime*1000, "PeriodicalCap Timer");
			}else{
				DBG_PRINT("spv: %s PeriodicalCapTimer exist", __FUNCTION__);
			}
			break;
		case PERIODICAL_CAPTURE_30SEC:
			break;
		case PERIODICAL_CAPTURE_60SEC:
			break;
		default:
			DBG_PRINT("[%s]Wrong periodical capture index[%d]\n", __FUNCTION__, pUiSetting->periodicalCap.periodicalSet);
			break;
	}

}

BOOL
appPeriodicalCapturePowerOffMsgSend(
	void
)
{
	/* Call by prestate is still capture to send power message */

	if(pUiSetting->periodicalCap.periodicalSet > PERIODICAL_CAPTURE_10SEC)
	{
		{
			sp5kHostMsgSend(SP5K_MSG_POWER_OFF,0);
			DBG_PRINT("spv: Enter periodical capture--->to power off");
			return SUCCESS;
		}
	}else{
		return FAIL;
	}
}

void
appPeriodicalCaptureS2Check(
	void
)
{
	/* Call by reboot wait disk ready then trigger capture in still view */

	if((pUiSetting->periodicalCap.periodicalSet > PERIODICAL_CAPTURE_10SEC) && (appPreviousStateGet() == APP_STATE_POWER_ON))
	{
		if(IS_KEY_OK_PRESSED){
			pUiSetting->periodicalCap.periodicalSet = PERIODICAL_CAPTURE_OFF;
		}else{
			{
				sp5kHostMsgSend(APP_KEY_PRESS_S2, 0);
			}
		}
	}
}
void
appPeriodicalCaptureTimerProc(
	UINT32 param
)
{
	/* Call by getting SP5K_TIMER_MSG to process in still view */

	if((pUiSetting->periodicalCap.periodicalSet < PERIODICAL_CAPTURE_30SEC) && (param == PeriodicalCapTimer)){
		sp5kHostMsgSend(APP_KEY_PRESS_S2,0);
		appBtnDisable(BTN_ALL);
	}

}

/*-------------------------------------------------------------------------
 *  File Name : appView_ItemDecrease
 *------------------------------------------------------------------------*/
UINT32
appView_ItemDecrease(
	UINT32  total,UINT32 curIndex
)
{

   	UINT32 nextIndex;
	nextIndex=(curIndex+1)%total;

	return nextIndex;
}

/*-------------------------------------------------------------------------
 *  File Name : appView_ItemIncrease
 *------------------------------------------------------------------------*/
UINT32
appView_ItemIncrease(
	UINT32  total,UINT32 curIndex
)
{
	UINT32 nextIndex;
	nextIndex=(curIndex+total-1)%total;

	return nextIndex;
}

