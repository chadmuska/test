/**************************************************************************
 *
 *       Copyright (c) 2012-2018 by iCatch Technology, Inc.
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
#ifndef _APP_BTTERY_CTRL_H_
#define _APP_BTTERY_CTRL_H_

#include "app_battery.h"

typedef void appBatteryChargeStartCBF_t(void);
typedef void appBatteryChargeStopCBF_t(void);
typedef UINT8 appBatteryCheckStatusCBF_t(void);

void appBatteryCharge_SetStartCBFRegister(appBatteryChargeStartCBF_t *aBatteryChargeStartCBF);
void appBatteryCharge_SetStartCBF(void);
void appBatteryCharge_SetStopCBFRegister(appBatteryChargeStopCBF_t *aBatteryChargeStopCBF);
void appBatteryCharge_SetStopCBF(void);
void appBatteryCharge_SetCheckStatusCBFRegister(appBatteryCheckStatusCBF_t *aBatteryCheckStatusCBF);
UINT8 appBatteryCharge_SetCheckStatusCBF(void);
UINT8 appBatteryCharge_CBRegister(void);
appDCSRC_e appBatteryCharge_LineStateGet(void);
UINT8 appBatteryCharge_Init(void);
UINT8 appBatteryCharge_ChargerStatusGet(void);

void appBatteryCharge_Start(void);
void appBatteryCharge_Stop(void);
UINT8 appBatteryCharge_CheckStatus(void);

#endif
