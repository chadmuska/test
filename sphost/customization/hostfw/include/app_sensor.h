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
#ifndef APP_SENSOR_H
#define APP_SENSOR_H

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
#include "app_icon_def.h"
#include "app_stringid.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/
#define  SENSOR_MODE_UNDEFINE       0x00

#if SP5K_SENSOR_SONYIMX175
#define	 SENSOR_MODE_1080P_30FPS    0x34
#define  SENSOR_MODE_1080P_60FPS	0x31
#define  SENSOR_MODE_1440P_30FPS	SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_960P_60FPS     SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_720P_30FPS	    0x32
#define  SENSOR_MODE_720P_60FPS	    0x33
#define  SENSOR_MODE_720P_120FPS	SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_VGA_30FPS	    0x37
#define  SENSOR_MODE_VGA_60FPS	    0x36
#define  SENSOR_MODE_VGA_120FPS     0x31
#define  SENSOR_MODE_VGA_240FPS		SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_QVGA_240FPS	0x31
#define  SENSOR_CAPTURE_FULL_SIZE   SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_4K_15FPS       SENSOR_MODE_1080P_30FPS
#define  SENSOR_MODE_27K_30FPS      SENSOR_MODE_1080P_30FPS
#define  SENSOR_MODE_QHD_30FPS		SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_VIDEO_HDR		SENSOR_MODE_UNDEFINE
#elif SP5K_SENSOR_SONYIMX118
#define  SENSOR_MODE_1080P_30FPS    0x30
#define  SENSOR_MODE_1080P_60FPS	0x31
#define  SENSOR_MODE_1440P_30FPS	SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_960P_60FPS     SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_720P_30FPS	    0x32
#define  SENSOR_MODE_720P_60FPS     0x32
#define  SENSOR_MODE_720P_120FPS	SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_VGA_30FPS	    SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_VGA_60FPS	    SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_VGA_120FPS	    0x33
#define  SENSOR_MODE_VGA_240FPS		SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_QVGA_240FPS	0x34
#define  SENSOR_CAPTURE_FULL_SIZE   SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_4K_15FPS       SENSOR_MODE_1080P_30FPS
#define  SENSOR_MODE_27K_30FPS      SENSOR_MODE_1080P_30FPS
#define  SENSOR_MODE_QHD_30FPS		SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_VIDEO_HDR		SENSOR_MODE_UNDEFINE
#elif SP5K_SENSOR_OV4689 || SP5K_SENSOR_OV4689_SPCA7002 || SP5K_SENSOR_OV4689_SERDES_SPCA7002 || SP5K_SENSOR_SERDES_SPCA7002
#define  SENSOR_MODE_1080P_30FPS    0x36
#define  SENSOR_MODE_1080P_60FPS	0x33
#define  SENSOR_MODE_1440P_30FPS	SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_960P_60FPS     SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_720P_30FPS	    0x30
#define  SENSOR_MODE_720P_60FPS     0x37
#define  SENSOR_MODE_720P_120FPS	0x31
#define  SENSOR_MODE_VGA_30FPS	    SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_VGA_60FPS	    SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_VGA_120FPS	    SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_VGA_240FPS		SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_QVGA_240FPS	SENSOR_MODE_UNDEFINE
#define  SENSOR_CAPTURE_FULL_SIZE   SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_4K_15FPS       SENSOR_MODE_1080P_30FPS
#define  SENSOR_MODE_27K_30FPS      SENSOR_MODE_1080P_30FPS
#define  SENSOR_MODE_QHD_30FPS		0x3B
#define  SENSOR_MODE_VIDEO_HDR		0x3a
#elif SP5K_SENSOR_AR0330CS
#define  SENSOR_MODE_1080P_30FPS    0x31
#define  SENSOR_MODE_1080P_60FPS    0x31
#define  SENSOR_MODE_1440P_30FPS	SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_960P_60FPS     SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_720P_30FPS     0x30
#define  SENSOR_MODE_720P_60FPS     SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_720P_120FPS	SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_VGA_30FPS	    SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_VGA_60FPS	    SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_VGA_120FPS     SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_VGA_240FPS		SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_QVGA_240FPS    SENSOR_MODE_UNDEFINE
#define  SENSOR_CAPTURE_FULL_SIZE   0x40
#define  SENSOR_MODE_4K_15FPS       SENSOR_MODE_1080P_30FPS
#define  SENSOR_MODE_27K_30FPS      SENSOR_MODE_1080P_30FPS
#define  SENSOR_MODE_QHD_30FPS		SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_VIDEO_HDR		SENSOR_MODE_UNDEFINE
#elif SP5K_SENSOR_AR0330 || SP5K_SENSOR_AR0330_SPCA7002 || SP5K_SENSOR_AR0330_SERDES_SPCA7002
#define  SENSOR_MODE_1080P_30FPS    0x30
#define  SENSOR_MODE_1080P_60FPS    0x31
#define  SENSOR_MODE_1440P_30FPS	SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_960P_60FPS     SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_720P_30FPS     0x30
#define  SENSOR_MODE_720P_60FPS     SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_720P_120FPS	SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_VGA_30FPS	    SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_VGA_60FPS	    SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_VGA_120FPS     SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_VGA_240FPS		SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_QVGA_240FPS    SENSOR_MODE_UNDEFINE
#define  SENSOR_CAPTURE_FULL_SIZE   SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_4K_15FPS       SENSOR_MODE_1080P_30FPS
#define  SENSOR_MODE_27K_30FPS      SENSOR_MODE_1080P_30FPS
#define  SENSOR_MODE_QHD_30FPS		SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_VIDEO_HDR		SENSOR_MODE_UNDEFINE
#elif SP5K_SENSOR_AR0230
#define  SENSOR_MODE_1080P_30FPS    0x30
#define  SENSOR_MODE_1080P_60FPS    0x31
#define  SENSOR_MODE_1440P_30FPS	SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_960P_60FPS     SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_720P_30FPS     SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_720P_60FPS     SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_720P_120FPS	SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_VGA_30FPS	    SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_VGA_60FPS	    SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_VGA_120FPS     SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_VGA_240FPS		SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_QVGA_240FPS    SENSOR_MODE_UNDEFINE
#define  SENSOR_CAPTURE_FULL_SIZE   0x40
#define  SENSOR_MODE_4K_15FPS       SENSOR_MODE_1080P_30FPS
#define  SENSOR_MODE_27K_30FPS      SENSOR_MODE_1080P_30FPS
#define  SENSOR_MODE_QHD_30FPS		SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_VIDEO_HDR		SENSOR_MODE_UNDEFINE
#elif SP5K_SENSOR_SONYIMX206
#define  SENSOR_MODE_1080P_30FPS	0x3a
#define  SENSOR_MODE_1080P_60FPS	0x32
#define  SENSOR_MODE_1440P_30FPS	0x30
#define  SENSOR_MODE_960P_60FPS     0x31
#define  SENSOR_MODE_720P_30FPS     0x3a
#define  SENSOR_MODE_720P_60FPS     SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_720P_120FPS	0x3b
#define  SENSOR_MODE_VGA_30FPS	    SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_VGA_60FPS	    SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_VGA_120FPS     SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_VGA_240FPS		0x33
#define  SENSOR_MODE_QVGA_240FPS    SENSOR_MODE_UNDEFINE
#define  SENSOR_CAPTURE_FULL_SIZE   0x40
#define  SENSOR_MODE_4K_15FPS       0x3c
#define  SENSOR_MODE_27K_30FPS      SENSOR_MODE_1080P_30FPS
#define  SENSOR_MODE_QHD_30FPS		SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_VIDEO_HDR		SENSOR_MODE_UNDEFINE
#elif SP5K_SENSOR_DUAL_SONYIMX206
#define  SENSOR_MODE_1080P_30FPS	0x3a
#define  SENSOR_MODE_1080P_60FPS	0x32
#define  SENSOR_MODE_1440P_30FPS	0x30
#define  SENSOR_MODE_960P_60FPS     0x31
#define  SENSOR_MODE_720P_30FPS     SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_720P_60FPS     SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_720P_120FPS	0x3b
#define  SENSOR_MODE_VGA_30FPS	    SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_VGA_60FPS	    SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_VGA_120FPS     SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_VGA_240FPS		0x33
#define  SENSOR_MODE_QVGA_240FPS    SENSOR_MODE_UNDEFINE
#define  SENSOR_CAPTURE_FULL_SIZE   0x40
#define  SENSOR_MODE_4K_15FPS       SENSOR_MODE_1080P_30FPS
#define  SENSOR_MODE_27K_30FPS      SENSOR_MODE_1080P_30FPS
#define  SENSOR_MODE_QHD_30FPS		SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_VIDEO_HDR		SENSOR_MODE_UNDEFINE
#elif SP5K_SENSOR_SONYIMX078
#define  SENSOR_MODE_1080P_30FPS	0x32
#define  SENSOR_MODE_1080P_60FPS	0x31
#define  SENSOR_MODE_1440P_30FPS	0x33
#define  SENSOR_MODE_960P_60FPS     SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_720P_30FPS     0x30
#define  SENSOR_MODE_720P_60FPS     0x34
#define  SENSOR_MODE_720P_120FPS	0x36
#define  SENSOR_MODE_VGA_30FPS	    SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_VGA_60FPS	    SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_VGA_120FPS     SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_VGA_240FPS		SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_QVGA_240FPS    SENSOR_MODE_UNDEFINE
#define  SENSOR_CAPTURE_FULL_SIZE   0x40
#define  SENSOR_MODE_4K_15FPS       SENSOR_MODE_1080P_30FPS
#define  SENSOR_MODE_27K_30FPS      SENSOR_MODE_1080P_30FPS
#define  SENSOR_MODE_QHD_30FPS		SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_VIDEO_HDR		SENSOR_MODE_UNDEFINE
#elif SP5K_SENSOR_SONYIMX291
#define  SENSOR_MODE_1080P_30FPS	0x31
#define  SENSOR_MODE_1080P_60FPS	0x30
#define  SENSOR_MODE_1440P_30FPS	SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_960P_60FPS     SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_720P_30FPS     0x34
#define  SENSOR_MODE_720P_60FPS     0x33
#define  SENSOR_MODE_720P_120FPS	0x32
#define  SENSOR_MODE_VGA_30FPS	    SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_VGA_60FPS	    SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_VGA_120FPS     SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_VGA_240FPS		SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_QVGA_240FPS    SENSOR_MODE_UNDEFINE
#define  SENSOR_CAPTURE_FULL_SIZE   0x40
#define  SENSOR_MODE_4K_15FPS       SENSOR_MODE_1080P_30FPS
#define  SENSOR_MODE_27K_30FPS      SENSOR_MODE_1080P_30FPS
#define  SENSOR_MODE_QHD_30FPS		SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_VIDEO_HDR		SENSOR_MODE_UNDEFINE
#elif SP5K_SENSOR_ADV7480
#define  SENSOR_MODE_1080P_30FPS    0x38
#define  SENSOR_MODE_1080P_60FPS    0x35
#define  SENSOR_MODE_1440P_30FPS	SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_960P_60FPS     SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_720P_30FPS     0x33
#define  SENSOR_MODE_720P_60FPS     0x33
#define  SENSOR_MODE_720P_120FPS	SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_VGA_30FPS	    SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_VGA_60FPS	    SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_VGA_120FPS     SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_VGA_240FPS		SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_QVGA_240FPS    SENSOR_MODE_UNDEFINE
#define  SENSOR_CAPTURE_FULL_SIZE   SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_4K_15FPS       SENSOR_MODE_1080P_30FPS
#define  SENSOR_MODE_27K_30FPS      SENSOR_MODE_1080P_30FPS
#define  SENSOR_MODE_QHD_30FPS		SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_VIDEO_HDR		SENSOR_MODE_UNDEFINE

#elif SP5K_SENSOR_HM2130
#define  SENSOR_MODE_4K_15FPS       0x30
#define  SENSOR_MODE_27K_30FPS      0x30
#define  SENSOR_MODE_1080P_30FPS    0x31
#define  SENSOR_MODE_1080P_60FPS	0x30
#define  SENSOR_MODE_1440P_30FPS	0x30
#define  SENSOR_MODE_960P_60FPS     0x30
#define  SENSOR_MODE_720P_30FPS	    0x31
#define  SENSOR_MODE_720P_60FPS     0x30
#define  SENSOR_MODE_720P_120FPS	0x30
#define  SENSOR_MODE_QHD_30FPS		0x31
#define  SENSOR_MODE_VGA_30FPS	    0x31
#define  SENSOR_MODE_VGA_60FPS	    0x30
#define  SENSOR_MODE_VGA_120FPS	    0x30
#define  SENSOR_MODE_VGA_240FPS		0x30
#define  SENSOR_MODE_QVGA_240FPS	0x30
#define  SENSOR_CAPTURE_FULL_SIZE   0x30
#define  SENSOR_MODE_VIDEO_HDR		0x30

#elif SP5K_SENSOR_AR1335
#define  SENSOR_MODE_4K_15FPS       0x30
#define  SENSOR_MODE_27K_30FPS      0x30
#define  SENSOR_MODE_1080P_30FPS    0x30
#define  SENSOR_MODE_1080P_60FPS	0x31
#define  SENSOR_MODE_1440P_30FPS	0x30
#define  SENSOR_MODE_960P_60FPS     0x30
#define  SENSOR_MODE_720P_30FPS	    0x30
#define  SENSOR_MODE_720P_60FPS     0x31
#define  SENSOR_MODE_720P_120FPS	0x30
#define  SENSOR_MODE_QHD_30FPS		0x30
#define  SENSOR_MODE_VGA_30FPS	    0x30
#define  SENSOR_MODE_VGA_60FPS	    0x31
#define  SENSOR_MODE_VGA_120FPS	    0x30
#define  SENSOR_MODE_VGA_240FPS		0x30
#define  SENSOR_MODE_QVGA_240FPS	0x30
#define  SENSOR_CAPTURE_FULL_SIZE   0x40
#define  SENSOR_MODE_VIDEO_HDR		0x30
#elif SP5K_SENSOR_OS08A10
#define  SENSOR_MODE_4K_15FPS       0x33
#define  SENSOR_MODE_27K_30FPS      0x30
#define  SENSOR_MODE_1080P_30FPS    0x30
#define  SENSOR_MODE_1080P_60FPS	0x30
#define  SENSOR_MODE_1440P_30FPS	0x30
#define  SENSOR_MODE_960P_60FPS     0x30
#define  SENSOR_MODE_720P_30FPS	    0x30
#define  SENSOR_MODE_720P_60FPS     0x30
#define  SENSOR_MODE_720P_120FPS	0x30
#define  SENSOR_MODE_QHD_30FPS		0x30
#define  SENSOR_MODE_VGA_30FPS	    0x30
#define  SENSOR_MODE_VGA_60FPS	    0x30
#define  SENSOR_MODE_VGA_120FPS	    0x30
#define  SENSOR_MODE_VGA_240FPS		0x30
#define  SENSOR_MODE_QVGA_240FPS	0x30
#define  SENSOR_CAPTURE_FULL_SIZE   0x40
#define  SENSOR_MODE_VIDEO_HDR		0x30

#else
#define  SENSOR_MODE_1080P_30FPS    0x30
#define  SENSOR_MODE_1080P_60FPS    0x30
#define  SENSOR_MODE_1440P_30FPS	0x30
#define  SENSOR_MODE_960P_60FPS     0x30
#define  SENSOR_MODE_720P_30FPS     0x30
#define  SENSOR_MODE_720P_60FPS     0x30
#define  SENSOR_MODE_720P_120FPS	0x30
#define  SENSOR_MODE_VGA_30FPS	    0x30
#define  SENSOR_MODE_VGA_60FPS	    0x30
#define  SENSOR_MODE_VGA_120FPS     0x30
#define  SENSOR_MODE_VGA_240FPS		0x30
#define  SENSOR_MODE_QVGA_240FPS    0x30
#define  SENSOR_CAPTURE_FULL_SIZE   SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_4K_15FPS       SENSOR_MODE_1080P_30FPS
#define  SENSOR_MODE_27K_30FPS      SENSOR_MODE_1080P_30FPS
#define  SENSOR_MODE_QHD_30FPS		SENSOR_MODE_UNDEFINE
#define  SENSOR_MODE_VIDEO_HDR		SENSOR_MODE_UNDEFINE
#endif

#if SP5K_SENSOR_SONYIMX175
/*sensor mode 0x36 capture size 4:3*/
#define DEFAULT_PREVIEW_MODE    SENSOR_MODE_VGA_60FPS
#define DEFAULT_CAPTURE_W       3648
#define DEFAULT_CAPTURE_H       2736
#define DEFAULT_CAPTURE_ICON    ID_ICON_RESOLUTION_10M
#define DEFAULT_CAPTURE_STRING  STRTK_SPV_SIZE_10M
#elif SP5K_SENSOR_SONYIMX118
/*sensor mode 0x30 capture size 16:9*/
#define DEFAULT_PREVIEW_MODE    SENSOR_MODE_1080P_30FPS
#define DEFAULT_CAPTURE_W       4896
#define DEFAULT_CAPTURE_H       3672
#define DEFAULT_CAPTURE_ICON    ID_ICON_RESOLUTION_20M
#define DEFAULT_CAPTURE_STRING  STRTK_SPV_SIZE_16M
#elif SP5K_SENSOR_OV4689 || SP5K_SENSOR_OV4689_SPCA7002 || SP5K_SENSOR_OV4689_SERDES_SPCA7002
/*sensor mode 0x36 capture size 16:9*/
#define DEFAULT_PREVIEW_MODE    SENSOR_MODE_1080P_30FPS
#define DEFAULT_CAPTURE_W       2688
#define DEFAULT_CAPTURE_H       1512
#define DEFAULT_CAPTURE_ICON    ID_ICON_RESOLUTION_4M
#define DEFAULT_CAPTURE_STRING  STRTK_SPV_SIZE_4M_1
#elif SP5K_SENSOR_AR0330 || SP5K_SENSOR_AR0330_SPCA7002 || SP5K_SENSOR_AR0330_SERDES_SPCA7002
/*sensor mode 0x30 capture size 16:9*/
#define DEFAULT_PREVIEW_MODE    SENSOR_MODE_1080P_30FPS
#define DEFAULT_CAPTURE_W       2304
#define DEFAULT_CAPTURE_H       1296
#define DEFAULT_CAPTURE_ICON    ID_ICON_RESOLUTION_3M
#define DEFAULT_CAPTURE_STRING  STRTK_SPV_SIZE_3M_1
#elif SP5K_SENSOR_AR0230
/*sensor mode 0x30 capture size 16:9*/
#define DEFAULT_PREVIEW_MODE    SENSOR_MODE_1080P_30FPS
#define DEFAULT_CAPTURE_W       1920
#define DEFAULT_CAPTURE_H       1080
#define DEFAULT_CAPTURE_ICON    ID_ICON_RESOLUTION_2M_16_9
#define DEFAULT_CAPTURE_STRING  STRTK_SPV_SIZE_2M_1
#elif SP5K_SENSOR_SONYIMX291
/*sensor mode 0x30 capture size 16:9*/
#define DEFAULT_PREVIEW_MODE    SENSOR_MODE_1080P_60FPS
#define DEFAULT_CAPTURE_W       1280
#define DEFAULT_CAPTURE_H       720
#define DEFAULT_CAPTURE_ICON    ID_ICON_RESOLUTION_2M_16_9
#define DEFAULT_CAPTURE_STRING  STRTK_SPV_SIZE_2M_1
#elif SP5K_SENSOR_SONYIMX206
/*sensor mode 0x31 capture size 16:9*/
#define DEFAULT_PREVIEW_MODE    SENSOR_MODE_1080P_30FPS
#define DEFAULT_CAPTURE_W       2688
#define DEFAULT_CAPTURE_H       1512
#define DEFAULT_CAPTURE_ICON    ID_ICON_RESOLUTION_4M
#define DEFAULT_CAPTURE_STRING  STRTK_SPV_SIZE_4M_1
#elif SP5K_SENSOR_DUAL_SONYIMX206
/*sensor mode 0x31 capture size 16:9*/
#define DEFAULT_PREVIEW_MODE    SENSOR_MODE_1080P_30FPS
#define DEFAULT_CAPTURE_W       2688
#define DEFAULT_CAPTURE_H       1512
#define DEFAULT_CAPTURE_ICON    ID_ICON_RESOLUTION_4M
#define DEFAULT_CAPTURE_STRING  STRTK_SPV_SIZE_4M_1
#elif SP5K_SENSOR_SONYIMX078
/*sensor mode 0x30 capture size 16:9*/
#define DEFAULT_PREVIEW_MODE    SENSOR_MODE_720P_30FPS
#define DEFAULT_CAPTURE_W       4000
#define DEFAULT_CAPTURE_H       3000
#define DEFAULT_CAPTURE_ICON    ID_ICON_RESOLUTION_12M
#define DEFAULT_CAPTURE_STRING  STRTK_SPV_SIZE_12M
#elif SP5K_SENSOR_HM2130
/*sensor mode 0x30 capture size 16:9*/
#define DEFAULT_PREVIEW_MODE    SENSOR_MODE_1080P_30FPS
#define DEFAULT_CAPTURE_W       1920
#define DEFAULT_CAPTURE_H       1080
#define DEFAULT_CAPTURE_ICON    ID_ICON_RESOLUTION_2M
#define DEFAULT_CAPTURE_STRING  STRTK_SPV_SIZE_2M_1
#elif SP5K_SENSOR_GC8603
/*sensor mode 0x30 capture size 4:3*/
#define DEFAULT_PREVIEW_MODE    0x30
#define DEFAULT_CAPTURE_W       3264
#define DEFAULT_CAPTURE_H       2448
#define DEFAULT_CAPTURE_ICON    ID_ICON_RESOLUTION_8M
#define DEFAULT_CAPTURE_STRING  STRTK_SPV_SIZE_8M

#elif SP5K_SENSOR_GC8603
/*sensor mode 0x30 capture size 4:3*/
#define DEFAULT_PREVIEW_MODE    0x30
#define DEFAULT_CAPTURE_W       1920
#define DEFAULT_CAPTURE_H       1080
#define DEFAULT_CAPTURE_ICON    ID_ICON_RESOLUTION_2M
#define DEFAULT_CAPTURE_STRING  STRTK_SPV_SIZE_2M_1

#elif SP5K_SENSOR_A1335
/*default sensor mode 0x32 capture size 16:9*/
#define DEFAULT_PREVIEW_MODE    0x32
#define DEFAULT_CAPTURE_W       4000
#define DEFAULT_CAPTURE_H       3000
#define DEFAULT_CAPTURE_ICON    ID_ICON_RESOLUTION_12M
#define DEFAULT_CAPTURE_STRING  STRTK_SPV_SIZE_12M

#else
/*default sensor mode 0x30 capture size 16:9*/
#define DEFAULT_PREVIEW_MODE    0x30
#define DEFAULT_CAPTURE_W       1920
#define DEFAULT_CAPTURE_H       1080
#define DEFAULT_CAPTURE_ICON    ID_ICON_RESOLUTION_2M
#define DEFAULT_CAPTURE_STRING  STRTK_SPV_SIZE_2M_1
#endif

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/

void appSensorPowerOff(void);
void appAfePowerCtrl(UINT32 fWakeup);
void appsensorAutoPwrSaveSet(UINT32 timeout);

#endif  /* APP_SENSOR_H */
