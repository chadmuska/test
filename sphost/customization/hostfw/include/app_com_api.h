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
#ifndef _APP_COMMON_API_H_
#define _APP_COMMON_API_H_

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
#include "sp5k_gfx_api.h"
#include "sp5k_disp_api.h"
#include "app_timer.h"
#include "app_sound.h"
#include "app_com_debug.h"
#include "app_osd.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define AUDIO_REC_SUPPORT  1
#define AUDIO_DEC_SUPPORT  1

#define APP_DEBUG_OSD_REFRESH      0
#define APP_DEBUG_DISP_LAYER_CTRL  1

enum {
#if SP5K_DIQ_HDR
    URGENT_HDR,
#endif
    URGENT_STAMP_MAIN,
#if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
    URGENT_STAMP_SEC,
    URGENT_STAMP_DUAL,
#endif
    URGENT_PIV,
#if TEST_ADAS
    URGENT_ADAS_MAIN,
    URGENT_ADAS_SEC,
    URGENT_ADAS_MAIN_SEC,
#endif
    URGENT_TOTNUM
};

enum {
#if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
    DISP_URGENT_PIP,
#endif
    DISP_URGENT_TOTNUM
};

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
UINT32 tmrMsTimeGet(void);

void appPowerSavingEnableSet(BOOL enable);
void appPowerSaveTimeOutReset(void);

void appPowerOffUiMsgSend(UINT32 param);
UINT32 appAutoPowerOffPhaseCheck(void);
UINT32 appBackgroundTimerTickIntervalGet(void);

SINT32 appPower2Get(SINT32 numerator, UINT32 denominator, UINT32 *pResultNumerator, UINT32 *pResultDenominator);

BOOL appFileExistCheck(UINT8 *filename);

void appModeWaitCfg(BOOL wait);
void appModeSet(UINT32 mode);

#if SP5K_SENSOR_DUAL_MODE
UINT8 appDualModeCfgSet(UINT8 cfg);
#endif

BOOL IsBracketMode(void);
UINT8 appBracketIntervalGet(void);
UINT8 appBracketSnapNumGet(void);

void appUrgentSet(UINT8 appUrgentFunc, UINT8 enable);
void appDispUrgentSet(UINT8 appUrgentFunc,UINT8 enable);

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/
#define APP_SLEEP_MS(ms)   appTimeDelay(ms, FALSE)
#define IS_APO_COUNT_DOWN  appAutoPowerOffPhaseCheck()
#define appMsTimeGet()     tmrMsTimeGet()
#define appTimeDelayMs(t)  do { if (t) sp5kTimeDelay( SP5K_TIME_DELAY_1MS , t); } while(0)

#define APP_DEBUG_NULL_FUNC  do {} while(0)

#if (APP_DEBUG_OSD_REFRESH)
#define osdRefresh_printf(str, arg...)  printf(str, ##arg)
#else
#define osdRefresh_printf(str, arg...)  APP_DEBUG_NULL_FUNC
#endif

#if (APP_DEBUG_DISP_LAYER_CTRL)
#define dispLayerCtrl_printf(str, arg...)  printf(str, ##arg)
#else
#define dispLayerCtrl_printf(str, arg...)  APP_DEBUG_NULL_FUNC
#endif

/*-- OSD Refresh Control */
#define appOsdRefresh(en) do { appOsd_GfxOsdRefreshSet((UINT32)en); osdRefresh_printf("\n((( Refresh [%s] )))\n\n", ((en) ? "ON" : "OFF")); } while(0)
#define APP_OSD_REFRESH_ON   appOsdRefresh(1)
#define APP_OSD_REFRESH_OFF  appOsdRefresh(0)

/*-- DISPLAY Layer Control */
#define appDispLayer_GLOBAL_Ctrl(en)  do { sp5kDispAttrSet(sp5kDispActChnlGet(0), SP5K_DISP_GLOB_ACTIVE, (en)); dispLayerCtrl_printf("\n((( DISP_GLOB [%s] )))\n\n", ((en) ? "ON" : "OFF")); } while(0)
#define appDispLayer_IMG_Ctrl(en)     do { sp5kDispAttrSet(sp5kDispActChnlGet(0), SP5K_DISP_IMG_ACTIVE, (en)); dispLayerCtrl_printf("\n((( DISP_IMG [%s] )))\n\n", ((en) ? "ON" : "OFF")); } while(0)
#define appDispLayer_OSD_Ctrl(en)     do { sp5kDispAttrSet(sp5kDispActChnlGet(0), SP5K_DISP_OSD_ACTIVE, (en)); dispLayerCtrl_printf("\n((( DISP_OSD [%s] )))\n\n", ((en) ? "ON" : "OFF")); } while(0)
#define appDispLayer_PF_Ctrl(en)      do { sp5kGfxAttrSet(SP5K_GFX_LAYER_ACTIVE, (en), 0, 0, 0); dispLayerCtrl_printf("\n((( DISP_PF [%s] )))\n\n", ((en) ? "ON" : "OFF")); } while(0)

#define APP_IMG_ON   appDispLayer_IMG_Ctrl(1)
#define APP_IMG_OFF  appDispLayer_IMG_Ctrl(0)

#define APP_OSD_ON   appDispLayer_OSD_Ctrl(1)
#define APP_OSD_OFF  appDispLayer_OSD_Ctrl(0)

#endif  /* _APP_COMMON_API_H_ */

