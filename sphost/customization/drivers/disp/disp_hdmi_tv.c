/**************************************************************************
 *
 *       Copyright (c) 2011-2018 by iCatch Technology, Inc.
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
/**
 * @file disp_hdmi_tv.c
 *
 * @author Matt Wang
 */
/*
  compile option in Makefile:
    DISP_ALL=1 : [2D]
                 VIC=1,  640X480P_60HZ_4T3 (pixel clk not correct),
                 VIC=2,  720X480P_60HZ_4T3,
                 VIC=3,  720X480P_60HZ_16T9,
                 VIC=4,  1280X720P_60HZ_16T9,
                 VIC=5,  1920X1080I_60HZ_16T9,
                 VIC=6,  720X480I_60HZ_4T3,
                 VIC=7,  720X480I_60HZ_16T9,
                 VIC=8,  720X240P_60HZ_4T3,
                 VIC=9,  720X240P_60HZ_16T9,
                 VIC=10, 2880X480I_60HZ_4T3 (pixel clk not correct),
                 VIC=11, 2880X480I_60HZ_16T9 (pixel clk not correct),
                 VIC=12, 2880X240P_60HZ_4T3 (pixel clk not correct),
                 VIC=13, 2880X240P_60HZ_16T9 (pixel clk not correct),
                 VIC=14, 1440X480P_60HZ_4T3 (pixel clk not correct),
                 VIC=15, 1440X480P_60HZ_16T9 (pixel clk not correct),
                 VIC=16, 1920X1080P_60HZ_16T9,
                 VIC=17, 720X576P_50HZ_4T3,
                 VIC=18, 720X576P_50HZ_16T9,
                 VIC=19, 1280X720P_50HZ_16T9,
                 VIC=20, 1920X1080I_50HZ_16T9,
                 VIC=21, 720X576I_50HZ_4T3,
                 VIC=22, 720X576I_50HZ_16T9,
                 VIC=23, 720X288P_50HZ_4T3,
                 VIC=24, 720X288P_50HZ_16T9,
                 VIC=25, 2880X576I_50HZ_4T3 (pixel clk not correct),
                 VIC=26, 2880X576I_50HZ_16T9 (pixel clk not correct),
                 VIC=27, 2880X288P_50HZ_4T3 (pixel clk not correct),
                 VIC=28, 2880X288P_50HZ_16T9 (pixel clk not correct),
                 VIC=29, 1440X576P_50HZ_4T3 (pixel clk not correct),
                 VIC=30, 1440X576P_50HZ_16T9 (pixel clk not correct),
                 VIC=31, 1920X1080P_50HZ_16T9,
                 VIC=32, 1920X1080P_24HZ_16T9,
                 VIC=33, 1920X1080P_25HZ_16T9,
                 VIC=34, 1920X1080P_30HZ_16T9,
                 VIC=35, 2880X480P_60HZ_4T3 (pixel clk not correct),
                 VIC=36, 2880X480P_60HZ_16T9 (pixel clk not correct),
                 VIC=37, 2880X576P_50HZ_4T3 (pixel clk not correct),
                 VIC=38, 2880X576P_50HZ_16T9 (pixel clk not correct),
                 VIC=40, 1920X1080I_100HZ_16T9,
                 VIC=41, 1280X720P_100HZ_16T9,
                 VIC=42, 720X576P_100HZ_4T3 (pixel clk not correct),
                 VIC=43, 720X576P_100HZ_16T9 (pixel clk not correct),
                 VIC=44, 720X576I_100HZ_4T3 (pixel clk not correct),
                 VIC=45, 720X576I_100HZ_16T9 (pixel clk not correct),
                 VIC=46, 1920X1080I_120HZ_16T9,
                 VIC=47, 1280X720P_120HZ_16T9,
                 VIC=48, 720X480P_120HZ_4T3 (pixel clk not correct),
                 VIC=49, 720X480P_120HZ_16T9 (pixel clk not correct),
                 VIC=50, 720X480I_120HZ_4T3 (pixel clk not correct),
                 VIC=51, 720X480I_120HZ_16T9 (pixel clk not correct),
                 VIC=52, 720X576P_200HZ_4T3 (pixel clk not correct),
                 VIC=53, 720X576P_200HZ_16T9 (pixel clk not correct),
                 VIC=54, 720X576I_200HZ_4T3 (pixel clk not correct),
                 VIC=55, 720X576I_200HZ_16T9 (pixel clk not correct),
                 VIC=56, 720X480P_240HZ_4T3 (pixel clk not correct),
                 VIC=57, 720X480P_240HZ_16T9 (pixel clk not correct),
                 VIC=58, 720X480I_240HZ_4T3 (pixel clk not correct),
                 VIC=59, 720X480I_240HZ_16T9 (pixel clk not correct),
                 VIC=60, 1280X720P_24HZ_16T9,
                 VIC=61, 1280X720P_25HZ_16T9,
                 VIC=62, 1280X720P_30HZ_16T9,
                 VIC=63, 1920X1080P_120HZ_16T9 (pixel clk not correct),
                 VIC=64, 1920X1080P_100HZ_16T9 (pixel clk not correct),
                 [3D Frame Packing]
                 VIC=4,  1280X720P_60HZ_16T9_3DFP,
                 VIC=19, 1280X720P_50HZ_16T9_3DFP,
                 VIC=32, 1920X1080P_24HZ_16T9_3DFP,
                 VIC=33, 1920X1080P_25HZ_16T9_3DFP (pixel clk not correct),
                 VIC=34, 1920X1080P_30HZ_16T9_3DFP (pixel clk not correct),
                 [3D Top-and-Bottom]
                 VIC=4,  1280X720P_60HZ_16T9_3DTAB,
                 VIC=16, 1920X1080P_60HZ_16T9_3DTAB,
                 VIC=19, 1280X720P_50HZ_16T9_3DTAB,
                 VIC=31, 1920X1080P_50HZ_16T9_3DTAB,
                 VIC=32, 1920X1080P_24HZ_16T9_3DTAB,
                 [3D Side-by-Side (Half) with horizontal sub-sampling]
                 VIC=4,  1280X720P_60HZ_16T9_3DSBSHHSS,
                 VIC=5,  1920X1080I_60HZ_16T9_3DSBSHHSS,
                 VIC=19, 1280X720P_50HZ_16T9_3DSBSHHSS,
                 VIC=20, 1920X1080I_50HZ_16T9_3DSBSHHSS,
                 VIC=32, 1920X1080P_24HZ_16T9_3DSBSHHSS.
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include <stdarg.h>  /* va_arg(). */
#include <string.h>  /* memcpy(), strcat(), strcpy(). */
#include <stdio.h>   /* sprintf(). */
#include "common.h"
#include "debug_mod/dbg_api.h"
#include "api/sp5k_cec_api.h"
#include "api/sp5k_disp_api.h"
#include "customization/app_init.h"


/**************************************************************************
 *                          C O N S T A N T S
 **************************************************************************/
/* render dimension. */
#define DEV_PID  1

/* audio sample rate. */

#define AUD_SAMPLE_RATE  AUDIO_SAMPLE_RATE


/* 640x480p @59.94/60Hz 4:3 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT1    "640X480P 60HZ 4:3"
#define DEV_TYPE_FMT1    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT1       DISP_PROGRESSIVE
#define FIELD_FMT1       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT1       640
#define HEIGHT_FMT1      480
#define HSYNCOS_FMT1     (16-1)
#define VSYNCOS_FMT1     (10-1)
#define HSYNCW_FMT1      (HSYNCOS_FMT1 + 96)
#define VSYNCW_FMT1      (VSYNCOS_FMT1 + 2)
#define TVH0_FMT1        (HSYNCW_FMT1 + 48)
#define TVV0_FMT1        (VSYNCW_FMT1 + 33)
#define TVH1_FMT1        (TVH0_FMT1 + WIDTH_FMT1 - 1)
#define TVV1_FMT1        (TVV0_FMT1 + HEIGHT_FMT1 - 1)
#define HPIXEL_FMT1      (TVH1_FMT1 + 2)
#define VLINE_FMT1       (TVV1_FMT1 + 2)
#ifndef FPGA
#define CLK_NX_FMT1      CLK_1X_FMT1
#define CLK_1X_FMT1      25200
#define CLK_SCL_FMT1     200000
#define CLK_PIXEL_FMT1   CLK_1X_FMT1
#else
#define CLK_NX_DIV_FMT1  1
#define CLK_1X_DIV_FMT1  1
#endif

/* 720x480p @59.94/60Hz 4:3 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT2    "720X480P 60HZ 4:3"
#define DEV_TYPE_FMT2    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT2       DISP_PROGRESSIVE
#define FIELD_FMT2       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT2       720
#define HEIGHT_FMT2      480
#define HSYNCOS_FMT2     (16-1)
#define VSYNCOS_FMT2     (9-1)
#define HSYNCW_FMT2      (HSYNCOS_FMT2 + 62)
#define VSYNCW_FMT2      (VSYNCOS_FMT2 + 6)
#define TVH0_FMT2        (HSYNCW_FMT2 + 60)
#define TVV0_FMT2        (VSYNCW_FMT2 + 30)
#define TVH1_FMT2        (TVH0_FMT2 + WIDTH_FMT2 - 1)
#define TVV1_FMT2        (TVV0_FMT2 + HEIGHT_FMT2 - 1)
#define HPIXEL_FMT2      (TVH1_FMT2 + 2)
#define VLINE_FMT2       (TVV1_FMT2 + 2)
#ifndef FPGA
#define CLK_NX_FMT2      CLK_1X_FMT2
#define CLK_1X_FMT2      27027
#define CLK_SCL_FMT2     200000
#define CLK_PIXEL_FMT2   CLK_1X_FMT2
#else
#define CLK_NX_DIV_FMT2  1
#define CLK_1X_DIV_FMT2  1
#endif

/* 720x480p @59.94/60Hz 16:9 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT3    "720X480P 60HZ 16:9"
#define DEV_TYPE_FMT3    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT3       DISP_PROGRESSIVE
#define FIELD_FMT3       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT3       720
#define HEIGHT_FMT3      480
#define HSYNCOS_FMT3     (16-1)
#define VSYNCOS_FMT3     (9-1)
#define HSYNCW_FMT3      (HSYNCOS_FMT3 + 62)
#define VSYNCW_FMT3      (VSYNCOS_FMT3 + 6)
#define TVH0_FMT3        (HSYNCW_FMT3 + 60)
#define TVV0_FMT3        (VSYNCW_FMT3 + 30)
#define TVH1_FMT3        (TVH0_FMT3 + WIDTH_FMT3 - 1)
#define TVV1_FMT3        (TVV0_FMT3 + HEIGHT_FMT3 - 1)
#define HPIXEL_FMT3      (TVH1_FMT3 + 2)
#define VLINE_FMT3       (TVV1_FMT3 + 2)
#ifndef FPGA
#define CLK_NX_FMT3      CLK_1X_FMT3
#define CLK_1X_FMT3      27027
#define CLK_SCL_FMT3     200000
#define CLK_PIXEL_FMT3   CLK_1X_FMT3
#else
#define CLK_NX_DIV_FMT3  1
#define CLK_1X_DIV_FMT3  1
#endif

/* 1280x720p @59.94/60Hz 16:9 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT4    "1280X720P 60HZ 16:9"
#define DEV_TYPE_FMT4    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT4       DISP_PROGRESSIVE
#define FIELD_FMT4       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT4       1280
#define HEIGHT_FMT4      720
#define HSYNCOS_FMT4     (110-1)
#define VSYNCOS_FMT4     (5-1)
#define HSYNCW_FMT4      (HSYNCOS_FMT4 + 40)
#define VSYNCW_FMT4      (VSYNCOS_FMT4 + 5)
#define TVH0_FMT4        (HSYNCW_FMT4 + 220)
#define TVV0_FMT4        (VSYNCW_FMT4 + 20)
#define TVH1_FMT4        (TVH0_FMT4 + WIDTH_FMT4 - 1)
#define TVV1_FMT4        (TVV0_FMT4 + HEIGHT_FMT4 - 1)
#define HPIXEL_FMT4      (TVH1_FMT4 + 2)
#define VLINE_FMT4       (TVV1_FMT4 + 2)
#ifndef FPGA
#define CLK_NX_FMT4      CLK_1X_FMT4
#define CLK_1X_FMT4      74250
#define CLK_SCL_FMT4     148500
#define CLK_PIXEL_FMT4   CLK_1X_FMT4
#else
#define CLK_NX_DIV_FMT4  4
#define CLK_1X_DIV_FMT4  1
#endif

/* 1920x1080i @59.94/60Hz 16:9 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT5    "1920X1080I 60HZ 16:9"
#define DEV_TYPE_FMT5    DISP_PANEL_DEVICE_TYPE_STD_INTERLACE_CCIR601
#define FRAME_FMT5       DISP_INTERLACE_NOSOF
#define FIELD_FMT5       DISP_SRC_FIELD_SWAP
#define WIDTH_FMT5       1920
#define HEIGHT_FMT5      (540+540)
#define HSYNCOS_FMT5     (88-1)
#define VSYNCOS_FMT5     ((2-1)+(2-1))
#define HSYNCW_FMT5      (HSYNCOS_FMT5 + 44)
#define VSYNCW_FMT5      (VSYNCOS_FMT5 + 5 + 5)
#define TVH0_FMT5        (HSYNCW_FMT5 + 148)
#define TVV0_FMT5        (VSYNCW_FMT5 + 15 + 16)
#define TVH1_FMT5        (TVH0_FMT5 + WIDTH_FMT5 - 1)
#define TVV1_FMT5        (TVV0_FMT5 + HEIGHT_FMT5 - 1)
#define HPIXEL_FMT5      (TVH1_FMT5 + 2)
#define VLINE_FMT5       (TVV1_FMT5 + 3)
#ifndef FPGA
#define CLK_NX_FMT5      CLK_1X_FMT5
#define CLK_1X_FMT5      74250
#define CLK_SCL_FMT5     148500
#define CLK_PIXEL_FMT5   CLK_1X_FMT5
#else
#define CLK_NX_DIV_FMT5  4
#define CLK_1X_DIV_FMT5  1
#endif

/* 720(1440)x480i @59.94/60Hz 4:3 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT6    "720X480I 60HZ 4:3"
#define DEV_TYPE_FMT6    DISP_PANEL_DEVICE_TYPE_STD_INTERLACE_CCIR601
#define FRAME_FMT6       DISP_INTERLACE_NOSOF
#define FIELD_FMT6       DISP_SRC_FIELD_SWAP
#define WIDTH_FMT6       1440
#define HEIGHT_FMT6      (240+240)
#define HSYNCOS_FMT6     (38-1)
#define VSYNCOS_FMT6     ((4-1)+(4-1))
#define HSYNCW_FMT6      (HSYNCOS_FMT6 + 124)
#define VSYNCW_FMT6      (VSYNCOS_FMT6 + 3 + 3)
#define TVH0_FMT6        (HSYNCW_FMT6 + 114)
#define TVV0_FMT6        (VSYNCW_FMT6 + 15 + 16)
#define TVH1_FMT6        (TVH0_FMT6 + WIDTH_FMT6 - 1)
#define TVV1_FMT6        (TVV0_FMT6 + HEIGHT_FMT6 - 1)
#define HPIXEL_FMT6      (TVH1_FMT6 + 2)
#define VLINE_FMT6       (TVV1_FMT6 + 3)
#ifndef FPGA
#define CLK_NX_FMT6      CLK_1X_FMT6
#define CLK_1X_FMT6      27027
#define CLK_SCL_FMT6     200000
#define CLK_PIXEL_FMT6   CLK_1X_FMT6
#else
#define CLK_NX_DIV_FMT6  1
#define CLK_1X_DIV_FMT6  1
#endif

/* 720(1440)x480i @59.94/60Hz 16:9 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT7    "720X480I 60HZ 16:9"
#define DEV_TYPE_FMT7    DISP_PANEL_DEVICE_TYPE_STD_INTERLACE_CCIR601
#define FRAME_FMT7       DISP_INTERLACE_NOSOF
#define FIELD_FMT7       DISP_SRC_FIELD_SWAP
#define WIDTH_FMT7       1440
#define HEIGHT_FMT7      (240+240)
#define HSYNCOS_FMT7     (38-1)
#define VSYNCOS_FMT7     ((4-1)+(4-1))
#define HSYNCW_FMT7      (HSYNCOS_FMT7 + 124)
#define VSYNCW_FMT7      (VSYNCOS_FMT7 + 3 + 3)
#define TVH0_FMT7        (HSYNCW_FMT7 + 114)
#define TVV0_FMT7        (VSYNCW_FMT7 + 15 + 16)
#define TVH1_FMT7        (TVH0_FMT7 + WIDTH_FMT7 - 1)
#define TVV1_FMT7        (TVV0_FMT7 + HEIGHT_FMT7 - 1)
#define HPIXEL_FMT7      (TVH1_FMT7 + 2)
#define VLINE_FMT7       (TVV1_FMT7 + 3)
#ifndef FPGA
#define CLK_NX_FMT7      CLK_1X_FMT7
#define CLK_1X_FMT7      27027
#define CLK_SCL_FMT7     200000
#define CLK_PIXEL_FMT7   CLK_1X_FMT7
#else
#define CLK_NX_DIV_FMT7  1
#define CLK_1X_DIV_FMT7  1
#endif

/* 720(1440)x240p @59.94/60Hz 4:3 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT8    "720X240P 60HZ 4:3"
#define DEV_TYPE_FMT8    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT8       DISP_PROGRESSIVE
#define FIELD_FMT8       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT8       1440
#define HEIGHT_FMT8      240
#define HSYNCOS_FMT8     (38-1)
#define VSYNCOS_FMT8     (4-1)
#define HSYNCW_FMT8      (HSYNCOS_FMT8 + 124)
#define VSYNCW_FMT8      (VSYNCOS_FMT8 + 3)
#define TVH0_FMT8        (HSYNCW_FMT8 + 114)
#define TVV0_FMT8        (VSYNCW_FMT8 + 15)
#define TVH1_FMT8        (TVH0_FMT8 + WIDTH_FMT8 - 1)
#define TVV1_FMT8        (TVV0_FMT8 + HEIGHT_FMT8 - 1)
#define HPIXEL_FMT8      (TVH1_FMT8 + 2)
#define VLINE_FMT8       (TVV1_FMT8 + 2)
#ifndef FPGA
#define CLK_NX_FMT8      CLK_1X_FMT8
#define CLK_1X_FMT8      27027
#define CLK_SCL_FMT8     200000
#define CLK_PIXEL_FMT8   CLK_1X_FMT8
#else
#define CLK_NX_DIV_FMT8  1
#define CLK_1X_DIV_FMT8  1
#endif

/* 720(1440)x240p @59.94/60Hz 16:9 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT9    "720X240P 60HZ 16:9"
#define DEV_TYPE_FMT9    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT9       DISP_PROGRESSIVE
#define FIELD_FMT9       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT9       1440
#define HEIGHT_FMT9      240
#define HSYNCOS_FMT9     (38-1)
#define VSYNCOS_FMT9     (4-1)
#define HSYNCW_FMT9      (HSYNCOS_FMT9 + 124)
#define VSYNCW_FMT9      (VSYNCOS_FMT9 + 3)
#define TVH0_FMT9        (HSYNCW_FMT9 + 114)
#define TVV0_FMT9        (VSYNCW_FMT9 + 15)
#define TVH1_FMT9        (TVH0_FMT9 + WIDTH_FMT9 - 1)
#define TVV1_FMT9        (TVV0_FMT9 + HEIGHT_FMT9 - 1)
#define HPIXEL_FMT9      (TVH1_FMT9 + 2)
#define VLINE_FMT9       (TVV1_FMT9 + 2)
#ifndef FPGA
#define CLK_NX_FMT9      CLK_1X_FMT9
#define CLK_1X_FMT9      27027
#define CLK_SCL_FMT9     200000
#define CLK_PIXEL_FMT9   CLK_1X_FMT9
#else
#define CLK_NX_DIV_FMT9  1
#define CLK_1X_DIV_FMT9  1
#endif

/* 2880x480i @59.94/60Hz 4:3 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT10    "2880X480I 60HZ 4:3"
#define DEV_TYPE_FMT10    DISP_PANEL_DEVICE_TYPE_STD_INTERLACE_CCIR601
#define FRAME_FMT10       DISP_INTERLACE_NOSOF
#define FIELD_FMT10       DISP_SRC_FIELD_SWAP
#define WIDTH_FMT10       2880
#define HEIGHT_FMT10      (240+240)
#define HSYNCOS_FMT10     (76-1)
#define VSYNCOS_FMT10     ((4-1)+(4-1))
#define HSYNCW_FMT10      (HSYNCOS_FMT10 + 248)
#define VSYNCW_FMT10      (VSYNCOS_FMT10 + 3 + 3)
#define TVH0_FMT10        (HSYNCW_FMT10 + 228)
#define TVV0_FMT10        (VSYNCW_FMT10 + 15 + 12)
#define TVH1_FMT10        (TVH0_FMT10 + WIDTH_FMT10 - 1)
#define TVV1_FMT10        (TVV0_FMT10 + HEIGHT_FMT10 - 1)
#define HPIXEL_FMT10      (TVH1_FMT10 + 2)
#define VLINE_FMT10       (TVV1_FMT10 + 3)
#ifndef FPGA
#define CLK_NX_FMT10      CLK_1X_FMT10
#define CLK_1X_FMT10      54000
#define CLK_SCL_FMT10     200000
#define CLK_PIXEL_FMT10   CLK_1X_FMT10
#else
#define CLK_NX_DIV_FMT10  1
#define CLK_1X_DIV_FMT10  1
#endif

/* 2880x480i @59.94/60Hz 16:9 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT11    "2880X480I 60HZ 16:9"
#define DEV_TYPE_FMT11    DISP_PANEL_DEVICE_TYPE_STD_INTERLACE_CCIR601
#define FRAME_FMT11       DISP_INTERLACE_NOSOF
#define FIELD_FMT11       DISP_SRC_FIELD_SWAP
#define WIDTH_FMT11       2880
#define HEIGHT_FMT11      (240+240)
#define HSYNCOS_FMT11     (76-1)
#define VSYNCOS_FMT11     ((4-1)+(4-1))
#define HSYNCW_FMT11      (HSYNCOS_FMT11 + 248)
#define VSYNCW_FMT11      (VSYNCOS_FMT11 + 3 + 3)
#define TVH0_FMT11        (HSYNCW_FMT11 + 228)
#define TVV0_FMT11        (VSYNCW_FMT11 + 15 + 12)
#define TVH1_FMT11        (TVH0_FMT11 + WIDTH_FMT11 - 1)
#define TVV1_FMT11        (TVV0_FMT11 + HEIGHT_FMT11 - 1)
#define HPIXEL_FMT11      (TVH1_FMT11 + 2)
#define VLINE_FMT11       (TVV1_FMT11 + 3)
#ifndef FPGA
#define CLK_NX_FMT11      CLK_1X_FMT11
#define CLK_1X_FMT11      54000
#define CLK_SCL_FMT11     200000
#define CLK_PIXEL_FMT11   CLK_1X_FMT11
#else
#define CLK_NX_DIV_FMT11  1
#define CLK_1X_DIV_FMT11  1
#endif

/* 2880x240p @59.94/60Hz 4:3 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT12    "2880X240P 60HZ 4:3"
#define DEV_TYPE_FMT12    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT12       DISP_PROGRESSIVE
#define FIELD_FMT12       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT12       2880
#define HEIGHT_FMT12      240
#define HSYNCOS_FMT12     (76-1)
#define VSYNCOS_FMT12     (4-1)
#define HSYNCW_FMT12      (HSYNCOS_FMT12 + 248)
#define VSYNCW_FMT12      (VSYNCOS_FMT12 + 3)
#define TVH0_FMT12        (HSYNCW_FMT12 + 228)
#define TVV0_FMT12        (VSYNCW_FMT12 + 15)
#define TVH1_FMT12        (TVH0_FMT12 + WIDTH_FMT12 - 1)
#define TVV1_FMT12        (TVV0_FMT12 + HEIGHT_FMT12 - 1)
#define HPIXEL_FMT12      (TVH1_FMT12 + 2)
#define VLINE_FMT12       (TVV1_FMT12 + 2)
#ifndef FPGA
#define CLK_NX_FMT12      CLK_1X_FMT12
#define CLK_1X_FMT12      54000
#define CLK_SCL_FMT12     200000
#define CLK_PIXEL_FMT12   CLK_1X_FMT12
#else
#define CLK_NX_DIV_FMT12  1
#define CLK_1X_DIV_FMT12  1
#endif

/* 2880x240p @59.94/60Hz 16:9 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT13    "2880X240P 60HZ 16:9"
#define DEV_TYPE_FMT13    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT13       DISP_PROGRESSIVE
#define FIELD_FMT13       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT13       2880
#define HEIGHT_FMT13      240
#define HSYNCOS_FMT13     (76-1)
#define VSYNCOS_FMT13     (4-1)
#define HSYNCW_FMT13      (HSYNCOS_FMT13 + 248)
#define VSYNCW_FMT13      (VSYNCOS_FMT13 + 3)
#define TVH0_FMT13        (HSYNCW_FMT13 + 228)
#define TVV0_FMT13        (VSYNCW_FMT13 + 15)
#define TVH1_FMT13        (TVH0_FMT13 + WIDTH_FMT13 - 1)
#define TVV1_FMT13        (TVV0_FMT13 + HEIGHT_FMT13 - 1)
#define HPIXEL_FMT13      (TVH1_FMT13 + 2)
#define VLINE_FMT13       (TVV1_FMT13 + 2)
#ifndef FPGA
#define CLK_NX_FMT13      CLK_1X_FMT13
#define CLK_1X_FMT13      54000
#define CLK_SCL_FMT13     200000
#define CLK_PIXEL_FMT13   CLK_1X_FMT13
#else
#define CLK_NX_DIV_FMT13  1
#define CLK_1X_DIV_FMT13  1
#endif

/* 1440x480p @59.94/60Hz 4:3 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT14    "1440X480P 60HZ 4:3"
#define DEV_TYPE_FMT14    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT14       DISP_PROGRESSIVE
#define FIELD_FMT14       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT14       1440
#define HEIGHT_FMT14      480
#define HSYNCOS_FMT14     (32-1)
#define VSYNCOS_FMT14     (9-1)
#define HSYNCW_FMT14      (HSYNCOS_FMT14 + 124)
#define VSYNCW_FMT14      (VSYNCOS_FMT14 + 6)
#define TVH0_FMT14        (HSYNCW_FMT14 + 120)
#define TVV0_FMT14        (VSYNCW_FMT14 + 30)
#define TVH1_FMT14        (TVH0_FMT14 + WIDTH_FMT14 - 1)
#define TVV1_FMT14        (TVV0_FMT14 + HEIGHT_FMT14 - 1)
#define HPIXEL_FMT14      (TVH1_FMT14 + 2)
#define VLINE_FMT14       (TVV1_FMT14 + 2)
#ifndef FPGA
#define CLK_NX_FMT14      CLK_1X_FMT14
#define CLK_1X_FMT14      54000
#define CLK_SCL_FMT14     240000
#define CLK_PIXEL_FMT14   CLK_1X_FMT14
#else
#define CLK_NX_DIV_FMT14  4
#define CLK_1X_DIV_FMT14  1
#endif

/* 1440x480p @59.94/60Hz 16:9 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT15    "1440X480P 60HZ 16:9"
#define DEV_TYPE_FMT15    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT15       DISP_PROGRESSIVE
#define FIELD_FMT15       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT15       1440
#define HEIGHT_FMT15      480
#define HSYNCOS_FMT15     (32-1)
#define VSYNCOS_FMT15     (9-1)
#define HSYNCW_FMT15      (HSYNCOS_FMT15 + 124)
#define VSYNCW_FMT15      (VSYNCOS_FMT15 + 6)
#define TVH0_FMT15        (HSYNCW_FMT15 + 120)
#define TVV0_FMT15        (VSYNCW_FMT15 + 30)
#define TVH1_FMT15        (TVH0_FMT15 + WIDTH_FMT15 - 1)
#define TVV1_FMT15        (TVV0_FMT15 + HEIGHT_FMT15 - 1)
#define HPIXEL_FMT15      (TVH1_FMT15 + 2)
#define VLINE_FMT15       (TVV1_FMT15 + 2)
#ifndef FPGA
#define CLK_NX_FMT15      CLK_1X_FMT15
#define CLK_1X_FMT15      54000
#define CLK_SCL_FMT15     240000
#define CLK_PIXEL_FMT15   CLK_1X_FMT15
#else
#define CLK_NX_DIV_FMT15  4
#define CLK_1X_DIV_FMT15  1
#endif

/* 1920x1080p @59.94/60Hz 16:9 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT16    "1920X1080P 60HZ 16:9"
#define DEV_TYPE_FMT16    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT16       DISP_PROGRESSIVE
#define FIELD_FMT16       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT16       1920
#define HEIGHT_FMT16      1080
#define HSYNCOS_FMT16     (88-1)
#define VSYNCOS_FMT16     (4-1)
#define HSYNCW_FMT16      (HSYNCOS_FMT16 + 44)
#define VSYNCW_FMT16      (VSYNCOS_FMT16 + 5)
#define TVH0_FMT16        (HSYNCW_FMT16 + 148)
#define TVV0_FMT16        (VSYNCW_FMT16 + 36)
#define TVH1_FMT16        (TVH0_FMT16 + WIDTH_FMT16 - 1)
#define TVV1_FMT16        (TVV0_FMT16 + HEIGHT_FMT16 - 1)
#define HPIXEL_FMT16      (TVH1_FMT16 + 2)
#define VLINE_FMT16       (TVV1_FMT16 + 2)
#ifndef FPGA
#define CLK_NX_FMT16      CLK_1X_FMT16
#define CLK_1X_FMT16      148500
#define CLK_SCL_FMT16     240000
#define CLK_PIXEL_FMT16   CLK_1X_FMT16
#else
#define CLK_NX_DIV_FMT16  4
#define CLK_1X_DIV_FMT16  1
#endif

/* 720x576p @50Hz 4:3 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT17    "720X576P 50HZ 4:3"
#define DEV_TYPE_FMT17    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT17       DISP_PROGRESSIVE
#define FIELD_FMT17       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT17       720
#define HEIGHT_FMT17      576
#define HSYNCOS_FMT17     (12-1)
#define VSYNCOS_FMT17     (5-1)
#define HSYNCW_FMT17      (HSYNCOS_FMT17 + 64)
#define VSYNCW_FMT17      (VSYNCOS_FMT17 + 5)
#define TVH0_FMT17        (HSYNCW_FMT17 + 68)
#define TVV0_FMT17        (VSYNCW_FMT17 + 39)
#define TVH1_FMT17        (TVH0_FMT17 + WIDTH_FMT17 - 1)
#define TVV1_FMT17        (TVV0_FMT17 + HEIGHT_FMT17 - 1)
#define HPIXEL_FMT17      (TVH1_FMT17 + 2)
#define VLINE_FMT17       (TVV1_FMT17 + 2)
#ifndef FPGA
#define CLK_NX_FMT17      CLK_1X_FMT17
#define CLK_1X_FMT17      27000
#define CLK_SCL_FMT17     200000
#define CLK_PIXEL_FMT17   CLK_1X_FMT17
#else
#define CLK_NX_DIV_FMT17  1
#define CLK_1X_DIV_FMT17  1
#endif

/* 720x576p @50Hz 16:9 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT18    "720X576P 50HZ 16:9"
#define DEV_TYPE_FMT18    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT18       DISP_PROGRESSIVE
#define FIELD_FMT18       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT18       720
#define HEIGHT_FMT18      576
#define HSYNCOS_FMT18     (12-1)
#define VSYNCOS_FMT18     (5-1)
#define HSYNCW_FMT18      (HSYNCOS_FMT18 + 64)
#define VSYNCW_FMT18      (VSYNCOS_FMT18 + 5)
#define TVH0_FMT18        (HSYNCW_FMT18 + 68)
#define TVV0_FMT18        (VSYNCW_FMT18 + 39)
#define TVH1_FMT18        (TVH0_FMT18 + WIDTH_FMT18 - 1)
#define TVV1_FMT18        (TVV0_FMT18 + HEIGHT_FMT18 - 1)
#define HPIXEL_FMT18      (TVH1_FMT18 + 2)
#define VLINE_FMT18       (TVV1_FMT18 + 2)
#ifndef FPGA
#define CLK_NX_FMT18      CLK_1X_FMT18
#define CLK_1X_FMT18      27000
#define CLK_SCL_FMT18     200000
#define CLK_PIXEL_FMT18   CLK_1X_FMT18
#else
#define CLK_NX_DIV_FMT18  1
#define CLK_1X_DIV_FMT18  1
#endif

/* 1280x720p @50Hz 16:9 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT19    "1280X720P 50HZ 16:9"
#define DEV_TYPE_FMT19    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT19       DISP_PROGRESSIVE
#define FIELD_FMT19       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT19       1280
#define HEIGHT_FMT19      720
#define HSYNCOS_FMT19     (440-1)
#define VSYNCOS_FMT19     (5-1)
#define HSYNCW_FMT19      (HSYNCOS_FMT19 + 40)
#define VSYNCW_FMT19      (VSYNCOS_FMT19 + 5)
#define TVH0_FMT19        (HSYNCW_FMT19 + 220)
#define TVV0_FMT19        (VSYNCW_FMT19 + 20)
#define TVH1_FMT19        (TVH0_FMT19 + WIDTH_FMT19 - 1)
#define TVV1_FMT19        (TVV0_FMT19 + HEIGHT_FMT19 - 1)
#define HPIXEL_FMT19      (TVH1_FMT19 + 2)
#define VLINE_FMT19       (TVV1_FMT19 + 2)
#ifndef FPGA
#define CLK_NX_FMT19      CLK_1X_FMT19
#define CLK_1X_FMT19      74250
#define CLK_SCL_FMT19     148500
#define CLK_PIXEL_FMT19   CLK_1X_FMT19
#else
#define CLK_NX_DIV_FMT19  4
#define CLK_1X_DIV_FMT19  1
#endif

/* 1920x1080i @50Hz 16:9 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT20    "1920X1080I 50HZ 16:9"
#define DEV_TYPE_FMT20    DISP_PANEL_DEVICE_TYPE_STD_INTERLACE_CCIR601
#define FRAME_FMT20       DISP_INTERLACE_NOSOF
#define FIELD_FMT20       DISP_SRC_FIELD_SWAP
#define WIDTH_FMT20       1920
#define HEIGHT_FMT20      (540+540)
#define HSYNCOS_FMT20     (528-1)
#define VSYNCOS_FMT20     ((2-1)+(2-1))
#define HSYNCW_FMT20      (HSYNCOS_FMT20 + 44)
#define VSYNCW_FMT20      (VSYNCOS_FMT20 + 5 + 5)
#define TVH0_FMT20        (HSYNCW_FMT20 + 148)
#define TVV0_FMT20        (VSYNCW_FMT20 + 15 + 16)
#define TVH1_FMT20        (TVH0_FMT20 + WIDTH_FMT20 - 1)
#define TVV1_FMT20        (TVV0_FMT20 + HEIGHT_FMT20 - 1)
#define HPIXEL_FMT20      (TVH1_FMT20 + 2)
#define VLINE_FMT20       (TVV1_FMT20 + 3)
#ifndef FPGA
#define CLK_NX_FMT20      CLK_1X_FMT20
#define CLK_1X_FMT20      74250
#define CLK_SCL_FMT20     148500
#define CLK_PIXEL_FMT20   CLK_1X_FMT20
#else
#define CLK_NX_DIV_FMT20  4
#define CLK_1X_DIV_FMT20  1
#endif

/* 720(1440)x576i @50Hz 4:3 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT21    "720X576I 50HZ 4:3"
#define DEV_TYPE_FMT21    DISP_PANEL_DEVICE_TYPE_STD_INTERLACE_CCIR601
#define FRAME_FMT21       DISP_INTERLACE_NOSOF
#define FIELD_FMT21       DISP_SRC_FIELD_SWAP
#define WIDTH_FMT21       1440
#define HEIGHT_FMT21      (288+288)
#define HSYNCOS_FMT21     (24-1)
#define VSYNCOS_FMT21     ((2-1)+(2-1))
#define HSYNCW_FMT21      (HSYNCOS_FMT21 + 126)
#define VSYNCW_FMT21      (VSYNCOS_FMT21 + 3 + 3)
#define TVH0_FMT21        (HSYNCW_FMT21 + 138)
#define TVV0_FMT21        (VSYNCW_FMT21 + 19 + 20)
#define TVH1_FMT21        (TVH0_FMT21 + WIDTH_FMT21 - 1)
#define TVV1_FMT21        (TVV0_FMT21 + HEIGHT_FMT21 - 1)
#define HPIXEL_FMT21      (TVH1_FMT21 + 2)
#define VLINE_FMT21       (TVV1_FMT21 + 3)
#ifndef FPGA
#define CLK_NX_FMT21      CLK_1X_FMT21
#define CLK_1X_FMT21      27000
#define CLK_SCL_FMT21     200000
#define CLK_PIXEL_FMT21   CLK_1X_FMT21
#else
#define CLK_NX_DIV_FMT21  1
#define CLK_1X_DIV_FMT21  1
#endif

/* 720(1440)x576i @50Hz 16:9 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT22    "720X576I 50HZ 16:9"
#define DEV_TYPE_FMT22    DISP_PANEL_DEVICE_TYPE_STD_INTERLACE_CCIR601
#define FRAME_FMT22       DISP_INTERLACE_NOSOF
#define FIELD_FMT22       DISP_SRC_FIELD_SWAP
#define WIDTH_FMT22       1440
#define HEIGHT_FMT22      (288+288)
#define HSYNCOS_FMT22     (24-1)
#define VSYNCOS_FMT22     ((2-1)+(2-1))
#define HSYNCW_FMT22      (HSYNCOS_FMT22 + 126)
#define VSYNCW_FMT22      (VSYNCOS_FMT22 + 3 + 3)
#define TVH0_FMT22        (HSYNCW_FMT22 + 138)
#define TVV0_FMT22        (VSYNCW_FMT22 + 19 + 20)
#define TVH1_FMT22        (TVH0_FMT22 + WIDTH_FMT22 - 1)
#define TVV1_FMT22        (TVV0_FMT22 + HEIGHT_FMT22 - 1)
#define HPIXEL_FMT22      (TVH1_FMT22 + 2)
#define VLINE_FMT22       (TVV1_FMT22 + 3)
#ifndef FPGA
#define CLK_NX_FMT22      CLK_1X_FMT22
#define CLK_1X_FMT22      27000
#define CLK_SCL_FMT22     200000
#define CLK_PIXEL_FMT22   CLK_1X_FMT22
#else
#define CLK_NX_DIV_FMT22  1
#define CLK_1X_DIV_FMT22  1
#endif

/* 720(1440)x288p @50Hz 4:3 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT23    "720X288P 50HZ 4:3"
#define DEV_TYPE_FMT23    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT23       DISP_PROGRESSIVE
#define FIELD_FMT23       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT23       1440
#define HEIGHT_FMT23      288
#define HSYNCOS_FMT23     (24-1)
#define VSYNCOS_FMT23     (2-1)
#define HSYNCW_FMT23      (HSYNCOS_FMT23 + 126)
#define VSYNCW_FMT23      (VSYNCOS_FMT23 + 3)
#define TVH0_FMT23        (HSYNCW_FMT23 + 138)
#define TVV0_FMT23        (VSYNCW_FMT23 + 19)
#define TVH1_FMT23        (TVH0_FMT23 + WIDTH_FMT23 - 1)
#define TVV1_FMT23        (TVV0_FMT23 + HEIGHT_FMT23 - 1)
#define HPIXEL_FMT23      (TVH1_FMT23 + 2)
#define VLINE_FMT23       (TVV1_FMT23 + 2)
#ifndef FPGA
#define CLK_NX_FMT23      CLK_1X_FMT23
#define CLK_1X_FMT23      27000
#define CLK_SCL_FMT23     200000
#define CLK_PIXEL_FMT23   CLK_1X_FMT23
#else
#define CLK_NX_DIV_FMT23  1
#define CLK_1X_DIV_FMT23  1
#endif

/* 720(1440)x288p @50Hz 16:9 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT24    "720X288P 50HZ 16:9"
#define DEV_TYPE_FMT24    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT24       DISP_PROGRESSIVE
#define FIELD_FMT24       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT24       1440
#define HEIGHT_FMT24      288
#define HSYNCOS_FMT24     (24-1)
#define VSYNCOS_FMT24     (2-1)
#define HSYNCW_FMT24      (HSYNCOS_FMT24 + 126)
#define VSYNCW_FMT24      (VSYNCOS_FMT24 + 3)
#define TVH0_FMT24        (HSYNCW_FMT24 + 138)
#define TVV0_FMT24        (VSYNCW_FMT24 + 19)
#define TVH1_FMT24        (TVH0_FMT24 + WIDTH_FMT24 - 1)
#define TVV1_FMT24        (TVV0_FMT24 + HEIGHT_FMT24 - 1)
#define HPIXEL_FMT24      (TVH1_FMT24 + 2)
#define VLINE_FMT24       (TVV1_FMT24 + 2)
#ifndef FPGA
#define CLK_NX_FMT24      CLK_1X_FMT24
#define CLK_1X_FMT24      27000
#define CLK_SCL_FMT24     200000
#define CLK_PIXEL_FMT24   CLK_1X_FMT24
#else
#define CLK_NX_DIV_FMT24  1
#define CLK_1X_DIV_FMT24  1
#endif

/* 2880x576i @50Hz 4:3 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT25    "2880X576I 50HZ 4:3"
#define DEV_TYPE_FMT25    DISP_PANEL_DEVICE_TYPE_STD_INTERLACE_CCIR601
#define FRAME_FMT25       DISP_INTERLACE_NOSOF
#define FIELD_FMT25       DISP_SRC_FIELD_SWAP
#define WIDTH_FMT25       2880
#define HEIGHT_FMT25      (288+288)
#define HSYNCOS_FMT25     (48-1)
#define VSYNCOS_FMT25     ((2-1)+(2-1))
#define HSYNCW_FMT25      (HSYNCOS_FMT25 + 252)
#define VSYNCW_FMT25      (VSYNCOS_FMT25 + 3 + 3)
#define TVH0_FMT25        (HSYNCW_FMT25 + 276)
#define TVV0_FMT25        (VSYNCW_FMT25 + 19 + 20)
#define TVH1_FMT25        (TVH0_FMT25 + WIDTH_FMT25 - 1)
#define TVV1_FMT25        (TVV0_FMT25 + HEIGHT_FMT25 - 1)
#define HPIXEL_FMT25      (TVH1_FMT25 + 2)
#define VLINE_FMT25       (TVV1_FMT25 + 3)
#ifndef FPGA
#define CLK_NX_FMT25      CLK_1X_FMT25
#define CLK_1X_FMT25      54000
#define CLK_SCL_FMT25     200000
#define CLK_PIXEL_FMT25   CLK_1X_FMT25
#else
#define CLK_NX_DIV_FMT25  1
#define CLK_1X_DIV_FMT25  1
#endif

/* 2880x576i @50Hz 16:9 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT26    "2880X576I 50HZ 16:9"
#define DEV_TYPE_FMT26    DISP_PANEL_DEVICE_TYPE_STD_INTERLACE_CCIR601
#define FRAME_FMT26       DISP_INTERLACE_NOSOF
#define FIELD_FMT26       DISP_SRC_FIELD_SWAP
#define WIDTH_FMT26       2880
#define HEIGHT_FMT26      (288+288)
#define HSYNCOS_FMT26     (48-1)
#define VSYNCOS_FMT26     ((2-1)+(2-1))
#define HSYNCW_FMT26      (HSYNCOS_FMT26 + 252)
#define VSYNCW_FMT26      (VSYNCOS_FMT26 + 3 + 3)
#define TVH0_FMT26        (HSYNCW_FMT26 + 276)
#define TVV0_FMT26        (VSYNCW_FMT26 + 19 + 20)
#define TVH1_FMT26        (TVH0_FMT26 + WIDTH_FMT26 - 1)
#define TVV1_FMT26        (TVV0_FMT26 + HEIGHT_FMT26 - 1)
#define HPIXEL_FMT26      (TVH1_FMT26 + 2)
#define VLINE_FMT26       (TVV1_FMT26 + 3)
#ifndef FPGA
#define CLK_NX_FMT26      CLK_1X_FMT26
#define CLK_1X_FMT26      54000
#define CLK_SCL_FMT26     200000
#define CLK_PIXEL_FMT26   CLK_1X_FMT26
#else
#define CLK_NX_DIV_FMT26  1
#define CLK_1X_DIV_FMT26  1
#endif

/* 2880x288p @50Hz 4:3 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT27    "2880X288P 50HZ 4:3"
#define DEV_TYPE_FMT27    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT27       DISP_PROGRESSIVE
#define FIELD_FMT27       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT27       2880
#define HEIGHT_FMT27      288
#define HSYNCOS_FMT27     (48-1)
#define VSYNCOS_FMT27     (2-1)
#define HSYNCW_FMT27      (HSYNCOS_FMT27 + 252)
#define VSYNCW_FMT27      (VSYNCOS_FMT27 + 3)
#define TVH0_FMT27        (HSYNCW_FMT27 + 276)
#define TVV0_FMT27        (VSYNCW_FMT27 + 18)
#define TVH1_FMT27        (TVH0_FMT27 + WIDTH_FMT27 - 1)
#define TVV1_FMT27        (TVV0_FMT27 + HEIGHT_FMT27 - 1)
#define HPIXEL_FMT27      (TVH1_FMT27 + 2)
#define VLINE_FMT27       (TVV1_FMT27 + 2)
#ifndef FPGA
#define CLK_NX_FMT27      CLK_1X_FMT27
#define CLK_1X_FMT27      54000
#define CLK_SCL_FMT27     200000
#define CLK_PIXEL_FMT27   CLK_1X_FMT27
#else
#define CLK_NX_DIV_FMT27  1
#define CLK_1X_DIV_FMT27  1
#endif

/* 2880x288p @50Hz 16:9 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT28    "2880X288P 50HZ 16:9"
#define DEV_TYPE_FMT28    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT28       DISP_PROGRESSIVE
#define FIELD_FMT28       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT28       2880
#define HEIGHT_FMT28      288
#define HSYNCOS_FMT28     (48-1)
#define VSYNCOS_FMT28     (2-1)
#define HSYNCW_FMT28      (HSYNCOS_FMT28 + 252)
#define VSYNCW_FMT28      (VSYNCOS_FMT28 + 3)
#define TVH0_FMT28        (HSYNCW_FMT28 + 276)
#define TVV0_FMT28        (VSYNCW_FMT28 + 18)
#define TVH1_FMT28        (TVH0_FMT28 + WIDTH_FMT28 - 1)
#define TVV1_FMT28        (TVV0_FMT28 + HEIGHT_FMT28 - 1)
#define HPIXEL_FMT28      (TVH1_FMT28 + 2)
#define VLINE_FMT28       (TVV1_FMT28 + 2)
#ifndef FPGA
#define CLK_NX_FMT28      CLK_1X_FMT28
#define CLK_1X_FMT28      54000
#define CLK_SCL_FMT28     200000
#define CLK_PIXEL_FMT28   CLK_1X_FMT28
#else
#define CLK_NX_DIV_FMT28  1
#define CLK_1X_DIV_FMT28  1
#endif

/* 1440x576p @50Hz 4:3 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT29    "1440X576P 50HZ 4:3"
#define DEV_TYPE_FMT29    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT29       DISP_PROGRESSIVE
#define FIELD_FMT29       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT29       1440
#define HEIGHT_FMT29      480
#define HSYNCOS_FMT29     (24-1)
#define VSYNCOS_FMT29     (5-1)
#define HSYNCW_FMT29      (HSYNCOS_FMT29 + 128)
#define VSYNCW_FMT29      (VSYNCOS_FMT29 + 5)
#define TVH0_FMT29        (HSYNCW_FMT29 + 136)
#define TVV0_FMT29        (VSYNCW_FMT29 + 39)
#define TVH1_FMT29        (TVH0_FMT29 + WIDTH_FMT29 - 1)
#define TVV1_FMT29        (TVV0_FMT29 + HEIGHT_FMT29 - 1)
#define HPIXEL_FMT29      (TVH1_FMT29 + 2)
#define VLINE_FMT29       (TVV1_FMT29 + 2)
#ifndef FPGA
#define CLK_NX_FMT29      CLK_1X_FMT29
#define CLK_1X_FMT29      54000
#define CLK_SCL_FMT29     240000
#define CLK_PIXEL_FMT29   CLK_1X_FMT29
#else
#define CLK_NX_DIV_FMT29  4
#define CLK_1X_DIV_FMT29  1
#endif

/* 1440x576p @50Hz 16:9 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT30    "1440X576P 50HZ 16:9"
#define DEV_TYPE_FMT30    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT30       DISP_PROGRESSIVE
#define FIELD_FMT30       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT30       1440
#define HEIGHT_FMT30      480
#define HSYNCOS_FMT30     (24-1)
#define VSYNCOS_FMT30     (5-1)
#define HSYNCW_FMT30      (HSYNCOS_FMT30 + 128)
#define VSYNCW_FMT30      (VSYNCOS_FMT30 + 5)
#define TVH0_FMT30        (HSYNCW_FMT30 + 136)
#define TVV0_FMT30        (VSYNCW_FMT30 + 39)
#define TVH1_FMT30        (TVH0_FMT30 + WIDTH_FMT30 - 1)
#define TVV1_FMT30        (TVV0_FMT30 + HEIGHT_FMT30 - 1)
#define HPIXEL_FMT30      (TVH1_FMT30 + 2)
#define VLINE_FMT30       (TVV1_FMT30 + 2)
#ifndef FPGA
#define CLK_NX_FMT30      CLK_1X_FMT30
#define CLK_1X_FMT30      54000
#define CLK_SCL_FMT30     240000
#define CLK_PIXEL_FMT30   CLK_1X_FMT30
#else
#define CLK_NX_DIV_FMT30  4
#define CLK_1X_DIV_FMT30  1
#endif

/* 1920x1080p @50Hz 16:9 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT31    "1920X1080P 50HZ 16:9"
#define DEV_TYPE_FMT31    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT31       DISP_PROGRESSIVE
#define FIELD_FMT31       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT31       1920
#define HEIGHT_FMT31      1080
#define HSYNCOS_FMT31     (528-1)
#define VSYNCOS_FMT31     (4-1)
#define HSYNCW_FMT31      (HSYNCOS_FMT31 + 44)
#define VSYNCW_FMT31      (VSYNCOS_FMT31 + 5)
#define TVH0_FMT31        (HSYNCW_FMT31 + 148)
#define TVV0_FMT31        (VSYNCW_FMT31 + 36)
#define TVH1_FMT31        (TVH0_FMT31 + WIDTH_FMT31 - 1)
#define TVV1_FMT31        (TVV0_FMT31 + HEIGHT_FMT31 - 1)
#define HPIXEL_FMT31      (TVH1_FMT31 + 2)
#define VLINE_FMT31       (TVV1_FMT31 + 2)
#ifndef FPGA
#define CLK_NX_FMT31      CLK_1X_FMT31
#define CLK_1X_FMT31      148500
#define CLK_SCL_FMT31     240000
#define CLK_PIXEL_FMT31   CLK_1X_FMT31
#else
#define CLK_NX_DIV_FMT31  4
#define CLK_1X_DIV_FMT31  1
#endif

/* 1920x1080p @ 23.98/24Hz 16:9 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT32    "1920X1080P 24HZ 16:9"
#define DEV_TYPE_FMT32    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT32       DISP_PROGRESSIVE
#define FIELD_FMT32       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT32       1920
#define HEIGHT_FMT32      1080
#define HSYNCOS_FMT32     (638-1)
#define VSYNCOS_FMT32     (4-1)
#define HSYNCW_FMT32      (HSYNCOS_FMT32 + 44)
#define VSYNCW_FMT32      (VSYNCOS_FMT32 + 5)
#define TVH0_FMT32        (HSYNCW_FMT32 + 148)
#define TVV0_FMT32        (VSYNCW_FMT32 + 36)
#define TVH1_FMT32        (TVH0_FMT32 + WIDTH_FMT32 - 1)
#define TVV1_FMT32        (TVV0_FMT32 + HEIGHT_FMT32 - 1)
#define HPIXEL_FMT32      (TVH1_FMT32 + 2)
#define VLINE_FMT32       (TVV1_FMT32 + 2)
#ifndef FPGA
#define CLK_NX_FMT32      CLK_1X_FMT32
#define CLK_1X_FMT32      74250
#define CLK_SCL_FMT32     148500
#define CLK_PIXEL_FMT32   CLK_1X_FMT32
#else
#define CLK_NX_DIV_FMT32  4
#define CLK_1X_DIV_FMT32  1
#endif

/* 1920x1080p @ 23.98/24Hz 16:9 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT33    "1920X1080P 25HZ 16:9"
#define DEV_TYPE_FMT33    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT33       DISP_PROGRESSIVE
#define FIELD_FMT33       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT33       1920
#define HEIGHT_FMT33      1080
#define HSYNCOS_FMT33     (528-1)
#define VSYNCOS_FMT33     (4-1)
#define HSYNCW_FMT33      (HSYNCOS_FMT33 + 44)
#define VSYNCW_FMT33      (VSYNCOS_FMT33 + 5)
#define TVH0_FMT33        (HSYNCW_FMT33 + 148)
#define TVV0_FMT33        (VSYNCW_FMT33 + 36)
#define TVH1_FMT33        (TVH0_FMT33 + WIDTH_FMT33 - 1)
#define TVV1_FMT33        (TVV0_FMT33 + HEIGHT_FMT33 - 1)
#define HPIXEL_FMT33      (TVH1_FMT33 + 2)
#define VLINE_FMT33       (TVV1_FMT33 + 2)
#ifndef FPGA
#define CLK_NX_FMT33      CLK_1X_FMT33
#define CLK_1X_FMT33      74250
#define CLK_SCL_FMT33     148500
#define CLK_PIXEL_FMT33   CLK_1X_FMT33
#else
#define CLK_NX_DIV_FMT33  4
#define CLK_1X_DIV_FMT33  1
#endif

/* 1920x1080p @ 23.98/24Hz 16:9 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT34    "1920X1080P 30HZ 16:9"
#define DEV_TYPE_FMT34    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT34       DISP_PROGRESSIVE
#define FIELD_FMT34       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT34       1920
#define HEIGHT_FMT34      1080
#define HSYNCOS_FMT34     (88-1)
#define VSYNCOS_FMT34     (4-1)
#define HSYNCW_FMT34      (HSYNCOS_FMT34 + 44)
#define VSYNCW_FMT34      (VSYNCOS_FMT34 + 5)
#define TVH0_FMT34        (HSYNCW_FMT34 + 148)
#define TVV0_FMT34        (VSYNCW_FMT34 + 36)
#define TVH1_FMT34        (TVH0_FMT34 + WIDTH_FMT34 - 1)
#define TVV1_FMT34        (TVV0_FMT34 + HEIGHT_FMT34 - 1)
#define HPIXEL_FMT34      (TVH1_FMT34 + 2)
#define VLINE_FMT34       (TVV1_FMT34 + 2)
#ifndef FPGA
#define CLK_NX_FMT34      CLK_1X_FMT34
#define CLK_1X_FMT34      74250
#define CLK_SCL_FMT34     148500
#define CLK_PIXEL_FMT34   CLK_1X_FMT34
#else
#define CLK_NX_DIV_FMT34  4
#define CLK_1X_DIV_FMT34  1
#endif

/* 2880x480p @59.94/60Hz 4:3 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT35    "2880X480P 60HZ 4:3"
#define DEV_TYPE_FMT35    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT35       DISP_PROGRESSIVE
#define FIELD_FMT35       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT35       2880
#define HEIGHT_FMT35      480
#define HSYNCOS_FMT35     (64-1)
#define VSYNCOS_FMT35     (9-1)
#define HSYNCW_FMT35      (HSYNCOS_FMT35 + 248)
#define VSYNCW_FMT35      (VSYNCOS_FMT35 + 6)
#define TVH0_FMT35        (HSYNCW_FMT35 + 240)
#define TVV0_FMT35        (VSYNCW_FMT35 + 30)
#define TVH1_FMT35        (TVH0_FMT35 + WIDTH_FMT35 - 1)
#define TVV1_FMT35        (TVV0_FMT35 + HEIGHT_FMT35 - 1)
#define HPIXEL_FMT35      (TVH1_FMT35 + 2)
#define VLINE_FMT35       (TVV1_FMT35 + 2)
#ifndef FPGA
#define CLK_NX_FMT35      CLK_1X_FMT35
#define CLK_1X_FMT35      108000
#define CLK_SCL_FMT35     200000
#define CLK_PIXEL_FMT35   CLK_1X_FMT35
#else
#define CLK_NX_DIV_FMT35  1
#define CLK_1X_DIV_FMT35  1
#endif

/* 2880x480p @59.94/60Hz 16:9 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT36    "2880X480P 60HZ 16:9"
#define DEV_TYPE_FMT36    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT36       DISP_PROGRESSIVE
#define FIELD_FMT36       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT36       2880
#define HEIGHT_FMT36      480
#define HSYNCOS_FMT36     (64-1)
#define VSYNCOS_FMT36     (9-1)
#define HSYNCW_FMT36      (HSYNCOS_FMT36 + 248)
#define VSYNCW_FMT36      (VSYNCOS_FMT36 + 6)
#define TVH0_FMT36        (HSYNCW_FMT36 + 240)
#define TVV0_FMT36        (VSYNCW_FMT36 + 30)
#define TVH1_FMT36        (TVH0_FMT36 + WIDTH_FMT36 - 1)
#define TVV1_FMT36        (TVV0_FMT36 + HEIGHT_FMT36 - 1)
#define HPIXEL_FMT36      (TVH1_FMT36 + 2)
#define VLINE_FMT36       (TVV1_FMT36 + 2)
#ifndef FPGA
#define CLK_NX_FMT36      CLK_1X_FMT36
#define CLK_1X_FMT36      108000
#define CLK_SCL_FMT36     200000
#define CLK_PIXEL_FMT36   CLK_1X_FMT36
#else
#define CLK_NX_DIV_FMT36  1
#define CLK_1X_DIV_FMT36  1
#endif

/* 2880x576p @50Hz 4:3 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT37    "2880X576P 60HZ 4:3"
#define DEV_TYPE_FMT37    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT37       DISP_PROGRESSIVE
#define FIELD_FMT37       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT37       2880
#define HEIGHT_FMT37      576
#define HSYNCOS_FMT37     (48-1)
#define VSYNCOS_FMT37     (5-1)
#define HSYNCW_FMT37      (HSYNCOS_FMT37 + 256)
#define VSYNCW_FMT37      (VSYNCOS_FMT37 + 5)
#define TVH0_FMT37        (HSYNCW_FMT37 + 272)
#define TVV0_FMT37        (VSYNCW_FMT37 + 39)
#define TVH1_FMT37        (TVH0_FMT37 + WIDTH_FMT37 - 1)
#define TVV1_FMT37        (TVV0_FMT37 + HEIGHT_FMT37 - 1)
#define HPIXEL_FMT37      (TVH1_FMT37 + 2)
#define VLINE_FMT37       (TVV1_FMT37 + 2)
#ifndef FPGA
#define CLK_NX_FMT37      CLK_1X_FMT37
#define CLK_1X_FMT37      108000
#define CLK_SCL_FMT37     200000
#define CLK_PIXEL_FMT37   CLK_1X_FMT37
#else
#define CLK_NX_DIV_FMT37  1
#define CLK_1X_DIV_FMT37  1
#endif

/* 2880x576p @50Hz 16:9 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT38    "2880X576P 60HZ 16:9"
#define DEV_TYPE_FMT38    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT38       DISP_PROGRESSIVE
#define FIELD_FMT38       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT38       2880
#define HEIGHT_FMT38      576
#define HSYNCOS_FMT38     (48-1)
#define VSYNCOS_FMT38     (5-1)
#define HSYNCW_FMT38      (HSYNCOS_FMT38 + 256)
#define VSYNCW_FMT38      (VSYNCOS_FMT38 + 5)
#define TVH0_FMT38        (HSYNCW_FMT38 + 272)
#define TVV0_FMT38        (VSYNCW_FMT38 + 39)
#define TVH1_FMT38        (TVH0_FMT38 + WIDTH_FMT38 - 1)
#define TVV1_FMT38        (TVV0_FMT38 + HEIGHT_FMT38 - 1)
#define HPIXEL_FMT38      (TVH1_FMT38 + 2)
#define VLINE_FMT38       (TVV1_FMT38 + 2)
#ifndef FPGA
#define CLK_NX_FMT38      CLK_1X_FMT38
#define CLK_1X_FMT38      108000
#define CLK_SCL_FMT38     200000
#define CLK_PIXEL_FMT38   CLK_1X_FMT38
#else
#define CLK_NX_DIV_FMT38  1
#define CLK_1X_DIV_FMT38  1
#endif

/* 1920x1080i @100Hz 16:9 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT40    "1920X1080I 100HZ 16:9"
#define DEV_TYPE_FMT40    DISP_PANEL_DEVICE_TYPE_STD_INTERLACE_CCIR601
#define FRAME_FMT40       DISP_INTERLACE_NOSOF
#define FIELD_FMT40       DISP_SRC_FIELD_SWAP
#define WIDTH_FMT40       1920
#define HEIGHT_FMT40      (540+540)
#define HSYNCOS_FMT40     (528-1)
#define VSYNCOS_FMT40     ((2-1)+(2-1))
#define HSYNCW_FMT40      (HSYNCOS_FMT40 + 44)
#define VSYNCW_FMT40      (VSYNCOS_FMT40 + 5 + 5)
#define TVH0_FMT40        (HSYNCW_FMT40 + 148)
#define TVV0_FMT40        (VSYNCW_FMT40 + 15 + 16)
#define TVH1_FMT40        (TVH0_FMT40 + WIDTH_FMT40 - 1)
#define TVV1_FMT40        (TVV0_FMT40 + HEIGHT_FMT40 - 1)
#define HPIXEL_FMT40      (TVH1_FMT40 + 2)
#define VLINE_FMT40       (TVV1_FMT40 + 3)
#ifndef FPGA
#define CLK_NX_FMT40      CLK_1X_FMT40
#define CLK_1X_FMT40      148500
#define CLK_SCL_FMT40     240000
#define CLK_PIXEL_FMT40   CLK_1X_FMT40
#else
#define CLK_NX_DIV_FMT40  4
#define CLK_1X_DIV_FMT40  1
#endif

/* 1280x720p @100Hz 16:9 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT41    "1280X720P 100HZ 16:9"
#define DEV_TYPE_FMT41    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT41       DISP_PROGRESSIVE
#define FIELD_FMT41       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT41       1280
#define HEIGHT_FMT41      720
#define HSYNCOS_FMT41     (440-1)
#define VSYNCOS_FMT41     (5-1)
#define HSYNCW_FMT41      (HSYNCOS_FMT41 + 40)
#define VSYNCW_FMT41      (VSYNCOS_FMT41 + 5)
#define TVH0_FMT41        (HSYNCW_FMT41 + 220)
#define TVV0_FMT41        (VSYNCW_FMT41 + 20)
#define TVH1_FMT41        (TVH0_FMT41 + WIDTH_FMT41 - 1)
#define TVV1_FMT41        (TVV0_FMT41 + HEIGHT_FMT41 - 1)
#define HPIXEL_FMT41      (TVH1_FMT41 + 2)
#define VLINE_FMT41       (TVV1_FMT41 + 2)
#ifndef FPGA
#define CLK_NX_FMT41      CLK_1X_FMT41
#define CLK_1X_FMT41      148500
#define CLK_SCL_FMT41     240000
#define CLK_PIXEL_FMT41   CLK_1X_FMT41
#else
#define CLK_NX_DIV_FMT41  4
#define CLK_1X_DIV_FMT41  1
#endif

/* 720x576p @100Hz 4:3 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT42    "720X576P 100HZ 4:3"
#define DEV_TYPE_FMT42    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT42       DISP_PROGRESSIVE
#define FIELD_FMT42       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT42       720
#define HEIGHT_FMT42      576
#define HSYNCOS_FMT42     (12-1)
#define VSYNCOS_FMT42     (5-1)
#define HSYNCW_FMT42      (HSYNCOS_FMT42 + 64)
#define VSYNCW_FMT42      (VSYNCOS_FMT42 + 5)
#define TVH0_FMT42        (HSYNCW_FMT42 + 68)
#define TVV0_FMT42        (VSYNCW_FMT42 + 39)
#define TVH1_FMT42        (TVH0_FMT42 + WIDTH_FMT42 - 1)
#define TVV1_FMT42        (TVV0_FMT42 + HEIGHT_FMT42 - 1)
#define HPIXEL_FMT42      (TVH1_FMT42 + 2)
#define VLINE_FMT42       (TVV1_FMT42 + 2)
#ifndef FPGA
#define CLK_NX_FMT42      CLK_1X_FMT42
#define CLK_1X_FMT42      54000
#define CLK_SCL_FMT42     200000
#define CLK_PIXEL_FMT42   CLK_1X_FMT42
#else
#define CLK_NX_DIV_FMT42  1
#define CLK_1X_DIV_FMT42  1
#endif

/* 720x576p @100Hz 16:9 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT43    "720X576P 100HZ 16:9"
#define DEV_TYPE_FMT43    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT43       DISP_PROGRESSIVE
#define FIELD_FMT43       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT43       720
#define HEIGHT_FMT43      576
#define HSYNCOS_FMT43     (12-1)
#define VSYNCOS_FMT43     (5-1)
#define HSYNCW_FMT43      (HSYNCOS_FMT43 + 64)
#define VSYNCW_FMT43      (VSYNCOS_FMT43 + 5)
#define TVH0_FMT43        (HSYNCW_FMT43 + 68)
#define TVV0_FMT43        (VSYNCW_FMT43 + 39)
#define TVH1_FMT43        (TVH0_FMT43 + WIDTH_FMT43 - 1)
#define TVV1_FMT43        (TVV0_FMT43 + HEIGHT_FMT43 - 1)
#define HPIXEL_FMT43      (TVH1_FMT43 + 2)
#define VLINE_FMT43       (TVV1_FMT43 + 2)
#ifndef FPGA
#define CLK_NX_FMT43      CLK_1X_FMT43
#define CLK_1X_FMT43      54000
#define CLK_SCL_FMT43     200000
#define CLK_PIXEL_FMT43   CLK_1X_FMT43
#else
#define CLK_NX_DIV_FMT43  1
#define CLK_1X_DIV_FMT43  1
#endif

/* 720(1440)x576i @100Hz 4:3 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT44    "720X576I 100HZ 4:3"
#define DEV_TYPE_FMT44    DISP_PANEL_DEVICE_TYPE_STD_INTERLACE_CCIR601
#define FRAME_FMT44       DISP_INTERLACE_NOSOF
#define FIELD_FMT44       DISP_SRC_FIELD_SWAP
#define WIDTH_FMT44       1440
#define HEIGHT_FMT44      (288+288)
#define HSYNCOS_FMT44     (24-1)
#define VSYNCOS_FMT44     ((2-1)+(2-1))
#define HSYNCW_FMT44      (HSYNCOS_FMT44 + 126)
#define VSYNCW_FMT44      (VSYNCOS_FMT44 + 3 + 3)
#define TVH0_FMT44        (HSYNCW_FMT44 + 138)
#define TVV0_FMT44        (VSYNCW_FMT44 + 19 + 20)
#define TVH1_FMT44        (TVH0_FMT44 + WIDTH_FMT44 - 1)
#define TVV1_FMT44        (TVV0_FMT44 + HEIGHT_FMT44 - 1)
#define HPIXEL_FMT44      (TVH1_FMT44 + 2)
#define VLINE_FMT44       (TVV1_FMT44 + 3)
#ifndef FPGA
#define CLK_NX_FMT44      CLK_1X_FMT44
#define CLK_1X_FMT44      54000
#define CLK_SCL_FMT44     200000
#define CLK_PIXEL_FMT44   CLK_1X_FMT44
#else
#define CLK_NX_DIV_FMT44  1
#define CLK_1X_DIV_FMT44  1
#endif

/* 720(1440)x576i @100Hz 16:9 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT45    "720X576I 100HZ 16:9"
#define DEV_TYPE_FMT45    DISP_PANEL_DEVICE_TYPE_STD_INTERLACE_CCIR601
#define FRAME_FMT45       DISP_INTERLACE_NOSOF
#define FIELD_FMT45       DISP_SRC_FIELD_SWAP
#define WIDTH_FMT45       1440
#define HEIGHT_FMT45      (288+288)
#define HSYNCOS_FMT45     (24-1)
#define VSYNCOS_FMT45     ((2-1)+(2-1))
#define HSYNCW_FMT45      (HSYNCOS_FMT45 + 126)
#define VSYNCW_FMT45      (VSYNCOS_FMT45 + 3 + 3)
#define TVH0_FMT45        (HSYNCW_FMT45 + 138)
#define TVV0_FMT45        (VSYNCW_FMT45 + 19 + 20)
#define TVH1_FMT45        (TVH0_FMT45 + WIDTH_FMT45 - 1)
#define TVV1_FMT45        (TVV0_FMT45 + HEIGHT_FMT45 - 1)
#define HPIXEL_FMT45      (TVH1_FMT45 + 2)
#define VLINE_FMT45       (TVV1_FMT45 + 3)
#ifndef FPGA
#define CLK_NX_FMT45      CLK_1X_FMT45
#define CLK_1X_FMT45      54000
#define CLK_SCL_FMT45     200000
#define CLK_PIXEL_FMT45   CLK_1X_FMT45
#else
#define CLK_NX_DIV_FMT45  1
#define CLK_1X_DIV_FMT45  1
#endif

/* 1920x1080i @119.88/120Hz 16:9 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT46    "1920X1080I 120HZ 16:9"
#define DEV_TYPE_FMT46    DISP_PANEL_DEVICE_TYPE_STD_INTERLACE_CCIR601
#define FRAME_FMT46       DISP_INTERLACE_NOSOF
#define FIELD_FMT46       DISP_SRC_FIELD_SWAP
#define WIDTH_FMT46       1920
#define HEIGHT_FMT46      (540+540)
#define HSYNCOS_FMT46     (88-1)
#define VSYNCOS_FMT46     ((2-1)+(2-1))
#define HSYNCW_FMT46      (HSYNCOS_FMT46 + 44)
#define VSYNCW_FMT46      (VSYNCOS_FMT46 + 5 + 5)
#define TVH0_FMT46        (HSYNCW_FMT46 + 148)
#define TVV0_FMT46        (VSYNCW_FMT46 + 15 + 16)
#define TVH1_FMT46        (TVH0_FMT46 + WIDTH_FMT46 - 1)
#define TVV1_FMT46        (TVV0_FMT46 + HEIGHT_FMT46 - 1)
#define HPIXEL_FMT46      (TVH1_FMT46 + 2)
#define VLINE_FMT46       (TVV1_FMT46 + 3)
#ifndef FPGA
#define CLK_NX_FMT46      CLK_1X_FMT46
#define CLK_1X_FMT46      148500
#define CLK_SCL_FMT46     300000
#define CLK_PIXEL_FMT46   CLK_1X_FMT46
#else
#define CLK_NX_DIV_FMT46  4
#define CLK_1X_DIV_FMT46  1
#endif

/* 1280x720p @5119.88/120Hz 16:9 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT47    "1280X720P 120HZ 16:9"
#define DEV_TYPE_FMT47    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT47       DISP_PROGRESSIVE
#define FIELD_FMT47       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT47       1280
#define HEIGHT_FMT47      720
#define HSYNCOS_FMT47     (110-1)
#define VSYNCOS_FMT47     (5-1)
#define HSYNCW_FMT47      (HSYNCOS_FMT47 + 40)
#define VSYNCW_FMT47      (VSYNCOS_FMT47 + 5)
#define TVH0_FMT47        (HSYNCW_FMT47 + 220)
#define TVV0_FMT47        (VSYNCW_FMT47 + 20)
#define TVH1_FMT47        (TVH0_FMT47 + WIDTH_FMT47 - 1)
#define TVV1_FMT47        (TVV0_FMT47 + HEIGHT_FMT47 - 1)
#define HPIXEL_FMT47      (TVH1_FMT47 + 2)
#define VLINE_FMT47       (TVV1_FMT47 + 2)
#ifndef FPGA
#define CLK_NX_FMT47      CLK_1X_FMT47
#define CLK_1X_FMT47      148500
#define CLK_SCL_FMT47     240000
#define CLK_PIXEL_FMT47   CLK_1X_FMT47
#else
#define CLK_NX_DIV_FMT47  4
#define CLK_1X_DIV_FMT47  1
#endif

/* 720x480p @119.88/120Hz 4:3 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT48    "720X480P 120HZ 4:3"
#define DEV_TYPE_FMT48    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT48       DISP_PROGRESSIVE
#define FIELD_FMT48       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT48       720
#define HEIGHT_FMT48      480
#define HSYNCOS_FMT48     (16-1)
#define VSYNCOS_FMT48     (9-1)
#define HSYNCW_FMT48      (HSYNCOS_FMT48 + 62)
#define VSYNCW_FMT48      (VSYNCOS_FMT48 + 6)
#define TVH0_FMT48        (HSYNCW_FMT48 + 60)
#define TVV0_FMT48        (VSYNCW_FMT48 + 30)
#define TVH1_FMT48        (TVH0_FMT48 + WIDTH_FMT48 - 1)
#define TVV1_FMT48        (TVV0_FMT48 + HEIGHT_FMT48 - 1)
#define HPIXEL_FMT48      (TVH1_FMT48 + 2)
#define VLINE_FMT48       (TVV1_FMT48 + 2)
#ifndef FPGA
#define CLK_NX_FMT48      CLK_1X_FMT48
#define CLK_1X_FMT48      54000
#define CLK_SCL_FMT48     200000
#define CLK_PIXEL_FMT48   CLK_1X_FMT48
#else
#define CLK_NX_DIV_FMT48  1
#define CLK_1X_DIV_FMT48  1
#endif

/* 720x480p @119.88/120Hz 16:9 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT49    "720X480P 120HZ 16:9"
#define DEV_TYPE_FMT49    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT49       DISP_PROGRESSIVE
#define FIELD_FMT49       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT49       720
#define HEIGHT_FMT49      480
#define HSYNCOS_FMT49     (16-1)
#define VSYNCOS_FMT49     (9-1)
#define HSYNCW_FMT49      (HSYNCOS_FMT49 + 62)
#define VSYNCW_FMT49      (VSYNCOS_FMT49 + 6)
#define TVH0_FMT49        (HSYNCW_FMT49 + 60)
#define TVV0_FMT49        (VSYNCW_FMT49 + 30)
#define TVH1_FMT49        (TVH0_FMT49 + WIDTH_FMT49 - 1)
#define TVV1_FMT49        (TVV0_FMT49 + HEIGHT_FMT49 - 1)
#define HPIXEL_FMT49      (TVH1_FMT49 + 2)
#define VLINE_FMT49       (TVV1_FMT49 + 2)
#ifndef FPGA
#define CLK_NX_FMT49      CLK_1X_FMT49
#define CLK_1X_FMT49      54000
#define CLK_SCL_FMT49     200000
#define CLK_PIXEL_FMT49   CLK_1X_FMT49
#else
#define CLK_NX_DIV_FMT49  1
#define CLK_1X_DIV_FMT49  1
#endif

/* 720(1440)x480i @119.88/120Hz 4:3 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT50    "720X480I 120HZ 4:3"
#define DEV_TYPE_FMT50    DISP_PANEL_DEVICE_TYPE_STD_INTERLACE_CCIR601
#define FRAME_FMT50       DISP_INTERLACE_NOSOF
#define FIELD_FMT50       DISP_SRC_FIELD_SWAP
#define WIDTH_FMT50       1440
#define HEIGHT_FMT50      (240+240)
#define HSYNCOS_FMT50     (38-1)
#define VSYNCOS_FMT50     ((4-1)+(4-1))
#define HSYNCW_FMT50      (HSYNCOS_FMT50 + 124)
#define VSYNCW_FMT50      (VSYNCOS_FMT50 + 3 + 3)
#define TVH0_FMT50        (HSYNCW_FMT50 + 114)
#define TVV0_FMT50        (VSYNCW_FMT50 + 15 + 16)
#define TVH1_FMT50        (TVH0_FMT50 + WIDTH_FMT50 - 1)
#define TVV1_FMT50        (TVV0_FMT50 + HEIGHT_FMT50 - 1)
#define HPIXEL_FMT50      (TVH1_FMT50 + 2)
#define VLINE_FMT50       (TVV1_FMT50 + 3)
#ifndef FPGA
#define CLK_NX_FMT50      CLK_1X_FMT50
#define CLK_1X_FMT50      54000
#define CLK_SCL_FMT50     200000
#define CLK_PIXEL_FMT50   CLK_1X_FMT50
#else
#define CLK_NX_DIV_FMT50  1
#define CLK_1X_DIV_FMT50  1
#endif

/* 720(1440)x480i @119.88/120Hz 16:9 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT51    "720X480I 120HZ 16:9"
#define DEV_TYPE_FMT51    DISP_PANEL_DEVICE_TYPE_STD_INTERLACE_CCIR601
#define FRAME_FMT51       DISP_INTERLACE_NOSOF
#define FIELD_FMT51       DISP_SRC_FIELD_SWAP
#define WIDTH_FMT51       1440
#define HEIGHT_FMT51      (240+240)
#define HSYNCOS_FMT51     (38-1)
#define VSYNCOS_FMT51     ((4-1)+(4-1))
#define HSYNCW_FMT51      (HSYNCOS_FMT51 + 124)
#define VSYNCW_FMT51      (VSYNCOS_FMT51 + 3 + 3)
#define TVH0_FMT51        (HSYNCW_FMT51 + 114)
#define TVV0_FMT51        (VSYNCW_FMT51 + 15 + 16)
#define TVH1_FMT51        (TVH0_FMT51 + WIDTH_FMT51 - 1)
#define TVV1_FMT51        (TVV0_FMT51 + HEIGHT_FMT51 - 1)
#define HPIXEL_FMT51      (TVH1_FMT51 + 2)
#define VLINE_FMT51       (TVV1_FMT51 + 3)
#ifndef FPGA
#define CLK_NX_FMT51      CLK_1X_FMT51
#define CLK_1X_FMT51      54000
#define CLK_SCL_FMT51     200000
#define CLK_PIXEL_FMT51   CLK_1X_FMT51
#else
#define CLK_NX_DIV_FMT51  1
#define CLK_1X_DIV_FMT51  1
#endif

/* 720x576p @200Hz 4:3 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT52    "720X576P 200HZ 4:3"
#define DEV_TYPE_FMT52    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT52       DISP_PROGRESSIVE
#define FIELD_FMT52       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT52       720
#define HEIGHT_FMT52      576
#define HSYNCOS_FMT52     (12-1)
#define VSYNCOS_FMT52     (5-1)
#define HSYNCW_FMT52      (HSYNCOS_FMT52 + 64)
#define VSYNCW_FMT52      (VSYNCOS_FMT52 + 5)
#define TVH0_FMT52        (HSYNCW_FMT52 + 68)
#define TVV0_FMT52        (VSYNCW_FMT52 + 39)
#define TVH1_FMT52        (TVH0_FMT52 + WIDTH_FMT52 - 1)
#define TVV1_FMT52        (TVV0_FMT52 + HEIGHT_FMT52 - 1)
#define HPIXEL_FMT52      (TVH1_FMT52 + 2)
#define VLINE_FMT52       (TVV1_FMT52 + 2)
#ifndef FPGA
#define CLK_NX_FMT52      CLK_1X_FMT52
#define CLK_1X_FMT52      108000
#define CLK_SCL_FMT52     200000
#define CLK_PIXEL_FMT52   CLK_1X_FMT52
#else
#define CLK_NX_DIV_FMT52  1
#define CLK_1X_DIV_FMT52  1
#endif

/* 720x576p @200Hz 16:9 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT53    "720X576P 200HZ 16:9"
#define DEV_TYPE_FMT53    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT53       DISP_PROGRESSIVE
#define FIELD_FMT53       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT53       720
#define HEIGHT_FMT53      576
#define HSYNCOS_FMT53     (12-1)
#define VSYNCOS_FMT53     (5-1)
#define HSYNCW_FMT53      (HSYNCOS_FMT53 + 64)
#define VSYNCW_FMT53      (VSYNCOS_FMT53 + 5)
#define TVH0_FMT53        (HSYNCW_FMT53 + 68)
#define TVV0_FMT53        (VSYNCW_FMT53 + 39)
#define TVH1_FMT53        (TVH0_FMT53 + WIDTH_FMT53 - 1)
#define TVV1_FMT53        (TVV0_FMT53 + HEIGHT_FMT53 - 1)
#define HPIXEL_FMT53      (TVH1_FMT53 + 2)
#define VLINE_FMT53       (TVV1_FMT53 + 2)
#ifndef FPGA
#define CLK_NX_FMT53      CLK_1X_FMT53
#define CLK_1X_FMT53      108000
#define CLK_SCL_FMT53     200000
#define CLK_PIXEL_FMT53   CLK_1X_FMT53
#else
#define CLK_NX_DIV_FMT53  1
#define CLK_1X_DIV_FMT53  1
#endif

/* 720(1440)x576i @200Hz 4:3 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT54    "720X576I 200HZ 4:3"
#define DEV_TYPE_FMT54    DISP_PANEL_DEVICE_TYPE_STD_INTERLACE_CCIR601
#define FRAME_FMT54       DISP_INTERLACE_NOSOF
#define FIELD_FMT54       DISP_SRC_FIELD_SWAP
#define WIDTH_FMT54       1440
#define HEIGHT_FMT54      (288+288)
#define HSYNCOS_FMT54     (24-1)
#define VSYNCOS_FMT54     ((2-1)+(2-1))
#define HSYNCW_FMT54      (HSYNCOS_FMT54 + 126)
#define VSYNCW_FMT54      (VSYNCOS_FMT54 + 3 + 3)
#define TVH0_FMT54        (HSYNCW_FMT54 + 138)
#define TVV0_FMT54        (VSYNCW_FMT54 + 19 + 20)
#define TVH1_FMT54        (TVH0_FMT54 + WIDTH_FMT54 - 1)
#define TVV1_FMT54        (TVV0_FMT54 + HEIGHT_FMT54 - 1)
#define HPIXEL_FMT54      (TVH1_FMT54 + 2)
#define VLINE_FMT54       (TVV1_FMT54 + 3)
#ifndef FPGA
#define CLK_NX_FMT54      CLK_1X_FMT54
#define CLK_1X_FMT54      108000
#define CLK_SCL_FMT54     200000
#define CLK_PIXEL_FMT54   CLK_1X_FMT54
#else
#define CLK_NX_DIV_FMT54  1
#define CLK_1X_DIV_FMT54  1
#endif

/* 720(1440)x576i @200Hz 16:9 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT55    "720X576I 200HZ 16:9"
#define DEV_TYPE_FMT55    DISP_PANEL_DEVICE_TYPE_STD_INTERLACE_CCIR601
#define FRAME_FMT55       DISP_INTERLACE_NOSOF
#define FIELD_FMT55       DISP_SRC_FIELD_SWAP
#define WIDTH_FMT55       1440
#define HEIGHT_FMT55      (288+288)
#define HSYNCOS_FMT55     (24-1)
#define VSYNCOS_FMT55     ((2-1)+(2-1))
#define HSYNCW_FMT55      (HSYNCOS_FMT55 + 126)
#define VSYNCW_FMT55      (VSYNCOS_FMT55 + 3 + 3)
#define TVH0_FMT55        (HSYNCW_FMT55 + 138)
#define TVV0_FMT55        (VSYNCW_FMT55 + 19 + 20)
#define TVH1_FMT55        (TVH0_FMT55 + WIDTH_FMT55 - 1)
#define TVV1_FMT55        (TVV0_FMT55 + HEIGHT_FMT55 - 1)
#define HPIXEL_FMT55      (TVH1_FMT55 + 2)
#define VLINE_FMT55       (TVV1_FMT55 + 3)
#ifndef FPGA
#define CLK_NX_FMT55      CLK_1X_FMT55
#define CLK_1X_FMT55      108000
#define CLK_SCL_FMT55     200000
#define CLK_PIXEL_FMT55   CLK_1X_FMT55
#else
#define CLK_NX_DIV_FMT55  1
#define CLK_1X_DIV_FMT55  1
#endif

/* 720x480p @239.76/240Hz 4:3 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT56    "720X480P 240HZ 4:3"
#define DEV_TYPE_FMT56    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT56       DISP_PROGRESSIVE
#define FIELD_FMT56       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT56       720
#define HEIGHT_FMT56      480
#define HSYNCOS_FMT56     (16-1)
#define VSYNCOS_FMT56     (9-1)
#define HSYNCW_FMT56      (HSYNCOS_FMT56 + 62)
#define VSYNCW_FMT56      (VSYNCOS_FMT56 + 6)
#define TVH0_FMT56        (HSYNCW_FMT56 + 60)
#define TVV0_FMT56        (VSYNCW_FMT56 + 30)
#define TVH1_FMT56        (TVH0_FMT56 + WIDTH_FMT56 - 1)
#define TVV1_FMT56        (TVV0_FMT56 + HEIGHT_FMT56 - 1)
#define HPIXEL_FMT56      (TVH1_FMT56 + 2)
#define VLINE_FMT56       (TVV1_FMT56 + 2)
#ifndef FPGA
#define CLK_NX_FMT56      CLK_1X_FMT56
#define CLK_1X_FMT56      108000
#define CLK_SCL_FMT56     200000
#define CLK_PIXEL_FMT56   CLK_1X_FMT56
#else
#define CLK_NX_DIV_FMT56  1
#define CLK_1X_DIV_FMT56  1
#endif

/* 720x480p @239.76/240Hz 16:9 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT57    "720X480P 240HZ 16:9"
#define DEV_TYPE_FMT57    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT57       DISP_PROGRESSIVE
#define FIELD_FMT57       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT57       720
#define HEIGHT_FMT57      480
#define HSYNCOS_FMT57     (16-1)
#define VSYNCOS_FMT57     (9-1)
#define HSYNCW_FMT57      (HSYNCOS_FMT57 + 62)
#define VSYNCW_FMT57      (VSYNCOS_FMT57 + 6)
#define TVH0_FMT57        (HSYNCW_FMT57 + 60)
#define TVV0_FMT57        (VSYNCW_FMT57 + 30)
#define TVH1_FMT57        (TVH0_FMT57 + WIDTH_FMT57 - 1)
#define TVV1_FMT57        (TVV0_FMT57 + HEIGHT_FMT57 - 1)
#define HPIXEL_FMT57      (TVH1_FMT57 + 2)
#define VLINE_FMT57       (TVV1_FMT57 + 2)
#ifndef FPGA
#define CLK_NX_FMT57      CLK_1X_FMT57
#define CLK_1X_FMT57      108000
#define CLK_SCL_FMT57     200000
#define CLK_PIXEL_FMT57   CLK_1X_FMT57
#else
#define CLK_NX_DIV_FMT57  1
#define CLK_1X_DIV_FMT57  1
#endif

/* 720(1440)x480i @239.76/240Hz 4:3 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT58    "720X480I 240HZ 4:3"
#define DEV_TYPE_FMT58    DISP_PANEL_DEVICE_TYPE_STD_INTERLACE_CCIR601
#define FRAME_FMT58       DISP_INTERLACE_NOSOF
#define FIELD_FMT58       DISP_SRC_FIELD_SWAP
#define WIDTH_FMT58       1440
#define HEIGHT_FMT58      (240+240)
#define HSYNCOS_FMT58     (38-1)
#define VSYNCOS_FMT58     ((4-1)+(4-1))
#define HSYNCW_FMT58      (HSYNCOS_FMT58 + 124)
#define VSYNCW_FMT58      (VSYNCOS_FMT58 + 3 + 3)
#define TVH0_FMT58        (HSYNCW_FMT58 + 114)
#define TVV0_FMT58        (VSYNCW_FMT58 + 15 + 16)
#define TVH1_FMT58        (TVH0_FMT58 + WIDTH_FMT58 - 1)
#define TVV1_FMT58        (TVV0_FMT58 + HEIGHT_FMT58 - 1)
#define HPIXEL_FMT58      (TVH1_FMT58 + 2)
#define VLINE_FMT58       (TVV1_FMT58 + 3)
#ifndef FPGA
#define CLK_NX_FMT58      CLK_1X_FMT58
#define CLK_1X_FMT58      108000
#define CLK_SCL_FMT58     200000
#define CLK_PIXEL_FMT58   CLK_1X_FMT58
#else
#define CLK_NX_DIV_FMT58  1
#define CLK_1X_DIV_FMT58  1
#endif

/* 720(1440)x480i @239.76/240Hz 16:9 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT59    "720X480I 240HZ 16:9"
#define DEV_TYPE_FMT59    DISP_PANEL_DEVICE_TYPE_STD_INTERLACE_CCIR601
#define FRAME_FMT59       DISP_INTERLACE_NOSOF
#define FIELD_FMT59       DISP_SRC_FIELD_SWAP
#define WIDTH_FMT59       1440
#define HEIGHT_FMT59      (240+240)
#define HSYNCOS_FMT59     (38-1)
#define VSYNCOS_FMT59     ((4-1)+(4-1))
#define HSYNCW_FMT59      (HSYNCOS_FMT59 + 124)
#define VSYNCW_FMT59      (VSYNCOS_FMT59 + 3 + 3)
#define TVH0_FMT59        (HSYNCW_FMT59 + 114)
#define TVV0_FMT59        (VSYNCW_FMT59 + 15 + 16)
#define TVH1_FMT59        (TVH0_FMT59 + WIDTH_FMT59 - 1)
#define TVV1_FMT59        (TVV0_FMT59 + HEIGHT_FMT59 - 1)
#define HPIXEL_FMT59      (TVH1_FMT59 + 2)
#define VLINE_FMT59       (TVV1_FMT59 + 3)
#ifndef FPGA
#define CLK_NX_FMT59      CLK_1X_FMT59
#define CLK_1X_FMT59      108000
#define CLK_SCL_FMT59     200000
#define CLK_PIXEL_FMT59   CLK_1X_FMT59
#else
#define CLK_NX_DIV_FMT59  1
#define CLK_1X_DIV_FMT59  1
#endif

/* 1280x720p @23.97/24Hz 16:9 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT60    "1280X720P 24HZ 16:9"
#define DEV_TYPE_FMT60    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT60       DISP_PROGRESSIVE
#define FIELD_FMT60       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT60       1280
#define HEIGHT_FMT60      720
#define HSYNCOS_FMT60     (1760-1)
#define VSYNCOS_FMT60     (5-1)
#define HSYNCW_FMT60      (HSYNCOS_FMT60 + 40)
#define VSYNCW_FMT60      (VSYNCOS_FMT60 + 5)
#define TVH0_FMT60        (HSYNCW_FMT60 + 220)
#define TVV0_FMT60        (VSYNCW_FMT60 + 20)
#define TVH1_FMT60        (TVH0_FMT60 + WIDTH_FMT60 - 1)
#define TVV1_FMT60        (TVV0_FMT60 + HEIGHT_FMT60 - 1)
#define HPIXEL_FMT60      (TVH1_FMT60 + 2)
#define VLINE_FMT60       (TVV1_FMT60 + 2)
#ifndef FPGA
#define CLK_NX_FMT60      CLK_1X_FMT60
#define CLK_1X_FMT60      59400
#define CLK_SCL_FMT60     148500
#define CLK_PIXEL_FMT60   CLK_1X_FMT60
#else
#define CLK_NX_DIV_FMT60  4
#define CLK_1X_DIV_FMT60  1
#endif

/* 1280x720p @25Hz 16:9 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT61    "1280X720P 25HZ 16:9"
#define DEV_TYPE_FMT61    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT61       DISP_PROGRESSIVE
#define FIELD_FMT61       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT61       1280
#define HEIGHT_FMT61      720
#define HSYNCOS_FMT61     (2420-1)
#define VSYNCOS_FMT61     (5-1)
#define HSYNCW_FMT61      (HSYNCOS_FMT61 + 40)
#define VSYNCW_FMT61      (VSYNCOS_FMT61 + 5)
#define TVH0_FMT61        (HSYNCW_FMT61 + 220)
#define TVV0_FMT61        (VSYNCW_FMT61 + 20)
#define TVH1_FMT61        (TVH0_FMT61 + WIDTH_FMT61 - 1)
#define TVV1_FMT61        (TVV0_FMT61 + HEIGHT_FMT61 - 1)
#define HPIXEL_FMT61      (TVH1_FMT61 + 2)
#define VLINE_FMT61       (TVV1_FMT61 + 2)
#ifndef FPGA
#define CLK_NX_FMT61      CLK_1X_FMT61
#define CLK_1X_FMT61      74250
#define CLK_SCL_FMT61     148500
#define CLK_PIXEL_FMT61   CLK_1X_FMT61
#else
#define CLK_NX_DIV_FMT61  4
#define CLK_1X_DIV_FMT61  1
#endif

/* 1280x720p @29.97/30Hz 16:9 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT62    "1280X720P 30HZ 16:9"
#define DEV_TYPE_FMT62    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT62       DISP_PROGRESSIVE
#define FIELD_FMT62       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT62       1280
#define HEIGHT_FMT62      720
#define HSYNCOS_FMT62     (1760-1)
#define VSYNCOS_FMT62     (5-1)
#define HSYNCW_FMT62      (HSYNCOS_FMT62 + 40)
#define VSYNCW_FMT62      (VSYNCOS_FMT62 + 5)
#define TVH0_FMT62        (HSYNCW_FMT62 + 220)
#define TVV0_FMT62        (VSYNCW_FMT62 + 20)
#define TVH1_FMT62        (TVH0_FMT62 + WIDTH_FMT62 - 1)
#define TVV1_FMT62        (TVV0_FMT62 + HEIGHT_FMT62 - 1)
#define HPIXEL_FMT62      (TVH1_FMT62 + 2)
#define VLINE_FMT62       (TVV1_FMT62 + 2)
#ifndef FPGA
#define CLK_NX_FMT62      CLK_1X_FMT62
#define CLK_1X_FMT62      74250
#define CLK_SCL_FMT62     148500
#define CLK_PIXEL_FMT62   CLK_1X_FMT62
#else
#define CLK_NX_DIV_FMT62  4
#define CLK_1X_DIV_FMT62  1
#endif

/* 1920x1080p @119.88/120Hz 16:9 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT63    "1920X1080P 120HZ 16:9"
#define DEV_TYPE_FMT63    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT63       DISP_PROGRESSIVE
#define FIELD_FMT63       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT63       1920
#define HEIGHT_FMT63      1080
#define HSYNCOS_FMT63     (88-1)
#define VSYNCOS_FMT63     (4-1)
#define HSYNCW_FMT63      (HSYNCOS_FMT63 + 44)
#define VSYNCW_FMT63      (VSYNCOS_FMT63 + 5)
#define TVH0_FMT63        (HSYNCW_FMT63 + 148)
#define TVV0_FMT63        (VSYNCW_FMT63 + 36)
#define TVH1_FMT63        (TVH0_FMT63 + WIDTH_FMT63 - 1)
#define TVV1_FMT63        (TVV0_FMT63 + HEIGHT_FMT63 - 1)
#define HPIXEL_FMT63      (TVH1_FMT63 + 2)
#define VLINE_FMT63       (TVV1_FMT63 + 2)
#ifndef FPGA
#define CLK_NX_FMT63      CLK_1X_FMT63
#define CLK_1X_FMT63      297000
#define CLK_SCL_FMT63     240000
#define CLK_PIXEL_FMT63   CLK_1X_FMT63
#else
#define CLK_NX_DIV_FMT63  4
#define CLK_1X_DIV_FMT63  1
#endif

/* 1920x1080p @100Hz 16:9 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT64    "1920X1080P 100HZ 16:9"
#define DEV_TYPE_FMT64    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT64       DISP_PROGRESSIVE
#define FIELD_FMT64       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT64       1920
#define HEIGHT_FMT64      1080
#define HSYNCOS_FMT64     (528-1)
#define VSYNCOS_FMT64     (4-1)
#define HSYNCW_FMT64      (HSYNCOS_FMT64 + 44)
#define VSYNCW_FMT64      (VSYNCOS_FMT64 + 5)
#define TVH0_FMT64        (HSYNCW_FMT64 + 148)
#define TVV0_FMT64        (VSYNCW_FMT64 + 36)
#define TVH1_FMT64        (TVH0_FMT64 + WIDTH_FMT64 - 1)
#define TVV1_FMT64        (TVV0_FMT64 + HEIGHT_FMT64 - 1)
#define HPIXEL_FMT64      (TVH1_FMT64 + 2)
#define VLINE_FMT64       (TVV1_FMT64 + 2)
#ifndef FPGA
#define CLK_NX_FMT64      CLK_1X_FMT64
#define CLK_1X_FMT64      297000
#define CLK_SCL_FMT64     240000
#define CLK_PIXEL_FMT64   CLK_1X_FMT64
#else
#define CLK_NX_DIV_FMT64  4
#define CLK_1X_DIV_FMT64  1
#endif

/* 1280x720p @23.97/24Hz 64:27 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT65    "1280X720P 24HZ 64:27"
#define DEV_TYPE_FMT65    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT65       DISP_PROGRESSIVE
#define FIELD_FMT65       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT65       1280
#define HEIGHT_FMT65      720
#define HSYNCOS_FMT65     (1760-1)
#define VSYNCOS_FMT65     (5-1)
#define HSYNCW_FMT65      (HSYNCOS_FMT65 + 40)
#define VSYNCW_FMT65      (VSYNCOS_FMT65 + 5)
#define TVH0_FMT65        (HSYNCW_FMT65 + 220)
#define TVV0_FMT65        (VSYNCW_FMT65 + 20)
#define TVH1_FMT65        (TVH0_FMT65 + WIDTH_FMT65 - 1)
#define TVV1_FMT65        (TVV0_FMT65 + HEIGHT_FMT65 - 1)
#define HPIXEL_FMT65      (TVH1_FMT65 + 2)
#define VLINE_FMT65       (TVV1_FMT65 + 2)
#ifndef FPGA
#define CLK_NX_FMT65      CLK_1X_FMT65
#define CLK_1X_FMT65      59400
#define CLK_SCL_FMT65     148500
#define CLK_PIXEL_FMT65   CLK_1X_FMT65
#else
#define CLK_NX_DIV_FMT65  4
#define CLK_1X_DIV_FMT65  1
#endif

/* 1280x720p @25Hz 64:27 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT66    "1280X720P 25HZ 64:27"
#define DEV_TYPE_FMT66    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT66       DISP_PROGRESSIVE
#define FIELD_FMT66       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT66       1280
#define HEIGHT_FMT66      720
#define HSYNCOS_FMT66     (2420-1)
#define VSYNCOS_FMT66     (5-1)
#define HSYNCW_FMT66      (HSYNCOS_FMT66 + 40)
#define VSYNCW_FMT66      (VSYNCOS_FMT66 + 5)
#define TVH0_FMT66        (HSYNCW_FMT66 + 220)
#define TVV0_FMT66        (VSYNCW_FMT66 + 20)
#define TVH1_FMT66        (TVH0_FMT66 + WIDTH_FMT66 - 1)
#define TVV1_FMT66        (TVV0_FMT66 + HEIGHT_FMT66 - 1)
#define HPIXEL_FMT66      (TVH1_FMT66 + 2)
#define VLINE_FMT66       (TVV1_FMT66 + 2)
#ifndef FPGA
#define CLK_NX_FMT66      CLK_1X_FMT66
#define CLK_1X_FMT66      74250
#define CLK_SCL_FMT66     148500
#define CLK_PIXEL_FMT66   CLK_1X_FMT66
#else
#define CLK_NX_DIV_FMT66  4
#define CLK_1X_DIV_FMT66  1
#endif

/* 1280x720p @29.97/30Hz 64:27 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT67    "1280X720P 30HZ 64:27"
#define DEV_TYPE_FMT67    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT67       DISP_PROGRESSIVE
#define FIELD_FMT67       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT67       1280
#define HEIGHT_FMT67      720
#define HSYNCOS_FMT67     (1760-1)
#define VSYNCOS_FMT67     (5-1)
#define HSYNCW_FMT67      (HSYNCOS_FMT67 + 40)
#define VSYNCW_FMT67      (VSYNCOS_FMT67 + 5)
#define TVH0_FMT67        (HSYNCW_FMT67 + 220)
#define TVV0_FMT67        (VSYNCW_FMT67 + 20)
#define TVH1_FMT67        (TVH0_FMT67 + WIDTH_FMT67 - 1)
#define TVV1_FMT67        (TVV0_FMT67 + HEIGHT_FMT67 - 1)
#define HPIXEL_FMT67      (TVH1_FMT67 + 2)
#define VLINE_FMT67       (TVV1_FMT67 + 2)
#ifndef FPGA
#define CLK_NX_FMT67      CLK_1X_FMT67
#define CLK_1X_FMT67      74250
#define CLK_SCL_FMT67     148500
#define CLK_PIXEL_FMT67   CLK_1X_FMT67
#else
#define CLK_NX_DIV_FMT67  4
#define CLK_1X_DIV_FMT67  1
#endif

/* 1280x720p @50Hz 64:27 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT68    "1280X720P 50HZ 64:27"
#define DEV_TYPE_FMT68    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT68       DISP_PROGRESSIVE
#define FIELD_FMT68       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT68       1280
#define HEIGHT_FMT68      720
#define HSYNCOS_FMT68     (440-1)
#define VSYNCOS_FMT68     (5-1)
#define HSYNCW_FMT68      (HSYNCOS_FMT68 + 40)
#define VSYNCW_FMT68      (VSYNCOS_FMT68 + 5)
#define TVH0_FMT68        (HSYNCW_FMT68 + 220)
#define TVV0_FMT68        (VSYNCW_FMT68 + 20)
#define TVH1_FMT68        (TVH0_FMT68 + WIDTH_FMT68 - 1)
#define TVV1_FMT68        (TVV0_FMT68 + HEIGHT_FMT68 - 1)
#define HPIXEL_FMT68      (TVH1_FMT68 + 2)
#define VLINE_FMT68       (TVV1_FMT68 + 2)
#ifndef FPGA
#define CLK_NX_FMT68      CLK_1X_FMT68
#define CLK_1X_FMT68      74250
#define CLK_SCL_FMT68     148500
#define CLK_PIXEL_FMT68   CLK_1X_FMT68
#else
#define CLK_NX_DIV_FMT68  4
#define CLK_1X_DIV_FMT68  1
#endif

/* 1280x720p @59.94/60Hz 64:27 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT69    "1280X720P 60HZ 64:27"
#define DEV_TYPE_FMT69    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT69       DISP_PROGRESSIVE
#define FIELD_FMT69       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT69       1280
#define HEIGHT_FMT69      720
#define HSYNCOS_FMT69     (110-1)
#define VSYNCOS_FMT69     (5-1)
#define HSYNCW_FMT69      (HSYNCOS_FMT69 + 40)
#define VSYNCW_FMT69      (VSYNCOS_FMT69 + 5)
#define TVH0_FMT69        (HSYNCW_FMT69 + 220)
#define TVV0_FMT69        (VSYNCW_FMT69 + 20)
#define TVH1_FMT69        (TVH0_FMT69 + WIDTH_FMT69 - 1)
#define TVV1_FMT69        (TVV0_FMT69 + HEIGHT_FMT69 - 1)
#define HPIXEL_FMT69      (TVH1_FMT69 + 2)
#define VLINE_FMT69       (TVV1_FMT69 + 2)
#ifndef FPGA
#define CLK_NX_FMT69      CLK_1X_FMT69
#define CLK_1X_FMT69      74250
#define CLK_SCL_FMT69     148500
#define CLK_PIXEL_FMT69   CLK_1X_FMT69
#else
#define CLK_NX_DIV_FMT69  4
#define CLK_1X_DIV_FMT69  1
#endif

/* 1280x720p @100Hz 64:27 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT70    "1280X720P 100HZ 64:27"
#define DEV_TYPE_FMT70    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT70       DISP_PROGRESSIVE
#define FIELD_FMT70       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT70       1280
#define HEIGHT_FMT70      720
#define HSYNCOS_FMT70     (440-1)
#define VSYNCOS_FMT70     (5-1)
#define HSYNCW_FMT70      (HSYNCOS_FMT70 + 40)
#define VSYNCW_FMT70      (VSYNCOS_FMT70 + 5)
#define TVH0_FMT70        (HSYNCW_FMT70 + 220)
#define TVV0_FMT70        (VSYNCW_FMT70 + 20)
#define TVH1_FMT70        (TVH0_FMT70 + WIDTH_FMT70 - 1)
#define TVV1_FMT70        (TVV0_FMT70 + HEIGHT_FMT70 - 1)
#define HPIXEL_FMT70      (TVH1_FMT70 + 2)
#define VLINE_FMT70       (TVV1_FMT70 + 2)
#ifndef FPGA
#define CLK_NX_FMT70      CLK_1X_FMT70
#define CLK_1X_FMT70      148500
#define CLK_SCL_FMT70     240000
#define CLK_PIXEL_FMT70   CLK_1X_FMT70
#else
#define CLK_NX_DIV_FMT70  4
#define CLK_1X_DIV_FMT70  1
#endif

/* 1280x720p @5119.88/120Hz 64:27 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT71    "1280X720P 120HZ 64:27"
#define DEV_TYPE_FMT71    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT71       DISP_PROGRESSIVE
#define FIELD_FMT71       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT71       1280
#define HEIGHT_FMT71      720
#define HSYNCOS_FMT71     (110-1)
#define VSYNCOS_FMT71     (5-1)
#define HSYNCW_FMT71      (HSYNCOS_FMT71 + 40)
#define VSYNCW_FMT71      (VSYNCOS_FMT71 + 5)
#define TVH0_FMT71        (HSYNCW_FMT71 + 220)
#define TVV0_FMT71        (VSYNCW_FMT71 + 20)
#define TVH1_FMT71        (TVH0_FMT71 + WIDTH_FMT71 - 1)
#define TVV1_FMT71        (TVV0_FMT71 + HEIGHT_FMT71 - 1)
#define HPIXEL_FMT71      (TVH1_FMT71 + 2)
#define VLINE_FMT71       (TVV1_FMT71 + 2)
#ifndef FPGA
#define CLK_NX_FMT71      CLK_1X_FMT71
#define CLK_1X_FMT71      148500
#define CLK_SCL_FMT71     240000
#define CLK_PIXEL_FMT71   CLK_1X_FMT71
#else
#define CLK_NX_DIV_FMT71  4
#define CLK_1X_DIV_FMT71  1
#endif

/* 1920x1080p @ 23.98/24Hz 64:27 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT72    "1920X1080P 24HZ 64:27"
#define DEV_TYPE_FMT72    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT72       DISP_PROGRESSIVE
#define FIELD_FMT72       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT72       1920
#define HEIGHT_FMT72      1080
#define HSYNCOS_FMT72     (638-1)
#define VSYNCOS_FMT72     (4-1)
#define HSYNCW_FMT72      (HSYNCOS_FMT72 + 44)
#define VSYNCW_FMT72      (VSYNCOS_FMT72 + 5)
#define TVH0_FMT72        (HSYNCW_FMT72 + 148)
#define TVV0_FMT72        (VSYNCW_FMT72 + 36)
#define TVH1_FMT72        (TVH0_FMT72 + WIDTH_FMT72 - 1)
#define TVV1_FMT72        (TVV0_FMT72 + HEIGHT_FMT72 - 1)
#define HPIXEL_FMT72      (TVH1_FMT72 + 2)
#define VLINE_FMT72       (TVV1_FMT72 + 2)
#ifndef FPGA
#define CLK_NX_FMT72      CLK_1X_FMT72
#define CLK_1X_FMT72      74250
#define CLK_SCL_FMT72     148500
#define CLK_PIXEL_FMT72   CLK_1X_FMT72
#else
#define CLK_NX_DIV_FMT72  4
#define CLK_1X_DIV_FMT72  1
#endif

/* 1920x1080p @ 23.98/24Hz 64:27 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT73    "1920X1080P 25HZ 64:27"
#define DEV_TYPE_FMT73    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT73       DISP_PROGRESSIVE
#define FIELD_FMT73       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT73       1920
#define HEIGHT_FMT73      1080
#define HSYNCOS_FMT73     (528-1)
#define VSYNCOS_FMT73     (4-1)
#define HSYNCW_FMT73      (HSYNCOS_FMT73 + 44)
#define VSYNCW_FMT73      (VSYNCOS_FMT73 + 5)
#define TVH0_FMT73        (HSYNCW_FMT73 + 148)
#define TVV0_FMT73        (VSYNCW_FMT73 + 36)
#define TVH1_FMT73        (TVH0_FMT73 + WIDTH_FMT73 - 1)
#define TVV1_FMT73        (TVV0_FMT73 + HEIGHT_FMT73 - 1)
#define HPIXEL_FMT73      (TVH1_FMT73 + 2)
#define VLINE_FMT73       (TVV1_FMT73 + 2)
#ifndef FPGA
#define CLK_NX_FMT73      CLK_1X_FMT73
#define CLK_1X_FMT73      74250
#define CLK_SCL_FMT73     148500
#define CLK_PIXEL_FMT73   CLK_1X_FMT73
#else
#define CLK_NX_DIV_FMT73  4
#define CLK_1X_DIV_FMT73  1
#endif

/* 1920x1080p @ 23.98/24Hz 64:27 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT74    "1920X1080P 30HZ 64:27"
#define DEV_TYPE_FMT74    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT74       DISP_PROGRESSIVE
#define FIELD_FMT74       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT74       1920
#define HEIGHT_FMT74      1080
#define HSYNCOS_FMT74     (88-1)
#define VSYNCOS_FMT74     (4-1)
#define HSYNCW_FMT74      (HSYNCOS_FMT74 + 44)
#define VSYNCW_FMT74      (VSYNCOS_FMT74 + 5)
#define TVH0_FMT74        (HSYNCW_FMT74 + 148)
#define TVV0_FMT74        (VSYNCW_FMT74 + 36)
#define TVH1_FMT74        (TVH0_FMT74 + WIDTH_FMT74 - 1)
#define TVV1_FMT74        (TVV0_FMT74 + HEIGHT_FMT74 - 1)
#define HPIXEL_FMT74      (TVH1_FMT74 + 2)
#define VLINE_FMT74       (TVV1_FMT74 + 2)
#ifndef FPGA
#define CLK_NX_FMT74      CLK_1X_FMT74
#define CLK_1X_FMT74      74250
#define CLK_SCL_FMT74     148500
#define CLK_PIXEL_FMT74   CLK_1X_FMT74
#else
#define CLK_NX_DIV_FMT74  4
#define CLK_1X_DIV_FMT74  1
#endif

/* 1920x1080p @50Hz 64:27 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT75    "1920X1080P 50HZ 64:27"
#define DEV_TYPE_FMT75    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT75       DISP_PROGRESSIVE
#define FIELD_FMT75       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT75       1920
#define HEIGHT_FMT75      1080
#define HSYNCOS_FMT75     (528-1)
#define VSYNCOS_FMT75     (4-1)
#define HSYNCW_FMT75      (HSYNCOS_FMT75 + 44)
#define VSYNCW_FMT75      (VSYNCOS_FMT75 + 5)
#define TVH0_FMT75        (HSYNCW_FMT75 + 148)
#define TVV0_FMT75        (VSYNCW_FMT75 + 36)
#define TVH1_FMT75        (TVH0_FMT75 + WIDTH_FMT75 - 1)
#define TVV1_FMT75        (TVV0_FMT75 + HEIGHT_FMT75 - 1)
#define HPIXEL_FMT75      (TVH1_FMT75 + 2)
#define VLINE_FMT75       (TVV1_FMT75 + 2)
#ifndef FPGA
#define CLK_NX_FMT75      CLK_1X_FMT75
#define CLK_1X_FMT75      148500
#define CLK_SCL_FMT75     240000
#define CLK_PIXEL_FMT75   CLK_1X_FMT75
#else
#define CLK_NX_DIV_FMT75  4
#define CLK_1X_DIV_FMT75  1
#endif

/* 1920x1080p @59.94/60Hz 64:27 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT76    "1920X1080P 60HZ 64:27"
#define DEV_TYPE_FMT76    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT76       DISP_PROGRESSIVE
#define FIELD_FMT76       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT76       1920
#define HEIGHT_FMT76      1080
#define HSYNCOS_FMT76     (88-1)
#define VSYNCOS_FMT76     (4-1)
#define HSYNCW_FMT76      (HSYNCOS_FMT76 + 44)
#define VSYNCW_FMT76      (VSYNCOS_FMT76 + 5)
#define TVH0_FMT76        (HSYNCW_FMT76 + 148)
#define TVV0_FMT76        (VSYNCW_FMT76 + 36)
#define TVH1_FMT76        (TVH0_FMT76 + WIDTH_FMT76 - 1)
#define TVV1_FMT76        (TVV0_FMT76 + HEIGHT_FMT76 - 1)
#define HPIXEL_FMT76      (TVH1_FMT76 + 2)
#define VLINE_FMT76       (TVV1_FMT76 + 2)
#ifndef FPGA
#define CLK_NX_FMT76      CLK_1X_FMT76
#define CLK_1X_FMT76      148500
#define CLK_SCL_FMT76     240000
#define CLK_PIXEL_FMT76   CLK_1X_FMT76
#else
#define CLK_NX_DIV_FMT76  4
#define CLK_1X_DIV_FMT76  1
#endif

/* 1920x1080p @100Hz 64:27 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT77    "1920X1080P 100HZ 64:27"
#define DEV_TYPE_FMT77    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT77       DISP_PROGRESSIVE
#define FIELD_FMT77       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT77       1920
#define HEIGHT_FMT77      1080
#define HSYNCOS_FMT77     (528-1)
#define VSYNCOS_FMT77     (4-1)
#define HSYNCW_FMT77      (HSYNCOS_FMT77 + 44)
#define VSYNCW_FMT77      (VSYNCOS_FMT77 + 5)
#define TVH0_FMT77        (HSYNCW_FMT77 + 148)
#define TVV0_FMT77        (VSYNCW_FMT77 + 36)
#define TVH1_FMT77        (TVH0_FMT77 + WIDTH_FMT77 - 1)
#define TVV1_FMT77        (TVV0_FMT77 + HEIGHT_FMT77 - 1)
#define HPIXEL_FMT77      (TVH1_FMT77 + 2)
#define VLINE_FMT77       (TVV1_FMT77 + 2)
#ifndef FPGA
#define CLK_NX_FMT77      CLK_1X_FMT77
#define CLK_1X_FMT77      297000
#define CLK_SCL_FMT77     240000
#define CLK_PIXEL_FMT77   CLK_1X_FMT77
#else
#define CLK_NX_DIV_FMT77  4
#define CLK_1X_DIV_FMT77  1
#endif

/* 1920x1080p @119.88/120Hz 64:27 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT78    "1920X1080P 120HZ 64:27"
#define DEV_TYPE_FMT78    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT78       DISP_PROGRESSIVE
#define FIELD_FMT78       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT78       1920
#define HEIGHT_FMT78      1080
#define HSYNCOS_FMT78     (88-1)
#define VSYNCOS_FMT78     (4-1)
#define HSYNCW_FMT78      (HSYNCOS_FMT78 + 44)
#define VSYNCW_FMT78      (VSYNCOS_FMT78 + 5)
#define TVH0_FMT78        (HSYNCW_FMT78 + 148)
#define TVV0_FMT78        (VSYNCW_FMT78 + 36)
#define TVH1_FMT78        (TVH0_FMT78 + WIDTH_FMT78 - 1)
#define TVV1_FMT78        (TVV0_FMT78 + HEIGHT_FMT78 - 1)
#define HPIXEL_FMT78      (TVH1_FMT78 + 2)
#define VLINE_FMT78       (TVV1_FMT78 + 2)
#ifndef FPGA
#define CLK_NX_FMT78      CLK_1X_FMT78
#define CLK_1X_FMT78      297000
#define CLK_SCL_FMT78     240000
#define CLK_PIXEL_FMT78   CLK_1X_FMT78
#else
#define CLK_NX_DIV_FMT78  4
#define CLK_1X_DIV_FMT78  1
#endif

/* 1680x720p @23.98/24Hz 64:27 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT79    "1680X720P 24HZ 64:27"
#define DEV_TYPE_FMT79    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT79       DISP_PROGRESSIVE
#define FIELD_FMT79       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT79       1680
#define HEIGHT_FMT79      720
#define HSYNCOS_FMT79     (1360-1)
#define VSYNCOS_FMT79     (5-1)
#define HSYNCW_FMT79      (HSYNCOS_FMT79 + 40)
#define VSYNCW_FMT79      (VSYNCOS_FMT79 + 5)
#define TVH0_FMT79        (HSYNCW_FMT79 + 220)
#define TVV0_FMT79        (VSYNCW_FMT79 + 20)
#define TVH1_FMT79        (TVH0_FMT79 + WIDTH_FMT79 - 1)
#define TVV1_FMT79        (TVV0_FMT79 + HEIGHT_FMT79 - 1)
#define HPIXEL_FMT79      (TVH1_FMT79 + 2)
#define VLINE_FMT79       (TVV1_FMT79 + 2)
#ifndef FPGA
#define CLK_NX_FMT79      CLK_1X_FMT79
#define CLK_1X_FMT79      59400
#define CLK_SCL_FMT79     148500
#define CLK_PIXEL_FMT79   CLK_1X_FMT79
#else
#define CLK_NX_DIV_FMT79  4
#define CLK_1X_DIV_FMT79  1
#endif

/* 1680x720p @25Hz 64:27 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT80    "1680X720P 25HZ 64:27"
#define DEV_TYPE_FMT80    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT80       DISP_PROGRESSIVE
#define FIELD_FMT80       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT80       1680
#define HEIGHT_FMT80      720
#define HSYNCOS_FMT80     (1228-1)
#define VSYNCOS_FMT80     (5-1)
#define HSYNCW_FMT80      (HSYNCOS_FMT80 + 40)
#define VSYNCW_FMT80      (VSYNCOS_FMT80 + 5)
#define TVH0_FMT80        (HSYNCW_FMT80 + 220)
#define TVV0_FMT80        (VSYNCW_FMT80 + 20)
#define TVH1_FMT80        (TVH0_FMT80 + WIDTH_FMT80 - 1)
#define TVV1_FMT80        (TVV0_FMT80 + HEIGHT_FMT80 - 1)
#define HPIXEL_FMT80      (TVH1_FMT80 + 2)
#define VLINE_FMT80       (TVV1_FMT80 + 2)
#ifndef FPGA
#define CLK_NX_FMT80      CLK_1X_FMT80
#define CLK_1X_FMT80      59400
#define CLK_SCL_FMT80     148500
#define CLK_PIXEL_FMT80   CLK_1X_FMT80
#else
#define CLK_NX_DIV_FMT80  4
#define CLK_1X_DIV_FMT80  1
#endif

/* 1680x720p @29.97/30Hz 64:27 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT81    "1680X720P 30HZ 64:27"
#define DEV_TYPE_FMT81    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT81       DISP_PROGRESSIVE
#define FIELD_FMT81       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT81       1680
#define HEIGHT_FMT81      720
#define HSYNCOS_FMT81     (700-1)
#define VSYNCOS_FMT81     (5-1)
#define HSYNCW_FMT81      (HSYNCOS_FMT81 + 40)
#define VSYNCW_FMT81      (VSYNCOS_FMT81 + 5)
#define TVH0_FMT81        (HSYNCW_FMT81 + 220)
#define TVV0_FMT81        (VSYNCW_FMT81 + 20)
#define TVH1_FMT81        (TVH0_FMT81 + WIDTH_FMT81 - 1)
#define TVV1_FMT81        (TVV0_FMT81 + HEIGHT_FMT81 - 1)
#define HPIXEL_FMT81      (TVH1_FMT81 + 2)
#define VLINE_FMT81       (TVV1_FMT81 + 2)
#ifndef FPGA
#define CLK_NX_FMT81      CLK_1X_FMT81
#define CLK_1X_FMT81      59400
#define CLK_SCL_FMT81     148500
#define CLK_PIXEL_FMT81   CLK_1X_FMT81
#else
#define CLK_NX_DIV_FMT81  4
#define CLK_1X_DIV_FMT81  1
#endif

/* 1680x720p @50Hz 64:27 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT82    "1680X720P 50HZ 64:27"
#define DEV_TYPE_FMT82    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT82       DISP_PROGRESSIVE
#define FIELD_FMT82       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT82       1680
#define HEIGHT_FMT82      720
#define HSYNCOS_FMT82     (260-1)
#define VSYNCOS_FMT82     (5-1)
#define HSYNCW_FMT82      (HSYNCOS_FMT82 + 40)
#define VSYNCW_FMT82      (VSYNCOS_FMT82 + 5)
#define TVH0_FMT82        (HSYNCW_FMT82 + 220)
#define TVV0_FMT82        (VSYNCW_FMT82 + 20)
#define TVH1_FMT82        (TVH0_FMT82 + WIDTH_FMT82 - 1)
#define TVV1_FMT82        (TVV0_FMT82 + HEIGHT_FMT82 - 1)
#define HPIXEL_FMT82      (TVH1_FMT82 + 2)
#define VLINE_FMT82       (TVV1_FMT82 + 2)
#ifndef FPGA
#define CLK_NX_FMT82      CLK_1X_FMT82
#define CLK_1X_FMT82      82500
#define CLK_SCL_FMT82     240000
#define CLK_PIXEL_FMT82   CLK_1X_FMT82
#else
#define CLK_NX_DIV_FMT82  4
#define CLK_1X_DIV_FMT82  1
#endif

/* 1680x720p @59.94/60Hz 64:27 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT83    "1680X720P 60HZ 64:27"
#define DEV_TYPE_FMT83    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT83       DISP_PROGRESSIVE
#define FIELD_FMT83       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT83       1680
#define HEIGHT_FMT83      720
#define HSYNCOS_FMT83     (260-1)
#define VSYNCOS_FMT83     (5-1)
#define HSYNCW_FMT83      (HSYNCOS_FMT83 + 40)
#define VSYNCW_FMT83      (VSYNCOS_FMT83 + 5)
#define TVH0_FMT83        (HSYNCW_FMT83 + 220)
#define TVV0_FMT83        (VSYNCW_FMT83 + 20)
#define TVH1_FMT83        (TVH0_FMT83 + WIDTH_FMT83 - 1)
#define TVV1_FMT83        (TVV0_FMT83 + HEIGHT_FMT83 - 1)
#define HPIXEL_FMT83      (TVH1_FMT83 + 2)
#define VLINE_FMT83       (TVV1_FMT83 + 2)
#ifndef FPGA
#define CLK_NX_FMT83      CLK_1X_FMT83
#define CLK_1X_FMT83      99000
#define CLK_SCL_FMT83     240000
#define CLK_PIXEL_FMT83   CLK_1X_FMT83
#else
#define CLK_NX_DIV_FMT83  4
#define CLK_1X_DIV_FMT83  1
#endif

/* 1680x720p @100Hz 64:27 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT84    "1680X720P 100HZ 64:27"
#define DEV_TYPE_FMT84    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT84       DISP_PROGRESSIVE
#define FIELD_FMT84       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT84       1680
#define HEIGHT_FMT84      720
#define HSYNCOS_FMT84     (60-1)
#define VSYNCOS_FMT84     (5-1)
#define HSYNCW_FMT84      (HSYNCOS_FMT84 + 40)
#define VSYNCW_FMT84      (VSYNCOS_FMT84 + 5)
#define TVH0_FMT84        (HSYNCW_FMT84 + 220)
#define TVV0_FMT84        (VSYNCW_FMT84 + 95)
#define TVH1_FMT84        (TVH0_FMT84 + WIDTH_FMT84 - 1)
#define TVV1_FMT84        (TVV0_FMT84 + HEIGHT_FMT84 - 1)
#define HPIXEL_FMT84      (TVH1_FMT84 + 2)
#define VLINE_FMT84       (TVV1_FMT84 + 2)
#ifndef FPGA
#define CLK_NX_FMT84      CLK_1X_FMT84
#define CLK_1X_FMT84      165000
#define CLK_SCL_FMT84     240000
#define CLK_PIXEL_FMT84   CLK_1X_FMT84
#else
#define CLK_NX_DIV_FMT84  4
#define CLK_1X_DIV_FMT84  1
#endif

/* 1680x720p @119.88/120Hz 64:27 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT85    "1680X720P 120HZ 64:27"
#define DEV_TYPE_FMT85    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT85       DISP_PROGRESSIVE
#define FIELD_FMT85       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT85       1680
#define HEIGHT_FMT85      720
#define HSYNCOS_FMT85     (60-1)
#define VSYNCOS_FMT85     (5-1)
#define HSYNCW_FMT85      (HSYNCOS_FMT85 + 40)
#define VSYNCW_FMT85      (VSYNCOS_FMT85 + 5)
#define TVH0_FMT85        (HSYNCW_FMT85 + 220)
#define TVV0_FMT85        (VSYNCW_FMT85 + 95)
#define TVH1_FMT85        (TVH0_FMT85 + WIDTH_FMT85 - 1)
#define TVV1_FMT85        (TVV0_FMT85 + HEIGHT_FMT85 - 1)
#define HPIXEL_FMT85      (TVH1_FMT85 + 2)
#define VLINE_FMT85       (TVV1_FMT85 + 2)
#ifndef FPGA
#define CLK_NX_FMT85      CLK_1X_FMT85
#define CLK_1X_FMT85      198000
#define CLK_SCL_FMT85     240000
#define CLK_PIXEL_FMT85   CLK_1X_FMT85
#else
#define CLK_NX_DIV_FMT85  4
#define CLK_1X_DIV_FMT85  1
#endif

/* 2560x1080p @23.98/24Hz 64:27 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT86    "2560X1080P 24HZ 64:27"
#define DEV_TYPE_FMT86    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT86       DISP_PROGRESSIVE
#define FIELD_FMT86       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT86       2560
#define HEIGHT_FMT86      1080
#define HSYNCOS_FMT86     (998-1)
#define VSYNCOS_FMT86     (4-1)
#define HSYNCW_FMT86      (HSYNCOS_FMT86 + 44)
#define VSYNCW_FMT86      (VSYNCOS_FMT86 + 5)
#define TVH0_FMT86        (HSYNCW_FMT86 + 148)
#define TVV0_FMT86        (VSYNCW_FMT86 + 11)
#define TVH1_FMT86        (TVH0_FMT86 + WIDTH_FMT86 - 1)
#define TVV1_FMT86        (TVV0_FMT86 + HEIGHT_FMT86 - 1)
#define HPIXEL_FMT86      (TVH1_FMT86 + 2)
#define VLINE_FMT86       (TVV1_FMT86 + 2)
#ifndef FPGA
#define CLK_NX_FMT86      CLK_1X_FMT86
#define CLK_1X_FMT86      99000
#define CLK_SCL_FMT86     240000
#define CLK_PIXEL_FMT86   CLK_1X_FMT86
#else
#define CLK_NX_DIV_FMT86  4
#define CLK_1X_DIV_FMT86  1
#endif

/* 2560x1080p @25Hz 64:27 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT87    "2560X1080P 25HZ 64:27"
#define DEV_TYPE_FMT87    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT87       DISP_PROGRESSIVE
#define FIELD_FMT87       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT87       2560
#define HEIGHT_FMT87      1080
#define HSYNCOS_FMT87     (448-1)
#define VSYNCOS_FMT87     (4-1)
#define HSYNCW_FMT87      (HSYNCOS_FMT87 + 44)
#define VSYNCW_FMT87      (VSYNCOS_FMT87 + 5)
#define TVH0_FMT87        (HSYNCW_FMT87 + 148)
#define TVV0_FMT87        (VSYNCW_FMT87 + 36)
#define TVH1_FMT87        (TVH0_FMT87 + WIDTH_FMT87 - 1)
#define TVV1_FMT87        (TVV0_FMT87 + HEIGHT_FMT87 - 1)
#define HPIXEL_FMT87      (TVH1_FMT87 + 2)
#define VLINE_FMT87       (TVV1_FMT87 + 2)
#ifndef FPGA
#define CLK_NX_FMT87      CLK_1X_FMT87
#define CLK_1X_FMT87      90000
#define CLK_SCL_FMT87     240000
#define CLK_PIXEL_FMT87   CLK_1X_FMT87
#else
#define CLK_NX_DIV_FMT87  4
#define CLK_1X_DIV_FMT87  1
#endif

/* 2560x1080p @29.97/30Hz 64:27 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT88    "2560X1080P 30HZ 64:27"
#define DEV_TYPE_FMT88    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT88       DISP_PROGRESSIVE
#define FIELD_FMT88       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT88       2560
#define HEIGHT_FMT88      1080
#define HSYNCOS_FMT88     (768-1)
#define VSYNCOS_FMT88     (4-1)
#define HSYNCW_FMT88      (HSYNCOS_FMT88 + 44)
#define VSYNCW_FMT88      (VSYNCOS_FMT88 + 5)
#define TVH0_FMT88        (HSYNCW_FMT88 + 148)
#define TVV0_FMT88        (VSYNCW_FMT88 + 36)
#define TVH1_FMT88        (TVH0_FMT88 + WIDTH_FMT88 - 1)
#define TVV1_FMT88        (TVV0_FMT88 + HEIGHT_FMT88 - 1)
#define HPIXEL_FMT88      (TVH1_FMT88 + 2)
#define VLINE_FMT88       (TVV1_FMT88 + 2)
#ifndef FPGA
#define CLK_NX_FMT88      CLK_1X_FMT88
#define CLK_1X_FMT88      118000
#define CLK_SCL_FMT88     240000
#define CLK_PIXEL_FMT88   CLK_1X_FMT88
#else
#define CLK_NX_DIV_FMT88  4
#define CLK_1X_DIV_FMT88  1
#endif

/* 2560x1080p @50Hz 64:27 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT89    "2560X1080P 50HZ 64:27"
#define DEV_TYPE_FMT89    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT89       DISP_PROGRESSIVE
#define FIELD_FMT89       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT89       2560
#define HEIGHT_FMT89      1080
#define HSYNCOS_FMT89     (548-1)
#define VSYNCOS_FMT89     (4-1)
#define HSYNCW_FMT89      (HSYNCOS_FMT89 + 44)
#define VSYNCW_FMT89      (VSYNCOS_FMT89 + 5)
#define TVH0_FMT89        (HSYNCW_FMT89 + 148)
#define TVV0_FMT89        (VSYNCW_FMT89 + 36)
#define TVH1_FMT89        (TVH0_FMT89 + WIDTH_FMT89 - 1)
#define TVV1_FMT89        (TVV0_FMT89 + HEIGHT_FMT89 - 1)
#define HPIXEL_FMT89      (TVH1_FMT89 + 2)
#define VLINE_FMT89       (TVV1_FMT89 + 2)
#ifndef FPGA
#define CLK_NX_FMT89      CLK_1X_FMT89
#define CLK_1X_FMT89      185625
#define CLK_SCL_FMT89     240000
#define CLK_PIXEL_FMT89   CLK_1X_FMT89
#else
#define CLK_NX_DIV_FMT89  4
#define CLK_1X_DIV_FMT89  1
#endif

/* 2560x1080p @59.94/60Hz 64:27 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT90    "2560X1080P 60HZ 64:27"
#define DEV_TYPE_FMT90    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT90       DISP_PROGRESSIVE
#define FIELD_FMT90       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT90       2560
#define HEIGHT_FMT90      1080
#define HSYNCOS_FMT90     (248-1)
#define VSYNCOS_FMT90     (4-1)
#define HSYNCW_FMT90      (HSYNCOS_FMT90 + 44)
#define VSYNCW_FMT90      (VSYNCOS_FMT90 + 5)
#define TVH0_FMT90        (HSYNCW_FMT90 + 148)
#define TVV0_FMT90        (VSYNCW_FMT90 + 11)
#define TVH1_FMT90        (TVH0_FMT90 + WIDTH_FMT90 - 1)
#define TVV1_FMT90        (TVV0_FMT90 + HEIGHT_FMT90 - 1)
#define HPIXEL_FMT90      (TVH1_FMT90 + 2)
#define VLINE_FMT90       (TVV1_FMT90 + 2)
#ifndef FPGA
#define CLK_NX_FMT90      CLK_1X_FMT90
#define CLK_1X_FMT90      198000
#define CLK_SCL_FMT90     240000
#define CLK_PIXEL_FMT90   CLK_1X_FMT90
#else
#define CLK_NX_DIV_FMT90  4
#define CLK_1X_DIV_FMT90  1
#endif

/* 2560x1080p @100Hz 64:27 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT91    "2560X1080P 100HZ 64:27"
#define DEV_TYPE_FMT91    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT91       DISP_PROGRESSIVE
#define FIELD_FMT91       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT91       2560
#define HEIGHT_FMT91      1080
#define HSYNCOS_FMT91     (218-1)
#define VSYNCOS_FMT91     (4-1)
#define HSYNCW_FMT91      (HSYNCOS_FMT91 + 44)
#define VSYNCW_FMT91      (VSYNCOS_FMT91 + 5)
#define TVH0_FMT91        (HSYNCW_FMT91 + 148)
#define TVV0_FMT91        (VSYNCW_FMT91 + 161)
#define TVH1_FMT91        (TVH0_FMT91 + WIDTH_FMT91 - 1)
#define TVV1_FMT91        (TVV0_FMT91 + HEIGHT_FMT91 - 1)
#define HPIXEL_FMT91      (TVH1_FMT91 + 2)
#define VLINE_FMT91       (TVV1_FMT91 + 2)
#ifndef FPGA
#define CLK_NX_FMT91      CLK_1X_FMT91
#define CLK_1X_FMT91      371250
#define CLK_SCL_FMT91     240000
#define CLK_PIXEL_FMT91   CLK_1X_FMT91
#else
#define CLK_NX_DIV_FMT91  4
#define CLK_1X_DIV_FMT91  1
#endif

/* 2560x1080p @119.88/120Hz 64:27 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT92    "2560X1080P 120HZ 64:27"
#define DEV_TYPE_FMT92    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT92       DISP_PROGRESSIVE
#define FIELD_FMT92       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT92       2560
#define HEIGHT_FMT92      1080
#define HSYNCOS_FMT92     (548-1)
#define VSYNCOS_FMT92     (4-1)
#define HSYNCW_FMT92      (HSYNCOS_FMT92 + 44)
#define VSYNCW_FMT92      (VSYNCOS_FMT92 + 5)
#define TVH0_FMT92        (HSYNCW_FMT92 + 148)
#define TVV0_FMT92        (VSYNCW_FMT92 + 161)
#define TVH1_FMT92        (TVH0_FMT92 + WIDTH_FMT92 - 1)
#define TVV1_FMT92        (TVV0_FMT92 + HEIGHT_FMT92 - 1)
#define HPIXEL_FMT92      (TVH1_FMT92 + 2)
#define VLINE_FMT92       (TVV1_FMT92 + 2)
#ifndef FPGA
#define CLK_NX_FMT92      CLK_1X_FMT92
#define CLK_1X_FMT92      495000
#define CLK_SCL_FMT92     240000
#define CLK_PIXEL_FMT92   CLK_1X_FMT92
#else
#define CLK_NX_DIV_FMT92  4
#define CLK_1X_DIV_FMT92  1
#endif

/* 3840x2160p @23.98/24Hz 16:9 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT93    "3840X2160P 24HZ 16:9"
#define DEV_TYPE_FMT93    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT93       DISP_PROGRESSIVE
#define FIELD_FMT93       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT93       3840
#define HEIGHT_FMT93      2160
#define HSYNCOS_FMT93     (1276-1)
#define VSYNCOS_FMT93     (8-1)
#define HSYNCW_FMT93      (HSYNCOS_FMT93 + 88)
#define VSYNCW_FMT93      (VSYNCOS_FMT93 + 10)
#define TVH0_FMT93        (HSYNCW_FMT93 + 296)
#define TVV0_FMT93        (VSYNCW_FMT93 + 72)
#define TVH1_FMT93        (TVH0_FMT93 + WIDTH_FMT93 - 1)
#define TVV1_FMT93        (TVV0_FMT93 + HEIGHT_FMT93 - 1)
#define HPIXEL_FMT93      (TVH1_FMT93 + 2)
#define VLINE_FMT93       (TVV1_FMT93 + 2)
#ifndef FPGA
#define CLK_NX_FMT93      CLK_1X_FMT93
#define CLK_1X_FMT93      297000
#define CLK_SCL_FMT93     240000
#define CLK_PIXEL_FMT93   CLK_1X_FMT93
#else
#define CLK_NX_DIV_FMT93  4
#define CLK_1X_DIV_FMT93  1
#endif

/* 3840x2160p @25Hz 16:9 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT94    "3840X2160P 25HZ 16:9"
#define DEV_TYPE_FMT94    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT94       DISP_PROGRESSIVE
#define FIELD_FMT94       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT94       3840
#define HEIGHT_FMT94      2160
#define HSYNCOS_FMT94     (1056-1)
#define VSYNCOS_FMT94     (8-1)
#define HSYNCW_FMT94      (HSYNCOS_FMT94 + 88)
#define VSYNCW_FMT94      (VSYNCOS_FMT94 + 10)
#define TVH0_FMT94        (HSYNCW_FMT94 + 296)
#define TVV0_FMT94        (VSYNCW_FMT94 + 72)
#define TVH1_FMT94        (TVH0_FMT94 + WIDTH_FMT94 - 1)
#define TVV1_FMT94        (TVV0_FMT94 + HEIGHT_FMT94 - 1)
#define HPIXEL_FMT94      (TVH1_FMT94 + 2)
#define VLINE_FMT94       (TVV1_FMT94 + 2)
#ifndef FPGA
#define CLK_NX_FMT94      CLK_1X_FMT94
#define CLK_1X_FMT94      297000
#define CLK_SCL_FMT94     240000
#define CLK_PIXEL_FMT94   CLK_1X_FMT94
#else
#define CLK_NX_DIV_FMT94  4
#define CLK_1X_DIV_FMT94  1
#endif

/* 3840x2160p @29.97/30Hz 16:9 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT95    "3840X2160P 30HZ 16:9"
#define DEV_TYPE_FMT95    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT95       DISP_PROGRESSIVE
#define FIELD_FMT95       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT95       3840
#define HEIGHT_FMT95      2160
#define HSYNCOS_FMT95     (176-1)
#define VSYNCOS_FMT95     (8-1)
#define HSYNCW_FMT95      (HSYNCOS_FMT95 + 88)
#define VSYNCW_FMT95      (VSYNCOS_FMT95 + 10)
#define TVH0_FMT95        (HSYNCW_FMT95 + 296)
#define TVV0_FMT95        (VSYNCW_FMT95 + 72)
#define TVH1_FMT95        (TVH0_FMT95 + WIDTH_FMT95 - 1)
#define TVV1_FMT95        (TVV0_FMT95 + HEIGHT_FMT95 - 1)
#define HPIXEL_FMT95      (TVH1_FMT95 + 2)
#define VLINE_FMT95       (TVV1_FMT95 + 2)
#ifndef FPGA
#define CLK_NX_FMT95      CLK_1X_FMT95
#define CLK_1X_FMT95      297000
#define CLK_SCL_FMT95     240000
#define CLK_PIXEL_FMT95   CLK_1X_FMT95
#else
#define CLK_NX_DIV_FMT95  4
#define CLK_1X_DIV_FMT95  1
#endif

/* 3840x2160p @50Hz 16:9 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT96    "3840X2160P 50HZ 16:9"
#define DEV_TYPE_FMT96    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT96       DISP_PROGRESSIVE
#define FIELD_FMT96       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT96       3840
#define HEIGHT_FMT96      2160
#define HSYNCOS_FMT96     (1056-1)
#define VSYNCOS_FMT96     (8-1)
#define HSYNCW_FMT96      (HSYNCOS_FMT96 + 88)
#define VSYNCW_FMT96      (VSYNCOS_FMT96 + 10)
#define TVH0_FMT96        (HSYNCW_FMT96 + 296)
#define TVV0_FMT96        (VSYNCW_FMT96 + 72)
#define TVH1_FMT96        (TVH0_FMT96 + WIDTH_FMT96 - 1)
#define TVV1_FMT96        (TVV0_FMT96 + HEIGHT_FMT96 - 1)
#define HPIXEL_FMT96      (TVH1_FMT96 + 2)
#define VLINE_FMT96       (TVV1_FMT96 + 2)
#ifndef FPGA
#define CLK_NX_FMT96      CLK_1X_FMT96
#define CLK_1X_FMT96      594000
#define CLK_SCL_FMT96     240000
#define CLK_PIXEL_FMT96   CLK_1X_FMT96
#else
#define CLK_NX_DIV_FMT96  4
#define CLK_1X_DIV_FMT96  1
#endif

/* 3840x2160p @59.94/60Hz 16:9 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT97    "3840X2160P 60HZ 16:9"
#define DEV_TYPE_FMT97    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT97       DISP_PROGRESSIVE
#define FIELD_FMT97       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT97       3840
#define HEIGHT_FMT97      2160
#define HSYNCOS_FMT97     (176-1)
#define VSYNCOS_FMT97     (8-1)
#define HSYNCW_FMT97      (HSYNCOS_FMT97 + 88)
#define VSYNCW_FMT97      (VSYNCOS_FMT97 + 10)
#define TVH0_FMT97        (HSYNCW_FMT97 + 296)
#define TVV0_FMT97        (VSYNCW_FMT97 + 72)
#define TVH1_FMT97        (TVH0_FMT97 + WIDTH_FMT97 - 1)
#define TVV1_FMT97        (TVV0_FMT97 + HEIGHT_FMT97 - 1)
#define HPIXEL_FMT97      (TVH1_FMT97 + 2)
#define VLINE_FMT97       (TVV1_FMT97 + 2)
#ifndef FPGA
#define CLK_NX_FMT97      CLK_1X_FMT97
#define CLK_1X_FMT97      594000
#define CLK_SCL_FMT97     240000
#define CLK_PIXEL_FMT97   CLK_1X_FMT97
#else
#define CLK_NX_DIV_FMT97  4
#define CLK_1X_DIV_FMT97  1
#endif

/* 4096x2160p @23.98/24Hz 256:135 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT98    "4096X2160P 24HZ 256:135"
#define DEV_TYPE_FMT98    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT98       DISP_PROGRESSIVE
#define FIELD_FMT98       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT98       4096
#define HEIGHT_FMT98      2160
#define HSYNCOS_FMT98     (1020-1)
#define VSYNCOS_FMT98     (8-1)
#define HSYNCW_FMT98      (HSYNCOS_FMT98 + 88)
#define VSYNCW_FMT98      (VSYNCOS_FMT98 + 10)
#define TVH0_FMT98        (HSYNCW_FMT98 + 296)
#define TVV0_FMT98        (VSYNCW_FMT98 + 72)
#define TVH1_FMT98        (TVH0_FMT98 + WIDTH_FMT98 - 1)
#define TVV1_FMT98        (TVV0_FMT98 + HEIGHT_FMT98 - 1)
#define HPIXEL_FMT98      (TVH1_FMT98 + 2)
#define VLINE_FMT98       (TVV1_FMT98 + 2)
#ifndef FPGA
#define CLK_NX_FMT98      CLK_1X_FMT98
#define CLK_1X_FMT98      297000
#define CLK_SCL_FMT98     240000
#define CLK_PIXEL_FMT98   CLK_1X_FMT98
#else
#define CLK_NX_DIV_FMT98  4
#define CLK_1X_DIV_FMT98  1
#endif

/* 4096x2160p @25Hz 256:135 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT99    "4096X2160P 25HZ 256:135"
#define DEV_TYPE_FMT99    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT99       DISP_PROGRESSIVE
#define FIELD_FMT99       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT99       4096
#define HEIGHT_FMT99      2160
#define HSYNCOS_FMT99     (968-1)
#define VSYNCOS_FMT99     (8-1)
#define HSYNCW_FMT99      (HSYNCOS_FMT99 + 88)
#define VSYNCW_FMT99      (VSYNCOS_FMT99 + 10)
#define TVH0_FMT99        (HSYNCW_FMT99 + 128)
#define TVV0_FMT99        (VSYNCW_FMT99 + 72)
#define TVH1_FMT99        (TVH0_FMT99 + WIDTH_FMT99 - 1)
#define TVV1_FMT99        (TVV0_FMT99 + HEIGHT_FMT99 - 1)
#define HPIXEL_FMT99      (TVH1_FMT99 + 2)
#define VLINE_FMT99       (TVV1_FMT99 + 2)
#ifndef FPGA
#define CLK_NX_FMT99      CLK_1X_FMT99
#define CLK_1X_FMT99      297000
#define CLK_SCL_FMT99     240000
#define CLK_PIXEL_FMT99   CLK_1X_FMT99
#else
#define CLK_NX_DIV_FMT99  4
#define CLK_1X_DIV_FMT99  1
#endif

/* 4096x2160p @29.97/30Hz 256:135 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT100    "4096X2160P 30HZ 256:135"
#define DEV_TYPE_FMT100    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT100       DISP_PROGRESSIVE
#define FIELD_FMT100       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT100       4096
#define HEIGHT_FMT100      2160
#define HSYNCOS_FMT100     (88-1)
#define VSYNCOS_FMT100     (8-1)
#define HSYNCW_FMT100      (HSYNCOS_FMT100 + 88)
#define VSYNCW_FMT100      (VSYNCOS_FMT100 + 10)
#define TVH0_FMT100        (HSYNCW_FMT100 + 128)
#define TVV0_FMT100        (VSYNCW_FMT100 + 72)
#define TVH1_FMT100        (TVH0_FMT100 + WIDTH_FMT100 - 1)
#define TVV1_FMT100        (TVV0_FMT100 + HEIGHT_FMT100 - 1)
#define HPIXEL_FMT100      (TVH1_FMT100 + 2)
#define VLINE_FMT100       (TVV1_FMT100 + 2)
#ifndef FPGA
#define CLK_NX_FMT100      CLK_1X_FMT100
#define CLK_1X_FMT100      297000
#define CLK_SCL_FMT100     240000
#define CLK_PIXEL_FMT100   CLK_1X_FMT100
#else
#define CLK_NX_DIV_FMT100  4
#define CLK_1X_DIV_FMT100  1
#endif

/* 4096x2160p @50Hz 256:135 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT101    "4096X2160P 50HZ 256:135"
#define DEV_TYPE_FMT101    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT101       DISP_PROGRESSIVE
#define FIELD_FMT101       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT101       4096
#define HEIGHT_FMT101      2160
#define HSYNCOS_FMT101     (968-1)
#define VSYNCOS_FMT101     (8-1)
#define HSYNCW_FMT101      (HSYNCOS_FMT101 + 88)
#define VSYNCW_FMT101      (VSYNCOS_FMT101 + 10)
#define TVH0_FMT101        (HSYNCW_FMT101 + 128)
#define TVV0_FMT101        (VSYNCW_FMT101 + 72)
#define TVH1_FMT101        (TVH0_FMT101 + WIDTH_FMT101 - 1)
#define TVV1_FMT101        (TVV0_FMT101 + HEIGHT_FMT101 - 1)
#define HPIXEL_FMT101      (TVH1_FMT101 + 2)
#define VLINE_FMT101       (TVV1_FMT101 + 2)
#ifndef FPGA
#define CLK_NX_FMT101      CLK_1X_FMT101
#define CLK_1X_FMT101      594000
#define CLK_SCL_FMT101     240000
#define CLK_PIXEL_FMT101   CLK_1X_FMT101
#else
#define CLK_NX_DIV_FMT101  4
#define CLK_1X_DIV_FMT101  1
#endif

/* 4096x2160p @59.94/60Hz 256:135 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT102    "4096X2160P 60HZ 256:135"
#define DEV_TYPE_FMT102    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT102       DISP_PROGRESSIVE
#define FIELD_FMT102       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT102       4096
#define HEIGHT_FMT102      2160
#define HSYNCOS_FMT102     (88-1)
#define VSYNCOS_FMT102     (8-1)
#define HSYNCW_FMT102      (HSYNCOS_FMT102 + 88)
#define VSYNCW_FMT102      (VSYNCOS_FMT102 + 10)
#define TVH0_FMT102        (HSYNCW_FMT102 + 128)
#define TVV0_FMT102        (VSYNCW_FMT102 + 72)
#define TVH1_FMT102        (TVH0_FMT102 + WIDTH_FMT102 - 1)
#define TVV1_FMT102        (TVV0_FMT102 + HEIGHT_FMT102 - 1)
#define HPIXEL_FMT102      (TVH1_FMT102 + 2)
#define VLINE_FMT102       (TVV1_FMT102 + 2)
#ifndef FPGA
#define CLK_NX_FMT102      CLK_1X_FMT102
#define CLK_1X_FMT102      594000
#define CLK_SCL_FMT102     240000
#define CLK_PIXEL_FMT102   CLK_1X_FMT102
#else
#define CLK_NX_DIV_FMT102  4
#define CLK_1X_DIV_FMT102  1
#endif

/* 3840x2160p @23.98/24Hz 64:27 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT103    "3840X2160P 24HZ 64:27"
#define DEV_TYPE_FMT103    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT103       DISP_PROGRESSIVE
#define FIELD_FMT103       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT103       3840
#define HEIGHT_FMT103      2160
#define HSYNCOS_FMT103     (1276-1)
#define VSYNCOS_FMT103     (8-1)
#define HSYNCW_FMT103      (HSYNCOS_FMT103 + 88)
#define VSYNCW_FMT103      (VSYNCOS_FMT103 + 10)
#define TVH0_FMT103        (HSYNCW_FMT103 + 296)
#define TVV0_FMT103        (VSYNCW_FMT103 + 72)
#define TVH1_FMT103        (TVH0_FMT103 + WIDTH_FMT103 - 1)
#define TVV1_FMT103        (TVV0_FMT103 + HEIGHT_FMT103 - 1)
#define HPIXEL_FMT103      (TVH1_FMT103 + 2)
#define VLINE_FMT103       (TVV1_FMT103 + 2)
#ifndef FPGA
#define CLK_NX_FMT103      CLK_1X_FMT103
#define CLK_1X_FMT103      297000
#define CLK_SCL_FMT103     240000
#define CLK_PIXEL_FMT103   CLK_1X_FMT103
#else
#define CLK_NX_DIV_FMT103  4
#define CLK_1X_DIV_FMT103  1
#endif

/* 3840x2160p @25Hz 64:27 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT104    "3840X2160P 25HZ 64:27"
#define DEV_TYPE_FMT104    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT104       DISP_PROGRESSIVE
#define FIELD_FMT104       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT104       3840
#define HEIGHT_FMT104      2160
#define HSYNCOS_FMT104     (1056-1)
#define VSYNCOS_FMT104     (8-1)
#define HSYNCW_FMT104      (HSYNCOS_FMT104 + 88)
#define VSYNCW_FMT104      (VSYNCOS_FMT104 + 10)
#define TVH0_FMT104        (HSYNCW_FMT104 + 296)
#define TVV0_FMT104        (VSYNCW_FMT104 + 72)
#define TVH1_FMT104        (TVH0_FMT104 + WIDTH_FMT104 - 1)
#define TVV1_FMT104        (TVV0_FMT104 + HEIGHT_FMT104 - 1)
#define HPIXEL_FMT104      (TVH1_FMT104 + 2)
#define VLINE_FMT104       (TVV1_FMT104 + 2)
#ifndef FPGA
#define CLK_NX_FMT104      CLK_1X_FMT104
#define CLK_1X_FMT104      297000
#define CLK_SCL_FMT104     240000
#define CLK_PIXEL_FMT104   CLK_1X_FMT104
#else
#define CLK_NX_DIV_FMT104  4
#define CLK_1X_DIV_FMT104  1
#endif

/* 3840x2160p @29.97/30Hz 64:27 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT105    "3840X2160P 30HZ 64:27"
#define DEV_TYPE_FMT105    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT105       DISP_PROGRESSIVE
#define FIELD_FMT105       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT105       3840
#define HEIGHT_FMT105      2160
#define HSYNCOS_FMT105     (176-1)
#define VSYNCOS_FMT105     (8-1)
#define HSYNCW_FMT105      (HSYNCOS_FMT105 + 88)
#define VSYNCW_FMT105      (VSYNCOS_FMT105 + 10)
#define TVH0_FMT105        (HSYNCW_FMT105 + 296)
#define TVV0_FMT105        (VSYNCW_FMT105 + 72)
#define TVH1_FMT105        (TVH0_FMT105 + WIDTH_FMT105 - 1)
#define TVV1_FMT105        (TVV0_FMT105 + HEIGHT_FMT105 - 1)
#define HPIXEL_FMT105      (TVH1_FMT105 + 2)
#define VLINE_FMT105       (TVV1_FMT105 + 2)
#ifndef FPGA
#define CLK_NX_FMT105      CLK_1X_FMT105
#define CLK_1X_FMT105      297000
#define CLK_SCL_FMT105     240000
#define CLK_PIXEL_FMT105   CLK_1X_FMT105
#else
#define CLK_NX_DIV_FMT105  4
#define CLK_1X_DIV_FMT105  1
#endif

/* 3840x2160p @50Hz 64:27 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT106    "3840X2160P 50HZ 64:27"
#define DEV_TYPE_FMT106    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT106       DISP_PROGRESSIVE
#define FIELD_FMT106       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT106       3840
#define HEIGHT_FMT106      2160
#define HSYNCOS_FMT106     (1056-1)
#define VSYNCOS_FMT106     (8-1)
#define HSYNCW_FMT106      (HSYNCOS_FMT106 + 88)
#define VSYNCW_FMT106      (VSYNCOS_FMT106 + 10)
#define TVH0_FMT106        (HSYNCW_FMT106 + 296)
#define TVV0_FMT106        (VSYNCW_FMT106 + 72)
#define TVH1_FMT106        (TVH0_FMT106 + WIDTH_FMT106 - 1)
#define TVV1_FMT106        (TVV0_FMT106 + HEIGHT_FMT106 - 1)
#define HPIXEL_FMT106      (TVH1_FMT106 + 2)
#define VLINE_FMT106       (TVV1_FMT106 + 2)
#ifndef FPGA
#define CLK_NX_FMT106      CLK_1X_FMT106
#define CLK_1X_FMT106      594000
#define CLK_SCL_FMT106     240000
#define CLK_PIXEL_FMT106   CLK_1X_FMT106
#else
#define CLK_NX_DIV_FMT106  4
#define CLK_1X_DIV_FMT106  1
#endif

/* 3840x2160p @59.94/60Hz 64:27 (EIA-CEA-861-F Table 1, Table 2, Table 3). */
#define DEV_NAME_FMT107    "3840X2160P 60HZ 64:27"
#define DEV_TYPE_FMT107    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT107       DISP_PROGRESSIVE
#define FIELD_FMT107       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT107       3840
#define HEIGHT_FMT107      2160
#define HSYNCOS_FMT107     (176-1)
#define VSYNCOS_FMT107     (8-1)
#define HSYNCW_FMT107      (HSYNCOS_FMT107 + 88)
#define VSYNCW_FMT107      (VSYNCOS_FMT107 + 10)
#define TVH0_FMT107        (HSYNCW_FMT107 + 296)
#define TVV0_FMT107        (VSYNCW_FMT107 + 72)
#define TVH1_FMT107        (TVH0_FMT107 + WIDTH_FMT107 - 1)
#define TVV1_FMT107        (TVV0_FMT107 + HEIGHT_FMT107 - 1)
#define HPIXEL_FMT107      (TVH1_FMT107 + 2)
#define VLINE_FMT107       (TVV1_FMT107 + 2)
#ifndef FPGA
#define CLK_NX_FMT107      CLK_1X_FMT107
#define CLK_1X_FMT107      594000
#define CLK_SCL_FMT107     240000
#define CLK_PIXEL_FMT107   CLK_1X_FMT107
#else
#define CLK_NX_DIV_FMT107  4
#define CLK_1X_DIV_FMT107  1
#endif

/* HDMI_Specification_Rev1.4 section 8.2.3.2 3D video format structure (HDMI1.4 Table 8-15/Figure 8-7). */
#define DEV_TYPE_FMT4_3D_FP   DEV_TYPE_FMT4
#define FRAME_FMT4_3D_FP      FRAME_FMT4
#define FIELD_FMT4_3D_FP      FIELD_FMT4
#define WIDTH_FMT4_3D_FP      WIDTH_FMT4
#define HEIGHT_FMT4_3D_FP     HEIGHT_FMT4
#define VACT_SPC_FMT4_3D_FP   30
#define HSYNCOS_FMT4_3D_FP    HSYNCOS_FMT4
#define VSYNCOS_FMT4_3D_FP    VSYNCOS_FMT4
#define HSYNCW_FMT4_3D_FP     HSYNCW_FMT4
#define VSYNCW_FMT4_3D_FP     VSYNCW_FMT4
#define TVH0_FMT4_3D_FP       TVH0_FMT4
#define TVV0_FMT4_3D_FP       TVV0_FMT4
#define SPCV0_FMT4_3D_FP      (TVV0_FMT4_3D_FP + HEIGHT_FMT4_3D_FP)
#define SPCV1_FMT4_3D_FP      (SPCV0_FMT4_3D_FP + VACT_SPC_FMT4_3D_FP)  /* SPCV0 <= active space < SPCV1 */
#define TVH1_FMT4_3D_FP       (TVH0_FMT4_3D_FP + WIDTH_FMT4_3D_FP - 1)
#define TVV1_FMT4_3D_FP       (SPCV1_FMT4_3D_FP + HEIGHT_FMT4_3D_FP - 1)
#define HPIXEL_FMT4_3D_FP     HPIXEL_FMT4
#define VLINE_FMT4_3D_FP      (VLINE_FMT4 + VACT_SPC_FMT4_3D_FP + HEIGHT_FMT4_3D_FP)
#define CLK_NX_FMT4_3D_FP     CLK_1X_FMT4_3D_FP
#define CLK_1X_FMT4_3D_FP     148500
#define CLK_SCL_FMT4_3D_FP    240000
#define CLK_PIXEL_FMT4_3D_FP  CLK_1X_FMT4_3D_FP

/* HDMI_Specification_Rev1.4 section 8.2.3.2 3D video format structure (HDMI1.4 Table 8-15/Figure 8-7). */
#define DEV_TYPE_FMT19_3D_FP   DEV_TYPE_FMT19
#define FRAME_FMT19_3D_FP      FRAME_FMT19
#define FIELD_FMT19_3D_FP      FIELD_FMT19
#define WIDTH_FMT19_3D_FP      WIDTH_FMT19
#define HEIGHT_FMT19_3D_FP     HEIGHT_FMT19
#define VACT_SPC_FMT19_3D_FP   30
#define HSYNCOS_FMT19_3D_FP    HSYNCOS_FMT19
#define VSYNCOS_FMT19_3D_FP    VSYNCOS_FMT19
#define HSYNCW_FMT19_3D_FP     HSYNCW_FMT19
#define VSYNCW_FMT19_3D_FP     VSYNCW_FMT19
#define TVH0_FMT19_3D_FP       TVH0_FMT19
#define TVV0_FMT19_3D_FP       TVV0_FMT19
#define SPCV0_FMT19_3D_FP      (TVV0_FMT19_3D_FP + HEIGHT_FMT19_3D_FP)
#define SPCV1_FMT19_3D_FP      (SPCV0_FMT19_3D_FP + VACT_SPC_FMT19_3D_FP)  /* SPCV0 <= active space < SPCV1 */
#define TVH1_FMT19_3D_FP       (TVH0_FMT19_3D_FP + WIDTH_FMT19_3D_FP - 1)
#define TVV1_FMT19_3D_FP       (SPCV1_FMT19_3D_FP + HEIGHT_FMT19_3D_FP - 1)
#define HPIXEL_FMT19_3D_FP     HPIXEL_FMT19
#define VLINE_FMT19_3D_FP      (VLINE_FMT19 + VACT_SPC_FMT19_3D_FP + HEIGHT_FMT19_3D_FP)
#define CLK_NX_FMT19_3D_FP     CLK_1X_FMT19_3D_FP
#define CLK_1X_FMT19_3D_FP     148500
#define CLK_SCL_FMT19_3D_FP    240000
#define CLK_PIXEL_FMT19_3D_FP  CLK_1X_FMT19_3D_FP

/* HDMI_Specification_Rev1.4 section 8.2.3.2 3D video format structure (HDMI1.4 Table 8-15/Figure 8-7). */
#define DEV_TYPE_FMT32_3D_FP   DEV_TYPE_FMT32
#define FRAME_FMT32_3D_FP      FRAME_FMT32
#define FIELD_FMT32_3D_FP      FIELD_FMT32
#define WIDTH_FMT32_3D_FP      WIDTH_FMT32
#define HEIGHT_FMT32_3D_FP     HEIGHT_FMT32
#define VACT_SPC_FMT32_3D_FP   45
#define HSYNCOS_FMT32_3D_FP    HSYNCOS_FMT32
#define VSYNCOS_FMT32_3D_FP    VSYNCOS_FMT32
#define HSYNCW_FMT32_3D_FP     HSYNCW_FMT32
#define VSYNCW_FMT32_3D_FP     VSYNCW_FMT32
#define TVH0_FMT32_3D_FP       TVH0_FMT32
#define TVV0_FMT32_3D_FP       TVV0_FMT32
#define SPCV0_FMT32_3D_FP      (TVV0_FMT32_3D_FP + HEIGHT_FMT32_3D_FP)
#define SPCV1_FMT32_3D_FP      (SPCV0_FMT32_3D_FP + VACT_SPC_FMT32_3D_FP)  /* SPCV0 <= active space < SPCV1 */
#define TVH1_FMT32_3D_FP       (TVH0_FMT32_3D_FP + WIDTH_FMT32_3D_FP - 1)
#define TVV1_FMT32_3D_FP       (SPCV1_FMT32_3D_FP + HEIGHT_FMT32_3D_FP - 1)
#define HPIXEL_FMT32_3D_FP     HPIXEL_FMT32
#define VLINE_FMT32_3D_FP      (VLINE_FMT32 + VACT_SPC_FMT32_3D_FP + HEIGHT_FMT32_3D_FP)
#define CLK_NX_FMT32_3D_FP     CLK_1X_FMT32_3D_FP
#define CLK_1X_FMT32_3D_FP     148500
#define CLK_SCL_FMT32_3D_FP    240000
#define CLK_PIXEL_FMT32_3D_FP  CLK_1X_FMT32_3D_FP

/* HDMI_Specification_Rev1.4 section 8.2.3.2 3D video format structure (HDMI1.4 Table 8-15/Figure 8-7). */
#define DEV_TYPE_FMT33_3D_FP   DEV_TYPE_FMT33
#define FRAME_FMT33_3D_FP      FRAME_FMT33
#define FIELD_FMT33_3D_FP      FIELD_FMT33
#define WIDTH_FMT33_3D_FP      WIDTH_FMT33
#define HEIGHT_FMT33_3D_FP     HEIGHT_FMT33
#define VACT_SPC_FMT33_3D_FP   45
#define HSYNCOS_FMT33_3D_FP    HSYNCOS_FMT33
#define VSYNCOS_FMT33_3D_FP    VSYNCOS_FMT33
#define HSYNCW_FMT33_3D_FP     HSYNCW_FMT33
#define VSYNCW_FMT33_3D_FP     VSYNCW_FMT33
#define TVH0_FMT33_3D_FP       TVH0_FMT33
#define TVV0_FMT33_3D_FP       TVV0_FMT33
#define SPCV0_FMT33_3D_FP      (TVV0_FMT33_3D_FP + HEIGHT_FMT33_3D_FP)
#define SPCV1_FMT33_3D_FP      (SPCV0_FMT33_3D_FP + VACT_SPC_FMT33_3D_FP)  /* SPCV0 <= active space < SPCV1 */
#define TVH1_FMT33_3D_FP       (TVH0_FMT33_3D_FP + WIDTH_FMT33_3D_FP - 1)
#define TVV1_FMT33_3D_FP       (SPCV1_FMT33_3D_FP + HEIGHT_FMT33_3D_FP - 1)
#define HPIXEL_FMT33_3D_FP     HPIXEL_FMT33
#define VLINE_FMT33_3D_FP      (VLINE_FMT33 + VACT_SPC_FMT33_3D_FP + HEIGHT_FMT33_3D_FP)
#define CLK_NX_FMT33_3D_FP     CLK_1X_FMT33_3D_FP
#define CLK_1X_FMT33_3D_FP     148500
#define CLK_SCL_FMT33_3D_FP    240000
#define CLK_PIXEL_FMT33_3D_FP  CLK_1X_FMT33_3D_FP

/* HDMI_Specification_Rev1.4 section 8.2.3.2 3D video format structure (HDMI1.4 Table 8-15/Figure 8-7). */
#define DEV_TYPE_FMT34_3D_FP   DEV_TYPE_FMT34
#define FRAME_FMT34_3D_FP      FRAME_FMT34
#define FIELD_FMT34_3D_FP      FIELD_FMT34
#define WIDTH_FMT34_3D_FP      WIDTH_FMT34
#define HEIGHT_FMT34_3D_FP     HEIGHT_FMT34
#define VACT_SPC_FMT34_3D_FP   45
#define HSYNCOS_FMT34_3D_FP    HSYNCOS_FMT34
#define VSYNCOS_FMT34_3D_FP    VSYNCOS_FMT34
#define HSYNCW_FMT34_3D_FP     HSYNCW_FMT34
#define VSYNCW_FMT34_3D_FP     VSYNCW_FMT34
#define TVH0_FMT34_3D_FP       TVH0_FMT34
#define TVV0_FMT34_3D_FP       TVV0_FMT34
#define SPCV0_FMT34_3D_FP      (TVV0_FMT34_3D_FP + HEIGHT_FMT34_3D_FP)
#define SPCV1_FMT34_3D_FP      (SPCV0_FMT34_3D_FP + VACT_SPC_FMT34_3D_FP)  /* SPCV0 <= active space < SPCV1 */
#define TVH1_FMT34_3D_FP       (TVH0_FMT34_3D_FP + WIDTH_FMT34_3D_FP - 1)
#define TVV1_FMT34_3D_FP       (SPCV1_FMT34_3D_FP + HEIGHT_FMT34_3D_FP - 1)
#define HPIXEL_FMT34_3D_FP     HPIXEL_FMT34
#define VLINE_FMT34_3D_FP      (VLINE_FMT34 + VACT_SPC_FMT34_3D_FP + HEIGHT_FMT34_3D_FP)
#define CLK_NX_FMT34_3D_FP     CLK_1X_FMT34_3D_FP
#define CLK_1X_FMT34_3D_FP     148500
#define CLK_SCL_FMT34_3D_FP    240000
#define CLK_PIXEL_FMT34_3D_FP  CLK_1X_FMT34_3D_FP

/* HDMI_Specification_Rev1.4 section 8.2.3.2 3D video format structure (HDMI1.4 Table 8-15/Figure 8-7). */
#define DEV_TYPE_FMT4_3D_TAB   DEV_TYPE_FMT4
#define FRAME_FMT4_3D_TAB      FRAME_FMT4
#define FIELD_FMT4_3D_TAB      FIELD_FMT4
#define WIDTH_FMT4_3D_TAB      WIDTH_FMT4
#define HEIGHT_FMT4_3D_TAB     HEIGHT_FMT4
#define HSYNCOS_FMT4_3D_TAB    HSYNCOS_FMT4
#define VSYNCOS_FMT4_3D_TAB    VSYNCOS_FMT4
#define HSYNCW_FMT4_3D_TAB     HSYNCW_FMT4
#define VSYNCW_FMT4_3D_TAB     VSYNCW_FMT4
#define TVH0_FMT4_3D_TAB       TVH0_FMT4
#define TVV0_FMT4_3D_TAB       TVV0_FMT4
#define TVH1_FMT4_3D_TAB       (TVH0_FMT4_3D_TAB + WIDTH_FMT4_3D_TAB - 1)
#define TVV1_FMT4_3D_TAB       (TVV0_FMT4_3D_TAB + HEIGHT_FMT4_3D_TAB - 1)
#define HPIXEL_FMT4_3D_TAB     HPIXEL_FMT4
#define VLINE_FMT4_3D_TAB      VLINE_FMT4
#define CLK_NX_FMT4_3D_TAB     CLK_1X_FMT4_3D_TAB
#define CLK_1X_FMT4_3D_TAB     CLK_1X_FMT4
#define CLK_SCL_FMT4_3D_TAB    240000
#define CLK_PIXEL_FMT4_3D_TAB  CLK_1X_FMT4_3D_TAB

/* HDMI_Specification_Rev1.4 section 8.2.3.2 3D video format structure (HDMI1.4 Table 8-15/Figure 8-7). */
#define DEV_TYPE_FMT16_3D_TAB   DEV_TYPE_FMT16
#define FRAME_FMT16_3D_TAB      FRAME_FMT16
#define FIELD_FMT16_3D_TAB      FIELD_FMT16
#define WIDTH_FMT16_3D_TAB      WIDTH_FMT16
#define HEIGHT_FMT16_3D_TAB     HEIGHT_FMT16
#define HSYNCOS_FMT16_3D_TAB    HSYNCOS_FMT16
#define VSYNCOS_FMT16_3D_TAB    VSYNCOS_FMT16
#define HSYNCW_FMT16_3D_TAB     HSYNCW_FMT16
#define VSYNCW_FMT16_3D_TAB     VSYNCW_FMT16
#define TVH0_FMT16_3D_TAB       TVH0_FMT16
#define TVV0_FMT16_3D_TAB       TVV0_FMT16
#define TVH1_FMT16_3D_TAB       (TVH0_FMT16_3D_TAB + WIDTH_FMT16_3D_TAB - 1)
#define TVV1_FMT16_3D_TAB       (TVV0_FMT16_3D_TAB + HEIGHT_FMT16_3D_TAB - 1)
#define HPIXEL_FMT16_3D_TAB     HPIXEL_FMT16
#define VLINE_FMT16_3D_TAB      VLINE_FMT16
#define CLK_NX_FMT16_3D_TAB     CLK_1X_FMT16_3D_TAB
#define CLK_1X_FMT16_3D_TAB     CLK_1X_FMT16
#define CLK_SCL_FMT16_3D_TAB    240000
#define CLK_PIXEL_FMT16_3D_TAB  CLK_1X_FMT16_3D_TAB

/* HDMI_Specification_Rev1.4 section 8.2.3.2 3D video format structure (HDMI1.4 Table 8-15/Figure 8-7). */
#define DEV_TYPE_FMT19_3D_TAB   DEV_TYPE_FMT19
#define FRAME_FMT19_3D_TAB      FRAME_FMT19
#define FIELD_FMT19_3D_TAB      FIELD_FMT19
#define WIDTH_FMT19_3D_TAB      WIDTH_FMT19
#define HEIGHT_FMT19_3D_TAB     HEIGHT_FMT19
#define HSYNCOS_FMT19_3D_TAB    HSYNCOS_FMT19
#define VSYNCOS_FMT19_3D_TAB    VSYNCOS_FMT19
#define HSYNCW_FMT19_3D_TAB     HSYNCW_FMT19
#define VSYNCW_FMT19_3D_TAB     VSYNCW_FMT19
#define TVH0_FMT19_3D_TAB       TVH0_FMT19
#define TVV0_FMT19_3D_TAB       TVV0_FMT19
#define TVH1_FMT19_3D_TAB       (TVH0_FMT19_3D_TAB + WIDTH_FMT19_3D_TAB - 1)
#define TVV1_FMT19_3D_TAB       (TVV0_FMT19_3D_TAB + HEIGHT_FMT19_3D_TAB - 1)
#define HPIXEL_FMT19_3D_TAB     HPIXEL_FMT19
#define VLINE_FMT19_3D_TAB      VLINE_FMT19
#define CLK_NX_FMT19_3D_TAB     CLK_1X_FMT19_3D_TAB
#define CLK_1X_FMT19_3D_TAB     CLK_1X_FMT19
#define CLK_SCL_FMT19_3D_TAB    240000
#define CLK_PIXEL_FMT19_3D_TAB  CLK_1X_FMT19_3D_TAB

/* HDMI_Specification_Rev1.4 section 8.2.3.2 3D video format structure (HDMI1.4 Table 8-15/Figure 8-7). */
#define DEV_TYPE_FMT31_3D_TAB   DEV_TYPE_FMT31
#define FRAME_FMT31_3D_TAB      FRAME_FMT31
#define FIELD_FMT31_3D_TAB      FIELD_FMT31
#define WIDTH_FMT31_3D_TAB      WIDTH_FMT31
#define HEIGHT_FMT31_3D_TAB     HEIGHT_FMT31
#define HSYNCOS_FMT31_3D_TAB    HSYNCOS_FMT31
#define VSYNCOS_FMT31_3D_TAB    VSYNCOS_FMT31
#define HSYNCW_FMT31_3D_TAB     HSYNCW_FMT31
#define VSYNCW_FMT31_3D_TAB     VSYNCW_FMT31
#define TVH0_FMT31_3D_TAB       TVH0_FMT31
#define TVV0_FMT31_3D_TAB       TVV0_FMT31
#define TVH1_FMT31_3D_TAB       (TVH0_FMT31_3D_TAB + WIDTH_FMT31_3D_TAB - 1)
#define TVV1_FMT31_3D_TAB       (TVV0_FMT31_3D_TAB + HEIGHT_FMT31_3D_TAB - 1)
#define HPIXEL_FMT31_3D_TAB     HPIXEL_FMT31
#define VLINE_FMT31_3D_TAB      VLINE_FMT31
#define CLK_NX_FMT31_3D_TAB     CLK_1X_FMT31_3D_TAB
#define CLK_1X_FMT31_3D_TAB     CLK_1X_FMT31
#define CLK_SCL_FMT31_3D_TAB    240000
#define CLK_PIXEL_FMT31_3D_TAB  CLK_1X_FMT31_3D_TAB

/* HDMI_Specification_Rev1.4 section 8.2.3.2 3D video format structure (HDMI1.4 Table 8-15/Figure 8-7). */
#define DEV_TYPE_FMT32_3D_TAB   DEV_TYPE_FMT32
#define FRAME_FMT32_3D_TAB      FRAME_FMT32
#define FIELD_FMT32_3D_TAB      FIELD_FMT32
#define WIDTH_FMT32_3D_TAB      WIDTH_FMT32
#define HEIGHT_FMT32_3D_TAB     HEIGHT_FMT32
#define HSYNCOS_FMT32_3D_TAB    HSYNCOS_FMT32
#define VSYNCOS_FMT32_3D_TAB    VSYNCOS_FMT32
#define HSYNCW_FMT32_3D_TAB     HSYNCW_FMT32
#define VSYNCW_FMT32_3D_TAB     VSYNCW_FMT32
#define TVH0_FMT32_3D_TAB       TVH0_FMT32
#define TVV0_FMT32_3D_TAB       TVV0_FMT32
#define TVH1_FMT32_3D_TAB       (TVH0_FMT32_3D_TAB + WIDTH_FMT32_3D_TAB - 1)
#define TVV1_FMT32_3D_TAB       (TVV0_FMT32_3D_TAB + HEIGHT_FMT32_3D_TAB - 1)
#define HPIXEL_FMT32_3D_TAB     HPIXEL_FMT32
#define VLINE_FMT32_3D_TAB      VLINE_FMT32
#define CLK_NX_FMT32_3D_TAB     CLK_1X_FMT32_3D_TAB
#define CLK_1X_FMT32_3D_TAB     CLK_1X_FMT32
#define CLK_SCL_FMT32_3D_TAB    240000
#define CLK_PIXEL_FMT32_3D_TAB  CLK_1X_FMT32_3D_TAB

/* HDMI_Specification_Rev1.4 section 8.2.3.2 3D video format structure (HDMI1.4 Table 8-15/Figure 8-7). */
#define DEV_TYPE_FMT4_3D_SBSHHSS   DEV_TYPE_FMT4
#define FRAME_FMT4_3D_SBSHHSS      FRAME_FMT4
#define FIELD_FMT4_3D_SBSHHSS      FIELD_FMT4
#define WIDTH_FMT4_3D_SBSHHSS      WIDTH_FMT4
#define HEIGHT_FMT4_3D_SBSHHSS     HEIGHT_FMT4
#define HSYNCOS_FMT4_3D_SBSHHSS    HSYNCOS_FMT4
#define VSYNCOS_FMT4_3D_SBSHHSS    VSYNCOS_FMT4
#define HSYNCW_FMT4_3D_SBSHHSS     HSYNCW_FMT4
#define VSYNCW_FMT4_3D_SBSHHSS     VSYNCW_FMT4
#define TVH0_FMT4_3D_SBSHHSS       TVH0_FMT4
#define TVV0_FMT4_3D_SBSHHSS       TVV0_FMT4
#define TVH1_FMT4_3D_SBSHHSS       (TVH0_FMT4_3D_SBSHHSS + WIDTH_FMT4_3D_SBSHHSS - 1)
#define TVV1_FMT4_3D_SBSHHSS       (TVV0_FMT4_3D_SBSHHSS + HEIGHT_FMT4_3D_SBSHHSS - 1)
#define HPIXEL_FMT4_3D_SBSHHSS     HPIXEL_FMT4
#define VLINE_FMT4_3D_SBSHHSS      VLINE_FMT4
#define CLK_NX_FMT4_3D_SBSHHSS     CLK_1X_FMT4_3D_SBSHHSS
#define CLK_1X_FMT4_3D_SBSHHSS     CLK_1X_FMT4
#define CLK_SCL_FMT4_3D_SBSHHSS    240000
#define CLK_PIXEL_FMT4_3D_SBSHHSS  CLK_1X_FMT4_3D_SBSHHSS

/* HDMI_Specification_Rev1.4 section 8.2.3.2 3D video format structure (HDMI1.4 Table 8-15/Figure 8-7). */
#define DEV_TYPE_FMT5_3D_SBSHHSS   DEV_TYPE_FMT5
#define FRAME_FMT5_3D_SBSHHSS      FRAME_FMT5
#define FIELD_FMT5_3D_SBSHHSS      FIELD_FMT5
#define WIDTH_FMT5_3D_SBSHHSS      WIDTH_FMT5
#define HEIGHT_FMT5_3D_SBSHHSS     HEIGHT_FMT5
#define HSYNCOS_FMT5_3D_SBSHHSS    HSYNCOS_FMT5
#define VSYNCOS_FMT5_3D_SBSHHSS    VSYNCOS_FMT5
#define HSYNCW_FMT5_3D_SBSHHSS     HSYNCW_FMT5
#define VSYNCW_FMT5_3D_SBSHHSS     VSYNCW_FMT5
#define TVH0_FMT5_3D_SBSHHSS       TVH0_FMT5
#define TVV0_FMT5_3D_SBSHHSS       TVV0_FMT5
#define TVH1_FMT5_3D_SBSHHSS       (TVH0_FMT5_3D_SBSHHSS + WIDTH_FMT5_3D_SBSHHSS - 1)
#define TVV1_FMT5_3D_SBSHHSS       (TVV0_FMT5_3D_SBSHHSS + HEIGHT_FMT5_3D_SBSHHSS - 1)
#define HPIXEL_FMT5_3D_SBSHHSS     HPIXEL_FMT5
#define VLINE_FMT5_3D_SBSHHSS      VLINE_FMT5
#define CLK_NX_FMT5_3D_SBSHHSS     CLK_1X_FMT5_3D_SBSHHSS
#define CLK_1X_FMT5_3D_SBSHHSS     CLK_1X_FMT5
#define CLK_SCL_FMT5_3D_SBSHHSS    240000
#define CLK_PIXEL_FMT5_3D_SBSHHSS  CLK_1X_FMT5_3D_SBSHHSS

/* HDMI_Specification_Rev1.4 section 8.2.3.2 3D video format structure (HDMI1.4 Table 8-15/Figure 8-7). */
#define DEV_TYPE_FMT19_3D_SBSHHSS   DEV_TYPE_FMT19
#define FRAME_FMT19_3D_SBSHHSS      FRAME_FMT19
#define FIELD_FMT19_3D_SBSHHSS      FIELD_FMT19
#define WIDTH_FMT19_3D_SBSHHSS      WIDTH_FMT19
#define HEIGHT_FMT19_3D_SBSHHSS     HEIGHT_FMT19
#define HSYNCOS_FMT19_3D_SBSHHSS    HSYNCOS_FMT19
#define VSYNCOS_FMT19_3D_SBSHHSS    VSYNCOS_FMT19
#define HSYNCW_FMT19_3D_SBSHHSS     HSYNCW_FMT19
#define VSYNCW_FMT19_3D_SBSHHSS     VSYNCW_FMT19
#define TVH0_FMT19_3D_SBSHHSS       TVH0_FMT19
#define TVV0_FMT19_3D_SBSHHSS       TVV0_FMT19
#define TVH1_FMT19_3D_SBSHHSS       (TVH0_FMT19_3D_SBSHHSS + WIDTH_FMT19_3D_SBSHHSS - 1)
#define TVV1_FMT19_3D_SBSHHSS       (TVV0_FMT19_3D_SBSHHSS + HEIGHT_FMT19_3D_SBSHHSS - 1)
#define HPIXEL_FMT19_3D_SBSHHSS     HPIXEL_FMT19
#define VLINE_FMT19_3D_SBSHHSS      VLINE_FMT19
#define CLK_NX_FMT19_3D_SBSHHSS     CLK_1X_FMT19_3D_SBSHHSS
#define CLK_1X_FMT19_3D_SBSHHSS     CLK_1X_FMT19
#define CLK_SCL_FMT19_3D_SBSHHSS    240000
#define CLK_PIXEL_FMT19_3D_SBSHHSS  CLK_1X_FMT19_3D_SBSHHSS

/* HDMI_Specification_Rev1.4 section 8.2.3.2 3D video format structure (HDMI1.4 Table 8-15/Figure 8-7). */
#define DEV_TYPE_FMT20_3D_SBSHHSS   DEV_TYPE_FMT20
#define FRAME_FMT20_3D_SBSHHSS      FRAME_FMT20
#define FIELD_FMT20_3D_SBSHHSS      FIELD_FMT20
#define WIDTH_FMT20_3D_SBSHHSS      WIDTH_FMT20
#define HEIGHT_FMT20_3D_SBSHHSS     HEIGHT_FMT20
#define HSYNCOS_FMT20_3D_SBSHHSS    HSYNCOS_FMT20
#define VSYNCOS_FMT20_3D_SBSHHSS    VSYNCOS_FMT20
#define HSYNCW_FMT20_3D_SBSHHSS     HSYNCW_FMT20
#define VSYNCW_FMT20_3D_SBSHHSS     VSYNCW_FMT20
#define TVH0_FMT20_3D_SBSHHSS       TVH0_FMT20
#define TVV0_FMT20_3D_SBSHHSS       TVV0_FMT20
#define TVH1_FMT20_3D_SBSHHSS       (TVH0_FMT20_3D_SBSHHSS + WIDTH_FMT20_3D_SBSHHSS - 1)
#define TVV1_FMT20_3D_SBSHHSS       (TVV0_FMT20_3D_SBSHHSS + HEIGHT_FMT20_3D_SBSHHSS - 1)
#define HPIXEL_FMT20_3D_SBSHHSS     HPIXEL_FMT20
#define VLINE_FMT20_3D_SBSHHSS      VLINE_FMT20
#define CLK_NX_FMT20_3D_SBSHHSS     CLK_1X_FMT20_3D_SBSHHSS
#define CLK_1X_FMT20_3D_SBSHHSS     CLK_1X_FMT20
#define CLK_SCL_FMT20_3D_SBSHHSS    240000
#define CLK_PIXEL_FMT20_3D_SBSHHSS  CLK_1X_FMT20_3D_SBSHHSS

/* HDMI_Specification_Rev1.4 section 8.2.3.2 3D video format structure (HDMI1.4 Table 8-15/Figure 8-7). */
#define DEV_TYPE_FMT32_3D_SBSHHSS   DEV_TYPE_FMT32
#define FRAME_FMT32_3D_SBSHHSS      FRAME_FMT32
#define FIELD_FMT32_3D_SBSHHSS      FIELD_FMT32
#define WIDTH_FMT32_3D_SBSHHSS      WIDTH_FMT32
#define HEIGHT_FMT32_3D_SBSHHSS     HEIGHT_FMT32
#define HSYNCOS_FMT32_3D_SBSHHSS    HSYNCOS_FMT32
#define VSYNCOS_FMT32_3D_SBSHHSS    VSYNCOS_FMT32
#define HSYNCW_FMT32_3D_SBSHHSS     HSYNCW_FMT32
#define VSYNCW_FMT32_3D_SBSHHSS     VSYNCW_FMT32
#define TVH0_FMT32_3D_SBSHHSS       TVH0_FMT32
#define TVV0_FMT32_3D_SBSHHSS       TVV0_FMT32
#define TVH1_FMT32_3D_SBSHHSS       (TVH0_FMT32_3D_SBSHHSS + WIDTH_FMT32_3D_SBSHHSS - 1)
#define TVV1_FMT32_3D_SBSHHSS       (TVV0_FMT32_3D_SBSHHSS + HEIGHT_FMT32_3D_SBSHHSS - 1)
#define HPIXEL_FMT32_3D_SBSHHSS     HPIXEL_FMT32
#define VLINE_FMT32_3D_SBSHHSS      VLINE_FMT32
#define CLK_NX_FMT32_3D_SBSHHSS     CLK_1X_FMT32_3D_SBSHHSS
#define CLK_1X_FMT32_3D_SBSHHSS     CLK_1X_FMT32
#define CLK_SCL_FMT32_3D_SBSHHSS    240000
#define CLK_PIXEL_FMT32_3D_SBSHHSS  CLK_1X_FMT32_3D_SBSHHSS

#define HDMI_PKT_EN     DISP_HDMI_PKT_AUDIO_IF_EN | DISP_HDMI_PKT_SPD_IF_EN | DISP_HDMI_PKT_AVI_IF_EN | \
                        DISP_HDMI_PKT_NP_EN | DISP_HDMI_PKT_AS_EN | DISP_HDMI_PKT_ACR_EN | DISP_HDMI_PKT_GC_EN
#define HDMI_PKT_3D_EN  HDMI_PKT_EN | DISP_HDMI_PKT_VS_IF_EN

/**************************************************************************
 *                              M A C R O S
 **************************************************************************/
#define READ8(_reg_)            (*((volatile UINT8  *)(_reg_)))
#define WRITE8(_reg_, _value_)  (*((volatile UINT8  *)(_reg_)) = (_value_))
#define READ16(_reg_)           (*((volatile UINT16 *)(_reg_)))
#define WRITE16(_reg_, _value_) (*((volatile UINT16 *)(_reg_)) = (_value_))

/**************************************************************************
 *              F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
/* device open. */
#define dispOpen             dispHdmiTvOpen
#define dispOpDevNameGet     dispHdmiTvOpDevNameGet
#define dispOpPowerOn        dispHdmiTvOpPowerOn
#define dispOpPowerOff       dispHdmiTvOpPowerOff
#define dispOpCapabilityGet  dispHdmiTvOpCapabilityGet
#define dispOpResolutionSet  dispHdmiTvOpResolutionSet
#define dispOpCfgSet         dispHdmiTvOpCfgSet

/* operating function. */
static UINT8 *dispOpDevNameGet(void);
static UINT32 dispOpPowerOn(void);
static UINT32 dispOpPowerOff(void);
static void   dispOpCapabilityGet(dispCapabDesc_t *pcapabDesc, UINT32 descSize);
static UINT32 dispOpResolutionSet(UINT32 resId);
static UINT32 dispOpCfgSet(UINT32 cfgId, va_list args);

/**************************************************************************
 *                        G L O B A L   D A T A
 **************************************************************************/
/* resolution id. */
static UINT32 dispResId = SP5K_EDID_720X480I_60HZ_4T3;

/* run-time configuration. */
static UINT32 dispColorFmt   = 0;  /* 0: RGB444, 1, YUV422, 2: YUV444. */
static UINT32 dispColorDepth = 0;  /* 0: color depth not indicated, 4: 24, 5: 30, 6: 36, 7: 48 bits per pixel. */
static UINT32 dispQuantRange = 0;  /* 0: default, 1: limited, 2: full quantization range. */
static UINT32 dispDviEn      = 0;  /* 0: disable, 1: enable DVI mode. */

/* device name. */
static UINT8 dispDevName[48];

static UINT8 *pdispDevName[] = {
	[SP5K_EDID_640X480P_60HZ_4T3      ] = (UINT8 *)DEV_NAME_FMT1,
	[SP5K_EDID_720X480P_60HZ_4T3      ] = (UINT8 *)DEV_NAME_FMT2,
	[SP5K_EDID_720X480P_60HZ_16T9     ] = (UINT8 *)DEV_NAME_FMT3,
	[SP5K_EDID_1280X720P_60HZ_16T9    ] = (UINT8 *)DEV_NAME_FMT4,
	[SP5K_EDID_1920X1080I_60HZ_16T9   ] = (UINT8 *)DEV_NAME_FMT5,
	[SP5K_EDID_720X480I_60HZ_4T3      ] = (UINT8 *)DEV_NAME_FMT6,
	[SP5K_EDID_720X480I_60HZ_16T9     ] = (UINT8 *)DEV_NAME_FMT7,
	[SP5K_EDID_720X240P_60HZ_4T3      ] = (UINT8 *)DEV_NAME_FMT8,
	[SP5K_EDID_720X240P_60HZ_16T9     ] = (UINT8 *)DEV_NAME_FMT9,
	[SP5K_EDID_2880X480I_60HZ_4T3     ] = (UINT8 *)DEV_NAME_FMT10,
	[SP5K_EDID_2880X480I_60HZ_16T9    ] = (UINT8 *)DEV_NAME_FMT11,
	[SP5K_EDID_2880X240P_60HZ_4T3     ] = (UINT8 *)DEV_NAME_FMT12,
	[SP5K_EDID_2880X240P_60HZ_16T9    ] = (UINT8 *)DEV_NAME_FMT13,
	[SP5K_EDID_1440X480P_60HZ_4T3     ] = (UINT8 *)DEV_NAME_FMT14,
	[SP5K_EDID_1440X480P_60HZ_16T9    ] = (UINT8 *)DEV_NAME_FMT15,
	[SP5K_EDID_1920X1080P_60HZ_16T9   ] = (UINT8 *)DEV_NAME_FMT16,
	[SP5K_EDID_720X576P_50HZ_4T3      ] = (UINT8 *)DEV_NAME_FMT17,
	[SP5K_EDID_720X576P_50HZ_16T9     ] = (UINT8 *)DEV_NAME_FMT18,
	[SP5K_EDID_1280X720P_50HZ_16T9    ] = (UINT8 *)DEV_NAME_FMT19,
	[SP5K_EDID_1920X1080I_50HZ_16T9   ] = (UINT8 *)DEV_NAME_FMT20,
	[SP5K_EDID_720X576I_50HZ_4T3      ] = (UINT8 *)DEV_NAME_FMT21,
	[SP5K_EDID_720X576I_50HZ_16T9     ] = (UINT8 *)DEV_NAME_FMT22,
	[SP5K_EDID_720X288P_50HZ_4T3      ] = (UINT8 *)DEV_NAME_FMT23,
	[SP5K_EDID_720X288P_50HZ_16T9     ] = (UINT8 *)DEV_NAME_FMT24,
	[SP5K_EDID_2880X576I_50HZ_4T3     ] = (UINT8 *)DEV_NAME_FMT25,
	[SP5K_EDID_2880X576I_50HZ_16T9    ] = (UINT8 *)DEV_NAME_FMT26,
	[SP5K_EDID_2880X288P_50HZ_4T3     ] = (UINT8 *)DEV_NAME_FMT27,
	[SP5K_EDID_2880X288P_50HZ_16T9    ] = (UINT8 *)DEV_NAME_FMT28,
	[SP5K_EDID_1440X576P_50HZ_4T3     ] = (UINT8 *)DEV_NAME_FMT29,
	[SP5K_EDID_1440X576P_50HZ_16T9    ] = (UINT8 *)DEV_NAME_FMT30,
	[SP5K_EDID_1920X1080P_50HZ_16T9   ] = (UINT8 *)DEV_NAME_FMT31,
	[SP5K_EDID_1920X1080P_24HZ_16T9   ] = (UINT8 *)DEV_NAME_FMT32,
	[SP5K_EDID_1920X1080P_25HZ_16T9   ] = (UINT8 *)DEV_NAME_FMT33,
	[SP5K_EDID_1920X1080P_30HZ_16T9   ] = (UINT8 *)DEV_NAME_FMT34,
	[SP5K_EDID_2880X480P_60HZ_4T3     ] = (UINT8 *)DEV_NAME_FMT35,
	[SP5K_EDID_2880X480P_60HZ_16T9    ] = (UINT8 *)DEV_NAME_FMT36,
	[SP5K_EDID_2880X576P_50HZ_4T3     ] = (UINT8 *)DEV_NAME_FMT37,
	[SP5K_EDID_2880X576P_50HZ_16T9    ] = (UINT8 *)DEV_NAME_FMT38,
	[SP5K_EDID_1920X1080I_100HZ_16T9  ] = (UINT8 *)DEV_NAME_FMT40,
	[SP5K_EDID_1280X720P_100HZ_16T9   ] = (UINT8 *)DEV_NAME_FMT41,
	[SP5K_EDID_720X576P_100HZ_4T3     ] = (UINT8 *)DEV_NAME_FMT42,
	[SP5K_EDID_720X576P_100HZ_16T9    ] = (UINT8 *)DEV_NAME_FMT43,
	[SP5K_EDID_720X576I_100HZ_4T3     ] = (UINT8 *)DEV_NAME_FMT44,
	[SP5K_EDID_720X576I_100HZ_16T9    ] = (UINT8 *)DEV_NAME_FMT45,
	[SP5K_EDID_1920X1080I_120HZ_16T9  ] = (UINT8 *)DEV_NAME_FMT46,
	[SP5K_EDID_1280X720P_120HZ_16T9   ] = (UINT8 *)DEV_NAME_FMT47,
	[SP5K_EDID_720X480P_120HZ_4T3     ] = (UINT8 *)DEV_NAME_FMT48,
	[SP5K_EDID_720X480P_120HZ_16T9    ] = (UINT8 *)DEV_NAME_FMT49,
	[SP5K_EDID_720X480I_120HZ_4T3     ] = (UINT8 *)DEV_NAME_FMT50,
	[SP5K_EDID_720X480I_120HZ_16T9    ] = (UINT8 *)DEV_NAME_FMT51,
	[SP5K_EDID_720X576P_200HZ_4T3     ] = (UINT8 *)DEV_NAME_FMT52,
	[SP5K_EDID_720X576P_200HZ_16T9    ] = (UINT8 *)DEV_NAME_FMT53,
	[SP5K_EDID_720X576I_200HZ_4T3     ] = (UINT8 *)DEV_NAME_FMT54,
	[SP5K_EDID_720X576I_200HZ_16T9    ] = (UINT8 *)DEV_NAME_FMT55,
	[SP5K_EDID_720X480P_240HZ_4T3     ] = (UINT8 *)DEV_NAME_FMT56,
	[SP5K_EDID_720X480P_240HZ_16T9    ] = (UINT8 *)DEV_NAME_FMT57,
	[SP5K_EDID_720X480I_240HZ_4T3     ] = (UINT8 *)DEV_NAME_FMT58,
	[SP5K_EDID_720X480I_240HZ_16T9    ] = (UINT8 *)DEV_NAME_FMT59,
	[SP5K_EDID_1280X720P_24HZ_16T9    ] = (UINT8 *)DEV_NAME_FMT60,
	[SP5K_EDID_1280X720P_25HZ_16T9    ] = (UINT8 *)DEV_NAME_FMT61,
	[SP5K_EDID_1280X720P_30HZ_16T9    ] = (UINT8 *)DEV_NAME_FMT62,
	[SP5K_EDID_1920X1080P_120HZ_16T9  ] = (UINT8 *)DEV_NAME_FMT63,
	[SP5K_EDID_1920X1080P_100HZ_16T9  ] = (UINT8 *)DEV_NAME_FMT64,
	[SP5K_EDID_1280X720P_24HZ_64T27   ] = (UINT8 *)DEV_NAME_FMT65,
	[SP5K_EDID_1280X720P_25HZ_64T27   ] = (UINT8 *)DEV_NAME_FMT66,
	[SP5K_EDID_1280X720P_30HZ_64T27   ] = (UINT8 *)DEV_NAME_FMT67,
	[SP5K_EDID_1280X720P_50HZ_64T27   ] = (UINT8 *)DEV_NAME_FMT68,
	[SP5K_EDID_1280X720P_60HZ_64T27   ] = (UINT8 *)DEV_NAME_FMT69,
	[SP5K_EDID_1280X720P_100HZ_64T27  ] = (UINT8 *)DEV_NAME_FMT70,
	[SP5K_EDID_1280X720P_120HZ_64T27  ] = (UINT8 *)DEV_NAME_FMT71,
	[SP5K_EDID_1920X1080P_24HZ_64T27  ] = (UINT8 *)DEV_NAME_FMT72,
	[SP5K_EDID_1920X1080P_25HZ_64T27  ] = (UINT8 *)DEV_NAME_FMT73,
	[SP5K_EDID_1920X1080P_30HZ_64T27  ] = (UINT8 *)DEV_NAME_FMT74,
	[SP5K_EDID_1920X1080P_50HZ_64T27  ] = (UINT8 *)DEV_NAME_FMT75,
	[SP5K_EDID_1920X1080P_60HZ_64T27  ] = (UINT8 *)DEV_NAME_FMT76,
	[SP5K_EDID_1920X1080P_100HZ_64T27 ] = (UINT8 *)DEV_NAME_FMT77,
	[SP5K_EDID_1920X1080P_120HZ_64T27 ] = (UINT8 *)DEV_NAME_FMT78,
	[SP5K_EDID_1680X720P_24HZ_64T27   ] = (UINT8 *)DEV_NAME_FMT79,
	[SP5K_EDID_1680X720P_25HZ_64T27   ] = (UINT8 *)DEV_NAME_FMT80,
	[SP5K_EDID_1680X720P_30HZ_64T27   ] = (UINT8 *)DEV_NAME_FMT81,
	[SP5K_EDID_1680X720P_50HZ_64T27   ] = (UINT8 *)DEV_NAME_FMT82,
	[SP5K_EDID_1680X720P_60HZ_64T27   ] = (UINT8 *)DEV_NAME_FMT83,
	[SP5K_EDID_1680X720P_100HZ_64T27  ] = (UINT8 *)DEV_NAME_FMT84,
	[SP5K_EDID_1680X720P_120HZ_64T27  ] = (UINT8 *)DEV_NAME_FMT85,
	[SP5K_EDID_2560X1080P_24HZ_64T27  ] = (UINT8 *)DEV_NAME_FMT86,
	[SP5K_EDID_2560X1080P_25HZ_64T27  ] = (UINT8 *)DEV_NAME_FMT87,
	[SP5K_EDID_2560X1080P_30HZ_64T27  ] = (UINT8 *)DEV_NAME_FMT88,
	[SP5K_EDID_2560X1080P_50HZ_64T27  ] = (UINT8 *)DEV_NAME_FMT89,
	[SP5K_EDID_2560X1080P_60HZ_64T27  ] = (UINT8 *)DEV_NAME_FMT90,
	[SP5K_EDID_2560X1080P_100HZ_64T27 ] = (UINT8 *)DEV_NAME_FMT91,
	[SP5K_EDID_2560X1080P_120HZ_64T27 ] = (UINT8 *)DEV_NAME_FMT92,
	[SP5K_EDID_3840X2160P_24HZ_16T9   ] = (UINT8 *)DEV_NAME_FMT93,
	[SP5K_EDID_3840X2160P_25HZ_16T9   ] = (UINT8 *)DEV_NAME_FMT94,
	[SP5K_EDID_3840X2160P_30HZ_16T9   ] = (UINT8 *)DEV_NAME_FMT95,
	[SP5K_EDID_3840X2160P_50HZ_16T9   ] = (UINT8 *)DEV_NAME_FMT96,
	[SP5K_EDID_3840X2160P_60HZ_16T9   ] = (UINT8 *)DEV_NAME_FMT97,
	[SP5K_EDID_4096X2160P_24HZ_256T135] = (UINT8 *)DEV_NAME_FMT98,
	[SP5K_EDID_4096X2160P_25HZ_256T135] = (UINT8 *)DEV_NAME_FMT99,
	[SP5K_EDID_4096X2160P_30HZ_256T135] = (UINT8 *)DEV_NAME_FMT100,
	[SP5K_EDID_4096X2160P_50HZ_256T135] = (UINT8 *)DEV_NAME_FMT101,
	[SP5K_EDID_4096X2160P_60HZ_256T135] = (UINT8 *)DEV_NAME_FMT102,
	[SP5K_EDID_3840X2160P_24HZ_64T27  ] = (UINT8 *)DEV_NAME_FMT103,
	[SP5K_EDID_3840X2160P_25HZ_64T27  ] = (UINT8 *)DEV_NAME_FMT104,
	[SP5K_EDID_3840X2160P_30HZ_64T27  ] = (UINT8 *)DEV_NAME_FMT105,
	[SP5K_EDID_3840X2160P_50HZ_64T27  ] = (UINT8 *)DEV_NAME_FMT106,
	[SP5K_EDID_3840X2160P_60HZ_64T27  ] = (UINT8 *)DEV_NAME_FMT107,
};

/* device capability. */
static dispCapabDesc_t dispCapabDesc[] = {
	[SP5K_EDID_640X480P_60HZ_4T3] = {  /* VIC=1. */
		.width        = WIDTH_FMT1,
		.height       = HEIGHT_FMT1,
		.xoff         = TVH0_FMT1,
		.yoff         = TVV0_FMT1,
		.aspectRatioX = 4,
		.aspectRatioY = 3,
		.logiEn       = 0,
		.frameRate    = 60000,
	},
	[SP5K_EDID_720X480P_60HZ_4T3] = {  /* VIC=2. */
		.width        = WIDTH_FMT2,
		.height       = HEIGHT_FMT2,
		.xoff         = TVH0_FMT2,
		.yoff         = TVV0_FMT2,
		.aspectRatioX = 4,
		.aspectRatioY = 3,
		.logiEn       = 0,
		.frameRate    = 60000,
	},
	[SP5K_EDID_720X480P_60HZ_16T9] = {  /* VIC=3. */
		.width        = WIDTH_FMT3,
		.height       = HEIGHT_FMT3,
		.xoff         = TVH0_FMT3,
		.yoff         = TVV0_FMT3,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 60000,
	},
	[SP5K_EDID_1280X720P_60HZ_16T9] = {  /* VIC=4. */
		.width        = WIDTH_FMT4,
		.height       = HEIGHT_FMT4,
		.xoff         = TVH0_FMT4,
		.yoff         = TVV0_FMT4,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 60000,
	},
	[SP5K_EDID_1920X1080I_60HZ_16T9] = {  /* VIC=5. */
		.width        = WIDTH_FMT5,
		.height       = HEIGHT_FMT5,
		.xoff         = TVH0_FMT5,
		.yoff         = TVV0_FMT5,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 60000,
	},
	[SP5K_EDID_720X480I_60HZ_4T3] = {  /* VIC=6. */
		.width        = WIDTH_FMT6,
		.height       = HEIGHT_FMT6,
		.xoff         = TVH0_FMT6,
		.yoff         = TVV0_FMT6,
		.aspectRatioX = 4,
		.aspectRatioY = 3,
		.logiEn       = 0,
		.frameRate    = 60000,
	},
	[SP5K_EDID_720X480I_60HZ_16T9] = {  /* VIC=7. */
		.width        = WIDTH_FMT7,
		.height       = HEIGHT_FMT7,
		.xoff         = TVH0_FMT7,
		.yoff         = TVV0_FMT7,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 60000,
	},
	[SP5K_EDID_720X240P_60HZ_4T3] = {  /* VIC=8. */
		.width        = WIDTH_FMT8,
		.height       = HEIGHT_FMT8,
		.xoff         = TVH0_FMT8,
		.yoff         = TVV0_FMT8,
		.aspectRatioX = 4,
		.aspectRatioY = 3,
		.logiEn       = 0,
		.frameRate    = 60000,
	},
	[SP5K_EDID_720X240P_60HZ_16T9] = {  /* VIC=9. */
		.width        = WIDTH_FMT9,
		.height       = HEIGHT_FMT9,
		.xoff         = TVH0_FMT9,
		.yoff         = TVV0_FMT9,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 60000,
	},
	[SP5K_EDID_2880X480I_60HZ_4T3] = {  /* VIC=10. */
		.width        = WIDTH_FMT10,
		.height       = HEIGHT_FMT10,
		.xoff         = TVH0_FMT10,
		.yoff         = TVV0_FMT10,
		.aspectRatioX = 4,
		.aspectRatioY = 3,
		.logiEn       = 0,
		.frameRate    = 60000,
	},
	[SP5K_EDID_2880X480I_60HZ_16T9] = {  /* VIC=11. */
		.width        = WIDTH_FMT11,
		.height       = HEIGHT_FMT11,
		.xoff         = TVH0_FMT11,
		.yoff         = TVV0_FMT11,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 60000,
	},
	[SP5K_EDID_2880X240P_60HZ_4T3] = {  /* VIC=12. */
		.width        = WIDTH_FMT12,
		.height       = HEIGHT_FMT12,
		.xoff         = TVH0_FMT12,
		.yoff         = TVV0_FMT12,
		.aspectRatioX = 4,
		.aspectRatioY = 3,
		.logiEn       = 0,
		.frameRate    = 60000,
	},
	[SP5K_EDID_2880X240P_60HZ_16T9] = {  /* VIC=13. */
		.width        = WIDTH_FMT13,
		.height       = HEIGHT_FMT13,
		.xoff         = TVH0_FMT13,
		.yoff         = TVV0_FMT13,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 60000,
	},
	[SP5K_EDID_1440X480P_60HZ_4T3] = {  /* VIC=14. */
		.width        = WIDTH_FMT14,
		.height       = HEIGHT_FMT14,
		.xoff         = TVH0_FMT14,
		.yoff         = TVV0_FMT14,
		.aspectRatioX = 4,
		.aspectRatioY = 3,
		.logiEn       = 0,
		.frameRate    = 60000,
	},
	[SP5K_EDID_1440X480P_60HZ_16T9] = {  /* VIC=15. */
		.width        = WIDTH_FMT15,
		.height       = HEIGHT_FMT15,
		.xoff         = TVH0_FMT15,
		.yoff         = TVV0_FMT15,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 60000,
	},
	[SP5K_EDID_1920X1080P_60HZ_16T9] = {  /* VIC=16. */
		.width        = WIDTH_FMT16,
		.height       = HEIGHT_FMT16,
		.xoff         = TVH0_FMT16,
		.yoff         = TVV0_FMT16,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 60000,
	},
	[SP5K_EDID_720X576P_50HZ_4T3] = {  /* VIC=17. */
		.width        = WIDTH_FMT17,
		.height       = HEIGHT_FMT17,
		.xoff         = TVH0_FMT17,
		.yoff         = TVV0_FMT17,
		.aspectRatioX = 4,
		.aspectRatioY = 3,
		.logiEn       = 0,
		.frameRate    = 50000,
	},
	[SP5K_EDID_720X576P_50HZ_16T9] = {  /* VIC=18. */
		.width        = WIDTH_FMT18,
		.height       = HEIGHT_FMT18,
		.xoff         = TVH0_FMT18,
		.yoff         = TVV0_FMT18,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 50000,
	},
	[SP5K_EDID_1280X720P_50HZ_16T9] = {  /* VIC=19. */
		.width        = WIDTH_FMT19,
		.height       = HEIGHT_FMT19,
		.xoff         = TVH0_FMT19,
		.yoff         = TVV0_FMT19,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 50000,
	},
	[SP5K_EDID_1920X1080I_50HZ_16T9] = {  /* VIC=20. */
		.width        = WIDTH_FMT20,
		.height       = HEIGHT_FMT20,
		.xoff         = TVH0_FMT20,
		.yoff         = TVV0_FMT20,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 50000,
	},
	[SP5K_EDID_720X576I_50HZ_4T3] = {  /* VIC=21. */
		.width        = WIDTH_FMT21,
		.height       = HEIGHT_FMT21,
		.xoff         = TVH0_FMT21,
		.yoff         = TVV0_FMT21,
		.aspectRatioX = 4,
		.aspectRatioY = 3,
		.logiEn       = 0,
		.frameRate    = 50000,
	},
	[SP5K_EDID_720X576I_50HZ_16T9] = {  /* VIC=22. */
		.width        = WIDTH_FMT22,
		.height       = HEIGHT_FMT22,
		.xoff         = TVH0_FMT22,
		.yoff         = TVV0_FMT22,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 50000,
	},
	[SP5K_EDID_720X288P_50HZ_4T3] = {  /* VIC=23. */
		.width        = WIDTH_FMT23,
		.height       = HEIGHT_FMT23,
		.xoff         = TVH0_FMT23,
		.yoff         = TVV0_FMT23,
		.aspectRatioX = 4,
		.aspectRatioY = 3,
		.logiEn       = 0,
		.frameRate    = 50000,
	},
	[SP5K_EDID_720X288P_50HZ_16T9] = {  /* VIC=24. */
		.width        = WIDTH_FMT24,
		.height       = HEIGHT_FMT24,
		.xoff         = TVH0_FMT24,
		.yoff         = TVV0_FMT24,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 50000,
	},
	[SP5K_EDID_2880X576I_50HZ_4T3] = {  /* VIC=25. */
		.width        = WIDTH_FMT25,
		.height       = HEIGHT_FMT25,
		.xoff         = TVH0_FMT25,
		.yoff         = TVV0_FMT25,
		.aspectRatioX = 4,
		.aspectRatioY = 3,
		.logiEn       = 0,
		.frameRate    = 50000,
	},
	[SP5K_EDID_2880X576I_50HZ_16T9] = {  /* VIC=26. */
		.width        = WIDTH_FMT26,
		.height       = HEIGHT_FMT26,
		.xoff         = TVH0_FMT26,
		.yoff         = TVV0_FMT26,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 50000,
	},
	[SP5K_EDID_2880X288P_50HZ_4T3] = {  /* VIC=27. */
		.width        = WIDTH_FMT27,
		.height       = HEIGHT_FMT27,
		.xoff         = TVH0_FMT27,
		.yoff         = TVV0_FMT27,
		.aspectRatioX = 4,
		.aspectRatioY = 3,
		.logiEn       = 0,
		.frameRate    = 50000,
	},
	[SP5K_EDID_2880X288P_50HZ_16T9] = {  /* VIC=28. */
		.width        = WIDTH_FMT28,
		.height       = HEIGHT_FMT28,
		.xoff         = TVH0_FMT28,
		.yoff         = TVV0_FMT28,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 50000,
	},
	[SP5K_EDID_1440X576P_50HZ_4T3] = {  /* VIC=29. */
		.width        = WIDTH_FMT29,
		.height       = HEIGHT_FMT29,
		.xoff         = TVH0_FMT29,
		.yoff         = TVV0_FMT29,
		.aspectRatioX = 4,
		.aspectRatioY = 3,
		.logiEn       = 0,
		.frameRate    = 50000,
	},
	[SP5K_EDID_1440X576P_50HZ_16T9] = {  /* VIC=30. */
		.width        = WIDTH_FMT30,
		.height       = HEIGHT_FMT30,
		.xoff         = TVH0_FMT30,
		.yoff         = TVV0_FMT30,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 50000,
	},
	[SP5K_EDID_1920X1080P_50HZ_16T9] = {  /* VIC=31. */
		.width        = WIDTH_FMT31,
		.height       = HEIGHT_FMT31,
		.xoff         = TVH0_FMT31,
		.yoff         = TVV0_FMT31,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 50000,
	},
	[SP5K_EDID_1920X1080P_24HZ_16T9] = {  /* VIC=32. */
		.width        = WIDTH_FMT32,
		.height       = HEIGHT_FMT32,
		.xoff         = TVH0_FMT32,
		.yoff         = TVV0_FMT32,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 24000,
	},
	[SP5K_EDID_1920X1080P_25HZ_16T9] = {  /* VIC=33. */
		.width        = WIDTH_FMT33,
		.height       = HEIGHT_FMT33,
		.xoff         = TVH0_FMT33,
		.yoff         = TVV0_FMT33,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 25000,
	},
	[SP5K_EDID_1920X1080P_30HZ_16T9] = {  /* VIC=34. */
		.width        = WIDTH_FMT34,
		.height       = HEIGHT_FMT34,
		.xoff         = TVH0_FMT34,
		.yoff         = TVV0_FMT34,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 30000,
	},
	[SP5K_EDID_2880X480P_60HZ_4T3] = {  /* VIC=35. */
		.width        = WIDTH_FMT35,
		.height       = HEIGHT_FMT35,
		.xoff         = TVH0_FMT35,
		.yoff         = TVV0_FMT35,
		.aspectRatioX = 4,
		.aspectRatioY = 3,
		.logiEn       = 0,
		.frameRate    = 60000,
	},
	[SP5K_EDID_2880X480P_60HZ_16T9] = {  /* VIC=36. */
		.width        = WIDTH_FMT36,
		.height       = HEIGHT_FMT36,
		.xoff         = TVH0_FMT36,
		.yoff         = TVV0_FMT36,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 60000,
	},
	[SP5K_EDID_2880X576P_50HZ_4T3] = {  /* VIC=37. */
		.width        = WIDTH_FMT37,
		.height       = HEIGHT_FMT37,
		.xoff         = TVH0_FMT37,
		.yoff         = TVV0_FMT37,
		.aspectRatioX = 4,
		.aspectRatioY = 3,
		.logiEn       = 0,
		.frameRate    = 60000,
	},
	[SP5K_EDID_2880X576P_50HZ_16T9] = {  /* VIC=38. */
		.width        = WIDTH_FMT38,
		.height       = HEIGHT_FMT38,
		.xoff         = TVH0_FMT38,
		.yoff         = TVV0_FMT38,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 60000,
	},
	[SP5K_EDID_1920X1080I_100HZ_16T9] = {  /* VIC=40. */
		.width        = WIDTH_FMT40,
		.height       = HEIGHT_FMT40,
		.xoff         = TVH0_FMT40,
		.yoff         = TVV0_FMT40,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 100000,
	},
	[SP5K_EDID_1280X720P_100HZ_16T9] = {  /* VIC=41. */
		.width        = WIDTH_FMT41,
		.height       = HEIGHT_FMT41,
		.xoff         = TVH0_FMT41,
		.yoff         = TVV0_FMT41,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 100000,
	},
	[SP5K_EDID_720X576P_100HZ_4T3] = {  /* VIC=42. */
		.width        = WIDTH_FMT42,
		.height       = HEIGHT_FMT42,
		.xoff         = TVH0_FMT42,
		.yoff         = TVV0_FMT42,
		.aspectRatioX = 4,
		.aspectRatioY = 3,
		.logiEn       = 0,
		.frameRate    = 100000,
	},
	[SP5K_EDID_720X576P_100HZ_16T9] = {  /* VIC=43. */
		.width        = WIDTH_FMT43,
		.height       = HEIGHT_FMT43,
		.xoff         = TVH0_FMT43,
		.yoff         = TVV0_FMT43,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 100000,
	},
	[SP5K_EDID_720X576I_100HZ_4T3] = {  /* VIC=44. */
		.width        = WIDTH_FMT44,
		.height       = HEIGHT_FMT44,
		.xoff         = TVH0_FMT44,
		.yoff         = TVV0_FMT44,
		.aspectRatioX = 4,
		.aspectRatioY = 3,
		.logiEn       = 0,
		.frameRate    = 100000,
	},
	[SP5K_EDID_720X576I_100HZ_16T9] = {  /* VIC=45. */
		.width        = WIDTH_FMT45,
		.height       = HEIGHT_FMT45,
		.xoff         = TVH0_FMT45,
		.yoff         = TVV0_FMT45,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 100000,
	},
	[SP5K_EDID_1920X1080I_120HZ_16T9] = {  /* VIC=46. */
		.width        = WIDTH_FMT46,
		.height       = HEIGHT_FMT46,
		.xoff         = TVH0_FMT46,
		.yoff         = TVV0_FMT46,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 120000,
	},
	[SP5K_EDID_1280X720P_120HZ_16T9] = {  /* VIC=47. */
		.width        = WIDTH_FMT47,
		.height       = HEIGHT_FMT47,
		.xoff         = TVH0_FMT47,
		.yoff         = TVV0_FMT47,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 120000,
	},
	[SP5K_EDID_720X480P_120HZ_4T3] = {  /* VIC=48. */
		.width        = WIDTH_FMT48,
		.height       = HEIGHT_FMT48,
		.xoff         = TVH0_FMT48,
		.yoff         = TVV0_FMT48,
		.aspectRatioX = 4,
		.aspectRatioY = 3,
		.logiEn       = 0,
		.frameRate    = 120000,
	},
	[SP5K_EDID_720X480P_120HZ_16T9] = {  /* VIC=49. */
		.width        = WIDTH_FMT49,
		.height       = HEIGHT_FMT49,
		.xoff         = TVH0_FMT49,
		.yoff         = TVV0_FMT49,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 120000,
	},
	[SP5K_EDID_720X480I_120HZ_4T3] = {  /* VIC=50. */
		.width        = WIDTH_FMT50,
		.height       = HEIGHT_FMT50,
		.xoff         = TVH0_FMT50,
		.yoff         = TVV0_FMT50,
		.aspectRatioX = 4,
		.aspectRatioY = 3,
		.logiEn       = 0,
		.frameRate    = 120000,
	},
	[SP5K_EDID_720X480I_120HZ_16T9] = {  /* VIC=51. */
		.width        = WIDTH_FMT51,
		.height       = HEIGHT_FMT51,
		.xoff         = TVH0_FMT51,
		.yoff         = TVV0_FMT51,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 120000,
	},
	[SP5K_EDID_720X576P_200HZ_4T3] = {  /* VIC=52. */
		.width        = WIDTH_FMT52,
		.height       = HEIGHT_FMT52,
		.xoff         = TVH0_FMT52,
		.yoff         = TVV0_FMT52,
		.aspectRatioX = 4,
		.aspectRatioY = 3,
		.logiEn       = 0,
		.frameRate    = 200000,
	},
	[SP5K_EDID_720X576P_200HZ_16T9] = {  /* VIC=53. */
		.width        = WIDTH_FMT53,
		.height       = HEIGHT_FMT53,
		.xoff         = TVH0_FMT53,
		.yoff         = TVV0_FMT53,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 200000,
	},
	[SP5K_EDID_720X576I_200HZ_4T3] = {  /* VIC=54. */
		.width        = WIDTH_FMT54,
		.height       = HEIGHT_FMT54,
		.xoff         = TVH0_FMT54,
		.yoff         = TVV0_FMT54,
		.aspectRatioX = 4,
		.aspectRatioY = 3,
		.logiEn       = 0,
		.frameRate    = 200000,
	},
	[SP5K_EDID_720X576I_200HZ_16T9] = {  /* VIC=55. */
		.width        = WIDTH_FMT55,
		.height       = HEIGHT_FMT55,
		.xoff         = TVH0_FMT55,
		.yoff         = TVV0_FMT55,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 200000,
	},
	[SP5K_EDID_720X480P_240HZ_4T3] = {  /* VIC=56. */
		.width        = WIDTH_FMT56,
		.height       = HEIGHT_FMT56,
		.xoff         = TVH0_FMT56,
		.yoff         = TVV0_FMT56,
		.aspectRatioX = 4,
		.aspectRatioY = 3,
		.logiEn       = 0,
		.frameRate    = 240000,
	},
	[SP5K_EDID_720X480P_240HZ_16T9] = {  /* VIC=57. */
		.width        = WIDTH_FMT57,
		.height       = HEIGHT_FMT57,
		.xoff         = TVH0_FMT57,
		.yoff         = TVV0_FMT57,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 240000,
	},
	[SP5K_EDID_720X480I_240HZ_4T3] = {  /* VIC=58. */
		.width        = WIDTH_FMT58,
		.height       = HEIGHT_FMT58,
		.xoff         = TVH0_FMT58,
		.yoff         = TVV0_FMT58,
		.aspectRatioX = 4,
		.aspectRatioY = 3,
		.logiEn       = 0,
		.frameRate    = 240000,
	},
	[SP5K_EDID_720X480I_240HZ_16T9] = {  /* VIC=59. */
		.width        = WIDTH_FMT59,
		.height       = HEIGHT_FMT59,
		.xoff         = TVH0_FMT59,
		.yoff         = TVV0_FMT59,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 240000,
	},
	[SP5K_EDID_1280X720P_24HZ_16T9] = {  /* VIC=60. */
		.width        = WIDTH_FMT60,
		.height       = HEIGHT_FMT60,
		.xoff         = TVH0_FMT60,
		.yoff         = TVV0_FMT60,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 24000,
	},
	[SP5K_EDID_1280X720P_25HZ_16T9] = {  /* VIC=61. */
		.width        = WIDTH_FMT61,
		.height       = HEIGHT_FMT61,
		.xoff         = TVH0_FMT61,
		.yoff         = TVV0_FMT61,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 25000,
	},
	[SP5K_EDID_1280X720P_30HZ_16T9] = {  /* VIC=62. */
		.width        = WIDTH_FMT62,
		.height       = HEIGHT_FMT62,
		.xoff         = TVH0_FMT62,
		.yoff         = TVV0_FMT62,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 30000,
	},
	[SP5K_EDID_1920X1080P_120HZ_16T9] = {  /* VIC=63. */
		.width        = WIDTH_FMT63,
		.height       = HEIGHT_FMT63,
		.xoff         = TVH0_FMT63,
		.yoff         = TVV0_FMT63,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 120000,
	},
	[SP5K_EDID_1920X1080P_100HZ_16T9] = {  /* VIC=64. */
		.width        = WIDTH_FMT64,
		.height       = HEIGHT_FMT64,
		.xoff         = TVH0_FMT64,
		.yoff         = TVV0_FMT64,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 100000,
	},
	[SP5K_EDID_1280X720P_24HZ_64T27] = {  /* VIC=65. */
		.width        = WIDTH_FMT65,
		.height       = HEIGHT_FMT65,
		.xoff         = TVH0_FMT65,
		.yoff         = TVV0_FMT65,
		.aspectRatioX = 64,
		.aspectRatioY = 27,
		.logiEn       = 0,
		.frameRate    = 24000,
	},
	[SP5K_EDID_1280X720P_25HZ_64T27] = {  /* VIC=66. */
		.width        = WIDTH_FMT66,
		.height       = HEIGHT_FMT66,
		.xoff         = TVH0_FMT66,
		.yoff         = TVV0_FMT66,
		.aspectRatioX = 64,
		.aspectRatioY = 27,
		.logiEn       = 0,
		.frameRate    = 25000,
	},
	[SP5K_EDID_1280X720P_30HZ_64T27] = {  /* VIC=67. */
		.width        = WIDTH_FMT67,
		.height       = HEIGHT_FMT67,
		.xoff         = TVH0_FMT67,
		.yoff         = TVV0_FMT67,
		.aspectRatioX = 64,
		.aspectRatioY = 27,
		.logiEn       = 0,
		.frameRate    = 30000,
	},
	[SP5K_EDID_1280X720P_50HZ_64T27] = {  /* VIC=68. */
		.width        = WIDTH_FMT68,
		.height       = HEIGHT_FMT68,
		.xoff         = TVH0_FMT68,
		.yoff         = TVV0_FMT68,
		.aspectRatioX = 64,
		.aspectRatioY = 27,
		.logiEn       = 0,
		.frameRate    = 50000,
	},
	[SP5K_EDID_1280X720P_60HZ_64T27] = {  /* VIC=69. */
		.width        = WIDTH_FMT69,
		.height       = HEIGHT_FMT69,
		.xoff         = TVH0_FMT69,
		.yoff         = TVV0_FMT69,
		.aspectRatioX = 64,
		.aspectRatioY = 27,
		.logiEn       = 0,
		.frameRate    = 60000,
	},
	[SP5K_EDID_1280X720P_100HZ_64T27] = {  /* VIC=70. */
		.width        = WIDTH_FMT70,
		.height       = HEIGHT_FMT70,
		.xoff         = TVH0_FMT70,
		.yoff         = TVV0_FMT70,
		.aspectRatioX = 64,
		.aspectRatioY = 27,
		.logiEn       = 0,
		.frameRate    = 100000,
	},
	[SP5K_EDID_1280X720P_120HZ_64T27] = {  /* VIC=71. */
		.width        = WIDTH_FMT71,
		.height       = HEIGHT_FMT71,
		.xoff         = TVH0_FMT71,
		.yoff         = TVV0_FMT71,
		.aspectRatioX = 64,
		.aspectRatioY = 27,
		.logiEn       = 0,
		.frameRate    = 120000,
	},
	[SP5K_EDID_1920X1080P_24HZ_64T27] = {  /* VIC=72. */
		.width        = WIDTH_FMT72,
		.height       = HEIGHT_FMT72,
		.xoff         = TVH0_FMT72,
		.yoff         = TVV0_FMT72,
		.aspectRatioX = 64,
		.aspectRatioY = 27,
		.logiEn       = 0,
		.frameRate    = 24000,
	},
	[SP5K_EDID_1920X1080P_25HZ_64T27] = {  /* VIC=73. */
		.width        = WIDTH_FMT73,
		.height       = HEIGHT_FMT73,
		.xoff         = TVH0_FMT73,
		.yoff         = TVV0_FMT73,
		.aspectRatioX = 64,
		.aspectRatioY = 27,
		.logiEn       = 0,
		.frameRate    = 25000,
	},
	[SP5K_EDID_1920X1080P_30HZ_64T27] = {  /* VIC=74. */
		.width        = WIDTH_FMT74,
		.height       = HEIGHT_FMT74,
		.xoff         = TVH0_FMT74,
		.yoff         = TVV0_FMT74,
		.aspectRatioX = 64,
		.aspectRatioY = 27,
		.logiEn       = 0,
		.frameRate    = 30000,
	},
	[SP5K_EDID_1920X1080P_50HZ_64T27] = {  /* VIC=75. */
		.width        = WIDTH_FMT75,
		.height       = HEIGHT_FMT75,
		.xoff         = TVH0_FMT75,
		.yoff         = TVV0_FMT75,
		.aspectRatioX = 64,
		.aspectRatioY = 27,
		.logiEn       = 0,
		.frameRate    = 50000,
	},
	[SP5K_EDID_1920X1080P_60HZ_64T27] = {  /* VIC=76. */
		.width        = WIDTH_FMT76,
		.height       = HEIGHT_FMT76,
		.xoff         = TVH0_FMT76,
		.yoff         = TVV0_FMT76,
		.aspectRatioX = 64,
		.aspectRatioY = 27,
		.logiEn       = 0,
		.frameRate    = 60000,
	},
	[SP5K_EDID_1920X1080P_100HZ_64T27] = {  /* VIC=77. */
		.width        = WIDTH_FMT77,
		.height       = HEIGHT_FMT77,
		.xoff         = TVH0_FMT77,
		.yoff         = TVV0_FMT77,
		.aspectRatioX = 64,
		.aspectRatioY = 27,
		.logiEn       = 0,
		.frameRate    = 100000,
	},
	[SP5K_EDID_1920X1080P_120HZ_64T27] = {  /* VIC=78. */
		.width        = WIDTH_FMT78,
		.height       = HEIGHT_FMT78,
		.xoff         = TVH0_FMT78,
		.yoff         = TVV0_FMT78,
		.aspectRatioX = 64,
		.aspectRatioY = 27,
		.logiEn       = 0,
		.frameRate    = 120000,
	},
	[SP5K_EDID_1680X720P_24HZ_64T27] = {  /* VIC=79. */
		.width        = WIDTH_FMT79,
		.height       = HEIGHT_FMT79,
		.xoff         = TVH0_FMT79,
		.yoff         = TVV0_FMT79,
		.aspectRatioX = 64,
		.aspectRatioY = 27,
		.logiEn       = 0,
		.frameRate    = 24000,
	},
	[SP5K_EDID_1680X720P_25HZ_64T27] = {  /* VIC=80. */
		.width        = WIDTH_FMT80,
		.height       = HEIGHT_FMT80,
		.xoff         = TVH0_FMT80,
		.yoff         = TVV0_FMT80,
		.aspectRatioX = 64,
		.aspectRatioY = 27,
		.logiEn       = 0,
		.frameRate    = 25000,
	},
	[SP5K_EDID_1680X720P_30HZ_64T27] = {  /* VIC=81. */
		.width        = WIDTH_FMT81,
		.height       = HEIGHT_FMT81,
		.xoff         = TVH0_FMT81,
		.yoff         = TVV0_FMT81,
		.aspectRatioX = 64,
		.aspectRatioY = 27,
		.logiEn       = 0,
		.frameRate    = 30000,
	},
	[SP5K_EDID_1680X720P_50HZ_64T27] = {  /* VIC=82. */
		.width        = WIDTH_FMT82,
		.height       = HEIGHT_FMT82,
		.xoff         = TVH0_FMT82,
		.yoff         = TVV0_FMT82,
		.aspectRatioX = 64,
		.aspectRatioY = 27,
		.logiEn       = 0,
		.frameRate    = 50000,
	},
	[SP5K_EDID_1680X720P_60HZ_64T27] = {  /* VIC=83. */
		.width        = WIDTH_FMT83,
		.height       = HEIGHT_FMT83,
		.xoff         = TVH0_FMT83,
		.yoff         = TVV0_FMT83,
		.aspectRatioX = 64,
		.aspectRatioY = 27,
		.logiEn       = 0,
		.frameRate    = 60000,
	},
	[SP5K_EDID_1680X720P_100HZ_64T27] = {  /* VIC=84. */
		.width        = WIDTH_FMT84,
		.height       = HEIGHT_FMT84,
		.xoff         = TVH0_FMT84,
		.yoff         = TVV0_FMT84,
		.aspectRatioX = 64,
		.aspectRatioY = 27,
		.logiEn       = 0,
		.frameRate    = 100000,
	},
	[SP5K_EDID_1680X720P_120HZ_64T27] = {  /* VIC=85. */
		.width        = WIDTH_FMT85,
		.height       = HEIGHT_FMT85,
		.xoff         = TVH0_FMT85,
		.yoff         = TVV0_FMT85,
		.aspectRatioX = 64,
		.aspectRatioY = 27,
		.logiEn       = 0,
		.frameRate    = 120000,
	},
	[SP5K_EDID_2560X1080P_24HZ_64T27] = {  /* VIC=86. */
		.width        = WIDTH_FMT86,
		.height       = HEIGHT_FMT86,
		.xoff         = TVH0_FMT86,
		.yoff         = TVV0_FMT86,
		.aspectRatioX = 64,
		.aspectRatioY = 27,
		.logiEn       = 0,
		.frameRate    = 24000,
	},
	[SP5K_EDID_2560X1080P_25HZ_64T27] = {  /* VIC=87. */
		.width        = WIDTH_FMT87,
		.height       = HEIGHT_FMT87,
		.xoff         = TVH0_FMT87,
		.yoff         = TVV0_FMT87,
		.aspectRatioX = 64,
		.aspectRatioY = 27,
		.logiEn       = 0,
		.frameRate    = 25000,
	},
	[SP5K_EDID_2560X1080P_30HZ_64T27] = {  /* VIC=88. */
		.width        = WIDTH_FMT88,
		.height       = HEIGHT_FMT88,
		.xoff         = TVH0_FMT88,
		.yoff         = TVV0_FMT88,
		.aspectRatioX = 64,
		.aspectRatioY = 27,
		.logiEn       = 0,
		.frameRate    = 30000,
	},
	[SP5K_EDID_2560X1080P_50HZ_64T27] = {  /* VIC=89. */
		.width        = WIDTH_FMT89,
		.height       = HEIGHT_FMT89,
		.xoff         = TVH0_FMT89,
		.yoff         = TVV0_FMT89,
		.aspectRatioX = 64,
		.aspectRatioY = 27,
		.logiEn       = 0,
		.frameRate    = 50000,
	},
	[SP5K_EDID_2560X1080P_60HZ_64T27] = {  /* VIC=90. */
		.width        = WIDTH_FMT90,
		.height       = HEIGHT_FMT90,
		.xoff         = TVH0_FMT90,
		.yoff         = TVV0_FMT90,
		.aspectRatioX = 64,
		.aspectRatioY = 27,
		.logiEn       = 0,
		.frameRate    = 60000,
	},
	[SP5K_EDID_2560X1080P_100HZ_64T27] = {  /* VIC=91. */
		.width        = WIDTH_FMT91,
		.height       = HEIGHT_FMT91,
		.xoff         = TVH0_FMT91,
		.yoff         = TVV0_FMT91,
		.aspectRatioX = 64,
		.aspectRatioY = 27,
		.logiEn       = 0,
		.frameRate    = 100000,
	},
	[SP5K_EDID_2560X1080P_120HZ_64T27] = {  /* VIC=92. */
		.width        = WIDTH_FMT92,
		.height       = HEIGHT_FMT92,
		.xoff         = TVH0_FMT92,
		.yoff         = TVV0_FMT92,
		.aspectRatioX = 64,
		.aspectRatioY = 27,
		.logiEn       = 0,
		.frameRate    = 120000,
	},
	[SP5K_EDID_3840X2160P_24HZ_16T9] = {  /* VIC=93. */
		.width        = WIDTH_FMT93,
		.height       = HEIGHT_FMT93,
		.xoff         = TVH0_FMT93,
		.yoff         = TVV0_FMT93,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 24000,
	},
	[SP5K_EDID_3840X2160P_25HZ_16T9] = {  /* VIC=94. */
		.width        = WIDTH_FMT94,
		.height       = HEIGHT_FMT94,
		.xoff         = TVH0_FMT94,
		.yoff         = TVV0_FMT94,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 25000,
	},
	[SP5K_EDID_3840X2160P_30HZ_16T9] = {  /* VIC=95. */
		.width        = WIDTH_FMT95,
		.height       = HEIGHT_FMT95,
		.xoff         = TVH0_FMT95,
		.yoff         = TVV0_FMT95,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 30000,
	},
	[SP5K_EDID_3840X2160P_50HZ_16T9] = {  /* VIC=96. */
		.width        = WIDTH_FMT96,
		.height       = HEIGHT_FMT96,
		.xoff         = TVH0_FMT96,
		.yoff         = TVV0_FMT96,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 50000,
	},
	[SP5K_EDID_3840X2160P_60HZ_16T9] = {  /* VIC=97. */
		.width        = WIDTH_FMT97,
		.height       = HEIGHT_FMT97,
		.xoff         = TVH0_FMT97,
		.yoff         = TVV0_FMT97,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 60000,
	},
	[SP5K_EDID_4096X2160P_24HZ_256T135] = {  /* VIC=98. */
		.width        = WIDTH_FMT98,
		.height       = HEIGHT_FMT98,
		.xoff         = TVH0_FMT98,
		.yoff         = TVV0_FMT98,
		.aspectRatioX = 256,
		.aspectRatioY = 135,
		.logiEn       = 0,
		.frameRate    = 24000,
	},
	[SP5K_EDID_4096X2160P_25HZ_256T135] = {  /* VIC=99. */
		.width        = WIDTH_FMT99,
		.height       = HEIGHT_FMT99,
		.xoff         = TVH0_FMT99,
		.yoff         = TVV0_FMT99,
		.aspectRatioX = 256,
		.aspectRatioY = 135,
		.logiEn       = 0,
		.frameRate    = 25000,
	},
	[SP5K_EDID_4096X2160P_30HZ_256T135] = {  /* VIC=100. */
		.width        = WIDTH_FMT100,
		.height       = HEIGHT_FMT100,
		.xoff         = TVH0_FMT100,
		.yoff         = TVV0_FMT100,
		.aspectRatioX = 256,
		.aspectRatioY = 135,
		.logiEn       = 0,
		.frameRate    = 30000,
	},
	[SP5K_EDID_4096X2160P_50HZ_256T135] = {  /* VIC=101. */
		.width        = WIDTH_FMT101,
		.height       = HEIGHT_FMT101,
		.xoff         = TVH0_FMT101,
		.yoff         = TVV0_FMT101,
		.aspectRatioX = 256,
		.aspectRatioY = 135,
		.logiEn       = 0,
		.frameRate    = 50000,
	},
	[SP5K_EDID_4096X2160P_60HZ_256T135] = {  /* VIC=102. */
		.width        = WIDTH_FMT102,
		.height       = HEIGHT_FMT102,
		.xoff         = TVH0_FMT102,
		.yoff         = TVV0_FMT102,
		.aspectRatioX = 256,
		.aspectRatioY = 135,
		.logiEn       = 0,
		.frameRate    = 60000,
	},
	[SP5K_EDID_3840X2160P_24HZ_64T27] = {  /* VIC=103. */
		.width        = WIDTH_FMT103,
		.height       = HEIGHT_FMT103,
		.xoff         = TVH0_FMT103,
		.yoff         = TVV0_FMT103,
		.aspectRatioX = 64,
		.aspectRatioY = 27,
		.logiEn       = 0,
		.frameRate    = 24000,
	},
	[SP5K_EDID_3840X2160P_25HZ_64T27] = {  /* VIC=104. */
		.width        = WIDTH_FMT104,
		.height       = HEIGHT_FMT104,
		.xoff         = TVH0_FMT104,
		.yoff         = TVV0_FMT104,
		.aspectRatioX = 64,
		.aspectRatioY = 27,
		.logiEn       = 0,
		.frameRate    = 25000,
	},
	[SP5K_EDID_3840X2160P_30HZ_64T27] = {  /* VIC=105. */
		.width        = WIDTH_FMT105,
		.height       = HEIGHT_FMT105,
		.xoff         = TVH0_FMT105,
		.yoff         = TVV0_FMT105,
		.aspectRatioX = 64,
		.aspectRatioY = 27,
		.logiEn       = 0,
		.frameRate    = 30000,
	},
	[SP5K_EDID_3840X2160P_50HZ_64T27] = {  /* VIC=106. */
		.width        = WIDTH_FMT106,
		.height       = HEIGHT_FMT106,
		.xoff         = TVH0_FMT106,
		.yoff         = TVV0_FMT106,
		.aspectRatioX = 64,
		.aspectRatioY = 27,
		.logiEn       = 0,
		.frameRate    = 50000,
	},
	[SP5K_EDID_3840X2160P_60HZ_64T27] = {  /* VIC=107. */
		.width        = WIDTH_FMT107,
		.height       = HEIGHT_FMT107,
		.xoff         = TVH0_FMT107,
		.yoff         = TVV0_FMT107,
		.aspectRatioX = 64,
		.aspectRatioY = 27,
		.logiEn       = 0,
		.frameRate    = 60000,
	},
};

static dispCapabDesc_t dispDviCapabDesc;
static dispDetailTiming_t dispDtlTime;

/* hdmi data island. */
static UINT8 dispHdmiGcPacketHeader[] = {
	DISP_HDMI_PKT_GC,  /* HB0 */ /* Packet Type. */
	0x00,              /* HB1 */ /* Version Number. */
	0x00,              /* HB2 */ /* Length. */
};

static UINT8 dispHdmiGcPacketBody[] = {
	0x00,  /* Checksum, but it will be figured automatically. */
	0x00,  /* SB0 */ /* 0   0   0   Clear_AVMUTE 0   0   0   Set_AVMUTE    */
	0x00,  /* SB1 */ /* PP3 PP2 PP1 PP0          CD3 CD2 CD1 CD0           */
	0x00,  /* SB2 */ /* 0   0   0   0            0   0   0   Default_Phase */
	0x00,  /* SB3 */
	0x00,  /* SB4 */
	0x00,  /* SB5 */
	0x00,  /* SB6 */
};

static UINT8 dispHdmiAviInfoFramePacketHeader[] = {
	DISP_HDMI_PKT_AVI_IF,  /* HB0 */ /* Packet Type. */
	0x02,                  /* HB1 */ /* Version Number. */
	0x0D,                  /* HB2 */ /* Length. */
};

static UINT8 dispHdmiAviInfoFramePacketBody[14];

static UINT8 disp640x480p60Hz4t3HdmiAviInfoFramePacketBody[] = {  /* VIC=1. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x58,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x01,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp720x480p60Hz4t3HdmiAviInfoFramePacketBody[] = {  /* VIC=2. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x58,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x02,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp720x480p60Hz16t9HdmiAviInfoFramePacketBody[] = {  /* VIC=3. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x68,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x03,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp1280x720p60Hz16t9HdmiAviInfoFramePacketBody[] = {  /* VIC=4. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xA8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x04,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp1920x1080i60Hz16t9HdmiAviInfoFramePacketBody[] = {  /* VIC=5. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xA8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x05,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp720x480i60Hz4t3HdmiAviInfoFramePacketBody[] = {  /* VIC=6. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x58,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x06,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x01,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp720x480i60Hz16t9HdmiAviInfoFramePacketBody[] = {  /* VIC=7. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x68,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x07,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x01,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp720x240p60Hz4t3HdmiAviInfoFramePacketBody[] = {  /* VIC=8. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x58,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x08,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x01,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp720x240p60Hz16t9HdmiAviInfoFramePacketBody[] = {  /* VIC=9. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x68,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x09,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x01,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp2880x480i60Hz4t3HdmiAviInfoFramePacketBody[] = {  /* VIC=10. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x58,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x0A,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp2880x480i60Hz16t9HdmiAviInfoFramePacketBody[] = {  /* VIC=11. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x68,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x0B,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp2880x240p60Hz4t3HdmiAviInfoFramePacketBody[] = {  /* VIC=12. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x58,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x0C,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp2880x240p60Hz16t9HdmiAviInfoFramePacketBody[] = {  /* VIC=13. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x68,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x0D,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp1440x480p60Hz4t3HdmiAviInfoFramePacketBody[] = {  /* VIC=14. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x58,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x0E,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp1440x480p60Hz16t9HdmiAviInfoFramePacketBody[] = {  /* VIC=15. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x68,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x0F,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp1920x1080p60Hz16t9HdmiAviInfoFramePacketBody[] = {  /* VIC=16. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xA8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x10,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp720x576p50Hz4t3HdmiAviInfoFramePacketBody[] = {  /* VIC=17. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x58,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x11,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp720x576p50Hz16t9HdmiAviInfoFramePacketBody[] = {  /* VIC=18. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x68,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x12,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp1280x720p50Hz16t9HdmiAviInfoFramePacketBody[] = {  /* VIC=19. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xA8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x13,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp1920x1080i50Hz16t9HdmiAviInfoFramePacketBody[] = {  /* VIC=20. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xA8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x14,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp720x576i50Hz4t3HdmiAviInfoFramePacketBody[] = {  /* VIC=21. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x58,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x15,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x01,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp720x576i50Hz16t9HdmiAviInfoFramePacketBody[] = {  /* VIC=22. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x68,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x16,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x01,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp720x288p50Hz4t3HdmiAviInfoFramePacketBody[] = {  /* VIC=23. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x58,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x17,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x01,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp720x288p50Hz16t9HdmiAviInfoFramePacketBody[] = {  /* VIC=24. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x68,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x18,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x01,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp2880x576i50Hz4t3HdmiAviInfoFramePacketBody[] = {  /* VIC=25. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x58,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x19,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp2880x576i50Hz16t9HdmiAviInfoFramePacketBody[] = {  /* VIC=26. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x68,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x1A,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp2880x288p50Hz4t3HdmiAviInfoFramePacketBody[] = {  /* VIC=27. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x58,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x1B,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp2880x288p50Hz16t9HdmiAviInfoFramePacketBody[] = {  /* VIC=28. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x68,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x1C,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp1440x576p50Hz4t3HdmiAviInfoFramePacketBody[] = {  /* VIC=29. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x58,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x1D,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp1440x576p50Hz16t9HdmiAviInfoFramePacketBody[] = {  /* VIC=30. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x68,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x1E,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp1920x1080p50Hz16t9HdmiAviInfoFramePacketBody[] = {  /* VIC=31. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xA8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x1F,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp1920x1080p24Hz16t9HdmiAviInfoFramePacketBody[] = {  /* VIC=32. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xA8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x20,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp1920x1080p25Hz16t9HdmiAviInfoFramePacketBody[] = {  /* VIC=33. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xA8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x21,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp1920x1080p30Hz16t9HdmiAviInfoFramePacketBody[] = {  /* VIC=34. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xA8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x22,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp2880x480p60Hz4t3HdmiAviInfoFramePacketBody[] = {  /* VIC=35. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x58,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x23,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp2880x480p60Hz16t9HdmiAviInfoFramePacketBody[] = {  /* VIC=36. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x68,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x24,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp2880x576p50Hz4t3HdmiAviInfoFramePacketBody[] = {  /* VIC=37. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x58,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x25,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp2880x576p50Hz16t9HdmiAviInfoFramePacketBody[] = {  /* VIC=38. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x68,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x26,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp1920x1080i100Hz16t9HdmiAviInfoFramePacketBody[] = {  /* VIC=40. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xA8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x28,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp1280x720p100Hz16t9HdmiAviInfoFramePacketBody[] = {  /* VIC=41. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xA8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x29,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp720x576p100Hz4t3HdmiAviInfoFramePacketBody[] = {  /* VIC=42. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x58,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x2A,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp720x576p100Hz16t9HdmiAviInfoFramePacketBody[] = {  /* VIC=43. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x68,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x2B,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp720x576i100Hz4t3HdmiAviInfoFramePacketBody[] = {  /* VIC=44. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x58,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x2C,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x01,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp720x576i100Hz16t9HdmiAviInfoFramePacketBody[] = {  /* VIC=45. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x68,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x2D,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x01,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp1920x1080i120Hz16t9HdmiAviInfoFramePacketBody[] = {  /* VIC=46. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xA8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x2E,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp1280x720p120Hz16t9HdmiAviInfoFramePacketBody[] = {  /* VIC=47. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xA8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x2F,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp720x480p120Hz4t3HdmiAviInfoFramePacketBody[] = {  /* VIC=48. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x58,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x30,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp720x480p120Hz16t9HdmiAviInfoFramePacketBody[] = {  /* VIC=49. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x68,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x31,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp720x480i120Hz4t3HdmiAviInfoFramePacketBody[] = {  /* VIC=50. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x58,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x32,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x01,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp720x480i120Hz16t9HdmiAviInfoFramePacketBody[] = {  /* VIC=51. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x68,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x33,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x01,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp720x576p200Hz4t3HdmiAviInfoFramePacketBody[] = {  /* VIC=52. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x58,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x34,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp720x576p200Hz16t9HdmiAviInfoFramePacketBody[] = {  /* VIC=53. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x68,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x35,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp720x576i200Hz4t3HdmiAviInfoFramePacketBody[] = {  /* VIC=54. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x58,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x36,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x01,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp720x576i200Hz16t9HdmiAviInfoFramePacketBody[] = {  /* VIC=55. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x68,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x37,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x01,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp720x480p240Hz4t3HdmiAviInfoFramePacketBody[] = {  /* VIC=56. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x58,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x38,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp720x480p240Hz16t9HdmiAviInfoFramePacketBody[] = {  /* VIC=57. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x68,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x39,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp720x480i240Hz4t3HdmiAviInfoFramePacketBody[] = {  /* VIC=58. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x58,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x3A,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x01,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp720x480i240Hz16t9HdmiAviInfoFramePacketBody[] = {  /* VIC=59. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x68,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x3B,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x01,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp1280x720p24Hz16t9HdmiAviInfoFramePacketBody[] = {  /* VIC=60. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xA8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x3C,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp1280x720p25Hz16t9HdmiAviInfoFramePacketBody[] = {  /* VIC=61. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xA8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x3D,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp1280x720p30Hz16t9HdmiAviInfoFramePacketBody[] = {  /* VIC=62. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xA8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x3E,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp1920x1080p120Hz16t9HdmiAviInfoFramePacketBody[] = {  /* VIC=63. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xA8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x3F,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp1920x1080p100Hz16t9HdmiAviInfoFramePacketBody[] = {  /* VIC=64. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xA8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x40,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp1280x720p24Hz64t27HdmiAviInfoFramePacketBody[] = {  /* VIC=65. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xB8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x41,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp1280x720p25Hz64t27HdmiAviInfoFramePacketBody[] = {  /* VIC=66. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xB8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x42,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp1280x720p30Hz64t27HdmiAviInfoFramePacketBody[] = {  /* VIC=67. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xB8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x43,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp1280x720p50Hz64t27HdmiAviInfoFramePacketBody[] = {  /* VIC=68. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xB8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x44,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp1280x720p60Hz64t27HdmiAviInfoFramePacketBody[] = {  /* VIC=69. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xB8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x45,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp1280x720p100Hz64t27HdmiAviInfoFramePacketBody[] = {  /* VIC=70. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xB8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x46,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp1280x720p120Hz64t27HdmiAviInfoFramePacketBody[] = {  /* VIC=71. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xB8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x47,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp1920x1080p24Hz64t27HdmiAviInfoFramePacketBody[] = {  /* VIC=72. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xB8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x48,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp1920x1080p25Hz64t27HdmiAviInfoFramePacketBody[] = {  /* VIC=73. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xB8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x49,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp1920x1080p30Hz64t27HdmiAviInfoFramePacketBody[] = {  /* VIC=74. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xB8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x4A,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp1920x1080p50Hz64t27HdmiAviInfoFramePacketBody[] = {  /* VIC=75. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xB8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x4B,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp1920x1080p60Hz64t27HdmiAviInfoFramePacketBody[] = {  /* VIC=76. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xB8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x4C,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp1920x1080p100Hz64t27HdmiAviInfoFramePacketBody[] = {  /* VIC=77. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xB8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x4D,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp1920x1080p120Hz64t27HdmiAviInfoFramePacketBody[] = {  /* VIC=78. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xB8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x4E,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp1680x720p24Hz64t27HdmiAviInfoFramePacketBody[] = {  /* VIC=79. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xB8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x4F,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp1680x720p25Hz64t27HdmiAviInfoFramePacketBody[] = {  /* VIC=80. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xB8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x50,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp1680x720p30Hz64t27HdmiAviInfoFramePacketBody[] = {  /* VIC=81. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xB8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x51,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp1680x720p50Hz64t27HdmiAviInfoFramePacketBody[] = {  /* VIC=82. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xB8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x52,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp1680x720p60Hz64t27HdmiAviInfoFramePacketBody[] = {  /* VIC=83. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xB8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x53,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp1680x720p100Hz64t27HdmiAviInfoFramePacketBody[] = {  /* VIC=84. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xB8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x54,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp1680x720p120Hz64t27HdmiAviInfoFramePacketBody[] = {  /* VIC=85. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xB8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x55,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp2560x1080p24Hz64t27HdmiAviInfoFramePacketBody[] = {  /* VIC=86. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xB8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x56,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp2560x1080p25Hz64t27HdmiAviInfoFramePacketBody[] = {  /* VIC=87. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xB8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x57,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp2560x1080p30Hz64t27HdmiAviInfoFramePacketBody[] = {  /* VIC=88. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xB8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x58,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp2560x1080p50Hz64t27HdmiAviInfoFramePacketBody[] = {  /* VIC=89. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xB8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x59,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp2560x1080p60Hz64t27HdmiAviInfoFramePacketBody[] = {  /* VIC=90. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xB8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x5A,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp2560x1080p100Hz64t27HdmiAviInfoFramePacketBody[] = {  /* VIC=91. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xB8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x5B,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp2560x1080p120Hz64t27HdmiAviInfoFramePacketBody[] = {  /* VIC=92. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xB8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x5C,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp3840x2160p24Hz16t9HdmiAviInfoFramePacketBody[] = {  /* VIC=93. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xA8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x5D,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp3840x2160p25Hz16t9HdmiAviInfoFramePacketBody[] = {  /* VIC=94. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xA8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x5E,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp3840x2160p30Hz16t9HdmiAviInfoFramePacketBody[] = {  /* VIC=95. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xA8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x5F,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp3840x2160p50Hz16t9HdmiAviInfoFramePacketBody[] = {  /* VIC=96. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xA8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x60,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp3840x2160p60Hz16t9HdmiAviInfoFramePacketBody[] = {  /* VIC=97. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xA8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x61,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp4096x2160p24Hz256t135HdmiAviInfoFramePacketBody[] = {  /* VIC=98. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xB8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x62,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp4096x2160p25Hz256t135HdmiAviInfoFramePacketBody[] = {  /* VIC=99. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xB8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x63,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp4096x2160p30Hz256t135HdmiAviInfoFramePacketBody[] = {  /* VIC=100. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xB8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x64,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp4096x2160p50Hz256t135HdmiAviInfoFramePacketBody[] = {  /* VIC=101. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xB8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x65,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp4096x2160p60Hz256t135HdmiAviInfoFramePacketBody[] = {  /* VIC=102. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xB8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x66,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp3840x2160p24Hz64t27HdmiAviInfoFramePacketBody[] = {  /* VIC=103. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xB8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x67,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp3840x2160p25Hz64t27HdmiAviInfoFramePacketBody[] = {  /* VIC=104. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xB8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x68,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp3840x2160p30Hz64t27HdmiAviInfoFramePacketBody[] = {  /* VIC=105. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xB8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x69,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp3840x2160p50Hz64t27HdmiAviInfoFramePacketBody[] = {  /* VIC=106. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xB8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x6A,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 disp3840x2160p60Hz64t27HdmiAviInfoFramePacketBody[] = {  /* VIC=107. */
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x10,  /* PB1 */ /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xB8,  /* PB2 */ /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* PB3 */ /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x6B,  /* PB4 */ /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* PB5 */ /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
	0x00,  /* PB11 */
	0x00,  /* PB12 */
	0x00,  /* PB13 */
};

static UINT8 *pdispHdmiAviInfoFramePacketBody[] = {
	[SP5K_EDID_640X480P_60HZ_4T3      ] = disp640x480p60Hz4t3HdmiAviInfoFramePacketBody,        /* VIC=1. */
	[SP5K_EDID_720X480P_60HZ_4T3      ] = disp720x480p60Hz4t3HdmiAviInfoFramePacketBody,        /* VIC=2. */
	[SP5K_EDID_720X480P_60HZ_16T9     ] = disp720x480p60Hz16t9HdmiAviInfoFramePacketBody,       /* VIC=3. */
	[SP5K_EDID_1280X720P_60HZ_16T9    ] = disp1280x720p60Hz16t9HdmiAviInfoFramePacketBody,      /* VIC=4. */
	[SP5K_EDID_1920X1080I_60HZ_16T9   ] = disp1920x1080i60Hz16t9HdmiAviInfoFramePacketBody,     /* VIC=5. */
	[SP5K_EDID_720X480I_60HZ_4T3      ] = disp720x480i60Hz4t3HdmiAviInfoFramePacketBody,        /* VIC=6. */
	[SP5K_EDID_720X480I_60HZ_16T9     ] = disp720x480i60Hz16t9HdmiAviInfoFramePacketBody,       /* VIC=7. */
	[SP5K_EDID_720X240P_60HZ_4T3      ] = disp720x240p60Hz4t3HdmiAviInfoFramePacketBody,        /* VIC=8. */
	[SP5K_EDID_720X240P_60HZ_16T9     ] = disp720x240p60Hz16t9HdmiAviInfoFramePacketBody,       /* VIC=9. */
	[SP5K_EDID_2880X480I_60HZ_4T3     ] = disp2880x480i60Hz4t3HdmiAviInfoFramePacketBody,       /* VIC=10. */
	[SP5K_EDID_2880X480I_60HZ_16T9    ] = disp2880x480i60Hz16t9HdmiAviInfoFramePacketBody,      /* VIC=11. */
	[SP5K_EDID_2880X240P_60HZ_4T3     ] = disp2880x240p60Hz4t3HdmiAviInfoFramePacketBody,       /* VIC=12. */
	[SP5K_EDID_2880X240P_60HZ_16T9    ] = disp2880x240p60Hz16t9HdmiAviInfoFramePacketBody,      /* VIC=13. */
	[SP5K_EDID_1440X480P_60HZ_4T3     ] = disp1440x480p60Hz4t3HdmiAviInfoFramePacketBody,       /* VIC=14. */
	[SP5K_EDID_1440X480P_60HZ_16T9    ] = disp1440x480p60Hz16t9HdmiAviInfoFramePacketBody,      /* VIC=15. */
	[SP5K_EDID_1920X1080P_60HZ_16T9   ] = disp1920x1080p60Hz16t9HdmiAviInfoFramePacketBody,     /* VIC=16. */
	[SP5K_EDID_720X576P_50HZ_4T3      ] = disp720x576p50Hz4t3HdmiAviInfoFramePacketBody,        /* VIC=17. */
	[SP5K_EDID_720X576P_50HZ_16T9     ] = disp720x576p50Hz16t9HdmiAviInfoFramePacketBody,       /* VIC=18. */
	[SP5K_EDID_1280X720P_50HZ_16T9    ] = disp1280x720p50Hz16t9HdmiAviInfoFramePacketBody,      /* VIC=19. */
	[SP5K_EDID_1920X1080I_50HZ_16T9   ] = disp1920x1080i50Hz16t9HdmiAviInfoFramePacketBody,     /* VIC=20. */
	[SP5K_EDID_720X576I_50HZ_4T3      ] = disp720x576i50Hz4t3HdmiAviInfoFramePacketBody,        /* VIC=21. */
	[SP5K_EDID_720X576I_50HZ_16T9     ] = disp720x576i50Hz16t9HdmiAviInfoFramePacketBody,       /* VIC=22. */
	[SP5K_EDID_720X288P_50HZ_4T3      ] = disp720x288p50Hz4t3HdmiAviInfoFramePacketBody,        /* VIC=23. */
	[SP5K_EDID_720X288P_50HZ_16T9     ] = disp720x288p50Hz16t9HdmiAviInfoFramePacketBody,       /* VIC=24. */
	[SP5K_EDID_2880X576I_50HZ_4T3     ] = disp2880x576i50Hz4t3HdmiAviInfoFramePacketBody,       /* VIC=25. */
	[SP5K_EDID_2880X576I_50HZ_16T9    ] = disp2880x576i50Hz16t9HdmiAviInfoFramePacketBody,      /* VIC=26. */
	[SP5K_EDID_2880X288P_50HZ_4T3     ] = disp2880x288p50Hz4t3HdmiAviInfoFramePacketBody,       /* VIC=27. */
	[SP5K_EDID_2880X288P_50HZ_16T9    ] = disp2880x288p50Hz16t9HdmiAviInfoFramePacketBody,      /* VIC=28. */
	[SP5K_EDID_1440X576P_50HZ_4T3     ] = disp1440x576p50Hz4t3HdmiAviInfoFramePacketBody,       /* VIC=29. */
	[SP5K_EDID_1440X576P_50HZ_16T9    ] = disp1440x576p50Hz16t9HdmiAviInfoFramePacketBody,      /* VIC=30. */
	[SP5K_EDID_1920X1080P_50HZ_16T9   ] = disp1920x1080p50Hz16t9HdmiAviInfoFramePacketBody,     /* VIC=31. */
	[SP5K_EDID_1920X1080P_24HZ_16T9   ] = disp1920x1080p24Hz16t9HdmiAviInfoFramePacketBody,     /* VIC=32. */
	[SP5K_EDID_1920X1080P_25HZ_16T9   ] = disp1920x1080p25Hz16t9HdmiAviInfoFramePacketBody,     /* VIC=33. */
	[SP5K_EDID_1920X1080P_30HZ_16T9   ] = disp1920x1080p30Hz16t9HdmiAviInfoFramePacketBody,     /* VIC=34. */
	[SP5K_EDID_2880X480P_60HZ_4T3     ] = disp2880x480p60Hz4t3HdmiAviInfoFramePacketBody,       /* VIC=35. */
	[SP5K_EDID_2880X480P_60HZ_16T9    ] = disp2880x480p60Hz16t9HdmiAviInfoFramePacketBody,      /* VIC=36. */
	[SP5K_EDID_2880X576P_50HZ_4T3     ] = disp2880x576p50Hz4t3HdmiAviInfoFramePacketBody,       /* VIC=37. */
	[SP5K_EDID_2880X576P_50HZ_16T9    ] = disp2880x576p50Hz16t9HdmiAviInfoFramePacketBody,      /* VIC=38. */
	[SP5K_EDID_1920X1080I_100HZ_16T9  ] = disp1920x1080i100Hz16t9HdmiAviInfoFramePacketBody,    /* VIC=40. */
	[SP5K_EDID_1280X720P_100HZ_16T9   ] = disp1280x720p100Hz16t9HdmiAviInfoFramePacketBody,     /* VIC=41. */
	[SP5K_EDID_720X576P_100HZ_4T3     ] = disp720x576p100Hz4t3HdmiAviInfoFramePacketBody,       /* VIC=42. */
	[SP5K_EDID_720X576P_100HZ_16T9    ] = disp720x576p100Hz16t9HdmiAviInfoFramePacketBody,      /* VIC=43. */
	[SP5K_EDID_720X576I_100HZ_4T3     ] = disp720x576i100Hz4t3HdmiAviInfoFramePacketBody,       /* VIC=44. */
	[SP5K_EDID_720X576I_100HZ_16T9    ] = disp720x576i100Hz16t9HdmiAviInfoFramePacketBody,      /* VIC=45. */
	[SP5K_EDID_1920X1080I_120HZ_16T9  ] = disp1920x1080i120Hz16t9HdmiAviInfoFramePacketBody,    /* VIC=46. */
	[SP5K_EDID_1280X720P_120HZ_16T9   ] = disp1280x720p120Hz16t9HdmiAviInfoFramePacketBody,     /* VIC=47. */
	[SP5K_EDID_720X480P_120HZ_4T3     ] = disp720x480p120Hz4t3HdmiAviInfoFramePacketBody,       /* VIC=48. */
	[SP5K_EDID_720X480P_120HZ_16T9    ] = disp720x480p120Hz16t9HdmiAviInfoFramePacketBody,      /* VIC=49. */
	[SP5K_EDID_720X480I_120HZ_4T3     ] = disp720x480i120Hz4t3HdmiAviInfoFramePacketBody,       /* VIC=50. */
	[SP5K_EDID_720X480I_120HZ_16T9    ] = disp720x480i120Hz16t9HdmiAviInfoFramePacketBody,      /* VIC=51. */
	[SP5K_EDID_720X576P_200HZ_4T3     ] = disp720x576p200Hz4t3HdmiAviInfoFramePacketBody,       /* VIC=52. */
	[SP5K_EDID_720X576P_200HZ_16T9    ] = disp720x576p200Hz16t9HdmiAviInfoFramePacketBody,      /* VIC=53. */
	[SP5K_EDID_720X576I_200HZ_4T3     ] = disp720x576i200Hz4t3HdmiAviInfoFramePacketBody,       /* VIC=54. */
	[SP5K_EDID_720X576I_200HZ_16T9    ] = disp720x576i200Hz16t9HdmiAviInfoFramePacketBody,      /* VIC=55. */
	[SP5K_EDID_720X480P_240HZ_4T3     ] = disp720x480p240Hz4t3HdmiAviInfoFramePacketBody,       /* VIC=56. */
	[SP5K_EDID_720X480P_240HZ_16T9    ] = disp720x480p240Hz16t9HdmiAviInfoFramePacketBody,      /* VIC=57. */
	[SP5K_EDID_720X480I_240HZ_4T3     ] = disp720x480i240Hz4t3HdmiAviInfoFramePacketBody,       /* VIC=58. */
	[SP5K_EDID_720X480I_240HZ_16T9    ] = disp720x480i240Hz16t9HdmiAviInfoFramePacketBody,      /* VIC=59. */
	[SP5K_EDID_1280X720P_24HZ_16T9    ] = disp1280x720p24Hz16t9HdmiAviInfoFramePacketBody,      /* VIC=60. */
	[SP5K_EDID_1280X720P_25HZ_16T9    ] = disp1280x720p25Hz16t9HdmiAviInfoFramePacketBody,      /* VIC=61. */
	[SP5K_EDID_1280X720P_30HZ_16T9    ] = disp1280x720p30Hz16t9HdmiAviInfoFramePacketBody,      /* VIC=62. */
	[SP5K_EDID_1920X1080P_120HZ_16T9  ] = disp1920x1080p120Hz16t9HdmiAviInfoFramePacketBody,    /* VIC=63. */
	[SP5K_EDID_1920X1080P_100HZ_16T9  ] = disp1920x1080p100Hz16t9HdmiAviInfoFramePacketBody,    /* VIC=64. */
	[SP5K_EDID_1280X720P_24HZ_64T27   ] = disp1280x720p24Hz64t27HdmiAviInfoFramePacketBody,     /* VIC=65. */
	[SP5K_EDID_1280X720P_25HZ_64T27   ] = disp1280x720p25Hz64t27HdmiAviInfoFramePacketBody,     /* VIC=66. */
	[SP5K_EDID_1280X720P_30HZ_64T27   ] = disp1280x720p30Hz64t27HdmiAviInfoFramePacketBody,     /* VIC=67. */
	[SP5K_EDID_1280X720P_50HZ_64T27   ] = disp1280x720p50Hz64t27HdmiAviInfoFramePacketBody,     /* VIC=68. */
	[SP5K_EDID_1280X720P_60HZ_64T27   ] = disp1280x720p60Hz64t27HdmiAviInfoFramePacketBody,     /* VIC=69. */
	[SP5K_EDID_1280X720P_100HZ_64T27  ] = disp1280x720p100Hz64t27HdmiAviInfoFramePacketBody,    /* VIC=70. */
	[SP5K_EDID_1280X720P_120HZ_64T27  ] = disp1280x720p120Hz64t27HdmiAviInfoFramePacketBody,    /* VIC=71. */
	[SP5K_EDID_1920X1080P_24HZ_64T27  ] = disp1920x1080p24Hz64t27HdmiAviInfoFramePacketBody,    /* VIC=72. */
	[SP5K_EDID_1920X1080P_25HZ_64T27  ] = disp1920x1080p25Hz64t27HdmiAviInfoFramePacketBody,    /* VIC=73. */
	[SP5K_EDID_1920X1080P_30HZ_64T27  ] = disp1920x1080p30Hz64t27HdmiAviInfoFramePacketBody,    /* VIC=74. */
	[SP5K_EDID_1920X1080P_50HZ_64T27  ] = disp1920x1080p50Hz64t27HdmiAviInfoFramePacketBody,    /* VIC=75. */
	[SP5K_EDID_1920X1080P_60HZ_64T27  ] = disp1920x1080p60Hz64t27HdmiAviInfoFramePacketBody,    /* VIC=76. */
	[SP5K_EDID_1920X1080P_100HZ_64T27 ] = disp1920x1080p100Hz64t27HdmiAviInfoFramePacketBody,   /* VIC=77. */
	[SP5K_EDID_1920X1080P_120HZ_64T27 ] = disp1920x1080p120Hz64t27HdmiAviInfoFramePacketBody,   /* VIC=78. */
	[SP5K_EDID_1680X720P_24HZ_64T27   ] = disp1680x720p24Hz64t27HdmiAviInfoFramePacketBody,     /* VIC=79. */
	[SP5K_EDID_1680X720P_25HZ_64T27   ] = disp1680x720p25Hz64t27HdmiAviInfoFramePacketBody,     /* VIC=80. */
	[SP5K_EDID_1680X720P_30HZ_64T27   ] = disp1680x720p30Hz64t27HdmiAviInfoFramePacketBody,     /* VIC=81. */
	[SP5K_EDID_1680X720P_50HZ_64T27   ] = disp1680x720p50Hz64t27HdmiAviInfoFramePacketBody,     /* VIC=82. */
	[SP5K_EDID_1680X720P_60HZ_64T27   ] = disp1680x720p60Hz64t27HdmiAviInfoFramePacketBody,     /* VIC=83. */
	[SP5K_EDID_1680X720P_100HZ_64T27  ] = disp1680x720p100Hz64t27HdmiAviInfoFramePacketBody,    /* VIC=84. */
	[SP5K_EDID_1680X720P_120HZ_64T27  ] = disp1680x720p120Hz64t27HdmiAviInfoFramePacketBody,    /* VIC=85. */
	[SP5K_EDID_2560X1080P_24HZ_64T27  ] = disp2560x1080p24Hz64t27HdmiAviInfoFramePacketBody,    /* VIC=86. */
	[SP5K_EDID_2560X1080P_25HZ_64T27  ] = disp2560x1080p25Hz64t27HdmiAviInfoFramePacketBody,    /* VIC=87. */
	[SP5K_EDID_2560X1080P_30HZ_64T27  ] = disp2560x1080p30Hz64t27HdmiAviInfoFramePacketBody,    /* VIC=88. */
	[SP5K_EDID_2560X1080P_50HZ_64T27  ] = disp2560x1080p50Hz64t27HdmiAviInfoFramePacketBody,    /* VIC=89. */
	[SP5K_EDID_2560X1080P_60HZ_64T27  ] = disp2560x1080p60Hz64t27HdmiAviInfoFramePacketBody,    /* VIC=90. */
	[SP5K_EDID_2560X1080P_100HZ_64T27 ] = disp2560x1080p100Hz64t27HdmiAviInfoFramePacketBody,   /* VIC=91. */
	[SP5K_EDID_2560X1080P_120HZ_64T27 ] = disp2560x1080p120Hz64t27HdmiAviInfoFramePacketBody,   /* VIC=92. */
	[SP5K_EDID_3840X2160P_24HZ_16T9   ] = disp3840x2160p24Hz16t9HdmiAviInfoFramePacketBody,     /* VIC=93. */
	[SP5K_EDID_3840X2160P_25HZ_16T9   ] = disp3840x2160p25Hz16t9HdmiAviInfoFramePacketBody,     /* VIC=94. */
	[SP5K_EDID_3840X2160P_30HZ_16T9   ] = disp3840x2160p30Hz16t9HdmiAviInfoFramePacketBody,     /* VIC=95. */
	[SP5K_EDID_3840X2160P_50HZ_16T9   ] = disp3840x2160p50Hz16t9HdmiAviInfoFramePacketBody,     /* VIC=96. */
	[SP5K_EDID_3840X2160P_60HZ_16T9   ] = disp3840x2160p60Hz16t9HdmiAviInfoFramePacketBody,     /* VIC=97. */
	[SP5K_EDID_4096X2160P_24HZ_256T135] = disp4096x2160p24Hz256t135HdmiAviInfoFramePacketBody,  /* VIC=98. */
	[SP5K_EDID_4096X2160P_25HZ_256T135] = disp4096x2160p25Hz256t135HdmiAviInfoFramePacketBody,  /* VIC=99. */
	[SP5K_EDID_4096X2160P_30HZ_256T135] = disp4096x2160p30Hz256t135HdmiAviInfoFramePacketBody,  /* VIC=100. */
	[SP5K_EDID_4096X2160P_50HZ_256T135] = disp4096x2160p50Hz256t135HdmiAviInfoFramePacketBody,  /* VIC=101. */
	[SP5K_EDID_4096X2160P_60HZ_256T135] = disp4096x2160p60Hz256t135HdmiAviInfoFramePacketBody,  /* VIC=102. */
	[SP5K_EDID_3840X2160P_24HZ_64T27  ] = disp3840x2160p24Hz64t27HdmiAviInfoFramePacketBody,    /* VIC=103. */
	[SP5K_EDID_3840X2160P_25HZ_64T27  ] = disp3840x2160p25Hz64t27HdmiAviInfoFramePacketBody,    /* VIC=104. */
	[SP5K_EDID_3840X2160P_30HZ_64T27  ] = disp3840x2160p30Hz64t27HdmiAviInfoFramePacketBody,    /* VIC=105. */
	[SP5K_EDID_3840X2160P_50HZ_64T27  ] = disp3840x2160p50Hz64t27HdmiAviInfoFramePacketBody,    /* VIC=106. */
	[SP5K_EDID_3840X2160P_60HZ_64T27  ] = disp3840x2160p60Hz64t27HdmiAviInfoFramePacketBody,    /* VIC=107. */
};

static UINT8 dispHdmiSpdInfoFramePacketHeader[] = {
	DISP_HDMI_PKT_SPD_IF,  /* HB0 */ /* Packet Type. */
	0x01,                  /* HB1 */ /* Version Number. */
	0x19,                  /* HB2 */ /* Length. */
};

static UINT8 dispHdmiSpdInfoFramePacketBody[] = {
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	'i',   /* PB1 */ /* PB1~PB8: VN1~VN8, Vendor Name (7-bit ASCII code). */
	'C',   /* PB2 */
	'a',   /* PB3 */
	't',   /* PB4 */
	'c',   /* PB5 */
	'h',   /* PB6 */
	' ',   /* PB7 */
	' ',   /* PB8 */
	'S',   /* PB9 */ /* PB9~PB24: PD1~PD16, Product Description (7-bit ASCII code). */
	'P',   /* PB10 */
	'C',   /* PB11 */
	'A',   /* PB12 */
	'6',   /* PB13 */
	'3',   /* PB14 */
	'5',   /* PB15 */
	'0',   /* PB16 */
	' ',   /* PB17 */
	' ',   /* PB18 */
	' ',   /* PB19 */
	' ',   /* PB20 */
	' ',   /* PB21 */
	' ',   /* PB22 */
	' ',   /* PB23 */
	' ',   /* PB24 */
	0x06,  /* PB25 */ /* Source Device Information: DSC. */
};

static UINT8 dispHdmiAudioInfoFramePacketHeader[] = {
	DISP_HDMI_PKT_AUDIO_IF,  /* HB0 */ /* Packet Type. */
	0x01,                    /* HB1 */ /* Version Number. */
	0x0A,                    /* HB2 */ /* Length. */
};

static UINT8 dispHdmiAudioInfoFramePacketBody[] = {
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x01,  /* PB1 */ /* CT3    CT2  CT1  CT0  | Rsvd CC2  CC1     CC0     */
	0x00,  /* PB2 */ /* Rsvd   Rsvd Rsvd SF2  | SF1  SF0  SS1     SS0     */
	0x00,  /* PB3 */ /* Format depends on coding type. */
	0x00,  /* PB4 */ /* CA7    CA6  CA5  CA4  | CA3  CA2  CA1     CA0     */
	0x00,  /* PB5 */ /* DM_INH LSV3 LSV2 LSV1 | LSV0 Rsvd LFEPBL1 LFEPBL0 */
	0x00,  /* PB6 */
	0x00,  /* PB7 */
	0x00,  /* PB8 */
	0x00,  /* PB9 */
	0x00,  /* PB10 */
};

static UINT8 dispHdmiVsInfoFramePacketHeader[] = {
	DISP_HDMI_PKT_VS_IF,  /* HB0 */ /* Packet Type. */
	0x01,                 /* HB1 */ /* Version Number. */
	0x1B,                 /* HB2 */ /* Length. */
};

/* HDMI_Video_Format(PB4[7:5])
 *   3b'000: No additional HDMI Video format is present in this packet
 *   3b'001: Extended resolution format(e.g. used for 4K x 2K video) present
 *   3b'010: 3D format indication present
 *   3b'011~111: Reserved for future use */
/* 3D_Structure(PB5[7:4]) refer to Spec HDMI 1.4 Table H-7
 *   4b'0000: Frame packing
 *   4b'0001: Field alternative
 *   4b'0010: Line alternative
 *   4b'0011: Side-by-Side (Full)
 *   4b'0100: L + depth
 *   4b'0101: L + depth + graphics-depth
 *   4b'0110: Top-and-Bottom
 *   4b'0111: Reserved for future use
 *   4b'1000: Side-by-Side (Half) with horizontal sub-sampling
 *   4b'1001~1110: Reserved for future use
 *   4b'1111: Side-by-Side (Half) with all quincunx sub-sampling
 */
/* this VS/VSI InfoFrame used for 3D video frame packing format only */
static UINT8 dispHdmiVsInfoFramePacketBody[] = {
	0x00,  /* PB0 */ /* Checksum, but it will be figured automatically. */
	0x03,  /* PB1 */ /* First  24-bit IEEE Registration Identifier */
	0x0C,  /* PB2 */ /* Second 24-bit IEEE Registration Identifier */
	0x00,  /* PB3 */ /* Last   24-bit IEEE Registration Identifier */
	0x40,  /* PB4 */ /* HDMI_Video_Format(PB4[7:5]) Rsvd |       Rsvd      Rsvd Rsvd Rsvd */
	0x00,  /* PB5 */ /* 3D_Structure(PB5[7:4])           | 3D_Meta_present Rsvd Rsvd Rsvd */
	0x00,  /* PB6 */ /* 3D_Ext_Data field(PB6[7:4])      |       Rsvd      Rsvd Rsvd Rsvd */
	0x00,  /* PB7 */ /* 3D_Metadata_type(PB7[7:5]) 3D_Metadata_Length(PB7[4:0]) */
	0x00, 0x00, 0x00, 0x00,  /* PB8~PB11 */ /* 3D_Metadata */
	0x00, 0x00, 0x00, 0x00,  /* PB12~PB15 */
	0x00, 0x00, 0x00, 0x00,  /* PB16~PB19 */
	0x00, 0x00, 0x00, 0x00,  /* PB20~PB23 */
	0x00, 0x00, 0x00, 0x00,  /* PB24~PB27 */
};

/* parameter table. */
static dispPara_t disp640x480p60Hz4t3PwrOnParaTbl[] = {  /* VIC=1. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT1, CLK_NX_FMT1, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT1, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT1, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT1, CLK_1X_DIV_FMT1, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT1, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT1, FIELD_FMT1, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x0, 0x0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT1, VLINE_FMT1, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT1, VSYNCOS_FMT1, HSYNCW_FMT1, VSYNCW_FMT1, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT1, TVV0_FMT1, TVH1_FMT1, TVV1_FMT1, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp720x480p60Hz4t3PwrOnParaTbl[] = {  /* VIC=2. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT2, CLK_NX_FMT2, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT2, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT2, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT2, CLK_1X_DIV_FMT2, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT2, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT2, FIELD_FMT2, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x0, 0x0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT2, VLINE_FMT2, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT2, VSYNCOS_FMT2, HSYNCW_FMT2, VSYNCW_FMT2, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT2, TVV0_FMT2, TVH1_FMT2, TVV1_FMT2, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp720x480p60Hz16t9PwrOnParaTbl[] = {  /* VIC=3. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT3, CLK_NX_FMT3, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT3, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT3, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT3, CLK_1X_DIV_FMT3, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT3, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT3, FIELD_FMT3, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x0, 0x0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT3, VLINE_FMT3, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT3, VSYNCOS_FMT3, HSYNCW_FMT3, VSYNCW_FMT3, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT3, TVV0_FMT3, TVH1_FMT3, TVV1_FMT3, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1280x720p60Hz16t9PwrOnParaTbl[] = {  /* VIC=4. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT4, CLK_NX_FMT4, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT4, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT4, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT4, CLK_1X_DIV_FMT4, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT4, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT4, FIELD_FMT4, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT4, VLINE_FMT4, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT4, VSYNCOS_FMT4, HSYNCW_FMT4, VSYNCW_FMT4, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT4, TVV0_FMT4, TVH1_FMT4, TVV1_FMT4, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1920x1080i60Hz16t9PwrOnParaTbl[] = {  /* VIC=5. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT5, CLK_NX_FMT5, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT5, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT5, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT5, CLK_1X_DIV_FMT5, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT5, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT5, FIELD_FMT5, 0, 0, 0},
	{DISP_PARA_IMG_NONSCALE_INTERLACE_MODE, DISP_NONSCALE_INTRLC_NOSWAP, 0, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT5, VLINE_FMT5, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT5, VSYNCOS_FMT5, HSYNCW_FMT5, VSYNCW_FMT5, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT5, TVV0_FMT5, TVH1_FMT5, TVV1_FMT5, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp720x480i60Hz4t3PwrOnParaTbl[] = {  /* VIC=6. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT6, CLK_NX_FMT6, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT6, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT6, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT6, CLK_1X_DIV_FMT6, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT6, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT6, FIELD_FMT6, 0, 0, 0},
	{DISP_PARA_IMG_NONSCALE_INTERLACE_MODE, DISP_NONSCALE_INTRLC_NOSWAP, 0, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x0, 0x0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 1, 1, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT6, VLINE_FMT6, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT6, VSYNCOS_FMT6, HSYNCW_FMT6, VSYNCW_FMT6, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT6, TVV0_FMT6, TVH1_FMT6, TVV1_FMT6, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp720x480i60Hz16t9PwrOnParaTbl[] = {  /* VIC=7. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT7, CLK_NX_FMT7, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT7, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT7, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT7, CLK_1X_DIV_FMT7, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT7, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT7, FIELD_FMT7, 0, 0, 0},
	{DISP_PARA_IMG_NONSCALE_INTERLACE_MODE, DISP_NONSCALE_INTRLC_NOSWAP, 0, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x0, 0x0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 1, 1, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT7, VLINE_FMT7, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT7, VSYNCOS_FMT7, HSYNCW_FMT7, VSYNCW_FMT7, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT7, TVV0_FMT7, TVH1_FMT7, TVV1_FMT7, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp720x240p60Hz4t3PwrOnParaTbl[] = {  /* VIC=8. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT8, CLK_NX_FMT8, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT8, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT8, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT8, CLK_1X_DIV_FMT8, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT8, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT8, FIELD_FMT8, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x0, 0x0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 1, 1, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT8, VLINE_FMT8, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT8, VSYNCOS_FMT8, HSYNCW_FMT8, VSYNCW_FMT8, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT8, TVV0_FMT8, TVH1_FMT8, TVV1_FMT8, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp720x240p60Hz16t9PwrOnParaTbl[] = {  /* VIC=9. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT9, CLK_NX_FMT9, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT9, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT9, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT9, CLK_1X_DIV_FMT9, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT9, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT9, FIELD_FMT9, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x0, 0x0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 1, 1, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT9, VLINE_FMT9, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT9, VSYNCOS_FMT9, HSYNCW_FMT9, VSYNCW_FMT9, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT9, TVV0_FMT9, TVH1_FMT9, TVV1_FMT9, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp2880x480i60Hz4t3PwrOnParaTbl[] = {  /* VIC=10. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT10, CLK_NX_FMT10, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT10, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT10, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT10, CLK_1X_DIV_FMT10, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT10, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT10, FIELD_FMT10, 0, 0, 0},
	{DISP_PARA_IMG_NONSCALE_INTERLACE_MODE, DISP_NONSCALE_INTRLC_NOSWAP, 0, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x0, 0x0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT10, VLINE_FMT10, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT10, VSYNCOS_FMT10, HSYNCW_FMT10, VSYNCW_FMT10, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT10, TVV0_FMT10, TVH1_FMT10, TVV1_FMT10, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp2880x480i60Hz16t9PwrOnParaTbl[] = {  /* VIC=11. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT11, CLK_NX_FMT11, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT11, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT11, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT11, CLK_1X_DIV_FMT11, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT11, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT11, FIELD_FMT11, 0, 0, 0},
	{DISP_PARA_IMG_NONSCALE_INTERLACE_MODE, DISP_NONSCALE_INTRLC_NOSWAP, 0, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x0, 0x0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT11, VLINE_FMT11, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT11, VSYNCOS_FMT11, HSYNCW_FMT11, VSYNCW_FMT11, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT11, TVV0_FMT11, TVH1_FMT11, TVV1_FMT11, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp2880x240p60Hz4t3PwrOnParaTbl[] = {  /* VIC=12. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT12, CLK_NX_FMT12, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT12, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT12, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT12, CLK_1X_DIV_FMT12, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT12, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT12, FIELD_FMT12, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x0, 0x0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT12, VLINE_FMT12, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT12, VSYNCOS_FMT12, HSYNCW_FMT12, VSYNCW_FMT12, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT12, TVV0_FMT12, TVH1_FMT12, TVV1_FMT12, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp2880x240p60Hz16t9PwrOnParaTbl[] = {  /* VIC=13. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT13, CLK_NX_FMT13, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT13, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT13, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT13, CLK_1X_DIV_FMT13, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT13, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT13, FIELD_FMT13, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x0, 0x0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT13, VLINE_FMT13, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT13, VSYNCOS_FMT13, HSYNCW_FMT13, VSYNCW_FMT13, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT13, TVV0_FMT13, TVH1_FMT13, TVV1_FMT13, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1440x480p60Hz4t3PwrOnParaTbl[] = {  /* VIC=14. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT14, CLK_NX_FMT14, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT14, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT14, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT14, CLK_1X_DIV_FMT14, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT14, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT14, FIELD_FMT14, 0, 0, 0},
	{DISP_PARA_IMG_NONSCALE_INTERLACE_MODE, DISP_NONSCALE_INTRLC_NOSWAP, 0, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x0, 0x0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT14, VLINE_FMT14, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT14, VSYNCOS_FMT14, HSYNCW_FMT14, VSYNCW_FMT14, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT14, TVV0_FMT14, TVH1_FMT14, TVV1_FMT14, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1440x480p60Hz16t9PwrOnParaTbl[] = {  /* VIC=15. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT15, CLK_NX_FMT15, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT15, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT15, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT15, CLK_1X_DIV_FMT15, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT15, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT15, FIELD_FMT15, 0, 0, 0},
	{DISP_PARA_IMG_NONSCALE_INTERLACE_MODE, DISP_NONSCALE_INTRLC_NOSWAP, 0, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x0, 0x0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT15, VLINE_FMT15, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT15, VSYNCOS_FMT15, HSYNCW_FMT15, VSYNCW_FMT15, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT15, TVV0_FMT15, TVH1_FMT15, TVV1_FMT15, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1920x1080p60Hz16t9PwrOnParaTbl[] = {  /* VIC=16. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT16, CLK_NX_FMT16, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT16, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT16, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT16, CLK_1X_DIV_FMT16, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT16, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT16, FIELD_FMT16, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 2, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 0, 31, 0, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT16, VLINE_FMT16, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT16, VSYNCOS_FMT16, HSYNCW_FMT16, VSYNCW_FMT16, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT16, TVV0_FMT16, TVH1_FMT16, TVV1_FMT16, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp720x576p50Hz4t3PwrOnParaTbl[] = {  /* VIC=17. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT17, CLK_NX_FMT17, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT17, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT17, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT17, CLK_1X_DIV_FMT17, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT17, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT17, FIELD_FMT17, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x0, 0x0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT17, VLINE_FMT17, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT17, VSYNCOS_FMT17, HSYNCW_FMT17, VSYNCW_FMT17, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT17, TVV0_FMT17, TVH1_FMT17, TVV1_FMT17, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp720x576p50Hz16t9PwrOnParaTbl[] = {  /* VIC=18. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT18, CLK_NX_FMT18, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT18, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT18, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT18, CLK_1X_DIV_FMT18, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT18, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT18, FIELD_FMT18, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x0, 0x0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT18, VLINE_FMT18, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT18, VSYNCOS_FMT18, HSYNCW_FMT18, VSYNCW_FMT18, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT18, TVV0_FMT18, TVH1_FMT18, TVV1_FMT18, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1280x720p50Hz16t9PwrOnParaTbl[] = {  /* VIC=19. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT19, CLK_NX_FMT19, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT19, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT19, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT19, CLK_1X_DIV_FMT19, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT19, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT19, FIELD_FMT19, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT19, VLINE_FMT19, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT19, VSYNCOS_FMT19, HSYNCW_FMT19, VSYNCW_FMT19, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT19, TVV0_FMT19, TVH1_FMT19, TVV1_FMT19, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1920x1080i50Hz16t9PwrOnParaTbl[] = {  /* VIC=20. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT20, CLK_NX_FMT20, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT20, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT20, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT20, CLK_1X_DIV_FMT20, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT20, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT20, FIELD_FMT20, 0, 0, 0},
	{DISP_PARA_IMG_NONSCALE_INTERLACE_MODE, DISP_NONSCALE_INTRLC_NOSWAP, 0, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT20, VLINE_FMT20, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT20, VSYNCOS_FMT20, HSYNCW_FMT20, VSYNCW_FMT20, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT20, TVV0_FMT20, TVH1_FMT20, TVV1_FMT20, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp720x576i50Hz4t3PwrOnParaTbl[] = {  /* VIC=21. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT21, CLK_NX_FMT21, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT21, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT21, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT21, CLK_1X_DIV_FMT21, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT21, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT21, FIELD_FMT21, 0, 0, 0},
	{DISP_PARA_IMG_NONSCALE_INTERLACE_MODE, DISP_NONSCALE_INTRLC_NOSWAP, 0, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x0, 0x0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 1, 1, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT21, VLINE_FMT21, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT21, VSYNCOS_FMT21, HSYNCW_FMT21, VSYNCW_FMT21, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT21, TVV0_FMT21, TVH1_FMT21, TVV1_FMT21, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp720x576i50Hz16t9PwrOnParaTbl[] = {  /* VIC=22. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT22, CLK_NX_FMT22, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT22, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT22, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT22, CLK_1X_DIV_FMT22, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT22, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT22, FIELD_FMT22, 0, 0, 0},
	{DISP_PARA_IMG_NONSCALE_INTERLACE_MODE, DISP_NONSCALE_INTRLC_NOSWAP, 0, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x0, 0x0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 1, 1, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT22, VLINE_FMT22, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT22, VSYNCOS_FMT22, HSYNCW_FMT22, VSYNCW_FMT22, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT22, TVV0_FMT22, TVH1_FMT22, TVV1_FMT22, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp720x288p50Hz4t3PwrOnParaTbl[] = {  /* VIC=23. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT23, CLK_NX_FMT23, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT23, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT23, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT23, CLK_1X_DIV_FMT23, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT23, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT23, FIELD_FMT23, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x0, 0x0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 1, 1, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT23, VLINE_FMT23, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT23, VSYNCOS_FMT23, HSYNCW_FMT23, VSYNCW_FMT23, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT23, TVV0_FMT23, TVH1_FMT23, TVV1_FMT23, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp720x288p50Hz16t9PwrOnParaTbl[] = {  /* VIC=24. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT24, CLK_NX_FMT24, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT24, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT24, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT24, CLK_1X_DIV_FMT24, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT24, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT24, FIELD_FMT24, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x0, 0x0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 1, 1, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT24, VLINE_FMT24, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT24, VSYNCOS_FMT24, HSYNCW_FMT24, VSYNCW_FMT24, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT24, TVV0_FMT24, TVH1_FMT24, TVV1_FMT24, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp2880x576i50Hz4t3PwrOnParaTbl[] = {  /* VIC=25. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT25, CLK_NX_FMT25, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT25, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT25, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT25, CLK_1X_DIV_FMT25, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT25, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT25, FIELD_FMT25, 0, 0, 0},
	{DISP_PARA_IMG_NONSCALE_INTERLACE_MODE, DISP_NONSCALE_INTRLC_NOSWAP, 0, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x0, 0x0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT25, VLINE_FMT25, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT25, VSYNCOS_FMT25, HSYNCW_FMT25, VSYNCW_FMT25, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT25, TVV0_FMT25, TVH1_FMT25, TVV1_FMT25, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp2880x576i50Hz16t9PwrOnParaTbl[] = {  /* VIC=26. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT26, CLK_NX_FMT26, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT26, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT26, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT26, CLK_1X_DIV_FMT26, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT26, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT26, FIELD_FMT26, 0, 0, 0},
	{DISP_PARA_IMG_NONSCALE_INTERLACE_MODE, DISP_NONSCALE_INTRLC_NOSWAP, 0, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x0, 0x0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT26, VLINE_FMT26, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT26, VSYNCOS_FMT26, HSYNCW_FMT26, VSYNCW_FMT26, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT26, TVV0_FMT26, TVH1_FMT26, TVV1_FMT26, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp2880x288p50Hz4t3PwrOnParaTbl[] = {  /* VIC=27. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT27, CLK_NX_FMT27, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT27, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT27, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT27, CLK_1X_DIV_FMT27, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT27, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT27, FIELD_FMT27, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x0, 0x0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT27, VLINE_FMT27, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT27, VSYNCOS_FMT27, HSYNCW_FMT27, VSYNCW_FMT27, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT27, TVV0_FMT27, TVH1_FMT27, TVV1_FMT27, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp2880x288p50Hz16t9PwrOnParaTbl[] = {  /* VIC=28. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT28, CLK_NX_FMT28, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT28, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT28, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT28, CLK_1X_DIV_FMT28, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT28, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT28, FIELD_FMT28, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x0, 0x0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT28, VLINE_FMT28, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT28, VSYNCOS_FMT28, HSYNCW_FMT28, VSYNCW_FMT28, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT28, TVV0_FMT28, TVH1_FMT28, TVV1_FMT28, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1440x576p50Hz4t3PwrOnParaTbl[] = {  /* VIC=29. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT29, CLK_NX_FMT29, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT29, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT29, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT29, CLK_1X_DIV_FMT29, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT29, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT29, FIELD_FMT29, 0, 0, 0},
	{DISP_PARA_IMG_NONSCALE_INTERLACE_MODE, DISP_NONSCALE_INTRLC_NOSWAP, 0, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x0, 0x0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT29, VLINE_FMT29, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT29, VSYNCOS_FMT29, HSYNCW_FMT29, VSYNCW_FMT29, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT29, TVV0_FMT29, TVH1_FMT29, TVV1_FMT29, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1440x576p50Hz16t9PwrOnParaTbl[] = {  /* VIC=30. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT30, CLK_NX_FMT30, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT30, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT30, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT30, CLK_1X_DIV_FMT30, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT30, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT30, FIELD_FMT30, 0, 0, 0},
	{DISP_PARA_IMG_NONSCALE_INTERLACE_MODE, DISP_NONSCALE_INTRLC_NOSWAP, 0, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x0, 0x0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT30, VLINE_FMT30, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT30, VSYNCOS_FMT30, HSYNCW_FMT30, VSYNCW_FMT30, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT30, TVV0_FMT30, TVH1_FMT30, TVV1_FMT30, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1920x1080p50Hz16t9PwrOnParaTbl[] = {  /* VIC=31. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT31, CLK_NX_FMT31, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT31, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT31, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT31, CLK_1X_DIV_FMT31, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT31, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT31, FIELD_FMT31, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 2, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 0, 31, 0, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT31, VLINE_FMT31, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT31, VSYNCOS_FMT31, HSYNCW_FMT31, VSYNCW_FMT31, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT31, TVV0_FMT31, TVH1_FMT31, TVV1_FMT31, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1920x1080p24Hz16t9PwrOnParaTbl[] = {  /* VIC=32. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT32, CLK_NX_FMT32, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT32, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT32, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT32, CLK_1X_DIV_FMT32, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT32, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT32, FIELD_FMT32, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT32, VLINE_FMT32, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT32, VSYNCOS_FMT32, HSYNCW_FMT32, VSYNCW_FMT32, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT32, TVV0_FMT32, TVH1_FMT32, TVV1_FMT32, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1920x1080p25Hz16t9PwrOnParaTbl[] = {  /* VIC=33. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT33, CLK_NX_FMT33, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT33, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT33, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT33, CLK_1X_DIV_FMT33, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT33, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT33, FIELD_FMT33, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT33, VLINE_FMT33, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT33, VSYNCOS_FMT33, HSYNCW_FMT33, VSYNCW_FMT33, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT33, TVV0_FMT33, TVH1_FMT33, TVV1_FMT33, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1920x1080p30Hz16t9PwrOnParaTbl[] = {  /* VIC=34. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT34, CLK_NX_FMT34, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT34, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT34, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT34, CLK_1X_DIV_FMT34, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT34, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT34, FIELD_FMT34, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT34, VLINE_FMT34, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT34, VSYNCOS_FMT34, HSYNCW_FMT34, VSYNCW_FMT34, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT34, TVV0_FMT34, TVH1_FMT34, TVV1_FMT34, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp2880x480p60Hz4t3PwrOnParaTbl[] = {  /* VIC=35. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT35, CLK_NX_FMT35, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT35, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT35, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT35, CLK_1X_DIV_FMT35, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT35, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT35, FIELD_FMT35, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 2, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 0, 31, 0, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x0, 0x0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT35, VLINE_FMT35, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT35, VSYNCOS_FMT35, HSYNCW_FMT35, VSYNCW_FMT35, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT35, TVV0_FMT35, TVH1_FMT35, TVV1_FMT35, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp2880x480p60Hz16t9PwrOnParaTbl[] = {  /* VIC=36. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT36, CLK_NX_FMT36, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT36, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT36, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT36, CLK_1X_DIV_FMT36, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT36, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT36, FIELD_FMT36, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 2, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 0, 31, 0, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x0, 0x0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT36, VLINE_FMT36, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT36, VSYNCOS_FMT36, HSYNCW_FMT36, VSYNCW_FMT36, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT36, TVV0_FMT36, TVH1_FMT36, TVV1_FMT36, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp2880x576p50Hz4t3PwrOnParaTbl[] = {  /* VIC=37. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT37, CLK_NX_FMT37, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT37, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT37, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT37, CLK_1X_DIV_FMT37, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT37, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT37, FIELD_FMT37, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 2, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 0, 31, 0, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x0, 0x0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT37, VLINE_FMT37, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT37, VSYNCOS_FMT37, HSYNCW_FMT37, VSYNCW_FMT37, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT37, TVV0_FMT37, TVH1_FMT37, TVV1_FMT37, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp2880x576p50Hz16t9PwrOnParaTbl[] = {  /* VIC=38. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT38, CLK_NX_FMT38, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT38, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT38, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT38, CLK_1X_DIV_FMT38, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT38, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT38, FIELD_FMT38, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 2, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 0, 31, 0, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x0, 0x0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT38, VLINE_FMT38, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT38, VSYNCOS_FMT38, HSYNCW_FMT38, VSYNCW_FMT38, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT38, TVV0_FMT38, TVH1_FMT38, TVV1_FMT38, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1920x1080i100Hz16t9PwrOnParaTbl[] = {  /* VIC=40. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT40, CLK_NX_FMT40, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT40, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT40, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT40, CLK_1X_DIV_FMT40, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT40, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT40, FIELD_FMT40, 0, 0, 0},
	{DISP_PARA_IMG_NONSCALE_INTERLACE_MODE, DISP_NONSCALE_INTRLC_NOSWAP, 0, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 2, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 0, 31, 0, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT40, VLINE_FMT40, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT40, VSYNCOS_FMT40, HSYNCW_FMT40, VSYNCW_FMT40, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT40, TVV0_FMT40, TVH1_FMT40, TVV1_FMT40, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1280x720p100Hz16t9PwrOnParaTbl[] = {  /* VIC=41. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT41, CLK_NX_FMT41, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT41, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT41, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT41, CLK_1X_DIV_FMT41, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT41, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT41, FIELD_FMT41, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 2, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 0, 31, 0, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT41, VLINE_FMT41, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT41, VSYNCOS_FMT41, HSYNCW_FMT41, VSYNCW_FMT41, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT41, TVV0_FMT41, TVH1_FMT41, TVV1_FMT41, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp720x576p100Hz4t3PwrOnParaTbl[] = {  /* VIC=42. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT42, CLK_NX_FMT42, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT42, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT42, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT42, CLK_1X_DIV_FMT42, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT42, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT42, FIELD_FMT42, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x0, 0x0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT42, VLINE_FMT42, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT42, VSYNCOS_FMT42, HSYNCW_FMT42, VSYNCW_FMT42, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT42, TVV0_FMT42, TVH1_FMT42, TVV1_FMT42, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp720x576p100Hz16t9PwrOnParaTbl[] = {  /* VIC=43. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT43, CLK_NX_FMT43, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT43, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT43, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT43, CLK_1X_DIV_FMT43, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT43, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT43, FIELD_FMT43, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x0, 0x0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT43, VLINE_FMT43, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT43, VSYNCOS_FMT43, HSYNCW_FMT43, VSYNCW_FMT43, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT43, TVV0_FMT43, TVH1_FMT43, TVV1_FMT43, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp720x576i100Hz4t3PwrOnParaTbl[] = {  /* VIC=44. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT44, CLK_NX_FMT44, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT44, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT44, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT44, CLK_1X_DIV_FMT44, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT44, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT44, FIELD_FMT44, 0, 0, 0},
	{DISP_PARA_IMG_NONSCALE_INTERLACE_MODE, DISP_NONSCALE_INTRLC_NOSWAP, 0, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x0, 0x0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 1, 1, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT44, VLINE_FMT44, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT44, VSYNCOS_FMT44, HSYNCW_FMT44, VSYNCW_FMT44, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT44, TVV0_FMT44, TVH1_FMT44, TVV1_FMT44, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp720x576i100Hz16t9PwrOnParaTbl[] = {  /* VIC=45. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT45, CLK_NX_FMT45, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT45, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT45, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT45, CLK_1X_DIV_FMT45, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT45, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT45, FIELD_FMT45, 0, 0, 0},
	{DISP_PARA_IMG_NONSCALE_INTERLACE_MODE, DISP_NONSCALE_INTRLC_NOSWAP, 0, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x0, 0x0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 1, 1, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT45, VLINE_FMT45, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT45, VSYNCOS_FMT45, HSYNCW_FMT45, VSYNCW_FMT45, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT45, TVV0_FMT45, TVH1_FMT45, TVV1_FMT45, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1920x1080i120Hz16t9PwrOnParaTbl[] = {  /* VIC=46. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT46, CLK_NX_FMT46, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT46, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT46, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT46, CLK_1X_DIV_FMT46, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT46, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT46, FIELD_FMT46, 0, 0, 0},
	{DISP_PARA_IMG_NONSCALE_INTERLACE_MODE, DISP_NONSCALE_INTRLC_NOSWAP, 0, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 2, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 0, 31, 0, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT46, VLINE_FMT46, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT46, VSYNCOS_FMT46, HSYNCW_FMT46, VSYNCW_FMT46, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT46, TVV0_FMT46, TVH1_FMT46, TVV1_FMT46, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1280x720p120Hz16t9PwrOnParaTbl[] = {  /* VIC=47. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT47, CLK_NX_FMT47, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT47, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT47, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT47, CLK_1X_DIV_FMT47, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT47, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT47, FIELD_FMT47, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 2, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 0, 31, 0, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT47, VLINE_FMT47, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT47, VSYNCOS_FMT47, HSYNCW_FMT47, VSYNCW_FMT47, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT47, TVV0_FMT47, TVH1_FMT47, TVV1_FMT47, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp720x480p120Hz4t3PwrOnParaTbl[] = {  /* VIC=48. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT48, CLK_NX_FMT48, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT48, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT48, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT48, CLK_1X_DIV_FMT48, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT48, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT48, FIELD_FMT48, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x0, 0x0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT48, VLINE_FMT48, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT48, VSYNCOS_FMT48, HSYNCW_FMT48, VSYNCW_FMT48, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT48, TVV0_FMT48, TVH1_FMT48, TVV1_FMT48, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp720x480p120Hz16t9PwrOnParaTbl[] = {  /* VIC=49. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT49, CLK_NX_FMT49, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT49, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT49, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT49, CLK_1X_DIV_FMT49, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT49, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT49, FIELD_FMT49, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x0, 0x0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT49, VLINE_FMT49, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT49, VSYNCOS_FMT49, HSYNCW_FMT49, VSYNCW_FMT49, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT49, TVV0_FMT49, TVH1_FMT49, TVV1_FMT49, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp720x480i120Hz4t3PwrOnParaTbl[] = {  /* VIC=50. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT50, CLK_NX_FMT50, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT50, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT50, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT50, CLK_1X_DIV_FMT50, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT50, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT50, FIELD_FMT50, 0, 0, 0},
	{DISP_PARA_IMG_NONSCALE_INTERLACE_MODE, DISP_NONSCALE_INTRLC_NOSWAP, 0, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x0, 0x0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 1, 1, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT50, VLINE_FMT50, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT50, VSYNCOS_FMT50, HSYNCW_FMT50, VSYNCW_FMT50, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT50, TVV0_FMT50, TVH1_FMT50, TVV1_FMT50, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp720x480i120Hz16t9PwrOnParaTbl[] = {  /* VIC=51. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT51, CLK_NX_FMT51, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT51, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT51, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT51, CLK_1X_DIV_FMT51, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT51, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT51, FIELD_FMT51, 0, 0, 0},
	{DISP_PARA_IMG_NONSCALE_INTERLACE_MODE, DISP_NONSCALE_INTRLC_NOSWAP, 0, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x0, 0x0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 1, 1, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT51, VLINE_FMT51, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT51, VSYNCOS_FMT51, HSYNCW_FMT51, VSYNCW_FMT51, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT51, TVV0_FMT51, TVH1_FMT51, TVV1_FMT51, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp720x576p200Hz4t3PwrOnParaTbl[] = {  /* VIC=52. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT52, CLK_NX_FMT52, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT52, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT52, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT52, CLK_1X_DIV_FMT52, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT52, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT52, FIELD_FMT52, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x0, 0x0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT52, VLINE_FMT52, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT52, VSYNCOS_FMT52, HSYNCW_FMT52, VSYNCW_FMT52, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT52, TVV0_FMT52, TVH1_FMT52, TVV1_FMT52, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp720x576p200Hz16t9PwrOnParaTbl[] = {  /* VIC=53. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT53, CLK_NX_FMT53, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT53, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT53, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT53, CLK_1X_DIV_FMT53, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT53, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT53, FIELD_FMT53, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x0, 0x0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT53, VLINE_FMT53, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT53, VSYNCOS_FMT53, HSYNCW_FMT53, VSYNCW_FMT53, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT53, TVV0_FMT53, TVH1_FMT53, TVV1_FMT53, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp720x576i200Hz4t3PwrOnParaTbl[] = {  /* VIC=54. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT54, CLK_NX_FMT54, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT54, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT54, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT54, CLK_1X_DIV_FMT54, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT54, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT54, FIELD_FMT54, 0, 0, 0},
	{DISP_PARA_IMG_NONSCALE_INTERLACE_MODE, DISP_NONSCALE_INTRLC_NOSWAP, 0, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x0, 0x0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 1, 1, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT54, VLINE_FMT54, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT54, VSYNCOS_FMT54, HSYNCW_FMT54, VSYNCW_FMT54, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT54, TVV0_FMT54, TVH1_FMT54, TVV1_FMT54, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp720x576i200Hz16t9PwrOnParaTbl[] = {  /* VIC=55. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT55, CLK_NX_FMT55, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT55, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT55, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT55, CLK_1X_DIV_FMT55, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT55, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT55, FIELD_FMT55, 0, 0, 0},
	{DISP_PARA_IMG_NONSCALE_INTERLACE_MODE, DISP_NONSCALE_INTRLC_NOSWAP, 0, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x0, 0x0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 1, 1, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT55, VLINE_FMT55, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT55, VSYNCOS_FMT55, HSYNCW_FMT55, VSYNCW_FMT55, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT55, TVV0_FMT55, TVH1_FMT55, TVV1_FMT55, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp720x480p240Hz4t3PwrOnParaTbl[] = {  /* VIC=56. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT56, CLK_NX_FMT56, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT56, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT56, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT56, CLK_1X_DIV_FMT56, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT56, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT56, FIELD_FMT56, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x0, 0x0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT56, VLINE_FMT56, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT56, VSYNCOS_FMT56, HSYNCW_FMT56, VSYNCW_FMT56, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT56, TVV0_FMT56, TVH1_FMT56, TVV1_FMT56, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp720x480p240Hz16t9PwrOnParaTbl[] = {  /* VIC=57. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT57, CLK_NX_FMT57, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT57, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT57, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT57, CLK_1X_DIV_FMT57, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT57, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT57, FIELD_FMT57, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x0, 0x0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT57, VLINE_FMT57, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT57, VSYNCOS_FMT57, HSYNCW_FMT57, VSYNCW_FMT57, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT57, TVV0_FMT57, TVH1_FMT57, TVV1_FMT57, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp720x480i240Hz4t3PwrOnParaTbl[] = {  /* VIC=58. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT58, CLK_NX_FMT58, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT58, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT58, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT58, CLK_1X_DIV_FMT58, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT58, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT58, FIELD_FMT58, 0, 0, 0},
	{DISP_PARA_IMG_NONSCALE_INTERLACE_MODE, DISP_NONSCALE_INTRLC_NOSWAP, 0, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x0, 0x0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 1, 1, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT58, VLINE_FMT58, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT58, VSYNCOS_FMT58, HSYNCW_FMT58, VSYNCW_FMT58, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT58, TVV0_FMT58, TVH1_FMT58, TVV1_FMT58, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp720x480i240Hz16t9PwrOnParaTbl[] = {  /* VIC=59. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT59, CLK_NX_FMT59, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT59, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT59, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT59, CLK_1X_DIV_FMT59, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT59, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT59, FIELD_FMT59, 0, 0, 0},
	{DISP_PARA_IMG_NONSCALE_INTERLACE_MODE, DISP_NONSCALE_INTRLC_NOSWAP, 0, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x0, 0x0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 1, 1, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT59, VLINE_FMT59, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT59, VSYNCOS_FMT59, HSYNCW_FMT59, VSYNCW_FMT59, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT59, TVV0_FMT59, TVH1_FMT59, TVV1_FMT59, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1280x720p24Hz16t9PwrOnParaTbl[] = {  /* VIC=60. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT60, CLK_NX_FMT60, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT60, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT60, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT60, CLK_1X_DIV_FMT60, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT60, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT60, FIELD_FMT60, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT60, VLINE_FMT60, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT60, VSYNCOS_FMT60, HSYNCW_FMT60, VSYNCW_FMT60, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT60, TVV0_FMT60, TVH1_FMT60, TVV1_FMT60, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1280x720p25Hz16t9PwrOnParaTbl[] = {  /* VIC=61. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT61, CLK_NX_FMT61, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT61, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT61, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT61, CLK_1X_DIV_FMT61, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT61, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT61, FIELD_FMT61, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT61, VLINE_FMT61, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT61, VSYNCOS_FMT61, HSYNCW_FMT61, VSYNCW_FMT61, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT61, TVV0_FMT61, TVH1_FMT61, TVV1_FMT61, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1280x720p30Hz16t9PwrOnParaTbl[] = {  /* VIC=62. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT62, CLK_NX_FMT62, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT62, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT62, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT62, CLK_1X_DIV_FMT62, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT62, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT62, FIELD_FMT62, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT62, VLINE_FMT62, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT62, VSYNCOS_FMT62, HSYNCW_FMT62, VSYNCW_FMT62, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT62, TVV0_FMT62, TVH1_FMT62, TVV1_FMT62, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1920x1080p120Hz16t9PwrOnParaTbl[] = {  /* VIC=63. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT63, CLK_NX_FMT63, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT63, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT63, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT63, CLK_1X_DIV_FMT63, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT63, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT63, FIELD_FMT63, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 2, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 0, 31, 0, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT63, VLINE_FMT63, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT63, VSYNCOS_FMT63, HSYNCW_FMT63, VSYNCW_FMT63, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT63, TVV0_FMT63, TVH1_FMT63, TVV1_FMT63, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1920x1080p100Hz16t9PwrOnParaTbl[] = {  /* VIC=64. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT64, CLK_NX_FMT64, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT64, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT64, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT64, CLK_1X_DIV_FMT64, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT64, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT64, FIELD_FMT64, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 2, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 0, 31, 0, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT64, VLINE_FMT64, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT64, VSYNCOS_FMT64, HSYNCW_FMT64, VSYNCW_FMT64, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT64, TVV0_FMT64, TVH1_FMT64, TVV1_FMT64, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1280x720p24Hz64t27PwrOnParaTbl[] = {  /* VIC=65. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT65, CLK_NX_FMT65, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT65, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT65, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT65, CLK_1X_DIV_FMT65, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT65, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT65, FIELD_FMT65, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT65, VLINE_FMT65, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT65, VSYNCOS_FMT65, HSYNCW_FMT65, VSYNCW_FMT65, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT65, TVV0_FMT65, TVH1_FMT65, TVV1_FMT65, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1280x720p25Hz64t27PwrOnParaTbl[] = {  /* VIC=66. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT66, CLK_NX_FMT66, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT66, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT66, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT66, CLK_1X_DIV_FMT66, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT66, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT66, FIELD_FMT66, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT66, VLINE_FMT66, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT66, VSYNCOS_FMT66, HSYNCW_FMT66, VSYNCW_FMT66, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT66, TVV0_FMT66, TVH1_FMT66, TVV1_FMT66, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1280x720p30Hz64t27PwrOnParaTbl[] = {  /* VIC=67. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT67, CLK_NX_FMT67, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT67, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT67, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT67, CLK_1X_DIV_FMT67, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT67, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT67, FIELD_FMT67, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT67, VLINE_FMT67, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT67, VSYNCOS_FMT67, HSYNCW_FMT67, VSYNCW_FMT67, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT67, TVV0_FMT67, TVH1_FMT67, TVV1_FMT67, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1280x720p50Hz64t27PwrOnParaTbl[] = {  /* VIC=68. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT68, CLK_NX_FMT68, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT68, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT68, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT68, CLK_1X_DIV_FMT68, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT68, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT68, FIELD_FMT68, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT68, VLINE_FMT68, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT68, VSYNCOS_FMT68, HSYNCW_FMT68, VSYNCW_FMT68, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT68, TVV0_FMT68, TVH1_FMT68, TVV1_FMT68, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1280x720p60Hz64t27PwrOnParaTbl[] = {  /* VIC=69. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT69, CLK_NX_FMT69, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT69, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT69, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT69, CLK_1X_DIV_FMT69, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT69, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT69, FIELD_FMT69, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT69, VLINE_FMT69, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT69, VSYNCOS_FMT69, HSYNCW_FMT69, VSYNCW_FMT69, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT69, TVV0_FMT69, TVH1_FMT69, TVV1_FMT69, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1280x720p100Hz64t27PwrOnParaTbl[] = {  /* VIC=70. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT70, CLK_NX_FMT70, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT70, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT70, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT70, CLK_1X_DIV_FMT70, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT70, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT70, FIELD_FMT70, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 2, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 0, 31, 0, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT70, VLINE_FMT70, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT70, VSYNCOS_FMT70, HSYNCW_FMT70, VSYNCW_FMT70, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT70, TVV0_FMT70, TVH1_FMT70, TVV1_FMT70, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1280x720p120Hz64t27PwrOnParaTbl[] = {  /* VIC=71. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT71, CLK_NX_FMT71, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT71, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT71, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT71, CLK_1X_DIV_FMT71, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT71, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT71, FIELD_FMT71, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 2, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 0, 31, 0, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT71, VLINE_FMT71, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT71, VSYNCOS_FMT71, HSYNCW_FMT71, VSYNCW_FMT71, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT71, TVV0_FMT71, TVH1_FMT71, TVV1_FMT71, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1920x1080p24Hz64t27PwrOnParaTbl[] = {  /* VIC=72. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT72, CLK_NX_FMT72, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT72, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT72, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT72, CLK_1X_DIV_FMT72, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT72, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT72, FIELD_FMT72, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT72, VLINE_FMT72, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT72, VSYNCOS_FMT72, HSYNCW_FMT72, VSYNCW_FMT72, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT72, TVV0_FMT72, TVH1_FMT72, TVV1_FMT72, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1920x1080p25Hz64t27PwrOnParaTbl[] = {  /* VIC=73. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT73, CLK_NX_FMT73, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT73, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT73, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT73, CLK_1X_DIV_FMT73, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT73, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT73, FIELD_FMT73, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT73, VLINE_FMT73, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT73, VSYNCOS_FMT73, HSYNCW_FMT73, VSYNCW_FMT73, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT73, TVV0_FMT73, TVH1_FMT73, TVV1_FMT73, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1920x1080p30Hz64t27PwrOnParaTbl[] = {  /* VIC=74. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT74, CLK_NX_FMT74, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT74, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT74, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT74, CLK_1X_DIV_FMT74, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT74, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT74, FIELD_FMT74, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT74, VLINE_FMT74, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT74, VSYNCOS_FMT74, HSYNCW_FMT74, VSYNCW_FMT74, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT74, TVV0_FMT74, TVH1_FMT74, TVV1_FMT74, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1920x1080p50Hz64t27PwrOnParaTbl[] = {  /* VIC=75. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT75, CLK_NX_FMT75, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT75, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT75, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT75, CLK_1X_DIV_FMT75, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT75, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT75, FIELD_FMT75, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 2, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 0, 31, 0, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT75, VLINE_FMT75, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT75, VSYNCOS_FMT75, HSYNCW_FMT75, VSYNCW_FMT75, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT75, TVV0_FMT75, TVH1_FMT75, TVV1_FMT75, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1920x1080p60Hz64t27PwrOnParaTbl[] = {  /* VIC=76. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT76, CLK_NX_FMT76, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT76, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT76, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT76, CLK_1X_DIV_FMT76, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT76, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT76, FIELD_FMT76, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 2, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 0, 31, 0, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT76, VLINE_FMT76, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT76, VSYNCOS_FMT76, HSYNCW_FMT76, VSYNCW_FMT76, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT76, TVV0_FMT76, TVH1_FMT76, TVV1_FMT76, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1920x1080p100Hz64t27PwrOnParaTbl[] = {  /* VIC=77. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT77, CLK_NX_FMT77, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT77, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT77, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT77, CLK_1X_DIV_FMT77, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT77, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT77, FIELD_FMT77, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 2, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 0, 31, 0, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT77, VLINE_FMT77, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT77, VSYNCOS_FMT77, HSYNCW_FMT77, VSYNCW_FMT77, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT77, TVV0_FMT77, TVH1_FMT77, TVV1_FMT77, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1920x1080p120Hz64t27PwrOnParaTbl[] = {  /* VIC=78. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT78, CLK_NX_FMT78, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT78, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT78, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT78, CLK_1X_DIV_FMT78, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT78, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT78, FIELD_FMT78, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 2, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 0, 31, 0, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT78, VLINE_FMT78, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT78, VSYNCOS_FMT78, HSYNCW_FMT78, VSYNCW_FMT78, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT78, TVV0_FMT78, TVH1_FMT78, TVV1_FMT78, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1680x720p24Hz64t27PwrOnParaTbl[] = {  /* VIC=79. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT79, CLK_NX_FMT79, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT79, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT79, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT79, CLK_1X_DIV_FMT79, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT79, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT79, FIELD_FMT79, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT79, VLINE_FMT79, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT79, VSYNCOS_FMT79, HSYNCW_FMT79, VSYNCW_FMT79, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT79, TVV0_FMT79, TVH1_FMT79, TVV1_FMT79, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1680x720p25Hz64t27PwrOnParaTbl[] = {  /* VIC=80. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT80, CLK_NX_FMT80, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT80, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT80, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT80, CLK_1X_DIV_FMT80, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT80, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT80, FIELD_FMT80, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT80, VLINE_FMT80, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT80, VSYNCOS_FMT80, HSYNCW_FMT80, VSYNCW_FMT80, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT80, TVV0_FMT80, TVH1_FMT80, TVV1_FMT80, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1680x720p30Hz64t27PwrOnParaTbl[] = {  /* VIC=81. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT81, CLK_NX_FMT81, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT81, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT81, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT81, CLK_1X_DIV_FMT81, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT81, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT81, FIELD_FMT81, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT81, VLINE_FMT81, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT81, VSYNCOS_FMT81, HSYNCW_FMT81, VSYNCW_FMT81, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT81, TVV0_FMT81, TVH1_FMT81, TVV1_FMT81, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1680x720p50Hz64t27PwrOnParaTbl[] = {  /* VIC=82. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT82, CLK_NX_FMT82, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT82, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT82, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT82, CLK_1X_DIV_FMT82, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT82, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT82, FIELD_FMT82, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT82, VLINE_FMT82, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT82, VSYNCOS_FMT82, HSYNCW_FMT82, VSYNCW_FMT82, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT82, TVV0_FMT82, TVH1_FMT82, TVV1_FMT82, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1680x720p60Hz64t27PwrOnParaTbl[] = {  /* VIC=83. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT83, CLK_NX_FMT83, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT83, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT83, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT83, CLK_1X_DIV_FMT83, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT83, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT83, FIELD_FMT83, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT83, VLINE_FMT83, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT83, VSYNCOS_FMT83, HSYNCW_FMT83, VSYNCW_FMT83, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT83, TVV0_FMT83, TVH1_FMT83, TVV1_FMT83, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1680x720p100Hz64t27PwrOnParaTbl[] = {  /* VIC=84. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT84, CLK_NX_FMT84, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT84, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT84, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT84, CLK_1X_DIV_FMT84, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT84, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT84, FIELD_FMT84, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 2, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 0, 31, 0, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT84, VLINE_FMT84, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT84, VSYNCOS_FMT84, HSYNCW_FMT84, VSYNCW_FMT84, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT84, TVV0_FMT84, TVH1_FMT84, TVV1_FMT84, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1680x720p120Hz64t27PwrOnParaTbl[] = {  /* VIC=85. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT85, CLK_NX_FMT85, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT85, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT85, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT85, CLK_1X_DIV_FMT85, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT85, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT85, FIELD_FMT85, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 2, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 0, 31, 0, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT85, VLINE_FMT85, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT85, VSYNCOS_FMT85, HSYNCW_FMT85, VSYNCW_FMT85, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT85, TVV0_FMT85, TVH1_FMT85, TVV1_FMT85, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp2560x1080p24Hz64t27PwrOnParaTbl[] = {  /* VIC=86. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT86, CLK_NX_FMT86, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT86, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT86, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT86, CLK_1X_DIV_FMT86, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT86, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT86, FIELD_FMT86, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT86, VLINE_FMT86, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT86, VSYNCOS_FMT86, HSYNCW_FMT86, VSYNCW_FMT86, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT86, TVV0_FMT86, TVH1_FMT86, TVV1_FMT86, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp2560x1080p25Hz64t27PwrOnParaTbl[] = {  /* VIC=87. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT87, CLK_NX_FMT87, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT87, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT87, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT87, CLK_1X_DIV_FMT87, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT87, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT87, FIELD_FMT87, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT87, VLINE_FMT87, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT87, VSYNCOS_FMT87, HSYNCW_FMT87, VSYNCW_FMT87, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT87, TVV0_FMT87, TVH1_FMT87, TVV1_FMT87, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp2560x1080p30Hz64t27PwrOnParaTbl[] = {  /* VIC=88. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT88, CLK_NX_FMT88, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT88, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT88, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT88, CLK_1X_DIV_FMT88, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT88, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT88, FIELD_FMT88, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT88, VLINE_FMT88, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT88, VSYNCOS_FMT88, HSYNCW_FMT88, VSYNCW_FMT88, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT88, TVV0_FMT88, TVH1_FMT88, TVV1_FMT88, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp2560x1080p50Hz64t27PwrOnParaTbl[] = {  /* VIC=89. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT89, CLK_NX_FMT89, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT89, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT89, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT89, CLK_1X_DIV_FMT89, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT89, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT89, FIELD_FMT89, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 2, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 0, 31, 0, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT89, VLINE_FMT89, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT89, VSYNCOS_FMT89, HSYNCW_FMT89, VSYNCW_FMT89, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT89, TVV0_FMT89, TVH1_FMT89, TVV1_FMT89, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp2560x1080p60Hz64t27PwrOnParaTbl[] = {  /* VIC=90. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT90, CLK_NX_FMT90, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT90, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT90, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT90, CLK_1X_DIV_FMT90, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT90, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT90, FIELD_FMT90, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 2, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 0, 31, 0, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT90, VLINE_FMT90, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT90, VSYNCOS_FMT90, HSYNCW_FMT90, VSYNCW_FMT90, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT90, TVV0_FMT90, TVH1_FMT90, TVV1_FMT90, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp2560x1080p100Hz64t27PwrOnParaTbl[] = {  /* VIC=91. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT91, CLK_NX_FMT91, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT91, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT91, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT91, CLK_1X_DIV_FMT91, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT91, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT91, FIELD_FMT91, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 2, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 0, 31, 0, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT91, VLINE_FMT91, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT91, VSYNCOS_FMT91, HSYNCW_FMT91, VSYNCW_FMT91, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT91, TVV0_FMT91, TVH1_FMT91, TVV1_FMT91, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp2560x1080p120Hz64t27PwrOnParaTbl[] = {  /* VIC=92. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT92, CLK_NX_FMT92, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT92, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT92, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT92, CLK_1X_DIV_FMT92, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT92, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT92, FIELD_FMT92, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 2, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 0, 31, 0, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT92, VLINE_FMT92, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT92, VSYNCOS_FMT92, HSYNCW_FMT92, VSYNCW_FMT92, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT92, TVV0_FMT92, TVH1_FMT92, TVV1_FMT92, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp3840x2160p24Hz16t9PwrOnParaTbl[] = {  /* VIC=93. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT93, CLK_NX_FMT93, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT93, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT93, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT93, CLK_1X_DIV_FMT93, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT93, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT93, FIELD_FMT93, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT93, VLINE_FMT93, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT93, VSYNCOS_FMT93, HSYNCW_FMT93, VSYNCW_FMT93, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT93, TVV0_FMT93, TVH1_FMT93, TVV1_FMT93, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp3840x2160p25Hz16t9PwrOnParaTbl[] = {  /* VIC=94. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT94, CLK_NX_FMT94, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT94, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT94, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT94, CLK_1X_DIV_FMT94, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT94, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT94, FIELD_FMT94, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT94, VLINE_FMT94, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT94, VSYNCOS_FMT94, HSYNCW_FMT94, VSYNCW_FMT94, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT94, TVV0_FMT94, TVH1_FMT94, TVV1_FMT94, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp3840x2160p30Hz16t9PwrOnParaTbl[] = {  /* VIC=95. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT95, CLK_NX_FMT95, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT95, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT95, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT95, CLK_1X_DIV_FMT95, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT95, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT95, FIELD_FMT95, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT95, VLINE_FMT95, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT95, VSYNCOS_FMT95, HSYNCW_FMT95, VSYNCW_FMT95, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT95, TVV0_FMT95, TVH1_FMT95, TVV1_FMT95, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp3840x2160p50Hz16t9PwrOnParaTbl[] = {  /* VIC=96. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT96, CLK_NX_FMT96, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT96, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT96, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT96, CLK_1X_DIV_FMT96, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT96, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT96, FIELD_FMT96, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT96, VLINE_FMT96, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT96, VSYNCOS_FMT96, HSYNCW_FMT96, VSYNCW_FMT96, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT96, TVV0_FMT96, TVH1_FMT96, TVV1_FMT96, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp3840x2160p60Hz16t9PwrOnParaTbl[] = {  /* VIC=97. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT97, CLK_NX_FMT97, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT97, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT97, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT97, CLK_1X_DIV_FMT97, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT97, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT97, FIELD_FMT97, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT97, VLINE_FMT97, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT97, VSYNCOS_FMT97, HSYNCW_FMT97, VSYNCW_FMT97, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT97, TVV0_FMT97, TVH1_FMT97, TVV1_FMT97, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp4096x2160p24Hz256t135PwrOnParaTbl[] = {  /* VIC=98. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT98, CLK_NX_FMT98, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT98, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT98, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT98, CLK_1X_DIV_FMT98, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT98, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT98, FIELD_FMT98, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT98, VLINE_FMT98, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT98, VSYNCOS_FMT98, HSYNCW_FMT98, VSYNCW_FMT98, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT98, TVV0_FMT98, TVH1_FMT98, TVV1_FMT98, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp4096x2160p25Hz256t135PwrOnParaTbl[] = {  /* VIC=99. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT99, CLK_NX_FMT99, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT99, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT99, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT99, CLK_1X_DIV_FMT99, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT99, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT99, FIELD_FMT99, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT99, VLINE_FMT99, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT99, VSYNCOS_FMT99, HSYNCW_FMT99, VSYNCW_FMT99, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT99, TVV0_FMT99, TVH1_FMT99, TVV1_FMT99, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp4096x2160p30Hz256t135PwrOnParaTbl[] = {  /* VIC=100. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT100, CLK_NX_FMT100, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT100, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT100, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT100, CLK_1X_DIV_FMT100, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT100, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT100, FIELD_FMT100, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT100, VLINE_FMT100, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT100, VSYNCOS_FMT100, HSYNCW_FMT100, VSYNCW_FMT100, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT100, TVV0_FMT100, TVH1_FMT100, TVV1_FMT100, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp4096x2160p50Hz256t135PwrOnParaTbl[] = {  /* VIC=101. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT101, CLK_NX_FMT101, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT101, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT101, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT101, CLK_1X_DIV_FMT101, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT101, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT101, FIELD_FMT101, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT101, VLINE_FMT101, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT101, VSYNCOS_FMT101, HSYNCW_FMT101, VSYNCW_FMT101, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT101, TVV0_FMT101, TVH1_FMT101, TVV1_FMT101, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp4096x2160p60Hz256t135PwrOnParaTbl[] = {  /* VIC=102. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT102, CLK_NX_FMT102, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT102, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT102, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT102, CLK_1X_DIV_FMT102, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT102, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT102, FIELD_FMT102, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT102, VLINE_FMT102, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT102, VSYNCOS_FMT102, HSYNCW_FMT102, VSYNCW_FMT102, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT102, TVV0_FMT102, TVH1_FMT102, TVV1_FMT102, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp3840x2160p24Hz64t27PwrOnParaTbl[] = {  /* VIC=103. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT103, CLK_NX_FMT103, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT103, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT103, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT103, CLK_1X_DIV_FMT103, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT103, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT103, FIELD_FMT103, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT103, VLINE_FMT103, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT103, VSYNCOS_FMT103, HSYNCW_FMT103, VSYNCW_FMT103, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT103, TVV0_FMT103, TVH1_FMT103, TVV1_FMT103, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp3840x2160p25Hz64t27PwrOnParaTbl[] = {  /* VIC=104. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT104, CLK_NX_FMT104, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT104, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT104, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT104, CLK_1X_DIV_FMT104, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT104, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT104, FIELD_FMT104, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT104, VLINE_FMT104, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT104, VSYNCOS_FMT104, HSYNCW_FMT104, VSYNCW_FMT104, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT104, TVV0_FMT104, TVH1_FMT104, TVV1_FMT104, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp3840x2160p30Hz64t27PwrOnParaTbl[] = {  /* VIC=105. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT105, CLK_NX_FMT105, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT105, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT105, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT105, CLK_1X_DIV_FMT105, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT105, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT105, FIELD_FMT105, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT105, VLINE_FMT105, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT105, VSYNCOS_FMT105, HSYNCW_FMT105, VSYNCW_FMT105, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT105, TVV0_FMT105, TVH1_FMT105, TVV1_FMT105, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp3840x2160p50Hz64t27PwrOnParaTbl[] = {  /* VIC=106. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT106, CLK_NX_FMT106, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT106, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT106, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT106, CLK_1X_DIV_FMT106, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT106, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT106, FIELD_FMT106, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT106, VLINE_FMT106, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT106, VSYNCOS_FMT106, HSYNCW_FMT106, VSYNCW_FMT106, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT106, TVV0_FMT106, TVH1_FMT106, TVV1_FMT106, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp3840x2160p60Hz64t27PwrOnParaTbl[] = {  /* VIC=107. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT107, CLK_NX_FMT107, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT107, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT107, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT107, CLK_1X_DIV_FMT107, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT107, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT107, FIELD_FMT107, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT107, VLINE_FMT107, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT107, VSYNCOS_FMT107, HSYNCW_FMT107, VSYNCW_FMT107, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT107, TVV0_FMT107, TVH1_FMT107, TVV1_FMT107, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t *pdispPwrOnParaTbl[] = {
	[SP5K_EDID_640X480P_60HZ_4T3      ] = disp640x480p60Hz4t3PwrOnParaTbl,        /* VIC=1. */
	[SP5K_EDID_720X480P_60HZ_4T3      ] = disp720x480p60Hz4t3PwrOnParaTbl,        /* VIC=2. */
	[SP5K_EDID_720X480P_60HZ_16T9     ] = disp720x480p60Hz16t9PwrOnParaTbl,       /* VIC=3. */
	[SP5K_EDID_1280X720P_60HZ_16T9    ] = disp1280x720p60Hz16t9PwrOnParaTbl,      /* VIC=4. */
	[SP5K_EDID_1920X1080I_60HZ_16T9   ] = disp1920x1080i60Hz16t9PwrOnParaTbl,     /* VIC=5. */
	[SP5K_EDID_720X480I_60HZ_4T3      ] = disp720x480i60Hz4t3PwrOnParaTbl,        /* VIC=6. */
	[SP5K_EDID_720X480I_60HZ_16T9     ] = disp720x480i60Hz16t9PwrOnParaTbl,       /* VIC=7. */
	[SP5K_EDID_720X240P_60HZ_4T3      ] = disp720x240p60Hz4t3PwrOnParaTbl,        /* VIC=8. */
	[SP5K_EDID_720X240P_60HZ_16T9     ] = disp720x240p60Hz16t9PwrOnParaTbl,       /* VIC=9. */
	[SP5K_EDID_2880X480I_60HZ_4T3     ] = disp2880x480i60Hz4t3PwrOnParaTbl,       /* VIC=10. */
	[SP5K_EDID_2880X480I_60HZ_16T9    ] = disp2880x480i60Hz16t9PwrOnParaTbl,      /* VIC=11. */
	[SP5K_EDID_2880X240P_60HZ_4T3     ] = disp2880x240p60Hz4t3PwrOnParaTbl,       /* VIC=12. */
	[SP5K_EDID_2880X240P_60HZ_16T9    ] = disp2880x240p60Hz16t9PwrOnParaTbl,      /* VIC=13. */
	[SP5K_EDID_1440X480P_60HZ_4T3     ] = disp1440x480p60Hz4t3PwrOnParaTbl,       /* VIC=14. */
	[SP5K_EDID_1440X480P_60HZ_16T9    ] = disp1440x480p60Hz16t9PwrOnParaTbl,      /* VIC=15. */
	[SP5K_EDID_1920X1080P_60HZ_16T9   ] = disp1920x1080p60Hz16t9PwrOnParaTbl,     /* VIC=16. */
	[SP5K_EDID_720X576P_50HZ_4T3      ] = disp720x576p50Hz4t3PwrOnParaTbl,        /* VIC=17. */
	[SP5K_EDID_720X576P_50HZ_16T9     ] = disp720x576p50Hz16t9PwrOnParaTbl,       /* VIC=18. */
	[SP5K_EDID_1280X720P_50HZ_16T9    ] = disp1280x720p50Hz16t9PwrOnParaTbl,      /* VIC=19. */
	[SP5K_EDID_1920X1080I_50HZ_16T9   ] = disp1920x1080i50Hz16t9PwrOnParaTbl,     /* VIC=20. */
	[SP5K_EDID_720X576I_50HZ_4T3      ] = disp720x576i50Hz4t3PwrOnParaTbl,        /* VIC=21. */
	[SP5K_EDID_720X576I_50HZ_16T9     ] = disp720x576i50Hz16t9PwrOnParaTbl,       /* VIC=22. */
	[SP5K_EDID_720X288P_50HZ_4T3      ] = disp720x288p50Hz4t3PwrOnParaTbl,        /* VIC=23. */
	[SP5K_EDID_720X288P_50HZ_16T9     ] = disp720x288p50Hz16t9PwrOnParaTbl,       /* VIC=24. */
	[SP5K_EDID_2880X576I_50HZ_4T3     ] = disp2880x576i50Hz4t3PwrOnParaTbl,       /* VIC=25. */
	[SP5K_EDID_2880X576I_50HZ_16T9    ] = disp2880x576i50Hz16t9PwrOnParaTbl,      /* VIC=26. */
	[SP5K_EDID_2880X288P_50HZ_4T3     ] = disp2880x288p50Hz4t3PwrOnParaTbl,       /* VIC=27. */
	[SP5K_EDID_2880X288P_50HZ_16T9    ] = disp2880x288p50Hz16t9PwrOnParaTbl,      /* VIC=28. */
	[SP5K_EDID_1440X576P_50HZ_4T3     ] = disp1440x576p50Hz4t3PwrOnParaTbl,       /* VIC=29. */
	[SP5K_EDID_1440X576P_50HZ_16T9    ] = disp1440x576p50Hz16t9PwrOnParaTbl,      /* VIC=30. */
	[SP5K_EDID_1920X1080P_50HZ_16T9   ] = disp1920x1080p50Hz16t9PwrOnParaTbl,     /* VIC=31. */
	[SP5K_EDID_1920X1080P_24HZ_16T9   ] = disp1920x1080p24Hz16t9PwrOnParaTbl,     /* VIC=32. */
	[SP5K_EDID_1920X1080P_25HZ_16T9   ] = disp1920x1080p25Hz16t9PwrOnParaTbl,     /* VIC=33. */
	[SP5K_EDID_1920X1080P_30HZ_16T9   ] = disp1920x1080p30Hz16t9PwrOnParaTbl,     /* VIC=34. */
	[SP5K_EDID_2880X480P_60HZ_4T3     ] = disp2880x480p60Hz4t3PwrOnParaTbl,       /* VIC=35. */
	[SP5K_EDID_2880X480P_60HZ_16T9    ] = disp2880x480p60Hz16t9PwrOnParaTbl,      /* VIC=36. */
	[SP5K_EDID_2880X576P_50HZ_4T3     ] = disp2880x576p50Hz4t3PwrOnParaTbl,       /* VIC=37. */
	[SP5K_EDID_2880X576P_50HZ_16T9    ] = disp2880x576p50Hz16t9PwrOnParaTbl,      /* VIC=38. */
	[SP5K_EDID_1920X1080I_100HZ_16T9  ] = disp1920x1080i100Hz16t9PwrOnParaTbl,    /* VIC=40. */
	[SP5K_EDID_1280X720P_100HZ_16T9   ] = disp1280x720p100Hz16t9PwrOnParaTbl,     /* VIC=41. */
	[SP5K_EDID_720X576P_100HZ_4T3     ] = disp720x576p100Hz4t3PwrOnParaTbl,       /* VIC=42. */
	[SP5K_EDID_720X576P_100HZ_16T9    ] = disp720x576p100Hz16t9PwrOnParaTbl,      /* VIC=43. */
	[SP5K_EDID_720X576I_100HZ_4T3     ] = disp720x576i100Hz4t3PwrOnParaTbl,       /* VIC=44. */
	[SP5K_EDID_720X576I_100HZ_16T9    ] = disp720x576i100Hz16t9PwrOnParaTbl,      /* VIC=45. */
	[SP5K_EDID_1920X1080I_120HZ_16T9  ] = disp1920x1080i120Hz16t9PwrOnParaTbl,    /* VIC=46. */
	[SP5K_EDID_1280X720P_120HZ_16T9   ] = disp1280x720p120Hz16t9PwrOnParaTbl,     /* VIC=47. */
	[SP5K_EDID_720X480P_120HZ_4T3     ] = disp720x480p120Hz4t3PwrOnParaTbl,       /* VIC=48. */
	[SP5K_EDID_720X480P_120HZ_16T9    ] = disp720x480p120Hz16t9PwrOnParaTbl,      /* VIC=49. */
	[SP5K_EDID_720X480I_120HZ_4T3     ] = disp720x480i120Hz4t3PwrOnParaTbl,       /* VIC=50. */
	[SP5K_EDID_720X480I_120HZ_16T9    ] = disp720x480i120Hz16t9PwrOnParaTbl,      /* VIC=51. */
	[SP5K_EDID_720X576P_200HZ_4T3     ] = disp720x576p200Hz4t3PwrOnParaTbl,       /* VIC=52. */
	[SP5K_EDID_720X576P_200HZ_16T9    ] = disp720x576p200Hz16t9PwrOnParaTbl,      /* VIC=53. */
	[SP5K_EDID_720X576I_200HZ_4T3     ] = disp720x576i200Hz4t3PwrOnParaTbl,       /* VIC=54. */
	[SP5K_EDID_720X576I_200HZ_16T9    ] = disp720x576i200Hz16t9PwrOnParaTbl,      /* VIC=55. */
	[SP5K_EDID_720X480P_240HZ_4T3     ] = disp720x480p240Hz4t3PwrOnParaTbl,       /* VIC=56. */
	[SP5K_EDID_720X480P_240HZ_16T9    ] = disp720x480p240Hz16t9PwrOnParaTbl,      /* VIC=57. */
	[SP5K_EDID_720X480I_240HZ_4T3     ] = disp720x480i240Hz4t3PwrOnParaTbl,       /* VIC=58. */
	[SP5K_EDID_720X480I_240HZ_16T9    ] = disp720x480i240Hz16t9PwrOnParaTbl,      /* VIC=59. */
	[SP5K_EDID_1280X720P_24HZ_16T9    ] = disp1280x720p24Hz16t9PwrOnParaTbl,      /* VIC=60. */
	[SP5K_EDID_1280X720P_25HZ_16T9    ] = disp1280x720p25Hz16t9PwrOnParaTbl,      /* VIC=61. */
	[SP5K_EDID_1280X720P_30HZ_16T9    ] = disp1280x720p30Hz16t9PwrOnParaTbl,      /* VIC=62. */
	[SP5K_EDID_1920X1080P_120HZ_16T9  ] = disp1920x1080p120Hz16t9PwrOnParaTbl,    /* VIC=63. */
	[SP5K_EDID_1920X1080P_100HZ_16T9  ] = disp1920x1080p100Hz16t9PwrOnParaTbl,    /* VIC=64. */
	[SP5K_EDID_1280X720P_24HZ_64T27   ] = disp1280x720p24Hz64t27PwrOnParaTbl,     /* VIC=65. */
	[SP5K_EDID_1280X720P_25HZ_64T27   ] = disp1280x720p25Hz64t27PwrOnParaTbl,     /* VIC=66. */
	[SP5K_EDID_1280X720P_30HZ_64T27   ] = disp1280x720p30Hz64t27PwrOnParaTbl,     /* VIC=67. */
	[SP5K_EDID_1280X720P_50HZ_64T27   ] = disp1280x720p50Hz64t27PwrOnParaTbl,     /* VIC=68. */
	[SP5K_EDID_1280X720P_60HZ_64T27   ] = disp1280x720p60Hz64t27PwrOnParaTbl,     /* VIC=69. */
	[SP5K_EDID_1280X720P_100HZ_64T27  ] = disp1280x720p100Hz64t27PwrOnParaTbl,    /* VIC=70. */
	[SP5K_EDID_1280X720P_120HZ_64T27  ] = disp1280x720p120Hz64t27PwrOnParaTbl,    /* VIC=71. */
	[SP5K_EDID_1920X1080P_24HZ_64T27  ] = disp1920x1080p24Hz64t27PwrOnParaTbl,    /* VIC=72. */
	[SP5K_EDID_1920X1080P_25HZ_64T27  ] = disp1920x1080p25Hz64t27PwrOnParaTbl,    /* VIC=73. */
	[SP5K_EDID_1920X1080P_30HZ_64T27  ] = disp1920x1080p30Hz64t27PwrOnParaTbl,    /* VIC=74. */
	[SP5K_EDID_1920X1080P_50HZ_64T27  ] = disp1920x1080p50Hz64t27PwrOnParaTbl,    /* VIC=75. */
	[SP5K_EDID_1920X1080P_60HZ_64T27  ] = disp1920x1080p60Hz64t27PwrOnParaTbl,    /* VIC=76. */
	[SP5K_EDID_1920X1080P_100HZ_64T27 ] = disp1920x1080p100Hz64t27PwrOnParaTbl,   /* VIC=77. */
	[SP5K_EDID_1920X1080P_120HZ_64T27 ] = disp1920x1080p120Hz64t27PwrOnParaTbl,   /* VIC=78. */
	[SP5K_EDID_1680X720P_24HZ_64T27   ] = disp1680x720p24Hz64t27PwrOnParaTbl,     /* VIC=79. */
	[SP5K_EDID_1680X720P_25HZ_64T27   ] = disp1680x720p25Hz64t27PwrOnParaTbl,     /* VIC=80. */
	[SP5K_EDID_1680X720P_30HZ_64T27   ] = disp1680x720p30Hz64t27PwrOnParaTbl,     /* VIC=81. */
	[SP5K_EDID_1680X720P_50HZ_64T27   ] = disp1680x720p50Hz64t27PwrOnParaTbl,     /* VIC=82. */
	[SP5K_EDID_1680X720P_60HZ_64T27   ] = disp1680x720p60Hz64t27PwrOnParaTbl,     /* VIC=83. */
	[SP5K_EDID_1680X720P_100HZ_64T27  ] = disp1680x720p100Hz64t27PwrOnParaTbl,    /* VIC=84. */
	[SP5K_EDID_1680X720P_120HZ_64T27  ] = disp1680x720p120Hz64t27PwrOnParaTbl,    /* VIC=85. */
	[SP5K_EDID_2560X1080P_24HZ_64T27  ] = disp2560x1080p24Hz64t27PwrOnParaTbl,    /* VIC=86. */
	[SP5K_EDID_2560X1080P_25HZ_64T27  ] = disp2560x1080p25Hz64t27PwrOnParaTbl,    /* VIC=87. */
	[SP5K_EDID_2560X1080P_30HZ_64T27  ] = disp2560x1080p30Hz64t27PwrOnParaTbl,    /* VIC=88. */
	[SP5K_EDID_2560X1080P_50HZ_64T27  ] = disp2560x1080p50Hz64t27PwrOnParaTbl,    /* VIC=89. */
	[SP5K_EDID_2560X1080P_60HZ_64T27  ] = disp2560x1080p60Hz64t27PwrOnParaTbl,    /* VIC=90. */
	[SP5K_EDID_2560X1080P_100HZ_64T27 ] = disp2560x1080p100Hz64t27PwrOnParaTbl,   /* VIC=91. */
	[SP5K_EDID_2560X1080P_120HZ_64T27 ] = disp2560x1080p120Hz64t27PwrOnParaTbl,   /* VIC=92. */
	[SP5K_EDID_3840X2160P_24HZ_16T9   ] = disp3840x2160p24Hz16t9PwrOnParaTbl,     /* VIC=93. */
	[SP5K_EDID_3840X2160P_25HZ_16T9   ] = disp3840x2160p25Hz16t9PwrOnParaTbl,     /* VIC=94. */
	[SP5K_EDID_3840X2160P_30HZ_16T9   ] = disp3840x2160p30Hz16t9PwrOnParaTbl,     /* VIC=95. */
	[SP5K_EDID_3840X2160P_50HZ_16T9   ] = disp3840x2160p50Hz16t9PwrOnParaTbl,     /* VIC=96. */
	[SP5K_EDID_3840X2160P_60HZ_16T9   ] = disp3840x2160p60Hz16t9PwrOnParaTbl,     /* VIC=97. */
	[SP5K_EDID_4096X2160P_24HZ_256T135] = disp4096x2160p24Hz256t135PwrOnParaTbl,  /* VIC=98. */
	[SP5K_EDID_4096X2160P_25HZ_256T135] = disp4096x2160p25Hz256t135PwrOnParaTbl,  /* VIC=99. */
	[SP5K_EDID_4096X2160P_30HZ_256T135] = disp4096x2160p30Hz256t135PwrOnParaTbl,  /* VIC=100. */
	[SP5K_EDID_4096X2160P_50HZ_256T135] = disp4096x2160p50Hz256t135PwrOnParaTbl,  /* VIC=101. */
	[SP5K_EDID_4096X2160P_60HZ_256T135] = disp4096x2160p60Hz256t135PwrOnParaTbl,  /* VIC=102. */
	[SP5K_EDID_3840X2160P_24HZ_64T27  ] = disp3840x2160p24Hz64t27PwrOnParaTbl,    /* VIC=103. */
	[SP5K_EDID_3840X2160P_25HZ_64T27  ] = disp3840x2160p25Hz64t27PwrOnParaTbl,    /* VIC=104. */
	[SP5K_EDID_3840X2160P_30HZ_64T27  ] = disp3840x2160p30Hz64t27PwrOnParaTbl,    /* VIC=105. */
	[SP5K_EDID_3840X2160P_50HZ_64T27  ] = disp3840x2160p50Hz64t27PwrOnParaTbl,    /* VIC=106. */
	[SP5K_EDID_3840X2160P_60HZ_64T27  ] = disp3840x2160p60Hz64t27PwrOnParaTbl,    /* VIC=107. */
};

static dispPara_t disp1280x720p60Hz16t9Fp3dPwrOnParaTbl[] = {  /* VIC=4. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT4_3D_FP, CLK_NX_FMT4_3D_FP, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT4_3D_FP, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT4_3D_FP, 0, 0, 0, 0},
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT4_3D_FP, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT4_3D_FP, FIELD_FMT4_3D_FP, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 2, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 0, 31, 0, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set active space region */
	{DISP_PARA_3D_ENABLE, 1, 0, 0, 0, 0},
	{DISP_PARA_3D_ACT_SPACE_COLOR, 0, 0, 255, 0, 0},  /* VACT SPACE SET BULE */
	{DISP_PARA_3D_ACT_SPACE_REGION, 0x1, SPCV0_FMT4_3D_FP, 0, SPCV1_FMT4_3D_FP, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT4_3D_FP, VLINE_FMT4_3D_FP, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT4_3D_FP, VSYNCOS_FMT4_3D_FP, HSYNCW_FMT4_3D_FP, VSYNCW_FMT4_3D_FP, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT4_3D_FP, TVV0_FMT4_3D_FP, TVH1_FMT4_3D_FP, TVV1_FMT4_3D_FP, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1280x720p50Hz16t9Fp3dPwrOnParaTbl[] = {  /* VIC=19. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT19_3D_FP, CLK_NX_FMT19_3D_FP, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT19_3D_FP, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT19_3D_FP, 0, 0, 0, 0},
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT19_3D_FP, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT19_3D_FP, FIELD_FMT19_3D_FP, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 2, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 0, 31, 0, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set active space region */
	{DISP_PARA_3D_ENABLE, 1, 0, 0, 0, 0},
	{DISP_PARA_3D_ACT_SPACE_COLOR, 0, 0, 255, 0, 0},  /* VACT SPACE SET BULE */
	{DISP_PARA_3D_ACT_SPACE_REGION, 0x1, SPCV0_FMT19_3D_FP, 0, SPCV1_FMT19_3D_FP, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT19_3D_FP, VLINE_FMT19_3D_FP, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT19_3D_FP, VSYNCOS_FMT19_3D_FP, HSYNCW_FMT19_3D_FP, VSYNCW_FMT19_3D_FP, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT19_3D_FP, TVV0_FMT19_3D_FP, TVH1_FMT19_3D_FP, TVV1_FMT19_3D_FP, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1920x1080p24Hz16t9Fp3dPwrOnParaTbl[] = {  /* VIC=32. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT32_3D_FP, CLK_NX_FMT32_3D_FP, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT32_3D_FP, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT32_3D_FP, 0, 0, 0, 0},
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT32_3D_FP, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT32_3D_FP, FIELD_FMT32_3D_FP, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 2, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 0, 31, 0, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set active space region */
	{DISP_PARA_3D_ENABLE, 1, 0, 0, 0, 0},
	{DISP_PARA_3D_ACT_SPACE_COLOR, 0, 0, 255, 0, 0},  /* VACT SPACE SET BULE */
	{DISP_PARA_3D_ACT_SPACE_REGION, 0x1, SPCV0_FMT32_3D_FP, 0, SPCV1_FMT32_3D_FP, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT32_3D_FP, VLINE_FMT32_3D_FP, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT32_3D_FP, VSYNCOS_FMT32_3D_FP, HSYNCW_FMT32_3D_FP, VSYNCW_FMT32_3D_FP, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT32_3D_FP, TVV0_FMT32_3D_FP, TVH1_FMT32_3D_FP, TVV1_FMT32_3D_FP, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1920x1080p25Hz16t9Fp3dPwrOnParaTbl[] = {  /* VIC=33. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT33_3D_FP, CLK_NX_FMT33_3D_FP, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT33_3D_FP, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT33_3D_FP, 0, 0, 0, 0},
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT33_3D_FP, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT33_3D_FP, FIELD_FMT33_3D_FP, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 2, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 0, 31, 0, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set active space region */
	{DISP_PARA_3D_ENABLE, 1, 0, 0, 0, 0},
	{DISP_PARA_3D_ACT_SPACE_COLOR, 0, 0, 255, 0, 0},  /* VACT SPACE SET BULE */
	{DISP_PARA_3D_ACT_SPACE_REGION, 0x1, SPCV0_FMT33_3D_FP, 0, SPCV1_FMT33_3D_FP, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT33_3D_FP, VLINE_FMT33_3D_FP, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT33_3D_FP, VSYNCOS_FMT33_3D_FP, HSYNCW_FMT33_3D_FP, VSYNCW_FMT33_3D_FP, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT33_3D_FP, TVV0_FMT33_3D_FP, TVH1_FMT33_3D_FP, TVV1_FMT33_3D_FP, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1920x1080p30Hz16t9Fp3dPwrOnParaTbl[] = {  /* VIC=34. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT34_3D_FP, CLK_NX_FMT34_3D_FP, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT34_3D_FP, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT34_3D_FP, 0, 0, 0, 0},
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT34_3D_FP, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT34_3D_FP, FIELD_FMT34_3D_FP, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 2, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 0, 31, 0, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set active space region */
	{DISP_PARA_3D_ENABLE, 1, 0, 0, 0, 0},
	{DISP_PARA_3D_ACT_SPACE_COLOR, 0, 0, 255, 0, 0},  /* VACT SPACE SET BULE */
	{DISP_PARA_3D_ACT_SPACE_REGION, 0x1, SPCV0_FMT34_3D_FP, 0, SPCV1_FMT34_3D_FP, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT34_3D_FP, VLINE_FMT34_3D_FP, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT34_3D_FP, VSYNCOS_FMT34_3D_FP, HSYNCW_FMT34_3D_FP, VSYNCW_FMT34_3D_FP, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT34_3D_FP, TVV0_FMT34_3D_FP, TVH1_FMT34_3D_FP, TVV1_FMT34_3D_FP, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t *pdispPwrOnFp3dParaTbl[] = {
	[SP5K_EDID_1280X720P_60HZ_16T9 ] = disp1280x720p60Hz16t9Fp3dPwrOnParaTbl,   /* VIC=4. */
	[SP5K_EDID_1280X720P_50HZ_16T9 ] = disp1280x720p50Hz16t9Fp3dPwrOnParaTbl,   /* VIC=19. */
	[SP5K_EDID_1920X1080P_24HZ_16T9] = disp1920x1080p24Hz16t9Fp3dPwrOnParaTbl,  /* VIC=32. */
	[SP5K_EDID_1920X1080P_25HZ_16T9] = disp1920x1080p25Hz16t9Fp3dPwrOnParaTbl,  /* VIC=33. */
	[SP5K_EDID_1920X1080P_30HZ_16T9] = disp1920x1080p30Hz16t9Fp3dPwrOnParaTbl,  /* VIC=34. */
};

static dispPara_t disp1280x720p60Hz16t9Tab3dPwrOnParaTbl[] = {  /* VIC=4. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT4_3D_TAB, CLK_NX_FMT4_3D_TAB, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT4_3D_TAB, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT4_3D_TAB, 0, 0, 0, 0},
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT4_3D_TAB, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT4_3D_TAB, FIELD_FMT4_3D_TAB, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 2, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 0, 31, 0, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT4_3D_TAB, VLINE_FMT4_3D_TAB, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT4_3D_TAB, VSYNCOS_FMT4_3D_TAB, HSYNCW_FMT4_3D_TAB, VSYNCW_FMT4_3D_TAB, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT4_3D_TAB, TVV0_FMT4_3D_TAB, TVH1_FMT4_3D_TAB, TVV1_FMT4_3D_TAB, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1920x1080p60Hz16t9Tab3dPwrOnParaTbl[] = {  /* VIC=16. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT16_3D_TAB, CLK_NX_FMT16_3D_TAB, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT16_3D_TAB, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT16_3D_TAB, 0, 0, 0, 0},
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT16_3D_TAB, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT16_3D_TAB, FIELD_FMT16_3D_TAB, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 2, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 0, 31, 0, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT16_3D_TAB, VLINE_FMT16_3D_TAB, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT16_3D_TAB, VSYNCOS_FMT16_3D_TAB, HSYNCW_FMT16_3D_TAB, VSYNCW_FMT16_3D_TAB, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT16_3D_TAB, TVV0_FMT16_3D_TAB, TVH1_FMT16_3D_TAB, TVV1_FMT16_3D_TAB, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1280x720p50Hz16t9Tab3dPwrOnParaTbl[] = {  /* VIC=19. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT19_3D_TAB, CLK_NX_FMT19_3D_TAB, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT19_3D_TAB, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT19_3D_TAB, 0, 0, 0, 0},
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT19_3D_TAB, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT19_3D_TAB, FIELD_FMT19_3D_TAB, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 2, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 0, 31, 0, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT19_3D_TAB, VLINE_FMT19_3D_TAB, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT19_3D_TAB, VSYNCOS_FMT19_3D_TAB, HSYNCW_FMT19_3D_TAB, VSYNCW_FMT19_3D_TAB, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT19_3D_TAB, TVV0_FMT19_3D_TAB, TVH1_FMT19_3D_TAB, TVV1_FMT19_3D_TAB, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1920x1080p50Hz16t9Tab3dPwrOnParaTbl[] = {  /* VIC=31. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT31_3D_TAB, CLK_NX_FMT31_3D_TAB, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT31_3D_TAB, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT31_3D_TAB, 0, 0, 0, 0},
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT31_3D_TAB, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT31_3D_TAB, FIELD_FMT31_3D_TAB, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 2, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 0, 31, 0, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT31_3D_TAB, VLINE_FMT31_3D_TAB, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT31_3D_TAB, VSYNCOS_FMT31_3D_TAB, HSYNCW_FMT31_3D_TAB, VSYNCW_FMT31_3D_TAB, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT31_3D_TAB, TVV0_FMT31_3D_TAB, TVH1_FMT31_3D_TAB, TVV1_FMT31_3D_TAB, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1920x1080p24Hz16t9Tab3dPwrOnParaTbl[] = {  /* VIC=32. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT32_3D_TAB, CLK_NX_FMT32_3D_TAB, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT32_3D_TAB, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT32_3D_TAB, 0, 0, 0, 0},
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT32_3D_TAB, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT32_3D_TAB, FIELD_FMT32_3D_TAB, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 2, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 0, 31, 0, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 1, 3, 0, 0},  /* high gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT32_3D_TAB, VLINE_FMT32_3D_TAB, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT32_3D_TAB, VSYNCOS_FMT32_3D_TAB, HSYNCW_FMT32_3D_TAB, VSYNCW_FMT32_3D_TAB, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT32_3D_TAB, TVV0_FMT32_3D_TAB, TVH1_FMT32_3D_TAB, TVV1_FMT32_3D_TAB, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t *pdispPwrOnTab3dParaTbl[] = {
	[SP5K_EDID_1280X720P_60HZ_16T9 ] = disp1280x720p60Hz16t9Tab3dPwrOnParaTbl,   /* VIC=4. */
	[SP5K_EDID_1920X1080P_60HZ_16T9] = disp1920x1080p60Hz16t9Tab3dPwrOnParaTbl,  /* VIC=16. */
	[SP5K_EDID_1280X720P_50HZ_16T9 ] = disp1280x720p50Hz16t9Tab3dPwrOnParaTbl,   /* VIC=19. */
	[SP5K_EDID_1920X1080P_50HZ_16T9] = disp1920x1080p50Hz16t9Tab3dPwrOnParaTbl,  /* VIC=31. */
	[SP5K_EDID_1920X1080P_24HZ_16T9] = disp1920x1080p24Hz16t9Tab3dPwrOnParaTbl,  /* VIC=32. */
};

static dispPara_t disp1280x720p60Hz16t9Sbshhss3dPwrOnParaTbl[] = {  /* VIC=4. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT4_3D_SBSHHSS, CLK_NX_FMT4_3D_SBSHHSS, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT4_3D_SBSHHSS, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT4_3D_SBSHHSS, 0, 0, 0, 0},
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT4_3D_SBSHHSS, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT4_3D_SBSHHSS, FIELD_FMT4_3D_SBSHHSS, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT4_3D_SBSHHSS, VLINE_FMT4_3D_SBSHHSS, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT4_3D_SBSHHSS, VSYNCOS_FMT4_3D_SBSHHSS, HSYNCW_FMT4_3D_SBSHHSS, VSYNCW_FMT4_3D_SBSHHSS, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT4_3D_SBSHHSS, TVV0_FMT4_3D_SBSHHSS, TVH1_FMT4_3D_SBSHHSS, TVV1_FMT4_3D_SBSHHSS, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1920x1080i60Hz16t9Sbshhss3dPwrOnParaTbl[] = {  /* VIC=5. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT5_3D_SBSHHSS, CLK_NX_FMT5_3D_SBSHHSS, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT5_3D_SBSHHSS, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT5_3D_SBSHHSS, 0, 0, 0, 0},
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT5_3D_SBSHHSS, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT5_3D_SBSHHSS, FIELD_FMT5_3D_SBSHHSS, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT5_3D_SBSHHSS, VLINE_FMT5_3D_SBSHHSS, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT5_3D_SBSHHSS, VSYNCOS_FMT5_3D_SBSHHSS, HSYNCW_FMT5_3D_SBSHHSS, VSYNCW_FMT5_3D_SBSHHSS, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT5_3D_SBSHHSS, TVV0_FMT5_3D_SBSHHSS, TVH1_FMT5_3D_SBSHHSS, TVV1_FMT5_3D_SBSHHSS, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1280x720p50Hz16t9Sbshhss3dPwrOnParaTbl[] = {  /* VIC=19. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT19_3D_SBSHHSS, CLK_NX_FMT19_3D_SBSHHSS, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT19_3D_SBSHHSS, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT19_3D_SBSHHSS, 0, 0, 0, 0},
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT19_3D_SBSHHSS, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT19_3D_SBSHHSS, FIELD_FMT19_3D_SBSHHSS, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT19_3D_SBSHHSS, VLINE_FMT19_3D_SBSHHSS, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT19_3D_SBSHHSS, VSYNCOS_FMT19_3D_SBSHHSS, HSYNCW_FMT19_3D_SBSHHSS, VSYNCW_FMT19_3D_SBSHHSS, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT19_3D_SBSHHSS, TVV0_FMT19_3D_SBSHHSS, TVH1_FMT19_3D_SBSHHSS, TVV1_FMT19_3D_SBSHHSS, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1920x1080i50Hz16t9Sbshhss3dPwrOnParaTbl[] = {  /* VIC=20. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT20_3D_SBSHHSS, CLK_NX_FMT20_3D_SBSHHSS, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT20_3D_SBSHHSS, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT20_3D_SBSHHSS, 0, 0, 0, 0},
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT20_3D_SBSHHSS, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT20_3D_SBSHHSS, FIELD_FMT20_3D_SBSHHSS, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT20_3D_SBSHHSS, VLINE_FMT20_3D_SBSHHSS, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT20_3D_SBSHHSS, VSYNCOS_FMT20_3D_SBSHHSS, HSYNCW_FMT20_3D_SBSHHSS, VSYNCW_FMT20_3D_SBSHHSS, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT20_3D_SBSHHSS, TVV0_FMT20_3D_SBSHHSS, TVH1_FMT20_3D_SBSHHSS, TVV1_FMT20_3D_SBSHHSS, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1920x1080p24Hz16t9Sbshhss3dPwrOnParaTbl[] = {  /* VIC=32. */
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT32_3D_SBSHHSS, CLK_NX_FMT32_3D_SBSHHSS, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, CLK_SCL_FMT32_3D_SBSHHSS, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_CLK_FREQ, CLK_PIXEL_FMT32_3D_SBSHHSS, 0, 0, 0, 0},
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT32_3D_SBSHHSS, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT32_3D_SBSHHSS, FIELD_FMT32_3D_SBSHHSS, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0},

	/* hdmi power and format. */
	{DISP_PARA_HDMI_PRE_EMPHASIS, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_CHARGE_PUMP, 3, 7, 3, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 1, 1, 0},  /* low gain. */
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x1, 0, 0, 0},
	{DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0},

	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT32_3D_SBSHHSS, VLINE_FMT32_3D_SBSHHSS, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT32_3D_SBSHHSS, VSYNCOS_FMT32_3D_SBSHHSS, HSYNCW_FMT32_3D_SBSHHSS, VSYNCW_FMT32_3D_SBSHHSS, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, TVH0_FMT32_3D_SBSHHSS, TVV0_FMT32_3D_SBSHHSS, TVH1_FMT32_3D_SBSHHSS, TVV1_FMT32_3D_SBSHHSS, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t *pdispPwrOnSbshhss3dParaTbl[] = {
	[SP5K_EDID_1280X720P_60HZ_16T9 ] = disp1280x720p60Hz16t9Sbshhss3dPwrOnParaTbl,   /* VIC=4. */
	[SP5K_EDID_1920X1080I_60HZ_16T9] = disp1920x1080i60Hz16t9Sbshhss3dPwrOnParaTbl,  /* VIC=5. */
	[SP5K_EDID_1280X720P_50HZ_16T9 ] = disp1280x720p50Hz16t9Sbshhss3dPwrOnParaTbl,   /* VIC=19. */
	[SP5K_EDID_1920X1080I_50HZ_16T9] = disp1920x1080i50Hz16t9Sbshhss3dPwrOnParaTbl,  /* VIC=20. */
	[SP5K_EDID_1920X1080P_24HZ_16T9] = disp1920x1080p24Hz16t9Sbshhss3dPwrOnParaTbl,  /* VIC=32. */
};

static dispPara_t dispPwrOffParaTbl[] = {
	{DISP_PARA_3D_ENABLE, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PACKET_ENABLE, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_DVI_ENABLE, 0, 0, 0, 0, 0},
	{DISP_PARA_HDMI_PHY_POWER, 0, 0, 0, 0, 0},
	{DISP_PARA_LCM_DATA_PACKET, 0, 0, 0, 0, 0},
	{DISP_PARA_IMG_NONSCALE_INTERLACE_MODE, DISP_NONSCALE_INTRLC_DISABLE, 0, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_BALANCE, 0, 0, 0, 1},

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
	dispOpFunc.pfOpCfgSet        = dispOpCfgSet;
	ret = sp5kDispDevReg(SP5K_DISP_CHNL_1, SP5K_DISP_CHNL_1_HDMI_TV, &dispOpFunc);

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

	if ( !dispDviEn ) {
		UINT32 vic, v3d;
		vic = dispResId & SP5K_EDID_VIC_MSK;
		v3d = dispResId & SP5K_EDID_3D_STRUCT_MSK;

		strcpy((char *)dispDevName, "HDMI - ");
		strcat((char *)dispDevName, (char *)pdispDevName[vic]);
		switch ( v3d ) {
		case SP5K_EDID_3D_FP:
			strcat((char *)dispDevName, " 3DFP");
			break;
		case SP5K_EDID_3D_TAB:
			strcat((char *)dispDevName, " 3DTAB");
			break;
		case SP5K_EDID_3D_SBSHHSS:
			strcat((char *)dispDevName, " 3DSBSHHSS");
			break;
		}
	}

	DBG_FUNC_RETURN(dispDevName, "%s", dispDevName);
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
	UINT32 vic, v3d, isHiPll, pktCfg;


	DBG_FUNC_ENTER(NULL);
    vic = v3d = 0; /*For compiler warnning*/

	isHiPll = 0;  /* 594MHz. */
	if ( dispDviEn ) {
		DBG_LINE_PRINT("DVI mode");

		if ( !(dispDtlTime.flags & 0x80) && dispDtlTime.horActive >= 1920 ) {
			isHiPll = 1;
		}
	}
	else {
		DBG_LINE_PRINT("HDMI mode");

		vic = dispResId & SP5K_EDID_VIC_MSK;
		DBG_LINE_PRINT("video id code=%d", vic);
		v3d = dispResId & SP5K_EDID_3D_STRUCT_MSK;
		if ( v3d ) {
			DBG_LINE_PRINT("3d structure=%d", (v3d >> 4) & 0xF0);
		}
		else {
			DBG_LINE_PRINT("no 3D");
		}

		if ( v3d ) {
			switch ( v3d ) {
			case SP5K_EDID_3D_FP:
			case SP5K_EDID_3D_TAB:
				isHiPll = 1;  /* 891MHz. */
				break;
			}
		}
		else {
			switch ( vic ) {
			case SP5K_EDID_1920X1080P_60HZ_16T9:
			case SP5K_EDID_1920X1080P_50HZ_16T9:
			case SP5K_EDID_2880X480P_60HZ_4T3:
			case SP5K_EDID_2880X480P_60HZ_16T9:
			case SP5K_EDID_2880X576P_50HZ_4T3:
			case SP5K_EDID_2880X576P_50HZ_16T9:
			case SP5K_EDID_1920X1080I_100HZ_16T9:
			case SP5K_EDID_1280X720P_100HZ_16T9:
			case SP5K_EDID_1920X1080I_120HZ_16T9:
			case SP5K_EDID_1280X720P_120HZ_16T9:
			case SP5K_EDID_1920X1080P_120HZ_16T9:
			case SP5K_EDID_1920X1080P_100HZ_16T9:
			case SP5K_EDID_1920X1080P_24HZ_64T27:
			case SP5K_EDID_1920X1080P_25HZ_64T27:
			case SP5K_EDID_1920X1080P_30HZ_64T27:
			case SP5K_EDID_1920X1080P_50HZ_64T27:
			case SP5K_EDID_1920X1080P_60HZ_64T27:
			case SP5K_EDID_1920X1080P_100HZ_64T27:
			case SP5K_EDID_1920X1080P_120HZ_64T27:
			case SP5K_EDID_2560X1080P_24HZ_64T27:
			case SP5K_EDID_2560X1080P_25HZ_64T27:
			case SP5K_EDID_2560X1080P_30HZ_64T27:
			case SP5K_EDID_2560X1080P_50HZ_64T27:
			case SP5K_EDID_2560X1080P_60HZ_64T27:
			case SP5K_EDID_2560X1080P_100HZ_64T27:
			case SP5K_EDID_2560X1080P_120HZ_64T27:
			case SP5K_EDID_3840X2160P_24HZ_16T9:
			case SP5K_EDID_3840X2160P_25HZ_16T9:
			case SP5K_EDID_3840X2160P_30HZ_16T9:
			case SP5K_EDID_3840X2160P_50HZ_16T9:
			case SP5K_EDID_3840X2160P_60HZ_16T9:
			case SP5K_EDID_4096X2160P_24HZ_256T135:
			case SP5K_EDID_4096X2160P_25HZ_256T135:
			case SP5K_EDID_4096X2160P_30HZ_256T135:
			case SP5K_EDID_4096X2160P_50HZ_256T135:
			case SP5K_EDID_4096X2160P_60HZ_256T135:
			case SP5K_EDID_3840X2160P_24HZ_64T27:
			case SP5K_EDID_3840X2160P_25HZ_64T27:
			case SP5K_EDID_3840X2160P_30HZ_64T27:
			case SP5K_EDID_3840X2160P_50HZ_64T27:
			case SP5K_EDID_3840X2160P_60HZ_64T27:
				isHiPll = 1;
				break;
			}
		}
	}

#if 1  /* for HDMI CTS Test ID 7-10. */
	if ( isHiPll ) {
		DBG_LINE_PRINT("HI PLL");

		WRITE8(0xB0000260, READ8(0xB0000260) | 0x1);  /* enable vco free run. */
		WRITE16(0xB000027A, 0x21);  /* hdmi pll frequency. */
		#if 0
		WRITE8(0xB0000296, 0x1);  /* hdmi pll clkvco=fckout/3. */
		#endif
		#if 1
		WRITE8(0xB000026C, (READ8(0xB000026C) & ~0xF) | 0x1);  /* hdmi pll free run frequency. */
		WRITE8(0xB0000297, 0x0);  /* hdmi pll vco bias resistor. */
		WRITE8(0xB0000295, 0x2);  /* hdmi pll filter resistor. */
		WRITE8(0xB0000279, 0x0);  /* hdmi pll charge pump current. */
		#endif

		#if 0
		WRITE8(0xB000AA08, 0x20);
		WRITE8(0xB000AA0C, 0x0);
		WRITE8(0xB000AA0D, 0x1F);
		WRITE8(0xB000AA0E, 0x0);
		WRITE8(0xB000AA0F, 0x43);
		#endif
	}
	else {
		DBG_LINE_PRINT("LO PLL");

		WRITE8(0xB0000260, READ8(0xB0000260) & ~0x1);  /* disable vco free run. */
		WRITE16(0xB000027A, 0x16);  /* hdmi pll frequency. */
		#if 0
		WRITE8(0xB0000296, 0x0);  /* hdmi pll clkvco=fckout/2. */
		#endif
		#if 1
		WRITE8(0xB000026C, READ8(0xB000026C) & ~0xF);  /* hdmi pll free run frequency. */
		WRITE8(0xB0000297, 0x1);  /* hdmi pll vco bias resistor. */
		WRITE8(0xB0000295, 0x1);  /* hdmi pll filter resistor. */
		WRITE8(0xB0000279, 0x2);  /* hdmi pll charge pump current. */
		#endif

		#if 0
		WRITE8(0xB000AA08, 0x0);
		WRITE8(0xB000AA0C, 0x3);
		WRITE8(0xB000AA0D, 0x7);
		WRITE8(0xB000AA0E, 0x3);
		WRITE8(0xB000AA0F, 0x5);
		#endif
	}
#endif

	if ( dispDviEn ) {
		UINT32 isItlc, mul, hpixel, vline, hsyncos, vsyncos, hsyncw, vsyncw, h0, v0, h1, v1;

		isItlc  = dispDtlTime.flags & 0x80;  /* interlaced or not? */
		mul     = isItlc ? 2 : 1;
		hpixel  = dispDtlTime.horActive + dispDtlTime.horBlanking;
		vline   = (dispDtlTime.verActive + dispDtlTime.verBlanking) * mul + (mul - 1);
		hsyncos = dispDtlTime.horSyncOffset - 1;
		vsyncos = (dispDtlTime.verSyncOffset - 1) * mul;
		hsyncw  = hsyncos + dispDtlTime.horSyncPulseWidth;
		vsyncw  = vsyncos + dispDtlTime.verSyncPulseWidth * mul;
		h0      = dispDtlTime.horBlanking - 1;
		v0      = dispDtlTime.verBlanking * mul - 1;
		h1      = hpixel - 2;
		v1      = vline - (mul + 1);

		/* set clk and type. */
		dispParaSet(DEV_PID, DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0);
		dispParaSet(DEV_PID, DISP_PARA_CLK_FREQ, dispDtlTime.pixelClk, dispDtlTime.pixelClk, 0, 0, 0);
		dispParaSet(DEV_PID, DISP_PARA_SCALE_CLK_FREQ, 240000, 0, 0, 0, 0);
		dispParaSet(DEV_PID, DISP_PARA_HDMI_PIXEL_CLK_FREQ, dispDtlTime.pixelClk, 0, 0, 0, 0);
		dispParaSet(DEV_PID, DISP_PARA_RESET, 0, 1, 0, 0, 0);
		dispParaSet(DEV_PID, DISP_PARA_DEVICE_TYPE,
		            isItlc ? DISP_PANEL_DEVICE_TYPE_STD_INTERLACE_CCIR601 : DISP_PANEL_DEVICE_TYPE_UPS051,
		            0, 0, 0, 0);
		dispParaSet(DEV_PID, DISP_PARA_INTERLACE_MODE,
		            isItlc ? DISP_INTERLACE_NOSOF : DISP_PROGRESSIVE,
		            isItlc ? DISP_SRC_FIELD_SWAP : DISP_SRC_FIELD_NOSWAP,
		            0, 0, 0);
		if ( isItlc ) {
			dispParaSet(DEV_PID, DISP_PARA_IMG_NONSCALE_INTERLACE_MODE, DISP_NONSCALE_INTRLC_NOSWAP, 0, 0, 0, 0);
		}
		dispParaSet(DEV_PID, DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0);
		dispParaSet(DEV_PID, DISP_PARA_EDGE_ENHANCE, 0, 0, 0, 0, 0);

		/* hdmi power and format. */
		dispParaSet(DEV_PID, DISP_PARA_HDMI_PRE_EMPHASIS, 0, isHiPll ? 2 : 0, 0, 0, 0);
		dispParaSet(DEV_PID, DISP_PARA_HDMI_CHARGE_PUMP, isHiPll ? 0 : 3, isHiPll ? 31 : 7, isHiPll ? 0 : 3, 0, 0);
		dispParaSet(DEV_PID, DISP_PARA_HDMI_PHY_POWER, 1, isHiPll, isHiPll ? 3 : 1, isHiPll ? 0 : 1, 0);  /* low/high gain. */
		dispParaSet(DEV_PID, DISP_PARA_HDMI_SIGNAL_POLARITY, 0x1, 0x0, 0, 0, 0);
		dispParaSet(DEV_PID, DISP_PARA_HDMI_PIXEL_REPETITION, 0, 0, 0, 0, 0);
		dispParaSet(DEV_PID, DISP_PARA_HDMI_COLOR_DEPTH, DISP_HDMI_COLOR_DEPTH_24_BIT, 0, 0, 0, 0);

		/* set vh timing and format. */
		dispParaSet(DEV_PID, DISP_PARA_DEVICE_TOTAL_RESOLUTION, hpixel, vline, 0, 0, 0);
		dispParaSet(DEV_PID, DISP_PARA_DEVICE_SYNC_TIME, hsyncos, vsyncos, hsyncw, vsyncw, 0);
		dispParaSet(DEV_PID, DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0);
		dispParaSet(DEV_PID, DISP_PARA_DEVICE_ACTIVE_REGION, h0, v0, h1, v1, 0);

		pktCfg = 0;
	}
	else {
		/* set color format. */
		memcpy(dispHdmiAviInfoFramePacketBody, pdispHdmiAviInfoFramePacketBody[vic], sizeof(dispHdmiAviInfoFramePacketBody));
		DBG_LINE_PRINT("color format=%d", dispColorFmt);
		dispHdmiAviInfoFramePacketBody[1] = (dispHdmiAviInfoFramePacketBody[1] & ~0x60) | (dispColorFmt << 5);  /* fill Y[1:0]. */
		#if 1  /* fix for HDMI CTS Test ID 7-23 and 7-24. */
		dispHdmiAviInfoFramePacketBody[3] &= ~0xC;   /* Q[1:0] default setting is Default. */
		dispHdmiAviInfoFramePacketBody[5] &= ~0xC0;  /* YQ[1:0] default setting is Limited Range. */
		if ( dispColorFmt == 0 ) {  /* RGB444. */
			/* for HDMI CTS Test ID 7-23. */
			dispHdmiAviInfoFramePacketBody[3] |= (dispQuantRange << 2);  /* fill Q[1:0]. */
		}
		else if ( dispColorFmt <= 2 ) {  /* YUV422 or YUV444. */
			/* for HDMI CTS Test ID 7-24. */
			dispHdmiAviInfoFramePacketBody[5] |= (dispQuantRange == 2 ? 0x40 : 0x00);  /* fill YQ[1:0]. */
		}
		#else
		DBG_LINE_PRINT("quant range=%d", dispQuantRange);
		dispHdmiAviInfoFramePacketBody[3] = (dispHdmiAviInfoFramePacketBody[3] & ~0xC) | (dispQuantRange << 2);  /* fill Q[1:0]. */
		dispHdmiAviInfoFramePacketBody[5] = (dispHdmiAviInfoFramePacketBody[5] & ~0xC0) | (dispQuantRange == 2 ? 0x40 : 0x00);  /* fill YQ[1:0]. */
		#endif
		DBG_LINE_PRINT2("Q[1:0]=%d YQ[1:0]=%d", (dispHdmiAviInfoFramePacketBody[3] & 0xC) >> 2, (dispHdmiAviInfoFramePacketBody[5] & 0xC0) >> 6);

		/* set color depth. */
		DBG_LINE_PRINT("color depth=%d", dispColorDepth);
		dispHdmiGcPacketBody[2] = (dispHdmiGcPacketBody[2] & ~0xF) | dispColorDepth;  /* fill CD[3:0]. */

		if ( v3d ) {
			/* set 3D structure. */
			dispHdmiVsInfoFramePacketBody[5] = (dispHdmiVsInfoFramePacketBody[5] & ~0xF0) | ((v3d >> 4) & 0xF0);  /* fill PB5[7:4]. */

			switch ( v3d ) {
			case SP5K_EDID_3D_FP:
				dispParaTableSet(DEV_PID, pdispPwrOnFp3dParaTbl[vic]);
				break;
			case SP5K_EDID_3D_TAB:
				dispParaTableSet(DEV_PID, pdispPwrOnTab3dParaTbl[vic]);
				break;
			case SP5K_EDID_3D_SBSHHSS:
				dispParaTableSet(DEV_PID, pdispPwrOnSbshhss3dParaTbl[vic]);
				break;
			}
		}
		else {
			dispParaTableSet(DEV_PID, pdispPwrOnParaTbl[vic]);
		}

		dispParaSet(DEV_PID, DISP_PARA_HDMI_COLOR_DEPTH, dispColorDepth ? (dispColorDepth - 4) << 1 : DISP_HDMI_COLOR_DEPTH_24_BIT, 0, 0, 0, 0);
		dispParaSet(DEV_PID, DISP_PARA_HDMI_DATA_ISLAND, (UINT32)dispHdmiGcPacketHeader, (UINT32)dispHdmiGcPacketBody, 0, 0, 0);
		dispParaSet(DEV_PID, DISP_PARA_HDMI_DATA_ISLAND, (UINT32)dispHdmiAviInfoFramePacketHeader, (UINT32)dispHdmiAviInfoFramePacketBody, 0, 0, 0);
		dispParaSet(DEV_PID, DISP_PARA_HDMI_DATA_ISLAND, (UINT32)dispHdmiSpdInfoFramePacketHeader, (UINT32)dispHdmiSpdInfoFramePacketBody, 0, 0, 0);
		dispParaSet(DEV_PID, DISP_PARA_HDMI_DATA_ISLAND, (UINT32)dispHdmiAudioInfoFramePacketHeader, (UINT32)dispHdmiAudioInfoFramePacketBody, 0, 0, 0);
		dispParaSet(DEV_PID, DISP_PARA_HDMI_AUD_SAMPLE_RATE, AUD_SAMPLE_RATE, 0, 0, 0, 0);
		if ( v3d ) {
			dispParaSet(DEV_PID, DISP_PARA_HDMI_DATA_ISLAND, (UINT32)dispHdmiVsInfoFramePacketHeader, (UINT32)dispHdmiVsInfoFramePacketBody, 0, 0, 0);
			pktCfg = HDMI_PKT_3D_EN;
		}
		else {
			pktCfg = HDMI_PKT_EN;
		}

		/* if yuv format, exchange r & b. */
		dispParaSet(DEV_PID, DISP_PARA_LCM_DATA_PACKET, dispColorFmt != 0 ? 1 : 0, 0, 0, 0, 0);
	}

	dispParaSet(DEV_PID, DISP_PARA_HDMI_PACKET_ENABLE, HDMI_PKT_3D_EN, pktCfg, 0, 0, 0);
	dispParaSet(DEV_PID, DISP_PARA_HDMI_DVI_ENABLE, dispDviEn, 0, 0, 0, 0);
	dispParaSet(DEV_PID, DISP_PARA_GLOB_ENABLE, 1, 0, 0, 0, 2);

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

#if 0
	dispColorDepth = 4;  /* 4: 24 bits per pixel. */
	dispColorFmt   = 0;  /* 0: RGB444. */
#endif

	DBG_FUNC_RETURN(SUCCESS, NULL);
}

/**
 * @brief Get capability in current resolution of the device.
 *
 * @param [out] *pcapabDesc  Capability descriptor.
 * @param [in]  descSize     Data size of the descriptor.
 *
 * @return None.
 */
static void
dispOpCapabilityGet(
	dispCapabDesc_t *pcapabDesc,
	UINT32 descSize
)
{
	DBG_FUNC_ENTER(NULL);

	if ( dispDviEn ) {
		*pcapabDesc = dispDviCapabDesc;
	}
	else {
		UINT32 vic;
		vic = dispResId & SP5K_EDID_VIC_MSK;
		*pcapabDesc = dispCapabDesc[vic];
	}

	DBG_FUNC_RETURN_VOID(NULL);
}

/**
 * @brief Set resolution for further operating.
 *
 * @param [in] resId  Resolution ID, \n
 *     SP5K_EDID_640X480P_60HZ_4T3, \n
 *     SP5K_EDID_720X480P_60HZ_4T3, \n
 *     SP5K_EDID_720X480P_60HZ_16T9, \n
 *     SP5K_EDID_1280X720P_60HZ_16T9, \n
 *     SP5K_EDID_1920X1080I_60HZ_16T9, \n
 *     SP5K_EDID_720X480I_60HZ_4T3, \n
 *     SP5K_EDID_720X480I_60HZ_16T9, \n
 *     SP5K_EDID_720X240P_60HZ_4T3, \n
 *     SP5K_EDID_720X240P_60HZ_16T9, \n
 *     SP5K_EDID_2880X480I_60HZ_4T3, \n
 *     SP5K_EDID_2880X480I_60HZ_16T9, \n
 *     SP5K_EDID_2880X240P_60HZ_4T3, \n
 *     SP5K_EDID_2880X240P_60HZ_16T9, \n
 *     SP5K_EDID_1440X480P_60HZ_4T3, \n
 *     SP5K_EDID_1440X480P_60HZ_16T9, \n
 *     SP5K_EDID_1920X1080P_60HZ_16T9, \n
 *     SP5K_EDID_720X576P_50HZ_4T3, \n
 *     SP5K_EDID_720X576P_50HZ_16T9, \n
 *     SP5K_EDID_1280X720P_50HZ_16T9, \n
 *     SP5K_EDID_1920X1080I_50HZ_16T9, \n
 *     SP5K_EDID_720X576I_50HZ_4T3, \n
 *     SP5K_EDID_720X576I_50HZ_16T9, \n
 *     SP5K_EDID_720X288P_50HZ_4T3, \n
 *     SP5K_EDID_720X288P_50HZ_16T9, \n
 *     SP5K_EDID_2880X576I_50HZ_4T3, \n
 *     SP5K_EDID_2880X576I_50HZ_16T9, \n
 *     SP5K_EDID_2880X288P_50HZ_4T3, \n
 *     SP5K_EDID_2880X288P_50HZ_16T9, \n
 *     SP5K_EDID_1440X576P_50HZ_4T3, \n
 *     SP5K_EDID_1440X576P_50HZ_16T9, \n
 *     SP5K_EDID_1920X1080P_50HZ_16T9, \n
 *     SP5K_EDID_1920X1080P_24HZ_16T9, \n
 *     SP5K_EDID_1920X1080P_25HZ_16T9, \n
 *     SP5K_EDID_1920X1080P_30HZ_16T9, \n
 *     SP5K_EDID_2880X480P_60HZ_4T3, \n
 *     SP5K_EDID_2880X480P_60HZ_16T9, \n
 *     SP5K_EDID_2880X576P_50HZ_4T3, \n
 *     SP5K_EDID_2880X576P_50HZ_16T9, \n
 *     SP5K_EDID_1920X1080I_100HZ_16T9, \n
 *     SP5K_EDID_1280X720P_100HZ_16T9, \n
 *     SP5K_EDID_720X576P_100HZ_4T3, \n
 *     SP5K_EDID_720X576P_100HZ_16T9, \n
 *     SP5K_EDID_720X576I_100HZ_4T3, \n
 *     SP5K_EDID_720X576I_100HZ_16T9, \n
 *     SP5K_EDID_1920X1080I_120HZ_16T9, \n
 *     SP5K_EDID_1280X720P_120HZ_16T9, \n
 *     SP5K_EDID_720X480P_120HZ_4T3, \n
 *     SP5K_EDID_720X480P_120HZ_16T9, \n
 *     SP5K_EDID_720X480I_120HZ_4T3, \n
 *     SP5K_EDID_720X480I_120HZ_16T9, \n
 *     SP5K_EDID_720X576P_200HZ_4T3, \n
 *     SP5K_EDID_720X576P_200HZ_16T9, \n
 *     SP5K_EDID_720X576I_200HZ_4T3, \n
 *     SP5K_EDID_720X576I_200HZ_16T9, \n
 *     SP5K_EDID_720X480P_240HZ_4T3, \n
 *     SP5K_EDID_720X480P_240HZ_16T9, \n
 *     SP5K_EDID_720X480I_240HZ_4T3, \n
 *     SP5K_EDID_720X480I_240HZ_16T9, \n
 *     SP5K_EDID_1280X720P_24HZ_16T9, \n
 *     SP5K_EDID_1280X720P_25HZ_16T9, \n
 *     SP5K_EDID_1280X720P_30HZ_16T9, \n
 *     SP5K_EDID_1920X1080P_120HZ_16T9, \n
 *     SP5K_EDID_1920X1080P_100HZ_16T9, \n
 *     \n
 *     SP5K_EDID_3D_FP | SP5K_EDID_1280X720P_60HZ_16T9, \n
 *     SP5K_EDID_3D_FP | SP5K_EDID_1280X720P_50HZ_16T9, \n
 *     SP5K_EDID_3D_FP | SP5K_EDID_1920X1080P_24HZ_16T9, \n
 *     SP5K_EDID_3D_FP | SP5K_EDID_1920X1080P_25HZ_16T9, \n
 *     SP5K_EDID_3D_FP | SP5K_EDID_1920X1080P_30HZ_16T9, \n
 *     \n
 *     SP5K_EDID_3D_TAB | SP5K_EDID_1280X720P_60HZ_16T9, \n
 *     SP5K_EDID_3D_TAB | SP5K_EDID_1920X1080P_60HZ_16T9, \n
 *     SP5K_EDID_3D_TAB | SP5K_EDID_1280X720P_50HZ_16T9, \n
 *     SP5K_EDID_3D_TAB | SP5K_EDID_1920X1080P_50HZ_16T9, \n
 *     SP5K_EDID_3D_TAB | SP5K_EDID_1920X1080P_24HZ_16T9, \n
 *     \n
 *     SP5K_EDID_3D_SBSHHSS | SP5K_EDID_1280X720P_60HZ_16T9, \n
 *     SP5K_EDID_3D_SBSHHSS | SP5K_EDID_1920X1080I_60HZ_16T9, \n
 *     SP5K_EDID_3D_SBSHHSS | SP5K_EDID_1280X720P_50HZ_16T9, \n
 *     SP5K_EDID_3D_SBSHHSS | SP5K_EDID_1920X1080I_50HZ_16T9, \n
 *     SP5K_EDID_3D_SBSHHSS | SP5K_EDID_1920X1080P_24HZ_16T9.
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

/**
 * @brief Set driver defined configuration.
 *
 * @param [in]  cfgId  Configuration ID,
 *              when 0, para1 = 0: RGB444, 1, YUV422, 2: YUV444.
 *              when 1, para1 = 0: color depth not indicated, 4: 24, 5: 30, 6: 36, 7: 48 bits per pixel.
 *              when 2, para1 = 0: default, 1: limited, 2: full quantization range.
 *              when 3, para1 = 0: disable, 1: enable DVI mode.
 *              when 4, para1 = pointer to dispDetailTiming_t data structure.
 *
 * @return None.
 */
static UINT32
dispOpCfgSet(
	UINT32 cfgId,
	va_list args
)
{
	UINT32 para1;


	DBG_FUNC_ENTER("%d", cfgId);

	para1 = va_arg(args, UINT32);
printf("ID=%X para1=%X\n", cfgId, para1);
	switch ( cfgId ) {
	case 0:
		dispColorFmt = para1;
		break;

	case 1:
		dispColorDepth = para1;
		break;

	case 2:
		dispQuantRange = para1;
		break;

	case 3:
		dispDviEn = para1;
		break;

	case 4:
		{
			UINT32 mul;
			dispDetailTiming_t *pdtlTime;

			pdtlTime = (dispDetailTiming_t *)para1;
			memcpy(&dispDtlTime, pdtlTime, sizeof(dispDetailTiming_t));
			printf("pclk=%d flg=0x%X ",
			                dispDtlTime.pixelClk, dispDtlTime.flags);
			printf("ha=%d hbl=%d hso=%d hspw=%d his=%d hbo=%d ",
			                dispDtlTime.horActive, dispDtlTime.horBlanking, dispDtlTime.horSyncOffset,
			                dispDtlTime.horSyncPulseWidth, dispDtlTime.horImgSize, dispDtlTime.horBorder);
			printf("va=%d vbl=%d vso=%d vspw=%d vis=%d vbo=%d\n",
			                dispDtlTime.verActive, dispDtlTime.verBlanking, dispDtlTime.verSyncOffset,
			                dispDtlTime.verSyncPulseWidth, dispDtlTime.verImgSize, dispDtlTime.verBorder);
			mul = (dispDtlTime.flags & 0x80) ? 2 : 1;  /* is it interlaced? */
			dispDviCapabDesc.width  = dispDtlTime.horActive;
			dispDviCapabDesc.height = dispDtlTime.verActive * mul;
			dispDviCapabDesc.xoff   = dispDtlTime.horBlanking - 1;
			dispDviCapabDesc.yoff   = dispDtlTime.verBlanking * mul - 1;
			if ( (dispDtlTime.horActive * 1000 / dispDtlTime.verActive) >= (16 * 1000 / 9) ) {
				dispDviCapabDesc.aspectRatioX = 16;
				dispDviCapabDesc.aspectRatioY = 9;
			}
			else {
				dispDviCapabDesc.aspectRatioX = 4;
				dispDviCapabDesc.aspectRatioY = 3;
			}
			dispDviCapabDesc.logiEn    = 0;
			dispDviCapabDesc.frameRate = (UINT64)dispDtlTime.pixelClk * 1000 * 1000 /  /* use UINT64 to avoid overflow. */
			                             ((dispDtlTime.horActive + dispDtlTime.horBlanking) *
			                              (dispDtlTime.verActive + dispDtlTime.verBlanking));

			sprintf((char *)dispDevName, "DVI - %dX%d%c %dHZ %d:%d",
			                             dispDviCapabDesc.width, dispDviCapabDesc.height,
			                             (dispDtlTime.flags & 0x80) ? 'I' : 'P',
			                             (dispDviCapabDesc.frameRate + 500) / 1000,
			                             dispDviCapabDesc.aspectRatioX, dispDviCapabDesc.aspectRatioY);
		}
	}

	va_end(args);

	DBG_FUNC_RETURN(SUCCESS, "OK");
}

