/**************************************************************************
 *
 *       Copyright (c) 2008-2016 by iCatch Technology, Inc.
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
#ifndef _LANEDETECT_DEF_H_
#define _LANEDETECT_DEF_H_

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "common.h"
#include "middleware/modesw_def.h"
#include "adas_def.h"


/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
typedef enum {
	LANEDET_DEPATURE_THRESHOLD,
	LANEDET_CENTRAL_POINT,
	LANEDET_IMAGE_SCALE_FACTOR,
	LANEDET_IMAGE_ROI_CROP_OFF,
	LANEDET_WARN_MSG_INTERVAL,
	LANEDET_SENSOR_FOCAL,
	LANEDET_SENSOR_PIXEL_UNIT,
	LANEDET_LEFT_DIST_THRES,
	LANEDET_RIGHT_DIST_THRES,

	/* the following for get only */
	LANEDET_DEPATURE_COORD,
	LANEDET_DETECT_RATE,
	LANEDET_RESULT_GET = 0x1000,
	LANEDET_CONFIG_GET = 0x2000,
} laneDetCfgSel_t;

/**************************************************************************
 *                                  M A C R O S
 **************************************************************************/

/**************************************************************************
 *                              D A T A   T Y P E S
 **************************************************************************/
typedef struct{
	int thita_L[5];
	int thita_R[5];
	int r_L[5];
	int r_R[5];
} LD_HOUGH_DATA;

typedef struct {
	UINT32 ftx1;
	UINT32 ftx2;
	UINT32 ftx3;
	UINT32 ftx4;
	UINT32 fty1;
	UINT32 fty2;
	UINT32 fty3;
	UINT32 fty4;
	UINT32 ROI_h_end;
} lanePosotion;

typedef struct {
	UINT8 leftLaneSignal;
	UINT8 rightLaneSignal;
} departureWarning;

typedef struct {
	lanePosotion pos;
	UINT32 ldws_flag;
	UINT32 center_to_lane_l;	
	UINT32 center_to_lane_r;	
} laneDetCfgResult_t;

typedef struct laneDetCfgGet_s{
	UINT32 ldws_angle_thr;
	UINT32 ldws_distance2center_thr;
	UINT32 ldws_center_x;
	UINT32 ldws_center_y;
	UINT32 ldws_img_scale_factor;
	UINT32 ldws_warn_lock_time;
	UINT32 ldws_focal_length;
	UINT32 ldws_sensor_pixel_size;
	UINT32 ldws_left_dist_thres;
	UINT32 ldws_right_dist_thres;
}laneDetCfgGet_t;

/**< frame buffer addr must align to 16 byte and width must be mutiple of 8 */
#if 1
typedef padasFrameBuf_t laneDetectFrameBuf_t;
#else
typedef modeswFrameBuf_t laneDetectFrameBuf_t;
#endif
#endif
