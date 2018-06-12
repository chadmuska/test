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
#ifndef APP_PLAYBACK_UIPARAM_H
#define APP_PLAYBACK_UIPARAM_H

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/

#include "app_cdfs_api.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/*-- Playback UI param
 */
typedef struct {
#if SP5K_CDFS_OPT	
    UINT8	selCdfs;
#endif
	UINT8	selImgProtect;
	UINT8	selImgDelete;
} pbUIParam_t;

typedef enum {
	PB_UI_PROTECT_SINGLE,
	PB_UI_PROTECT_ALL,
	PB_UI_PROTECT_UNLOCK_ALL,
	PB_UI_PROTECT_TOTAL,
} pbUiProtectSel_t;

typedef enum {
	PB_UI_DELETE_SINGLE,
	PB_UI_DELETE_ALL,
	PB_UI_DELETE_TOTAL,
} pbUiDeleteSel;

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

extern pbUIParam_t gpbUiParam;

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/


#endif /* APP_PLAYBACK_UIPARAM_H */



