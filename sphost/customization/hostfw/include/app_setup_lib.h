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
#ifndef APP_SETUP_LIB_H
#define APP_SETUP_LIB_H

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
#include "app_menu.h"
#include "app_rtc.h"
#include "app_osdfont.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
appRTC_t *appSetupMenu_RTC_Get(void);
void appSetupMenuFunctionSetCB(appMenuAction_e action,appMenuInfo_t *pmenuCurTab,appSubMenuInfo_t *psubMenuItem);
void appSetupMenu_RTC_Adjust(set_RTC_Item_e rtcItem, UINT32 dir);
void appSetupMenu_Sound_Adjust(soundsMode_e soundItem, UINT32 dir);
void appSetupMenu_SetCurLanguage(UINT8 nCurLanguage);
void applightFrequencySetting(lightFreq_e lightfreq);

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/

#endif  /* APP_SETUP_LIB_H*/

