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

#include <math.h>
#include "api/sp5k_capture_api.h"
#include "api/sp5k_cdsp_api.h"
#include "api/sp5k_cal_api.h"
#include "api/sp5k_moe_api.h"
#include "api/sp5k_gfx_api.h"
#include "api/sp5k_moe_api.h"
#include "api/sp5k_dbg_api.h"
#include "api/sp5k_utility_api.h"
#include "api/sp5k_media_api.h"
#include "api/sp5k_sensor_api.h"

#include "app_com_def.h"
#include "app_com_api.h"
#include "app_timer.h"
#include "app_still.h"
#include "app_sensor.h"
#include "app_view_param_def.h"
#include "app_view_param.h"
#include "app_face_detect_ex.h"
#include "app_zoom_api.h"
#include "app_calib_api.h"
#include "app_view_osd.h"
#include "app_still_hdr.h"
#include "app_stringid.h"
#include "middleware/sysg_resource_def.h"
#include "app_awbalg_api.h"
#include "app_playback_def.h"
#include "app_errorosd.h"
#if SP5K_DIQ_FRMWRK
#include "sp5k_dq_api.h"
#endif
#if SP5K_DIQ_AUTO_WDR
#include "app_auto_alg.h"
#endif
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define RAW_BATCH_FILE		"D:\\RAW\\RAWBATCH.TXT"
#define FORCE_CHANGE_RAW    0
#define SHADE_BY_SCALE		0
#define W		6048 /* _IMAGE_24M_WIDTH_ */
#define H		4032 /* _IMAGE_24M_HEIGHT_ */
#define LUTSIZE	3635 /* sqrt((w/2)^2+(h/2)^2) + 1 */
#define LOGI_UNCACH_BADDR_TO_LOGI_CACH_BADDR(addr) 	((void *)((UINT32)(addr) & ~0x20000000))
#define LOGI_CACH_BADDR_TO_LOGI_UNCACH_BADDR(addr) 	((void *)((UINT32)(addr) | 0x20000000))
#define ROUND_DOWN_TO_DIVISIBLE(num,div) ( (UINT32)(num) & -(UINT32)(div) )
#define ROUND_UP_TO_DIVISIBLE(num,div) ROUND_DOWN_TO_DIVISIBLE( (UINT32)(num) + (div) - 1, div )

#define DUAL2ND_YUV_WIDTH  1920
#define DUAL2ND_YUV_HEIGHT 1080
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static BOOL fRawBatchInited;
static UINT8 *RawBatch;
static UINT32 RawBatchPos,RawBatchSize;
UINT32 BlinkYUVCallBackSet =0;
UINT32 BlinkGet=0; /*Have Blink Eyes*/
extern UINT32 BlinkSaveSelect;
extern UINT32 CapWithFaceCheck;

//appEffectCapture
typedef struct yuvFrameInfo_s
{
	UINT8 *pbuf;
	sp5kPrevYuvRoi_t roi;
} yuvFrameInfo_t;

static UINT8 *pyuvFrameBuf;
static UINT32 yuvCapDone, yuvSaveDone;
UINT32 gSensorMode=0x30;

//pvqv
#if !SP5K_PVQV_DISABLE
sp5kGfxPicture_t PvQVsrcPic;
UINT8 *PvQVdestBuf;
#endif

//effect
struct {
	UINT8 *alpha;       //toycam / lighttone
	UINT8 *noise;       //effect mono:
	UINT8 *noiseSmall;  //effect mono:
	UINT32 w,h;
    UINT32 type;
} effectCb;

static sp5kPrevUrgentCallback_t ucbs[2];
static UINT16 sW, sH;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern void SP_CapturePostWb(UINT32 addr, UINT32 RawFmt, void* ImgSize);
extern void cpuDCacheFlush(void* addr, UINT32 size);
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
UINT32 appStillWbCb(UINT32 addr,UINT32 rawfmt,void* size);

/*-------------------------------------------------------------------------
 *  Function Name : _rawBatchTokenGet
 *  Description :
 *------------------------------------------------------------------------*/
UINT32
_rawBatchTokenGet(
	UINT8 *buf,
	UINT32 bufsize
)
{
	UINT32 i,l=0;
	for (i=RawBatchPos ; i<RawBatchSize ; i++) {
		switch (RawBatch[i]) {
		case 10: case 13: case 9: case ' ':
			if (l) goto Exit_RawBatchTokenGet;
			continue;
			break;
		default:
			buf[l++] = RawBatch[i];
			if (l>=bufsize) goto Exit_RawBatchTokenGet;
			break;
		}
	}
Exit_RawBatchTokenGet:
	if (l+1<=bufsize)
		buf[l] = 0;
	RawBatchPos = i;
	return RawBatchPos<RawBatchSize && l+1<=bufsize ? SUCCESS : FAIL;
}

/*-------------------------------------------------------------------------
 *  Function Name : _rawBatchEntryGet
 *  Description :
 *------------------------------------------------------------------------*/
UINT32
_rawBatchEntryGet(
	UINT8 *fname,
	appWbGain_t *pwb
)
{
	UINT8 buf[8];
	if (_rawBatchTokenGet(fname, 32)!=SUCCESS) return FAIL;
	if (_rawBatchTokenGet(buf, 8)!=SUCCESS) return FAIL;
	pwb->rgain = atoi((const char *)buf);
	if (_rawBatchTokenGet(buf, 8)!=SUCCESS) return FAIL;
	pwb->grgain = atoi((const char *)buf);
	if (_rawBatchTokenGet(buf, 8)!=SUCCESS) return FAIL;
	pwb->bgain = atoi((const char *)buf);
	if (_rawBatchTokenGet(buf, 8)!=SUCCESS) return FAIL;
	pwb->gbgain = atoi((const char *)buf);
	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  Function Name : appChangeRawCb
 *  Description :
 *------------------------------------------------------------------------*/
void
appChangeRawCb(
	UINT32 Addr,
	UINT32 RawFmt,
	sp5kYuvCbImgSize_t *  pImgSize
)
{
	UINT16 *imgAddr=(UINT16*)Addr;
	UINT32 bufw,bufh,imgw=0,imgh=0;
	UINT32 fd,fl;
	char fname[32];
	appWbGain_t wb;

	if (!fRawBatchInited) {
		fRawBatchInited = 1;
		ReadFileEx((UINT8*)RAW_BATCH_FILE, &RawBatch, &RawBatchSize);
		RawBatchPos = 0;
	}

	if (RawBatchSize) {
		if (_rawBatchEntryGet((UINT8*)fname, &wb)==FAIL) {
			DBG_PRINT("scb: RawBatch end\n");
			sp5kFree(RawBatch);
			RawBatchSize = 0;
			return;
		}
	}

 	DBG_PRINT("scb: RawChange:%s ...", fname);
	fd = sp5kFsFileOpen((const UINT8 *)fname, SP5K_FS_OPEN_RDONLY);
	DBG_PRINT("scb: %s\n", fd ? "OK" : "NG");
	if (fd==0) return;
	fl = sp5kFsFileSizeGet(fd);
	bufw = pImgSize->yuvWidth;
	bufh = pImgSize->yuvHeight;
	if (bufw==3264 && bufh==2448 && fl==3240*2430*2) { // Sony 3H raw format
		imgw = 3240; imgh = 2430;
	} else if (bufw*bufh*2 <= fl) {
		imgw = bufw; imgh = bufh;
	} else {
		DBG_PRINT("scb: [ERR] raw change error!!\n");
	}

	if (imgw>0 && imgh>0) {
		if (bufw==imgw && bufh==imgh) {
			sp5kFsFileRead(fd, (UINT8*)imgAddr, imgw*imgh*2);
		} else {
			UINT16 *src,*dst;
			UINT32 y;
			src = imgAddr + (bufw*bufh - imgw*imgh)*2;
			dst = imgAddr;
			sp5kFsFileRead(fd, (UINT8*)src, imgw*imgh*2);
			for (y=0 ; y<imgh ; y++) {
				memcpy(dst, src, imgw*2);
				src += imgw;
				dst += bufw;
			}
		}
	}
	sp5kFsFileClose(fd);

	if (RawBatchSize) {
		DBG_PRINT("scb: RawBatch WB:%d %d/%d %d\n", wb.rgain, wb.grgain, wb.gbgain, wb.bgain);
		if(wb.rgain ==0 && wb.grgain ==0 && wb.gbgain == 0 && wb.bgain ==0) {
			appStillWbCb(Addr, RawFmt, pImgSize);
		}
	}
}

/*-------------------------------------------------------------------------
 *  Function Name : appStillYuvCB
 *  Description :
 *------------------------------------------------------------------------*/
UINT32
appStillYuvCB(
	UINT32 addr,
	UINT32 qvAddr,
	void* size
)
{
	sp5kHostMsgSend(APP_UI_MSG_VIEW_YUV_CALLBACK, 0);
	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  Function Name : appStillWbCb
 *  Description :   This will do post white balance. It also be called in
 * 					Costomer WB function.
 *------------------------------------------------------------------------*/
extern appWbGain_t cwb;
UINT32
appStillWbCb(
	UINT32 addr,
	UINT32 rawfmt,
	void* size
)
{
	HOST_PROF_LOG_PRINT(LEVEL_INFO, "scb: %s init s",__FUNCTION__);
	if (pViewParam->wb != UI_WB_CUSTOM)
	{
    	appAWBALG_CapPostWBSet(addr, rawfmt, size);
	}
	else
	{
		sp5kIqModeSet(SP5K_MODE_STILL_SNAP);
		sp5kIqModeSetDone(SP5K_MODE_STILL_SNAP);
	}
	return SUCCESS;
}

UINT32
appStillAutoWDRCb(
	UINT32 addr,
	UINT32 rawfmt,
	void* rawInfo
)
{
#if SP5K_DIQ_AUTO_WDR
	sp5kImgWbInfo_t *info=(sp5kImgWbInfo_t*)rawInfo;
	UINT8 bayer[4];
	UINT16 *p=(UINT16 *)addr;
        UINT8 *p8=(UINT8 *)addr;
	UINT32 w,h,x,y,dx,dy;
	UINT32 filedOffs;
	UINT8 ridx[]={1, 0, 3, 2};//BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,
	UINT8 gidx[]={0, 1, 2, 3};
	/*fmt 0 : 8bits , fmt 1 : 10bits , 2 : 12bits , 3 : 14bits, 4 : 16bits*/
	UINT32 shiftBit[5] = {0,0,0,2,4};
        UINT32 lumin[4];
        UINT32 Histo[256]= {0};
        UINT32 R,GR,GB,B,Y;
        UINT32 wbgain[4];
        UINT8 *Gma;
	UINT32 mode5k;

	sp5kModeGet(&mode5k);
	sp5kIqModeSet( mode5k  );
        sp5kIqCfgGet( SP5K_IQ_CFG_WB_GAIN_R_GAIN, &wbgain[0] );
        sp5kIqCfgGet( SP5K_IQ_CFG_WB_GAIN_GR_GAIN, &wbgain[1] );
        sp5kIqCfgGet( SP5K_IQ_CFG_WB_GAIN_B_GAIN, &wbgain[2] );
        sp5kIqCfgGet( SP5K_IQ_CFG_WB_GAIN_GB_GAIN, &wbgain[3] );
        sp5kIqModeSetDone( mode5k );


        sp5kDynIqCfgGet(SP5K_DYN_IQ_CFG_RGB_GAMMA_GET, &Gma, SYSG_RESOURCE_ID_R_GAMMA);

	w = info->width;
	h = info->height;

	filedOffs = info->width;

	bayer[0] = ridx[info->bayerOrder]; //R
	bayer[1] = gidx[info->bayerOrder]; //Gr
	bayer[2] = 3-gidx[info->bayerOrder]; //Gb
	bayer[3] = 3-ridx[info->bayerOrder]; //B

	dx = dy = 0;

	do{
		dx += 2;
		dy += 2;
	}while( ((w/dx+1)*(h/dy+1)) > 65535);

    if(rawfmt==4)//16bits
    {
    	for( y = 0; y < h; y += dy )
    	{
            for ( x = 0; x < w; x += dx )
            {
                /* to 12 bit */
                lumin[0] = p[x + 0] >> shiftBit[rawfmt];
                lumin[1] = p[x + 1] >> shiftBit[rawfmt];
                lumin[2] = p[x + 0 + filedOffs] >> shiftBit[rawfmt];
                lumin[3] = p[x + 1 + filedOffs] >> shiftBit[rawfmt];

                R = lumin[bayer[0]]*wbgain[0]/0x400;
                GR = lumin[bayer[1]];
                GB = lumin[bayer[2]];
                B = lumin[bayer[3]]*wbgain[2]/0x400;

                if(R>4095)
                R=4095;

                if(B>4095)
                B=4095;

                R=Gma[R];
                GR=Gma[GR];
                GB=Gma[GB];
                B=Gma[B];

                Y = ((R*299) + (((GR+GB)*587)>>1) + (B*114))/1000;

                Histo[Y]++;
            }
            p += dy * w;
    	}
    }
    else if(rawfmt==2)//12bit pack
    {
        filedOffs = w*3/2;

    	for( y = 0; y <  h; y += dy )
    	{
            for ( x = 0; x < w*3/2; x += dx*3/2 )
            {
                /* to 12 bit */
                lumin[0] = ((((UINT32)(p8[x+1]&0x0f)) << 8)|p8[x ] )>> shiftBit[rawfmt];
                lumin[1] = ((UINT32)(p8[x+2]<<4) |(p8[x+1 ]>>4) )>> shiftBit[rawfmt];
                lumin[2] = ((((UINT32)(p8[x+1+ filedOffs]&0x0f)) << 8)|p8[x+filedOffs] )>> shiftBit[rawfmt];
                lumin[3] = ((UINT32)(p8[x+2+ filedOffs]<<4) |(p8[x+ 1+filedOffs]>>4) )>> shiftBit[rawfmt];

                R = lumin[bayer[0]]*wbgain[0]/0x400;
                GR = lumin[bayer[1]];
                GB = lumin[bayer[2]];
                B = lumin[bayer[3]]*wbgain[2]/0x400;

                if(R>4095)
                    R=4095;

                if(B>4095)
                    B=4095;

                R=Gma[R];
                GR=Gma[GR];
                GB=Gma[GB];
                B=Gma[B];

                Y = ((R*299) + (((GR+GB)*587)>>1) + (B*114))/1000;

                Histo[Y]++;
            }
            p8 += dy * w*3/2;
    	}
    }
    else//no support
    {
        printf("Raw Fmt Unsupport!\n");
    }

    appAutoWDRLibOneShot(Histo);
#endif

    return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  Function Name : appStillBpcCb
 *  Description :   Bad pixel compensation
 *------------------------------------------------------------------------*/
UINT32
appStillRawCbExecute(
	UINT32 addr,
	UINT32 rawfmt,
	sp5kYuvCbImgSize_t *  pImgSize
)
{
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "scb: %s init s",__FUNCTION__);

    //appStillAutoWDRCb(addr, rawfmt, pImgSize);

	if(appCalibRawBatchEnableGet() || FORCE_CHANGE_RAW==1){
		appChangeRawCb(addr, rawfmt, pImgSize);
	}

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  Function Name : appStill_ViewMenuSelFunctionCB
 *  Description :   This callback will response and record the user setting
 *					in menu. It will take charge in still/video menu. Some
 *					items need immeditly response to the user selection.
 *------------------------------------------------------------------------*/
void
appStillRawCbSet(
	void
)
{
	/*WB CallBack set*/
	sp5kStillCapCallbackSet(SP5K_CAPTURE_CALLBACK_WB ,(fp_sp5kCallback_t)appStillWbCb);


	if(appCalibRawBatchEnableGet() || FORCE_CHANGE_RAW==1)
	{
		UINT32 compressionRatio = 200;
		sp5kStillCapCfgSet(SP5K_CAPTURE_BRC, 0);
		sp5kStillCapCfgSet(SP5K_CAPTURE_MAIN_COMPRESSION_RATIO, compressionRatio);
		sp5kStillCapCallbackSet(SP5K_CAPTURE_CALLBACK_WB , NULL);
	}

	sp5kStillCapCallbackSet(SP5K_CAPTURE_CALLBACK_RAW ,(fp_sp5kCallback_t)appStillRawCbExecute);

}

/*-------------------------------------------------------------------------
 *  Function Name : appStillYuvCB
 *  Description :
 *------------------------------------------------------------------------*/
#if !SP5K_PVQV_DISABLE
UINT32
appStillQVCB(
	UINT32 addr,
	UINT32 qvAddr,
	void* size
)
{
    UINT32 ret = SUCCESS;
    UINT32 srcAddr;
    UINT32 ScalW, ScalH;
    sp5kGfxObj_t srcObj, dstObj;
    sp5kYuvCbImgSize_t *pYuvSize=(sp5kYuvCbImgSize_t *)size;

    srcAddr = qvAddr;
    ScalW = 160;
    ScalH = 128;
    HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "scb: appStillQVCB w=%d h=%d",ScalW,ScalH);
    if (PvQVdestBuf == NULL)
        PvQVdestBuf = sp5kYuvBufferAlloc(ScalW,ScalH);
    else
    {
        sp5kYuvBufferFree(PvQVdestBuf);
        PvQVdestBuf = sp5kYuvBufferAlloc(ScalW,ScalH);
    }

    if (PvQVdestBuf == NULL)
    {
        HOST_PROF_LOG_ADD(LEVEL_ERROR, "scb: appStillQVCB fail");
        sp5kYuvBufferFree(PvQVdestBuf);
        PvQVdestBuf = NULL;
        return FAIL;
    }

    srcObj.pbuf = (UINT8 *)srcAddr;
    srcObj.bufW = ROUND_UP_TO_DIVISIBLE(pYuvSize->roiWidth, 16);
    srcObj.bufH = ROUND_UP_TO_DIVISIBLE(pYuvSize->roiHeight, 16);
    srcObj.roiX = 0;
    srcObj.roiY = 0;
    srcObj.fmt  = SP5K_GFX_FMT_YUV422;
    srcObj.roiW = pYuvSize->roiWidth;
    srcObj.roiH = pYuvSize->roiHeight;
    dstObj.pbuf = PvQVdestBuf;
    dstObj.bufW = ScalW;
    dstObj.bufH = ScalH;
    dstObj.roiX = 0;
    dstObj.roiY = 0;
    dstObj.fmt  = SP5K_GFX_FMT_YUV422;
    dstObj.roiW = ScalW;
    dstObj.roiH = ScalH;
    sp5kGfxObjectBilinearScale(&srcObj, &dstObj);

    //for debug use
#if 0
    sp5kGfxPicture_t dstPic;
    UINT32 roiX, roiY;

    PvQVsrcPic.pbuf = PvQVdestBuf;
    PvQVsrcPic.frmW = ScalW;
    PvQVsrcPic.frmH = ScalH;
    PvQVsrcPic.roiX = 0;
    PvQVsrcPic.roiY = 0;
    PvQVsrcPic.roiW = ScalW;
    PvQVsrcPic.roiH = ScalH;
    PvQVsrcPic.fmt  = SP5K_GFX_FMT_YUV422;

    sp5kGfxAttrSet( SP5K_GFX_REFRESH_ACTIVE, 0, 0, 0, 0 );
    sp5kGfxPageClear( SP5K_GFX_PAGE_PIP_0, PAL_NOCOLOR );
    printf("appStillQVCB %d %d %d %d\n",pYuvSize->cropWidth, pYuvSize->cropHeight,\
           pYuvSize->width, pYuvSize->height);

    roiX = ( pYuvSize->cropWidth  * 75 / 100 ) & ( ~0x0001 ) ;
    roiY = ( pYuvSize->cropHeight * 75 / 100 ) & ( ~0x0001 );

    dstPic.pbuf = ( UINT8* )qvAddr;
    dstPic.frmW = pYuvSize->cropWidth;
    dstPic.frmH = pYuvSize->cropHeight;
    dstPic.roiX = roiX;
    dstPic.roiY = roiY;
    dstPic.roiW = ScalW;
    dstPic.roiH = ScalH;
    dstPic.fmt  = SP5K_GFX_FMT_YUV422;
    ret = sp5kGfxPictureBlend(&dstPic, 128, 1, 0, &PvQVsrcPic);

    sp5kGfxPageClear(SP5K_GFX_PAGE_PIP_0, PAL_NOCOLOR);
    sp5kGfxAttrSet(SP5K_GFX_REFRESH_ACTIVE, 1, 0, 0, 0);
#endif

    return ret;
}


/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_PvQVInfoGet
 *  Description :
 *------------------------------------------------------------------------*/
UINT8
appViewOsd_PvQVInfoGet(
sp5kGfxPicture_t** pic
)
{
    if(PvQVdestBuf==NULL)
        return FAIL;
    else
        *pic = &PvQVsrcPic;

    return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewOsd_PvQVInfoFree
 *  Description :
 *------------------------------------------------------------------------*/
void
appViewOsd_PvQVInfoFree(
void
)
{
	if (PvQVdestBuf != NULL)
	{
        sp5kYuvBufferFree(PvQVdestBuf);
        PvQVdestBuf = NULL;
	}
}

/*-------------------------------------------------------------------------
 *  Function Name : appStillQVCbSet
 *  Description :
 *------------------------------------------------------------------------*/
void
appStillQVCbSet(
	BOOL isEnable
)
{

	/* Avoid black color is set to transparency */
	sp5kGfxAttrSet(SP5K_GFX_YUV422_TO_RGB565_DITHER_ENABLE, 1, 0, 0, 0);

	if(isEnable)
	{
  		sp5kStillCapCallbackSet(SP5K_CAPTURE_CALLBACK_QV ,appStillQVCB);

        appViewOsd_PvQVDraw(FALSE, gStillCB.cap.qvSec);
	}
	else
	{
   		sp5kStillCapCallbackSet(SP5K_CAPTURE_CALLBACK_QV ,NULL);

        if(gStillCB.osd <= OSD_VIEW_VERBOSE)
            appViewOsd_PvQVDraw(TRUE, gStillCB.cap.qvSec);
        else
            appViewOsd_PvQVDraw(FALSE, gStillCB.cap.qvSec);
	}
}
#endif
/*-------------------------------------------------------------------------
 *  Function Name : appStillYuvCbSet
 *  Description :   To enable/disable yuv callback. If we want to use fieldQV
 *					we should disable this. If we have highlight function
 * 					it will also be implement in yub callback.
 *------------------------------------------------------------------------*/
void
appStillYuvCbSet(
	BOOL isEnable
)
{

	if(isEnable)
	{
       	HOST_PROF_LOG_ADD(LEVEL_INFO, "scb: yuv cb en");
		sp5kStillCapCallbackSet(SP5K_CAPTURE_CALLBACK_YUV_WITH_CDSP ,appStillYuvCB);
	}
	else
	{
		sp5kStillCapCallbackSet(SP5K_CAPTURE_CALLBACK_YUV_WITH_CDSP ,NULL);
	}
}

static UINT32 appPreviewYUVCb(
	UINT32 streamId,        /* nth stream, from 0 */
	UINT32 fid,             /* frame id */
	frameBufInfo_t *pframe, /* buffer information */
	UINT8 *pbufdup          /* duplicated buffer address */
)
{
    /*printf("RawYUV CB id=%d %d %x\n", fid, streamId, (UINT32)pframe->pbuf);*/

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
        .pbuf = pyuvFrameBuf,
        .bufW = pframe->width,
        .bufH = pframe->height,
        .roiX = pframe->roiX,
        .roiY = pframe->roiY,
        .fmt = SP5K_GFX_FMT_YUV422,
        .roiW = pframe->roiW,
        .roiH = pframe->roiH
    };

    if (!yuvCapDone)
    {
        sW = pframe->width;
        sH = pframe->height;
        //memcpy(pyuvFrameBuf, pframe->pbuf, sW * sH*2);
        sp5kGfxObjectCopy(&srcObj, &dstObj);
    	yuvCapDone = 1;
    }
	/*printf("UrgentCb (%d,%d)\n",sW, sH);*/
    return SUCCESS;
}

static void
_SaveJPGFileProc(
	void
)
{
#if defined(CUSTOM_EXIF_TIME_SET)
    UINT8 time[32];
#endif
    UINT32 i=0;
    tmx_t my_tmx;
    sp5kPrevYuvRoi_t roi;

    profLogPrintf(0, "Save YUV to Jpeg S");

    sp5kRtcDateTimeGet(0, &my_tmx);
    if (pViewParam->stillStamp != UI_DATESTAMP_OFF) {
        sp5kPreviewYuvCapDateTimeSet(&my_tmx);
    }

#if defined(CUSTOM_EXIF_TIME_SET)
    sprintf((char *)time, "%04u:%02u:%02u:%02u:%02u:%02u",
        (unsigned)(my_tmx.tmx_year + 1900),
        (unsigned)(my_tmx.tmx_mon + 1), /* modified by Edward 20070521 */
        (unsigned)my_tmx.tmx_mday,
        (unsigned)my_tmx.tmx_hour,
        (unsigned)my_tmx.tmx_min,
        (unsigned)my_tmx.tmx_sec);

    printf("\n\n *** time:%s\n\n",time);
#endif

    //FXIME!!  If use DCF should be a different name.
    for(i=0; i<1; i++)
    {
        profLogPrintf(0, "[host]scb: jpgSave:%d", i);

#if defined(CUSTOM_EXIF_TIME_SET)
        //config date-stamp time as exif time
        sp5kSystemCfgSet(SP5K_EXIF_CUSTOM_RTC_CFG,time);
#endif

        memset(&roi, 0, sizeof(sp5kPrevYuvRoi_t));
        roi.width = sW;
        roi.height= sH;
        roi.roiw = DUAL2ND_YUV_WIDTH;
        roi.roih = DUAL2ND_YUV_HEIGHT;

        sp5kPreviewJpegSave(pyuvFrameBuf, &roi, DUAL2ND_YUV_WIDTH, DUAL2ND_YUV_HEIGHT);
    }

#if defined(CUSTOM_EXIF_TIME_SET)
    //disable custom RTC config
    sp5kSystemCfgSet(SP5K_EXIF_CUSTOM_RTC_CFG,"");
#endif

    sp5kYuvBufferFree(pyuvFrameBuf);
    pyuvFrameBuf = NULL;
    profLogAdd(0, "[host]scb: Save YUV to Jpeg E");
}

/*-------------------------------------------------------------------------
 *  Function Name : appYUVCapture
 *  Description :
 *		Use for dual sensor preview capture.
 *		path : 0 is front sensor, 1 is rear sensor
 *------------------------------------------------------------------------*/
void appYUVCapture(
	UINT32 path
)
{
    UINT32 TempWidth = 0, TempHeight = 0;
    UINT32 TempDualSenMode;

    if(yuvSaveDone)
    {
        HOST_PROF_LOG_ADD(LEVEL_INFO, "scb: 2nd capture again");
        return;
    }
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_WIDTH, DUAL2ND_YUV_WIDTH);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_HEIGHT, DUAL2ND_YUV_HEIGHT);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_WIDTH, DUAL2ND_YUV_WIDTH);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_HEIGHT, DUAL2ND_YUV_HEIGHT);

	sp5kSensorModeCfgSet(SP5K_MODE_VIDEO_PREVIEW, DEFAULT_PREVIEW_MODE);
    TempDualSenMode = sp5kSensorModeCfgGet(SP5K_MODE_SEC_SENSOR_SEL|SP5K_MODE_VIDEO_PREVIEW);
    sp5kSensorModeCfgSet(SP5K_MODE_SEC_SENSOR_SEL|SP5K_MODE_VIDEO_PREVIEW, DEFAULT_PREVIEW_MODE);

	sp5kModeSet(SP5K_MODE_VIDEO_PREVIEW);
	sp5kModeWait(SP5K_MODE_VIDEO_PREVIEW);

	yuvCapDone = 0;
	appSetYUVSaveStatus(1);
    TempWidth = ROUND_UP_TO_DIVISIBLE(DUAL2ND_YUV_WIDTH, 16);
    TempHeight = ROUND_UP_TO_DIVISIBLE(DUAL2ND_YUV_HEIGHT, 16);
	pyuvFrameBuf = sp5kYuvBufferAlloc(TempWidth*2, TempHeight);
	if (pyuvFrameBuf)
	{
		if (path == 1)
		{
		        ucbs[0].fp = NULL;
		        ucbs[1].fp = appPreviewYUVCb;
		        HOST_PROF_LOG_ADD(LEVEL_INFO, "scb: Wait Preview YUV s1");
		        sp5kVideoUrgentCallbackSet(1, 0, ucbs, 2);  //Because PreviewCB not surpport get 2nd YUV 20140509.
		}
		else
		{
		        ucbs[0].fp = appPreviewYUVCb;
		        ucbs[1].fp = NULL;
		        HOST_PROF_LOG_ADD(LEVEL_INFO, "scb: Wait Preview YUV s0");
		        sp5kVideoUrgentCallbackSet(1, 0, ucbs, 1);  //Because PreviewCB not surpport get 2nd YUV 20140509.
		}

		while (!yuvCapDone)
		{
			sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 20);
		}

        ucbs[0].fp = NULL;
        ucbs[1].fp = NULL;
        sp5kVideoUrgentCallbackSet(1, 0, ucbs, 1);
        sp5kVideoUrgentCallbackSet(1, 0, ucbs, 2);
		HOST_PROF_LOG_ADD(LEVEL_INFO, "scb: Wait Preview YUV e");

#if defined(CUSTOM_EXIF_TIME_SET)
        //get RTC for exif and date-stamp
        sp5kRtcDateTimeGet( 0, &my_tmx);
        my_tmx.tmx_mon -= 1; /* 1~12 to 0~11 */
#endif
        _SaveJPGFileProc();
	}
	else
	{
        HOST_PROF_LOG_ADD(LEVEL_WARN, "scb: no buffer for speedy burst");
	}
    appModeSet(SP5K_MODE_STILL_PREVIEW);
    sp5kSensorModeCfgSet(SP5K_MODE_SEC_SENSOR_SEL|SP5K_MODE_VIDEO_PREVIEW, TempDualSenMode);
	profLogAdd(0, "YUV capture end");
}

/*-------------------------------------------------------------------------
 *  Function Name : appGetYUVSaveStatus
 *  Description :
 *------------------------------------------------------------------------*/
UINT32 appGetYUVSaveStatus(void)
{
    return yuvSaveDone;
}

/*-------------------------------------------------------------------------
 *  Function Name : appSetYUVSaveStatus
 *  Description :
 *------------------------------------------------------------------------*/
void appSetYUVSaveStatus(UINT32 val)
{
    yuvSaveDone = val;
}
