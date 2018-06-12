/**************************************************************************
 *
 *       Copyright (c) 2009-2016 by iCatch Technology, Inc.
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
 *  Author: Yi-Ming Kao
 *
 **************************************************************************/
/* DO NOT EDIT the next 2 lines for auto revision by subversion
$Rev: 21108 $
$Date:: 2015-04-02 16:08:18#$
 */


/**
 * @file mview_def.h
 * @brief The common definition of multiple preview function.
 * @author Yi-Ming Kao
 */
#ifndef _UHD_DEF_H_
#define _UHD_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

/* event flags for main thread control */
#define UHD_EVT_DO               (1 << 31)
#define UHD_EVT_STOP             (1 << 30)
#define UHD_EVT_STOP_ACK         (1 << 29)
#define UHD_EVT_START            (1 << 28)

/* event flags for diq flush in 4k2k mode */
#define UHD_EVT1_DIQ_FLUSH		 (1 << 0)


typedef UINT32 (*fpuhdCallback_t)(UINT32, UINT32, void *);

typedef struct {
	UINT32 id;       /*!< the sequence identifier */
	UINT32 addr;   /*!< logical byte address of this yuv window */
	UINT32 xSize;  /*!< width */
	UINT32 ySize; /*!< height */
} uhdWindow_t;

typedef struct {
	UINT32 xSize;  /*!< width */
	UINT32 ySize; /*!< height */
} uhdSize_t;

typedef struct {
	UINT32 addr;
	UINT32 bufXSize;
	UINT32 bufYSize;
	UINT32 xSize;
	UINT32 ySize;
	UINT32 xOffset;
	UINT32 yOffset;
} uhdImgInfo_t;

typedef struct pvRawDoParam_s {
	UINT32 sensorXSize;
	UINT32 sensorYSize;
	UINT32 sensorXRatio;
	UINT32 sensorYRatio;
	UINT32 bayerScalingXSize;
	UINT32 bayerScalingYSize;
	UINT32 rawWidth;
	UINT32 rawHeight;
	UINT32 cropWidth;
	UINT32 cropHeight;
	UINT32 cropXOffset;
	UINT32 cropYOffset;
	UINT32 cdspWidth;
	UINT32 cdspHeight;
	UINT32 cdsp2ndXSize; /* cdsp 2nd path size. It is for QV */
	UINT32 cdsp2ndYSize;

	/* date time stamp options */
	UINT32 stampOpt;
	UINT32 rawBitMode;
	/* main cdsp */
	UINT32 ldcEn;
	UINT32 cqAddr;
	fpuhdCallback_t *fpuhdRawCb;	
	fpuhdCallback_t *fpuhdYuvPreCb;
	fpuhdCallback_t *fpuhdYuvPostCb;
	fpuhdCallback_t *fpuhdFrameCb;

	UINT32 do2ndEn;
} pvRawDoParam_t;

typedef struct {
	UINT32 id;    /*!< frame id */
	UINT32 addr;  /*!< physical address of raw buffer */
	UINT32 inUse;
	pvRawDoParam_t rawDoInfo;
} uhdRawFrame_t;


typedef enum {	
	UHD_PARAM_MODESW_INSTALL = 0,
	UHD_PARAM_RAW_NUM		 = 1,
	UHD_PARAM_RAW_BITMODE    = 2,
	UHD_PARAM_PERIOD         = 3,
	UHD_PARAM_RAW_CB         = 4,
	UHD_PARAM_YUV_PRE_CB     = 5,
	UHD_PARAM_YUV_POST_CB    = 6,
	UHD_PARAM_FRAME_CB       = 7,
	UHD_PARAM_2ND_PATH       = 8,
} uhdParamSel_t;


uhdRawFrame_t* uhdCurrBufGet(UINT32 bufIdx);
UINT32 uhdParamGet(	UINT32 cfg);

#ifdef __cplusplus
}
#endif

#endif /* _UHD_DEF_H_ */
