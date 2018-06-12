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
#ifndef APP_STORBE_H
#define APP_STORBE_H

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
#include "gpio_custom.h"
#include "sp5k_sensor_api.h"
#include "sp5k_os_api.h"
#include "app_timer.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

typedef enum strobeBattChkCase_s {
	STROBE_BATT_CASE_CHARGE_CREATE,
	STROBE_BATT_CASE_CHARGE_DESTROY,
	STROBE_BATT_CASE_MAX
} strobeBattChkCase_t;

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/

BOOL appStrobeQueryReady(void);
BOOL appStrobeQueryChargeEnable(void);
BOOL appStrobeQueryPowerOn(void);
UINT32 appStrobeChargeStart(void);
void appStrobeChargeStop(void);
void appStrobePowerOn(void);
void appStrobePowerOff(void);
void appStrobeTrig(UINT32 durationUS);
#endif  /* APP_STORBE_H */

