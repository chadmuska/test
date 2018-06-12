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
 *  Author: Matt Wang                                                     *
 *                                                                        *
 **************************************************************************/
/* DO NOT EDIT the next 2 lines for auto revision by subversion
$Rev: 26010 $
$Date:: 2016-04-20 16:06:10#$
 */

#ifndef __FNT_DEF_H__
#define __FNT_DEF_H__

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
#include "common.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
/*
 * fntResourceFileLoad(),
 * fntCapabilityGet(),
 * fntSizeGet(),
 * fntShapeGet(),
 * fntIdSeek().
 */
typedef enum {
	FNT_TYPE_FONT    = 0,
	FNT_TYPE_ICON    = 1,
	FNT_TYPE_ICON_2  = 2,
	FNT_TYPE_ICON_3  = 3,
	FNT_TYPE_ICON_4  = 4,
	FNT_TYPE_ICON_5  = 5,
	FNT_TYPE_ICON_6  = 6,
	FNT_TYPE_ICON_7  = 7,
	FNT_TYPE_ICON_8  = 8,
	FNT_TYPE_ICON_9  = 9,
	FNT_TYPE_ICON_10 = 10,
	FNT_TYPE_ICON_11 = 11,
	FNT_TYPE_ICON_12 = 12,
	FNT_TYPE_ICON_13 = 13,
	FNT_TYPE_ICON_14 = 14,
	FNT_TYPE_ICON_15 = 15,
	FNT_TYPE_ICON_16 = 16,
	FNT_TYPE_STAMP,
	FNT_TYPE_FONT_CUSTOM,
	FNT_TYPE_TOT,
} fntType_e;

/**************************************************************************
 *                           D A T A   T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/
/*
 * debug switch.
 *
 * macros:
 *   FNT_DBG_SWITCH_SET(),
 *   FNT_DBG_SWITCH_GET().
 */
#if ( RELEASE_VER == 1 )
#define FNT_DBG_SWITCH_SET(sw)
#define FNT_DBG_SWITCH_GET()  0
#else
#define FNT_DBG_SWITCH_SET(sw)  fntDbgSwitch = sw
#define FNT_DBG_SWITCH_GET()  fntDbgSwitch
#endif

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/
/*
 * fnt.c
 */
extern UINT32 fntDbgSwitch;  /* DON'T ACCESS THIS VARIABLE DIRECTLY, USE MACROS!! */

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
/*
 * fnt.c
 */
void fntTimeMeasureReset(void);

#endif  /* __FNT_DEF_H__ */

