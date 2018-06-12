/**************************************************************************
 *
 *       Copyright (c) 2006-2018 by iCatch Technology, Inc.
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
#ifndef _APP_COMMON_DEBUG_H_
#define _APP_COMMON_DEBUG_H_

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
#include "sp5k_gfx_api.h"
#include "sp5k_disp_api.h"
#include "app_timer.h"
#include "app_sound.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

#define COMMON_DBG_GLOBAL_CONTROL	0

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/
#define APP_COMMON_DBG_NULL_FUNC		do {} while(0)

#if (COMMON_DBG_GLOBAL_CONTROL)
#define comDbg_printf(str, arg...)	DBG_PRINT(str, ##arg)
#else
#define comDbg_printf(str, arg...)	APP_COMMON_DBG_NULL_FUNC
#endif

#define FOOT_PRINT(a)		comDbg_printf("dbgflow: ____(%d)___\n", (a))

#endif  /* _APP_COMMON_DEBUG_H_ */


