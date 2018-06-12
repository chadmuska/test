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

#ifndef __STR_DEF_H__
#define __STR_DEF_H__

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
#include "common.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
typedef enum {
	STR_ENCODE_ASCII   = 0,
	STR_ENCODE_UNICODE = 1,
} strEncode_t;

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
 *   STR_DBG_SWITCH_SET(),
 *   STR_DBG_SWITCH_GET().
 */
#if ( RELEASE_VER == 1 )
#define STR_DBG_SWITCH_SET(sw)
#define STR_DBG_SWITCH_GET()  0
#else
#define STR_DBG_SWITCH_SET(sw)  strDbgSwitch = sw
#define STR_DBG_SWITCH_GET()  strDbgSwitch
#endif

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/
/*
 * str.c
 */
extern UINT32 strDbgSwitch;  /* DON'T ACCESS THIS VARIABLE DIRECTLY, USE MACROS!! */

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/

#endif  /* __STR_DEF_H__ */

