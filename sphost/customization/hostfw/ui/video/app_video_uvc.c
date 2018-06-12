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
 *	File name    : App_video_uvc.c
 *	Function     : about video uvc		
 *	First editor :   
 *	First date   :   
 *	History      :
 *               : 2018.05.31	Edward.Lu  "Add description and modify code"
 **************************************************************************/

/*

 The suggestion video fllow.
 1. set video size.
 2. sp5k mode set to "video_privew." (need to allocate buffer)
 3. sp5k mode set to "video_record" as recording.
 4. set media abort to stop. (to stop and add file name to DCF)
 5. set sp5k mode set to "video_preview."

 */

#include "sp5k_global_api.h"
#include "sp5k_ae_api.h"
#include "sp5k_disp_api.h"
#include "sp5k_disk_api.h"
#include "sp5k_rsvblk_api.h"
#include "sp5k_gfx_api.h"
#include "sp5k_dcf_api.h"
#include "sp5k_media_api.h"
#include "sp5k_sensor_api.h"
#include "sp5k_utility_api.h"
#include "sp5k_usbhost_api.h"

#include "gpio_custom.h"
#include "app_com_def.h"
#include "app_com_api.h"
#include "app_still.h"
#include "app_video.h"
#include "app_user_setting.h"
#include "app_view_param.h"
#include "app_view_osd.h"
#include "app_video_uvc.h"
#include "app_osd_api.h"
#include "app_error_msg.h"
#include "customization/app_init.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define HOST_DBG 0

#define ROUND_DOWN_TO_DIVISIBLE(num,div) ( (UINT32)(num) & -(UINT32)(div) )
#define ROUND_UP_TO_DIVISIBLE(num,div) ROUND_DOWN_TO_DIVISIBLE( (UINT32)(num) + (div) - 1, div )
#define DUAL_PIP_RATION         30

#define UVC_CAP_WIDTH      1920
#define UVC_CAP_HEIGHT     1080
#define UVC_EVT_TRIG_TMR   5000

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static UINT8 dual_pip_mode 	= DUALSEN_SINGLE_MAIN;
static UINT8 reverse_gear 	= CAR_REVERSE_DEF_CTRL;
static UINT8 uvc_flip 		= FLIP_DIS;
static UINT8 event_flag 	= EVENT_TRIG_DIS;
static UINT32 pwr_on_rec 	= PWR_ON_REC_DIS;

static UINT8 *puvc_yuv_sbuf;
static UINT8 *puvc_yuv_dbuf;
static UINT8 uvc_cap_proc = 0, uvc_save_done = 0;
static UINT32 uvc_w,uvc_h;

static frameBufInfo_t pvmain;
static frameBufInfo_t pv2nd;
static sp5kGfxObj_t imgSrc, imgDst;
static sp5kGfxMapProjectParam_t  proPrm;
static UINT32 ImeCbInited = 0;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/*--------------------------------------------------------------------------*
* Function name    : appUvcCamEvtMsgCb
* Function         : Uvc event message callback
* Return value	   : result (UINT32 SUCCESS/FAIL)
* Parameter1	   : USB system event (USBSysEvt)
* Parameter2	   : parameter (unsigned long)
* Parameter3	   : user data (unsigned long)
* History          :
*                  : 2018.05.31  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32 appUvcCamEvtMsgCb(USBSysEvt evt, unsigned long param, unsigned long udata)
{
	switch(evt)
    {
    	case USB_SYSEVT_DEVICE_PLG_IN:
    		HOST_PROF_LOG_ADD(LEVEL_DEBUG, "uvc: msg device plug in");
    		break;

    	case USB_SYSEVT_MSDC_PLG_IN:
    		HOST_PROF_LOG_ADD(LEVEL_INFO, "uvc: msg msdc plug in");
    		break;

    	case USB_SYSEVT_UVC_PLG_IN:
            appUvcHostCfgSet(UVC_HOST_CFG_PLUG_IN, UVC_PLUG_IN);
    		HOST_PROF_LOG_ADD(LEVEL_INFO, "uvc: msg uvc plug in");
    		break;

    	case USB_SYSEVT_MSDC_PLG_OUT:
            HOST_PROF_LOG_ADD(LEVEL_INFO, "uvc: msg msdc plug out");
    		break;

    	case USB_SYSEVT_SET_ADDR_ERR:
    	case USB_SYSEVT_GET_DSCR_ERR:
        case USB_SYSEVT_UVC_PROBE_ERR:
            if(evt == USB_SYSEVT_SET_ADDR_ERR)
                HOST_PROF_LOG_ADD(LEVEL_ERROR, "uvc: msg set address error");
            if(evt == USB_SYSEVT_GET_DSCR_ERR)
                HOST_PROF_LOG_ADD(LEVEL_ERROR, "uvc: msg get descriptor error");
            if(evt == USB_SYSEVT_UVC_PROBE_ERR)
                HOST_PROF_LOG_ADD(LEVEL_ERROR, "uvc: msg probe error");

            appUvcHostCfgSet(UVC_HOST_CFG_PLUG_FAIL, UVC_PLUG_FAIL);
            break;

    	case USB_SYSEVT_UVC_PLG_OUT:
            appUvcHostCfgSet(UVC_HOST_CFG_PLUG_OUT, UVC_PLUG_OUT);
            HOST_PROF_LOG_ADD(LEVEL_INFO, "uvc: msg uvc plug out");
    		break;

    	case USB_SYSEVT_DEVICE_PLG_OUT:
            HOST_PROF_LOG_ADD(LEVEL_DEBUG, "uvc: msg device plug out");
    		break;

    	case USB_SYSEVT_UNKNOWN_PLG_IN:
            HOST_PROF_LOG_ADD(LEVEL_WARN, "uvc: msg unknown device plug in");
    		break;

    	case USB_SYSEVT_UNKNOWN_PLG_OUT:
            HOST_PROF_LOG_ADD(LEVEL_WARN, "uvc: msg unknown device plug out");
    		break;

    	default:
    		printf("uvc: msg Unknown State, usbhost sys evt: %d\n", evt);
			return FAIL;
	}

    return SUCCESS;
}

/*--------------------------------------------------------------------------*
* Function name    : appUvcPvUrCb
* Function         : uvc preview urgent callback, copy pframe(yuv) to puvc_yuv_sbuf
* Return value	   : result (UINT32 SUCCESS/FAIL)
* Parameter1	   : stream (UINT32 no use for this case)
* Parameter2	   : frame index (UINT32 no use for this case)
* Parameter3	   : YUV frame point (frameBufInfo_t* YUV frame address)
* Parameter4	   : duplicated buffer address (UINT8* no use for this case)
* History          :
*                  : 2018.05.31  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

static UINT32 appUvcPvUrCb(
	UINT32 streamId,        /* nth stream, from 0 */
	UINT32 fid,             /* frame id */
	frameBufInfo_t *pframe, /* buffer information */
	UINT8 *pbufdup          /* duplicated buffer address */
)
{
    if(pframe->pbuf == NULL)
        return FAIL;

    sp5kGfxObj_t srcObj =
    {
        .pbuf = pframe->pbuf,
        .bufW = pframe->width,
        .bufH = pframe->height,
        .roiX = pframe->roiX,
        .roiY = pframe->roiY,
        .fmt = SP5K_GFX_FMT_YUV422,
        .roiW = pframe->roiW,
        .roiH = pframe->roiH
    };

    sp5kGfxObj_t dstObj =
    {
        .pbuf = puvc_yuv_sbuf,
        .bufW = pframe->width,
        .bufH = pframe->height,
        .roiX = pframe->roiX,
        .roiY = pframe->roiY,
        .fmt = SP5K_GFX_FMT_YUV422,
        .roiW = pframe->roiW,
        .roiH = pframe->roiH
    };

    if(uvc_cap_proc)
    {
        uvc_w = pframe->width;
        uvc_h = pframe->height;
		uvc_cap_proc = 0;
		return sp5kGfxObjectCopy(&srcObj, &dstObj);
    }
    return SUCCESS;
}

/*--------------------------------------------------------------------------*
* Function name    : appUvcYuvScale
* Function         : do uvc yuv scale
* Return value	   : result (UINT32 SUCCESS/FAIL)
* Parameter1	   : psrcBuf (UINT8 * source yuv frame address)
* Parameter2	   : srcw (UINT32 source width)
* Parameter3	   : srch (UINT32 source height)
* Parameter4	   : pdstBuf (UINT8 * destination yuv frame address)
* Parameter5	   : destw (UINT32 destination width)
* Parameter6	   : desth (UINT32 destination height)
* History          :
*                  : 2018.05.31  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32 appUvcYuvScale(UINT8 *psrcBuf, UINT32 srcw, UINT32 srch, UINT8 *pdstBuf, UINT32 destw, UINT32 desth)
{
	sp5kGfxObj_t srcObj, dstObj;
    UINT32 outw, outh;

    if(!psrcBuf || !pdstBuf)
    {
        HOST_PROF_LOG_PRINT(LEVEL_ERROR, "uvc: yuv scale fail");
        return FAIL;
    }

    outw = ROUND_UP_TO_DIVISIBLE(destw, ALIGN_16); // alignment 16
    outh = ROUND_UP_TO_DIVISIBLE(desth, ALIGN_16); // alignment 16

    srcObj.pbuf = psrcBuf;
	srcObj.bufW = srcObj.roiW = srcw;
	srcObj.bufH = srcObj.roiH = srch;

    dstObj.pbuf = pdstBuf;
	dstObj.bufW = dstObj.roiW = outw;
	dstObj.bufH = dstObj.roiH = outh;

    srcObj.paplaneBuf = dstObj.paplaneBuf = NULL;
	srcObj.roiX = srcObj.roiY = dstObj.roiX = dstObj.roiY = 0;
    srcObj.fmt = dstObj.fmt = SP5K_GFX_FMT_YUV422; // set yuv422 format

	sp5kGfxObjectScale(&srcObj, &dstObj); // do scale
    return SUCCESS;
}

/*--------------------------------------------------------------------------*
* Function name    : appUvcCapModeSet
* Function         : set uvc mode
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.31  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appUvcCapModeSet(void)
{
    UINT32 mode, path;
    static sp5kPrevUrgentCallback_t ucbs[2];
    sp5kPrevYuvRoi_t roi;

    sp5kModeGet(&mode);
    if(mode!=SP5K_MODE_VIDEO_PREVIEW)
    {
        HOST_PROF_LOG_ADD(LEVEL_WARN, "uvc: capture fail because not video view mode");
        return;
    }

    if(uvc_save_done)
    {
        HOST_PROF_LOG_ADD(LEVEL_INFO, "uvc: capture again");
        return;
    }
	// set JPG folder
    appCdfsFileFolderSet(CDFS_FILE_FOLDER_JPG);
    appCdfsFolderInit(CDFS_FILE_FOLDER_JPG);

    for(path=0; path<2; path++)
    {

        if(!appUvcHostCfgGet(UVC_HOST_CFG_PLUG_COMPLETE) && path==1)
    {
            HOST_PROF_LOG_ADD(LEVEL_INFO, "uvc: not capture uvc camera image");
            break;
    }
	UINT32 yuvw;
	UINT32 yuvh;
    yuvw = ROUND_UP_TO_DIVISIBLE(UVC_CAP_WIDTH, ALIGN_16); // alignment 16
    yuvh = ROUND_UP_TO_DIVISIBLE(UVC_CAP_HEIGHT, ALIGN_16); // alignment 16
	puvc_yuv_sbuf = sp5kYuvBufferAlloc(yuvw, yuvh);
    puvc_yuv_dbuf = sp5kYuvBufferAlloc(yuvw, yuvh);

	if(puvc_yuv_sbuf && puvc_yuv_dbuf)
	{
		UINT32 dupstream0=0;
        uvc_cap_proc = uvc_save_done = 1|(1<<path);

		if(path==1)
		{
            ucbs[0].fp = NULL;
            ucbs[1].fp = appUvcPvUrCb;
            sp5kVideoUrgentCallbackSet(INTERVAL_FRAMES_COMES, dupstream0, ucbs, NUMBER_TWO_STREAMS);
		}
		else
		{
	        ucbs[0].fp = appUvcPvUrCb;
	        ucbs[1].fp = NULL;
	        sp5kVideoUrgentCallbackSet(INTERVAL_FRAMES_COMES, dupstream0, ucbs, NUMBER_ONE_STREAM);
		}

		while(uvc_cap_proc)
		    sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 20);

        HOST_PROF_LOG_PRINT(LEVEL_INFO, "uvc: wait for cap proc s %d %d %d",uvc_save_done ,uvc_w ,uvc_h);

        ucbs[0].fp = ucbs[1].fp = NULL;
        sp5kVideoUrgentCallbackSet(INTERVAL_FRAMES_COMES, dupstream0, ucbs, NUMBER_ONE_STREAM);
        sp5kVideoUrgentCallbackSet(INTERVAL_FRAMES_COMES, dupstream0, ucbs, NUMBER_TWO_STREAMS);
        memset(&roi, 0, sizeof(sp5kPrevYuvRoi_t));

        if((uvc_w!=ROUND_UP_TO_DIVISIBLE(UVC_CAP_WIDTH, ALIGN_16))
            && (uvc_h!=ROUND_UP_TO_DIVISIBLE(UVC_CAP_HEIGHT, ALIGN_16)))
        {
            appUvcYuvScale(puvc_yuv_sbuf, uvc_w, uvc_h,
                           puvc_yuv_dbuf, UVC_CAP_WIDTH, UVC_CAP_HEIGHT);

            roi.width   = yuvw;
            roi.height  = yuvh;
            roi.roiw    = UVC_CAP_WIDTH;
            roi.roih    = UVC_CAP_HEIGHT;
            sp5kPreviewJpegSave(puvc_yuv_dbuf,
                            &roi, UVC_CAP_WIDTH, UVC_CAP_HEIGHT);
        }
        else
        {
            roi.width   = uvc_w;
            roi.height  = uvc_h;
            roi.roiw    = UVC_CAP_WIDTH;
            roi.roih    = UVC_CAP_HEIGHT;
            sp5kPreviewJpegSave(puvc_yuv_sbuf,
                            &roi, UVC_CAP_WIDTH, UVC_CAP_HEIGHT);
        }

        if(puvc_yuv_sbuf)
        {
            sp5kYuvBufferFree(puvc_yuv_sbuf);
            puvc_yuv_sbuf = NULL;
        }

        if(puvc_yuv_dbuf)
        {
            sp5kYuvBufferFree(puvc_yuv_dbuf);
            puvc_yuv_dbuf = NULL;
        }

            uvc_save_done = uvc_cap_proc = 0;
        HOST_PROF_LOG_ADD(LEVEL_INFO, "uvc: cap save done");
	}
	else
	{
        HOST_PROF_LOG_ADD(LEVEL_WARN, "uvc: no buffer for capture");
	}
    }

    appErrMsgTimeISRWithSound(ERR_MSG_CAPTURE_DONE,ERR_MSG_ONLY, 1000/*ms*/);
  	appCdfsFileFolderSet(CDFS_FILE_FOLDER_VIDEO);
	appCdfsFolderInit(CDFS_FILE_FOLDER_VIDEO);
    uvc_save_done = uvc_cap_proc = 0;
}

/*--------------------------------------------------------------------------*
* Function name    : appUvcCapModeStsGet
* Function         : Get uvc mode status
* Return value	   : uvc_save_done (UINT8 0:save done, 1:no save done)
* Parameter1	   : void
* History          :
*                  : 2018.05.31  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT8 appUvcCapModeStsGet(void)
{
    return uvc_save_done;
}

/*--------------------------------------------------------------------------*
* Function name    : appUvcImeTrigCb
* Function         : uvc yuv trig, mapping the ROI region from the frame buffer 
                     of the source gfx object to the destination gfx object. 
* Return value	   : result (UINT32 SUCCESS/FAIL)
* Parameter1	   : stream index (UINT32)
* Parameter2	   : frame index (UINT32)
* Parameter3	   : YUV frame point (frameBufInfo_t* YUV frame address)
* Parameter4	   : duplicated buffer address (UINT8*)
* History          :
*                  : 2018.05.31  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32 appUvcImeTrigCb(
	UINT32 streamId,        /* nth stream, from 0 */
	UINT32 fid,             /* frame id */
	frameBufInfo_t *pframe, /* buffer information */
	UINT8 *pbufdup          /* duplicated buffer address */
)
{
	UINT32 srcRoiw = 0, srcRoih = 0, srcRoix = 0, srcRoiy = 0;
	UINT32 dstRoiw = 0, dstRoih = 0, dstRoix = 0, dstRoiy = 0;
	UINT32 offsetx = 0, offsety = 0;
	
	
    HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "uvc: cb %d %d %d %d",ImeCbInited ,dual_pip_mode ,reverse_gear, uvc_flip);

	#if !UVC_NO_WAIT_FLAG
    if(!appUvcHostCfgGet(UVC_HOST_CFG_PLUG_IN))
        return FAIL;
	#endif

#if PREFORMANCE_PROF
    profLogPrintf(PROFID, "uvc: imecb s");
#endif
#if TEST
    {   /*lester test performance*/
        UINT32 diskmeas, vfifo, afifo;
        sp5kDiskCfgGet(SP5K_DISK_BUSY_MEAS_CFG, &diskmeas);
        sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_REC_STATE_VIDEO_FIFO_USED, &vfifo);
        sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_REC_STATE_AUDIO_FIFO_USED, &afifo);
        profLogPrintf(0, "#L#FIFO#N#diskmeas=%d,fifo=(%d,%d)", diskmeas, vfifo, afifo);
    }
#endif
    if (ImeCbInited) {

        /*for skip frames*/
        if(ImeCbInited <= UVC_RING_DELAY)
        {
            ImeCbInited++;
            return FAIL;
        }
		UINT32 nstream=0, sensor_number=0;
        sp5kDispUrgentCallbackYuvGet(&pvmain, nstream, sensor_number);
		nstream=1;
        sp5kDispUrgentCallbackYuvGet(&pv2nd, nstream, sensor_number);
    }
    else {
        return FAIL;
    }

    if( !pvmain.pbuf || !pv2nd.pbuf || !pframe->pbuf  ) {
            printf("\n%u %u 0x%x PbFrm=%u %u x=%u y=%u w=%u h=%u\n",streamId,fid,(UINT32)pframe->pbuf,
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
	
    if(reverse_gear == CAR_REVERSE_EN)
    {
    	/* dst is in disp pbpv buf, sensor[0] */
 //   	dstRoiw = pframe->roiW;
//    	dstRoih = pframe->roiH;
//    	dstRoix = pframe->roiX;
//    	dstRoiy = pframe->roiY;

    	/* src is in 2nd buf, sensor[1] */
    	imgSrc.pbuf = pv2nd.pbuf;
    	imgSrc.bufW = pv2nd.width;
    	imgSrc.bufH = pv2nd.height;

    	srcRoiw = imgSrc.roiW = pv2nd.roiW;
    	srcRoih = imgSrc.roiH = pv2nd.roiH;
    	srcRoix = imgSrc.roiX = pv2nd.roiX;
    	srcRoiy = imgSrc.roiY = pv2nd.roiY;
        dstRoiw = pframe->roiW;
        dstRoih = pframe->roiH;
	    dstRoix = dstRoiy = 0;
    }
    else if (dual_pip_mode == DUALSEN_SINGLE_MAIN || dual_pip_mode == DUALSEN_SINGLE_SEC)
    {
        HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "uvc: cb single main return");
        return FAIL;
    }
    else if(dual_pip_mode == DUALSEN_PIP_MAIN)
    {
        dstRoiw = pframe->roiW;
        dstRoih = pframe->roiH;
        dstRoix = pframe->roiX;
        dstRoiy = pframe->roiY;

        /* src is in 2nd buf, sensor[1] */
        imgSrc.pbuf = pv2nd.pbuf;
        imgSrc.bufW = pv2nd.width;
        imgSrc.bufH = pv2nd.height;

        srcRoiw = imgSrc.roiW = pv2nd.roiW;
        srcRoih = imgSrc.roiH = pv2nd.roiH;
        srcRoix = imgSrc.roiX = pv2nd.roiX;
        srcRoiy = imgSrc.roiY = pv2nd.roiY;
        offsetx = dstRoiw;
        offsety = dstRoih;
    }
    else if(dual_pip_mode == DUALSEN_PIP_SEC)
    {
		/* dst is in disp pbpv buf,  sensor[1] */
		dstRoiw = pframe->roiW;
		dstRoih = pframe->roiH;
		dstRoix = pframe->roiX;
		dstRoiy = pframe->roiY;

		/* src is in main buf, sensor[0] */
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

    if(reverse_gear != CAR_REVERSE_EN)
    {
		UINT32 fac;
        if(dual_pip_mode == DUALSEN_PIP_MAIN)
        {
        	fac = pv2nd.roiW/pframe->roiW;
		dstRoiw = pv2nd.roiW/fac;
		dstRoih = pv2nd.roiH/fac;
        	dstRoiw = ROUND_DOWN_TO_DIVISIBLE((dstRoiw*DUAL_PIP_RATION/100)&(~0x0001), ALIGN_16);
    	    dstRoih = ROUND_DOWN_TO_DIVISIBLE((dstRoih*DUAL_PIP_RATION/100)&(~0x0001), ALIGN_16);
    	    dstRoix = offsetx - dstRoiw;
        	dstRoiy = offsety - dstRoih;
        }
	 else if(dual_pip_mode == DUALSEN_PIP_SEC)
	 {
        	fac = pvmain.roiW/pframe->roiW;
		dstRoiw = pvmain.roiW/fac;
		dstRoih = pvmain.roiH/fac;
        	dstRoiw = ROUND_DOWN_TO_DIVISIBLE((dstRoiw*DUAL_PIP_RATION/100)&(~0x0001), ALIGN_16);
    	    dstRoih = ROUND_DOWN_TO_DIVISIBLE((dstRoih*DUAL_PIP_RATION/100)&(~0x0001), ALIGN_16);
    	    dstRoix = offsetx - dstRoiw;
        	dstRoiy = offsety - dstRoih;
        }
    }

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

    if((uvc_flip ==FLIP_EN) && (dual_pip_mode == DUALSEN_PIP_MAIN))
    {
        proPrm.vec.dst[1].x = dstRoix;
        proPrm.vec.dst[1].y = dstRoiy;
        proPrm.vec.dst[0].x = dstRoix+dstRoiw-1;
        proPrm.vec.dst[0].y = dstRoiy;
        proPrm.vec.dst[3].x = dstRoix+dstRoiw-1;
        proPrm.vec.dst[3].y = dstRoiy+dstRoih-1;
        proPrm.vec.dst[2].x = dstRoix;
        proPrm.vec.dst[2].y = dstRoiy+dstRoih-1;
    }
    else
    {
    	proPrm.vec.dst[0].x = dstRoix;
    	proPrm.vec.dst[0].y = dstRoiy;
    	proPrm.vec.dst[1].x = dstRoix+dstRoiw-1;
    	proPrm.vec.dst[1].y = dstRoiy;
    	proPrm.vec.dst[2].x = dstRoix+dstRoiw-1;
    	proPrm.vec.dst[2].y = dstRoiy+dstRoih-1;
    	proPrm.vec.dst[3].x = dstRoix;
    	proPrm.vec.dst[3].y = dstRoiy+dstRoih-1;
    }
    sp5kGfxObjectMapping(
        &imgSrc,
        &imgDst,
        SP5K_GFX_MAP_MODE_STILL,
        SP5K_GFX_MAPPING_ID_PROJECT, /*image scale up/down*/
        (UINT32)&proPrm
    );

#if PREFORMANCE_PROF
    profLogPrintf(PROFID, "uvc: imecb e");
#endif

	return SUCCESS;
}

/*--------------------------------------------------------------------------*
* Function name    : appUvcRingBufDone
* Function         : Uvc Ring Buf Done
* Return value	   : image callback init (UINT32 if inited ImeCbInited=1)
* Parameter1	   : void
* History          :
*                  : 2018.05.31  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32 appUvcRingBufDone(void)
{
    return ImeCbInited;
}

/*--------------------------------------------------------------------------*
* Function name    : appUvcImeCbUnReg
* Function         : uvc image callback unregister
* Return value	   : result (UINT32 SUCCESS/FAIL)
* Parameter1	   : void
* History          :
*                  : 2018.05.31  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32 appUvcImeCbUnReg(void)
{
	if ( ImeCbInited ) {
		HOST_PROF_LOG_ADD(LEVEL_INFO, "uvc: ime disp cb Unreg");
        appDispUrgentSet(DISP_URGENT_PIP, DISABLE);
	}
	else {
		HOST_PROF_LOG_PRINT(LEVEL_WARN, "uvc: already unregister callback");
		return FAIL;
	}

	ImeCbInited = 0;

	return SUCCESS;
}

/*--------------------------------------------------------------------------*
* Function name    : appUvcImeCbReg
* Function         : uvc image callback register
* Return value	   : result (UINT32 SUCCESS/FAIL)
* Parameter1	   : void
* History          :
*                  : 2018.05.31  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32 appUvcImeCbReg(void)
{
	if ( !ImeCbInited ) {
        HOST_PROF_LOG_ADD(LEVEL_INFO, "uvc: ime disp cb Reg");
        /*lester test performance*/
#if DISKBUSY
        sp5kDiskCfgSet(SP5K_DISK_BUSY_MEAS_CFG, SP5K_DRIVE_SD, 1000); // Start/Stop disk speed periodic measurement
#endif
		appDispUrgentSet(DISP_URGENT_PIP, ENABLE);
    }
	else {
		HOST_PROF_LOG_PRINT(LEVEL_WARN, "uvc: already register callback");
		return FAIL;
	}

    ImeCbInited = 1;

	return SUCCESS;
}

/*--------------------------------------------------------------------------*
* Function name    : appUvcCarLineDraw
* Function         : Uvc Car Line Draw
* Return value	   : void
* Parameter1	   : bShow (BOOL 1:show Car Line)
* Parameter2	   : sel (UINT8 case CAR_REVERSE_LINE)
* History          :
*                  : 2018.05.31  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appUvcCarLineDraw(BOOL bShow, UINT8 sel)
{
    UINT32 state;
  	UINT32 int_x = LCD_SIZE_X>>3;
  	UINT32 end_x = LCD_SIZE_X - int_x;
    UINT32 hor_y = LCD_SIZE_Y>>1;

    state = appActiveStateGet();
    if((state != APP_STATE_VIDEO_PREVIEW && state != APP_STATE_VIDEO_REC))
    {
        HOST_PROF_LOG_PRINT(LEVEL_WARN, "uvc: car line return");
        return ;
    }

    switch(sel)
    {
        case CAR_REVERSE_LINE:
			UINT32 penwidth=3;
            sp5kGfxAttrSet(SP5K_GFX_PEN_WIDTH, penwidth, DISABLE, DISABLE, DISABLE);
            if(!bShow)
            {
                sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, PAL_NOCOLOR, DISABLE, DISABLE, DISABLE);
                sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, PAL_NOCOLOR, DISABLE, DISABLE, DISABLE);
            }
            else
            {
            sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, CVR_HOR_LINE_COLOR, DISABLE, DISABLE, DISABLE);
            sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, CVR_HOR_LINE_COLOR, DISABLE, DISABLE, DISABLE);
            }
            sp5kGfxLineDraw(SP5K_GFX_PAGE_OSD_0, \
                            int_x+int_x, hor_y, end_x-int_x, hor_y);

            if(!bShow)
            {
                sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, PAL_NOCOLOR, DISABLE, DISABLE, DISABLE);
                sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, PAL_NOCOLOR, DISABLE, DISABLE, DISABLE);
            }
            else
            {
            sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, CVR_WARN_LINE_COLOR, DISABLE, DISABLE, DISABLE);
            sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, CVR_WARN_LINE_COLOR, DISABLE, DISABLE, DISABLE);
            }
            sp5kGfxLineDraw(SP5K_GFX_PAGE_OSD_0, \
                            int_x+(int_x>>1), hor_y + (LCD_SIZE_Y>>2), end_x-(int_x>>1), hor_y + (LCD_SIZE_Y>>2));

            if(!bShow)
            {
                sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, PAL_NOCOLOR, DISABLE, DISABLE, DISABLE);
                sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, PAL_NOCOLOR, DISABLE, DISABLE, DISABLE);
            }
            else
            {
            sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, CVR_SAFE_LINE_COLOR, DISABLE, DISABLE, DISABLE);
            sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, CVR_SAFE_LINE_COLOR, DISABLE, DISABLE, DISABLE);
            }
            sp5kGfxLineDraw(SP5K_GFX_PAGE_OSD_0, \
                            int_x, LCD_SIZE_Y, int_x+int_x, hor_y);
            sp5kGfxLineDraw(SP5K_GFX_PAGE_OSD_0, \
                            end_x, LCD_SIZE_Y, end_x-int_x, hor_y);

			penwidth=0;
            sp5kGfxAttrSet(SP5K_GFX_PEN_WIDTH, penwidth, DISABLE, DISABLE, DISABLE);
        break;
    }
}

/*--------------------------------------------------------------------------*
* Function name    : appUvcCarModeGet
* Function         : Get Uvc Car Mode
* Return value	   : result (UINT8 TRUE/FALSE)
* Parameter1	   : void
* History          :
*                  : 2018.05.31  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT8 appUvcCarModeGet(void)
{
    if(reverse_gear == CAR_REVERSE_DEF_CTRL || reverse_gear == CAR_REVERSE_DIS)
        return FALSE;
    else
        return TRUE;
}

/*--------------------------------------------------------------------------*
* Function name    : appVideoSeamlessGPSFileDel
* Function         : Set Uvc Car Mode
* Return value	   : void
* Parameter1	   : en (UINT8 CAR_REVERSE_DIS/CAR_REVERSE_EN/CAR_REVERSE_DEF_CTRL)
* History          :
*                  : 2018.05.31  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appUvcCarModeSet(UINT8 en)
{
    UINT32 mode;
    if(reverse_gear!=en)
    {
		UINT8 ctrl;
        reverse_gear = en;
        sp5kModeGetPeek(&mode);
        if(dual_pip_mode == DUALSEN_SINGLE_MAIN && en==CAR_REVERSE_EN)
        {
            HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "uvc: single main => back");
            ctrl = CAR_REVERSE_DEF_CTRL;
            if(mode==SP5K_MODE_VIDEO_PREVIEW)
            	appUvcPipMode();
        }

        if(dual_pip_mode == DUALSEN_SINGLE_SEC && en==CAR_REVERSE_DIS && ctrl==CAR_REVERSE_DEF_CTRL)
        {
            HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "uvc: back => single main");
            //ctrl = CAR_REVERSE_DIS;
            appDualPipModeSet(DUALSEN_SINGLE_MAIN);
        }

        appUvcCarLineDraw(reverse_gear, CAR_REVERSE_LINE);
        appViewOsd_DualModeDraw();
    }
}

/*--------------------------------------------------------------------------*
* Function name    : appUvcFlipModeGet
* Function         : Get Uvc Flip Mode
* Return value	   : uvc_flip (UINT8 FLIP_EN/FLIP_DIS)
* Parameter1	   : void
* History          :
*                  : 2018.05.31  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT8 appUvcFlipModeGet(void)
{
    HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "uvc: flip %d", uvc_flip);

    /*pip sec need use base fw flow*/
    if(dual_pip_mode == DUALSEN_PIP_SEC)
        return uvc_flip;
    else
        return FLIP_DIS;
}

/*--------------------------------------------------------------------------*
* Function name    : appUvcFlipModeSet
* Function         : Set Uvc Flip Mode
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.31  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appUvcFlipModeSet(void)
{
    (uvc_flip==FLIP_EN)?(uvc_flip=FLIP_DIS):(uvc_flip=FLIP_EN);

    if(appUvcHostCfgGet(UVC_HOST_CFG_PLUG_IN) && (dual_pip_mode == DUALSEN_PIP_SEC))
    {
    	UINT32 w_factor=(-1), h_factor=(-1); //If W-factor and H-factor are -1, display playback size will use still preview size.
    	// Set preview display sourceas playback. 
    	UINT32 setting=(1<<28)|(DISP_CB_2ND_TO_MAIN<<24)|(uvc_flip<<12)|(3<<8)|1;
        sp5kModeCfgSet(SP5K_MODE_CFG_PV_DISP_SCALE_FACTOR, w_factor, h_factor, setting);
    }
}

/*--------------------------------------------------------------------------*
* Function name    : appUvcModeSet
* Function         : Set Uvc Mode
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.31  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appUvcModeSet(void)
{
    UINT32 plug = appUvcHostCfgGet(UVC_HOST_CFG_PLUG_IN);

    if(dual_pip_mode == DUALSEN_PIP_SEC || dual_pip_mode == DUALSEN_SINGLE_SEC)
        appUvcModeCfgSet(plug, DISP_CB_2ND_TO_MAIN);
    else
        appUvcModeCfgSet(plug, DISP_CB_MAIN_PATH);
}

/*--------------------------------------------------------------------------*
* Function name    : appUvcPwrOnRecSet
* Function         : set uvc power on record
* Return value	   : void
* Parameter1	   : sel (UINT8 PWR_ON_REC_EN/PWR_ON_REC_DIS)
* History          :
*                  : 2018.05.31  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appUvcPwrOnRecSet(UINT8 sel)
{
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "uvc: enable pwr on record");
    pwr_on_rec = sel;
}

/*--------------------------------------------------------------------------*
* Function name    : appUvcPwrOnRecGet
* Function         : get uvc power on record status
* Return value	   : pwr_on_rec (UINT32 do uvc power on record:pwr_on_rec=1)
* Parameter1	   : void
* History          :
*                  : 2018.05.31  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32 appUvcPwrOnRecGet(void)
{
    return pwr_on_rec;
}

/*--------------------------------------------------------------------------*
* Function name    : appUvcEventSet
* Function         : set uvc event trigger
* Return value	   : result (UINT32 ENABLE/DISABLE)
* Parameter1	   : sel (UINT8 EVENT_TRIG_INIT/EVENT_TRIG_PROC)
* History          :
*                  : 2018.05.31  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32 appUvcEventSet(UINT8 sel)
{
    static unsigned char fname[32];
    UINT32 seamless_tmr, filetype;	
	tmx_t rtc;
	
    sp5kMediaRecCfgGet(SP5K_MEDIA_REC_SEAMLESS_TIME_SLOT, &seamless_tmr);

    if(sel == EVENT_TRIG_INIT)
    {
        event_flag = appView_ItemDecrease(EVENT_TRIG_MAX, (UINT32)event_flag);
        if(event_flag == EVENT_TRIG_EN)
        {
            HOST_PROF_LOG_PRINT(LEVEL_INFO, "uvc: enable event trig");
           	sp5kMediaRecCfgSet(SP5K_MEDIA_REC_EVENT_TRIGGER_EN, 1);
            sp5kMediaRecCfgSet(SP5K_MEDIA_REC_EVENT_TRIGGER_TYPE,SP5K_MEDIA_REC_UNINTERRUPT_SEAMLESS);
            sp5kMediaRecCfgSet(SP5K_MEDIA_REC_TRIGGER_FRONT_TIME, UVC_EVT_TRIG_TMR );
            sp5kMediaRecCfgSet(SP5K_MEDIA_REC_TRIGGER_BEHIND_TIME, UVC_EVT_TRIG_TMR );	
			return ENABLE;
        }
        else
        {
            HOST_PROF_LOG_PRINT(LEVEL_INFO, "uvc: disable event trig");
            sp5kMediaRecCfgSet(SP5K_MEDIA_REC_EVENT_TRIGGER_EN, 0);
            sp5kMediaRecCfgSet(SP5K_MEDIA_REC_TRIGGER_TIME_SLOT, 0);
        }
		return DISABLE;
    }
    else if(sel == EVENT_TRIG_PROC)
    {
        if(event_flag == EVENT_TRIG_EN && seamless_tmr>0)
        {
			sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_FILE_TYPE, &filetype);
			sp5kRtcDateTimeGet( SP5K_DATE_TIME_OPTION, &rtc);
			
            sprintf((char *)fname,"D:\\EVENT\\%04d%02d%02d_%02d%02d%02dM",
                rtc.tmx_year+1900,
                rtc.tmx_mon,
                rtc.tmx_mday,
                rtc.tmx_hour,
                rtc.tmx_min,
                rtc.tmx_sec);

	    	switch(filetype)
	    	{
	    		case SP5K_DCF_FILETYPE_AVI:
	    			strcat((char *)fname, ".AVI");
	    			break;
	    		case SP5K_DCF_FILETYPE_MOV:
	    			strcat((char *)fname, ".MOV");
	    			break;
				default:
					return DISABLE;
	    	}
		
			sp5kMediaRecCfgSet(SP5K_MEDIA_REC_TRIGGER_FILE_NAME, (UINT32)fname);
			HOST_PROF_LOG_PRINT(LEVEL_INFO, "uvc: evt trig0 %s", fname);						
			
	        if(appUvcHostCfgGet(UVC_HOST_CFG_PLUG_COMPLETE))
	        {
		        char *p = strchr((char *)fname, '.');
				fname[strlen((const char *)fname)-strlen((const char *)p)-1]='S';
				HOST_PROF_LOG_PRINT(LEVEL_INFO, "uvc: evt trig1 %s", fname);
				
	        	sp5kMediaRecCfgSet(SP5K_MEDIA_REC_2ND_TRIGGER_FILE_NAME, (UINT32)fname);		
	        }	
			
			sp5kMediaRecControl(SP5K_MEDIA_REC_EVENT_TRIGGER_MODE);
			return ENABLE;
        }
		else
			return DISABLE;
    }
	else
		return DISABLE;
}

/*--------------------------------------------------------------------------*
* Function name    : appUvcPipMode
* Function         : Config Uvc Pip Mode
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.31  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appUvcPipMode(void)
{
    if(reverse_gear == CAR_REVERSE_EN)
        dual_pip_mode = DUALSEN_SINGLE_SEC;
    else
	    dual_pip_mode = appView_ItemDecrease(DUAL_SEN_MAX, dual_pip_mode);

    appVideoDualStreamCfgSet(dual_pip_mode);
	
    //appViewOsd_DualModeDraw();
}

/*--------------------------------------------------------------------------*
* Function name    : appDualPipModeSet
* Function         : Set Uvc Pip Mode
* Return value	   : void
* Parameter1	   : val 
  (UINT8 DUALSEN_SINGLE_MAIN/DUALSEN_SINGLE_SEC/DUALSEN_PIP_MAIN/DUALSEN_PIP_SEC)
* History          :
*                  : 2018.05.31  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appDualPipModeSet(UINT8 val)
{
    dual_pip_mode = val;
}

/*--------------------------------------------------------------------------*
* Function name    : appDualPipModeGet
* Function         : Get Uvc Pip Mode
* Return value	   : dual_pip_mode
  (UINT8 DUALSEN_SINGLE_MAIN/DUALSEN_SINGLE_SEC/DUALSEN_PIP_MAIN/DUALSEN_PIP_SEC)
* Parameter1	   : void
* History          :
*                  : 2018.05.31  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT8 appDualPipModeGet(void)
{
	return dual_pip_mode;
}

/*--------------------------------------------------------------------------*
* Function name    : appUvcCamVpvEnter
* Function         : uvc video prevew mode enter
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.31  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appUvcCamVpvEnter(void)
{
    appVideoDualStreamCfgSet(appDualPipModeGet());
    appModeSet(SP5K_MODE_VIDEO_PREVIEW);
}

/*--------------------------------------------------------------------------*
* Function name    : appUvcCamInit
* Function         : delete first index GPS file
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.31  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appUvcCamInit(void)
{
	UINT32 usbdata=0; // no use
    sp5kUsbhostSysEvtHandlerSet(appUvcCamEvtMsgCb, usbdata);
    appUvcCamCfgInit();
}

