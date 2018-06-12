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
#ifndef _APP_TVLCD_H_
#define _APP_TVLCD_H_

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
#include "gpio_custom.h"
#include "app_user_setting.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
typedef enum {
	APP_DISP_TV_NTSC = TV_NTSC,
	APP_DISP_TV_PAL = TV_PAL,
	APP_DISP_LCD,
 	APP_DISP_HDMI ,
 	APP_DISP_NONE
} appDispMode_e;

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/
#define IS_LCD_OUT  (appCurrDispModeGet() == APP_DISP_LCD)? 1: 0
#define DUAL_DISP_ENABLE   0
/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
void appTvLcdInit(void);
void appTvLcdStart(void);
void appTvLcdStop(void);
void appLcdTurnOn(void);
void appLcdTurnOff(void);
void appLcdTvSwitch(appDispMode_e dispDev, UINT32 isWaitSwitchRdy);
void appDispGfxInit(void);
appDispMode_e appCurrDispModeGet( void );
void appHDMIColorMatrix(void);
void appWaitOSDInitReady(void);

#endif  /* _APP_TVLCD_H_ */


