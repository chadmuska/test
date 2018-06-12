/**************************************************************************
 *
 *       Copyright (c) 2005-2016 by iCatch Technology, Inc.
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
 *                                                                        *
 *  Author:Stephen Hung                                                   *
 *                                                                        *
 **************************************************************************/
#ifndef _SP5K_CAPTURE_API_H_
#define _SP5K_CAPTURE_API_H_

#include "api/sp5k_global_api.h"
#include "middleware/common_types.h"
#include "middleware/dist_def.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
/* Error code */
#define SP5K_ERR_SUCCESS                        0x00000000
#define SP5K_ERR_CAPTURE_CFG                    0x00000001

/* Host message */
/* Moved to sp5k_msg_def.h */

/* -- Still capture configuration (sp5kStillCapCfgSet) -- */
/* Selector */
typedef enum {
	SP5K_CAPTURE_MODE = 0,
	SP5K_CAPTURE_MAIN_IMAGESIZE,
	SP5K_CAPTURE_MAIN_COMPRESSION_RATIO,
	SP5K_CAPTURE_MAIN_VLC_LOW_BOUND,
	SP5K_CAPTURE_MAIN_VLC_MAX_ENCODE_CNT,
	SP5K_CAPTURE_QV_LENGTH,
	SP5K_CAPTURE_STAMP,
	SP5K_CAPTURE_SHUTTER_SND_NAME,
	SP5K_CAPTURE_BURST_LENGTH,
	SP5K_CAPTURE_QV_RESOLUTION,
	SP5K_CAPTURE_WB_SIZE,
	SP5K_CAPTURE_IMG_MERGE_MODE,
	
	SP5K_CAPTURE_ORIENT,
	SP5K_CAPTURE_DARK_FRAME, /* obsolete */
	SP5K_CAPTURE_CONFIRM_STORE,
	SP5K_CAPTURE_BRC,
	SP5K_CAPTURE_SAVE_RAW,
	SP5K_CAPTURE_SAVE_RAW_OUT_PATH,
	SP5K_CAPTURE_VIEW_DISP_CTRL,
	SP5K_CAPTURE_CUSTOM_QTABLE,
	SP5K_CAPTURE_PURE_QV_SHOW,
	SP5K_CAPTURE_YUV_CB_AFTER_STAMP,
	SP5K_CAPTURE_BURST_INTERVAL,
	SP5K_CAPTURE_YUV_TRIG_NUM,
	SP5K_CAPTURE_MPO_IMGNUM,
	SP5K_CAPTURE_MPO_WITH_JPEG,
	SP5K_CAPTURE_RAW_COMPRESS_EN,
	SP5K_CAPTURE_SR_ENABLE,                  
	SP5K_CAPTURE_SR_EDGE_ENHANCE,
	SP5K_CAPTURE_SR_LCE,

	SP5K_CAPTURE_STORE_CONFIRM_TIMEOUT,
	SP5K_CAPTURE_MAX_COMPRESSION_RATIO,
	SP5K_CAPTURE_DBG_JPEG_CHECK_SUM,
	SP5K_CAPTURE_VLC_BUF_NUM,
	SP5K_CAPTURE_RAW_BUF_NUM,
	SP5K_CAPTURE_RAW_COMPRESS_BUF_NUM,
	SP5K_CAPTURE_YUV_BUF_NUM,
	SP5K_CAPTURE_FAST_BK_PREVIEW,
	SP5K_CAPTURE_FAST_BK_PREVIEW_SKIP_FRAME,
	SP5K_CAPTURE_VLC_STUFFING,
	SP5K_CAPTURE_BURST_FAST_SENSOR_TRIG,
	SP5K_CAPTURE_RAW_DATA_TYPE,

	SP5K_CAPTURE_PIV_PRESHOT_FRAME_NUM_S1,
	SP5K_CAPTURE_PIV_PRESHOT_FRAME_NUM_S2,
	SP5K_CAPTURE_PIV_PRESHOT_FRAME_PERIOD,
	SP5K_CAPTURE_PIV_PRESHOT_S2_ABORT,
	SP5K_CAPTURE_PIV_PRESHOT_MODE,

	SP5K_CAPTURE_BEST_SHOT_ID,

	SP5K_CAPTURE_QV_COMPRESSION_RATIO, /* obsolete */
	SP5K_CAPTURE_MAX_QV_COMPRESSION_RATIO,
	SP5K_CAPTURE_MAX_THM_COMPRESSION_RATIO,

	SP5K_CAPTURE_BEST_SHOT_VLC_ENABLE,
	SP5K_CAPTURE_FINAL_BURST_VLC_ENABLE,
	SP5K_CAPTURE_FINAL_BURST_VLC_NUMBER,
	SP5K_CAPTURE_USE_QUICK_QV,

	SP5K_CAPTURE_SAVE_YUV,
	SP5K_CAPTURE_YUV_CB_TYPE,
	SP5K_CAPTURE_JPG_CB_TYPE,
	SP5K_CAPTURE_DARKSUB_REDO,
	SP5K_CAPTURE_PREQV_ENABLE,
	SP5K_CAPTURE_YUV_SIZE_CFG,
	SP5K_CAPTURE_LDC_STITCH_FINAL_YUV_SIZE ,

	SP5K_CAPTURE_QV_BY_GPE,	/* To reduce JPG usage for performance. */
	SP5K_CAPTURE_BURST_ALL_FRAME_WB,
	SP5K_CAPTURE_BURST_DROP_ABORT,
	SP5K_CAPTURE_DO_IMAGE_ROI,
	SP5K_CAPTURE_LDC_STITCH,
}sp5kStillCapCfg;


/* Definition Of Selector; SP5K_CAPTURE_MODE */
#define SP5K_CAPTURE_MODE_SINGLE                 0x01
#define SP5K_CAPTURE_MODE_BURST                  0x04

/* Definition Of Selector: SP5K_CAPTURE_MAIN_COMPRESSION */
#define SP5K_CAPTURE_MAIN_COMPRESSION_2         0x01
#define SP5K_CAPTURE_MAIN_COMPRESSION_4         0x02
#define SP5K_CAPTURE_MAIN_COMPRESSION_6         0x03
#define SP5K_CAPTURE_MAIN_COMPRESSION_8         0x04
#define SP5K_CAPTURE_MAIN_COMPRESSION_10        0x05
#define SP5K_CAPTURE_MAIN_COMPRESSION_12        0x06
#define SP5K_CAPTURE_MAIN_COMPRESSION_14        0x07
#define SP5K_CAPTURE_MAIN_COMPRESSION_16        0x08
#define SP5K_CAPTURE_MAIN_COMPRESSION_18        0x09
#define SP5K_CAPTURE_MAIN_COMPRESSION_20        0x0a
#define SP5K_CAPTURE_MAIN_COMPRESSION_3         0x0b
#define SP5K_CAPTURE_MAIN_COMPRESSION_5         0x0c
#define SP5K_CAPTURE_MAIN_COMPRESSION_7         0x0d
#define SP5K_CAPTURE_MAIN_COMPRESSION_9         0x0e
#define SP5K_CAPTURE_MAIN_COMPRESSION_11        0x0f
#define SP5K_CAPTURE_MAIN_COMPRESSION_13        0x10
#define SP5K_CAPTURE_MAIN_COMPRESSION_15        0x11
#define SP5K_CAPTURE_MAIN_COMPRESSION_17        0x12
#define SP5K_CAPTURE_MAIN_COMPRESSION_19        0x13

/* Definition Of Structure; sp5kCaptureStamp _t */
#define SP5K_CAPTURE_DATE_YYYYMMDD              0x01   /*SNAP_DATE_YYYYMMDD*/
#define SP5K_CAPTURE_DATE_MMDDYYYY              0x02   /*SNAP_DATE_MMDDYYYY*/
#define SP5K_CAPTURE_DATE_DDMMYYYY              0x03   /*SNAP_DATE_DDMMYYYY*/
#define SP5K_CAPTURE_DATE_CUSTOM                0x09   /*SNAP_DATE_CUSTOM*/
#define SP5K_CAPTURE_DATE_TIME                  0x0100 /*SNAP_DATE_TIME*/
#define SP5K_CAPTURE_DATE_TIME_NO_SECOND        0x0200 /*SNAP_DATE_TIME_NO_SECOND*/
#define SP5K_CAPTURE_DATE_TIME_12HR             0x8100 /*SNAP_DATE_TIME_12HR*/
#define SP5K_CAPTURE_DATE_TIME_NO_SECOND_12HR   0x8200 /*SNAP_DATE_TIME_NO_SECOND_12HR*/

/* Definition of SP5K_CAPTURE_QV_NAIL_TYPE */
#define SP5K_CAPTURE_QV_NAIL_FMT_VLC             0x0
#define SP5K_CAPTURE_QV_NAIL_FMT_JPG             0x1

/* Selector */

/* Definition Of Selector SP5K_CAPTURE_ORIENT */
#define SP5K_CAPTURE_ORIENT_ROTATE_0            0x00
#define SP5K_CAPTURE_ORIENT_ROTATE_90           0x01
#define SP5K_CAPTURE_ORIENT_ROTATE_180          0x02
#define SP5K_CAPTURE_ORIENT_ROTATE_270          0x03
#define SP5K_CAPTURE_ORIENT_FLIP_V              0x04
#define SP5K_CAPTURE_ORIENT_FLIP_H              0x05

/* -- Still advanced capture mode configuration (sp5kStillCapAdvModeSet) -- */
/* Selector */
#define SP5K_CAPTURE_CALIBRATION                0x71

/* view display control */
/*
 * sp5kStillCapCfgSet(SP5K_CAPTURE_VIEW_DISP_CTRL, Option);
 * Option:
 *   - Auto:
 *       firmware controls the on/off. All turn off when entering capture,
 *       turn on when entering preivew if the original setting is on.
 *
 *   - Otherwise:
 *       bit[31:24] Enter snap. Image layer setting.
 *       bit[23:16] Enter snap. Osd layer setting.
 *       bit[15:8]  Exit  snap. Image layer setting.
 *       bit[0:7]   Eixt  snap. Osd layer setting.
 *       setting values: 0 -> off, 1 -> on, 2 -> bypass (let host control).
 */
#define SP5K_CAPTURE_VIEW_DISP_OFF        0x00000000
#define SP5K_CAPTURE_VIEW_DISP_ON         0x00000101
#define SP5K_CAPTURE_VIEW_DISP_AUTO       0xffffffff
#define SP5K_CAPTURE_VIEW_DISP_BYPASS     0x00000202
#define SP5K_CAPTURE_VIEW_DISP_BYPASS_OSD 0x00020102

/* -- Callback function selectors -- */
typedef enum {
	/* UINT32 wbCb(UINT32 addr, UINT32 rawFmt, sp5kImgWbInfo_t *pinfo) */
	SP5K_CAPTURE_CALLBACK_WB,
	/* UINT32 rawCb(UINT32 addr, UINT32 rawFmt, sp5kImgWbInfo_t *pinfo) */
	SP5K_CAPTURE_CALLBACK_RAW,
	/*
	 * UINT32 yuvCb(UINT32 yuvAddr, UINT32 rsvd, sp5kYuvCbImgSize_t *psize)
	 * return: [0] stop capture flow after yuvCb.
	 */
	SP5K_CAPTURE_CALLBACK_YUV_WITH_CDSP,
	/*
	 * UINT32 yuvCb(UINT32 yuvAddr, UINT32 rsvd, sp5kYuvCbImgSize_t *psize)
	 */
	SP5K_CAPTURE_CALLBACK_YUV_STANDALONE,
	/*
	 * UINT32 yuvCb(UINT32 yuvAddr, UINT32 rsvd, sp5kYuvCbImgSize_t *psize)
	 */
	SP5K_CAPTURE_CALLBACK_YUV_WITH_JPEG,
	/* UINT32 exifpCb(UINT32 imgId, UINT32 bufsize, void *ptr) */
	SP5K_CAPTURE_CALLBACK_EXIF_PREPARE,
	/* UINT32 exifCb(UINT32 imgId, UINT32 bufsize, void *ptr) */
	SP5K_CAPTURE_CALLBACK_EXIF_CREATE,
	/* UINT32 viewNotifyCb(UINT32 rsvd1, UINT32 rsvd2, void *rsvd3) */
	SP5K_CAPTURE_CALLBACK_VIEW_NOTIFY,
	/* UINT32 qvCb(UINT32 imgId, UINT32 qvaddr, sp5kYuvCbImgSize_t *psize) */
	SP5K_CAPTURE_CALLBACK_QV,
	/* UINT32 pureQvCb(UINT32 imgId, UINT32 qvaddr, sp5kYuvCbImgSize_t *psize) */
	SP5K_CAPTURE_CALLBACK_PURE_QV,
	/* UINT32 vlcCb(UINT32 imgId, UINT32 vlcsize, void *rsvd) */
	SP5K_CAPTURE_CALLBACK_VLC,
	/* UINT32 dcfCb(UINT32 rsvd1, UINT32 rsvd2, void *rsvd3) */
	SP5K_CAPTURE_CALLBACK_DCF,
	/* UINT32 burstCb(UINT32 rsvd1, UINT32 rsvd2, void *rsvd3) */
	SP5K_CAPTURE_CALLBACK_BURST,
	/* UINT32 darksubCb(UINT32 addr, UINT32 rawbit, sp5kImgWbInfo_t *pinfo) */
	SP5K_CAPTURE_CALLBACK_DARKSUB,
	/* UINT32 darksubCb2((UINT32 addr, UINT32 rawbit, sp5kImgWbInfo_t *pinfo) */
	SP5K_CAPTURE_CALLBACK2_DARKSUB,
	/*
	 * UINT32 jpgReencUpdCb(UINT32 proc, UINT32 vlcSize, UINT32 bufSize)
	 * proc   - 0: main, 1: thumbnail, 2: qv.
	 * return - 0: continue re-encode, 1: abort re-encode.
	 */
	SP5K_CAPTURE_CALLBACK_JPG_REENC_UPDATE,
	/**
	 * \note callback function after QV is displayed
	 * UINT32 postQvCb(UINT32 imgId, UINT32 rsvd2, void *rsvd3)
	 */
	SP5K_CAPTURE_CALLBACK_POST_QV,
	/*
	 * UINT32 stampProc(UINT32 rsvd1, UINT32 rsvd2, struct tm *ptime)
	 */
	SP5K_CAPTURE_CALLBACK_STAMP_PROC,
	SP5K_CAPTURE_CALLBACK_SNAP_TRIGGER,
	SP5K_CAPTURE_CALLBACK_IQ_TRIGGER,
	SP5K_CAPTURE_CALLBACK_MPF_ATTR_VALUE_SET,
	SP5K_CAPTURE_CALLBACK_AE,
	SP5K_CAPTURE_CALLBACK_STORE_DONE,

	SP5K_CAPTURE_CALLBACK_YUV_STITCH,
} sp5kStillCapCbFunc_e;

/* -- Callback Services (sp5kStillCapCallbackSet) -- */
/* raw data formats */
#define SP5K_CAPTURE_RAW_FMT_8_BIT                  0x00
#define SP5K_CAPTURE_RAW_FMT_10_BIT                 0x01
#define SP5K_CAPTURE_RAW_FMT_12_BIT                 0x02
#define SP5K_CAPTURE_RAW_FMT_16_BIT                 0x04

/* -- Debug Option -- */
#define SP5K_CAPTURE_DEBUG_1                 	0x01 /* Debug for snap flow's steps */
#define SP5K_CAPTURE_DEBUG_2                 	0x02 /* Print out to files for CDSP parameters */
#define SP5K_CAPTURE_DEBUG_9                 	0x09 /* Print out the information during snap flow */

/* -- Max STAMP strings -- */
#define SP5K_CAPTURE_DATE_CUSTOM_MAX_LEN        SNAP_DATE_CUSTOM_MAX_LEN

/* Raw callback return value */
#define SP5K_RAW_CB_RET_REDO_LSC 		SNAP_RAW_CB_RET_REDO_LSC
#define SP5K_RAW_CB_RET_REDO_WB_OFFSET	SNAP_RAW_CB_RET_REDO_WB_OFFSET
#define SP5K_RAW_CB_RET_REDO_WB_PREGAIN	SNAP_RAW_CB_RET_REDO_WB_PREGAIN


/* YUV callback return value */
#define SP5K_YUV_CB_RET_SKIP_FLOW SNAP_YUV_CB_RET_SKIP_FLOW  /* the snap flow will be skipped if host return this value in yuv callback */
#define SP5K_YUV_CB_RET_LOCK_BUF SNAP_YUV_CB_RET_LOCK_BUF
#define SP5K_YUV_CB_RET_SKIP_AND_LOCK_BUF (SNAP_YUV_CB_RET_LOCK_BUF | SNAP_YUV_CB_RET_SKIP_FLOW) /* the snap flow will be skipped and rgb/yuv buffer will not be returned if host return this value in yuv callback */
#define SP5K_YUV_CB_RET_RELEASE_ALL_LOCKED_BUF (SNAP_YUV_CB_RET_UNLOCK_ALL_BUF) /* all locked rgb/yuv buffer will be returned if host return this value in yuv callback */
#define SP5K_YUV_CB_RET_CDSP_REDO (SNAP_YUV_CB_RET_CDSP_REDO) /* all locked rgb/yuv buffer will be returned if host return this value in yuv callback */
#define SP5K_YUV_CB_RET_WAIT_FOR_HOST SNAP_YUV_CB_RET_WAIT_FOR_HOST /* for best shot */
#define SP5K_YUV_CB_RET_REDO_SPECIFIC_BUF SNAP_YUV_CB_RET_REDO_SPECIFIC_BUF
#define SP5K_YUV_CB_RET_SPECIFIC_BUF_ID_BIT_POS SNAP_YUV_CB_RET_SPECIFIC_BUF_ID_BIT_POS
#define SP5K_YUV_CB_RET_RELEASE_SPECIFIC_LOCKED_BUF ( SNAP_YUV_CB_RET_UNLOCK_SPECIFIC_BUF )

#define SP5K_QV_CB_RET_QV_SHOW  SNAP_QV_CB_RET_QV_SHOW
#define SP5K_QV_CB_RET_LOCK_CURR_BUF SNAP_QV_CB_RET_QV_LOCK_CURR_BUF
#define SP5K_QV_CB_RET_SHOW_AND_LOCK_CURR_BUF (SNAP_QV_CB_RET_QV_SHOW | SNAP_QV_CB_RET_QV_LOCK_CURR_BUF) 
#define SP5K_QV_CB_RET_RELEASE_ALL_LOCKED_BUF (SNAP_QV_CB_RET_QV_UNLOCK_ALL_BUF) 
#define SP5K_SNAP_VLC_CB_RET_BEST_SHOT_DROP SNAP_VLC_CB_RET_BEST_SHOT_DROP
#define SP5K_SNAP_VLC_CB_RET_BEST_SHOT_KEEP SNAP_VLC_CB_RET_BEST_SHOT_KEEP

#define SP5K_SNAP_VLC_CB_RET_BEST_SHOT_CONTINUE SNAP_VLC_CB_RET_BEST_SHOT_CONTINUE
#define SP5K_SNAP_VLC_CB_RET_BEST_SHOT_FINISH SNAP_VLC_CB_RET_BEST_SHOT_FINISH

#define SP5K_CAPTURE_HDR_MODE_OFF 		SNAP_HDR_MODE_OFF
#define SP5K_CAPTURE_HDR_MODE_ACC 		SNAP_HDR_MODE_ACC
#define SP5K_CAPTURE_HDR_MODE_NORMAL 	SNAP_HDR_MODE_NORMAL

#define SP5K_CAPTURE_HDR_ACC_AVERAGE_OFF	SNAP_HDR_ACC_AVERAGE_OFF
#define SP5K_CAPTURE_HDR_ACC_AVERAGE_ON		SNAP_HDR_ACC_AVERAGE_ON

#define SP5K_CAPTURE_YUV_CB_REF_YUV			SNAP_YUV_CB_REF_YUV
#define SP5K_CAPTURE_YUV_CB_MODIFY_YUV	    SNAP_YUV_CB_MODIFY_YUV


/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define SP5K_FEATURE_CAPTURE_EXIF_PREPARE
#define SP5K_FEATURE_CAPTURE_QV_CALLBACK
#define SP5K_FEATURE_CAPTURE_POST_QV_CALLBACK

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
/* For SP5K callback phototype */
/* typedef UINT32 (*fp_sp5kCallback_t)(UINT32, UINT32, void *); */
#define fp_sp5kCallback_t                       fpsp5kCallback_t

/* Definition Of Structure; sp5kImgSize_t */
typedef struct sp5kImgSize_s {
	UINT32 width;
	UINT32 height;
	UINT32 cropWidth;
	UINT32 cropHeight;
} sp5kImgSize_t;


/* Definition Of Structure; sp5kImgRoiSize_t */
typedef struct sp5kImgRoiSize_s {
	UINT32 cropX;
	UINT32 cropY;
	UINT32 cropWidth;
	UINT32 cropHeight;
} sp5kImgRoiSize_t;
/* Definition of structure; sp5kImgVirtualSize_t */
typedef struct sp5kImgVirtualSize_s {
	UINT32 width;
	UINT32 height;
	UINT32 phyWidth;
	UINT32 phyHeight;
} sp5kImgVirtualSize_t;



/* Definition Of Structure; sp5kCaptureFrame_t */
typedef struct sp5kCaptureFrame_s {
	UINT8 *pf;
	UINT32 pathLen;
	UINT16 w;
	UINT16 h;
	UINT16 x;
	UINT16 y;
}sp5kCaptureFrame_t;

/* Definition Of Structure; sp5kCaptureStamp_t */
typedef struct sp5kCaptureStamp_s {
	UINT32 dateFmtId;
	UINT8 *pdate;
	UINT32 dateLen;
	UINT32 backgndColor;
	UINT32 fontColor;
	UINT32 OutlineColor;
	UINT32 scaleRatio;
	UINT16 x;
	UINT16 y;
}sp5kCaptureStamp_t;

/* Definition Of Structure; sp5kCaptureBurstQvLayout_t */
typedef struct sp5kCaptureBurstQvLayout_s {
	UINT32 qvEn;
	UINT16 n;
	UINT16 m;
	UINT16 w;
	UINT16 gapX;
	UINT16 h;
	UINT16 gapY;
}sp5kCaptureBurstQvLayout_t;

/* Preview callback */
typedef UINT32 (*fp_preCallback_t)(
	UINT32 fid, 	/* Frame ID */
	UINT8 *pbuf,	/* Frame buffer pointer */
	UINT32 w,	/* Buffer width */
	UINT32 h	/* Buffer height */
);

typedef struct sp5kPrevCallback_s {
	fp_preCallback_t fpcb; /* callback */
	UINT32 w; /* expected output width */
	UINT32 h; /* expected output height */
} sp5kPrevCallback_t;

/* Urgent callback */
typedef UINT32 (*fp_preUrgentCallback_t)(
	UINT32 streamId,        /* nth stream, from 0 */
	UINT32 fid,             /* frame id */
	frameBufInfo_t *pframe, /* buffer information */
	UINT8 *pbufdup          /* duplicated buffer address */
);

typedef struct sp5kPrevUrgentCallback_s {
	fp_preUrgentCallback_t fp;
} sp5kPrevUrgentCallback_t;

typedef UINT32 (*fp_pvDual360StitchCb_t)(
	UINT32 senId,        /* nth sensor */
	frameBufInfo_t *srcFrame,  /* src buffer information */
	frameBufInfo_t *dstFrame   /* dst buffer information */
);

typedef struct sp5kPvDual360StitchCb_s {
	fp_pvDual360StitchCb_t fp;
} sp5kPvDual360StitchCb_t;

/* Preview YUV ROI */
typedef struct sp5kPrevYuvRoi_s {
	UINT32 width;	/* Buffer width */
	UINT32 height;	/* Buffer height */
	UINT32 roix;	/* offset x of roi */
	UINT32 roiy;	/* offset y of roi */
	UINT32 roiw;	/* width of roi */
	UINT32 roih;	/* height of roi */
	UINT32 secroix;	/* offset x of roi */
	UINT32 secroiy;	/* offset y of roi */
	UINT32 secroiw;	/* width of roi */
	UINT32 secroih;	/* height of roi */
} sp5kPrevYuvRoi_t;

typedef struct sp5kCapBestShot_ID_s {
	UINT32 num;
	UINT32 *id;
} sp5kCapBestShot_ID_t;

#if SPCA5210
typedef struct stillDistGmvInfo_s sp5kStillDistGmvInfo_t;


typedef enum {
	SP5K_CAPTURE_DIST_EW_BLOCKS_H = STILL_DIST_EW_BLOCKS_H,
	SP5K_CAPTURE_DIST_EW_BLOCKS_V = STILL_DIST_EW_BLOCKS_V,
	SP5K_CAPTURE_DIST_EW_THRESHOLD_H = STILL_DIST_EW_THRESHOLD_H,
	SP5K_CAPTURE_DIST_EW_THRESHOLD_V = STILL_DIST_EW_THRESHOLD_V,
	SP5K_CAPTURE_DIST_ME_BLOCKS_THRESHOLD = STILL_DIST_ME_BLOCKS_THRESHOLD,
	SP5K_CAPTURE_DIST_GMV_INFO_LENGTH = STILL_DIST_GMV_INFO_LENGTH,
	SP5K_CAPTURE_DIST_EXP_ISO_STEP = STILL_DIST_EXP_ISO_STEP,
	SP5K_CAPTURE_DIST_ENV_MODE = STILL_DIST_ENV_MODE,

} sp5kStillDistCfgSel_e;
#endif

/* still capture image merge mode */
typedef enum{
	SP5K_CAPTURE_MERGE_MODE_OFF = 0,
	SP5K_CAPTURE_MERGE_MODE_PHOTOSOLID,
	SP5K_CAPTURE_MERGE_MODE_HDR,
	SP5K_CAPTURE_MERGE_MODE_MAX
} sp5kStillImageMergMode_e;

typedef enum{
	SP5K_CAPTURE_PRESHOT_DISABLE = 0,
	SP5K_CAPTURE_PRESHOT_ENABLE
} sp5kCapturePreShotMode_e;

typedef enum {
	SP5K_SNAP_BUF_TYPE_RAW = 0,
	SP5K_SNAP_BUF_TYPE_YUV,
	SP5K_SNAP_BUF_TYPE_ANY,
} sp5kSnapBufType_e;

typedef enum {
	SP5K_LDC_STITCH_PRI_RAW = (1 << 0),
	SP5K_LDC_STITCH_SEC_RAW = (1 << 1),
	SP5K_LDC_STITCH_ALL_RAW_READY = SP5K_LDC_STITCH_PRI_RAW | SP5K_LDC_STITCH_SEC_RAW,
} sp5kPvStitchSensorRaw_t;

typedef enum {
	SP5K_CDSP_DRAM_V_TYPE_YUV_422,
	SP5K_CDSP_DRAM_V_TYPE_YUV_420,
} sp5kCdspDramVType_t;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
/* General APIs */
UINT32 sp5kStillCapCfgSet(UINT32 selector, UINT32 param);
UINT32 sp5kStillCapAbort(void);
UINT32 sp5kStillCapStoreConfirm(UINT32 flag);
UINT32 sp5kStillCapJobInfo(UINT32 *pwaitJob, UINT32 *pavailableJob);
UINT32 sp5kStillCapDistCfgSet(UINT32 selector, UINT32 value);
UINT32 sp5kStillCapDistCfgGet(UINT32 selector, UINT32 *pvalue);
UINT32 sp5kStillCapDistStart(void);
UINT32 sp5kStillCapDistStop(void);
UINT32 sp5kStillCapDistGmvInfoGet(sp5kStillDistGmvInfo_t **, UINT32 *);
UINT32 sp5kStillCapDistExpStepGet(UINT32, SINT32 *);

/* Callback functions */
UINT32 sp5kStillCapCallbackSet(UINT32 selector, void *pfunc);
#if SPCA6350
/**
 * \brief Set both main/second path preview callback function 
 * \param[in] interval How many frames comes one preview callback.
 * \param[in] pmain Callback related information of main path.
 * \param[in] psecond Callback related information of 2nd path.
 *
 * Note that the psecond is also need to be set when only wants to enable the
 * main path callback. Fill callback function to be null in this case.
 */
UINT32 sp5kPreviewCallbackSet(UINT32 interval, 
	sp5kPrevCallback_t *pmain, sp5kPrevCallback_t *psecond);
	
UINT32 sp5kPreviewYuvSizeGet( sp5kPrevYuvRoi_t *pmainyuv, sp5kPrevYuvRoi_t *p2ndyuv);
#else
/**
 * \brief Set the preview callback function
 * \param[in] interval How many frames comes one preview callback.
 * \param[in] pfunc Callback function to be invocated.
 * \param[in] w Expected width.
 * \parma[in] h Expected height.
 */
UINT32 sp5kPreviewCallbackSet(UINT32 interval,fp_preCallback_t pfunc,
	UINT32 w,UINT32 h);
#endif
#if SPCA6350
/**
 * \brief Set both main/second path (streams) video urgent callback function 
 * \param[in] interval How many frames comes one video urgent callback.
 * \param[in] ctrl Control. b[0]: dup 0th stream, b[1]: dup 1st stream
 * \param[in] cbStream Pointer to stream callback lists.
 * \param[in] cbStreamNum Number of streams.
 *
 * Note that the main path(0th stream) is also needed if we only want to use
 * the second path (1st stream). Fill callback function as null in this case.
 */
UINT32 sp5kPreviewUrgentCallbackSet(UINT32 interval, UINT32 ctrl,
	sp5kPrevUrgentCallback_t *cbStream, UINT32 cbStreamNum);
#else
/**
 * \brief Set the preview urgent callback function
 * \param[in] interval How many frames comes one preview urgent callback.
 * \param[in] pfunc Callback function to be invocated.
 */
UINT32 sp5kPreviewUrgentCallbackSet(UINT32 interval, 
	fp_preCallback_t pfunc);
#endif
UINT32 sp5kPreviewMvInfoGet(UINT32 *fid, SINT16 *x1, SINT16 *y1, SINT16 *x2, SINT16 *y2, UINT32 *numx, UINT32 *numy);
UINT32 sp5kPreviewGmvInfoGet(UINT32 *fid, SINT16 *x1, SINT16 *y1, SINT16 *x2, SINT16 *y2, UINT32 *numx, UINT32 *numy);
UINT32 sp5kPreviewLmvTableGet(UINT32 *fid, UINT8 *plmvTbl, UINT32 tblSize);

#if SPCA6350
/**
 * \brief Set both main/second path (streams) video urgent callback function 
 * \param[in] interval How many frames comes one video urgent callback.
 * \param[in] ctrl Control. b[0]: dup 0th stream, b[1]: dup 1st stream
 * \param[in] cbStream Pointer to stream callback lists.
 * \param[in] cbStreamNum Number of streams.
 *
 * Note that the main path(0th stream) is also needed if we only want to use
 * the second path (1st stream).
 */
UINT32 sp5kVideoUrgentCallbackSet(UINT32 interval, UINT32 ctrl,
	sp5kPrevUrgentCallback_t *cbStream, UINT32 cbStreamNum);

/**
 * \brief Get the main/second path (streams) information.
 * \param[in] pbuf Pointer to buffer information.
 * \param[in] nStream nth number of stream information to get.
 */
UINT32 sp5kUrgentCallbackYuvGet(frameBufInfo_t *pbuf, UINT32 nStream, UINT32 nSen);

#else
/**
 * \brief Set the video urgent callback function
 * \param[in] interval How many frames comes one video urgent callback.
 * \param[in] pfunc Callback function to be invocated.
 */
UINT32 sp5kVideoUrgentCallbackSet(UINT32 interval, 
	fp_preCallback_t pfunc);
#endif
UINT32 sp5kDispUrgentCallbackSet(UINT32 interval,UINT32 ctrl,sp5kPrevUrgentCallback_t *cbStream,UINT32 cbStreamNum);
UINT32 sp5kDispUrgentCallbackYuvGet(frameBufInfo_t *pbuf, UINT32 nStream, UINT32 nSen);


/* Misc */
void sp5kStillCapCropSizeGet(UINT32 *width, UINT32 *height);
void sp5kStillCapQvSizeGet(UINT32 imgW, UINT32 imgH, UINT32 qvW, UINT32 qvH, UINT32 *realW, UINT32 *realH);

void sp5kStillCapFastPreviewShow(void);
UINT32 sp5kStillCapPbBufInit(UINT32 width, UINT32 height);
UINT32 sp5kStillCapPbBufDisplay(UINT8 *paddr);
UINT32 sp5kStillCapPbBufDestory(void);

UINT32 sp5kStillCapBuffUnLock(UINT32 param1,UINT32 param2,void * param3);

/* Speedy Burst Preview Yuv */
/**
 * \note pfunc in sp5kPreviewYuvCapCallbackSet()
 * 1st arg: frame idx
 * 2nd arg: YUV buffer address
 * 3rd arg: pointer to sp5kPrevYuvRoi_t
 */
void sp5kPreviewYuvCapCallbackSet(fp_sp5kCallback_t pfunc);
UINT32 sp5kPreviewYuvCapStart(UINT32 num, UINT8 *pbuf, UINT32 maxSize);
UINT32 sp5kPreviewYuvCapStop(void);
UINT32 sp5kPreviewJpegSave(UINT8 *pbuf, sp5kPrevYuvRoi_t *proi, UINT32 jpegW, UINT32 jpegH);
void sp5kPreviewYuvCapDateTimeSet(tmx_t *ptime);

/* MPO parameter set */
void sp5kStillCapMpoParamSet(UINT8 idxNum, UINT8 attrNum, UINT32 imgNum);

/* preshot */

/* snap buffer alloc/free */
UINT32 sp5kStillCapBufAlloc( UINT32 type, UINT32 size );
UINT32 sp5kStillCapBufFree( UINT32 addr );


#endif /* _SP5K_CAPTURE_API_H_ */

