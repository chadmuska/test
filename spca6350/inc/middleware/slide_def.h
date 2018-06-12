/**************************************************************************
 *
 *       Copyright (c) 2005-2016 by iCatch Technology, Inc.
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
/* DO NOT EDIT the next 2 lines for auto revision by subversion
$Rev: 26010 $
$Date:: 2016-04-20 16:06:10#$
 */

#ifndef _SLIDE_DEF_H_
#define _SLIDE_DEF_H_

/*******************************************************************************
*                              C O N S T A N T S
*******************************************************************************/
/* for slideCfgSet(). */
typedef enum _slideCfg_e {
	SLIDE_UNIT_TIME,
	SLIDE_UNIT_SIZE,
	SLIDE_STEP_SIZE,
	SLIDE_FRAME_COLOR,
	SLIDE_CLR_PREV_IMG_ENABLE,
	SLIDE_EFFECT_ID,
	SLIDE_ALIGN,
	SLIDE_DIR,
	SLIDE_MOVE,
	SLIDE_ORIENT,
	SLIDE_SIDE,
	SLIDE_CORNER,
	SLIDE_AXIS,
	SLIDE_HORI,
	SLIDE_VERTI,
	SLIDE_DIAGONAL,
	SLIDE_PLANE,
	SLIDE_CFG_MAX,
} slideCfg_e;

/* for slideCfgSet(SLIDE_EFFECT_ID, ...). */
typedef enum _slideEffect_e {
	SLIDE_EFFECT_NONE = 0,
	SLIDE_EFFECT_RANDOM,
	SLIDE_EFFECT_CUSTOM,
	SLIDE_EFFECT_FADE,
	SLIDE_EFFECT_FRAME_DRAG,
	SLIDE_EFFECT_FRAME_SCROLL,
	SLIDE_EFFECT_FRAME_STRETCH,
	SLIDE_EFFECT_STRIPE_DRAG,
	SLIDE_EFFECT_DRAPE_DRAG,
	SLIDE_EFFECT_DRAPE_PROGRESS,
	SLIDE_EFFECT_SLOPE_PROGRESS,
	SLIDE_EFFECT_ZIPPER_DRAG,
	SLIDE_EFFECT_ZOOM_FADE,
	SLIDE_EFFECT_DIAGONAL_CUT,
	SLIDE_EFFECT_STRIPE_FADE,
	SLIDE_EFFECT_TWIST,
	SLIDE_EFFECT_SWIRL_DRAG,
	SLIDE_EFFECT_CROSS_SPLIT,
	SLIDE_EFFECT_DOOR_OPEN,
	SLIDE_EFFECT_BLOCK_PROGRESS,
	SLIDE_EFFECT_MAX,
} slideEffect_e;

/* for slideCfgSet(SLIDE_ALIGN, ...). */
typedef enum _slideAlign_e {
	SLIDE_ALIGN_TOP_LEFT     = 0x11,
	SLIDE_ALIGN_TOP          = 0x12,
	SLIDE_ALIGN_TOP_RIGHT    = 0x14,
	SLIDE_ALIGN_LEFT         = 0x21,
	SLIDE_ALIGN_CENTER       = 0x22,
	SLIDE_ALIGN_RIGHT        = 0x24,
	SLIDE_ALIGN_BOTTOM_LEFT  = 0x41,
	SLIDE_ALIGN_BOTTOM       = 0x42,
	SLIDE_ALIGN_BOTTOM_RIGHT = 0x44,
	SLIDE_ALIGN_MAX,
} slideAlign_e;

/* for slideCfgSet(SLIDE_DIR, ...). */
typedef enum _slideDir_e {
	SLIDE_DIR_TOP_LEFT     = 0x11,
	SLIDE_DIR_TOP          = 0x12,
	SLIDE_DIR_TOP_RIGHT    = 0x14,
	SLIDE_DIR_LEFT         = 0x21,
	SLIDE_DIR_RIGHT        = 0x24,
	SLIDE_DIR_BOTTOM_LEFT  = 0x41,
	SLIDE_DIR_BOTTOM       = 0x42,
	SLIDE_DIR_BOTTOM_RIGHT = 0x44,
	SLIDE_DIR_MAX,
} slideDir_e;

/* for slideCfgSet(SLIDE_MOVE, ...). */
typedef enum _slideMove_e {
	SLIDE_MOVE_IN  = 0,
	SLIDE_MOVE_OUT = 1,
	SLIDE_MOVE_MAX,
} slideMove_e;

/* for slideCfgSet(SLIDE_ORIENT, ...). */
typedef enum _slideOrient_e {
	SLIDE_ORIENT_CW  = 0,
	SLIDE_ORIENT_CCW = 1,
	SLIDE_ORIENT_MAX,
} slideOrient_e;

/* for slideCfgSet(SLIDE_SIDE, ...). */
typedef enum _slideSide_e {
	SLIDE_SIDE_TOP    = 0x12,
	SLIDE_SIDE_LEFT   = 0x21,
	SLIDE_SIDE_RIGHT  = 0x24,
	SLIDE_SIDE_BOTTOM = 0x42,
	SLIDE_SIDE_MAX,
} slideSide_e;

/* for slideCfgSet(SLIDE_CORNER, ...). */
typedef enum _slideCorner_e {
	SLIDE_CORNER_TOP_LEFT     = 0x11,
	SLIDE_CORNER_TOP_RIGHT    = 0x14,
	SLIDE_CORNER_BOTTOM_RIGHT = 0x44,
	SLIDE_CORNER_BOTTOM_LEFT  = 0x41,
	SLIDE_CORNER_MAX,
} slideCorner_e;

/* for slideCfgSet(SLIDE_AXIS, ...). */
typedef enum _slideAxis_e {
	SLIDE_AXIS_HORI  = 0,
	SLIDE_AXIS_VERTI = 1,
	SLIDE_AXIS_MAX,
} slideAxis_e;

/* for slideCfgSet(SLIDE_HORI, ...). */
typedef enum _slideHori_e {
	SLIDE_HORI_LEFT  = 0,
	SLIDE_HORI_RIGHT = 1,
	SLIDE_HORI_MAX,
} slideHori_e;

/* for slideCfgSet(SLIDE_VERTI, ...). */
typedef enum _slideVerti_e {
	SLIDE_VERTI_TOP    = 0,
	SLIDE_VERTI_BOTTOM = 1,
	SLIDE_VERTI_MAX,
} slideVerti_e;

/* for slideCfgSet(SLIDE_DIAGONAL, ...). */
typedef enum _slideDiagonal_e {
	SLIDE_DIAGONAL_FORE = 0,
	SLIDE_DIAGONAL_BACK = 1,
	SLIDE_DIAGONAL_MAX,
} slideDiagonal_e;

/* for slideCfgSet(SLIDE_PLANE, ...). */
typedef enum _slidePlane_e {
	SLIDE_PLANE_FRONT = 0,
	SLIDE_PLANE_BACK  = 1,
	SLIDE_PLANE_MAX,
} slidePlane_e;

/* custom frame format. */
typedef enum _slideCustomFrameFmt_e {
	SLIDE_CUSTOM_FRAME_FMT_RGB565 = 0,
	SLIDE_CUSTOM_FRAME_FMT_YUV422 = 1,
} slideCustomFrameFmt_e;

/* custom effect state. */
typedef enum _slideCustomEffectState_e {
	SLIDE_CUSTOM_EFFECT_PLAY = 0,
	SLIDE_CUSTOM_EFFECT_STOP = 1,
} slideCustomEffectState_e;

typedef struct _slideImage_t {
	UINT8 *pfrmBuf;
	UINT32 frmDramW;
	UINT32 frmDramH;
	UINT32 frmDispW;
	UINT32 frmDispH;
	UINT32 frmFmt;
} slideImage_t;

/* for slideInitCfgSet(). */
typedef enum _slideInitCfg_e {
	SLIDE_DISP_FRAME_BUF  = 0,
	SLIDE_DISP_FRAME_SIZE = 1,
	SLIDE_COMPACT_MODE    = 2,
} slideInitCfg_e;

/*******************************************************************************
*                              D A T A   T Y P E S
*******************************************************************************/
typedef UINT32 (*slideCustomEffectCallback_t)(slideImage_t *pfrm, slideImage_t *ptarget);

/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*******************************************************************************/
void slideEffectFadeInit(void);
void slideEffectZoomFadeInit(void);
void slideEffectFrameDragInit(void);
void slideEffectFrameScrollInit(void);
void slideEffectFrameStretchInit(void);
void slideEffectStripeDragInit(void);
void slideEffectDrapeDragInit(void);
void slideEffectDrapeProgressInit(void);
void slideEffectSlopeProgressInit(void);
void slideEffectZipperDragInit(void);
void slideEffectDiagonalCutInit(void);
void slideEffectStripeFadeInit(void);
void slideEffectTwistInit(void);
void slideEffectSwirlDragInit(void);
void slideEffectCrossSplitInit(void);
void slideEffectDoorOpenInit(void);
void slideEffectBlockProgressInit(void);

#endif  /* _SLIDE_DEF_H_ */

