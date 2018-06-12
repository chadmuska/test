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
#ifndef _REDEYE_DEF_H_
#define _REDEYE_DEF_H_

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "common.h"

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/

/**************************************************************************
 *                                  M A C R O S
 **************************************************************************/

/**************************************************************************
 *                              D A T A   T Y P E S
 **************************************************************************/
typedef struct redeyeFaceInfo_s {
	UINT32 faceX;		/* ROI start x */
	UINT32 faceY;		/* ROI start y */
	UINT32 faceSize;	/* ROI width/height */
	UINT32 faceAngle;   /* The angle of this face */
} redeyeFaceInfo_t;

typedef struct redeyeEyeInfo_s {
	UINT32 eyeX;	/* ROI start x */
	UINT32 eyeY;	/* ROI start y */
	UINT32 eyeW;	/* ROI width */
	UINT32 eyeH;	/* ROI height */
} redeyeEyeInfo_t;

typedef struct redeyeEyeCircleInfo_s {
	UINT16 centerX; /* center x */
	UINT16 centerY; /* center y */
	UINT16 radius;  /* radius */
	UINT16 index;   /* face index */
} redeyeEyeCircleInfo_t;

#if 0/*use ftrackAngle directly*/
typedef enum {
	REDEYE_FACE_ANGLE_0 = 0x00,
	REDEYE_FACE_ANGLE_30,
	REDEYE_FACE_ANGLE_60,
	REDEYE_FACE_ANGLE_90,
	REDEYE_FACE_ANGLE_120,
	REDEYE_FACE_ANGLE_150,
	REDEYE_FACE_ANGLE_180,
	REDEYE_FACE_ANGLE_210,
	REDEYE_FACE_ANGLE_240,
	REDEYE_FACE_ANGLE_270,
	REDEYE_FACE_ANGLE_300,
	REDEYE_FACE_ANGLE_330,
	REDEYE_FACE_ANGLE_NONE = 0xFF,
} redeyeFaceAngle_e;
#endif

#endif /* _REDEYE_DEF_H_ */
