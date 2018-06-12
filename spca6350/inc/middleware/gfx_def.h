/**************************************************************************
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
 *  Author: Matt Wang
 *
 **************************************************************************/
#ifndef _GFX_DEF_H_
#define _GFX_DEF_H_

/**************************************************************************
 *                          C O N S T A N T S
 **************************************************************************/
typedef enum _gfxAlign_e {
	GFX_ALIGN_TOP_LEFT      = 0x11,  /**< @image html gfx_GFX_ALIGN_TOP_LEFT.gif */
	GFX_ALIGN_TOP_CENTER    = 0x12,  /**< @image html gfx_GFX_ALIGN_TOP_CENTER.gif */
	GFX_ALIGN_TOP_RIGHT     = 0x14,  /**< @image html gfx_GFX_ALIGN_TOP_RIGHT.gif */
	GFX_ALIGN_LEFT          = 0x21,  /**< @image html gfx_GFX_ALIGN_LEFT.gif */
	GFX_ALIGN_CENTER        = 0x22,  /**< @image html gfx_GFX_ALIGN_CENTER.gif */
	GFX_ALIGN_RIGHT         = 0x24,  /**< @image html gfx_GFX_ALIGN_RIGHT.gif */
	GFX_ALIGN_BOTTOM_LEFT   = 0x41,  /**< @image html gfx_GFX_ALIGN_BOTTOM_LEFT.gif */
	GFX_ALIGN_BOTTOM_CENTER = 0x42,  /**< @image html gfx_GFX_ALIGN_BOTTOM_CENTER.gif */
	GFX_ALIGN_BOTTOM_RIGHT  = 0x44,  /**< @image html gfx_GFX_ALIGN_BOTTOM_RIGHT.gif */
} gfxAlign_e;

typedef enum _gfxInitCfg_e {
	GFX_INIT_DISP_SRC,
	GFX_INIT_LAYER_TOT,
	GFX_INIT_FRAME_SIZE,
	GFX_INIT_VIRTUAL_FRAME_SIZE,
	GFX_INIT_LAYER_WINDOW_OFFSET,
	GFX_INIT_LAYER_WINDOW_SIZE,
	GFX_INIT_LAYER_VIRTUAL_WINDOW_OFFSET,
	GFX_INIT_LAYER_VIRTUAL_WINDOW_SIZE,
	GFX_INIT_LAYER_FRAME_SIZE,
	GFX_INIT_LAYER_FRAME_ROI_OFFSET,
	GFX_INIT_LAYER_FRAME_ROI_SIZE,
	GFX_INIT_LAYER_VIRTUAL_FRAME_SIZE,
	GFX_INIT_LAYER_VIRTUAL_FRAME_ROI_OFFSET,
	GFX_INIT_LAYER_VIRTUAL_FRAME_ROI_SIZE,
	GFX_INIT_LAYER_TYPE,
	GFX_INIT_LAYER_DEFAULT_TYPE,
	GFX_INIT_LAYER_FORMAT,
	GFX_INIT_LAYER_CMD_TOT,
	GFX_INIT_LAYER_POOL_SIZE,
	GFX_INIT_LAYER_USED,
	GFX_INIT_LAYER_SHOWN,
	GFX_INIT_LAYER_WITH_ALPHA_PLANE,
	GFX_INIT_LAYER_WIDTH_3X,
	GFX_INIT_CFG_TOT,
} gfxInitCfg_e;

typedef enum _gfxDispSrc_e {
	GFX_DISP_SRC_PRIMARY   = (1 << 0),
	GFX_DISP_SRC_SECONDARY = (1 << 1),
} gfxDispSrc_e;

#define GFX_ROT_EXT_FLAG (0x80000000)

#endif  /* _GFX_DEF_H_ */

