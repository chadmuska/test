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
#ifndef _SP5K_TS_API_H_
#define _SP5K_TS_API_H_

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "middleware/ts_def.h"

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
#define SP5K_TS_SYS_CFG_COORD_DUMP	0

/**************************************************************************
 *                                  M A C R O S
 **************************************************************************/
#define sp5kTSGpioCfg(grp, pin)		((grp << 8) | pin)

/**************************************************************************
 *                              D A T A   T Y P E S
 **************************************************************************/
typedef enum {
	SP5K_TS_LINE_TOP_LEFT_TO_RIGHT		= LINE_TOP_LEFT_TO_RIGHT,
	SP5K_TS_LINE_CENTER_LEFT_TO_RIGHT	= LINE_CENTER_LEFT_TO_RIGHT,
	SP5K_TS_LINE_BOTTOM_LEFT_TO_RIGHT	= LINE_BOTTOM_LEFT_TO_RIGHT,
	SP5K_TS_LINE_TOP_RIGHT_TO_LEFT		= LINE_TOP_RIGHT_TO_LEFT,
	SP5K_TS_LINE_CENTER_RIGHT_TO_LEFT	= LINE_CENTER_RIGHT_TO_LEFT,
	SP5K_TS_LINE_BOTTOM_RIGHT_TO_LEFT	= LINE_BOTTOM_RIGHT_TO_LEFT,
	SP5K_TS_LINE_LEFT_TOP_TO_BOTTOM		= LINE_LEFT_TOP_TO_BOTTOM,
	SP5K_TS_LINE_CENTER_TOP_TO_BOTTOM	= LINE_CENTER_TOP_TO_BOTTOM,
	SP5K_TS_LINE_RIGHT_TOP_TO_BOTTOM	= LINE_RIGHT_TOP_TO_BOTTOM,
	SP5K_TS_LINE_LEFT_BOTTOM_TO_TOP		= LINE_LEFT_BOTTOM_TO_TOP,
	SP5K_TS_LINE_CENTER_BOTTOM_TO_TOP	= LINE_CENTER_BOTTOM_TO_TOP,
	SP5K_TS_LINE_RIGHT_BOTTOM_TO_TOP	= LINE_RIGHT_BOTTOM_TO_TOP,
	SP5K_TS_DIAGONAL_LTOP_TO_RBOTTOM	= DIAGONAL_LTOP_TO_RBOTTOM,
	SP5K_TS_DIAGONAL_RTOP_TO_LBOTTOM	= DIAGONAL_RTOP_TO_LBOTTOM,
	SP5K_TS_DIAGONAL_LBOTTOM_TO_RTOP	= DIAGONAL_LBOTTOM_TO_RTOP,
	SP5K_TS_DIAGONAL_RBOTTOM_TO_LTOP	= DIAGONAL_RBOTTOM_TO_LTOP,
	SP5K_TS_CIRCLE_REGULAR				= CIRCLE_REGULAR,
	SP5K_TS_CIRCLE_INVERSE				= CIRCLE_INVERSE,
	SP5K_TS_NOT_DEFINE					= NOT_DEFINE,
} sp5kTsTrajectory_e;

/**************************************************************************
 *                      E X T E R N A L   R E F E R E N C E
 **************************************************************************/

/**************************************************************************
 *                  F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
UINT32 sp5kTsPinCfg(TouchScreenPinCfg_t *ptsCfg);
UINT32 sp5kTsInit(void);
void   sp5kTsRelease(void);
UINT32 sp5kTsEnable(UINT32 en);
void   sp5kTsSampleIntervalSet(UINT32 msec);
UINT32 sp5kTsSampleIntervalGet(void);
UINT32 sp5kTsCalibrationGet(TouchScreenCalibration_t *ptsCal);
UINT32 sp5kTsCalibrationSet(TouchScreenCalibration_t *ptsCal);
UINT32 sp5kTsEventReset(void);
UINT32 sp5kTsEventNumGet(void);
UINT32 sp5kTsEventGet(TouchScreenEvent_t *ptsEvent);
void   sp5kTsPenDownCbSet(fpTsCallback_t pfunc);
UINT32 sp5kTsThresholdSet(TouchScreenThreshold_t *ptsThresold);
UINT32 sp5kTsThresholdGet(TouchScreenThreshold_t *ptsThresold);
UINT32 sp5kTsTrajectoryResultGet(TouchScreenEvent_t *EvtQue,UINT32 tsEvtnum,TouchScreenTsTrajectory_t *result);
UINT32 sp5kTsSysCfgSet(UINT32 selector,UINT32 value);

#ifdef __cplusplus
}
#endif

#endif /* _SP5K_TS_API_H_ */
