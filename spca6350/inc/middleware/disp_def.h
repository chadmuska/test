/**************************************************************************
 *
 *       Copyright (c) 2006-2015 by iCatch Technology, Inc.
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
 * @file disp_def.h
 *
 * @author Matt Wang
 */
#ifndef _DISP_DEF_H_
#define _DISP_DEF_H_

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include <stdarg.h>  /* va_list. */

/**************************************************************************
 *                          C O N S T A N T S
 **************************************************************************/
#define DISP_DRIVER_NEW_ARCH  1  /* if 1, use multiple channel display device architecture. */

/** @brief Disp channel ID. */
typedef enum _dispChnlId_e {
	DISP_CHNL_NULL = -1,
	DISP_CHNL_0    = 0,
	DISP_CHNL_1    = 1,
} dispChnlId_e;  /* plz keep sync to halDispChnlId_e. */

/** @brief Disp source ID. */
typedef enum _dispSrcId_e {
	DISP_SRC_NULL = -1,
	DISP_SRC_0    = 0,
	DISP_SRC_1    = 1,
} dispSrcId_e;

/** @brief Disp device ID. */
typedef enum _dispDevId_e {
	DISP_DEV_NULL            = -1,

	DISP_CHNL_0_PANEL        = 0,
	DISP_CHNL_0_DEV_TOT      = 1,

	DISP_CHNL_1_PANEL        = 0,
	DISP_CHNL_1_COMPOSITE_TV = 1,
	DISP_CHNL_1_HDMI_TV      = 2,
	DISP_CHNL_1_DEV_TOT      = 3,
} dispDevId_e;

typedef enum _dispDevCmd_e {
	DISP_DEV_CMD_POWERON,
	DISP_DEV_CMD_SUSPEND,
	DISP_DEV_CMD_RESUME,
	DISP_DEV_CMD_POWEROFF,
	DISP_DEV_CMD_MAX,
} dispDevCmd_e;

typedef enum _dispColorKeyId_e {
	DISP_COLOR_KEY_RGB565 = 0,
} dispColorKeyId_e;

typedef enum _dispClkSrc_e {
	DISP_CLK_SRC_DISPPLL = 0,
	DISP_CLK_SRC_XTAL27M = 1,

	/* backward compatible. */
	DISP_CLK_SRC_DRAMPLL = DISP_CLK_SRC_DISPPLL,
} dispClkSrc_e;  /* plz keep sync to halPllDispClkSrc_e. */

typedef enum _dispTvType_e {
	DISP_TV_TYPE_DIGITAL = (0 << 4),
	DISP_TV_TYPE_ANALOG  = (1 << 4),
} dispTvType_e; /* plz keep sync to halPllDispTvType_e. */

typedef enum _dispEvent_e {
	DISP_EVENT_PANEL_VSYNC_RISE  =  0,
	DISP_EVENT_PANEL_VSYNC_FALL  =  1,
	DISP_EVENT_PANEL_VVALID_RISE =  2,
	DISP_EVENT_PANEL_VVALID_FALL =  3,
	DISP_EVENT_LCM_VSYNC_RISE    =  4,
	DISP_EVENT_LCM_VSYNC_FALL    =  5,
	DISP_EVENT_TV_VSYNC_RISE     =  6,
	DISP_EVENT_TV_VSYNC_FALL     =  7,
	DISP_EVENT_TV_VVALID_RISE    =  8,
	DISP_EVENT_TV_VVALID_FALL    =  9,
	#if SPCA6350
	DISP_EVENT_HDMI_HPD_RISE     = 18,
	DISP_EVENT_HDMI_HPD_FALL     = 19,
	#else
	DISP_EVENT_HDMI_HPD_RISE     = 10,
	DISP_EVENT_HDMI_HPD_FALL     = 11,
	#endif
} dispEvent_e;  /* plz keep sync to halDispInt_e. */

typedef enum _dispYuv422ToYuv444Mode_e {
	DISP_UV_EXTEND_DUP    = (0 << 0),
	DISP_UV_EXTEND_INTRPL = (1 << 0),
} dispUvExtendMode_e;  /* plz keep sync to halDispUvExtendMode_e. */

typedef enum _dispParaId_e {
	DISP_PARA_RESET,
	DISP_PARA_GLOB_ENABLE,
	DISP_PARA_DEVICE_TYPE,
	DISP_PARA_DEVICE_TOTAL_REGION,  /* please use DISP_PARA_DEVICE_TOTAL_RESOLUTION and DISP_PARA_DEVICE_SYNC_TIME to replace. */
	DISP_PARA_DEVICE_TOTAL_RESOLUTION,
	DISP_PARA_DEVICE_SYNC_TIME,
	DISP_PARA_DEVICE_SYNC_TIME_POINT,
	DISP_PARA_DEVICE_ACTIVE_REGION,
	DISP_PARA_CHNL_SRC,
	DISP_PARA_CLK_ENABLE,
	DISP_PARA_CLK_DIVISOR,
	DISP_PARA_CLK_PHASE,
	DISP_PARA_BANDWIDTH_BUFFER_MODE,
	DISP_PARA_BG_COLOR,
	DISP_PARA_INTERLACE_MODE,
	DISP_PARA_EDGE_ENHANCE,
	DISP_PARA_IMG_LAYER_ENABLE,
	DISP_PARA_IMG_LAYER_FLIP,
	DISP_PARA_IMG_NONSCALE_INTERLACE_MODE,
	DISP_PARA_IMG_COLOR_KEY_ENABLE,
	DISP_PARA_IMG_COLOR_KEY,
	DISP_PARA_IMG_FORMAT,
	DISP_PARA_IMG_MONOCHROME_ENABLE,
	DISP_PARA_IMG_UV_RECONSTRUCT_MODE,
	DISP_PARA_IMG_SCALE_BUFFER_SIZE,
	DISP_PARA_IMG_SRC,
	DISP_PARA_IMG_PV_REQUEST_PERIOD,
	DISP_PARA_IMG_PV_FRAME_BUFFER,
	DISP_PARA_IMG_PV_2ND_PATH_FRAME_BUFFER,
	DISP_PARA_IMG_PV_RENDER_REGION,
	DISP_PARA_IMG_PV_CROP_REGION,
	DISP_PARA_IMG_PV_AUTO_SCALE,
	DISP_PARA_IMG_PB_REQUEST_PERIOD,
	DISP_PARA_IMG_PB_FRAME_BUFFER,
	DISP_PARA_IMG_PB_RENDER_REGION,
	DISP_PARA_IMG_PB_CROP_REGION,
	DISP_PARA_IMG_PB_AUTO_SCALE,
	DISP_PARA_OSD_LAYER_ENABLE,
	DISP_PARA_OSD_LAYER_FLIP,
	DISP_PARA_OSD_COLOR_KEY_ENABLE,
	DISP_PARA_OSD_COLOR_KEY,
	DISP_PARA_OSD_BG_COLOR_KEY_ENABLE,
	DISP_PARA_OSD_BG_COLOR_KEY,
	DISP_PARA_OSD_FORMAT,
	DISP_PARA_OSD_GEN_ALPHA,
	DISP_PARA_OSD_ALPHA_PLANE_MODE,
	DISP_PARA_OSD_REQUEST_PERIOD,
	DISP_PARA_OSD_ALPHA_PLANE_REQUEST_PERIOD,
	DISP_PARA_OSD_FRAME_BUFFER,
	DISP_PARA_OSD_ALPHA_PLANE_BUFFER,
	DISP_PARA_OSD_RENDER_REGION,
	DISP_PARA_OSD_CROP_REGION,
	DISP_PARA_OSD_ALPHA_PLANE_CROP_REGION,
	DISP_PARA_OSD_AUTO_SCALE,
	DISP_PARA_BRIGHTNESS,
	DISP_PARA_CONTRAST,
	DISP_PARA_HUE,
	DISP_PARA_SATURATION,
	DISP_PARA_WHITE_BALANCE,
	DISP_PARA_COLOR_MATRIX_ENABLE,
	DISP_PARA_COLOR_MATRIX,
	DISP_PARA_COLOR_CLAMP,
	DISP_PARA_GAMMA_TABLE_ENABLE,
	DISP_PARA_GAMMA_TABLE,
	DISP_PARA_COLOR_GAMMA_ORDER,
	DISP_PARA_ROLLING_SHUTTER,
	DISP_PARA_VIDEO_DAC_POWER,
	DISP_PARA_TV_ENCODER_FILTER,
	DISP_PARA_TV_HUE,
	DISP_PARA_TV_SATURATION,
	DISP_PARA_TV_SYNC_LEVEL,
	DISP_PARA_TV_BLANK_LEVEL,
	DISP_PARA_TV_BLACK_LEVEL,
	DISP_PARA_VIDEO_DAC_FILTER,
	DISP_PARA_PIXEL_COMBINE_MODE,
	#if SPCA6350
	DISP_PARA_PIXEL_LOW_PASS_FILTER,
	#endif
	DISP_PARA_DOT_AVERAGE_MODE,
	DISP_PARA_RGB888_ROUND_MODE,
	DISP_PARA_UPS_HSD_MODE,
	DISP_PARA_UPS_LAYOUT_MODE,
	DISP_PARA_UPS_COLOR_FILTER,
	DISP_PARA_UPS051_RGB888_ENABLE,
	DISP_PARA_UPS052_RGB_ONLY_ENABLE,
	DISP_PARA_CCIR_OUTPUT_SEQUENCE,
	DISP_PARA_BT1120_OUTPUT_SEQUENCE,
	DISP_PARA_BT1120_VERTICAL_FRONT_PORCH,
	DISP_PARA_SIGNAL_POLARITY,
	DISP_PARA_LCM_BUS_CFG,
	DISP_PARA_LCM_DATA_FORMAT,
	DISP_PARA_LCM_DATA_PACKET,
	DISP_PARA_LCM_SIGNAL_UPDATE_MODE,
	DISP_PARA_LCM_CMD_DELAY,
	DISP_PARA_LCM_AUTO_CMD,
	#if SPCA6350
	DISP_PARA_RGB_MODE_ENABLE,
	DISP_PARA_DSI_PHY_ENABLE,
	DISP_PARA_DSI_OPERATION_MODE,
	DISP_PARA_DSI_TRANSMITTER_SWITCH,
	DISP_PARA_DSI_MULTI_LANE_NUM,
	DISP_PARA_DSI_EOT_PACKET_ENABLE,
	DISP_PARA_DSI_BTA_TIMEOUT,
	DISP_PARA_DSI_TOTAL_RESOLUTION,
	DISP_PARA_DSI_DATA_TYPE,
	DISP_PARA_DSI_VIDEO_MODE,
	DISP_PARA_DSI_BLANK_LEVEL,
	DISP_PARA_DSI_SIGNAL_UPDATE_MODE,
	DISP_PARA_DSI_CLK_MODE,
	DISP_PARA_DSI_HS_LANE_TIMING,
	DISP_PARA_DSI_DPHY_TX_PLL,
	DISP_PARA_DSI_DHPY_HS_ATTR,
	DISP_PARA_DSI_DHPY_LP_ATTR,
	#endif
	DISP_PARA_HDMI_PHY_POWER,
	DISP_PARA_HDMI_SIGNAL_POLARITY,
	DISP_PARA_HDMI_HPD_CFG,
	DISP_PARA_HDMI_PIXEL_REPETITION,
	DISP_PARA_HDMI_DATA_ISLAND,
	DISP_PARA_HDMI_PACKET_ENABLE,
	DISP_PARA_HDMI_PRE_EMPHASIS,
	DISP_PARA_HDMI_COLOR_DEPTH,
	DISP_PARA_HDMI_CHARGE_PUMP,
	DISP_PARA_HDMI_DVI_ENABLE,
	#if SPCA6350
	DISP_PARA_3D_ENABLE,
	DISP_PARA_3D_ACT_SPACE_COLOR,
	DISP_PARA_3D_ACT_SPACE_REGION,
	DISP_PARA_3D_IMG_PV_RENDER_REGION,
	DISP_PARA_3D_IMG_PV_CROP_REGION,
	DISP_PARA_3D_IMG_PB_RENDER_REGION,
	DISP_PARA_3D_IMG_PB_CROP_REGION,
	DISP_PARA_3D_OSD_RENDER_REGION,
	DISP_PARA_3D_OSD_CROP_REGION,
	DISP_PARA_3D_OSD_ALPHA_PLANE_CROP_REGION,
	DISP_PARA_3D_ROLLING_SHUTTER,
	#endif

	#if SPCA6350
	DISP_PARA_DUAL_DISPLAY,
	#endif
	
	DISP_PARA_CLK_SRC,
	DISP_PARA_CLK_FREQ,
	DISP_PARA_SCALE_CLK_FREQ,
	DISP_PARA_HDMI_PIXEL_CLK_FREQ,
	DISP_PARA_GPIO_A_FUNC_CFG,
	DISP_PARA_GPIO_A_DIR_CFG,
	DISP_PARA_GPIO_A_PULL_CFG,
	DISP_PARA_GPIO_A_DATA,
	DISP_PARA_GPIO_B_FUNC_CFG,
	DISP_PARA_GPIO_B_DIR_CFG,
	DISP_PARA_GPIO_B_PULL_CFG,
	DISP_PARA_GPIO_B_DATA,
	DISP_PARA_PIN_MUX,
	DISP_PARA_PIN_SWAP,
	DISP_PARA_PIN_SELECT,
	DISP_PARA_HDMI_AUD_SAMPLE_RATE,

	DISP_PARA_MAX_ID,

	/* backward compatible. */
	/* please use new names, don't use old names anymore. */
	DISP_PARA_3D_ACT_SPACE_DATA = DISP_PARA_3D_ACT_SPACE_COLOR,
	DISP_PARA_GPIO_FUNC_CFG     = DISP_PARA_GPIO_A_FUNC_CFG,
	DISP_PARA_GPIO_DIR_CFG      = DISP_PARA_GPIO_A_DIR_CFG,
	DISP_PARA_GPIO_PULL_CFG     = DISP_PARA_GPIO_A_PULL_CFG,
	DISP_PARA_GPIO_DATA         = DISP_PARA_GPIO_A_DATA,

	/* invalid id. */
	DISP_PARA_INVALID_ID = 1000000,
	DISP_PARA_IMG_COLOR_KEY_BG_SRC,
	DISP_PARA_IMG_UV_EXTEND_MODE,
	DISP_PARA_IMG_SCALE_DEVICE,
	DISP_PARA_IMG_PV_SCALE_FACTOR,
	DISP_PARA_IMG_PV_SCALE_INIT_FACTOR,
	DISP_PARA_IMG_PV_SCALE_MODE,
	DISP_PARA_IMG_PV_SCALE_ORDER,
	DISP_PARA_IMG_PV_SCALE_INTERLACE_DROP_MODE_ENABLE,
	DISP_PARA_IMG_PB_SCALE_FACTOR,
	DISP_PARA_IMG_PB_SCALE_INIT_FACTOR,
	DISP_PARA_IMG_PB_SCALE_MODE,
	DISP_PARA_IMG_PB_SCALE_ORDER,
	DISP_PARA_IMG_PB_SCALE_INTERLACE_DROP_MODE_ENABLE,
	DISP_PARA_OSD_ARGB_ALPHA,
	DISP_PARA_OSD_BOUNDARY_AVERAGE_ENABLE,
	DISP_PARA_OSD_SCALE_FACTOR,
	DISP_PARA_OSD_SCALE_INITFACTOR,
	DISP_PARA_OSD_SCALE_MODE,
	DISP_PARA_UPS051_LIGHTVIEW_ENABLE,
	DISP_PARA_CCIR_CLAMP_MODE,
	DISP_PARA_CCIR_SIGNAL_POLARITY,
} dispParaId_e;  /* plz keep sync to halDispParaId_e. */

typedef enum _dispInfId_e {
	/*
	  UINT32 *pfrmRate = Actual frame rate.
	*/
	DISP_INF_FRAME_RATE,
} dispInfId_e;

typedef enum _dispChnlSrc_e {
	DISP_C0S0_C1S1 = (0 << 0),
	DISP_C0S1_C1S0 = (1 << 0),
} dispChnlSrc_e;  /* plz keep sync to halDispChnlSrc_e. */

typedef enum _dispBandwidthBufferMode_e {
	DISP_BANDWIDTH_BALANCE     = (0 << 0),  /**< Panel 256(spca5330)/1920(spca6330) pixels, tv 384(spca5330)/2880(spca6330) pixels. */
	DISP_BANDWIDTH_PANEL_MAJOR = (1 << 0),  /**< Panel 640(spca5330)/4800(spca6330) pixels, tv 0 pixels. */
	DISP_BANDWIDTH_TV_MAJOR    = (2 << 0),  /**< Panel 0 pixels, tv 640(spca5330)/4800(spca6330) pixels. */
} dispBandwidthBufferMode_e;  /* plz keep sync to halDispBandwidthBufferMode_e. */

typedef enum _dispNonscaleIntrlcMode_e {
	DISP_NONSCALE_INTRLC_DISABLE = (0 << 0),
	DISP_NONSCALE_INTRLC_NOSWAP  = ((0 << 1) | (1 << 0)),
	DISP_NONSCALE_INTRLC_SWAP    = ((1 << 1) | (1 << 0)),
} dispNonscaleIntrlcMode_e;  /* plz keep sync to halDispNonscaleIntrlcMode_e. */

typedef enum _dispDeviceType_e {
	DISP_DEVICE_TYPE_NTSC_M                 = 0x000,  /**< For TV(CHNL_1) interface only. */
	DISP_DEVICE_TYPE_NTSC_J                 = 0x040,  /**< For TV(CHNL_1) interface only. */
	DISP_DEVICE_TYPE_NTSC_443               = 0x080,  /**< For TV(CHNL_1) interface only. */
	DISP_DEVICE_TYPE_PAL                    = 0x001,  /**< For TV(CHNL_1) interface only. */
	DISP_DEVICE_TYPE_PAL_M                  = 0x020,  /**< For TV(CHNL_1) interface only. */
	DISP_DEVICE_TYPE_CCIR656_NTSC           = 0x002,  /**< For both panel(CHNL_0) and TV(CHNL_1) interfaces. */
	DISP_DEVICE_TYPE_CCIR656_PAL            = 0x003,  /**< For both panel(CHNL_0) and TV(CHNL_1) interfaces. */
	DISP_DEVICE_TYPE_CCIR601_NTSC_8         = 0x004,  /**< For both panel(CHNL_0) and TV(CHNL_1) interfaces. */
	DISP_DEVICE_TYPE_CCIR601_NTSC_16        = 0x104,  /**< For both panel(CHNL_0) and TV(CHNL_1) interfaces. */
	DISP_DEVICE_TYPE_CCIR601_PAL_8          = 0x005,  /**< For both panel(CHNL_0) and TV(CHNL_1) interfaces. */
	DISP_DEVICE_TYPE_CCIR601_PAL_16         = 0x105,  /**< For both panel(CHNL_0) and TV(CHNL_1) interfaces. */
	DISP_DEVICE_TYPE_UPS051                 = 0x008,  /**< For both panel(CHNL_0) and TV(CHNL_1) interfaces. */
	DISP_DEVICE_TYPE_UPS052                 = 0x009,  /**< For both panel(CHNL_0) and TV(CHNL_1) interfaces. */
	DISP_DEVICE_TYPE_LCM                    = 0x00A,  /**< For both panel(CHNL_0) and TV(CHNL_1) interfaces. */
	DISP_DEVICE_TYPE_USB                    = 0x00B,  /**< For TV(CHNL_1) interface only. */
	#if SPCA6350
	DISP_DEVICE_TYPE_DSI                    = 0x00C,  /**< For panel(CHNL_0) interface only. */
	#endif
	DISP_DEVICE_TYPE_INTERLACE_CCIR656      = 0x010,  /**< For both panel(CHNL_0) and TV(CHNL_1) interfaces. */
	DISP_DEVICE_TYPE_INTERLACE_CCIR601_8    = 0x013,  /**< For both panel(CHNL_0) and TV(CHNL_1) interfaces. */
	DISP_DEVICE_TYPE_INTERLACE_CCIR601_16   = 0x113,  /**< For both panel(CHNL_0) and TV(CHNL_1) interfaces. */
	DISP_DEVICE_TYPE_STD_INTERLACE_CCIR656  = 0x014,  /**< For both panel(CHNL_0) and TV(CHNL_1) interfaces. */
	#if SPCA6350
	DISP_DEVICE_TYPE_STD_INTERLACE_BT1120   = 0x015,  /**< For both panel(CHNL_0) and TV(CHNL_1) interfaces. */
	#endif
	DISP_DEVICE_TYPE_STD_INTERLACE_CCIR601  = 0x017,  /**< For both panel(CHNL_0) and TV(CHNL_1) interfaces. */
	DISP_DEVICE_TYPE_PROGRESSIVE_CCIR656    = 0x018,  /**< For both panel(CHNL_0) and TV(CHNL_1) interfaces. */
	#if SPCA6350
	DISP_DEVICE_TYPE_PROGRESSIVE_BT1120     = 0x019,  /**< For both panel(CHNL_0) and TV(CHNL_1) interfaces. */
	#endif
	DISP_DEVICE_TYPE_PROGRESSIVE_CCIR601_8  = 0x01B,  /**< For both panel(CHNL_0) and TV(CHNL_1) interfaces. */
	DISP_DEVICE_TYPE_PROGRESSIVE_CCIR601_16 = 0x11B,  /**< For both panel(CHNL_0) and TV(CHNL_1) interfaces. */
	/* backward compatible. */
	DISP_DEVICE_TYPE_INTERLACE_CCIR601           = DISP_DEVICE_TYPE_INTERLACE_CCIR601_8,
	DISP_DEVICE_TYPE_PROGRESSIVE_CCIR601         = DISP_DEVICE_TYPE_PROGRESSIVE_CCIR601_8,
	DISP_PANEL_DEVICE_TYPE_CCIR656_NTSC          = DISP_DEVICE_TYPE_CCIR656_NTSC,
	DISP_PANEL_DEVICE_TYPE_CCIR656_PAL           = DISP_DEVICE_TYPE_CCIR656_PAL,
	DISP_PANEL_DEVICE_TYPE_CCIR601_NTSC_8        = DISP_DEVICE_TYPE_CCIR601_NTSC_8,
	DISP_PANEL_DEVICE_TYPE_CCIR601_NTSC_16       = DISP_DEVICE_TYPE_CCIR601_NTSC_16,
	DISP_PANEL_DEVICE_TYPE_CCIR601_PAL_8         = DISP_DEVICE_TYPE_CCIR601_PAL_8,
	DISP_PANEL_DEVICE_TYPE_CCIR601_PAL_16        = DISP_DEVICE_TYPE_CCIR601_PAL_16,
	DISP_PANEL_DEVICE_TYPE_UPS051                = DISP_DEVICE_TYPE_UPS051,
	DISP_PANEL_DEVICE_TYPE_UPS052                = DISP_DEVICE_TYPE_UPS052,
	DISP_PANEL_DEVICE_TYPE_LCM                   = DISP_DEVICE_TYPE_LCM,
	DISP_PANEL_DEVICE_TYPE_INTERLACE_CCIR656     = DISP_DEVICE_TYPE_INTERLACE_CCIR656,
	DISP_PANEL_DEVICE_TYPE_INTERLACE_CCIR601     = DISP_DEVICE_TYPE_INTERLACE_CCIR601,
	DISP_PANEL_DEVICE_TYPE_STD_INTERLACE_CCIR656 = DISP_DEVICE_TYPE_STD_INTERLACE_CCIR656,
	DISP_PANEL_DEVICE_TYPE_STD_INTERLACE_CCIR601 = DISP_DEVICE_TYPE_STD_INTERLACE_CCIR601,
	DISP_PANEL_DEVICE_TYPE_PROGRESSIVE_CCIR656   = DISP_DEVICE_TYPE_PROGRESSIVE_CCIR656,
	DISP_PANEL_DEVICE_TYPE_PROGRESSIVE_CCIR601   = DISP_DEVICE_TYPE_PROGRESSIVE_CCIR601,
	DISP_TV_DEVICE_TYPE_NTSC_M                   = DISP_DEVICE_TYPE_NTSC_M,
	DISP_TV_DEVICE_TYPE_NTSC_J                   = DISP_DEVICE_TYPE_NTSC_J,
	DISP_TV_DEVICE_TYPE_NTSC_443                 = DISP_DEVICE_TYPE_NTSC_443,
	DISP_TV_DEVICE_TYPE_PAL                      = DISP_DEVICE_TYPE_PAL,
	DISP_TV_DEVICE_TYPE_PAL_M                    = DISP_DEVICE_TYPE_PAL_M,
	DISP_TV_DEVICE_TYPE_USB                      = DISP_DEVICE_TYPE_USB,
} dispDeviceType_e;  /* plz keep sync to halDispDeviceType_e. */

typedef enum _dispImgFmt_e {
	#if SPCA6350
	DISP_IMG_FMT_YUV422 = ((0 << 1) | (0 << 0)),
	DISP_IMG_FMT_YUV420 = ((1 << 1) | (0 << 0)),
	#else
	DISP_IMG_FMT_YUYV422 = ((0 << 1) | (1 << 0)),
	DISP_IMG_FMT_YUYV420 = ((1 << 1) | (1 << 0)),
	#endif
} dispImgFmt_e;  /* plz keep sync to halDispImgFmt_e. */

typedef enum _dispOsdFmt_e {
	DISP_OSD_FMT_RGB565  = 0,
	#if !SPCA6350
	DISP_OSD_FMT_YUYV422 = 1,
	#endif
} dispOsdFmt_e;  /* plz keep sync to halDispOsdFmt_e. */

typedef enum _dispImgSrc_e {
	DISP_IMG_SRC_PV = (1 << 1),
	DISP_IMG_SRC_PB = (0 << 1),
} dispImgSrc_e;  /* plz keep sync to halDispImgSrc_e. */

typedef enum _dispImgUvReconstuctMode_e {
	#if SPCA6350
	DISP_IMG_UV_HSCL_DUP    = (0 << 0),
	DISP_IMG_UV_HSCL_INTRPL = (1 << 0),

	DISP_IMG_UV_BLD_DUP     = (0 << 4),
	DISP_IMG_UV_BLD_JOINT   = (1 << 4),
	DISP_IMG_UV_BLD_SEPR    = (2 << 4),
	DISP_IMG_UV_BLD_REFINE  = (3 << 4),
	#else
	DISP_IMG_UV_DUP         = (0 << 0),
	DISP_IMG_UV_INTRPL      = (1 << 0),
	DISP_IMG_UV_REFINE      = (2 << 0),

	DISP_IMG_UV_DROP_JOINT  = (0 << 4),
	DISP_IMG_UV_DROP_SEPR   = (1 << 4),
	DISP_IMG_UV_DROP_INTRPL = (2 << 4),
	DISP_IMG_UV_DROP_SATUR  = (3 << 4),

	DISP_IMG_UV_HSCL_DUP    = (0 << 8),
	DISP_IMG_UV_HSCL_INTRPL = (1 << 8),

	DISP_IMG_UV_BLD_DUP     = (0 << 12),
	DISP_IMG_UV_BLD_JOINT   = (1 << 12),
	DISP_IMG_UV_BLD_SEPR    = (2 << 12),
	DISP_IMG_UV_BLD_REFINE  = (3 << 12),
	#endif
} dispImgUvReconstuctMode_e;  /* plz keep sync to halDispImgUvReconstuctMode_e. */

typedef enum _dispInterlaceMode_e {
	DISP_PROGRESSIVE      = ((0 << 2) | (0 << 0)),
	DISP_INTERLACE_NOSWAP = ((1 << 2) | (0 << 1) | (1 << 0)),
	DISP_INTERLACE_SWAP   = ((1 << 2) | (1 << 1) | (1 << 0)),
	DISP_INTERLACE_NOSOF  = ((0 << 2) | (0 << 1) | (1 << 0)),
} dispInterlaceMode_e;  /* plz keep sync to halDispInterlaceMode_e. */

typedef enum _dispSrcFieldMode_e {  /* spca6330. */
	DISP_SRC_FIELD_NOSWAP = (0 << 4),
	DISP_SRC_FIELD_SWAP   = (1 << 4),
} dispSrcFieldMode_e;  /* plz keep sync to halDispSrcFieldMode_e. */

typedef enum _dispColorClampType_e {
	DISP_COLOR_CLAMP_RGB_0_255    = (0 << 4),
	DISP_COLOR_CLAMP_YCBCR_16_235 = (1 << 4),
	DISP_COLOR_CLAMP_YUV_0_255    = (2 << 4),
	DISP_COLOR_CLAMP_RGB_16_235   = (5 << 4),
} dispColorClampType_e;  /* plz keep sync to halDispColorClampType_e. */

typedef enum _dispColorGammaOrder_e {  /* spca6330. */
	DISP_GAMMA_AFTER_COLOR  = (0 << 2),
	DISP_GAMMA_BEFORE_COLOR = (1 << 2),
} dispColorGammaOrder_e;  /* plz keep sync to halDispColorGammaOrder_e. */

typedef enum _dispPixelCombineMode_e {
	DISP_PIXEL_COMBINE_DISABLE = ((0 << 4) | (0 << 2) | (0 << 0)),
	DISP_PIXEL_COMBINE_RGB     = ((2 << 4) | (1 << 2) | (0 << 0)),
	DISP_PIXEL_COMBINE_GBR     = ((1 << 4) | (0 << 2) | (2 << 0)),
	DISP_PIXEL_COMBINE_BRG     = ((0 << 4) | (2 << 2) | (1 << 0)),
	DISP_PIXEL_COMBINE_BGR     = ((0 << 4) | (1 << 2) | (2 << 0)),
	DISP_PIXEL_COMBINE_RBG     = ((1 << 4) | (2 << 2) | (0 << 0)),
	DISP_PIXEL_COMBINE_GRB     = ((2 << 4) | (0 << 2) | (1 << 0)),
} dispPixelCombineMode_e;  /* plz keep sync to halDispPixelCombineMode_e. */

typedef enum _dispPixelLpfMode_e {
	DISP_PIXEL_LPF_DISABLE     = (0 << 0),
	DISP_PIXEL_LPF_CHANNEL_R   = (1 << 2),
	DISP_PIXEL_LPF_CHANNEL_G   = (1 << 1),
	DISP_PIXEL_LPF_CHANNEL_B   = (1 << 0),

	DISP_PIXEL_LPF_CHANNEL_RG  = ((1 << 2) | (1 << 1)),
	DISP_PIXEL_LPF_CHANNEL_RB  = ((1 << 2) | (1 << 0)),
	DISP_PIXEL_LPF_CHANNEL_GB  = ((1 << 1) | (1 << 0)),

	DISP_PIXEL_LPF_CHANNEL_RGB = ((1 << 2) | (1 << 1) | (1 << 0)),
} dispPixelLpfMode_e;/* plz keep sync to halDispPixelLpfMode_e. */

typedef enum _dispDotAverageMode_e {
	DISP_DOT_EVEN_R100 = (0 << 0),
	DISP_DOT_EVEN_R75  = (1 << 0),
	DISP_DOT_EVEN_R50  = (2 << 0),
	DISP_DOT_EVEN_R25  = (3 << 0),
	DISP_DOT_EVEN_R0   = (4 << 0),

	DISP_DOT_ODD_R100  = (0 << 4),
	DISP_DOT_ODD_R75   = (1 << 4),
	DISP_DOT_ODD_R50   = (2 << 4),
	DISP_DOT_ODD_R25   = (3 << 4),
	DISP_DOT_ODD_R0    = (4 << 4),

	DISP_DOT_EVEN_G100 = (0 << 8),
	DISP_DOT_EVEN_G75  = (1 << 8),
	DISP_DOT_EVEN_G50  = (2 << 8),
	DISP_DOT_EVEN_G25  = (3 << 8),
	DISP_DOT_EVEN_G0   = (4 << 8),

	DISP_DOT_ODD_G100  = (0 << 12),
	DISP_DOT_ODD_G75   = (1 << 12),
	DISP_DOT_ODD_G50   = (2 << 12),
	DISP_DOT_ODD_G25   = (3 << 12),
	DISP_DOT_ODD_G0    = (4 << 12),

	DISP_DOT_EVEN_B100 = (0 << 16),
	DISP_DOT_EVEN_B75  = (1 << 16),
	DISP_DOT_EVEN_B50  = (2 << 16),
	DISP_DOT_EVEN_B25  = (3 << 16),
	DISP_DOT_EVEN_B0   = (4 << 16),

	DISP_DOT_ODD_B100  = (0 << 20),
	DISP_DOT_ODD_B75   = (1 << 20),
	DISP_DOT_ODD_B50   = (2 << 20),
	DISP_DOT_ODD_B25   = (3 << 20),
	DISP_DOT_ODD_B0    = (4 << 20),

	DISP_DOT_ALL_FULL  = 0x000000,
	DISP_DOT_ALL_HALF  = 0x222222,
} dispDotAverageMode_e;  /* plz keep sync to halDispDotAverageMode_e. */

typedef enum _dispRgb888RoundMode_e {
	DISP_RGB888_ROUND_DISABLE   = (0 << 4),
	DISP_RGB888_ROUND_TO_RGB666 = (1 << 4),
	DISP_RGB888_ROUND_TO_RGB565 = (2 << 4),
	DISP_RGB888_ROUND_TO_RGB555 = (3 << 4),
} dispRgb888RoundMode_e;  /* plz keep sync to halDispRgb888RoundMode_e. */

typedef enum _dispUpsHsdMode_e {
	DISP_UPS051_HSD_HSYNCW   = (0 << 0),
	DISP_UPS051_HSD_HSYNCOS  = (1 << 0),

	DISP_UPS052_HSD_NO_DELAY = (1 << 0),
	DSIP_UPS052_HSD_DELAY_1T = (2 << 0),
	DSIP_UPS052_HSD_DELAY_2T = (4 << 0),
	DISP_UPS052_HSD_DELAY_3T = (0 << 0),

	/* backward compatible. */
	DISP_UPS_HSD_NO_DELAY    = DISP_UPS052_HSD_NO_DELAY,
	DISP_UPS_HSD_DELAY_3T    = DISP_UPS052_HSD_DELAY_3T,
	DISP_UPS_HSD_DELAY_2T    = DSIP_UPS052_HSD_DELAY_2T,
} dispUpsHsdMode_e;  /* plz keep sync to halDispUpsHsdMode_e. */

typedef enum _dispUpsLayoutMode_e {
	DISP_UPS_LAYOUT_DISABLE       = (0 << 0),
	DISP_UPS_LAYOUT_ODD_DELAY_1T  = (1 << 0),
	DISP_UPS_LAYOUT_EVEN_DELAY_1T = (3 << 0),
} dispUpsLayoutMode_e;  /* plz keep sync to halDispUpsLayoutMode_e. */

typedef enum _dispUpsEvenColorFilter_e {
	DISP_UPS051_EVEN_RGB = (0 << 0),
	DISP_UPS051_EVEN_GBR = (1 << 0),
	DISP_UPS051_EVEN_BRG = (2 << 0),
	DISP_UPS051_EVEN_BGR = (3 << 0),
	DISP_UPS051_EVEN_RBG = (4 << 0),
	DISP_UPS051_EVEN_GRB = (5 << 0),

	DISP_UPS052_EVEN_RGB = (0 << 0),
	DISP_UPS052_EVEN_GBR = (1 << 0),
	DISP_UPS052_EVEN_BRG = (2 << 0),
	DISP_UPS052_EVEN_BGR = (3 << 0),
	DISP_UPS052_EVEN_RBG = (4 << 0),
	DISP_UPS052_EVEN_GRB = (5 << 0),
} dispUpsEvenColorFilter_e;  /* plz keep sync to halDispUpsEvenColorFilter_e. */

typedef enum _dispUpsOddColorFilter_e {
	DISP_UPS051_ODD_RGB = (0 << 4),
	DISP_UPS051_ODD_GBR = (1 << 4),
	DISP_UPS051_ODD_BRG = (2 << 4),
	DISP_UPS051_ODD_BGR = (3 << 4),
	DISP_UPS051_ODD_RBG = (4 << 4),
	DISP_UPS051_ODD_GRB = (5 << 4),

	DISP_UPS052_ODD_RGB = (0 << 4),
	DISP_UPS052_ODD_GBR = (1 << 4),
	DISP_UPS052_ODD_BRG = (2 << 4),
	DISP_UPS052_ODD_BGR = (3 << 4),
	DISP_UPS052_ODD_RBG = (4 << 4),
	DISP_UPS052_ODD_GRB = (5 << 4),
} dispUpsOddColorFilter_e;  /* plz keep sync to halDispUpsOddColorFilter_e. */

typedef enum _dispCcirOutputSeq_e {
	DISP_CCIR_YCRYCB = (0 << 0),
	DISP_CCIR_CRYCBY = (1 << 0),
	DISP_CCIR_YCBYCR = (2 << 0),
	DISP_CCIR_CBYCRY = (3 << 0),
} dispCcirOutputSeq_e;  /* plz keep sync to halDispCcirOutputSeq_e. */

typedef enum _dispCcirClampMode_e {
	DISP_CCIR_CLAMP_DISABLE = (0 << 4),
	DISP_CCIR_CLAMP_YCBCR   = (1 << 4),
	DISP_CCIR_CLAMP_YUV     = (2 << 4),
} dispCcirClampMode_e;  /* plz keep sync to halDispCcirClampMode_e. */

typedef enum _dispBt1120OutputFmt_e {
	DISP_BT1120_DS_R_G_B         = (0 << 0),
	DISP_BT1120_DS_Y_CBCR        = (1 << 0),
	DISP_BT1120_DS_YCBCR_COSITED = (2 << 0),  /* reserved for ccir656 */
} dispBt1120OutputFmt_e;  /* plz keep sync to _halDispBt1120OutputFmt_e. */

typedef enum _dispLcmBusType_e {
	DISP_LCM_I80 = (0 << 0),
	DISP_LCM_M68 = (1 << 0),
} dispLcmBusType_e;  /* plz keep sync to halDispLcmBusType_e. */

typedef enum _dispLcmDataBusWidth_e {
	DISP_LCM_8_BIT  = (0 << 4),
	DISP_LCM_9_BIT  = (1 << 4),
	DISP_LCM_16_BIT = (2 << 4),
	DISP_LCM_18_BIT = (3 << 4),
	DISP_LCM_24_BIT = (4 << 4),
} dispLcmDataBusWidth_e;  /* plz keep sync to halDispLcmDataBusWidth_e. */

typedef enum _dispLcmDataFmt_e {
	DISP_LCM_RGB565 = (0 << 4),
	DISP_LCM_RGB666 = (1 << 4),
} dispLcmDataFmt_e;  /* plz keep sync to halDispLcmDataFmt_e. */

typedef enum _dispLcmDataAlign_e {
	DISP_LCM_ALIGN_NONE = (0 << 5),
	DISP_LCM_ALIGN_BIG  = (1 << 5),
	DISP_LCM_ALIGN_MID  = (2 << 5),
	DISP_LCM_ALIGN_BOTH = (3 << 5),
} dispLcmDataAlign_e;  /* plz keep sync to halDispLcmDataAlign_e. */

typedef enum _dispDsiDphyTxClkSel_e {
	DISP_DSI_0_250_MHZ    = (0 << 0) | (0 << 1) | (0 << 3),
	DISP_DSI_250_550_MHZ  = (1 << 0) | (1 << 1) | (0 << 3),
	DISP_DSI_550_1000_MHZ = (1 << 0) | (3 << 1) | (1 << 3),
} dispDsiDphyTxClkSel_e;  /* plz keep sync to halDispDsiDphyTxClk_e. */

typedef enum _dispDsiOpMode_e {
	DISP_DSI_VIDEO_MODE   = (0 << 4),
	DISP_DSI_COMMAND_MODE = (1 << 4),
	DISP_DSI_DMA_MODE     = (2 << 4),
} dispDsiOpMode_e;  /* plz keep sync to halDispDsiOpMode_e. */

typedef enum _dispDsiClkMode_e {
	DISP_DSI_CLK_NON_CONTINUOUS = (0 << 4),
	DISP_DSI_CLK_CONTINUOUS     = (1 << 4),
} dispDsiClkMode_e;  /* plz keep sync to halDispDsiClkMode_e. */

typedef enum _dispDsiDevId_e {
	DISP_DSI_DEV   = 0,
	DISP_DSI_DEV_D = 1,
} dispDsiDevId_e;  /* plz keep sync to halDispDsiDevId_e. */

/** @brief Refer to spec MIPI Alliance Specification for Display Serial Interface v1.02.00 Table 16. */
typedef enum _dispDsiDataType_e {
	DISP_DSI_DT_YCBCR422_20_BIT_LOOSELY = 0x0C,  /* Loosely Packed Pixel Stream, 20-bit YCbCr, 4:2:2 Format */
	DISP_DSI_DT_YCBCR422_24_BIT         = 0x1C,  /* Packed Pixel Stream, 24-bit YCbCr, 4:2:2 Format */
	DISP_DSI_DT_YCBCR422_16_BIT         = 0x2C,  /* Packed Pixel Stream, 16-bit YCbCr, 4:2:2 Format */
	DISP_DSI_DT_RGB101010_30_BIT        = 0x0D,  /* Packed Pixel Stream, 30-bit RGB, 10-10-10 Format */
	DISP_DSI_DT_RGB121212_36_BIT        = 0x1D,  /* Packed Pixel Stream, 36-bit RGB, 12-12-12 Format */
	DISP_DSI_DT_YCBCR420_12_BIT         = 0x3D,  /* Packed Pixel Stream, 12-bit YCbCr, 4:2:0 Format */
	DISP_DSI_DT_RGB565_16_BIT           = 0x0E,  /* Packed Pixel Stream, 16-bit RGB, 5-6-5 Format */
	DISP_DSI_DT_RGB666_18_BIT           = 0x1E,  /* Packed Pixel Stream, 18-bit RGB, 6-6-6 Format */
	DISP_DSI_DT_RGB666_18_BIT_LOOSELY   = 0x2E,  /* Loosely Packed Pixel Stream, 18-bit RGB, 6-6-6 Format */
	DISP_DSI_DT_RGB888_24_BIT           = 0x3E,  /* Packed Pixel Stream, 24-bit RGB, 8-8-8 Format */
} dispDsiDataType_e;  /* plz keep sync to halDispDsiPixelStreamDataType_e. */

typedef enum _dispDsiVideoMode_e {
	DISP_DSI_NON_BURST_SYNC_PULSE  = (0 << 6),
	DISP_DSI_NON_BURST_SYNC_EVENT  = (1 << 6),
	DISP_DSI_BURST                 = (2 << 6),
} dispDsiVideoMode_e;  /* plz keep sync to halDispDsiVideoMode_e. */

typedef enum _dispDsiBllpSel_e {
	DISP_DSI_HS_MODE          = (0 << 6),  /* Transmit null packets using HS Mode */
	DISP_DSI_LP11_ESCAPE_MODE = (1 << 6),  /* Remain in LP-11 state or Transmit packets using Escape Mode */
} dispDsiBllpSel_e;  /* plz keep sync to halDispDsiBllpSel_e. */

typedef enum _dispPinSel_e {
	DISP_PIN_SEL_SIF2          = 4,  /**< 0: gpio[15:13], 1: sif2sdo/sif2sck/sif2sen. */
	DISP_PIN_SEL_TVGPIO_15TO0  = 5,  /**< 0: gpio[30:17, 12:11], 1: tvgpio[15:0]. */
	DISP_PIN_SEL_TVGPIO_27TO16 = 6,  /**< 0: gpio[42:31, 15:13], 1: tvgpio[27:16, 27:25]. */
	DISP_PIN_SEL_TV_LCM        = 7,  /**< TV LCM interface multiplex selection 0: pinloc 1, 1: pinloc 2. */
} dispPinSel_e;  /* plz keep sync to halDispPinSel_e. */

typedef enum _dispHdmiPktType_e {  /* spca6330. */
	DISP_HDMI_PKT_ACR      = 0x01,  /**< Audio Clock Regeneration Packet, refer to spec HDMI 1.4 section 5.3.3 and 7.2. */
	DISP_HDMI_PKT_GC       = 0x03,  /**< General Control Packet, refer to spec HDMI 1.4 section 5.3.6. */
	DISP_HDMI_PKT_ACP      = 0x04,  /**< Audio Content Protection Packet, refer to spec HDMI 1.4 section 5.3.7. */
	DISP_HDMI_PKT_ISRC1    = 0x05,  /**< ISRC1 Packet, refer to spec HDMI 1.4 section 5.3.8. */
	DISP_HDMI_PKT_ISRC2    = 0x06,  /**< ISRC2 Packet, refer to spec HDMI 1.4 section 5.3.8. */
	DISP_HDMI_PKT_GM       = 0x0A,  /**< Gamut Metadata Packet, refer to spec HDMI 1.4 section 5.3.12. */
	DISP_HDMI_PKT_VS_IF    = 0x81,  /**< Vendor Specific InfoFrame Packet, refer to spec HDMI 1.4 section 8.2.3. */
	DISP_HDMI_PKT_AVI_IF   = 0x82,  /**< AVI InfoFrame Packet, refer to spec HDMI 1.4 section 8.2.1. */
	DISP_HDMI_PKT_SPD_IF   = 0x83,  /**< Source Product Descriptor InfoFrame Packet. */
	DISP_HDMI_PKT_AUDIO_IF = 0x84,  /**< Audio InfoFrame Packet, refer to spec HDMI 1.4 section 8.2.2. */
	DISP_HDMI_PKT_MPEG_IF  = 0x85,  /**< MPEG Source InfoFrame Packet. */
} dispHdmiPktType_e;  /* plz keep sync to halDispHdmiPktType_e. */

typedef enum _dispHdmiPktEn_e {  /* spca6330. */
	DISP_HDMI_PKT_GC_EN       = (1 <<  0),  /**< General Control Packet, refer to spec HDMI 1.4 section 5.3.6. */
	DISP_HDMI_PKT_GM_EN       = (1 <<  1),  /**< Gamut Metadata Packet, refer to spec HDMI 1.4 section 5.3.12. */
	DISP_HDMI_PKT_ACR_EN      = (1 <<  2),  /**< Audio Clock Regeneration Packet, refer to spec HDMI 1.4 section 5.3.3 and 7.2. */
	DISP_HDMI_PKT_AS_EN       = (1 <<  3),  /**< Audio Sample Packet, refer to spec HDMI 1.4 section 5.3.4. */
	#if SPCA6350
	DISP_HDMI_PKT_NP_EN       = (1 <<  4),  /**< Null Packet, refer to spec HDMI 1.4 section 5.3.2. */
	#else
	DISP_HDMI_PKT_ACP_EN      = (1 <<  4),  /**< Audio Content Protection Packet, refer to spec HDMI 1.4 section 5.3.7. */
	DISP_HDMI_PKT_ISRC1_EN    = (1 <<  5),  /**< ISRC1 Packet, refer to spec HDMI 1.4 section 5.3.8. */
	DISP_HDMI_PKT_ISRC2_EN    = (1 <<  6),  /**< ISRC2 Packet, refer to spec HDMI 1.4 section 5.3.8. */
	#endif
	DISP_HDMI_PKT_VS_IF_EN    = (1 <<  7),  /**< Vendor Specific InfoFrame Packet, refer to spec HDMI 1.4 section 8.2.3. */
	DISP_HDMI_PKT_AVI_IF_EN   = (1 <<  8),  /**< AVI InfoFrame Packet, refer to spec HDMI 1.4 section 8.2.1. */
	DISP_HDMI_PKT_SPD_IF_EN   = (1 <<  9),  /**< Source Product Descriptor InfoFrame Packet. */
	DISP_HDMI_PKT_AUDIO_IF_EN = (1 << 10),  /**< Audio InfoFrame Packet, refer to spec HDMI 1.4 section 8.2.2. */
	DISP_HDMI_PKT_MPEG_IF_EN  = (1 << 11),  /**< MPEG Source InfoFrame Packet. */
} dispHdmiPktEn_e;  /* plz keep sync to halDispHdmiPktEn_e. */

typedef enum _dispHdmiColorDepth_e {
	DISP_HDMI_COLOR_DEPTH_24_BIT = (0 << 1),
	DISP_HDMI_COLOR_DEPTH_30_BIT = (1 << 1),
	DISP_HDMI_COLOR_DEPTH_36_BIT = (2 << 1),
	DISP_HDMI_COLOR_DEPTH_48_BIT = (3 << 1),
} dispHdmiColorDepth_e;  /* plz keep sync to halDispHdmiColorDepth_e. */

typedef enum _dispEdid3dStructId_e {
	DISP_EDID_3D_STRUCT_FP       = 0x0,  /* Frame packing. */
	DISP_EDID_3D_STRUCT_FA       = 0x1,  /* Field alternative. */
	DISP_EDID_3D_STRUCT_LA       = 0x2,  /* Line alternative. */
	DISP_EDID_3D_STRUCT_SBSF     = 0x3,  /* Side-by-Side (Full). */
	DISP_EDID_3D_STRUCT_LD       = 0x4,  /* L + depth. */
	DISP_EDID_3D_STRUCT_LDGGD    = 0x5,  /* L + depth + graphics + graphics-depth. */
	DISP_EDID_3D_STRUCT_TAB      = 0x6,  /* Top-and-Bottom. */
	DISP_EDID_3D_STRUCT_SBSHHSS  = 0x8,  /* Side-by-Side (Half) with horizontal sub-sampling. */
	DISP_EDID_3D_STRUCT_SBSHAQSS = 0xF,  /* Side-by-Side (Half) with all quincunx sub-sampling. */
	DISP_EDID_3D_STRUCT_TOT,
} dispEdid3dStructId_e;

/** @brief EDID video format, refer to spec CEA-861-F Table 3 and HDMI 1.4 Table H-2, H-7. */
typedef enum _dispEdidVideoFormat_e {
	/* 2D formats. */
	DISP_EDID_VIC_MSK                      = 0xFF,  /* driver can use this mask to get video format identification code. */
	DISP_EDID_640X480P_60HZ_4T3            =  1,
	DISP_EDID_720X480P_60HZ_4T3            =  2,
	DISP_EDID_720X480P_60HZ_16T9           =  3,
	DISP_EDID_1280X720P_60HZ_16T9          =  4,
	DISP_EDID_1920X1080I_60HZ_16T9         =  5,
	DISP_EDID_720X480I_60HZ_4T3            =  6,
	DISP_EDID_720X480I_60HZ_16T9           =  7,
	DISP_EDID_720X240P_60HZ_4T3            =  8,
	DISP_EDID_720X240P_60HZ_16T9           =  9,
	DISP_EDID_2880X480I_60HZ_4T3           = 10,
	DISP_EDID_2880X480I_60HZ_16T9          = 11,
	DISP_EDID_2880X240P_60HZ_4T3           = 12,
	DISP_EDID_2880X240P_60HZ_16T9          = 13,
	DISP_EDID_1440X480P_60HZ_4T3           = 14,
	DISP_EDID_1440X480P_60HZ_16T9          = 15,
	DISP_EDID_1920X1080P_60HZ_16T9         = 16,
	DISP_EDID_720X576P_50HZ_4T3            = 17,
	DISP_EDID_720X576P_50HZ_16T9           = 18,
	DISP_EDID_1280X720P_50HZ_16T9          = 19,
	DISP_EDID_1920X1080I_50HZ_16T9         = 20,
	DISP_EDID_720X576I_50HZ_4T3            = 21,
	DISP_EDID_720X576I_50HZ_16T9           = 22,
	DISP_EDID_720X288P_50HZ_4T3            = 23,
	DISP_EDID_720X288P_50HZ_16T9           = 24,
	DISP_EDID_2880X576I_50HZ_4T3           = 25,
	DISP_EDID_2880X576I_50HZ_16T9          = 26,
	DISP_EDID_2880X288P_50HZ_4T3           = 27,
	DISP_EDID_2880X288P_50HZ_16T9          = 28,
	DISP_EDID_1440X576P_50HZ_4T3           = 29,
	DISP_EDID_1440X576P_50HZ_16T9          = 30,
	DISP_EDID_1920X1080P_50HZ_16T9         = 31,
	DISP_EDID_1920X1080P_24HZ_16T9         = 32,
	DISP_EDID_1920X1080P_25HZ_16T9         = 33,
	DISP_EDID_1920X1080P_30HZ_16T9         = 34,
	DISP_EDID_2880X480P_60HZ_4T3           = 35,
	DISP_EDID_2880X480P_60HZ_16T9          = 36,
	DISP_EDID_2880X576P_50HZ_4T3           = 37,
	DISP_EDID_2880X576P_50HZ_16T9          = 38,
	DISP_EDID_1920X1080I_1250TOT_50HZ_16T9 = 39,
	DISP_EDID_1920X1080I_100HZ_16T9        = 40,
	DISP_EDID_1280X720P_100HZ_16T9         = 41,
	DISP_EDID_720X576P_100HZ_4T3           = 42,
	DISP_EDID_720X576P_100HZ_16T9          = 43,
	DISP_EDID_720X576I_100HZ_4T3           = 44,
	DISP_EDID_720X576I_100HZ_16T9          = 45,
	DISP_EDID_1920X1080I_120HZ_16T9        = 46,
	DISP_EDID_1280X720P_120HZ_16T9         = 47,
	DISP_EDID_720X480P_120HZ_4T3           = 48,
	DISP_EDID_720X480P_120HZ_16T9          = 49,
	DISP_EDID_720X480I_120HZ_4T3           = 50,
	DISP_EDID_720X480I_120HZ_16T9          = 51,
	DISP_EDID_720X576P_200HZ_4T3           = 52,
	DISP_EDID_720X576P_200HZ_16T9          = 53,
	DISP_EDID_720X576I_200HZ_4T3           = 54,
	DISP_EDID_720X576I_200HZ_16T9          = 55,
	DISP_EDID_720X480P_240HZ_4T3           = 56,
	DISP_EDID_720X480P_240HZ_16T9          = 57,
	DISP_EDID_720X480I_240HZ_4T3           = 58,
	DISP_EDID_720X480I_240HZ_16T9          = 59,
	DISP_EDID_1280X720P_24HZ_16T9          = 60,
	DISP_EDID_1280X720P_25HZ_16T9          = 61,
	DISP_EDID_1280X720P_30HZ_16T9          = 62,
	DISP_EDID_1920X1080P_120HZ_16T9        = 63,
	DISP_EDID_1920X1080P_100HZ_16T9        = 64,
	DISP_EDID_1280X720P_24HZ_64T27         = 65,
	DISP_EDID_1280X720P_25HZ_64T27         = 66,
	DISP_EDID_1280X720P_30HZ_64T27         = 67,
	DISP_EDID_1280X720P_50HZ_64T27         = 68,
	DISP_EDID_1280X720P_60HZ_64T27         = 69,
	DISP_EDID_1280X720P_100HZ_64T27        = 70,
	DISP_EDID_1280X720P_120HZ_64T27        = 71,
	DISP_EDID_1920X1080P_24HZ_64T27        = 72,
	DISP_EDID_1920X1080P_25HZ_64T27        = 73,
	DISP_EDID_1920X1080P_30HZ_64T27        = 74,
	DISP_EDID_1920X1080P_50HZ_64T27        = 75,
	DISP_EDID_1920X1080P_60HZ_64T27        = 76,
	DISP_EDID_1920X1080P_100HZ_64T27       = 77,
	DISP_EDID_1920X1080P_120HZ_64T27       = 78,
	DISP_EDID_1680X720P_24HZ_64T27         = 79,
	DISP_EDID_1680X720P_25HZ_64T27         = 80,
	DISP_EDID_1680X720P_30HZ_64T27         = 81,
	DISP_EDID_1680X720P_50HZ_64T27         = 82,
	DISP_EDID_1680X720P_60HZ_64T27         = 83,
	DISP_EDID_1680X720P_100HZ_64T27        = 84,
	DISP_EDID_1680X720P_120HZ_64T27        = 85,
	DISP_EDID_2560X1080P_24HZ_64T27        = 86,
	DISP_EDID_2560X1080P_25HZ_64T27        = 87,
	DISP_EDID_2560X1080P_30HZ_64T27        = 88,
	DISP_EDID_2560X1080P_50HZ_64T27        = 89,
	DISP_EDID_2560X1080P_60HZ_64T27        = 90,
	DISP_EDID_2560X1080P_100HZ_64T27       = 91,
	DISP_EDID_2560X1080P_120HZ_64T27       = 92,
	DISP_EDID_3840X2160P_24HZ_16T9         = 93,
	DISP_EDID_3840X2160P_25HZ_16T9         = 94,
	DISP_EDID_3840X2160P_30HZ_16T9         = 95,
	DISP_EDID_3840X2160P_50HZ_16T9         = 96,
	DISP_EDID_3840X2160P_60HZ_16T9         = 97,
	DISP_EDID_4096X2160P_24HZ_256T135      = 98,
	DISP_EDID_4096X2160P_25HZ_256T135      = 99,
	DISP_EDID_4096X2160P_30HZ_256T135      = 100,
	DISP_EDID_4096X2160P_50HZ_256T135      = 101,
	DISP_EDID_4096X2160P_60HZ_256T135      = 102,
	DISP_EDID_3840X2160P_24HZ_64T27        = 103,
	DISP_EDID_3840X2160P_25HZ_64T27        = 104,
	DISP_EDID_3840X2160P_30HZ_64T27        = 105,
	DISP_EDID_3840X2160P_50HZ_64T27        = 106,
	DISP_EDID_3840X2160P_60HZ_64T27        = 107,

	/* alias. */
	DISP_EDID_1440X480I_60HZ_4T3           = DISP_EDID_720X480I_60HZ_4T3,
	DISP_EDID_1440X480I_60HZ_16T9          = DISP_EDID_720X480I_60HZ_16T9,
	DISP_EDID_1440X240P_60HZ_4T3           = DISP_EDID_720X240P_60HZ_4T3,
	DISP_EDID_1440X240P_60HZ_16T9          = DISP_EDID_720X240P_60HZ_16T9,
	DISP_EDID_1440X576I_50HZ_4T3           = DISP_EDID_720X576I_50HZ_4T3,
	DISP_EDID_1440X576I_50HZ_16T9          = DISP_EDID_720X576I_50HZ_16T9,
	DISP_EDID_1440X288P_50HZ_4T3           = DISP_EDID_720X288P_50HZ_4T3,
	DISP_EDID_1440X288P_50HZ_16T9          = DISP_EDID_720X288P_50HZ_16T9,
	DISP_EDID_1440X576I_100HZ_4T3          = DISP_EDID_720X576I_100HZ_4T3,
	DISP_EDID_1440X576I_100HZ_16T9         = DISP_EDID_720X576I_100HZ_16T9,
	DISP_EDID_1440X480I_120HZ_4T3          = DISP_EDID_720X480I_120HZ_4T3,
	DISP_EDID_1440X480I_120HZ_16T9         = DISP_EDID_720X480I_120HZ_16T9,
	DISP_EDID_1440X576I_200HZ_4T3          = DISP_EDID_720X576I_200HZ_4T3,
	DISP_EDID_1440X576I_200HZ_16T9         = DISP_EDID_720X576I_200HZ_16T9,
	DISP_EDID_1440X480I_240HZ_4T3          = DISP_EDID_720X480I_240HZ_4T3,
	DISP_EDID_1440X480I_240HZ_16T9         = DISP_EDID_720X480I_240HZ_16T9,

	/* 3D structures. */
	DISP_EDID_3D_STRUCT_MSK                = 0x1F00,  /* driver can use this mask to get 3D structure. */
	DISP_EDID_3D_FP                        = (0x1000 | (DISP_EDID_3D_STRUCT_FP       << 8)),
	DISP_EDID_3D_FA                        = (0x1000 | (DISP_EDID_3D_STRUCT_FA       << 8)),
	DISP_EDID_3D_LA                        = (0x1000 | (DISP_EDID_3D_STRUCT_LA       << 8)),
	DISP_EDID_3D_SBSF                      = (0x1000 | (DISP_EDID_3D_STRUCT_SBSF     << 8)),
	DISP_EDID_3D_LD                        = (0x1000 | (DISP_EDID_3D_STRUCT_LD       << 8)),
	DISP_EDID_3D_LDGGD                     = (0x1000 | (DISP_EDID_3D_STRUCT_LDGGD    << 8)),
	DISP_EDID_3D_TAB                       = (0x1000 | (DISP_EDID_3D_STRUCT_TAB      << 8)),
	DISP_EDID_3D_SBSHHSS                   = (0x1000 | (DISP_EDID_3D_STRUCT_SBSHHSS  << 8)),
	DISP_EDID_3D_SBSHAQSS                  = (0x1000 | (DISP_EDID_3D_STRUCT_SBSHAQSS << 8)),
} dispEdidVideoFormat_e;

#define DISP_EDID_2D_VIDEO_FMT_TOT  DISP_EDID_1920X1080P_100HZ_16T9
#define DISP_EDID_VIDEO_FMT_TOT     (DISP_EDID_2D_VIDEO_FMT_TOT * DISP_EDID_3D_STRUCT_TOT)
#define DISP_EDID_AUDIO_FMT_TOT     16

typedef enum _dispLcmCmdHeader_e {
	DISP_LCM_CMD_INST     = 0x01000000L,
	DISP_LCM_CMD_PARA     = 0x02000000L,
	DISP_LCM_CMD_WAIT     = 0x03000000L,
	DISP_LCM_CMD_STOP     = 0x04000000L,
	DISP_LCM_CMD_CS_ACT   = 0x05000000L,
	DISP_LCM_CMD_CS_INACT = 0x06000000L,
} dispLcmCmdHeader_e;  /* plz keep sync to halDispLcmCmdHeader_e. */

/** @brief Refer to spec MIPI Alliance Specification for Display Serial Interface v1.02.00 Table 16. */
typedef enum _dispDsiPacketDataType_e {
	DISP_DSI_DT_VSYNC_START           = 0x01,  /* Sync Event, V Sync Start. */
	DISP_DSI_DT_VSYNC_END             = 0x11,  /* Sync Event, V Sync End. */
	DISP_DSI_DT_HSYNC_START           = 0x21,  /* Sync Event, H Sync Start. */
	DISP_DSI_DT_HSYNC_END             = 0x31,  /* Sync Event, V Sync End. */
	DISP_DSI_DT_EOTP                  = 0x08,  /* End of Transmission packet. */
	DISP_DSI_DT_COLOR_MODE_OFF        = 0x02,  /* Color Mode Off. */
	DISP_DSI_DT_COLOR_MODE_ON         = 0x12,  /* Color Mode On. */
	DISP_DSI_DT_SHUT_DOWN             = 0x22,  /* Shut Down Peripheral. */
	DISP_DSI_DT_TURN_ON               = 0x32,  /* Turn On Peripheral. */
	DISP_DSI_DT_GENERIC_SHORT_WRITE   = 0x03,  /* Generic Short WRITE, no parameters. */
	DISP_DSI_DT_GENERIC_SHORT_WRITE_1 = 0x13,  /* Generic Short WRITE, 1 parameter. */
	DISP_DSI_DT_GENERIC_SHORT_WRITE_2 = 0x23,  /* Generic Short WRITE, 2 parameters. */
	DISP_DSI_DT_GENERIC_READ          = 0x04,  /* Generic READ, no parameters. */
	DISP_DSI_DT_GENERIC_READ_1        = 0x14,  /* Generic READ, 1 parameter. */
	DISP_DSI_DT_GENERIC_READ_2        = 0x24,  /* Generic READ, 2 parameters. */
	DISP_DSI_DT_DCS_SHORT_WRITE       = 0x05,  /* DCS Short WRITE, no parameters. */
	DISP_DSI_DT_DCS_SHORT_WRITE_1     = 0x15,  /* DCS Short WRITE, 1 parameter. */
	DISP_DSI_DT_DCS_READ              = 0x06,  /* DCS READ, no parameters. */
	DISP_DSI_DT_MAX_RETURN_SIZE_SET   = 0x37,  /* Set Maximum Return Packet Size. */
	DISP_DSI_DT_EOT_PACKET            = 0x08,  /* Enable EoT packet. */
	DISP_DSI_DT_NULL_PACKET           = 0x09,  /* Null Packet, no data. */
	DISP_DSI_DT_BLANKING_PACKET       = 0x19,  /* Blanking Packet, no data. */
	DISP_DSI_DT_GENERIC_LONG_WRITE    = 0x29,  /* Generic Long Write. */
	DISP_DSI_DT_DCS_LONG_WRITE        = 0x39,  /* DCS Long Write/write_LUT Command Packet. */
	DISP_DSI_DT_WAIT                  = 0xFF,  /* SW delay. */
} dispDsiPacketDataType_e;  /* plz keep sync to halDispDsiPacketDataType_e. */

/** @brief Refer to spec MIPI Alliance Specification for Display Command Set v1.02.00 Table 1. */
typedef enum _dispDcs_e {
	DISP_DCS_ENTER_IDLE_MODE       = 0x39,
	DISP_DCS_ENTER_INVERT_MODE     = 0x21,
	DISP_DCS_ENTER_NORMAL_MODE     = 0x13,
	DISP_DCS_ENTER_PARTIAL_MODE    = 0x12,
	DISP_DCS_ENTER_SLEEP_MODE      = 0x10,
	DISP_DCS_EXIT_IDLE_MODE        = 0x38,
	DISP_DCS_EXIT_INVERT_MODE      = 0x20,
	DISP_DCS_EXIT_SLEEP_MODE       = 0x11,
	DISP_DCS_GET_ADDRESS_MODE      = 0x0B,
	DISP_DCS_GET_BLUE_CHANNEL      = 0x08,
	DISP_DCS_GET_DIAGNOSTIC_RESULT = 0x0F,
	DISP_DCS_GET_DISPLAY_MODE      = 0x0D,
	DISP_DCS_GET_GREEN_CHANNEL     = 0x07,
	DISP_DCS_GET_PIXEL_FORMAT      = 0x0C,
	DISP_DCS_GET_POWER_MODE        = 0x0A,
	DISP_DCS_GET_RED_CHANNEL       = 0x06,
	DISP_DCS_GET_SCANLINE          = 0x45,
	DISP_DCS_GET_SIGNAL_MODE       = 0x0E,
	DISP_DCS_NOP                   = 0x00,
	DISP_DCS_READ_DDB_CONTINUE     = 0xA8,
	DISP_DCS_READ_DDB_START        = 0xA1,
	DISP_DCS_READ_MEMORY_CONTINUE  = 0x3E,
	DISP_DCS_READ_MEMORY_START     = 0x2E,
	DISP_DCS_SET_ADDRESS_MODE      = 0x36,
	DISP_DCS_SET_COLUMN_ADDRESS    = 0x2A,
	DISP_DCS_SET_DISPLAY_OFF       = 0x28,
	DISP_DCS_SET_DISPLAY_ON        = 0x29,
	DISP_DCS_SET_GAMMA_CURVE       = 0x26,
	DISP_DCS_SET_PAGE_ADDRESS      = 0x2B,
	DISP_DCS_SET_PARTIAL_COLUMNS   = 0x31,
	DISP_DCS_SET_PARTIAL_ROWS      = 0x30,
	DISP_DCS_SET_PIXEL_FORMAT      = 0x3A,
	DISP_DCS_SET_SCROLL_AREA       = 0x33,
	DISP_DCS_SET_SCROLL_START      = 0x37,
	DISP_DCS_SET_TEAR_OFF          = 0x34,
	DISP_DCS_SET_TEAR_ON           = 0x35,
	DISP_DCS_SET_TEAR_SCANLINE     = 0x44,
	DISP_DCS_SOFT_RESET            = 0x01,
	DISP_DCS_WRITE_LUT             = 0x2D,
	DISP_DCS_WRITE_MEMORY_CONTINUE = 0x3C,
	DISP_DCS_WRITE_MEMORY_START    = 0x2C,
} dispDcs_e;

typedef enum _dispDzoomFitMethod_e {
	DISP_DZOOM_FIT_METHOD_DST_INSIDE,
	DISP_DZOOM_FIT_METHOD_DST_OUTSIDE,
	DISP_DZOOM_FIT_METHOD_SRC_VARY_WINDOW,
	DISP_DZOOM_FIT_METHOD_SRC_FIX_WINDOW,
	DISP_DZOOM_FIT_METHOD_SRC_STRETCH,
} dispDzoomFitMethod_e;  /* plz keep sync to halCanvasFitMethod_e. */

#define DISP_DEV_TOT              5
#define DISP_SRC_TOT              2
#define DISP_CHNL_TOT             2
#define DISP_ALPHA_MAX            128
#define DISP_BLEND_TBL_ENTRY_TOT  4
#define DISP_PALETTE_ENTRY_TOT    256
#define DISP_HDMI_CHANNEL_TOT     1

/**************************************************************************
 *                          D A T A   T Y P E S
 **************************************************************************/
typedef struct _dispCapabDesc_t {
	UINT32 width;
	UINT32 height;
	UINT32 xoff;
	UINT32 yoff;
	UINT32 aspectRatioX;
	UINT32 aspectRatioY;
	UINT32 logiEn;        /*!< logical size is in aspectRatioX and aspectRatioY or not */
	UINT32 frameRate;     /* frame rate in 1000X. */
} dispCapabDesc_t;

typedef UINT8 *(*pfOperatingDispDevNameGet_t)(void);
typedef UINT32 (*pfOperatingDispPowerOn_t)(void);
typedef UINT32 (*pfOperatingDispPowerOff_t)(void);
typedef void   (*pfOperatingDispCapabilityGet_t)(dispCapabDesc_t* pcapabDesc, UINT32 descSize);
typedef UINT32 (*pfOperatingDispResolutionSet_t)(UINT32 resId);
typedef UINT32 (*pfOperatingDispCfgSet_t)(UINT32 cfgId, va_list args);
typedef UINT32 (*pfOperationDispDevCmd)(UINT32 mode);

typedef struct _dispOperatingFunc_t {
	pfOperatingDispDevNameGet_t pfOpDevNameGet;
	pfOperatingDispPowerOn_t pfOpPowerOn;
	pfOperatingDispPowerOff_t pfOpPowerOff;
	pfOperatingDispCapabilityGet_t pfOpCapabilityGet;
	pfOperatingDispResolutionSet_t pfOpResolutionSet;
	pfOperatingDispCfgSet_t pfOpCfgSet;
	pfOperationDispDevCmd pfOpDevCmd;
} dispOperatingFunc_t;

typedef struct _dispPara_t {
	UINT32 paraId;
	SINT32 para1;
	SINT32 para2;
	SINT32 para3;
	SINT32 para4;
	UINT32 upd;
} dispPara_t;

typedef UINT16 dispHdmiVideoFmt_t;
typedef UINT32 dispHdmiAudioFmt_t;

typedef struct _dispDetailTiming_t {
	UINT32 pixelClk;
	UINT32 horActive;
	UINT32 horBlanking;
	UINT32 horSyncOffset;
	UINT32 horSyncPulseWidth;
	UINT32 horImgSize;
	UINT32 horBorder;
	UINT32 verActive;
	UINT32 verBlanking;
	UINT32 verSyncOffset;
	UINT32 verSyncPulseWidth;
	UINT32 verImgSize;
	UINT32 verBorder;
	UINT32 flags;
} dispDetailTiming_t;

typedef struct _dispHdmiEedid_t {
	UINT32 isDviMode;    /**< if 1, it's DVI mode. */
	UINT32 isY444;       /**< if 1, support YCbCr 4:4:4, refer to spec HDMI 1.4 section 6.5.1. */
	UINT32 isY422;       /**< if 1, support YCbCr 4:2:2, refer to spec HDMI 1.4 section 6.5.1. */
	UINT32 isDc48bit;    /**< if 1, support Deep Color 48 bits/pixel, refer to spec HDMI 1.4 section 6.5.2. */
	UINT32 isDc36bit;    /**< if 1, support Deep Color 36 bits/pixel, refer to spec HDMI 1.4 section 6.5.2. */
	UINT32 isDc30bit;    /**< if 1, support Deep Color 30 bits/pixel, refer to spec HDMI 1.4 section 6.5.2. */
	UINT32 isDcY444;     /**< if 1, support Deep Color YCbCr 4:4:4, refer to spec HDMI 1.4 section 6.2.4. */
	UINT32 isRgbQrs;     /**< if 1, RGB Quantization Range Selectable, refer to spec CEA-861-E section 7.5.6. */
	UINT32 isYuvQrs;     /**< if 1, YUV Quantization Range Selectable, refer to spec CEA-861-E section 7.5.6. */
	UINT32 is3dPresent;  /**< EDID video 3D format present, refer to spec CEA-861-E section 8.3.2. */
	UINT32 phyAddr;      /**< Physical address in A.B.C.D format, where bit[15:12]=A, [11:8]=B, [7:4]=C, [3:0]=D. */
	UINT32 videoFmtNum;  /**< EDID video format number. */
	dispHdmiVideoFmt_t videoFmtTbl[DISP_EDID_VIDEO_FMT_TOT];  /**< EDID video format table, includes 2D and 3D formats. */
	UINT32 audioFmtNum;  /**< EDID audio format number. */
	dispHdmiAudioFmt_t audioFmtTbl[DISP_EDID_AUDIO_FMT_TOT];  /**< EDID audio format table. */
	UINT32 dtlTimeNum;   /**< Detailed timing number. */
	dispDetailTiming_t dtlTimeTbl[4];  /* Detailed timing table. */
} dispHdmiEedid_t;

/**************************************************************************
 *                              M A C R O S
 **************************************************************************/
#define RGB888_TO_RGB565(rgb888) \
	(UINT16)((UINT32)(((rgb888) & 0xF80000) >> 8) | \
	         (UINT32)(((rgb888) & 0x00FC00) >> 5) | \
	         (UINT32)(((rgb888) & 0x0000F8) >> 3))

#define BGR888_TO_RGB565(bgr888) \
	(UINT16)((UINT32)(((bgr888) & 0xF80000) >> 19) | \
	         (UINT32)(((bgr888) & 0x00FC00) >> 5) | \
	         (UINT32)(((bgr888) & 0x0000F8) << 8))

#define RGB888_TO_BGR565(rgb888)  BGR888_TO_RGB565(rgb888)

#define BGR888_TO_BGR565(bgr888)  RGB888_TO_RGB565(bgr888)

/*
 * DSI command, for dispDsiCmdTableSend().
 */
/* Generic Short WRITE, no parameters. */
#define DSI_GEN_SWR() \
	(DISP_DSI_DT_GENERIC_SHORT_WRITE << 24)
/* Generic Short WRITE, 1 parameter. */
#define DSI_GEN_SWR_1(prm) \
	((DISP_DSI_DT_GENERIC_SHORT_WRITE_1 << 24) | (prm))
/* Generic Short WRITE, 2 parameters. */
#define DSI_GEN_SWR_2(prm0, prm1) \
	((DISP_DSI_DT_GENERIC_SHORT_WRITE_2 << 24) | ((prm1) << 8) | (prm0))
/* Generic Long WRITE. */
#define DSI_GEN_LWR(len) \
	((DISP_DSI_DT_GENERIC_LONG_WRITE << 24) | len)
/* DCS Short WRITE, no parameters. */
#define DSI_DCS_SWR(cmd) \
	((DISP_DSI_DT_DCS_SHORT_WRITE << 24) | (cmd))
/* DCS Short WRITE, 1 parameter. */
#define DSI_DCS_SWR_1(cmd, prm) \
	((DISP_DSI_DT_DCS_SHORT_WRITE_1 << 24) | ((prm) << 8) | (cmd))	
/* DCS Short READ, no parameters. [ICOM-3684] For DsiRxCmd*/
#define DSI_DCS_SR(cmd) \
			((DISP_DSI_DT_DCS_READ << 24) | (cmd))
#define DSI_DCS_MRP(cmd) \
				((DISP_DSI_DT_MAX_RETURN_SIZE_SET << 24) | (cmd))
/* DCS Long WRITE. */
#define DSI_DCS_LWR(len) \
	((DISP_DSI_DT_DCS_LONG_WRITE << 24) | len)
/* Command composed by Data Type and parameter. */
#define DSI_DT_PRM(dt, prm) \
	(((dt) << 24) | prm)
/* At most 4 bytes of data, following DSI_GEN_LWR() or DSI_DCS_LWR(). */
#define DSI_DAT_4(d0, d1, d2, d3) \
	(((d3) << 24) | ((d2) << 16) | ((d1) << 8) | (d0))
/* SW delay. */
#define DSI_WAIT(val) \
	((DISP_DSI_DT_WAIT << 24) | (val))
/* End of command table. */
#define DSI_CMD_END() \
	0xFFFFFFFF

/**************************************************************************
 *              F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
/* disp_chnl_src.c */
UINT32 dispChnlSrcSet(UINT32 chnlId, UINT32 srcId);
UINT32 dispChnlSrcGet(UINT32 chnlId);
UINT32 dispSrcChnlGet(UINT32 srcId);

/* disp_dev.c */
UINT32 dispDevAlloc(UINT32 chnlId, UINT32 devTot);
UINT32 dispDevReg(UINT32 chnlId, UINT32 devId, dispOperatingFunc_t *popFunc);
UINT32 dispPowerOn(UINT32 chnlId, UINT32 devId, UINT32 resId, UINT32 msgEn);
UINT32 dispPowerOff(UINT32 chnlId, UINT32 msgEn);
UINT32 dispCapabilityGet(UINT32 chnlId, dispCapabDesc_t* pcapabDesc);
UINT32 dispCfgSet(UINT32 chnlId, UINT32 devId, UINT32 cfgId, ...);
UINT32 dispActChnlGet(UINT32 queId);
UINT32 dispInfGet(UINT32 chnlId, UINT32 infId, ...);

/* disp_dsi.c */
UINT32 dispDsiCmdTableSend(UINT32 *cmdTbl);
UINT32 dispDsiCmdTableSendEx(UINT32 *cmdTbl, UINT32 timeout);
UINT32 dispDsiCmdRecieve(UINT8 *cmd, UINT32 cmdLen, UINT32 attr, UINT8 *pdata);
UINT32 dispDsiRxCmdReceive(	UINT32 *cmdTbl, UINT32 timeout, UINT8 *pdata, UINT32 HSmode); /*@20161117 Elvis add . For DsiRxCmd*/

/* disp_hdmi.c */
UINT32 dispHdmiHotPlugDetectGet(UINT32 chnlId);
UINT32 dispHdmiEedidGet(UINT32 chnlId, dispHdmiEedid_t **ppedid);
UINT32 dispHdmiIsVideoFmtSupport(UINT32 chnlId, UINT32 vidFmt);

/* disp_lcm.c */
UINT32 dispLcmCmdTableSend(UINT32 *cmdTbl);
UINT32 dispLcmCmdRecieve(UINT32 cmd, UINT32 dataLen, UINT32 rdPin, UINT32 attr, UINT32 *pdata);

/* disp_para.c */
UINT32 dispParaSet(UINT32 chnlId, UINT32 paraId, SINT32 para1, SINT32 para2, SINT32 para3, SINT32 para4, UINT32 upd);
UINT32 dispParaTableSet(UINT32 chnlId, dispPara_t *paraTbl);

#endif  /* _DISP_DEF_H_ */

