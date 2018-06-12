/**************************************************************************
 *
 *       Copyright (c) 2005-2011 by iCatch Technology, Inc.
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
 *  Hsin-Chu, Taiwan, R.O.C.
 *
 *  Author: Austin
 *
 **************************************************************************/
/* DO NOT EDIT the next 2 lines for auto revision by subversion
$Rev: 10774 $
$Date:: 2013-12-09 11:48:33#$
 */

#ifndef _SPEFFECT_DEF_H_
#define _SPEFFECT_DEF_H_

/**************************************************************************
 *                           C O N S T A N T S
 **************************************************************************/
typedef enum {
	SPE_EFFECT_BW = 0,
	SPE_EFFECT_NEGATIVE,
	SPE_EFFECT_EMBOSS,
	SPE_EFFECT_MOSAIC,
	SPE_EFFECT_SEPIA,
	SPE_EFFECT_RED,
	SPE_EFFECT_GREEN,
	SPE_EFFECT_BLUE,
	SPE_EFFECT_BLUR_OBSOLETE, /* not documented in WIKI nor implemented */
	SPE_EFFECT_FISHEYE,
	SPE_EFFECT_YUV_REDO_OBSOLETE, /* duplicated wtih SP5K_IMGEDIT_CDSP */
} speffectID_t;

/**************************************************************************
 *                          D A T A    T Y P E S
 **************************************************************************/
typedef struct {
	UINT8 *pbuf;
	UINT32 bufW;
	UINT32 bufH;
	SINT32 roiX;
	SINT32 roiY;
} effectYuvBuf_t;

#endif /* _SPEFFECT_DEF_H_ */

