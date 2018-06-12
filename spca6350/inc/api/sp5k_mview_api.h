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
 *  Author: Ymkao
 *
 **************************************************************************/
#ifndef __SP5K_MVIEW_API_H__
#define __SP5K_MVIEW_API_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include "middleware/mview_def.h"

typedef mviewWindow_t sp5kMviewWindow_t;
typedef mviewRawInfo_t sp5kMviewRawInfo_t;

typedef enum {
	SP5K_MVIEW_CFG_ACCUM_PERIOD = MVIEW_CFG_ACCUM_PERIOD,
	SP5K_MVIEW_CFG_WINDOW_NUM = MVIEW_CFG_WINDOW_NUM,
	SP5K_MVIEW_CFG_WINDOW_MAX_WIDTH = MVIEW_CFG_WINDOW_MAX_WIDTH,
	SP5K_MVIEW_CFG_WINDOW_MAX_HEIGHT = MVIEW_CFG_WINDOW_MAX_HEIGHT,
	SP5K_MVIEW_CFG_WINDOW_INFO = MVIEW_CFG_WINDOW_INFO,
	SP5K_MVIEW_CFG_WINDOW_PRE_CB = MVIEW_CFG_WINDOW_PRE_CB,
	SP5K_MVIEW_CFG_WINDOW_POST_CB = MVIEW_CFG_WINDOW_POST_CB,
	SP5K_MVIEW_CFG_FRAME_PRE_CB = MVIEW_CFG_FRAME_PRE_CB,
	SP5K_MVIEW_CFG_FRAME_RAW_CB = MVIEW_CFG_FRAME_RAW_CB,
} sp5kMviewCfgSel_t;

/* The window callback format */
typedef UINT32 (*sp5kMviewWinCb_t)(UINT32, sp5kMviewWindow_t, void *);
/* The frame callback format */
typedef UINT32 (*sp5kMviewFrameCb_t)(UINT32, void *);
/* The frame raw callback format */
typedef UINT32 (*sp5kMviewFrameRawCb_t)(UINT32 addr, UINT32 rsvd, 
	sp5kMviewRawInfo_t *);

/* Formal APIs */
extern UINT32 sp5kMviewCfgSet(UINT32 selector, UINT32 value);
extern UINT32 sp5kMviewCfgGet(UINT32 selector);
extern UINT32 sp5kMviewCreate(void);
extern UINT32 sp5kMviewDestroy(void);
extern UINT32 sp5kMviewWindowRelease(UINT32 id);

#ifdef __cplusplus
}
#endif

#endif /* __SP5K_MVIEW_API_H__ */

