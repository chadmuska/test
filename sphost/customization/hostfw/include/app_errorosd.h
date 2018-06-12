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

#ifndef _APP_ERROROSD_H_
#define _APP_ERROROSD_H_
#include "app_stringid.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define TK_WARNING_X                60
#define TK_WARNING_Y                65
#define TK_WARNING_W                200
#define TK_WARNING_H                110
#define TKTYPE_WARNING_VOICEMEMO_Y  70
#define TK_WARNING_FRAME2_X         40
#define TK_WARNING_FRAME2_Y         65
#define TK_WARNING_FRAME2_W         240
#define TK_WARNING_FRAME2_H         110

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                           VARIABLE                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
void  appOsdMessageBoxShow(BOOL bShow, unsigned short usStrId);
void  appErrorOsd_VoiceMemoWarnScreenDraw(BOOL bShow, unsigned short usStrId);
void  appErrorOsd_BlackWarnScreen(BOOL bShow, unsigned short usStrId);

#endif /* _APP_ERROROSD_H_ */

