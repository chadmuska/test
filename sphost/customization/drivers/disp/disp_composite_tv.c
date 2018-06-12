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
*******************************************************************************/
/**
 * @file disp_composite_tv.c
 *
 * @author Matt Wang
 */

/*******************************************************************************
*                            H E A D E R   F I L E S
*******************************************************************************/
#include "common.h"
#include "debug_mod/dbg_api.h"
#include "middleware/disp_def.h"
#include "api/sp5k_disp_api.h"

/*******************************************************************************
*                              C O N S T A N T S
*******************************************************************************/
/*
  compile option in Makefile:
    DISP_NTSC=1 : NTSC-M, NTSC-J, NTSC-4.43.
    DISP_PAL=1  : PAL, PAL-M.
    DISP_ALL=1  : NTSC-M, NTSC-J, NTSC-4.43, PAL, PAL-M.
*/

/* render dimension. */
#define DEV_PID  1

#if DISP_NTSC == 1 || DISP_ALL == 1
#define DEV_NAME_NTSC_M  "TV - NTSC-M"
#define DEV_TYPE_NTSC_M  DISP_TV_DEVICE_TYPE_NTSC_M
#define SYNCLVL_NTSC_M   16
#define BLANKLVL_NTSC_M  240
#define BLACKLVL_NTSC_M  282
#define WIDTH_NTSC_M     720
#define HEIGHT_NTSC_M    480
#define HSYNCW_NTSC_M    3
#define VSYNCW_NTSC_M    64
#define HPIXEL_NTSC_M    858
#define VLINE_NTSC_M     525
#define PNLH0_NTSC_M     121
#define PNLV0_NTSC_M     30
#define PNLH1_NTSC_M     (PNLH0_NTSC_M + WIDTH_NTSC_M - 1)
#define PNLV1_NTSC_M     (PNLV0_NTSC_M + HEIGHT_NTSC_M - 1)
#define CLK_SRC_NTSC_M   DISP_CLK_SRC_XTAL27M
#define CLK_SCL_NTSC_M   99000
#define CLK_NX_NTSC_M    27000
#define CLK_1X_NTSC_M    (CLK_NX_NTSC_M/2)

#define DEV_NAME_NTSC_J  "TV - NTSC-J"
#define DEV_TYPE_NTSC_J  DISP_TV_DEVICE_TYPE_NTSC_J
#define SYNCLVL_NTSC_J   16
#define BLANKLVL_NTSC_J  240
#define BLACKLVL_NTSC_J  240
#define WIDTH_NTSC_J     720
#define HEIGHT_NTSC_J    480
#define HSYNCW_NTSC_J    3
#define VSYNCW_NTSC_J    64
#define HPIXEL_NTSC_J    858
#define VLINE_NTSC_J     525
#define PNLH0_NTSC_J     121
#define PNLV0_NTSC_J     30
#define PNLH1_NTSC_J     (PNLH0_NTSC_J + WIDTH_NTSC_J - 1)
#define PNLV1_NTSC_J     (PNLV0_NTSC_J + HEIGHT_NTSC_J - 1)
#define CLK_SRC_NTSC_J   DISP_CLK_SRC_XTAL27M
#define CLK_SCL_NTSC_J   99000
#define CLK_NX_NTSC_J    27000
#define CLK_1X_NTSC_J    (CLK_NX_NTSC_J/2)

#define DEV_NAME_NTSC_443  "TV - NTSC-4.43"
#define DEV_TYPE_NTSC_443  DISP_TV_DEVICE_TYPE_NTSC_443
#define SYNCLVL_NTSC_443   16
#define BLANKLVL_NTSC_443  240
#define BLACKLVL_NTSC_443  282
#define WIDTH_NTSC_443     720
#define HEIGHT_NTSC_443    480
#define HSYNCW_NTSC_443    3
#define VSYNCW_NTSC_443    64
#define HPIXEL_NTSC_443    858
#define VLINE_NTSC_443     525
#define PNLH0_NTSC_443     121
#define PNLV0_NTSC_443     30
#define PNLH1_NTSC_443     (PNLH0_NTSC_443 + WIDTH_NTSC_443 - 1)
#define PNLV1_NTSC_443     (PNLV0_NTSC_443 + HEIGHT_NTSC_443 - 1)
#define CLK_SRC_NTSC_443   DISP_CLK_SRC_XTAL27M
#define CLK_SCL_NTSC_443   99000
#define CLK_NX_NTSC_443    27000
#define CLK_1X_NTSC_443    (CLK_NX_NTSC_443/2)
#if SPCA6350
#define tvfilter    0x0C
#else
#define tvfilter    0x03

#endif

#endif

#if DISP_PAL == 1 || DISP_ALL == 1
#define DEV_NAME_PAL  "TV - PAL"
#define DEV_TYPE_PAL  DISP_TV_DEVICE_TYPE_PAL
#define WIDTH_PAL     720
#define HEIGHT_PAL    576
#define HSYNCW_PAL    3
#define VSYNCW_PAL    64
#define HPIXEL_PAL    864
#define VLINE_PAL     625
#define PNLH0_PAL     126
#define PNLV0_PAL     40
#define PNLH1_PAL     (PNLH0_PAL + WIDTH_PAL - 1)
#define PNLV1_PAL     (PNLV0_PAL + HEIGHT_PAL - 1)
#define CLK_SRC_PAL   DISP_CLK_SRC_XTAL27M
#define CLK_SCL_PAL   99000
#define CLK_NX_PAL    27000
#define CLK_1X_PAL    (CLK_NX_PAL/2)
#define SYNCLVL_PAL   16
#define BLANKLVL_PAL  252
#define BLACKLVL_PAL  252

#define DEV_NAME_PAL_M  "TV - PAL-M"
#define DEV_TYPE_PAL_M  DISP_TV_DEVICE_TYPE_PAL_M
#define SYNCLVL_PAL_M   16
#define BLANKLVL_PAL_M  240
#define BLACKLVL_PAL_M  282
#define WIDTH_PAL_M     720
#define HEIGHT_PAL_M    480
#define HSYNCW_PAL_M    3
#define VSYNCW_PAL_M    64
#define HPIXEL_PAL_M    858
#define VLINE_PAL_M     525
#define PNLH0_PAL_M     121
#define PNLV0_PAL_M     30
#define PNLH1_PAL_M     (PNLH0_PAL_M + WIDTH_PAL_M - 1)
#define PNLV1_PAL_M     (PNLV0_PAL_M + HEIGHT_PAL_M - 1)
#define CLK_SRC_PAL_M   DISP_CLK_SRC_XTAL27M
#define CLK_SCL_PAL_M   99000
#define CLK_NX_PAL_M    27000
#define CLK_1X_PAL_M    (CLK_NX_PAL_M/2)
#if SPCA6350
#define tvfilter    0x0C
#else
#define tvfilter    0x03

#endif

#endif

/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*******************************************************************************/
/* device open. */
#if DISP_NTSC == 1
#define dispOpen             dispCompositeTvNtscOpen
#define dispOpDevNameGet     dispCompositeTvNtscOpDevNameGet
#define dispOpPowerOn        dispCompositeTvNtscOpPowerOn
#define dispOpPowerOff       dispCompositeTvNtscOpPowerOff
#define dispOpCapabilityGet  dispCompositeTvNtscOpCapabilityGet
#define dispOpResolutionSet  dispCompositeTvNtscOpResolutionSet
#endif

#if DISP_PAL == 1
#define dispOpen             dispCompositeTvPalOpen
#define dispOpDevNameGet     dispCompositeTvPalOpDevNameGet
#define dispOpPowerOn        dispCompositeTvPalOpPowerOn
#define dispOpPowerOff       dispCompositeTvPalOpPowerOff
#define dispOpCapabilityGet  dispCompositeTvPalOpCapabilityGet
#define dispOpResolutionSet  dispCompositeTvPalOpResolutionSet
#endif

#if DISP_ALL == 1
#define dispOpen             dispCompositeTvAllOpen
#define dispOpDevNameGet     dispCompositeTvAllOpDevNameGet
#define dispOpPowerOn        dispCompositeTvAllOpPowerOn
#define dispOpPowerOff       dispCompositeTvAllOpPowerOff
#define dispOpCapabilityGet  dispCompositeTvAllOpCapabilityGet
#define dispOpResolutionSet  dispCompositeTvAllOpResolutionSet
#endif

/* operating function. */
static UINT8 *dispOpDevNameGet(void);
static UINT32 dispOpPowerOn(void);
static UINT32 dispOpPowerOff(void);
static void   dispOpCapabilityGet(dispCapabDesc_t *pcapabDesc, UINT32 descSize);
static UINT32 dispOpResolutionSet(UINT32 resId);

/*******************************************************************************
*                            G L O B A L   D A T A
*******************************************************************************/
/* resolution id. */
static UINT32 dispResId;

/* device name. */
#if DISP_NTSC == 1
static UINT8 *pdispDevName[] = {
	[SP5K_DISP_TV_NTSC_M  ] = (UINT8 *)DEV_NAME_NTSC_M,
	[SP5K_DISP_TV_NTSC_J  ] = (UINT8 *)DEV_NAME_NTSC_J,
	[SP5K_DISP_TV_NTSC_443] = (UINT8 *)DEV_NAME_NTSC_443,
};
#endif

#if DISP_PAL == 1
static UINT8 *pdispDevName[] = {
	[SP5K_DISP_TV_PAL  ] = (UINT8 *)DEV_NAME_PAL,
	[SP5K_DISP_TV_PAL_M] = (UINT8 *)DEV_NAME_PAL_M,
};
#endif

#if DISP_ALL == 1
static UINT8 *pdispDevName[] = {
	[SP5K_DISP_TV_NTSC_M  ] = (UINT8 *)DEV_NAME_NTSC_M,
	[SP5K_DISP_TV_NTSC_J  ] = (UINT8 *)DEV_NAME_NTSC_J,
	[SP5K_DISP_TV_NTSC_443] = (UINT8 *)DEV_NAME_NTSC_443,
	[SP5K_DISP_TV_PAL     ] = (UINT8 *)DEV_NAME_PAL,
	[SP5K_DISP_TV_PAL_M   ] = (UINT8 *)DEV_NAME_PAL_M,
};
#endif

/* device capability. */
#if DISP_NTSC == 1
static dispCapabDesc_t dispCapabDesc[] = {
	[SP5K_DISP_TV_NTSC_M] = {
		.width        = WIDTH_NTSC_M,
		.height       = HEIGHT_NTSC_M,
		.xoff         = PNLH0_NTSC_M,
		.yoff         = PNLV0_NTSC_M,
		.aspectRatioX = 4,
		.aspectRatioY = 3,
		.logiEn       = 0,
		.frameRate    = 60000,
	},
	[SP5K_DISP_TV_NTSC_J] = {
		.width        = WIDTH_NTSC_J,
		.height       = HEIGHT_NTSC_J,
		.xoff         = PNLH0_NTSC_J,
		.yoff         = PNLV0_NTSC_J,
		.aspectRatioX = 4,
		.aspectRatioY = 3,
		.logiEn       = 0,
		.frameRate    = 60000,
	},
	[SP5K_DISP_TV_NTSC_443] = {
		.width        = WIDTH_NTSC_443,
		.height       = HEIGHT_NTSC_443,
		.xoff         = PNLH0_NTSC_443,
		.yoff         = PNLV0_NTSC_443,
		.aspectRatioX = 4,
		.aspectRatioY = 3,
		.logiEn       = 0,
		.frameRate    = 60000,
	},
};
#endif

#if DISP_PAL == 1
static dispCapabDesc_t dispCapabDesc[] = {
	[SP5K_DISP_TV_PAL] = {
		.width        = WIDTH_PAL,
		.height       = HEIGHT_PAL,
		.xoff         = PNLH0_PAL,
		.yoff         = PNLV0_PAL,
		.aspectRatioX = 4,
		.aspectRatioY = 3,
		.logiEn       = 0,
		.frameRate    = 50000,
	},
	[SP5K_DISP_TV_PAL_M] = {
		.width        = WIDTH_PAL_M,
		.height       = HEIGHT_PAL_M,
		.xoff         = PNLH0_PAL_M,
		.yoff         = PNLV0_PAL_M,
		.aspectRatioX = 4,
		.aspectRatioY = 3,
		.logiEn       = 0,
		.frameRate    = 60000,
	},
};
#endif

#if DISP_ALL == 1
static dispCapabDesc_t dispCapabDesc[] = {
	[SP5K_DISP_TV_NTSC_M] = {
		.width        = WIDTH_NTSC_M,
		.height       = HEIGHT_NTSC_M,
		.xoff         = PNLH0_NTSC_M,
		.yoff         = PNLV0_NTSC_M,
		.aspectRatioX = 4,
		.aspectRatioY = 3,
		.logiEn       = 0,
		.frameRate    = 60000,
	},
	[SP5K_DISP_TV_NTSC_J] = {
		.width        = WIDTH_NTSC_J,
		.height       = HEIGHT_NTSC_J,
		.xoff         = PNLH0_NTSC_J,
		.yoff         = PNLV0_NTSC_J,
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		.aspectRatioX = 16,
		.aspectRatioY = 9,
#else
		.aspectRatioX = 4,
		.aspectRatioY = 3,
#endif
		.logiEn       = 0,
		.frameRate    = 60000,
	},
	[SP5K_DISP_TV_NTSC_443] = {
		.width        = WIDTH_NTSC_443,
		.height       = HEIGHT_NTSC_443,
		.xoff         = PNLH0_NTSC_443,
		.yoff         = PNLV0_NTSC_443,
		.aspectRatioX = 4,
		.aspectRatioY = 3,
		.logiEn       = 0,
		.frameRate    = 60000,
	},
	[SP5K_DISP_TV_PAL] = {
		.width        = WIDTH_PAL,
		.height       = HEIGHT_PAL,
		.xoff         = PNLH0_PAL,
		.yoff         = PNLV0_PAL,
		.aspectRatioX = 4,
		.aspectRatioY = 3,
		.logiEn       = 0,
		.frameRate    = 50000,
	},
	[SP5K_DISP_TV_PAL_M] = {
		.width        = WIDTH_PAL_M,
		.height       = HEIGHT_PAL_M,
		.xoff         = PNLH0_PAL_M,
		.yoff         = PNLV0_PAL_M,
		.aspectRatioX = 4,
		.aspectRatioY = 3,
		.logiEn       = 0,
		.frameRate    = 60000,
	},
};
#endif  /* END #if DISP_ALL */

/* parameter table. */
#if DISP_NTSC == 1 || DISP_ALL == 1
static dispPara_t dispTvNtscMPwrOnParaTbl[] = {
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, CLK_SRC_NTSC_M, DISP_TV_TYPE_ANALOG, 0, 0, 0},
	{DISP_PARA_CLK_FREQ, CLK_1X_NTSC_M, CLK_NX_NTSC_M, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_NTSC_M, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_NTSC_M, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* sync, blank, black level */
	{DISP_PARA_TV_SYNC_LEVEL, SYNCLVL_NTSC_M, 0, 0, 0},
	{DISP_PARA_TV_BLANK_LEVEL, BLANKLVL_NTSC_M, 0, 0, 0},
	{DISP_PARA_TV_BLACK_LEVEL, BLACKLVL_NTSC_M, 0, 0, 0},

	/* set vh timing and format */
	{DISP_PARA_DEVICE_TOTAL_REGION, HPIXEL_NTSC_M, VLINE_NTSC_M, HSYNCW_NTSC_M, VSYNCW_NTSC_M, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, PNLH0_NTSC_M, PNLV0_NTSC_M, PNLH1_NTSC_M, PNLV1_NTSC_M, 0},
	{DISP_PARA_INTERLACE_MODE, DISP_INTERLACE_NOSOF, 0, 0, 0, 0},
	{DISP_PARA_IMG_NONSCALE_INTERLACE_MODE, DISP_NONSCALE_INTRLC_NOSWAP, 0, 0, 0, 0},
	{DISP_PARA_TV_ENCODER_FILTER, 0, tvfilter, 0, 0, 0},
	/*{DISP_PARA_CCIR_CLAMP_MODE, DISP_CCIR_CLAMP_DISABLE, 0, 0, 0, 0},*/
	{DISP_PARA_VIDEO_DAC_POWER, 1, 1, 0, 0, 0},
	{DISP_PARA_GLOB_ENABLE, 1, 0, 0, 0, 2},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t dispTvNtscJPwrOnParaTbl[] = {
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, CLK_SRC_NTSC_J, DISP_TV_TYPE_ANALOG, 0, 0, 0},
	{DISP_PARA_CLK_FREQ, CLK_1X_NTSC_J, CLK_NX_NTSC_J, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_NTSC_J, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_NTSC_J, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* sync, blank, black level */
	{DISP_PARA_TV_SYNC_LEVEL, SYNCLVL_NTSC_J, 0, 0, 0},
	{DISP_PARA_TV_BLANK_LEVEL, BLANKLVL_NTSC_J, 0, 0, 0},
	{DISP_PARA_TV_BLACK_LEVEL, BLACKLVL_NTSC_J, 0, 0, 0},

	/* set vh timing and format */
	{DISP_PARA_DEVICE_TOTAL_REGION, HPIXEL_NTSC_J, VLINE_NTSC_J, HSYNCW_NTSC_J, VSYNCW_NTSC_J, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, PNLH0_NTSC_J, PNLV0_NTSC_J, PNLH1_NTSC_J, PNLV1_NTSC_J, 0},
	{DISP_PARA_INTERLACE_MODE, DISP_INTERLACE_NOSOF, 0, 0, 0, 0},
	{DISP_PARA_IMG_NONSCALE_INTERLACE_MODE, DISP_NONSCALE_INTRLC_NOSWAP, 0, 0, 0, 0},
	{DISP_PARA_TV_ENCODER_FILTER, 0, tvfilter, 0, 0, 0},
	/*{DISP_PARA_CCIR_CLAMP_MODE, DISP_CCIR_CLAMP_DISABLE, 0, 0, 0, 0},*/
	{DISP_PARA_VIDEO_DAC_POWER, 1, 1, 0, 0, 0},
	{DISP_PARA_GLOB_ENABLE, 1, 0, 0, 0, 2},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t dispTvNtsc443PwrOnParaTbl[] = {
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, CLK_SRC_NTSC_443, DISP_TV_TYPE_ANALOG, 0, 0, 0},
	{DISP_PARA_CLK_FREQ, CLK_1X_NTSC_443, CLK_NX_NTSC_443, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_NTSC_443, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_NTSC_443, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* sync, blank, black level */
	{DISP_PARA_TV_SYNC_LEVEL, SYNCLVL_NTSC_443, 0, 0, 0},
	{DISP_PARA_TV_BLANK_LEVEL, BLANKLVL_NTSC_443, 0, 0, 0},
	{DISP_PARA_TV_BLACK_LEVEL, BLACKLVL_NTSC_443, 0, 0, 0},

	/* set vh timing and format */
	{DISP_PARA_DEVICE_TOTAL_REGION, HPIXEL_NTSC_443, VLINE_NTSC_443, HSYNCW_NTSC_443, VSYNCW_NTSC_443, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, PNLH0_NTSC_443, PNLV0_NTSC_443, PNLH1_NTSC_443, PNLV1_NTSC_443, 0},
	{DISP_PARA_INTERLACE_MODE, DISP_INTERLACE_NOSOF, 0, 0, 0, 0},
	{DISP_PARA_IMG_NONSCALE_INTERLACE_MODE, DISP_NONSCALE_INTRLC_NOSWAP, 0, 0, 0, 0},
	{DISP_PARA_TV_ENCODER_FILTER, 0, tvfilter, 0, 0, 0},
	/*{DISP_PARA_CCIR_CLAMP_MODE, DISP_CCIR_CLAMP_DISABLE, 0, 0, 0, 0},*/
	{DISP_PARA_VIDEO_DAC_POWER, 1, 1, 0, 0, 0},
	{DISP_PARA_GLOB_ENABLE, 1, 0, 0, 0, 2},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};
#endif

#if DISP_PAL == 1 || DISP_ALL == 1
static dispPara_t dispTvPalPwrOnParaTbl[] = {
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, CLK_SRC_PAL, DISP_TV_TYPE_ANALOG, 0, 0, 0},
	{DISP_PARA_CLK_FREQ, CLK_1X_PAL, CLK_NX_PAL, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_PAL, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_PAL, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* sync, blank, black level */
	{DISP_PARA_TV_SYNC_LEVEL, SYNCLVL_PAL, 0, 0, 0},
	{DISP_PARA_TV_BLANK_LEVEL, BLANKLVL_PAL, 0, 0, 0},
	{DISP_PARA_TV_BLACK_LEVEL, BLACKLVL_PAL, 0, 0, 0},

	/* set vh timing and format */
	{DISP_PARA_DEVICE_TOTAL_REGION, HPIXEL_PAL, VLINE_PAL, HSYNCW_PAL, VSYNCW_PAL, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, PNLH0_PAL, PNLV0_PAL, PNLH1_PAL, PNLV1_PAL, 0},
	{DISP_PARA_INTERLACE_MODE, DISP_INTERLACE_NOSOF, 0, 0, 0, 0},
	{DISP_PARA_IMG_NONSCALE_INTERLACE_MODE, DISP_NONSCALE_INTRLC_NOSWAP, 0, 0, 0, 0},
	{DISP_PARA_TV_ENCODER_FILTER, 0, tvfilter, 0, 0, 0},
	/*{DISP_PARA_CCIR_CLAMP_MODE, DISP_CCIR_CLAMP_DISABLE, 0, 0, 0, 0},*/
	{DISP_PARA_VIDEO_DAC_POWER, 1, 1, 0, 0, 0},
	{DISP_PARA_GLOB_ENABLE, 1, 0, 0, 0, 2},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t dispTvPalMPwrOnParaTbl[] = {
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, CLK_SRC_PAL_M, DISP_TV_TYPE_ANALOG, 0, 0, 0},
	{DISP_PARA_CLK_FREQ, CLK_1X_PAL_M, CLK_NX_PAL_M, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_PAL_M, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_PAL_M, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* sync, blank, black level */
	{DISP_PARA_TV_SYNC_LEVEL, SYNCLVL_PAL_M, 0, 0, 0},
	{DISP_PARA_TV_BLANK_LEVEL, BLANKLVL_PAL_M, 0, 0, 0},
	{DISP_PARA_TV_BLACK_LEVEL, BLACKLVL_PAL_M, 0, 0, 0},

	/* set vh timing and format */
	{DISP_PARA_DEVICE_TOTAL_REGION, HPIXEL_PAL_M, VLINE_PAL_M, HSYNCW_PAL_M, VSYNCW_PAL_M, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, PNLH0_PAL_M, PNLV0_PAL_M, PNLH1_PAL_M, PNLV1_PAL_M, 0},
	{DISP_PARA_INTERLACE_MODE, DISP_INTERLACE_NOSOF, 0, 0, 0, 0},
	{DISP_PARA_IMG_NONSCALE_INTERLACE_MODE, DISP_NONSCALE_INTRLC_NOSWAP, 0, 0, 0, 0},
	{DISP_PARA_TV_ENCODER_FILTER, 0, tvfilter, 0, 0, 0},
	/*{DISP_PARA_CCIR_CLAMP_MODE, DISP_CCIR_CLAMP_DISABLE, 0, 0, 0, 0},*/
	{DISP_PARA_VIDEO_DAC_POWER, 1, 1, 0, 0, 0},
	{DISP_PARA_GLOB_ENABLE, 1, 0, 0, 0, 2},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};
#endif

#if DISP_NTSC == 1
static dispPara_t *pdispPwrOnParaTbl[] = {
	[SP5K_DISP_TV_NTSC_M  ] = dispTvNtscMPwrOnParaTbl,
	[SP5K_DISP_TV_NTSC_J  ] = dispTvNtscJPwrOnParaTbl,
	[SP5K_DISP_TV_NTSC_443] = dispTvNtsc443PwrOnParaTbl,
};
#endif

#if DISP_PAL == 1
static dispPara_t *pdispPwrOnParaTbl[] = {
	[SP5K_DISP_TV_PAL  ] = dispTvPalPwrOnParaTbl,
	[SP5K_DISP_TV_PAL_M] = dispTvPalMPwrOnParaTbl,
};
#endif

#if DISP_ALL == 1
static dispPara_t *pdispPwrOnParaTbl[] = {
	[SP5K_DISP_TV_NTSC_M  ] = dispTvNtscMPwrOnParaTbl,
	[SP5K_DISP_TV_NTSC_J  ] = dispTvNtscJPwrOnParaTbl,
	[SP5K_DISP_TV_NTSC_443] = dispTvNtsc443PwrOnParaTbl,
	[SP5K_DISP_TV_PAL     ] = dispTvPalPwrOnParaTbl,
	[SP5K_DISP_TV_PAL_M   ] = dispTvPalMPwrOnParaTbl,
};
#endif

static dispPara_t dispPwrOffParaTbl[] = {
	{DISP_PARA_IMG_NONSCALE_INTERLACE_MODE, DISP_NONSCALE_INTRLC_DISABLE, 0, 0, 0, 0},
	{DISP_PARA_VIDEO_DAC_POWER, 0, 0, 0, 0, 1},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

/**
 * @brief Open (register) a device driver.
 *
 * @param None.
 *
 * @return Device handle.
 */
UINT32
dispOpen(
	void
)
{
	UINT32 ret;
	dispOperatingFunc_t dispOpFunc;


	DBG_FUNC_ENTER(NULL);

	dispOpFunc.pfOpDevNameGet    = dispOpDevNameGet;
	dispOpFunc.pfOpPowerOn       = dispOpPowerOn;
	dispOpFunc.pfOpPowerOff      = dispOpPowerOff;
	dispOpFunc.pfOpCapabilityGet = dispOpCapabilityGet;
	dispOpFunc.pfOpResolutionSet = dispOpResolutionSet;
	ret = sp5kDispDevReg(SP5K_DISP_CHNL_1, SP5K_DISP_CHNL_1_COMPOSITE_TV, &dispOpFunc);

	DBG_FUNC_RETURN(ret, NULL);
}

/**
 * @brief Get device name string in current resolution of the device.
 *
 * @param None.
 *
 * @return Device name string.
 */
static UINT8 *
dispOpDevNameGet(
	void
)
{
	DBG_FUNC_ENTER(NULL);

	DBG_FUNC_RETURN(pdispDevName[dispResId], "%s", pdispDevName[dispResId]);
}

/**
 * @brief Power on the device.
 *
 * @param None.
 *
 * @return SUCCESS or FAIL.
 */
static UINT32
dispOpPowerOn(
	void
)
{
	DBG_FUNC_ENTER(NULL);

	dispParaTableSet(DEV_PID, pdispPwrOnParaTbl[dispResId]);

	DBG_FUNC_RETURN(SUCCESS, NULL);
}

/**
 * @brief Power off the device.
 *
 * @param None.
 *
 * @return SUCCESS or FAIL.
 */
static UINT32
dispOpPowerOff(
	void
)
{
	DBG_FUNC_ENTER(NULL);

	dispParaTableSet(DEV_PID, dispPwrOffParaTbl);

	DBG_FUNC_RETURN(SUCCESS, NULL);
}

/**
 * @brief Get capability in current resolution of the device.
 *
 * @param [out] *pcapabDesc Capability descriptor.
 * @param [in]  descSize    Data size of the descriptor.
 *
 * @return None.
 */
static void
dispOpCapabilityGet(
	dispCapabDesc_t *pcapabDesc,
	UINT32 descSize
)
{
	*pcapabDesc = dispCapabDesc[dispResId];
}

/**
 * @brief Set resolution for further operating.
 *
 * @param [in] resId Resolution ID, \n
 *                   One of _sp5kDispCompositeTvId_e.
 *
 * @return SUCCESS or FAIL.
 */
static UINT32
dispOpResolutionSet(
	UINT32 resId
)
{
	DBG_FUNC_ENTER("%d", resId);

	dispResId = resId;

	DBG_FUNC_RETURN(SUCCESS, "OK");
}

