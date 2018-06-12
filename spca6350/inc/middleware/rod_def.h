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
#ifndef _ROD_DEF_H_
#define _ROD_DEF_H_

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
	ROD_DEPATURE_THRESHOLD,
	ROD_CENTRAL_POINT,
	ROD_IMAGE_SCALE_FACTOR,
	ROD_IMAGE_ROI_CROP_OFF,
	ROD_MAP_PARAM,

	/* the following for get only */
	ROD_WARNREGION_GET,
	ROD_WARNREGION_COORD_GET,
	ROD_DISTANCE_GET,
	ROD_WARN_FLAG,
	ROD_RESULT_GET = 0x1000,
	ROD_CONFIG_GET = 0x2000,
} rodCfgSel_t;

/**************************************************************************
 *                                  M A C R O S
 **************************************************************************/

/**************************************************************************
 *                              D A T A   T Y P E S
 **************************************************************************/
 #if 0
typedef struct redeyeFaceInfo_s {
	UINT32 faceX;		/* ROI start x */
	UINT32 faceY;		/* ROI start y */
	UINT32 faceSize;	/* ROI width/height */
	UINT32 faceAngle;   /* The angle of this face */
} redeyeFaceInfo_t;
#endif

typedef struct{
	int thita_L[5];
	int thita_R[5];
	int r_L[5];
	int r_R[5];
}ROD_LD_HOUGH_DATA;
typedef struct {
	UINT32 dw0,dw1,dw2,dw3,dw4,dw5,dw6,dw7,dw8,dw9,dw10,dw11
		#if SPCA5330
		,
		dw12,
		dw13,
		dw14,
		dw15
		#endif
		;
}moeQue_t;

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
}rodlanePosotion;

typedef struct rodMapSet_s{
	UINT32 up_coord_y;
	UINT32 mid_coord_y;
	UINT32 low_coord_y;
	UINT32 image_width;
	UINT32 image_Height;
}rodMapSet_t;

typedef struct rodCfgGet_s{
	UINT32 up_coord_y;
	UINT32 mid_coord_y;
	UINT32 low_coord_y;
	UINT32 image_width;
	UINT32 image_Height;
}rodCfgGet_t;

typedef struct {
	UINT8 leftLaneSignal;
	UINT8 rightLaneSignal;
}roddepartureWarning;


/**< frame buffer addr must align to 16 byte and width must be mutiple of 8 */
#if 1
typedef padasFrameBuf_t rodFrameBuf_t;
#else
typedef modeswFrameBuf_t rodFrameBuf_t;
#endif

#endif
