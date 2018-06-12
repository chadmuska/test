/*******************************************************************************
*
*       Copyright (c) 2005-2012 by iCatch Technology, Inc.
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
*  Hsin-Chu, Taiwan, R.O.C.
*
*  Author: Phacnia Lai
*
*******************************************************************************/
#ifndef _SP5K_PB_API_H_
#define _SP5K_PB_API_H_

#include "common.h"

/*******************************************************************************
*                              C O N S T A N T S
*******************************************************************************/
typedef enum {
    SP5K_MPB_STYLE_SHOW_ALL_ONCE=0x00, /* show all things after we got all of the thumbnails */
    SP5K_MPB_STYLE_SHOW_ONE_ONCE, /* once we got an thumbnail, just show it */
}sp5kMpbStyle_e;

/* Basic Playback Configuration Selector Constants */
typedef enum {
	SP5K_SPB_IMG_TYPE = 0x00000000,
	SP5K_SPB_BACKGROUND_COLOR,
	SP5K_SPB_QV_ROTATE_BASE,
	SP5K_SPB_THUMB_ROTATE_BASE,
	SP5K_SPB_PRIMARY_ROTATE_BASE,
	SP5K_SPB_BLOG_BG_COLOR, /*!< YUV background color in UINT32, V[23:16]Y[15:8]U[7:0] */
	SP5K_SPB_BLOG_SCALE, /*!< scale factor from 100(0.1x) ~ 1000(1x) */
	SP5K_SPB_MAIN_DECODE, /*!< 1:enable main decode, 0:disable */
	SP5K_SPB_HIDE_IMAGE, /*!< 1:don't show image 0:show */
	SP5K_SPB_RELOAD_CURRENT, /*!< 1:reload the current image 0:no */
	SP5K_SPB_MAIN_DECODE_ORI_OVERRIDE,
	SP5K_SPB_QV_DECODE_ORI_OVERRIDE,
	SP5K_SPB_THUM_DECODE_ORI_OVERRIDE,
	SP5K_SPB_QV_RESOLUTION,
	SP5K_SPB_SCALE_QUALITY,
	SP5K_SPB_RING_BUFFER_MAX_NUM,
	SP5K_SPB_FORCE_RELOAD, /*!< 1:always reload when host set PB mode 0:no */
	SP5K_SPB_MAIN_BUF_SIZE_LIMIT_RATIO,
	SP5K_SPB_PIP_WINDOW,
	
} sp5kSpbCfgSelector_e;

/* Playback Image Type Constant */
typedef enum {
	SP5K_SPB_IMG_PRIMARY = 0x00,
	SP5K_SPB_IMG_THUMB,
	SP5K_SPB_IMG_THUMB_PRIMARY,
	SP5K_SPB_IMG_QV,
} sp5kSpbImgType_e;

/* Playback Qv Rotate Constant */
typedef enum {
	SP5K_SPB_QV_ROTATE_BY_QV = 0x00,
	SP5K_SPB_QV_ROTATE_BY_THUMB,
	SP5K_SPB_QV_ROTATE_BY_PRIMARY,
	SP5K_SPB_THUMB_ROTATE_NONE,
	SP5K_SPB_THUMB_ROTATE_BY_THUMB,
	SP5K_SPB_PRIMARY_ROTATE_BY_NONE,
	SP5K_SPB_PRIMARY_ROTATE_BY_PRIMARY,
	SP5K_SPB_QV_ROTATE_BY_NONE,
	SP5K_SPB_THUMB_ROTATE_BY_PRIMARY,
} sp5kSpbImgRotateBase_e;

/* Multi Playback Configuration Selector Constants */
typedef enum {
	SP5K_MPB_START_FILEINDEX = 0x00,
	SP5K_MPB_FOCUS_OSD_COLOR,
	SP5K_MPB_FOCUS_OSD_BORDER,
	SP5K_MPB_BACKGROUND_COLOR,
	SP5K_MPB_BACKGROUND_IMG,
	SP5K_MPB_ICON_FOR_AVI,
	SP5K_MPB_ICON_FOR_ASF,
	SP5K_MPB_ICON_FOR_MOV,
	SP5K_MPB_ICON_FOR_WAV,
	SP5K_MPB_ICON_FOR_WMA,
	SP5K_MPB_ICON_FOR_MP3,
	SP5K_MPB_ICON_FOR_MP4,
	SP5K_MPB_ICON_FOR_ERR_FILE,
	SP5K_MPB_ENLARGE_CURRENT_FILE,
	SP5K_MPB_ENLARGE_WINDOW,
	SP5K_MPB_ENLARGE_WINDOW_LIST,
	SP5K_MPB_DCF_START_INDEX,
	SP5K_MPB_DCF_END_INDEX,
	SP5K_MPB_DCF_SORT_MODE, /*!> 0:default, 1:sort by date, 2:custom start/end idx */
	SP5K_MPB_NORMAL_FRAME_BORDER,
	SP5K_MPB_NORMAL_FRAME_COLOR,
	SP5K_MPB_DCF_CYCLIC_MODE,
	SP5K_MPB_PIP_SYNC_MODE, /*!> 0:auto refresh, 1:refresh by host */
	SP5K_MPB_FRAME_SIZE,
	SP5K_MPB_STYLE,
	SP5K_MPB_FORCE_QQVGA_THM,
	SP5K_MPB_DECODE_SIZE_SEQ,	
	SP5K_MPB_DONE_MSG_TIMEOUT,
} sp5kMpbCfgSelector_e;

/* Multi Playback Type Constants */
typedef enum {
	SP5K_MPB_NXM_LAYOUT    = 0x00,
	SP5K_MPB_CUSTOMIZE_LAYOUT,
} sp5kMpbLayoutType_e;

/* Multi Playback Shift Contants */
typedef enum {
	SP5K_MPB_SHIFT_UP = 0x00,
	SP5K_MPB_SHIFT_DOWN,
	SP5K_MPB_SHIFT_SHOW_FOCUS,
} sp5kMpbShiftDir_e;

/* Image Edit Contants */
#define SP5K_IMGEDIT_ATTR_PREVIEW	0x80000000
#define SP5K_IMGEDIT_ATTR_NO_DISP	0x40000000	/* don't update display while doing main editing */
#define SP5K_IMGEDIT_ATTR_TIME_SET	0x20000000 /*Note(2014.9.23): replaced by SP5K_IMGEDIT_SAVE_FLAG_TIME_SET*/
#define SP5K_IMGEDIT_ATTR_EXIF_ROT  0x10000000 /* keep exif rotation tag of origin image in output image */
#define SP5K_IMGEDIT_ATTR_KEEP_EXIF_TIME	0x08000000 /*Note(2014.9.23): replaced by SP5K_IMGEDIT_SAVE_FLAG_KEEP_EXIF_TIME*/
#define SP5K_IMGEDIT_ATTR_COMBI		0x00010000 /* combination more function and save */

#define SP5K_IMGEDIT_SAVE_TYPE_DCF			0
#define SP5K_IMGEDIT_SAVE_TYPE_RESOURCE		1

#define SP5K_IMGEDIT_SAVE_FLAG_NEWFILE		0x0001
#define SP5K_IMGEDIT_SAVE_FLAG_KEEP_DCF_IDX	0x0002
#define SP5K_IMGEDIT_SAVE_FLAG_UPDATE_APP1	0x0010
#define SP5K_IMGEDIT_SAVE_FLAG_TIME_SET 		0x0020 /*!> set custome time for image edit save */
#define SP5K_IMGEDIT_SAVE_FLAG_KEEP_EXIF_TIME 	0x0040/* don't touch EXIF time tag in output image */

typedef enum {
	SP5K_IMGEDIT_CROP = 0x00,
	SP5K_IMGEDIT_SCALE_DOWN,
	SP5K_IMGEDIT_SCALE_UP,
	SP5K_IMGEDIT_ROTATE_EXIF_TAG = 0x04,
	SP5K_IMGEDIT_CDSP,	
	SP5K_IMGEDIT_SAVE,
	SP5K_IMGEDIT_PHOTOFRM_FILE  = 0x1000,
	SP5K_IMGEDIT_SPECIAL_EFFECT = 0x1001,
	SP5K_IMGEDIT_PHOTOFRM_LOC   = 0x1002,
	SP5K_IMGEDIT_OSD_BLEND      = 0x1003,
	SP5K_IMGEDIT_CUSTOMIZE      = 0x2000,
} sp5kImageEdit_e;

/* Image rotate/flip*/
typedef enum {
	SP5K_ROTATE_0 = 0x00,
	SP5K_ROTATE_90,
	SP5K_ROTATE_180,
	SP5K_ROTATE_270,
	SP5K_ROTATE_FLIP_H,
	SP5K_ROTATE_FLIP_V,
	SP5K_ROTATE_SET_NUM,
} sp5kRotFlip_e;


typedef enum {
	SP5K_DECODE_ROTATE_OVERRIDE_0	  = 0x00, 
	SP5K_DECODE_ROTATE_OVERRIDE_90,
	SP5K_DECODE_ROTATE_OVERRIDE_180,
	SP5K_DECODE_ROTATE_OVERRIDE_270,
	SP5K_DECODE_ROTATE_OVERRIDE_FLIP_H,
	SP5K_DECODE_ROTATE_OVERRIDE_FLIP_V,
	SP5K_DECODE_ROTATE_OVERRIDE_NONE,
} sp5kMainDecRotOverride_e;


/* Image file type*/
typedef enum {
	SP5K_PB_FILE_JPG = 0x0001,
	SP5K_PB_FILE_ASF = 0x0002,
	SP5K_PB_FILE_AVI = 0x0004,
	SP5K_PB_FILE_MOV = 0x0008,
	SP5K_PB_FILE_WAV = 0x0020,
	SP5K_PB_FILE_WMA = 0x0040,
	SP5K_PB_FILE_MP3 = 0x0080,
} sp5kPbImageFileType_e;

/* Multi Playback status contants*/
typedef enum {
	SP5K_PB_FILE_RECOGNIZED = SUCCESS,
	SP5K_PB_FILE_UNRECOGNIZED,
} ap5kMpbStatusInfo_e;

/* still image YUV buffer infomation constants */
typedef enum {
	SP5K_PB_IMG_ATTR_ADDR   = 0x0000,
	SP5K_PB_IMG_ATTR_WIDTH  = 0x0001,
	SP5K_PB_IMG_ATTR_HEIGHT = 0x0002,
} sp5kPbYUVInfo_e;

/*PB zoom scale */
typedef enum {
    SP5K_SPB_SCALE_QUALITY_FASTEST=0,  /*use GPE*/
    SP5K_SPB_SCALE_QUALITY_FAST, 	/*use GPE*/
    SP5K_SPB_SCALE_QUALITY_NORMAL, 	/*use GPE Bilinear*/
    SP5K_SPB_SCALE_QUALITY_GOOD, /*use GPE Bilinear*/
    SP5K_SPB_SCALE_QUALITY_BEST, /*use JPEG*/
    SP5K_SPB_SCALE_QUALITY_MAX,
} sp5kPbZoomScaleQuality_e;

/**/
typedef enum {
    SP5K_EFFECT_BW = 0x00,
    SP5K_EFFECT_NEGATIVE,
    SP5K_EFFECT_EMBOSS,
    SP5K_EFFECT_MOSAIC,
    SP5K_EFFECT_SEPIA,
    SP5K_EFFECT_RED,
    SP5K_EFFECT_GREEN,
    SP5K_EFFECT_BLUE,
    SP5K_EFFECT_BLUR,
    SP5K_EFFECT_FISHEYE,
} sp5kPbEffectID_e;

/* Multi Playback decode size*/
typedef enum {
	SP5K_MPB_DEC_SIZE_MAIN = 1 << 29,
	SP5K_MPB_DEC_SIZE_QV = 1 << 30,
	SP5K_MPB_DEC_SIZE_THM = 1 << 31,
	SP5K_MPB_DEC_SIZE_ALL = SP5K_MPB_DEC_SIZE_MAIN|SP5K_MPB_DEC_SIZE_QV|SP5K_MPB_DEC_SIZE_THM,
} sp5KmpbDecSize_e;

/* Multi Playback decode Sequence*/
typedef enum {
	SP5K_MPB_DEC_SEQ_THM_QV_MAIN = 0,
	SP5K_MPB_DEC_SEQ_QV_MAIN_THM,
	SP5K_MPB_DEC_SEQ_QV_THM_MAIN,
	SP5K_MPB_DEC_SEQ_MAX,
} sp5KmpbDecSeq_e;

typedef enum {
	SP5K_PBCB_MULTI_PB_OBJ_EDIT = 0x100,
	SP5K_PBCB_PB_SCALE = 0x200,
	SP5K_PBCB_IMG_EDIT = 0x300,
	SP5K_PBCB_EXTEND_FILE_DECODE = 0x400,
	SP5K_PBCB_IMG_EDIT_EXIF_UPDATE,
} sp5kPbCbSel_e;
/*******************************************************************************
*                              D A T A   T Y P E S
*******************************************************************************/
typedef struct sp5kSpbRect_s {
	UINT32 left;
	UINT32 top;
	UINT32 right;
	UINT32 bottom;
	UINT32 drawImageOrder;
} sp5kSpbRect_t;

/* MULTI_PLAY_NXM_LAUOUT Structure Definition */
typedef struct sp5kMpbNxMLayout_s{
	UINT8  valueN;        /* specify how many thumbnails like to be displayed on screen horizontal */
	UINT8  valueM;        /* specify how many thumbnails like to be displayed on screen vertical */
	UINT16 leftMargin;    /* specify the spcae margin in left side */
	UINT16 gapX;          /* specify the inter-gap in horizontal */
	UINT16 rightMargin;   /* specify the spcae margin in right side */
	UINT16 topMargin;     /* specify the spcae margin in top side */
	UINT16 gapY;          /* specify the inter-gap in vertical */
	UINT16 bottomMargin;  /* specify the spcae margin in bottom side */
} sp5kMpbNxMLayout_t;

/* MULTI_PLAY_CUSTOMIZE_LAUOUT Structure Definition */
typedef struct sp5kMpbCustomizeLayout_s{
	UINT8         count;     /* specify how many thumbnails like to be displayed on whole screen */
	sp5kSpbRect_t *prect;    /* specify the initial values of each custom layout */
} sp5kMpbCustomizeLayout_t;

/* Image editing callback */
typedef UINT32 (*fp_pbImgEditCallback_t)(
	UINT32,	/* YUV buffer pointer */
	UINT32,	/* Buffer width */
	UINT32	/* Buffer height */
);

/* Window definition of enlarged file */
typedef struct sp5kMpbEnlargeWindow_s {
	SINT32	offX;		/* offset x of left-top point in the thumbnail */
	SINT32	offY;		/* offset y of left-top point in the thumbnail */
	UINT32	deltaW;		/* increment of thumbnail width */
	UINT32	deltaH;		/* increment of thumbnail height */
} sp5kMpbEnlargeWindow_t;

/*!< roi region of image edit scale down */
typedef struct sp5kImgEditRoi_s {
	UINT32 roiX;
	UINT32 roiY;
	UINT32 roiW;
	UINT32 roiH;
} sp5kImgEditRoi_t;

typedef struct sp5kMpbFrameSize_s {
	UINT32 frmW;
	UINT32 frmH;
	UINT8 LargeFrmMode;
} sp5kMpbFrameSize_t;

typedef struct sp5kpbObj_s{
	UINT8 *pbuf;
	UINT8 *paplaneBuf;
	UINT32 bufW;
	UINT32 bufH;
	SINT32 roiX;
	SINT32 roiY;
	UINT32 fmt;   
	UINT32 roiW;
	UINT32 roiH;
} sp5kPbObj_t;
/*plz keep sp5kPbObj_t in same data structure with pbObj_t */

/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*******************************************************************************/
UINT32 sp5kStillPlayCfgSet(UINT32 selector, UINT32 value);
UINT32 sp5kMultiPlayCfgSet(UINT32 selector, UINT32 value, UINT8 *pfilename);
UINT32 sp5kMultiPlayLayoutInit(UINT32 type,sp5kMpbNxMLayout_t *pregularLayout, sp5kMpbCustomizeLayout_t *pcustomLayout);
UINT32 sp5kMultiPlayShift(UINT32 shift, UINT32 value);
UINT32 sp5kMultiPlayInfoGet(UINT32 thumbId, UINT32 *fileType, UINT32 *status, UINT32 *paudioMemo);
void sp5kStillPlayAbort(void);
void sp5kMultiPlayAbort(void);
/*
 * Selector: SP5K_IMGEDIT_CROP
 *   param1 - x, param2 - y, param3 - width, param4 - height
 *
 * Selector: SP5K_IMGEDIT_SCALE_DOWN, SP5K_IMGEDIT_SCALE_UP
 *   param1 - width, param2 - height, param3 - pointer to sp5kImgEditRoi_t
 *
 * Selector: SP5K_IMGEDIT_ROTATE_EXIF_TAG
 *   param1 - rotate
 *
 * Selector: SP5K_IMGEDIT_SAVE
 *   param1 - file type, param2 - control flags, param3 - outWidth, param4 - outHeight
 *   file type: 0 - jpg, 1 - resource
 *   control flags:
 *     bit[0]: 0 - no new file, 1 - new file
 *     bit[1]: 0 - auto jump to new file, 1 - not auto jump
 *
 * Selector: SP5K_IMGEDIT_PHOTOFRM_FILE
 *   param1 - filename pointer
 *
 * Selector: IMGEDIT_PHOTOFRM_LOC
 *   param1 - x, param2 - y, param3 - width, param4 - height
 *
 * Selector: IMGEDIT_SPECIAL_EFFECT
 *   param1 - effect
 *
 * Selector: IMGEDIT_OSDBLEND
 *   param1 - layer id
 *
 * Selector: IMGEDIT_CUSTOMIZE
 *   param1 - only decode main
 * if para4=0: original custom callback flow
 * if para4=1: decode main to user defined maximal size directly then call img edit
 *             custom callback fuction
 *    para2=maximal output width, para3=maximal output height
 */
UINT32 sp5kImageEdit(UINT32 option, UINT32 para1, UINT32 para2, UINT32 para3, UINT32 para4);
UINT32 sp5kStillPlayImageInfoGet(UINT32 imgType, UINT32 *pbuf, UINT32 *pbufW, UINT32 *pbufH,
                                 UINT32 *proiX, UINT32 *proiY, UINT32 *proiW, UINT32 *proiH);
void sp5kImageEditCallbackSet(void *pfunc);
UINT32 sp5kStillPlayZoomInfoGet(UINT16 *pcropOffX, UINT16 *pcropOffY, UINT16 *pcropWidth, UINT16 *pcropHeight);
UINT32 sp5kImageEditExifUpdateCallbackSet(UINT32 (*pfunc)(UINT32, UINT32, void *));
/**
 *  \note
 *  callback function
 *  UINT32 CallbackFunc(UINT8 *pfilename, UINT32 *pscale, UINT32 *pbgColor)
 *  @param[in]    pfilename    pointer to input filename
 *  @param[out]   pscale       pointer to ouput scale factor, 100(0.1x) ~ 1000(1x)
 *  @param[out]   pbgColor     pointer to YUV background color (v[15:11]y[10:5]u[4:0])
 *  @return       SUCCESS (do scale) or FAIL (not do)
 */
void sp5kStillPlayScaleCallbackSet(UINT32 (*pfunc)(UINT8 *, UINT32 *, UINT32 *));

void sp5kPlayBackCallbackSet(UINT32 selector, void* pfunc);

#endif /* _SP5K_PB_API_H_ */

