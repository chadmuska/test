/**************************************************************************
 *
 *       Copyright (c) 2009-2018 by iCatch Technology, Inc.
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
 *	File name    : App_video_stamp.c
 *	Function     : about video stamp 		
 *	First editor :   
 *	First date   :   
 *	History      :
 *               : 2018.05.28	Edward.Lu  "Add description and modify code"
 **************************************************************************/

#include "app_com_def.h"
#include "app_com_api.h"

#include "sp5k_dbg_api.h" /**< HOST_ASSERT(expr); */
#include "sp5k_utility_api.h"

#include "app_aealg_api.h"
#include "app_osdfont.h"
#include "app_video.h"
#include "app_view_param.h"
#include "sp5k_media_api.h"
#include "app_osd_api.h"

#if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
#include "sp5k_media_api.h"
#if SP5K_SENSOR_DUAL_MODE
#include "app_still.h"
#endif
#endif
#if (SP5K_3A_INFO_DBG)
#include "sp5k_dq_api.h"
#include "app_awbalg_api.h"
#endif

#ifdef VIDEO_REC_STAMP
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
#define STRING_SIZE 82
#define REC_STAMP_W 1920 /* 21* '5' about 320pixel,  smaller char smaller wdith*/
#define REC_STAMP_H 40  /*one string is 32 lines.*/
#else
#define STRING_SIZE 50
#define REC_STAMP_W 640 /* 21* '5' about 320pixel,  smaller char smaller wdith*/
#define REC_STAMP_H 32  /*one string is 32 lines.*/
#endif
#define REC_STAMP_X 8
#define REC_STAMP_Y 4
#define REC_WARN_W  192
#define REC_WARN_H  192

#define CDSP_SKIP_DETECTION   	0

#if (SP5K_VREC_YUV_SAVE)
#define YUV_GET_MAX 20
#endif

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
#if (SP5K_VREC_YUV_SAVE)
typedef struct {
    UINT8 *pBuf;
    UINT32 width;
    UINT32 height;
} vrecYuvGet_t;
#endif

typedef enum {
    USE_PIP_LAYER,
    USE_OTHERS
} appStampSrc_e;

typedef struct {
    appStampSrc_e srcBuf;
    UINT16 u1st_X;
    UINT16 u1st_Y;
    UINT16 u1st_W;
    UINT16 u1st_H;
    UINT8* u2nd_Buf;
    UINT16 u2nd_X;
    UINT16 u2nd_Y;
    UINT16 u2nd_W;
    UINT16 u2nd_H;
    UINT16 u3rd_X;
    UINT16 u3rd_Y;
} appStampObj_t;

enum {
    STAMP_STAGE_NONE,
    STAMP_STAGE_1ST,
    STAMP_STAGE_2ND,
    STAMP_STAGE_HIDE
};

enum {
    STAMP_CFG_BUF_INIT,
    STAMP_CFG_BUF_FREE,
    STAMP_CFG_SCALE_PARM,
    STAMP_CFG_BLEND_PARM
};

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
tmx_t rtc;

/* date stamp mutex control */
static UINT32 urgentCbCnt = 0;
static UINT32 waitCnt = 0;
static SP5K_MUTEX vidurgmut;

static UINT8 s_recStampInitFlag = 0;
static UINT8 *stampDateBuf = NULL;  /* for string buffer */

static sp5kGfxObj_t stampSrcPic, stampDstPic;
#if (SP5K_3A_INFO_DBG)
static UINT8 *stamp3AInfoBuf = NULL;  /* for 3A info buffer */
static appStampObj_t Stamp1 = {
    .srcBuf     = USE_PIP_LAYER,
    .u1st_X     = 0,
    .u1st_Y     = 0,
    .u1st_W     = 1920,
    .u1st_H     = REC_STAMP_H*2,
    .u2nd_Buf   = NULL,
    .u2nd_X     = 0,
    .u2nd_Y     = 0,
    .u2nd_W     = REC_STAMP_W,
    .u2nd_H     = REC_STAMP_H*2,
    .u3rd_X     = REC_STAMP_X,
    .u3rd_Y     = REC_STAMP_Y,
};
#elif defined(PRJ_17DVR)||defined(PRJ_19DVR)
static appStampObj_t Stamp1 = {
    .srcBuf     = USE_PIP_LAYER,
    .u1st_X     = 0,
    .u1st_Y     = 0,
    .u1st_W     = 1920,
    .u1st_H     = REC_STAMP_H*2,
    .u2nd_Buf   = NULL,
    .u2nd_X     = 0,
    .u2nd_Y     = 0,
    .u2nd_W     = REC_STAMP_W,
    .u2nd_H     = REC_STAMP_H*2,
    .u3rd_X     = REC_STAMP_X,
    .u3rd_Y     = REC_STAMP_Y,
};
#else
static appStampObj_t Stamp1 = {
    .srcBuf     = USE_PIP_LAYER,
    .u1st_X     = 0,
    .u1st_Y     = 0,
    .u1st_W     = 320,
    .u1st_H     = REC_STAMP_H,
    .u2nd_Buf   = NULL,
    .u2nd_X     = 0,
    .u2nd_Y     = 0,
    .u2nd_W     = REC_STAMP_W,
    .u2nd_H     = REC_STAMP_H,
    .u3rd_X     = REC_STAMP_X,
    .u3rd_Y     = REC_STAMP_Y,
};
#endif

#if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
static sp5kGfxObj_t stamp2SrcPic, stamp2DstPic;
static appStampObj_t Stamp2 = {
    .srcBuf     = USE_PIP_LAYER,
    .u1st_X     = 0,
    .u1st_Y     = REC_STAMP_H*2,
    .u1st_W     = 320,
    .u1st_H     = REC_STAMP_H,
    .u2nd_Buf   = NULL,
    .u2nd_X     = 0,
    .u2nd_Y     = 0,
    .u2nd_W     = REC_STAMP_W,
    .u2nd_H     = REC_STAMP_H,
    .u3rd_X     = REC_STAMP_X,
    .u3rd_Y     = REC_STAMP_Y,
};
#endif
#if SP5K_SENSOR_DUAL_MODE
extern UINT8 isSensorResume;
static UINT8 Stamp3_stage = STAMP_STAGE_NONE;
static sp5kGfxObj_t IconSrcPic, IconDstPic;
static appStampObj_t Stamp3_warn = { /* WARNICON=47*47, scale 4x */
    .srcBuf     = USE_PIP_LAYER,
    .u1st_X     = 0,
    .u1st_Y     = REC_STAMP_H*3,
    .u1st_W     = 48,
    .u1st_H     = 48,
    .u2nd_Buf   = NULL,
    .u2nd_X     = 0,
    .u2nd_Y     = 0,
    .u2nd_W     = REC_WARN_W,
    .u2nd_H     = REC_WARN_H,
    .u3rd_X     = 160,
    .u3rd_Y     = 120,
};
#endif
#if (SP5K_VREC_YUV_SAVE)
static UINT8 isYuvGet = FALSE;
static UINT8 vrecStartCount = 0;
static UINT8 vrecYuvCount = 0;
vrecYuvGet_t vrecYuvBuf[YUV_GET_MAX];
#endif

#if GPS_OPEN
extern UINT8 stamp_GPSSpeedBuf[20];
static UINT32 stampPage = 0;
UINT8 GPSledflag = 0;
UINT8 LastFileNotRellyClose=GPS_INIT;/*1:receive msg 2:file close 3:temp buffer write finish*/
#endif

UINT32 RearDateStampCnt = 0; /* ICOM-3690*/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
extern char * appInfoPostionALLStampGet(void);
#endif

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
UINT32 appVideoRecStampCfgSet(UINT32 cfg);

/*--------------------------------------------------------------------------*
* Function name    : appVideoRecStampInitStateGet
* Function		   : Get VideoRec Stamp Init State
* Return value	   : rec Stamp Init Flag (UINT8 init:1/no init:0)
* Parameter1	   : void
* History	       :
*				   : 2018.05.28  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT8
appVideoRecStampInitStateGet(
    void
)
{
    return s_recStampInitFlag;
}

/*--------------------------------------------------------------------------*
* Function name    : appVideoRecStampInitStateSet
* Function		   : Set VideoRec Stamp Init State
* Return value	   : void
* Parameter1	   : set init flag (UINT8 init:1/no init:0)
* History	       :
*				   : 2018.05.28  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appVideoRecStampInitStateSet(
    UINT8 val
)
{
	if((val==0) || (val==1))
    	s_recStampInitFlag = val;
	else
	{
		printf("VideoRec Stamp Init Fail\n");
		s_recStampInitFlag = 0;
	}
}

/*--------------------------------------------------------------------------*
* Function name    : appVideoRecStampCb
* Function		   : Video urgent callback function, it will blend stamp information in YUV frame.
* Return value	   : result (UINT32 SUCCESS/FAIL)
* Parameter1	   : use 0th stream (UINT32 main stream)
* Parameter2	   : frame index (UINT32)
* Parameter3	   : YUV frame point (frameBufInfo_t* YUV frame address)
* Parameter4	   : duplicated buffer address (UINT8*)
* History	       :
*				   : 2018.05.28  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32
appVideoRecStampCb(
    UINT32 streamId,        /* nth stream, from 0 */
    UINT32 fid,             /* frame id */
    frameBufInfo_t *pframe, /* buffer information */
    UINT8 *pbufdup          /* duplicated buffer address */
)
{
    HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "vrec: cb frame %d %d", pframe->width, pframe->height);
#if CDSP_SKIP_DETECTION
    static void *lastframe = NULL;

    if (lastframe == pframe->pbuf) {
        UINT32 elsTime;
        sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_ELAPSED_TIME, &elsTime);
        #if SP5K_USB_UVC_SUPPORT
        if (pViewParam->videoStamp == UI_DATESTAMP_OFF) {
            return FAIL;
        }
        #else
        printf("\n\n[host][warning] The same main buffer comes again @ %dms\nPlease check dram bandwidth!\n", elsTime);
        #endif
    }
    lastframe = pframe->pbuf;
#endif

    /* date stamp mutex control */
    if (urgentCbCnt > waitCnt) {
        sp5kOsMutexGet(&vidurgmut, TX_WAIT_FOREVER);
        urgentCbCnt--;
        sp5kOsMutexPut(&vidurgmut);
    }

    if (pViewParam->videoStamp == UI_DATESTAMP_OFF) {
    }
    else {
        HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "vrec: appVideoRecStampCb %s", stampDateBuf);

        stampSrcPic.pbuf = Stamp1.u2nd_Buf;
        stampDstPic.pbuf = pframe->pbuf;
        stampDstPic.bufW = pframe->width;
        stampDstPic.bufH = pframe->height;
		UINT32 x = stampDstPic.roiX;
		UINT32 y = stampDstPic.roiY;
		stampDstPic.roiX += (pframe->roiX & ~1);
		stampDstPic.roiY += pframe->roiY;
        sp5kGfxObjectBlend(&stampSrcPic, &stampDstPic, &stampDstPic, BLEND_ALPHA);
		stampDstPic.roiX = x;
		stampDstPic.roiY = y;
        #if SIMPLE_WRITE_YUV
        fsSimpleWrite("D:\\Result_1st.YUV", stampDstPic.pbuf, stampDstPic.bufW * stampDstPic.bufH * 2);
        #endif

        /* Jerry add for YUV save */
#if (SP5K_VREC_YUV_SAVE)
        //static UINT8 i;
        if (isYuvGet == FALSE) {
        }
        else {
            if (vrecYuvCount >= YUV_GET_MAX) {
                return;
            }
			static UINT8 i;
            vrecYuvBuf[vrecYuvCount].width = pframe->width;
            vrecYuvBuf[vrecYuvCount].height = pframe->height;
            memcpy(vrecYuvBuf[vrecYuvCount].pBuf, pframe->pbuf, vrecYuvBuf[vrecYuvCount].width * vrecYuvBuf[vrecYuvCount].height * 2);

            vrecYuvCount++;
            i++;
            if (i >= 10) {
                i = 0;
                isYuvGet = FALSE;
            }
        }
#endif /* SP5K_VREC_YUV_SAVE */
    }

    /* date stamp mutex control */
    sp5kOsMutexGet(&vidurgmut, TX_WAIT_FOREVER);
    urgentCbCnt += 1;
    sp5kOsMutexPut(&vidurgmut);

    return SUCCESS;
}

/*--------------------------------------------------------------------------*
* Function name    : appVideoRecStampDispCb
* Function		   : Video urgent callback function, it will blend stamp information in YUV frame.
* Return value	   : result (UINT32 SUCCESS/FAIL)
* Parameter1	   : use 1th stream (UINT32 second stream)
* Parameter2	   : frame index (UINT32)
* Parameter3	   : YUV frame point (frameBufInfo_t* YUV frame address)
* Parameter4	   : duplicated buffer address (UINT8*)

* History	       :
*				   : 2018.05.28  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32
appVideoRecStampDispCb(
    UINT32 streamId,        /* nth stream, from 0 */
    UINT32 fid,             /* frame id */
    frameBufInfo_t *pframe, /* buffer information */
    UINT8 *pbufdup          /* duplicated buffer address */
)
{
    HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "vrec: cb frame %d %d", pframe->width, pframe->height);
    if (pViewParam->videoStamp == UI_DATESTAMP_OFF) {
        return FAIL;
    }
    else {
        HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "vrec: appVideoRecStampCb %s", stampDateBuf);

        stampSrcPic.pbuf = Stamp1.u2nd_Buf;
        stampDstPic.pbuf = pframe->pbuf;
        stampDstPic.bufW = pframe->width;
        stampDstPic.bufH = pframe->height;
		UINT32 x = stampDstPic.roiX;
		UINT32 y = stampDstPic.roiY;
		stampDstPic.roiX += (pframe->roiX & ~1);
		stampDstPic.roiY += pframe->roiY;
        sp5kGfxObjectBlend(&stampSrcPic, &stampDstPic, &stampDstPic, BLEND_ALPHA);
		stampDstPic.roiX = x;
		stampDstPic.roiY = y;
        #if SIMPLE_WRITE_YUV
        fsSimpleWrite("D:\\Result_disp.YUV", stampDstPic.pbuf, stampDstPic.bufW * stampDstPic.bufH * 2);
        #endif
    }

    return SUCCESS;
}
#if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE

/*--------------------------------------------------------------------------*
* Function name    : appVideoRecStampDispCb
* Function		   : (Second sensor used)Video urgent callback function, it will blend stamp information in YUV frame.
* Return value	   : result (UINT32 SUCCESS/FAIL)
* Parameter1	   : use 1th stream (UINT32 second stream)
* Parameter2	   : frame index (UINT32)
* Parameter3	   : YUV frame point (frameBufInfo_t* YUV frame address)
* Parameter4	   : duplicated buffer address (UINT8*)

* History	       :
*				   : 2018.05.28  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32
appVideoRecStampCbSub(
    UINT32 streamId,        /* nth stream, from 0 */
    UINT32 fid,             /* frame id */
    frameBufInfo_t *pframe, /* buffer information */
    UINT8 *pbufdup          /* duplicated buffer address */
)
{
    HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "vrec: cb frame %d %d", pframe->width, pframe->height);
#if CDSP_SKIP_DETECTION
    static void *lastframe = NULL;
    if (lastframe == pframe->pbuf) {
        UINT32 elsTime;
        sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_ELAPSED_TIME, &elsTime);
        printf("\n\n[host][warning] The same 2nd buffer comes again @ %dms\nPlease check dram bandwidth!\n", elsTime);
    }
    lastframe = pframe->pbuf;
#endif
#if SP5K_SENSOR_DUAL_MODE
    if (Stamp3_stage == STAMP_STAGE_2ND && (appDualPipModeGet() >= DUALSEN_SINGLE_SEC)) {
        IconSrcPic.pbuf = Stamp3_warn.u2nd_Buf;
        IconDstPic.pbuf = pframe->pbuf;
        IconDstPic.bufW = pframe->width;
        IconDstPic.bufH = pframe->height;
        sp5kGfxObjectBlend(&IconSrcPic, &IconDstPic, &IconDstPic, BLEND_ALPHA);
        #if SIMPLE_WRITE_YUV
        fsSimpleWrite("D:\\Result_icon.YUV", IconDstPic.pbuf, IconDstPic.bufW * IconDstPic.bufH * 2);
        #endif
    }
#endif
    if (pViewParam->videoStamp != UI_DATESTAMP_OFF) {
        HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "vrec: appVideoRecStampCb %s", stampDateBuf);
        #if SP5K_SENSOR_DUAL_MODE || SP5K_USB_UVC_SUPPORT
        stamp2SrcPic.pbuf = Stamp2.u2nd_Buf;
        #else
        stamp2SrcPic.pbuf = Stamp1.u2nd_Buf;
        #endif
        stamp2DstPic.pbuf = pframe->pbuf;
        stamp2DstPic.bufW = pframe->width;
        stamp2DstPic.bufH = pframe->height;
        sp5kGfxObjectBlend(&stamp2SrcPic, &stamp2DstPic, &stamp2DstPic, BLEND_ALPHA);
        #if SIMPLE_WRITE_YUV
        fsSimpleWrite("D:\\Result_2nd.YUV", stamp2DstPic.pbuf, stamp2DstPic.bufW * stamp2DstPic.bufH * 2);
        #endif
    }
    return SUCCESS;
}
#endif /* SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE */

/*--------------------------------------------------------------------------*
* Function name    : appVideoRecStampCfgSet
* Function		   : Video Rec Stamp Config Set
* Return value	   : result (UINT32 SUCCESS/FAIL)
* Parameter1	   : Config (UINT32 STAMP_CFG_BUF_INIT/STAMP_CFG_BUF_FREE/STAMP_CFG_BLEND_PARM/STAMP_CFG_SCALE_PARM)
* History	       :
*				   : 2018.05.28  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32
appVideoRecStampCfgSet(
    UINT32 cfg
)
{
    /* for scale */
    static sp5kGfxPageCapabDesc_t rec;
    static sp5kGfxObj_t gfxObjSrc, gfxObjDst;

    switch (cfg) {
    case STAMP_CFG_BUF_INIT:
        sp5kGfxPageCapabilityGet(SP5K_GFX_PAGE_PIP_0, &rec);
        printf("PIP_0 SIZE = (%d,%d)\n", rec.frmW, rec.frmH);

        if (stampDateBuf == NULL) {
            stampDateBuf = malloc(STRING_SIZE);
            memset(stampDateBuf, 0, STRING_SIZE);
        }

        if (Stamp1.u2nd_Buf== NULL) {
            Stamp1.u2nd_Buf = sp5kYuvBufferAlloc(Stamp1.u2nd_W, Stamp1.u2nd_H);
        }

        if (Stamp1.u2nd_Buf == NULL || stampDateBuf == NULL) {
            HOST_PROF_LOG_PRINT(LEVEL_ERROR, "vrec: Stamp buf init Fail");
            HOST_ASSERT(0);
        }

#if (SP5K_3A_INFO_DBG)
        if (stamp3AInfoBuf == NULL) {
            stamp3AInfoBuf = malloc(STRING_SIZE);
            memset(stamp3AInfoBuf, 0, STRING_SIZE);
        }

        if (stamp3AInfoBuf == NULL) {
            HOST_PROF_LOG_PRINT(LEVEL_ERROR, "vrec: Stamp 3A info buf init Fail");
            HOST_ASSERT(0);
        }
#endif
#if SP5K_SENSOR_DUAL_MODE ||SP5K_USB_UVC_SUPPORT
        if (Stamp2.u2nd_Buf == NULL) {
            Stamp2.u2nd_Buf = sp5kYuvBufferAlloc(Stamp2.u2nd_W, Stamp2.u2nd_H);
            if (Stamp2.u2nd_Buf == NULL ) {
                HOST_PROF_LOG_PRINT(LEVEL_ERROR, "vrec: dual mode buf init Fail");
                HOST_ASSERT(0);
            }
        }
#if SP5K_SENSOR_DUAL_MODE
        if (Stamp3_warn.u2nd_Buf == NULL) {
            Stamp3_warn.u2nd_Buf = sp5kYuvBufferAlloc(Stamp3_warn.u2nd_W, Stamp3_warn.u2nd_H);
            if (Stamp3_warn.u2nd_Buf == NULL) {
                HOST_PROF_LOG_PRINT(LEVEL_ERROR, "vrec: dual mode icon buf init Fail");
                HOST_ASSERT(0);
            }
        }
#endif
#endif
        break;

    case STAMP_CFG_BUF_FREE:
        if (stampDateBuf != NULL) {
            free(stampDateBuf);
            stampDateBuf = NULL;
        }

        if (Stamp1.u2nd_Buf != NULL) {
            sp5kYuvBufferFree(Stamp1.u2nd_Buf);
            Stamp1.u2nd_Buf = NULL;
        }

#if (SP5K_3A_INFO_DBG)
        if (stamp3AInfoBuf != NULL) {
            free(stamp3AInfoBuf);
            stamp3AInfoBuf = NULL;
        }
#endif
#if SP5K_SENSOR_DUAL_MODE|| SP5K_USB_UVC_SUPPORT
        if (Stamp2.u2nd_Buf != NULL) {
            sp5kYuvBufferFree(Stamp2.u2nd_Buf);
            Stamp2.u2nd_Buf = NULL;
            RearDateStampCnt = 0; /* ICOM-3690*/
        }
#if SP5K_SENSOR_DUAL_MODE
        if (Stamp3_warn.u2nd_Buf != NULL) {
            sp5kYuvBufferFree(Stamp3_warn.u2nd_Buf);
            Stamp3_warn.u2nd_Buf = NULL;
        }
#endif
#endif
        break;

    case STAMP_CFG_BLEND_PARM:
        stampSrcPic.pbuf = Stamp1.u2nd_Buf;
        stampSrcPic.bufW = Stamp1.u2nd_W;
        stampSrcPic.bufH = Stamp1.u2nd_H;
        stampSrcPic.fmt  = stampDstPic.fmt  = SP5K_GFX_FMT_YUV422;
        stampSrcPic.roiX = Stamp1.u2nd_X;
        stampSrcPic.roiY = Stamp1.u2nd_Y;
        stampDstPic.roiX = Stamp1.u3rd_X;
        stampDstPic.roiY = Stamp1.u3rd_Y;
        stampSrcPic.roiW = stampDstPic.roiW = Stamp1.u2nd_W/2;
        stampSrcPic.roiH = stampDstPic.roiH = Stamp1.u2nd_H;
#if SP5K_SENSOR_DUAL_MODE || SP5K_USB_UVC_SUPPORT
        stamp2SrcPic.pbuf = Stamp2.u2nd_Buf;
        stamp2SrcPic.bufW = Stamp2.u2nd_W;
        stamp2SrcPic.bufH = Stamp2.u2nd_H;
        stamp2SrcPic.fmt  = stamp2DstPic.fmt  = SP5K_GFX_FMT_YUV422;
        stamp2SrcPic.roiX = Stamp2.u2nd_X;
        stamp2SrcPic.roiY = Stamp2.u2nd_Y;
        stamp2DstPic.roiX = Stamp2.u3rd_X;
        stamp2DstPic.roiY = Stamp2.u3rd_Y;
        stamp2SrcPic.roiW = stamp2DstPic.roiW = Stamp2.u2nd_W;
        stamp2SrcPic.roiH = stamp2DstPic.roiH = Stamp2.u2nd_H;
#if SP5K_SENSOR_DUAL_MODE
        IconSrcPic.pbuf = Stamp3_warn.u2nd_Buf;
        IconSrcPic.bufW = Stamp3_warn.u2nd_W;
        IconSrcPic.bufH = Stamp3_warn.u2nd_H;
        IconSrcPic.fmt  = IconDstPic.fmt  = SP5K_GFX_FMT_YUV422;
        IconSrcPic.roiX = Stamp3_warn.u2nd_X;
        IconSrcPic.roiY = Stamp3_warn.u2nd_Y;
        IconDstPic.roiX = Stamp3_warn.u3rd_X;
        IconDstPic.roiY = Stamp3_warn.u3rd_Y;
        IconSrcPic.roiW = IconDstPic.roiW = Stamp3_warn.u2nd_W;
        IconSrcPic.roiH = IconDstPic.roiH = Stamp3_warn.u2nd_H;
#endif
#endif
        break;

    case STAMP_CFG_SCALE_PARM:
        sp5kGfxPageCapabilityGet(SP5K_GFX_PAGE_PIP_0, &rec);
        HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "vrec: cb rec %d %d", rec.frmW, rec.frmH);

        /* String 1 convert and scale */
        {
            gfxObjSrc.pbuf = rec.pbuf;
            gfxObjSrc.bufW = rec.frmW;
            gfxObjSrc.bufH = rec.frmH;
            gfxObjSrc.roiX = Stamp1.u1st_X;
            gfxObjSrc.roiY = Stamp1.u1st_Y;
            gfxObjSrc.roiW = (rec.frmW > Stamp1.u1st_W) ? Stamp1.u1st_W : rec.frmW; /* PIP just 320X240 */
            gfxObjSrc.roiH = Stamp1.u1st_H;
            gfxObjSrc.fmt  = SP5K_GFX_FMT_RGB565;
            gfxObjDst.pbuf = Stamp1.u2nd_Buf;
            gfxObjDst.bufW = Stamp1.u2nd_W;
            gfxObjDst.bufH = Stamp1.u2nd_H;
            gfxObjDst.roiX = Stamp1.u2nd_X;
            gfxObjDst.roiY = Stamp1.u2nd_Y;
            gfxObjDst.roiW = Stamp1.u2nd_W/2;
            gfxObjDst.roiH = Stamp1.u2nd_H;
            gfxObjDst.fmt  = SP5K_GFX_FMT_YUV422;
            sp5kGfxObjectScale(&gfxObjSrc, &gfxObjDst);
            #if SAVE_RGB_YUV
            fsSimpleWrite("D:\\Stamp1.RGB", gfxObjSrc.pbuf, gfxObjSrc.bufW * gfxObjSrc.bufH * 2);
            fsSimpleWrite("D:\\Stamp1.YUV", gfxObjDst.pbuf, gfxObjDst.bufW * gfxObjDst.bufH * 2);
            #endif
        }
#if SP5K_SENSOR_DUAL_MODE || SP5K_USB_UVC_SUPPORT
        {
            gfxObjSrc.pbuf = rec.pbuf;
            gfxObjSrc.bufW = rec.frmW;
            gfxObjSrc.bufH = rec.frmH;
            gfxObjSrc.roiX = Stamp2.u1st_X;
            gfxObjSrc.roiY = Stamp2.u1st_Y;
            gfxObjSrc.roiW = (rec.frmW > Stamp2.u1st_W) ? Stamp2.u1st_W : rec.frmW; /* PIP just 320X240 */
            gfxObjSrc.roiH = Stamp2.u1st_H;
            gfxObjSrc.fmt  = SP5K_GFX_FMT_RGB565;
            gfxObjDst.pbuf = Stamp2.u2nd_Buf;
            gfxObjDst.bufW = Stamp2.u2nd_W;
            gfxObjDst.bufH = Stamp2.u2nd_H;
            gfxObjDst.roiX = Stamp2.u2nd_X;
            gfxObjDst.roiY = Stamp2.u2nd_Y;
            gfxObjDst.roiW = Stamp2.u2nd_W;
            gfxObjDst.roiH = Stamp2.u2nd_H;
            gfxObjDst.fmt  = SP5K_GFX_FMT_YUV422;
            sp5kGfxObjectScale(&gfxObjSrc, &gfxObjDst);
            #if SAVE_RGB_YUV
            fsSimpleWrite("D:\\Stamp2.RGB", gfxObjSrc.pbuf, gfxObjSrc.bufW * gfxObjSrc.bufH * 2);
            fsSimpleWrite("D:\\Stamp2.YUV", gfxObjDst.pbuf, gfxObjDst.bufW * gfxObjDst.bufH * 2);
            #endif
        }
#if SP5K_SENSOR_DUAL_MODE
        {
            if (Stamp3_stage == STAMP_STAGE_1ST && (appDualPipModeGet() >= DUALSEN_SINGLE_SEC)) {
                gfxObjSrc.pbuf = rec.pbuf;
                gfxObjSrc.bufW = rec.frmW;
                gfxObjSrc.bufH = rec.frmH;
                gfxObjSrc.roiX = Stamp3_warn.u1st_X;
                gfxObjSrc.roiY = Stamp3_warn.u1st_Y;
                gfxObjSrc.roiW = (rec.frmW > Stamp3_warn.u1st_W) ? Stamp3_warn.u1st_W : rec.frmW; /* PIP just 320X240 */
                gfxObjSrc.roiH = Stamp3_warn.u1st_H;
                gfxObjSrc.fmt  = SP5K_GFX_FMT_RGB565;
                gfxObjDst.pbuf = Stamp3_warn.u2nd_Buf;
                gfxObjDst.bufW = Stamp3_warn.u2nd_W;
                gfxObjDst.bufH = Stamp3_warn.u2nd_H;
                gfxObjDst.roiX = Stamp3_warn.u2nd_X;
                gfxObjDst.roiY = Stamp3_warn.u2nd_Y;
                gfxObjDst.roiW = Stamp3_warn.u2nd_W;
                gfxObjDst.roiH = Stamp3_warn.u2nd_H;
                gfxObjDst.fmt = SP5K_GFX_FMT_YUV422;
                sp5kGfxObjectScale(&gfxObjSrc, &gfxObjDst);
                Stamp3_stage = STAMP_STAGE_2ND;
                #if SAVE_RGB_YUV
                fsSimpleWrite("D:\\Stamp3.RGB", gfxObjSrc.pbuf, gfxObjSrc.bufW * gfxObjSrc.bufH * 2);
                fsSimpleWrite("D:\\Stamp3.YUV", gfxObjDst.pbuf, gfxObjDst.bufW * gfxObjDst.bufH * 2);
                #endif
            }
        }
#endif
#endif
        break;

	default:
		return FAIL;
    }
    return SUCCESS;
}

#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
extern char *appInfoPostionALLStampGet(void);
#endif

/*--------------------------------------------------------------------------*
* Function name    : appVideoRecStampStringGet
* Function		   : Get Stamp String
* Return value	   : void
* Parameter1	   : void
* History	       :
*				   : 2018.05.28  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appVideoRecStampStringGet(
    void
)
{
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
    int i;
#endif

    if (!stampDateBuf) {
        appVideoRecStampInit(SP5K_MODE_VIDEO_RECORD, REC_STAMP_X, REC_STAMP_Y);
    }

    sp5kRtcDateTimeGet(SP5K_DATE_TIME_OPTION, &rtc);

    if (pViewParam->videoStamp == UI_DATESTAMP_DATE) {
        switch (pUiSetting->dateTime) {
        case DATE_TIME_MMDDYY:
            sprintf((char *)stampDateBuf, "%02d/%02d/%d", rtc.tmx_mon, rtc.tmx_mday, rtc.tmx_year+1900);
            break;
        case DATE_TIME_DDMMYY:
            sprintf((char *)stampDateBuf, "%02d/%02d/%d", rtc.tmx_mday, rtc.tmx_mon, rtc.tmx_year+1900);
            break;
        case DATE_TIME_YYMMDD:
        default:
            sprintf((char *)stampDateBuf, "%d/%02d/%02d", rtc.tmx_year+1900, rtc.tmx_mon, rtc.tmx_mday);
            break;
        }
    }
    else if (pViewParam->videoStamp == UI_DATESTAMP_DATETIME) {
        switch (pUiSetting->dateTime) {
        case DATE_TIME_MMDDYY:
            sprintf((char *)stampDateBuf, "%02d/%02d/%d  %02d:%02d:%02d",
                rtc.tmx_mon, rtc.tmx_mday, rtc.tmx_year+1900,
                rtc.tmx_hour, rtc.tmx_min, rtc.tmx_sec);
            break;
        case DATE_TIME_DDMMYY:
            sprintf((char *)stampDateBuf, "%02d/%02d/%d  %02d:%02d:%02d",
               rtc.tmx_mday, rtc.tmx_mon, rtc.tmx_year+1900,
               rtc.tmx_hour, rtc.tmx_min, rtc.tmx_sec);
            break;
        case DATE_TIME_YYMMDD:
        default:
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
            i = snprintf((char *)stampDateBuf, STRING_SIZE,
                    "%d/%02d/%02d %02d:%02d:%02d ",
                    rtc.tmx_year+1900, rtc.tmx_mon, rtc.tmx_mday,
                    rtc.tmx_hour, rtc.tmx_min, rtc.tmx_sec);
            strncat((char *) &stampDateBuf[i], appInfoPostionALLStampGet(),
                    STRING_SIZE - i);
#else
            sprintf((char *)stampDateBuf,  "%d/%02d/%02d  %02d:%02d:%02d",
              rtc.tmx_year+1900, rtc.tmx_mon, rtc.tmx_mday,
              rtc.tmx_hour, rtc.tmx_min, rtc.tmx_sec);
#endif
            break;
        }
    }

    /* Jerry add for 3A info debug */
#if (SP5K_3A_INFO_DBG==1)
    UINT32 clrtmp, ctIdx, isoIdx, agcidx;
    SINT32 expidx;
    AEStatus_t AEStatus;

    appAWBALG_WBParamGet(AWB_PARA_CLRTMP, &clrtmp);

    /* Get Exp & Agc index */
    appAEPreviewExpAgcGet(&expidx, &agcidx);
    appAEALG_ParamGet(AE_PARA_STATUS, (SINT32*)&AEStatus);

    /* Get CT index */
    sp5kDynIqCfgGet(SP5K_DYN_IQ_CFG_CT_IDX_GET, &ctIdx);

    /* Get ISO index */
    sp5kDynIqCfgGet(SP5K_DYN_IQ_CFG_ISO_IDX_GET, &isoIdx);

    sprintf((char *)stamp3AInfoBuf, "%u/%u/%u/%u/%u/%u", AEStatus.LVx1000, clrtmp, expidx, agcidx, ctIdx, isoIdx);
#elif (SP5K_3A_INFO_DBG==2)
    AEStatus_t AEStatus;
    appAEALG_ParamGet(AE_PARA_STATUS, (SINT32*)&AEStatus);

    sprintf((char *)stamp3AInfoBuf, "%d/%d/%d/%d/%d/%d", AEStatus.BLStatus, AEStatus.BLEVx1000Diff, AEStatus.DeltaEVx1000,
        AEStatus.UpLVx1000, AEStatus.UpY, AEStatus.DownY);
#endif /* SP5K_3A_INFO_DBG */
}

/*--------------------------------------------------------------------------*
* Function name    : appVideoRecStampUpdate
* Function		   : Stamp Update
* Return value	   : void
* Parameter1	   : void
* History	       :
*				   : 2018.05.28  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appVideoRecStampUpdate(
    void
)
{
    BOOL bDoStamp = FALSE;

#if GPS_OPEN
    UINT8 vtagstr[80];
    UINT8 buf[6];

    memset(vtagstr, 0, sizeof(vtagstr));
    memset(buf, 0, sizeof(buf));
    memset(buf, 0x20, 3);
    strcat(vtagstr, buf);

    if (pUiSetting->GPSSTATUS == 1) {
        strcat(vtagstr, stamp_GPSSpeedBuf);
    }
#endif

    if (pViewParam->videoStamp)
    {
        appVideoRecStampStringGet();

        if (stampDateBuf != NULL && Stamp1.u2nd_Buf != NULL) {
            sp5kGfxAttrSet(SP5K_GFX_FONT_ID, SP5K_GFX_STAMP, 0, 0, 0);
            sp5kGfxAttrSet(SP5K_GFX_REFRESH_ACTIVE, 0, 0, 0, 0);
            sp5kGfxPageClear(SP5K_GFX_PAGE_PIP_0, PAL_NOCOLOR);
            appOsd_ColorDrawTextColorSet(PAL_RED, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		 	sp5kGfxAttrSet(SP5K_GFX_RGB565_PIC_SCALE_FACTOR, 1500, 1500, 0, 0);
#else
            sp5kGfxAttrSet(SP5K_GFX_RGB565_PIC_SCALE_FACTOR, 1000, 1000, 0, 0);
#endif
            appOsdLib_TextPIPDraw(Stamp1.u1st_X, Stamp1.u1st_Y, SP5K_GFX_ALIGN_TOP_LEFT, stampDateBuf);

            RearDateStampCnt ++; /* ICOM-3690*/
#if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
            if (Stamp2.u2nd_Buf != NULL && RearDateStampCnt > 2) { /* ICOM-3690*/
                sp5kGfxAttrSet(SP5K_GFX_RGB565_PIC_SCALE_FACTOR, 1000, 1000, 0, 0);
                appOsdLib_TextPIPDraw(Stamp2.u1st_X, Stamp2.u1st_Y, SP5K_GFX_ALIGN_TOP_LEFT, stampDateBuf);
            }
#endif
        }

#if (SP5K_3A_INFO_DBG)
        if (stamp3AInfoBuf != NULL && Stamp1.u2nd_Buf != NULL) {
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		 	sp5kGfxAttrSet(SP5K_GFX_RGB565_PIC_SCALE_FACTOR, 1500, 1500, 0, 0);
#else
            sp5kGfxAttrSet(SP5K_GFX_RGB565_PIC_SCALE_FACTOR, 1000, 1000, 0, 0);
#endif
            appOsdLib_TextPIPDraw(Stamp1.u1st_X, Stamp1.u1st_Y+REC_STAMP_H, SP5K_GFX_ALIGN_TOP_LEFT, stamp3AInfoBuf);
        }
#endif
        bDoStamp = TRUE;
    }
#if SP5K_SENSOR_DUAL_MODE
    if (appDualPipModeGet() >= DUALSEN_SINGLE_SEC) {
        if (isSensorResume == FALSE) {
            if (Stamp3_stage == STAMP_STAGE_NONE) {
                sp5kGfxAttrSet(SP5K_GFX_REFRESH_ACTIVE, 0, 0, 0, 0);
                sp5kGfxAttrSet(SP5K_GFX_RGB565_PIC_SCALE_FACTOR, 1000, 1000, 0, 0);
                appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, Stamp3_warn.u1st_X, Stamp3_warn.u1st_Y, SP5K_GFX_ALIGN_TOP_LEFT,
                    (UINT8 *)ID_WARNICON, 0x0000);  /* ID_WARNICON JUST 47*37 */
                bDoStamp = TRUE;
                Stamp3_stage = STAMP_STAGE_1ST;
            }
            else {
                Stamp3_stage = STAMP_STAGE_2ND;
            }
        }
        else if (Stamp3_stage == STAMP_STAGE_2ND) {
            Stamp3_stage = STAMP_STAGE_HIDE;
        }
    }
#endif

    if (bDoStamp) {
        sp5kGfxAttrSet(SP5K_GFX_RGB565_PIC_SCALE_FACTOR, 1000, 1000, 0, 0);
        appVideoRecStampCfgSet(STAMP_CFG_SCALE_PARM);
        sp5kGfxPageClear(SP5K_GFX_PAGE_PIP_0, PAL_NOCOLOR);
        sp5kGfxAttrSet(SP5K_GFX_REFRESH_ACTIVE, 1, 0, 1, 0);
    }
}

/*--------------------------------------------------------------------------*
* Function name    : appVideoRecStampCbReg
* Function		   : VideoRec Stamp Register
* Return value	   : result (UINT32 SUCCESS/FAIL)
* Parameter1	   : mode (UINT32 SP5K_MODE_VIDEO_RECORD)
* History	       :
*				   : 2018.05.28  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32
appVideoRecStampCbReg(
    UINT32 mode
)
{
    /* date stamp mutex control */
    sp5kOsMutexCreate(&vidurgmut, "vidurgmut", 1);

    sp5kGfxSysCfgSet(SP5K_GFX_SYS_CFG_SCALE_QUALITY, SP5K_GFX_SCALE_QUALITY_FAST, 0, 0, 0);
    appVideoRecStampCfgSet(STAMP_CFG_BUF_INIT);
    appVideoRecStampCfgSet(STAMP_CFG_BLEND_PARM);
    appVideoRecStampUpdate();
#if SP5K_USB_UVC_SUPPORT
    appUrgentSet(URGENT_STAMP_DUAL, ENABLE);
#elif SP5K_SENSOR_DUAL_MODE
    UINT32 ret = appDualPipModeGet();
    switch (ret) {
    case DUALSEN_SINGLE_MAIN:
        appUrgentSet(URGENT_STAMP_MAIN, ENABLE);
        break;
    case DUALSEN_SINGLE_SEC:
        appUrgentSet(URGENT_STAMP_SEC, ENABLE);
        break;
    case DUALSEN_PIP_MAIN:
    case DUALSEN_PIP_SEC:
        appUrgentSet(URGENT_STAMP_DUAL, ENABLE);
        break;
	default:
		return FAIL;
    }
#else
    appUrgentSet(URGENT_STAMP_MAIN, ENABLE);
#endif

    return SUCCESS;
}

/*--------------------------------------------------------------------------*
* Function name    : appVideoRecStampCbUnReg
* Function		   : VideoRec Stamp Unregister
* Return value	   : result (UINT32 SUCCESS/FAIL)
* Parameter1	   : void
* History	       :
*				   : 2018.05.28  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32
appVideoRecStampCbUnReg(
    void
)
{
#if SP5K_USB_UVC_SUPPORT
    appUrgentSet(URGENT_STAMP_DUAL, DISABLE);
#elif SP5K_SENSOR_DUAL_MODE
    UINT32 ret = appDualPipModeGet();
    switch (ret) {
    case DUALSEN_SINGLE_MAIN:
        appUrgentSet(URGENT_STAMP_MAIN, DISABLE);
        break;
    case DUALSEN_SINGLE_SEC:
        appUrgentSet(URGENT_STAMP_SEC, DISABLE);
        break;
    case DUALSEN_PIP_MAIN:
    case DUALSEN_PIP_SEC:
        appUrgentSet(URGENT_STAMP_DUAL, DISABLE);
        break;
	default:
		return FAIL;
    }
#else
    appUrgentSet(URGENT_STAMP_MAIN, DISABLE);
#endif
    appVideoRecStampCfgSet(STAMP_CFG_BUF_FREE);

    /* date stamp mutex control */
    if (urgentCbCnt > waitCnt) {
        sp5kOsMutexGet(&vidurgmut, TX_WAIT_FOREVER);
        urgentCbCnt--;
        sp5kOsMutexPut(&vidurgmut);
    }
    sp5kOsMutexDelete(&vidurgmut);

    return SUCCESS;
}

/*--------------------------------------------------------------------------*
* Function name    : appVideoRecStampInit
* Function		   : VideoRec Stamp Init
* Return value	   : void
* Parameter1	   : mode (UINT32 SP5K_MODE_VIDEO_RECORD)
* Parameter2	   : set roi x (UINT32 <bufW)
* Parameter3	   : set roi y (UINT32 <bufH)
* History	       :
*				   : 2018.05.28  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appVideoRecStampInit(
    UINT32 mode,
    UINT32 roix,
    UINT32 roiy
)
{
#if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
    UINT32 w, h;
    sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_2ND_WIDTH, &w);
    sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_2ND_HEIGHT, &h);
    if (w == 1920) {
        Stamp2.u2nd_W = Stamp2.u1st_W<<1;
        Stamp2.u2nd_H = Stamp2.u1st_H<<1;
#if SP5K_SENSOR_DUAL_MODE
        Stamp3_warn.u2nd_W = Stamp3_warn.u1st_W*3;
        Stamp3_warn.u2nd_H = Stamp3_warn.u1st_H*3;
#endif
    }
    else {
#if defined(HW_EVB_DSC13003) || defined(HW_EVB_DSC18001A)
        Stamp2.u3rd_Y = 50;	/*DON'T KNOW WHY*/
#else
        Stamp2.u3rd_Y = 4;
#endif
        Stamp2.u2nd_W = Stamp2.u1st_W;
        Stamp2.u2nd_H = Stamp2.u1st_H;
#if SP5K_SENSOR_DUAL_MODE
        Stamp3_warn.u2nd_W = Stamp3_warn.u1st_W*2;
        Stamp3_warn.u2nd_H = Stamp3_warn.u1st_H*2;
#endif
    }
#if SP5K_SENSOR_DUAL_MODE
    Stamp3_warn.u3rd_X = (w>>1)-(Stamp3_warn.u2nd_W>>1);
    Stamp3_warn.u3rd_Y = (h>>1)-(Stamp3_warn.u2nd_H>>1);
    Stamp3_stage = STAMP_STAGE_NONE;
#endif
#endif
    {
	Stamp1.u3rd_X = roix;
	Stamp1.u3rd_Y = roiy;

	/* datestamp setting */
	appOsd_GfxFontIDSet(SP5K_GFX_STAMP);

	/* for v35 video rec stamp */
	appVideoRecStampCbReg(mode);
    }
}

/*--------------------------------------------------------------------------*
* Function name    : appVideoRecStampExit
* Function		   : VideoRec Stamp Exit
* Return value	   : void
* Parameter1	   : void
* History	       :
*				   : 2018.05.28  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appVideoRecStampExit(
    void
)
{
    if (pViewParam->videoStamp) {
        appOsd_GfxFontIDSet(SP5K_GFX_FONT_ID_1);
    }
}

#if (SP5K_VREC_YUV_SAVE)

/*--------------------------------------------------------------------------*
* Function name    : appVideoYuvSaveStart
* Function		   : Init YuvSave Buffer in VideoRec Start
* Return value	   : vlcSize (UINT32)
* Parameter1	   : stream index (UINT32 SUCCESS/FAIL)
* History	       :
*				   : 2018.05.28  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appVideoYuvSaveStart(
    void
)
{
    UINT32 i, w, h;

    sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_WIDTH, &w);
    sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_HEIGHT, &h);

    for (i = 0; i < YUV_GET_MAX; i++) {
        if (vrecYuvBuf[i].pBuf == NULL) {
            vrecYuvBuf[i].pBuf = sp5kYuvBufferAlloc(w, h);
        }
    }

    vrecStartCount++;
    vrecYuvCount = 0;
    isYuvGet = FALSE;
}

/*--------------------------------------------------------------------------*
* Function name    : appVideoYuvSaveStop
* Function		   : Free YuvSave Buffer in VideoRec Stop
* Return value	   : void
* Parameter1	   : void
* History	       :
*				   : 2018.05.28  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appVideoYuvSaveStop(
    void
)
{
    UINT8 fname[32], i;
    UINT32 time_t0 = 0, time_t1 = 0;

    for (i = 0; i < vrecYuvCount; i++) {
        if (vrecYuvBuf[i].pBuf != NULL) {
            sprintf((char *)fname, "D:\\TEST%d_%05d.YUV", vrecStartCount, i);
            fsSimpleWrite( fname, vrecYuvBuf[i].pBuf, vrecYuvBuf[i].width * vrecYuvBuf[i].height * 2 );
            sp5kYuvBufferFree(vrecYuvBuf[i].pBuf);
            vrecYuvBuf[i].pBuf = NULL;
        }
    }

    time_t0 = appMsTimeGet();
    time_t1 = appMsTimeGet() - time_t0;
    if (time_t1 < 2000) {
        sp5kTimeDelay(SP5K_TIME_DELAY_1MS, (2000 - time_t1));
    }
}

/*--------------------------------------------------------------------------*
* Function name    : appVideoYuvFlagSet
* Function		   : If flag enable, will save YUV.
* Return value	   : void
* Parameter1	   : enable (UINT32 ENALBE:save YUV/DISABLE)
* History	       :
*				   : 2018.05.28  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appVideoYuvFlagSet(
    UINT8 en
)
{
    isYuvGet = en;
}
#endif /* SP5K_VREC_YUV_SAVE */
#endif /*VIDEO_REC_STAMP*/

