/**************************************************************************
 *
 *       Copyright (c) 2006-2016 by iCatch Technology, Inc.
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
 *  Author: Edward Kuo
 *
 **************************************************************************/
/* DO NOT EDIT the next 2 lines for auto revision by subversion
$Rev: 26010 $
$Date:: 2016-04-20 16:06:10#$
 */

#ifndef _TS_DEF_H_
#define _TS_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************
 *                            H E A D E R   F I L E S
  **************************************************************************/
#include "middleware/global_def.h"

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/

/**************************************************************************
 *                                  M A C R O S
 **************************************************************************/

/**************************************************************************
 *                              D A T A   T Y P E S
 **************************************************************************/
/**
 *  @brief        data structure of ts event 
 *
 */
typedef struct TouchScreenEvent_s {
	UINT16 pressure;
	UINT16 x;
	UINT16 y;
	UINT16 msec;
} TouchScreenEvent_t;

/**
 *  @brief        transformation matrix for ts calibration
 *
 */
typedef struct TouchScreenCalibration_s {
	SINT32 xscale;			/* new_x = (xscale * x + xtrans) >> shift */
	SINT32 xtrans;
	SINT32 yscale;			/* new_y = (yscale * y + ytrans) >> shift */
	SINT32 ytrans;
	UINT16 shift;
	UINT16 xyswap;			/* swap x and y */
} TouchScreenCalibration_t;

/**
 *  @brief        data structure to config gpio pins and adc
 *
 */
typedef struct TouchScreenPinCfg_s {
	UINT16 pinXplus;			/* V top of X axis */    /* (gpio grp << 8) | pin num */
	UINT16 pinXpLvl;			/* active level of X plus */
	UINT16 pinXminus;			/* V bottom of X axis */
	UINT16 pinXmLvl;			/* active level of X minus */
	UINT16 pinYplus;			/* V top of Y axis */
	UINT16 pinYpLvl;			/* active level of Y plus */
	UINT16 pinYminus;			/* V bottom of Y axis */
	UINT16 pinYmLvl;			/* active level of Y minus */
	UINT8  adcDownPin;			/* ts down detect pin */ /* 0:X minus, 1:Y minus*/
	UINT8  adcPosX;				/* adc to read x pos */
	UINT8  adcPosY;				/* adc to read y pos */
} TouchScreenPinCfg_t;

/**
 *  @brief	data structure to ts ADC Threshold
 *
 */
typedef struct TouchScreenThreshold_s {
	SINT32 minThresholdX;
	SINT32 maxThresholdX;	
	SINT32 minThresholdY;
	SINT32 maxThresholdY;			
} TouchScreenThreshold_t;

/**
 *  @brief enum Trajectory
 *
 */

typedef enum {
	LINE_TOP_LEFT_TO_RIGHT     		= 0x00,
	LINE_CENTER_LEFT_TO_RIGHT     	= 0x01,
	LINE_BOTTOM_LEFT_TO_RIGHT     	= 0x02,
	LINE_TOP_RIGHT_TO_LEFT     		= 0x03,
	LINE_CENTER_RIGHT_TO_LEFT     	= 0x04,
	LINE_BOTTOM_RIGHT_TO_LEFT      = 0x05,
	LINE_LEFT_TOP_TO_BOTTOM     	= 0x06,
	LINE_CENTER_TOP_TO_BOTTOM     = 0x07,
	LINE_RIGHT_TOP_TO_BOTTOM     	= 0x08,
	LINE_LEFT_BOTTOM_TO_TOP     	= 0x09,
	LINE_CENTER_BOTTOM_TO_TOP     = 0x0A,
	LINE_RIGHT_BOTTOM_TO_TOP    	= 0x0B,
	DIAGONAL_LTOP_TO_RBOTTOM     	= 0x0C,
	DIAGONAL_RTOP_TO_LBOTTOM     	= 0x0D,
	DIAGONAL_LBOTTOM_TO_RTOP     	= 0x0E,
	DIAGONAL_RBOTTOM_TO_LTOP     	= 0x0F,
	CIRCLE_REGULAR     				= 0x10,
	CIRCLE_INVERSE    				= 0x11,
	NOT_DEFINE    					= 0xFF,
}TsTrajectory_e;

typedef struct TouchScreenTsTrajectory_s {
	TsTrajectory_e result;
} TouchScreenTsTrajectory_t;

typedef struct TouchScreenMatrixPos_s {
	UINT16 MatrixPos;
} TouchScreenMatrixPos_t;

typedef void (*fpTsCallback_t)(UINT32);

#ifdef __cplusplus
}
#endif

#endif /* _TS_DEF_H_ */
