/**************************************************************************
 *
 *       Copyright (c) 2010-2014 by iCatch Technology, Inc.
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
/**
 * @file sp5k_fbeauty_api.h
 * @brief Face beauty api interface.
 * @author Yi-Ming Kao
 *
 */
#ifndef _SP5K_FBEAUTY_API_H_
#define _SP5K_FBEAUTY_API_H_

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "common.h"
#include "middleware/common_types.h"
#include "middleware/alg_effect_def.h"



/**************************************************************************
 *                          C O N S T A N T S
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S
 **************************************************************************/

/**************************************************************************
 *                          D A T A   T Y P E S
 **************************************************************************/
#if 1
#define sp5kFaceBeautyFaceInfo_t fbFaceInfo_t
#else
typedef struct sp5kFaceBeautyFaceInfo_s {
	UINT32 x;
	UINT32 y;
	UINT32 size;
} sp5kFaceBeautyFaceInfo_t;
#endif

typedef struct sp5kFaceBeautyParam_s {
	sp5kFaceBeautyFaceInfo_t *pfaceInfo;
	UINT32 numFace;    /* Number of face*/
	UINT8 *psoftMask;  /* soften mask */
	UINT8 *pwhiteMask; /* whiten mask */
	UINT8 *psoftMaskPre;  /* soften mask */
	UINT8 *pwhiteMaskPre; /* whiten mask */
	UINT32 fbLevel; /*Face beauty level, 0 (no beauty) ~ 12 (strongest). */
	UINT32 fwLevel; /*Face whiteness level, 0 (no whitening) ~ 40 (strongest). */
	UINT32 eFac; /*Edge softening threshold.  If eFac increases, the more edges with larger contrast are soften.  0~255, suggest 30 */
	int deltaU;  /*U shift of color zone of skin, -255(yellow) ~ 255(blue), suggest 0*/
	int deltaV;  /*V shift of color zone of skin, -255(red) ~ 255(green), suggest 0*/
	UINT32 sclSM; /* Ratio of soften face range and FD face range, 4.4bit, suggest 48 */
	UINT32 sclWM; /* Ratio of whiten face range and FD face range, 4.4bit, suggest 96 */
	int dilaSM; /* 2b, 0(weak)~2(strong), defaut:0 */
	int dilaWM; /* 2b, 0(weak)~2(strong), defaut:0 */
	int smoothSM; /* 6b, 1(weak)~64(strong), default:8 */
	int smoothWM; /* 6b, 1(weak)~64(strong), default:32 */
} sp5kFaceBeautyParam_t;


typedef enum {
	SP5K_FACE_BEAUTY_CFG_MODE,
} sp5kFaceBeautyCfg_e;


typedef enum {
	SP5K_FACE_BEAUTY_MODE_PLAYBACK,
	SP5K_FACE_BEAUTY_MODE_PREVIEW,
	SP5K_FACE_BEAUTY_MODE_CAPTURE,
	SP5K_FACE_BEAUTY_MODE_CAPTURE_SPECIAL,
} sp5kFaceBeautyMode_e;




/**************************************************************************
 *                  E X T E R N A L   R E F E R E N C E
 **************************************************************************/

/**************************************************************************
 *              F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
UINT32 sp5kFaceBeautyPrevMaskCalc(frameBufInfo_t *pframe, 
	sp5kFaceBeautyParam_t *pcfg);
/*
 * sp5kFaceBeautyCfgSet(SP5K_FACE_BEAUTY_CFG_MODE, sp5kFaceBeautyMode_e, 
 *     sp5kFaceBeautyParam_t *);
 */
UINT32 sp5kFaceBeautyCfgSet(UINT32 selector, ...);
UINT32 sp5kFaceBeautyDo(frameBufInfo_t *psrc1, frameBufInfo_t *psrc2, 
	frameBufInfo_t *pdst);
void  sp5kFaceBeautyDetailLevelSet(UINT8 enable);
void  sp5kFaceBeautyLceSet(UINT8 enable);
void  sp5kFaceBeautySrc1BypassSet(UINT8 enable);
void  sp5kFaceBeautyLceCfg(
		UINT8 w1, 				/* strength of high frequency local difference, 2.6b, sugget 44  (ex:0.6875*64=44) */
		UINT8 w2, 				/* strength of medium frequency local difference, 2.6b, sugget 12  (ex:0.1875*64=12) */
		UINT8 overShootRatio	/* strength of over-shoot, 0.8b, suggest 179, (ex:0.7x256=179) */
);



#ifdef __cplusplus
}
#endif

#endif /*_TEMPLATE_H_*/
