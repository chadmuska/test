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
 *                                                                        *
 *  Author:				                                                  *
 *                                                                        *
 **************************************************************************/
/* DO NOT EDIT the next 2 lines for auto revision by subversion
$Rev: 26010 $
$Date:: 2016-04-20 16:06:10#$
 */

#ifndef _IME_DEF_H_
#define _IME_DEF_H_

#include "common.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

typedef enum _imeEffect_e{
	IME_MORPHING_EFF_FAT		,
	IME_MORPHING_EFF_FATTEN		,
	IME_MORPHING_EFF_PUNCH		,
	
	IME_MORPHING_EFF_THIN		,
	IME_MORPHING_EFF_SHORT		,
	IME_MORPHING_EFF_PINCH		,
	
	IME_MORPHING_EFF_BUBBLE		,
	
	IME_MORPHING_EFF_WAVE1		,
	IME_MORPHING_EFF_WAVE2		,
	
	IME_MORPHING_EFF_WAVE3		,
	
	IME_MORPHING_EFF_TWIST		,

	IME_MORPHING_EFF_LMIRROR	,
	IME_MORPHING_EFF_RMIRROR	,

	IME_MORPHING_EFF_TOTAL
} imeEffect_e;

/**************************************************************************
 *                          D A T A   T Y P E S
 **************************************************************************/

typedef struct _VECT_t{
	SINT32 x;
	SINT32 y;
} VECT_t;


#endif

