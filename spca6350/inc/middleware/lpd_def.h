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
#ifndef _LPD_DEF_H_
#define _LPD_DEF_H_

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "common.h"
#include "middleware/modesw_def.h"
#include "adas_def.h"


/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
typedef enum {
	LPD_RESULT_GET = 0x1000,
} lpdCfgSel_t;

typedef enum {
	LPD_RESULT_NOT_FOUND = 0,
	LPD_RESULT_LESS_THAN_SIX_DIGITS,
	LPD_RESULT_FOUND
} lpdResultState_t;

/**************************************************************************
 *                                  M A C R O S
 **************************************************************************/

/**************************************************************************
 *                              D A T A   T Y P E S
 **************************************************************************/


/**< frame buffer addr must align to 16 byte and width must be mutiple of 8 */
#if 1
typedef padasFrameBuf_t lpdFrameBuf_t;
#else
typedef modeswFrameBuf_t lpdFrameBuf_t;
#endif

#endif
