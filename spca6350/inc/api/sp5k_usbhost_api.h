/**************************************************************************
 *
 *       Copyright (c) 2013-2013 by iCatch Technology, Inc.
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
/**
 * @file sp5k_usbhost_api.h
 * @brief
 * @author j.li
 *
 *
 */
#ifndef _SP5K_USBHOST_API_H_
#define _SP5K_USBHOST_API_H_

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "common.h"

/**************************************************************************
 *                          C O N S T A N T S
 **************************************************************************/
#define SP5K_USBHOST_CTRL_UVC_PREVIEW_START SP5K_USBHOST_CTRL_UVC_DEVICE_ATTACH
#define SP5K_USBHOST_CTRL_UVC_PREVIEW_STOP  SP5K_USBHOST_CTRL_UVC_DEVICE_DETACH

/**************************************************************************
 *                              M A C R O S
 **************************************************************************/

/**************************************************************************
 *                          D A T A   T Y P E S
 **************************************************************************/
typedef enum {
	SP5K_USBHOST_CTRL_INIT		= 1,
	SP5K_USBHOST_CTRL_STOP		= 2,
	SP5K_USBHOST_CTRL_RESTART	= 3,
	SP5K_USBHOST_CTRL_EXIT		= 4,
	SP5K_USBHOST_CTRL_UVC_DEVICE_OPEN   = 5,
	SP5K_USBHOST_CTRL_UVC_DEVICE_ATTACH = 6,
	SP5K_USBHOST_CTRL_UVC_DEVICE_DETACH = 7,
	SP5K_USBHOST_CTRL_UVC_DEVICE_CLOSE  = 8,
} sp5kUsbhostCtrl_t;

typedef enum {
	SP5K_USBHOST_CFG_PHY				= 1,
	SP5K_USBHOST_CFG_PWR				= 2,
	SP5K_USBHOST_CFG_NUM				= 3,
	SP5K_USBHOST_CFG_PHY_SEL			= 4,
	SP5K_USBHOST_PM_RUNTIME_SCHEDULE	= 5,
	SP5K_USBHOST_PM_RUNTIME_AUTOSUSPEND	= 6,
	SP5K_USBHOST_PM_HC_SCHEDULE			= 7,
} sp5kUsbhostCfg_t;

typedef enum {
	SP5K_USBHOST_PM_ACTIVE_STATE		= 0,
	SP5K_USBHOST_PM_RESUMING_STATE		= 1,	
	SP5K_USBHOST_PM_SUSPEND_STATE		= 2,
	SP5K_USBHOST_PM_SUSPENDING_STATE	= 3,
	SP5K_USBHOST_PM_ERROR_STATE			= 4
} sp5kUsbhostPMState_t;
	
typedef enum {
	SP5K_USBHOST_STR_DESC		= 1,
} sp5kUsbhostInfo_t;

/**************************************************************************
 *                  E X T E R N A L   R E F E R E N C E
 **************************************************************************/


/**************************************************************************
 *              F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
UINT32 sp5kUsbhostControl(sp5kUsbhostCtrl_t, ...);
UINT32 sp5kUsbhostCfgSet(sp5kUsbhostCfg_t, SINT32);
UINT32 sp5kUsbhostCfgGet(sp5kUsbhostCfg_t, UINT32*);
UINT32 sp5kUsbhostPMRuntimeSet(sp5kUsbhostCfg_t cfg, void* device, SINT32 delay);
UINT32 sp5kUsbhostPMRuntimeGet(void* device, sp5kUsbhostPMState_t *state);
UINT32 sp5kUsbhostVndrScsiCmd(void* cmd, UINT32 isRead, void* buf);
UINT32 sp5kUsbhostVndrScsiCbReg(SINT32 (*cb)(void));
UINT32 sp5kUsbhostSysEvtHandlerSet(void* handler, UINT32 udata);
int sp5kUsbHostInfoGet(sp5kUsbhostInfo_t, ...);
       

#ifdef __cplusplus
}
#endif

#endif /*_SP5K_USBHOST_API_H_*/
