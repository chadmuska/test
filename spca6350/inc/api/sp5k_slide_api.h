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
#ifndef _SP5K_SLIDE_API_H_
#define _SP5K_SLIDE_API_H_

/*******************************************************************************
*                            H E A D E R   F I L E S
*******************************************************************************/
#include "middleware/slide_def.h"

/*******************************************************************************
*                              C O N S T A N T S
*******************************************************************************/
/* sp5kSlideCfgSet() */
typedef enum _sp5kSlideCfg_e {
	SP5K_SLIDE_UNIT_TIME           = SLIDE_UNIT_TIME,
	SP5K_SLIDE_UNIT_SIZE           = SLIDE_UNIT_SIZE,
	SP5K_SLIDE_STEP_SIZE           = SLIDE_STEP_SIZE,
	SP5K_SLIDE_FRAME_COLOR         = SLIDE_FRAME_COLOR,
	SP5K_SLIDE_CLR_PREV_IMG_ENABLE = SLIDE_CLR_PREV_IMG_ENABLE,
	SP5K_SLIDE_EFFECT_ID           = SLIDE_EFFECT_ID,
	SP5K_SLIDE_ALIGN               = SLIDE_ALIGN,
	SP5K_SLIDE_DIR                 = SLIDE_DIR,
	SP5K_SLIDE_MOVE                = SLIDE_MOVE,
	SP5K_SLIDE_ORIENT              = SLIDE_ORIENT,
	SP5K_SLIDE_SIDE                = SLIDE_SIDE,
	SP5K_SLIDE_CORNER              = SLIDE_CORNER,
	SP5K_SLIDE_AXIS                = SLIDE_AXIS,
} sp5kSlideCfg_e;

/* sp5kSlideCfgSet(SP5K_SLIDE_EFFECT_ID, ...) */
typedef enum _sp5kSlideEffect_e {
	SP5K_SLIDE_EFFECT_NONE           = SLIDE_EFFECT_NONE,
	SP5K_SLIDE_EFFECT_RANDOM         = SLIDE_EFFECT_RANDOM,
	SP5K_SLIDE_EFFECT_CUSTOM         = SLIDE_EFFECT_CUSTOM,
	SP5K_SLIDE_EFFECT_FADE           = SLIDE_EFFECT_FADE,
	SP5K_SLIDE_EFFECT_FRAME_DRAG     = SLIDE_EFFECT_FRAME_DRAG,
	SP5K_SLIDE_EFFECT_FRAME_SCROLL   = SLIDE_EFFECT_FRAME_SCROLL,
	SP5K_SLIDE_EFFECT_FRAME_STRETCH  = SLIDE_EFFECT_FRAME_STRETCH,
	SP5K_SLIDE_EFFECT_STRIP_DRAG     = SLIDE_EFFECT_STRIPE_DRAG,
	SP5K_SLIDE_EFFECT_STRIPE_DRAG    = SLIDE_EFFECT_STRIPE_DRAG,
	SP5K_SLIDE_EFFECT_DRAPE_DRAG     = SLIDE_EFFECT_DRAPE_DRAG,
	SP5K_SLIDE_EFFECT_DRAPE_PROGRESS = SLIDE_EFFECT_DRAPE_PROGRESS,
	SP5K_SLIDE_EFFECT_SLOPE_PROGRESS = SLIDE_EFFECT_SLOPE_PROGRESS,
	SP5K_SLIDE_EFFECT_ZIPPER_DRAG    = SLIDE_EFFECT_ZIPPER_DRAG,
	SP5K_SLIDE_EFFECT_ZOOM_FADE      = SLIDE_EFFECT_ZOOM_FADE,
	SP5K_SLIDE_EFFECT_DIAGONAL_CUT   = SLIDE_EFFECT_DIAGONAL_CUT,
	SP5K_SLIDE_EFFECT_STRIPE_FADE    = SLIDE_EFFECT_STRIPE_FADE,
	SP5K_SLIDE_EFFECT_TWIST          = SLIDE_EFFECT_TWIST,
	SP5K_SLIDE_EFFECT_SWIRL_DRAG     = SLIDE_EFFECT_SWIRL_DRAG,
	SP5K_SLIDE_EFFECT_CROSS_SPLIT    = SLIDE_EFFECT_CROSS_SPLIT,
	SP5K_SLIDE_EFFECT_DOOR_OPEN      = SLIDE_EFFECT_DOOR_OPEN,
	SP5K_SLIDE_EFFECT_BLOCK_PROGRESS = SLIDE_EFFECT_BLOCK_PROGRESS,
} sp5kSlideEffect_e;

/* sp5kSlideCfgSet(SP5K_SLIDE_ALIGN, ...) */
typedef enum _sp5kSlideAlign_e {
	SP5K_SLIDE_ALIGN_TOP_LEFT     = SLIDE_ALIGN_TOP_LEFT,
	SP5K_SLIDE_ALIGN_TOP          = SLIDE_ALIGN_TOP,
	SP5K_SLIDE_ALIGN_TOP_RIGHT    = SLIDE_ALIGN_TOP_RIGHT,
	SP5K_SLIDE_ALIGN_LEFT         = SLIDE_ALIGN_LEFT,
	SP5K_SLIDE_ALIGN_CENTER       = SLIDE_ALIGN_CENTER,
	SP5K_SLIDE_ALIGN_RIGHT        = SLIDE_ALIGN_RIGHT,
	SP5K_SLIDE_ALIGN_BOTTOM_LEFT  = SLIDE_ALIGN_BOTTOM_LEFT,
	SP5K_SLIDE_ALIGN_BOTTOM       = SLIDE_ALIGN_BOTTOM,
	SP5K_SLIDE_ALIGN_BOTTOM_RIGHT = SLIDE_ALIGN_BOTTOM_RIGHT,
} sp5kSlideAlign_e;

/* sp5kSlideCfgSet(SP5K_SLIDE_DIR, ...) */
typedef enum _sp5kSlideDir_e {
	SP5K_SLIDE_DIR_TOP_LEFT     = SLIDE_DIR_TOP_LEFT,
	SP5K_SLIDE_DIR_TOP          = SLIDE_DIR_TOP,
	SP5K_SLIDE_DIR_TOP_RIGHT    = SLIDE_DIR_TOP_RIGHT,
	SP5K_SLIDE_DIR_LEFT         = SLIDE_DIR_LEFT,
	SP5K_SLIDE_DIR_RIGHT        = SLIDE_DIR_RIGHT,
	SP5K_SLIDE_DIR_BOTTOM_LEFT  = SLIDE_DIR_BOTTOM_LEFT,
	SP5K_SLIDE_DIR_BOTTOM       = SLIDE_DIR_BOTTOM,
	SP5K_SLIDE_DIR_BOTTOM_RIGHT = SLIDE_DIR_BOTTOM_RIGHT,
} sp5kSlideDir_e;

/* sp5kSlideCfgSet(SP5K_SLIDE_MOVE, ...) */
typedef enum _sp5kSlideMove_e {
	SP5K_SLIDE_MOVE_IN  = SLIDE_MOVE_IN,
	SP5K_SLIDE_MOVE_OUT = SLIDE_MOVE_OUT,
} sp5kSlideMove_e;

/* sp5kSlideCfgSet(SP5K_SLIDE_ORIENT, ...) */
typedef enum _sp5kSlideOrient_e {
	SP5K_SLIDE_ORIENT_CW  = SLIDE_ORIENT_CW,
	SP5K_SLIDE_ORIENT_CCW = SLIDE_ORIENT_CCW,
} sp5kSlideOrient_e;

/* sp5kSlideCfgSet(SP5K_SLIDE_SIDE, ...) */
typedef enum _sp5kSlideSide_e {
	SP5K_SLIDE_SIDE_TOP    = SLIDE_SIDE_TOP,
	SP5K_SLIDE_SIDE_LEFT   = SLIDE_SIDE_LEFT,
	SP5K_SLIDE_SIDE_RIGHT  = SLIDE_SIDE_RIGHT,
	SP5K_SLIDE_SIDE_BOTTOM = SLIDE_SIDE_BOTTOM,
} sp5kSlideSide_e;

/* sp5kSlideCfgSet(SP5K_SLIDE_CORNER, ...) */
typedef enum _sp5kSlideCorner_e {
	SP5K_SLIDE_CORNER_TOP_LEFT     = SLIDE_CORNER_TOP_LEFT,
	SP5K_SLIDE_CORNER_TOP_RIGHT    = SLIDE_CORNER_TOP_RIGHT,
	SP5K_SLIDE_CORNER_BOTTOM_RIGHT = SLIDE_CORNER_BOTTOM_RIGHT,
	SP5K_SLIDE_CORNER_BOTTOM_LEFT  = SLIDE_CORNER_BOTTOM_LEFT,
} sp5kSlideCorner_e;

/* sp5kSlideCfgSet(SP5K_SLIDE_AXIS, ...) */
typedef enum _sp5kSlideAxis_e {
	SP5K_SLIDE_AXIS_HORI  = SLIDE_AXIS_HORI,
	SP5K_SLIDE_AXIS_VERTI = SLIDE_AXIS_VERTI,
} sp5kSlideAxis_e;

/* sp5kSlideCfgSet(SP5K_SLIDE_HORI, ...). */
typedef enum _sp5kSlideHori_e {
	SP5K_SLIDE_HORI_LEFT  = SLIDE_HORI_LEFT,
	SP5K_SLIDE_HORI_RIGHT = SLIDE_HORI_RIGHT,
} sp5kSlideHori_e;

/* sp5kSlideCfgSet(SP5K_SLIDE_VERTI, ...). */
typedef enum _sp5kSlideVerti_e {
	SP5K_SLIDE_VERTI_TOP    = SLIDE_VERTI_TOP,
	SP5K_SLIDE_VERTI_BOTTOM = SLIDE_VERTI_BOTTOM,
} sp5kSlideVerti_e;

/* sp5kSlideCfgSet(SP5K_SLIDE_DIAGONAL, ...). */
typedef enum _sp5kSlideDiagonal_e {
	SP5K_SLIDE_DIAGONAL_FORE = SLIDE_DIAGONAL_FORE,
	SP5K_SLIDE_DIAGONAL_BACK = SLIDE_DIAGONAL_BACK,
} sp5kSlideDiagonal_e;

/* sp5kSlideCfgSet(SP5K_SLIDE_PLANE, ...). */
typedef enum _sp5kSlidePlane_e {
	SP5K_SLIDE_PLANE_FRONT = SLIDE_PLANE_FRONT,
	SP5K_SLIDE_PLANE_BACK  = SLIDE_PLANE_BACK,
} sp5kSlidePlane_e;

/* custom frame format. */
typedef enum _sp5kSlideCustomFrameFmt_e {
	SP5K_SLIDE_CUSTOM_FRAME_FMT_RGB565 = SLIDE_CUSTOM_FRAME_FMT_RGB565,
	SP5K_SLIDE_CUSTOM_FRAME_FMT_YUV422 = SLIDE_CUSTOM_FRAME_FMT_YUV422,
} sp5kSlideCustomFrameFmt_e;

/* custom effect state. */
typedef enum _sp5kSlideCustomEffectState_e {
	SP5K_SLIDE_CUSTOM_EFFECT_PLAY = SLIDE_CUSTOM_EFFECT_PLAY,
	SP5K_SLIDE_CUSTOM_EFFECT_STOP = SLIDE_CUSTOM_EFFECT_STOP,
} sp5kSlideCustomEffectState_e;

/*******************************************************************************
*                              D A T A   T Y P E S
*******************************************************************************/
#define sp5kSlideImage_t slideImage_t
#define sp5kSlideCustomEffectCallback_t slideCustomEffectCallback_t 

/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*******************************************************************************/
/* sp5k_slide.c */
UINT32 sp5kSlideCfgSet(UINT32 paramId, UINT32 param1, UINT32 param2, UINT32 param3, UINT32 param4);
UINT32 sp5kSlideImageSet(UINT8 *imgBuf, UINT32 dramW, UINT32 dramH, UINT32 dispW, UINT32 dispH);
UINT32 sp5kSlideStart(void);
UINT32 sp5kSlideStop(void);
UINT32 sp5kSlidePause(void);
UINT32 sp5kSlideResume(void);
UINT32 sp5kSlideCustomEffectCallbackReg(void *func);

#endif  /* _SP5K_SLIDE_API_H_ */

