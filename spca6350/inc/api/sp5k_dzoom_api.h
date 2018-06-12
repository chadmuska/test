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
*  File Description:
*
*  Author: Matt Wang
*
*******************************************************************************/
#ifndef _SP5K_DZOOM_API_H_
#define _SP5K_DZOOM_API_H_

/*******************************************************************************
*                            H E A D E R   F I L E S
*******************************************************************************/
#include "common.h"
#include "middleware/disp_def.h"

/*******************************************************************************
*                              C O N S T A N T S
*******************************************************************************/
typedef enum {
	SP5K_DZOOM_RATIO_TYPE = 0,
	/*SP5K_DZOOM_PAN_ENABLE = 1,*/
} sp5kDzoomCfg_e;

typedef enum {
	SP5K_DZOOM_RATIO_KEEP_INSIDE  = DISP_DZOOM_FIT_METHOD_DST_INSIDE,
	SP5K_DZOOM_RATIO_KEEP_OUTSIDE = DISP_DZOOM_FIT_METHOD_DST_OUTSIDE,
	SP5K_DZOOM_RATIO_FIT_IMAGE    = DISP_DZOOM_FIT_METHOD_SRC_VARY_WINDOW,
	SP5K_DZOOM_RATIO_FIX_WINDOW   = DISP_DZOOM_FIT_METHOD_SRC_FIX_WINDOW,
	SP5K_DZOOM_RATIO_STRETCH      = DISP_DZOOM_FIT_METHOD_SRC_STRETCH,
} sp5kDzoomRatioType_e;

/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*******************************************************************************/
UINT32 sp5kZoomFactorSet(UINT32 mode, UINT32 zfac);
UINT32 sp5kPanCenterSet(UINT32 mode, SINT32 xpcen, SINT32 ypcen);
UINT32 sp5kZoomCfgSet(UINT32 id, UINT32 para1, UINT32 para2, UINT32 para3, UINT32 para4);
/**
 * \note
 * x: offset x, must be multiple of 2
 * y: offset y
 * width: roi width, must be multiple of 2
 * height: roi height
 */
UINT32 sp5kZoomRoiSet(UINT32 mode, UINT32 x, UINT32 y, UINT32 width, UINT32 height);
UINT32 sp5kZoomRoiGet(UINT32 mode, UINT32 *px, UINT32 *py, UINT32 *pwidth, UINT32 *pheight);

#endif  /* _SP5K_DZOOM_API_H_ */

