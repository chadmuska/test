/**************************************************************************
 *
 *       Copyright (c) 2005-2014 by iCatch Technology, Inc.
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
 * @file sp5k_disp_api.h
 *
 * @author Matt Wang
 */
#ifndef _SP5K_DISP_API_H_
#define _SP5K_DISP_API_H_

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "common.h"
#include "middleware/disp_def.h"

/**************************************************************************
 *                          C O N S T A N T S
 **************************************************************************/
/** @brief Disp channel ID. */
typedef enum _sp5kDispChnlId_e {
	SP5K_DISP_CHNL_NULL = DISP_CHNL_NULL,
	SP5K_DISP_CHNL_0    = DISP_CHNL_0,
	SP5K_DISP_CHNL_1    = DISP_CHNL_1,
} sp5kDispChnlId_e;

/** @brief Disp source ID. */
typedef enum _sp5kDispSrcId_e {
	SP5K_DISP_SRC_NULL = DISP_SRC_NULL,
	SP5K_DISP_SRC_0    = DISP_SRC_0,
	SP5K_DISP_SRC_1    = DISP_SRC_1,
} sp5kDispSrcId_e;

/** @brief Disp device ID. */
typedef enum _sp5kDispDevId_e {
	SP5K_DISP_DEV_NULL            = DISP_DEV_NULL,

	SP5K_DISP_CHNL_0_PANEL        = DISP_CHNL_0_PANEL,
	SP5K_DISP_CHNL_0_DEV_TOT      = DISP_CHNL_0_DEV_TOT,

	SP5K_DISP_CHNL_1_PANEL        = DISP_CHNL_1_PANEL,
	SP5K_DISP_CHNL_1_COMPOSITE_TV = DISP_CHNL_1_COMPOSITE_TV,
	SP5K_DISP_CHNL_1_HDMI_TV      = DISP_CHNL_1_HDMI_TV,
	SP5K_DISP_CHNL_1_DEV_TOT      = DISP_CHNL_1_DEV_TOT,
} sp5kDispDevId_e;

/** @brief Disp parameter ID, para1 ~ para4 are all SINT32. */
typedef enum _sp5kDispParaId_e {
	/*
	  For sp5kDispAttrSet():

	  para1 = r value, 0~255.
	  para2 = g value, 0~255.
	  para3 = b value, 0~255.
	*/
	SP5K_DISP_BG_COLOR,
	/*
	  For sp5kDispAttrSet():

	  para1 = brightness, -128 < bri < +128.
	*/
	SP5K_DISP_BRIGHT,
	/*
	  For sp5kDispAttrSet():

	  para1 = contrast, 0 < con < 510.
	*/
	SP5K_DISP_CONTRAST,
	/*
	  For sp5kDispAttrSet():

	  para1 = hue, -30 < hue < +30.
	*/
	SP5K_DISP_HUE,
	/*
	  For sp5kDispAttrSet():

	  para1 = saturation, 0 < sat < 510.
	*/
	SP5K_DISP_SATURATION,
	/*
	  For sp5kDispAttrSet():

	  para1 = matrix.
	  para2 = offset.
	*/
	SP5K_DISP_COLOR_MATRIX,
	/*
	  For sp5kDispAttrSet():

	  para1 = enhancement type, 0~2.
	  para2 = sharpness gain, 1.3 unsigned, 0: 0 ~ 15: 1.875.
	  para3 = hcore, 0~3, real value is 2 * para3.
	  para4 = hclp, 0~3, real value is 16 * (2 ^ para4).
	*/
	SP5K_DISP_EDGE_ENHANCE,
	/*
	  For sp5kDispAttrSet():

	  para1 = 0: inactive, 1: active.
	*/
	SP5K_DISP_GLOB_ACTIVE,
	/*
	  For sp5kDispAttrSet():

	  para1 = 0: inactive, 1: active.
	*/
	SP5K_DISP_IMG_ACTIVE,
	/*
	  For sp5kDispAttrSet():

	  para1 = 0: no flip, 1: flip in vertical direction.
	  para2 = 0: no flip, 1: flip in horizontal direction.
	*/
	SP5K_DISP_IMG_FLIP,
	/*
	  For sp5kDispAttrSet():

	  para1 = x offset.
	  para2 = y offset.
	  para3 = width.
	  para4 = height.
	*/
	SP5K_DISP_IMG_WINDOW,
	/*
	  For sp5kDispAttrSet():

	  para1 = x offset.
	  para2 = y offset.
	  para3 = width.
	  para4 = height.
	*/
	SP5K_DISP_IMG_FRAME_ROI,
	/*
	  For sp5kDispAttrSet():

	  para1 = x offset.
	  para2 = y offset.
	  para3 = width.
	  para4 = height.
	*/
	SP5K_DISP_IMG_PV_WINDOW_SYNC,
	/*
	  For sp5kDispAttrGet():

	  *ppara1 = The x offset relative to the whole preview frame buffer.
	  *ppara2 = The y offset relative to the whole preview frame buffer.
	  *ppara3 = The width of cropped region.
	  *ppara4 = The height of corpped region.

	  This function returns the variable in preview modesw which might be updated
	  to hardware already or not. In face tracking application, it doesn't need the
	  correct region but need almost correct region.
	*/
	SP5K_DISP_IMG_PV_REGION,
	/*
	  For sp5kDispAttrSet():

	  para1 = 0: unfreeze, 1: freeze.
	*/
	SP5K_DISP_IMG_PREVIEW_FREEZE,
	/*
	  For sp5kDispAttrSet():

	  para1 = 0: inactive, 1: active.
	*/
	SP5K_DISP_OSD_ACTIVE,
	/*
	  For sp5kDispAttrSet():

	  para1 = 0: no flip, 1: flip in vertical direction.
	  para2 = 0: no flip, 1: flip in horizontal direction.
	*/
	SP5K_DISP_OSD_FLIP,
	/*
	  For sp5kDispAttrSet():

	  para1 = x offset.
	  para2 = y offset.
	  para3 = width.
	  para4 = height.
	*/
	SP5K_DISP_OSD_WINDOW,
	/*
	  For sp5kDispAttrSet():

	  para1 = id, one of sp5kDispColorKeyId_e.
	  para2 = color key.
	  para3 = alpha value.
	*/
	SP5K_DISP_OSD_COLOR_KEY,
	/*
	  For sp5kDispAttrSet():

	  para1 = Blending table, 256 entries, but finally get 4 entries at most.
	*/
	SP5K_DISP_OSD_BLEND_TABLE,
	/*
	  For sp5kDispAttrSet():

	  para1 = Palette table.
	  para2 = Begin index.
	  para3 = Length.
	*/
	SP5K_DISP_OSD_PALETTE,
	/*
	  For sp5kDispAttrSet():

	  para1 = width.
	  para2 = height.
	*/
	SP5K_DISP_VIRTUAL_WINDOW_SIZE,
	/*
	  For sp5kDispAttrGet():

	  *ppara1 = The returned width.
	  *ppara2 = The returned height.

	  Get dimenstion inforamtion of the current display device.
	*/
	SP5K_DISP_DEV_DIMENSION,
	/*
	  For sp5kDispAttrSet():

	  para1 = hue value (0~255).
	*/
	SP5K_DISP_TV_HUE,
	/*
	  For sp5kDispAttrSet():

	  para1 = saturation value (0~511).

	*/
	SP5K_DISP_TV_SATURATION,
	/*
	  For sp5kDispAttrSet():

	  para1 = sync level value (0~255).

	*/
	SP5K_DISP_TV_SYNC_LEVEL,
	/*
	  For sp5kDispAttrSet():

	  para1 = blank level value (0~511).
	*/
	SP5K_DISP_TV_BLANK_LEVEL,
	/*
	  For sp5kDispAttrSet():

	  para1 = black level value (0~511).
	*/
	SP5K_DISP_TV_BLACK_LEVEL,
	/*
	  For sp5kDispAttrSet():

	  para1 = Device command mode which is one of sp5kDispDevCmd_e.
	*/
	SP5K_DISP_DEV_CMD,
	/*
	  For sp5kDispAttrGet():

	  *ppara1 = Frame buffer start address.
	  *ppara2 = Frame width.
	  *ppara3 = Frame height.
	*/
	SP5K_DISP_PB_FRAME_BUF,
	/*
	  For sp5kDispAttrSet():

	  para1 = Y-filter type, 0~3
	  para2 = C-filter type, 0~3
	  para3 = 0: disable, 1: enable filter mirror.
	*/
	SP5K_DISP_TV_ENCODER_FILTER,
} sp5kDispParaId_e;

typedef enum _sp5kDispDevCmd_e {
	SP5K_DISP_DEV_CMD_POWERON  = DISP_DEV_CMD_POWERON,
	SP5K_DISP_DEV_CMD_SUSPEND  = DISP_DEV_CMD_SUSPEND,
	SP5K_DISP_DEV_CMD_RESUME   = DISP_DEV_CMD_RESUME,
	SP5K_DISP_DEV_CMD_POWEROFF = DISP_DEV_CMD_POWEROFF,
	SP5K_DISP_DEV_CMD_MAX      = DISP_DEV_CMD_MAX,
} sp5kDispDevCmd_e;

/* SP5K_DISP_OSD_COLOR_KEY. */
typedef enum _sp5kDispColorKeyId_e {
	SP5K_DISP_COLOR_KEY_RGB565 = DISP_COLOR_KEY_RGB565,
} sp5kDispColorKeyId_e;

/* sp5kDispPowerOn().resId for composite TV. */
typedef enum _sp5kDispCompositeTvId_e {
	SP5K_DISP_TV_NTSC_M,
	SP5K_DISP_TV_NTSC_J,
	SP5K_DISP_TV_NTSC_443,
	SP5K_DISP_TV_PAL,
	SP5K_DISP_TV_PAL_M,
} sp5kDispCompositeTvId_e;

#define SP5K_DISP_SRC_TOT   DISP_SRC_TOT
#if SPCA6350
#define SP5K_DISP_CHNL_TOT  DISP_CHNL_TOT
#endif
#define SP5K_DISP_HDMI_CHANNEL_TOT  DISP_HDMI_CHANNEL_TOT

#define SP5K_DISP_EDID_VIDEO_FMT_TOT  DISP_EDID_VIDEO_FMT_TOT

/**************************************************************************
 *                          D A T A   T Y P E S
 **************************************************************************/
typedef dispCapabDesc_t      sp5kDispCapabDesc_t;
typedef dispOperatingFunc_t  sp5kDispOperatingFunc_t;

typedef dispHdmiVideoFmt_t  sp5kDispHdmiVideoFmt_t;
typedef dispHdmiEedid_t     sp5kDispHdmiEedid_t;

/**************************************************************************
 *              F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
/* direct macro mapping. */
#define sp5kDispChnlSrcSet  dispChnlSrcSet  /*UINT32 (UINT32 chnlId, UINT32 srcId)*/
#define sp5kDispChnlSrcGet  dispChnlSrcGet  /*UINT32 (UINT32 chnlId)*/
#define sp5kDispSrcChnlGet  dispSrcChnlGet  /*UINT32 (UINT32 srcId)*/

#define sp5kDispDevAlloc       dispDevAlloc       /*UINT32 (UINT32 chnlId, UINT32 devTot)*/
#define sp5kDispDevReg         dispDevReg         /*UINT32 (UINT32 chnlId, UINT32 devId, sp5kDispOperatingFunc_t *popFunc)*/
#define sp5kDispPowerOn        dispPowerOn        /*UINT32 (UINT32 chnlId, UINT32 devId, UINT32 resId, UINT32 msgEn)*/
#define sp5kDispPowerOff       dispPowerOff       /*UINT32 (UINT32 chnlId, UINT32 msgEn)*/
#define sp5kDispCapabilityGet  dispCapabilityGet  /*UINT32 (UINT32 chnlId, sp5kDispCapabDesc_t *pcapabDesc)*/
#define sp5kDispCfgSet         dispCfgSet         /*UINT32 (UINT32 chnlId, UINT32 devId, UINT32 cfgId, ...)*/
#define sp5kDispActChnlGet     dispActChnlGet     /*UINT32 (UINT32 queId)*/

#define sp5kDispHdmiHotPlugDetectGet   dispHdmiHotPlugDetectGet   /*UINT32 (UINT32 chnlId)*/
#define sp5kDispHdmiEedidGet           dispHdmiEedidGet           /*UINT32 (UINT32 chnlId, sp5kDispHdmiEedid_t **ppeedid)*/
#define sp5kDispHdmiIsVideoFmtSupport  dispHdmiIsVideoFmtSupport  /*UINT32 (UINT32 chnlId, UINT32 vidFmt)*/

/* sp5k_disp_attr.c */
UINT32 sp5kDispAttrSet(UINT32 chnlId, UINT32 paraId, ...);
UINT32 sp5kDispAttrGet(UINT32 chnlId, UINT32 paraId, ...);

#endif  /* _SP5K_DISP_API_H_ */

