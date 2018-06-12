/**************************************************************************
 *
 *       Copyright (c) 2005-2013 by iCatch Technology, Inc.
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
 **************************************************************************/
/**
 * @file sp5k_gfx_api.h
 *
 * @author Matt Wang
 */
#ifndef _SP5K_GFX_API_H_
#define _SP5K_GFX_API_H_

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "common.h"
#include "middleware/gfx_def.h"
#include "middleware/fnt_def.h"
#include "middleware/str_def.h"
#include "middleware/ime_def.h"

/**************************************************************************
 *                          C O N S T A N T S
 **************************************************************************/
/* sp5kGfxInitCfgSet(). */
typedef enum _sp5kGfxInitCfg_e {
	SP5K_GFX_INIT_DISP_SRC                       = GFX_INIT_DISP_SRC,
	SP5K_GFX_INIT_LAYER_TOT                      = GFX_INIT_LAYER_TOT,
	SP5K_GFX_INIT_FRAME_SIZE                     = GFX_INIT_FRAME_SIZE,
	SP5K_GFX_INIT_VIRTUAL_FRAME_SIZE             = GFX_INIT_VIRTUAL_FRAME_SIZE,
	SP5K_GFX_INIT_LAYER_WINDOW_OFFSET            = GFX_INIT_LAYER_WINDOW_OFFSET,
	SP5K_GFX_INIT_LAYER_WINDOW_SIZE              = GFX_INIT_LAYER_WINDOW_SIZE,
	SP5K_GFX_INIT_LAYER_VIRTUAL_WINDOW_OFFSET    = GFX_INIT_LAYER_VIRTUAL_WINDOW_OFFSET,
	SP5K_GFX_INIT_LAYER_VIRTUAL_WINDOW_SIZE      = GFX_INIT_LAYER_VIRTUAL_WINDOW_SIZE,
	SP5K_GFX_INIT_LAYER_FRAME_SIZE               = GFX_INIT_LAYER_FRAME_SIZE,
	SP5K_GFX_INIT_LAYER_FRAME_ROI_OFFSET         = GFX_INIT_LAYER_FRAME_ROI_OFFSET,
	SP5K_GFX_INIT_LAYER_FRAME_ROI_SIZE           = GFX_INIT_LAYER_FRAME_ROI_SIZE,
	SP5K_GFX_INIT_LAYER_VIRTUAL_FRAME_SIZE       = GFX_INIT_LAYER_VIRTUAL_FRAME_SIZE,
	SP5K_GFX_INIT_LAYER_VIRTUAL_FRAME_ROI_OFFSET = GFX_INIT_LAYER_VIRTUAL_FRAME_ROI_OFFSET,
	SP5K_GFX_INIT_LAYER_VIRTUAL_FRAME_ROI_SIZE   = GFX_INIT_LAYER_VIRTUAL_FRAME_ROI_SIZE,
	SP5K_GFX_INIT_LAYER_TYPE                     = GFX_INIT_LAYER_TYPE,
	SP5K_GFX_INIT_LAYER_FORMAT                   = GFX_INIT_LAYER_FORMAT,
	SP5K_GFX_INIT_LAYER_CMD_TOT                  = GFX_INIT_LAYER_CMD_TOT,
	SP5K_GFX_INIT_LAYER_ACTIVE                   = GFX_INIT_LAYER_SHOWN,
	SP5K_GFX_INIT_LAYER_WITH_ALPHA_PLANE         = GFX_INIT_LAYER_WITH_ALPHA_PLANE,
	SP5K_GFX_INIT_LAYER_WIDTH_3X                 = GFX_INIT_LAYER_WIDTH_3X,
	SP5K_GFX_INIT_CFG_NEW_TOT,

	SP5K_GFX_AB_FRAME_SIZE                       = SP5K_GFX_INIT_CFG_NEW_TOT,
	SP5K_GFX_AB_VIRTUAL_FRAME_SIZE,
	SP5K_GFX_AB_BUFFER_MODE,
	SP5K_GFX_AB_BUFFER_WITH_ALPHA_PLANE,
	SP5K_GFX_DISP_SRC,
	SP5K_GFX_USR_PAGE_ID,
	SP5K_GFX_USR_FMT,
	SP5K_GFX_USR_LAYER_DEFAULT_TYPE,
	SP5K_GFX_USR_FRAME_SIZE,
	SP5K_GFX_USR_FRAME_ROI,
	SP5K_GFX_USR_VIRTUAL_FRAME_SIZE,
	SP5K_GFX_USR_VIRTUAL_FRAME_ROI,
	SP5K_GFX_USR_WINDOW,
	SP5K_GFX_USR_OBJ_TOT,
	SP5K_GFX_USR_POOL_SIZE,
	SP5K_GFX_USR_WITH_ALPHA_PLANE,
	SP5K_GFX_USR_WIDTH_3X,
	SP5K_GFX_OSD_PAGE_TOT,
	SP5K_GFX_OSD_FMT,
	SP5K_GFX_OSD_LAYER_DEFAULT_TYPE,
	SP5K_GFX_OSD_FRAME_SIZE,
	SP5K_GFX_OSD_VIRTUAL_FRAME_SIZE,
	SP5K_GFX_OSD_WINDOW,
	SP5K_GFX_OSD_OBJ_TOT,
	SP5K_GFX_OSD_POOL_SIZE,
	SP5K_GFX_OSD_WITH_ALPHA_PLANE,
	SP5K_GFX_OSD_WIDTH_3X,
	SP5K_GFX_PIP_PAGE_TOT,
	SP5K_GFX_PIP_FMT,
	SP5K_GFX_PIP_LAYER_DEFAULT_TYPE,
	SP5K_GFX_PIP_FRAME_SIZE,
	SP5K_GFX_PIP_VIRTUAL_FRAME_SIZE,
	SP5K_GFX_PIP_WINDOW,
	SP5K_GFX_PIP_OBJ_TOT,
	SP5K_GFX_PIP_POOL_SIZE,
	SP5K_GFX_PIP_WITH_ALPHA_PLANE,
	SP5K_GFX_PIP_WIDTH_3X,
	SP5K_GFX_PF_PAGE_TOT,
	SP5K_GFX_PF_FMT,
	SP5K_GFX_PF_LAYER_DEFAULT_TYPE,
	SP5K_GFX_PF_FRAME_SIZE,
	SP5K_GFX_PF_VIRTUAL_FRAME_SIZE,
	SP5K_GFX_PF_WINDOW,
	SP5K_GFX_PF_OBJ_TOT,
	SP5K_GFX_PF_POOL_SIZE,
	SP5K_GFX_PF_WITH_ALPHA_PLANE,
	SP5K_GFX_PF_WIDTH_3X,
	SP5K_GFX_VCT_PAGE_TOT,
	SP5K_GFX_VCT_FMT,
	SP5K_GFX_VCT_LAYER_DEFAULT_TYPE,
	SP5K_GFX_VCT_FRAME_SIZE,
	SP5K_GFX_VCT_VIRTUAL_FRAME_SIZE,
	SP5K_GFX_VCT_WINDOW,
	SP5K_GFX_VCT_OBJ_TOT,
	SP5K_GFX_VCT_POOL_SIZE,
	SP5K_GFX_VCT_WITH_ALPHA_PLANE,
	SP5K_GFX_INIT_CFG_TOT,
} sp5kGfxInitCfg_e;

/* display source. */
typedef enum _sp5kGfxDispSrc_e {
	SP5K_GFX_DISP_SRC_PRIMARY   = GFX_DISP_SRC_PRIMARY,
	SP5K_GFX_DISP_SRC_SECONDARY = GFX_DISP_SRC_SECONDARY,
} sp5kGfxDispSrc_e;

/* sp5kGfxSysCfgSet */
typedef enum _sp5kGfxSysCfgId_e {
	SP5K_GFX_SYS_CFG_GLOBAL_FNT_ID_ERR_MSG,
	SP5K_GFX_SYS_CFG_PER_PIXEL_BLEND_MODE=0x1000,  /* 0:MOE blend, 1:GPE blend, default=0 */
	SP5K_GFX_SYS_CFG_SCALE_QUALITY,                /* 0:TBD, 1:GPE Scale, 2:GPE Scale, 3: GPE Bilinear Scale, 4: GPE Bilinear Scale*/
	SP5K_GFX_SYS_CFG_CONVERT_JFIF_JPEG_OUTSIDE_BUF,
	SP5K_GFX_SYS_JFIF_CUSTOM_QTABLE_CFG,
	SP5K_GFX_SYS_CALLBACK_JFIF_REENC_UPDATE,
} sp5kGfxSysCfgId_e; /* plz keep sync to gfxBaseGlobalCfgId_e. */

/* layer type. */
typedef enum _sp5kGfxLayerType_e {
	SP5K_GFX_LAYER_TYPE_BITMAP  = 0,  /* default. */
	SP5K_GFX_LAYER_TYPE_VIRTUAL = 1,
} sp5kGfxLayerType_e;

/* data format. */
typedef enum _sp5kGfxFmt_e {
	SP5K_GFX_FMT_YUV422    = 0,
	SP5K_GFX_FMT_RGB565    = 1,
	SP5K_GFX_FMT_IDX8      = 3,     /* it's also for ALPHA and 8-BIT. */
	#if SPCA6330
	SP5K_GFX_FMT_YUV420    = 8,
	#if SPCA6350
	SP5K_GFX_FMT_YUV422_Y  = 0x10,
	SP5K_GFX_FMT_YUV422_U  = 0x11,
	SP5K_GFX_FMT_YUV422_V  = 0x12,
	SP5K_GFX_FMT_YUV422_UV = 0x13,
	SP5K_GFX_FMT_YUV420_Y  = 0x14,
	SP5K_GFX_FMT_YUV420_U  = 0x15,
	SP5K_GFX_FMT_YUV420_V  = 0x16,
	SP5K_GFX_FMT_YUV420_UV = 0x17,
	#endif
	#endif
	SP5K_GFX_FMT_LINEAR_Y_LINEAR_UV = 0x100,
	SP5K_GFX_FMT_YUYV      = 0x101,
	SP5K_GFX_FMT_LINEAR_Y  = 0x102,
	SP5K_GFX_FMT_LINEAR_UV = 0x103,
	SP5K_GFX_FMT_JFIF      = 0x1000,
	SP5K_GFX_FMT_JFIF420   = SP5K_GFX_FMT_JFIF,
	SP5K_GFX_FMT_JPG       = 0x1001,
	SP5K_GFX_FMT_JPG_OUTSIDE_BUF = 0x1002,
	SP5K_GFX_FMT_JFIF422   = 0x1003,
	SP5K_GFX_FMT_RAW_16    = 0x2000,

	/* bit flag, should or with specific GFX format */
	SP5K_GFX_FMT_SCALE_JPEG = 0x40000000,
	SP5K_GFX_FMT_SCALE_IME = 0x80000000
} sp5kGfxFmt_e;  /* plz keep sync to gfxDataFmt_e. */

/* page id base, SP5K_GFX_PAGE_USR_0 is the bottomest page. */
typedef enum _sp5kGfxPageNum_e {
	SP5K_GFX_PAGE_USR_0 = 0,   /* 8 pages. */
	SP5K_GFX_PAGE_OSD_0 = 8,   /* 2 pages. */
	SP5K_GFX_PAGE_PIP_0 = 10,  /* 2 pages. */
	SP5K_GFX_PAGE_VCT_0 = 12,  /* 2 page. */
	SP5K_GFX_PAGE_PF_0  = 14,  /* 2 page. */
	SP5K_GFX_PAGE_TOT   = 16,
} sp5kGfxPageNum_e;

typedef enum _sp5kGfxMiscPageNum_e {
	SP5K_GFX_PAGE_SHOWN      = 16,  /* 1 page. */
	SP5K_GFX_PAGE_HIDDEN     = 17,  /* 1 page. */
	SP5K_GFX_PAGE_IMAGE      = 18,  /* 1 page. */
	SP5K_GFX_PAGE_PIP_SHOWN  = 19,  /* 1 page. */
	SP5K_GFX_PAGE_PIP_HIDDEN = 20,  /* 1 page. */
	SP5K_GFX_PAGE_PIP_MAIN   = 21,  /* 1 page. */
	SP5K_GFX_PAGE_PIP_BASE   = 22,  /* 8 pages. */
	SP5K_GFX_PAGE_APLANE     = 32,  /* alpha plane of SP5K_GFX_PAGE_XXX in sp5kGfxPageNum_e is (SP5K_GFX_PAGE_ALPHA + SP5K_GFX_PAGE_XXX). */
} sp5kGfxMiscPageNum_e;

/* sp5kGfxAttrSet(). */
typedef enum _sp5kGfxAttr_e {
	SP5K_GFX_TRANS_COLOR,          /* equals to set both SP5K_GFX_IDX8_TRANS_COLOR + SP5K_GFX_RGB565_TRANS_COLOR. */
	SP5K_GFX_PEN_ENABLE,           /* equals to set both SP5K_GFX_IDX8_PEN_ENABLE + SP5K_GFX_RGB565_PEN_ENABLE. */
	SP5K_GFX_PEN_COLOR,            /* equals to set both SP5K_GFX_IDX8_PEN_COLOR + SP5K_GFX_RGB565_PEN_COLOR. */
	SP5K_GFX_PEN_WIDTH,            /* equals to set both SP5K_GFX_IDX8_PEN_WIDTH + SP5K_GFX_RGB565_PEN_WIDTH. */
	SP5K_GFX_FILL_ENABLE,          /* equals to set both SP5K_GFX_IDX8_FILL_ENABLE + SP5K_GFX_RGB565_FILL_ENABLE. */
	SP5K_GFX_FILL_COLOR,           /* equals to set both SP5K_GFX_IDX8_FILL_COLOR + SP5K_GFX_RGB565_FILL_COLOR. */
	SP5K_GFX_TEXT_PITCH,           /* equals to set both SP5K_GFX_IDX8_TEXT_PITCH + SP5K_GFX_RGB565_TEXT_PITCH. */
	SP5K_GFX_PIC_SCALE_FACTOR,     /* equals to set both SP5K_GFX_IDX8_PIC_SCALE_FACTOR + SP5K_GFX_RGB565_PIC_SCALE_FACTOR. */
	SP5K_GFX_PIC_SCALE_INIT,       /* equals to set both SP5K_GFX_IDX8_PIC_SCALE_INIT + SP5K_GFX_RGB565_PIC_SCALE_INIT. */
	SP5K_GFX_BLEND_FACTOR,         /* equals to set both SP5K_GFX_IDX8_BLEND_FACTOR + SP5K_GFX_RGB565_BLEND_FACTOR. */
	SP5K_GFX_PIC_SCALE_ANTIALIAS,  /* equals to set both SP5K_GFX_IDX8_SCALE_ANTIALIAS + SP5K_GFX_RGB565_SCALE_ANTIALIAS. */
	SP5K_GFX_PIC_ROTATE,
	SP5K_GFX_LAYER_PIC_ROTATE,
	SP5K_GFX_ATTR_TOT,

	SP5K_GFX_IDX8_TRANS_COLOR = SP5K_GFX_ATTR_TOT,
	SP5K_GFX_IDX8_PEN_ENABLE,
	SP5K_GFX_IDX8_PEN_COLOR,
	SP5K_GFX_IDX8_PEN_WIDTH,
	SP5K_GFX_IDX8_FILL_ENABLE,
	SP5K_GFX_IDX8_FILL_COLOR,
	SP5K_GFX_IDX8_TEXT_PITCH,
	SP5K_GFX_IDX8_PIC_SCALE_FACTOR,
	SP5K_GFX_IDX8_PIC_SCALE_INIT,
	SP5K_GFX_IDX8_BLEND_FACTOR,
	SP5K_GFX_IDX8_SCALE_ANTIALIAS,
	SP5K_GFX_IDX8_ROTATE,
	SP5K_GFX_IDX8_LAYER_ROTATE,
	SP5K_GFX_IDX8_ATTR_TOT,

	SP5K_GFX_RGB565_TRANS_COLOR = SP5K_GFX_IDX8_ATTR_TOT,
	SP5K_GFX_RGB565_PEN_ENABLE,
	SP5K_GFX_RGB565_PEN_COLOR,
	SP5K_GFX_RGB565_PEN_WIDTH,
	SP5K_GFX_RGB565_FILL_ENABLE,
	SP5K_GFX_RGB565_FILL_COLOR,
	SP5K_GFX_RGB565_TEXT_PITCH,
	SP5K_GFX_RGB565_PIC_SCALE_FACTOR,
	SP5K_GFX_RGB565_PIC_SCALE_INIT,
	SP5K_GFX_RGB565_BLEND_FACTOR,
	SP5K_GFX_RGB565_SCALE_ANTIALIAS,
	SP5K_GFX_RGB565_ROTATE,
	SP5K_GFX_RGB565_LAYER_ROTATE,
	SP5K_GFX_RGB565_ATTR_TOT,

	SP5K_GFX_ICON_ID = SP5K_GFX_RGB565_ATTR_TOT,
	SP5K_GFX_LAYER_ICON_ID,
	SP5K_GFX_FONT_ID,
	SP5K_GFX_LAYER_FONT_ID,
	SP5K_GFX_CUSTOM_FNT_CAPABILITY,
	SP5K_GFX_CUSTOM_FNT_SIZE_CALLBACK ,
	SP5K_GFX_CUSTOM_FNT_DATA_CALLBACK,
	SP5K_GFX_TEXT_COLOR,
	SP5K_GFX_ICON_COLOR,
	SP5K_GFX_ALPHA_MAPPING,
	SP5K_GFX_YUV422_TO_RGB565_DITHER_ENABLE,
	/**
	 * \note
	      para1 = 0: disable, 1: enable.
	      para2 = 0: waiting for dirty clear, 1: no waiting.
	      para3 = 0: not, 1: disable refresh when done.
	      para4 = 0: not, 1: toggle a/b (shown/hidden) frame.
	              2: blend all layers to a/b frame (no clear) then toggle.
	              3: toggle a/b (show/hidden) frame but not do zfupdate.
	              4: blend all layers to a/b frame but not toggle.
	 */
	SP5K_GFX_REFRESH_ACTIVE,
	SP5K_GFX_REFRESH_INTERVAL,
	SP5K_GFX_ORDER_REFRESH, /*!< must be under refresh done and disable */
	SP5K_GFX_LAYER_ACTIVE,
	SP5K_GFX_LAYER_ALPHA,
	SP5K_GFX_LAYER_WINDOW_OFFSET,
	SP5K_GFX_LAYER_WINDOW_SIZE,
	SP5K_GFX_LAYER_FRAME_ROI_OFFSET,
	SP5K_GFX_LAYER_FRAME_ROI_SIZE,
	SP5K_GFX_LAYER_TYPE,
	SP5K_GFX_LAYER_RENDER_ORDER, /*!< the less number means the topper layer */
	SP5K_GFX_LAYER_ATTR_TOT,

	/**
	 * \note
	 * param1: 0=disable, 1=enable
	 * param2: 0=wait refresh done, 1=not
	 * param3: 0=not, 1:disable refresh after done
	 * param4: don't care
	 */
	SP5K_GFX_PIP_REFRESH_ACTIVE = SP5K_GFX_LAYER_ATTR_TOT,
	/**
	 * \note
	 * param1: gfx pip page id
	 * param2: 0:hide, 1: show
	 */
	SP5K_GFX_PIP_LAYER_ACTIVE,
	/**
	 * \note
	 * param1: gfx pip page id
	 * param2: alpha value, 0~32
	 */
	SP5K_GFX_PIP_LAYER_ALPHA,
	/**
	 * \note
	 * param1: gfx pip page id
	 * param2: offset x of window
	 * param3: offset y of window
	 */
	SP5K_GFX_PIP_LAYER_WINDOW_OFFSET,
	/**
	 * \note
	 * param1: gfx pip page id
	 * param2: layer frame roi x offset.
	 * param3: layer frame roi y offset.
	*/
	SP5K_GFX_PIP_LAYER_FRAME_ROI_OFFSET,
	/**
	 * \note
	 * param1: gfx pip page id
	 * param2: layer frame roi width
	 * param3: layer frame roi height.
	*/
	SP5K_GFX_PIP_LAYER_FRAME_ROI_SIZE,
	/**
	 * \note
	 * if y value is less than the threshold, the pixel will be treated
	 * as color key
	 * param1: gfx pip page id
	 * param2: y threshold from 0 to 255
	 * param3: 0:color key disable (default), 1:enable
	 */
	SP5K_GFX_PIP_LAYER_COLOR_KEY,
	SP5K_GFX_PIP_LAYER_ATTR_TOT,
	SP5K_GFX_MERGE_DEFAULT_COLOR,
	SP5K_GFX_OBJ_ATTR_BASE = 0x10000,/*0~0xffff: gfx; 0x10000~0x1ffff: gfx obj*/
} sp5kGfxAttr_e;

typedef enum _sp5kGfxIconId_e {
	SP5K_GFX_FONT_ID_1 = FNT_TYPE_FONT,
	SP5K_GFX_ICON_ID_1 = FNT_TYPE_ICON,
	SP5K_GFX_ICON_ID_2 = FNT_TYPE_ICON_2,
	SP5K_GFX_ICON_ID_3 = FNT_TYPE_ICON_3,
	SP5K_GFX_ICON_ID_4 = FNT_TYPE_ICON_4,
	SP5K_GFX_ICON_ID_5 = FNT_TYPE_ICON_5,
	SP5K_GFX_ICON_ID_6 = FNT_TYPE_ICON_6,
	SP5K_GFX_ICON_ID_7 = FNT_TYPE_ICON_7,
	SP5K_GFX_ICON_ID_8 = FNT_TYPE_ICON_8,
	SP5K_GFX_ICON_ID_9 = FNT_TYPE_ICON_9,
	SP5K_GFX_ICON_ID_10 = FNT_TYPE_ICON_10,
	SP5K_GFX_ICON_ID_11 = FNT_TYPE_ICON_11,
	SP5K_GFX_ICON_ID_12 = FNT_TYPE_ICON_12,
	SP5K_GFX_ICON_ID_13 = FNT_TYPE_ICON_13,
	SP5K_GFX_ICON_ID_14 = FNT_TYPE_ICON_14,
	SP5K_GFX_ICON_ID_15 = FNT_TYPE_ICON_15,
	SP5K_GFX_ICON_ID_16 = FNT_TYPE_ICON_16,
	SP5K_GFX_STAMP     = FNT_TYPE_STAMP,
	SP5K_GFX_FONT_ID_CUSTOM   = FNT_TYPE_FONT_CUSTOM,
	SP5K_GFX_FONT_ID_2 = 100,
} sp5kGfxIconId_e;

/* sp5kGfxIconFileDraw(), sp5kGfxStringDraw(), sp5kGfxStringIdDraw(). */
typedef enum _sp5kGfxAlign_e {
	SP5K_GFX_ALIGN_TOP_LEFT     = GFX_ALIGN_TOP_LEFT,
	SP5K_GFX_ALIGN_TOP          = GFX_ALIGN_TOP_CENTER,
	SP5K_GFX_ALIGN_TOP_RIGHT    = GFX_ALIGN_TOP_RIGHT,
	SP5K_GFX_ALIGN_LEFT         = GFX_ALIGN_LEFT,
	SP5K_GFX_ALIGN_CENTER       = GFX_ALIGN_CENTER,
	SP5K_GFX_ALIGN_RIGHT        = GFX_ALIGN_RIGHT,
	SP5K_GFX_ALIGN_BOTTOM_LEFT  = GFX_ALIGN_BOTTOM_LEFT,
	SP5K_GFX_ALIGN_BOTTOM       = GFX_ALIGN_BOTTOM_CENTER,
	SP5K_GFX_ALIGN_BOTTOM_RIGHT = GFX_ALIGN_BOTTOM_RIGHT,
} sp5kGfxAlign_e;

typedef enum _sp5kGfxRotFlip_e {
	SP5K_GFX_ROTATE_0    = 0x00,
	SP5K_GFX_ROTATE_90,
	SP5K_GFX_ROTATE_180,
	SP5K_GFX_ROTATE_270,
	SP5K_GFX_ROTATE_0_FLIP,
	SP5K_GFX_ROTATE_90_FLIP,
	SP5K_GFX_ROTATE_180_FLIP,
	SP5K_GFX_ROTATE_270_FLIP,
	SP5K_GFX_ROTATE_TOT,
	SP5K_GFX_ROTATE_EXT = GFX_ROT_EXT_FLAG
} sp5kGfxRotFlip_t;

/* sp5kGfxStringDraw(). */
typedef enum _sp5kGfxEncode_e {
	SP5K_GFX_ENCODE_ASCII   = STR_ENCODE_ASCII,
	SP5K_GFX_ENCODE_UNICODE = STR_ENCODE_UNICODE,
} sp5kGfxEncode_e;

/* sp5kGfxObjectEdit(). */
typedef enum _sp5kGfxObjEdit_e {
	SP5K_GFX_OBJECT_EDIT_CDSP,
	SP5K_GFX_OBJECT_EDIT_CDSP_RGB,
	SP5K_GFX_OBJECT_EDIT_CDSP_RGB_ROI,
	SP5K_GFX_OBJECT_EDIT_CDSP_RGB_ROI_2,
	SP5K_GFX_OBJECT_EDIT_CDSP_RGB_ONE_JOB,
	SP5K_GFX_OBJECT_EDIT_MOSAIC,
	SP5K_GFX_OBJECT_EDIT_RADIAL_BLUR,
	SP5K_GFX_OBJECT_EDIT_SOFT,
	SP5K_GFX_OBJECT_EDIT_SEPIA,
	SP5K_GFX_OBJECT_EDIT_PAINT,
	SP5K_GFX_OBJECT_EDIT_VIDEO_ANR_Y,
	SP5K_GFX_OBJECT_EDIT_VIDEO_ANR_UV,
	SP5K_GFX_OBJECT_EDIT_VIDEO_ANR_Y_UV,
	SP5K_GFX_OBJECT_EDIT_DYNAMIC_TONE,
	SP5K_GFX_OBJECT_EDIT_AIR_BALL,
	SP5K_GFX_OBJECT_EDIT_SR,
	SP5K_GFX_OBJECT_EDIT_POSTERIZE,
	SP5K_GFX_OBJECT_EDIT_CDSP_YUV_REDO,
	SP5K_GFX_OBJECT_EDIT_TOT,
} sp5kGfxObjEdit_t;

typedef enum _sp5kGfxMapMode_e{
	SP5K_GFX_MAP_MODE_STILL,
	SP5K_GFX_MAP_MODE_PREVIEW
} sp5kGfxMapMode_e;

typedef enum _sp5kGfxMapWaitMode_e{
	SP5K_GFX_MAP_MODE_DIRECT_WAIT,
	SP5K_GFX_MAP_MODE_ASYNC_WAIT
} sp5kGfxMapWaitMode_e;

/* for sp5kGfxMapMorphingParam_t effId . */
typedef enum _sp5kGfxMapEffect_e{
	SP5K_GFX_MAP_EFF_FAT    = IME_MORPHING_EFF_FAT,
	SP5K_GFX_MAP_EFF_FATTEN = IME_MORPHING_EFF_FATTEN,
	SP5K_GFX_MAP_EFF_PUNCH  = IME_MORPHING_EFF_PUNCH,

	SP5K_GFX_MAP_EFF_THIN   = IME_MORPHING_EFF_THIN,
	SP5K_GFX_MAP_EFF_SHORT  = IME_MORPHING_EFF_SHORT,
	SP5K_GFX_MAP_EFF_PINCH  = IME_MORPHING_EFF_PINCH,

	SP5K_GFX_MAP_EFF_BUBBLE = IME_MORPHING_EFF_BUBBLE,

	SP5K_GFX_MAP_EFF_WAVE1  = IME_MORPHING_EFF_WAVE1,
	SP5K_GFX_MAP_EFF_WAVE2  = IME_MORPHING_EFF_WAVE2,
	SP5K_GFX_MAP_EFF_WAVE3  = IME_MORPHING_EFF_WAVE3,

	SP5K_GFX_MAP_EFF_TWIST  = IME_MORPHING_EFF_TWIST,

	SP5K_GFX_MAP_EFF_LMIRROR = IME_MORPHING_EFF_LMIRROR,
	SP5K_GFX_MAP_EFF_RMIRROR = IME_MORPHING_EFF_RMIRROR
} sp5kGfxMapEffect_e;

typedef enum _sp5kGfxMapColorTransMode_e{
	SP5K_GFX_MAP_COLOR_TRANS_BYPASS,
	SP5K_GFX_MAP_COLOR_TRANS_YUV_LUT
} sp5kGfxMapColorTransMode_e;

typedef enum _sp5kGfxMapColorTransLut_e{
	SP5K_GFX_MAP_COLOR_TRANS_LUT_Y = 1,
	SP5K_GFX_MAP_COLOR_TRANS_LUT_U = 2,
	SP5K_GFX_MAP_COLOR_TRANS_LUT_V = 4
} sp5kGfxMapColorTransLut_e;

typedef enum _sp5kGfxMappingItp_e{
	SP5K_GFX_MAP_ITP_NO_INTERPOLATION,
	SP5K_GFX_MAP_ITP_NEAREST_NEIGHBOR,
	SP5K_GFX_MAP_ITP_BILINEAR,
	SP5K_GFX_MAP_ITP_HERMITE
} sp5kGfxMappingItp_e;

typedef enum _sp5kGfxMappingId_e{
	/**
	 * \note
	 * param1: struct sp5kGfxMappingAttr_t
	 */
	SP5K_GFX_MAPPING_ID_COLOR_TRANS,

	/**
	 * \note
	 * param1: struct sp5kGfxScatterParam_t
	 */
	SP5K_GFX_MAPPING_ID_SCATTER,

	/**
	 * \note
	 * param1: struct sp5kGfxProjectParam_t
	 */
	SP5K_GFX_MAPPING_ID_PROJECT,

	/**
	 * \note
	 * param1: struct sp5kGfxMorphingParam_t
	 */
	SP5K_GFX_MAPPING_ID_MORPHING
} sp5kGfxMappingId_e;

typedef enum _sp5kGfxFt2Cfg_e {
	SP5K_GFX_FT2_CHAR_SIZE,
} sp5kGfxFt2AttrCfg_e;

typedef enum _sp5kImageEffectAirBallMode_e{
	SP5K_IMG_EFF_AIR_BALL_STILL,
	SP5K_IMG_EFF_AIR_BALL_VIDEO_MAIN,
	SP5K_IMG_EFF_AIR_BALL_VIDEO_SEC
} sp5kImageEffectAirBallMode_e;

typedef enum _sp5kImageEffectAirBallAct_e{
	SP5K_IMG_EFF_AIR_BALL_INIT,
	SP5K_IMG_EFF_AIR_BALL_DO,
	SP5K_IMG_EFF_AIR_BALL_FINISH,
	SP5K_IMG_EFF_AIR_BALL_CFG,
} sp5kImageEffectAirBallAct_e;

/*gfx scale quality */
typedef enum {
	SP5K_GFX_SCALE_QUALITY_FASTEST=0,	/*TBD*/
	SP5K_GFX_SCALE_QUALITY_FAST,		/*use GPE Scale*/
	SP5K_GFX_SCALE_QUALITY_NORMAL,		/*use GPE Scale*/
	SP5K_GFX_SCALE_QUALITY_GOOD,		/*use GPE Bilinear Scale*/
	SP5K_GFX_SCALE_QUALITY_BEST,		/*use GPE Bilinear Scale*/
	SP5K_GFX_SCALE_QUALITY_MAX,
} sp5kGfxScaleQuality_e;

/**************************************************************************
 *                          D A T A   T Y P E S
 **************************************************************************/
/* sp5kGfxPolygonDraw() */
typedef struct _sp5kGfxPoint_t {
	SINT32 iX;
	SINT32 iY;
} sp5kGfxPoint_t;

/* sp5kGfxPageCapabilityGet() */
typedef struct _sp5kGfxPageCapabDesc_t {
	UINT8 *pbuf;     /* layer frame buffer. */
	UINT32 frmW;     /* frame buffer width, set by SP5K_GFX_INIT_LAYER_FRAME_SIZE. */
	UINT32 frmH;     /* frame buffer height, set by SP5K_GFX_INIT_LAYER_FRAME_SIZE. */
	SINT32 frmRoiX;  /* frame buffer roi x offset, set by SP5K_GFX_INIT_LAYER_FRAME_ROI_OFFSET or SP5K_GFX_LAYER_FRAME_ROI_OFFSET. */
	SINT32 frmRoiY;  /* frame buffer roi y offset, set by SP5K_GFX_INIT_LAYER_FRAME_ROI_OFFSET or SP5K_GFX_LAYER_FRAME_ROI_OFFSET. */
	UINT32 frmRoiW;  /* frame buffer roi width, set by SP5K_GFX_INIT_LAYER_FRAME_ROI_SIZE or SP5K_GFX_LAYER_FRAME_ROI_SIZE. */
	UINT32 frmRoiH;  /* frame buffer roi height, set by SP5K_GFX_INIT_LAYER_FRAME_ROI_SIZE or SP5K_GFX_LAYER_FRAME_ROI_SIZE. */
	SINT32 winX;     /* window x offset on a/b frame, set by SP5K_GFX_INIT_LAYER_WINDOW_OFFSET or SP5K_GFX_LAYER_WINDOW_OFFSET. */
	SINT32 winY;     /* window y offset on a/b frame, set by SP5K_GFX_INIT_LAYER_WINDOW_OFFSET or SP5K_GFX_LAYER_WINDOW_OFFSET. */
	UINT32 winW;     /* window width on a/b frame, set by SP5K_GFX_INIT_LAYER_WINDOW_SIZE or SP5K_GFX_LAYER_WINDOW_SIZE. */
	UINT32 winH;     /* window height on a/b frame, set by SP5K_GFX_INIT_LAYER_WINDOW_SIZE or SP5K_GFX_LAYER_WINDOW_SIZE. */
	UINT32 type;     /* one of sp5kGfxLayerType_e, set by SP5K_GFX_INIT_LAYER_TYPE. */
	UINT32 fmt;      /* one of sp5kGfxFmt_e, set by SP5K_GFX_INIT_LAYER_FORMAT. */
	UINT32 isShow;   /* 0: layer hidden, 1: layer shown, set by SP5K_GFX_INIT_LAYER_ACTIVE or SP5K_GFX_LAYER_ACTIVE. */
	UINT32 alpha;    /* layer alpha, set by SP5K_GFX_LAYER_ALPHA. */
	UINT8 *pabuf;    /* alpha frame buffer */
} sp5kGfxPageCapabDesc_t;

typedef struct _sp5kGfxPicture_t {
	UINT8 *pbuf;  /* picture frame buffer. */
	UINT32 fmt;   /* one of sp5kGfxFmt_e. */
	UINT32 frmW;  /* frame buffer width. */
	UINT32 frmH;  /* frame buffer height. */
	SINT32 roiX;  /* frame buffer roi x offset. */
	SINT32 roiY;  /* frame buffer roi y offset. */
	UINT32 roiW;  /* frame buffer roi width. */
	UINT32 roiH;  /* frame buffer roi height. */
} sp5kGfxPicture_t;  /* plz keep sync to gfxPicture_t. */

typedef struct _sp5kGfxRoi_t {
	SINT32 roiX;
	SINT32 roiY;
	UINT32 roiW;
	UINT32 roiH;
} sp5kGfxRoi_t;

typedef struct _sp5kGfxObj_t {
	UINT8 *pbuf;
	UINT8 *paplaneBuf;
	UINT32 bufW;
	UINT32 bufH;
	SINT32 roiX;
	SINT32 roiY;
	UINT32 fmt;   /* one of sp5kGfxFmt_e. */
	UINT32 roiW;
	UINT32 roiH;
} sp5kGfxObj_t;  /* note that roiW & roiH is in sp5kGfxObj_t only. changing gfxObj_t & halXxxObj_t is too risky.
plz keep halGpeObj_t, halGteObj_t and gfxObj_t all in same data structure except roiW & roiH. */

typedef struct _sp5kGfxLayer_t {
	UINT8 *pbuf;          /* layer frame buffer. */
	UINT8 *paplaneBuf;    /* alpha plane frame buffer. */
	UINT32 rsv1[3];
	UINT32 frmW;          /* frame buffer width. */
	UINT32 frmH;          /* frame buffer height. */
	UINT32 rsv2[4];
	SINT32 frmRoiX;       /* frame buffer roi x offset. */
	SINT32 frmRoiY;       /* frame buffer roi y offset. */
	UINT32 frmRoiW;       /* frame buffer roi width. */
	UINT32 frmRoiH;       /* frame buffer roi height. */
	UINT32 rsv3;
	SINT32 winX;          /* window x offset on a/b frame. */
	SINT32 winY;          /* window y offset on a/b frame. */
	UINT32 winW;          /* window width on a/b frame. */
	UINT32 winH;          /* window height on a/b frame. */
	UINT32 rsv4[2];
	UINT32 fmt;           /* one of gfxDataFmt_e. */
	UINT32 rsv5[4];
	UINT32 alpha;         /* layer alpha. */
	UINT32 isWithAplane;  /* 0: without, 1: with alpha plane. */
	UINT32 rsv6[3];
} sp5kGfxLayer_t;
/* plz keep sp5kGfxLayer_t compatible with gfxLayer_t */

typedef struct _sp5kGfxMapProjVect_t{
	VECT_t src[4];  /* source image vector */
	VECT_t dst[4];  /* destination image vector */
} sp5kGfxMapProjVect_t;

typedef struct _sp5kGfxMapMorphingTRoi_t{
	UINT8  xEn;      /* Transform ROI x-direction enable : 1, disable : 0 (Full ROI) */
	UINT16 xStr;     /* x starting value */
	UINT16 xEnd;     /* x ending value */

	UINT8  yEn;      /* Transform ROI y-direction enable : 1, disable : 0 (Full ROI) */
	UINT16 yStr;     /* y starting value */
	UINT16 yEnd;     /* y ending value */

	UINT8  rEn;      /* Transform ROI r-direction enable : 1, disable : 0 (Full ROI) */

	UINT16 xc;       /* x center point */
	UINT16 yc;       /* y center point */
	UINT32 radiusA;  /* radius of x-direction */
	UINT32 radiusB;  /* radius of y-direction */
} sp5kGfxMapMorphingTRoi_t;

typedef struct _sp5kGfxMappingAttr_t {
	UINT8  itpAlg;    /* interpolation algorithm (for mapping only), one of sp5kGfxMappingItp_e */
	UINT8  ctMode;    /* Color transform mode (for color transform only), one of sp5kGfxMapColorTransMode_e */
	UINT8  ctLut;     /* Color transform LUT setting for transform single Y, U, V or all (for color transform only), select from sp5kGfxColorTransLut_e */
	UINT32 *pctTblY;  /* Color transform Y LUT table */
	UINT32 *pctTblU;  /* Color transform U LUT table */
	UINT32 *pctTblV;  /* Color transform V LUT table */
	UINT8  bgClrY;    /* back ground color Y value */
	UINT8  bgClrU;    /* back ground color U value */
	UINT8  bgClrV;    /* back ground color V value */
	UINT32 waitMode;
} sp5kGfxMappingAttr_t;

typedef struct _sp5kGfxMapScatterParam_t {
	sp5kGfxMappingAttr_t mapAttr;
	UINT16 xRange;
	UINT16 yRange;
} sp5kGfxMapScatterParam_t;

typedef struct _sp5kGfxMapProjectParam_t {
	sp5kGfxMappingAttr_t mapAttr;
	sp5kGfxMapProjVect_t vec;
} sp5kGfxMapProjectParam_t;

typedef struct _sp5kGfxMapMorphingParam_t {
	sp5kGfxMappingAttr_t mapAttr;
	UINT8 fullTRoiEn;  /* enable this flag means FULL TROI, no need to set troiParam */
	sp5kGfxMapMorphingTRoi_t troiParam;
	UINT8 effId;
	UINT8 level;
} sp5kGfxMapMorphingParam_t;


typedef struct _sp5kGfxObjEditSoftCfg_t {
	UINT32 sclFac1; /*0 ~ 32 (detail level, suggest 16,  must less than soften level)*/
	UINT32 sclFac2; /*0 ~ 32 (soften level, suggest 24)*/
} sp5kGfxObjEditSoftCfg_t;


typedef struct _sp5kGfxObjEditSepiaCfg_t {
	UINT32 uVal; /* 0~255, u value */
	UINT32 vVal; /* 0~255, v value */
} sp5kGfxObjEditSepiaCfg_t;


typedef struct _sp5kGfxObjEditPaintCfg_t {
	UINT32 wdres;  /* 0(weak)~255(strong), luma of the whole image */
	UINT32 wdrqfh; /* 0(weak)~255(strong), brighten the drak part of image */
	UINT32 haloSize; /* 0(weak)~31(strong), degree of halo */
	UINT32 detailGain; /*range: 0(weak) ~ 63(strong), suggest: 15*/
	UINT32 toneGain; /*range: 0(weak) ~ 15(strong), suggest: 4*/
	int toneOfs;  /*1s5b*/	/*range: -32(dark) ~ 31(bright), suggest: 10*/
} sp5kGfxObjEditPaintCfg_t;


typedef struct _sp5kGfxObjEditVideoCfg_t {
	SINT32 iRthr1;
	SINT32 iRthr2;
	SINT32 iRthr2v;
	SINT32 iRthr3;
	SINT32 iRthr3v;
	SINT32 iLthr1;
	SINT32 iLthr2;
	SINT32 iLthr2v;
	SINT32 iLthr3;
	SINT32 iLthr3v;
	UINT32 mskStp;
} sp5kGfxObjEditVideoCfg_t;

typedef struct _sp5kGfxObjEditDynamicToneCfg_t {
	UINT32 sclFac;  		/*7b*/	/*range: 0(weak) ~ 127(strong), suggest 60*/
	UINT32 haloLevel;  	/*3b*/	/*range: 0(weak) ~ 7(strong), suggest 4*/
} sp5kGfxObjEditDynamicToneCfg_t;

typedef struct _sp5kGfxObjEditAirBallCfg_t {
	UINT32 radiusBubble;
	UINT8 fltLevel;
	UINT8 fgSclLevel;
	UINT8 bgSclLevel;
} sp5kGfxObjEditAirBallCfg_t;

typedef struct sp5kPlaybackSR_s {
	UINT32 srcAddr; 	/* src img addr*/ 
	UINT32 srcBufW;	/* src img width */
	UINT32 srcBufH;	/* src img height */
	UINT32 srcRoiX;	/* src img width */
	UINT32 srcRoiY;	/* src img height */
	UINT32 dstAddr; 	/* dst img addr*/ 
	UINT32 dstBufW;	/* dst img width */
	UINT32 dstBufH;	/* dst img height */
	UINT32 dstRoiX;	/* dst img width */
	UINT32 dstRoiY;	/* dst img height */
	UINT32 tmpBufAddr;	/* SR tmp addr*/
} sp5kPlaybackSR_t;
typedef struct _sp5kGfxObjEditPosterizeCfg_t {
	UINT8 cmpNum;       /* 8b;	range:0,1(off), 2(more contour)~255(less contour);	suggest 51; compression number */
	UINT8 ptrzLevel;	/* 1.7b;	range:0(off)~128(strong);	suggest 86; posterize level */
	UINT8 satGain; 	    /* 2.5b;	range:0(off)~127(strong);	suggest 64; saturation gain */
} sp5kGfxObjEditPosterizeCfg_t;

/**************************************************************************
 *              F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
/* sp5k_gfx_init.c */
UINT32 sp5kGfxInitCfgSet(UINT32 paraId, UINT32 para1, UINT32 para2, UINT32 para3, UINT32 para4);
UINT32 sp5kGfxInitCfgGet(UINT32 paraId, UINT32 *ppara1, UINT32 *ppara2, UINT32 *ppara3,	UINT32 *ppara4);
UINT32 sp5kGfxInit(void);

/* sp5k_gfx_cfg.c */
UINT32 sp5kGfxSysCfgSet(UINT32 paraId, UINT32 para1, UINT32 para2, UINT32 para3, UINT32 para4);

/* sp5k_gfx_page.c */
UINT32 sp5kGfxPageClear(UINT32 pageId, UINT32 color);
UINT32 sp5kGfxPageCopy(UINT32 srcPageId, UINT32 dstPageId);
UINT32 sp5kGfxPageScale(UINT32 srcPageId, UINT32 dstPageId, SINT32 srcRoiX, SINT32 srcRoiY, UINT32 srcRoiW, UINT32 srcRoiH, SINT32 dstRoiX, SINT32 dstRoiY, UINT32 dstRoiW, UINT32 dstRoiH);
UINT32 sp5kGfxPageCapabilityGet(UINT32 pageId, sp5kGfxPageCapabDesc_t *pcapabDesc);

/* sp5k_gfx_attr.c */
UINT32 sp5kGfxAttrSet(UINT32 paraId, UINT32 para1, UINT32 para2, UINT32 para3, UINT32 para4);

/* sp5k_gfx_icon.c */
UINT32 sp5kGfxIconDraw(UINT32 pageId, SINT32 x, SINT32 y, UINT32 align, UINT32 iconId);
UINT32 sp5kGfxIconFileDraw(UINT32 pageId, SINT32 x, SINT32 y, UINT32 align, UINT8 *fileName, UINT32 iconId);
UINT32 sp5kGfxIconSizeGet(UINT32 iconId, UINT32 *pwidth, UINT32 *pheight);
UINT32 sp5kGfxPictureScale(UINT32 pageId, SINT32 roiX, SINT32 roiY, UINT32 roiW, UINT32 roiH, sp5kGfxPicture_t *ppic);
UINT32 sp5kGfxIconColorSet(UINT8 *colorTbl, UINT32 begIdx, UINT32 len);
UINT32 sp5kGfxIconColorGet(UINT8 *colorTbl, UINT32 begIdx, UINT32 len);
UINT32 sp5kGfxPictureBlend(sp5kGfxPicture_t *pdstPic, UINT32 alpha, UINT32 keyEn, UINT32 colorKey, sp5kGfxPicture_t *psrcPic);

/* sp5k_gfx_rect.c */
UINT32 sp5kGfxRectDraw(UINT32 pageId, SINT32 x0, SINT32 y0, SINT32 x1, SINT32 y1);
UINT32 sp5kGfxRectFillDirect(UINT32 pageId, SINT32 x, SINT32 y, UINT32 width, UINT32 height, UINT16 color);
/**
 * \note
 * only for SP5K_GFX_LAYER_TYPE_BITMAP page
 * x:     offset x, multiple of 2 if YUV format
 * y:     offset y
 * width: roi width, multiple of 2 if YUV format
 * height: roi height
 * color: YUV422/RGB565/IDX8
 *        YUV422 => V:[23~16], Y:[15~8], U:[7~0]
 */
UINT32 sp5kGfxRectFill(UINT32 pageId, SINT32 x, SINT32 y, UINT32 width, UINT32 height, UINT32 color);
/**
 * \note
 * only for the layer whose format is SP5K_GFX_FMT_RGB565
 * ppvRoi    virtual roi of pv frame, the range is from 0 to 1024
 * ppageRoi  physical roi of the selected page
 */
UINT32 sp5kGfxPvFrameScale(UINT32 pageId,sp5kGfxRoi_t *ppvRoi, sp5kGfxRoi_t *ppageRoi);

/* sp5k_gfx_round_rect.c */
UINT32 sp5kGfxRoundRectDraw(UINT32 pageId, SINT32 x0, SINT32 y0, SINT32 x1, SINT32 y1, UINT32 radiusX, UINT32 radiusY);

/* sp5k_gfx_str.c */
UINT32 sp5kGfxStringDraw(UINT32 pageId, SINT32 x, SINT32 y, UINT32 align, UINT32 encodeType, UINT8 *str);
UINT32 sp5kGfxStringIdDraw(UINT32 pageId, SINT32 x, SINT32 y, UINT32 align, UINT32 strId);
UINT32 sp5kGfxStringIdStringGet(UINT32 strId, UINT32 *plangId, UINT32 *pencodeType, UINT8 **pstr);
UINT32 sp5kGfxStringSizeGet(UINT32 encodeType, UINT8 *str, UINT32 *pwidth, UINT32 *pheight);
UINT32 sp5kGfxStringIdSizeGet(UINT32 strId, UINT32 *pwidth, UINT32 *pheight);
UINT32 sp5kGfxCharDraw(UINT32 pageId, SINT32 x, SINT32 y, UINT32 align, UINT32 chrId);
UINT32 sp5kGfxFontSizeGet(UINT32 fntId, UINT32 *pwidth, UINT32 *pheight);
UINT32 sp5kGfxTextColorSet(UINT8 *colorTbl, UINT32 begIdx, UINT32 len);
UINT32 sp5kGfxTextColorGet(UINT8 *colorTbl, UINT32 begIdx, UINT32 len);

UINT32 sp5kGfxFt2CfgSet( UINT32 paramId, ... );
UINT32 sp5kGfxFt2CacheInit( UINT32 cacheSize );
UINT32 sp5kGfxFt2CahceFont( UINT8* str, UINT32 encodeType );
UINT32 sp5kGfxFt2Done();

/* sp5k_gfx_pixel.c */
UINT32 sp5kGfxPixelDraw(UINT32 pageId, SINT32 x, SINT32 y);

/* sp5k_gfx_line.c */
UINT32 sp5kGfxLineDraw(UINT32 pageId, SINT32 x0, SINT32 y0, SINT32 x1, SINT32 y1);

/* sp5k_gfx_circle.c */
UINT32 sp5kGfxCircleDraw(UINT32 pageId, SINT32 x, SINT32 y, SINT32 radius);
UINT32 sp5kGfxEllipseDraw(UINT32 pageId, SINT32 x, SINT32 y, SINT32 radiusX, SINT32 radiusY);

/* sp5k_gfx_polygon.c */
UINT32 sp5kGfxPolygonDraw(UINT32 pageId, SINT32 x0, SINT32 y0, sp5kGfxPoint_t *points, UINT32 tot);

/* sp5k_gfx_obj.c */
UINT32 sp5kGfxObjectFill(sp5kGfxObj_t *pobj, UINT32 color);
UINT32 sp5kGfxObjectScale(sp5kGfxObj_t *psrcObj, sp5kGfxObj_t *pdstObj);
UINT32 sp5kGfxObjectBilinearScale(sp5kGfxObj_t *psrcObj, sp5kGfxObj_t *pdstObj);
UINT32 sp5kGfxObjectPerPixelBlend(sp5kGfxObj_t *psrc1Obj, sp5kGfxObj_t *psrc2Obj, sp5kGfxObj_t *pdstObj);
UINT32 sp5kGfxObjectConvert(sp5kGfxObj_t *psrcObj, sp5kGfxObj_t *pdstObj);
UINT32 sp5kGfxObjectEdit(sp5kGfxObj_t *psrcObj, sp5kGfxObj_t *pdstObj, UINT32 editId, UINT32 rsv, ...);
UINT32 sp5kGfxObjectSave(sp5kGfxObj_t *pobj, UINT32 rotate, UINT32 jpgW, UINT32 jpgH, UINT8 *pfname);
UINT32 sp5kGfxObjectDraw(UINT32 pageId, sp5kGfxRoi_t *proi, UINT32 rotate, sp5kGfxObj_t *pobj);

/* sp5k_gfx_obj_blend.c */
UINT32 sp5kGfxObjectBlend(sp5kGfxObj_t *psrc1Obj, sp5kGfxObj_t *psrc2Obj, sp5kGfxObj_t *pdstObj, UINT32 alpha);

/* sp5k_gfx_obj_copy.c */
UINT32 sp5kGfxObjectCopy(sp5kGfxObj_t *psrcObj, sp5kGfxObj_t *pdstObj);

/* sp5k_gfx_obj_gamma.c */
UINT32 sp5kGfxObjectGammaCorrect(sp5kGfxObj_t *psrcObj, sp5kGfxObj_t *pdstObj, void *pyLut);

/* sp5k_gfx_obj_rect_to_trap.c */
UINT32 sp5kGfxObjectRectangleToTrapeziumTransform(sp5kGfxObj_t *psrcObj, sp5kGfxObj_t *pdstObj, SINT32 ax, SINT32 ay, SINT32 bx, SINT32 by, SINT32 cx, SINT32 cy, SINT32 dx, SINT32 dy);

/* sp5k_gfx_obj_rot.c */
UINT32 sp5kGfxObjectRotate(sp5kGfxObj_t *psrcObj, sp5kGfxObj_t *pdstObj, UINT32 rotate);
UINT32 sp5kGfxObjectFreeRotate(sp5kGfxObj_t *psrcObj, sp5kGfxObj_t *pdstObj, int xc, int yc, int angle, double scale); /* (xc,yc): rotate center, angle unit=degree */

/* sp5k_gfx_obj_trap_to_rect.c */
UINT32 sp5kGfxObjectTrapeziumToRectangleTransform(sp5kGfxObj_t *psrcObj, sp5kGfxObj_t *pdstObj, SINT32 ax, SINT32 ay, SINT32 bx, SINT32 by, SINT32 cx, SINT32 cy, SINT32 dx, SINT32 dy);

/* sp5k_gfx_obj_yuv_avg.c */
UINT32 sp5kGfxObjectYuvAverageGet(sp5kGfxObj_t *pobj, UINT32 *py, UINT32 *pu, UINT32 *pv);
UINT32 sp5kGfxObjectYuvBlockAverageGet(sp5kGfxObj_t *pobj, UINT32 blkW, UINT32 blkH, UINT8 *pbufY, UINT8 *pbufU, UINT8 *pbufV);

/* sp5k_gfx_compatible.c : note that APIs moved here are kept for compatible only, should be removed later */
UINT32 sp5kGfxObjectColorFill(sp5kGfxObj_t *pobj, UINT32 roiW, UINT32 roiH, UINT32 color);
UINT32 sp5kGfxColorConvert(sp5kGfxPicture_t *psrcPic, sp5kGfxPicture_t *pdstPic);
UINT32 sp5kGfxYuvLineDraw(UINT8 *pyuvBuf, UINT32 yuvWidth, UINT32 yuvHeight, SINT32 x0, SINT32 y0, SINT32 x1, SINT32 y1, UINT32 yuvColor, UINT16 penWidth);

/* sp5k_gfx_ime.c */
UINT32 sp5kGfxObjectMapping(sp5kGfxObj_t *psrcObj, sp5kGfxObj_t *pdstObj, UINT32 mapMode, UINT32 paraId, UINT32 para1);
UINT32 sp5kGfxObjectMappingAsyncWait(UINT32 ms);

UINT32 sp5kGfxObjectAffineTransform(sp5kGfxObj_t *psrcObj, sp5kGfxObj_t *pdstObj, float a, float b, float c, float d, float e, float f);

UINT32 sp5kGfxObjectHistGet(sp5kGfxObj_t *pobj, UINT32 *hist);
UINT32 sp5kGfxObjectYAvgGet(sp5kGfxObj_t *pobj, UINT32 *yavg);

/* sp5k_gfx_obj_360_blend.c */
void sp5kGfxObj360Blend(sp5kGfxObj_t *Lobj, sp5kGfxObj_t *Robj, sp5kGfxObj_t *pdstObj, UINT8 blendingW);


#endif  /* _SP5K_GFX_API_H_ */

