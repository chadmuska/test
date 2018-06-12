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

#ifndef _UHD_DEF_H_
#define _UHD_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "middleware/common_types.h"

#define EACH_SENSOR_RAW_BUF_TOT 3	
#define SENSOR_MAX_TOT 2
	
/*sensor type*/
#define SENSOR_TYPE_NORMAL 0
#define SENSOR_TYPE_BRIDGE 1



#define PV_RAW_EVENT_SKIP      			(1<<31)/*ISR can't get available raw buffer for task is too busy to process data, then this frame should be skipped. */
#define PV_RAW_EVENT_STOP     			(1<<30)


#define PV_RAW_EVENT_RAW_NUM_MASK	(0x0f)

typedef struct pvRawBufInfo_s{
	frameBufInfo_t bufInfo;
	UINT32 inUse;	   /*To determine if this buffer is available for cdsp write.*/
	UINT32 frameIdx;   /*frame index*/
	UINT32 sensorIdx;  		/*sensor index*/
	UINT32 rawBit;
} pvRawBufInfo_t;

typedef struct pvRawMsg_s{
	pvRawBufInfo_t *pEntry;    /*a pointer to pvRawBufInfo_t,*/
	UINT32 evt; 			   /*[31]: skip event [0:3]: frame Number*/
} pvRawMsg_t;

typedef struct pvRawDbg_s{
	UINT32 redoing;
	UINT32 crossFrame;
	UINT32 redoSt;
	UINT32 redoEt;
} pvRawDbg_t;

typedef enum {	
	PV_RAW_PARAM_MODESW_INSTALL = 0,
	PV_RAW_PARAM_RAW2YUV_CB,
	PV_RAW_PARAM_RAW2YUV_POST_CB,
	PV_RAW_PARAM_RAW2YUV_PRE_STOP
} pvRawParamSel_t;

typedef UINT32 (*fp_pvRaw2YuvCb)(frameBufInfo_t *praws, UINT32 rawNum, UINT32 rawbit, frameBufInfo_t *pyuvs, UINT32 yuvNum);
typedef void (*fp_pvRaw2YuvPostCb)(frameBufInfo_t *pyuvs, UINT32 yuvNum);

typedef struct	pvRawCb_s{
	/*modesw related*/
	UINT32 openFlag; 					   /*is preview raw flow is opened by modesw.*/
	UINT32 (*fp_modeswCbSet)(UINT32);   /*a callback to plugin/plugout pvRawFuns in modesw.*/
	UINT32 diff;
   
  	 /*cfg*/
	UINT32 sensorType;				   /*sensor type*/
	UINT32 sensorTot;				   /*sensor total*/
	UINT32 rawBit;					   /*for sp5k do raw api, refer HAL_CDSP_DATA_TYPE_RAW_XXX*/

	/*baswfw*/
	UINT32 startFlag;
	UINT32 pvRawBufInfoTot; 			   /*raw buffer total: pvRawBufInfoEachTot* sensor num*/
	UINT32 pvRawBufInfoEachTot;
	pvRawBufInfo_t *pEntry; 			   /*a pointer to a memory pool of pvRawBufInfo_t*/

	/*for management of buffer*/
	UINT32 cdspWSenId;
	UINT32 cdspWIdx[SENSOR_MAX_TOT];   /*For management of raw buffer.*/
	UINT32 frameIdx[SENSOR_MAX_TOT];
	
	UINT32 sensorFrameCbId; 
	void (*psensorFrameCb)(void); 		/*fp to a callback funciton to get raw and set the next snap raw buffer.*/

	/*for yuvdotask*/
	UINT32 queueId;
	UINT32 rawqueId;
	UINT32 yuvindexId;
	UINT32 queue_th;
	UINT32 raw2yuv_th;
	UINT32 raw2yuvPost_th;
	
	fp_pvRaw2YuvCb fpRaw2YuvCB;
	fp_pvRaw2YuvPostCb fpRaw2YuvPostCB;

	/*for pop first raw*/
	UINT32 first_pri;
	UINT32 first_sec;

	UINT32 first_yuv;
	UINT32 redoing;
	pvRawDbg_t dbg;
} pvRawCb_t;


/* event flags for main thread control */
#define UHD_EVT_DO               (1 << 31)
#define UHD_EVT_STOP             (1 << 30)
#define UHD_EVT_STOP_ACK         (1 << 29)
#define UHD_EVT_START            (1 << 28)

/* event flags for diq flush in 4k2k mode */
#define UHD_EVT1_DIQ_FLUSH		 (1 << 0)

#define UHD_RAW_BUF_NUM             2


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
	UHD_PARAM_RAW_BITMODE    = 1,
	UHD_PARAM_PERIOD,
	UHD_PARAM_RAW_CB,
	UHD_PARAM_YUV_PRE_CB,
	UHD_PARAM_YUV_POST_CB,
	UHD_PARAM_FRAME_CB,
	UHD_PARAM_2ND_PATH,
} uhdParamSel_t;


uhdRawFrame_t* uhdCurrBufGet(UINT32 bufIdx);

#ifdef __cplusplus
}
#endif

#endif /* _UHD_DEF_H_ */
