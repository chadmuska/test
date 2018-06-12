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
#ifndef _APP_BATTERY_H_
#define _APP_BATTERY_H_

#include "app_user_setting.h"

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 typedef enum {
	APP_BATT_CHK_SYS_BOOT  = 0,
	APP_BATT_CHK_LENS_BOOT,
	APP_BATT_CHK_LENS_MOVE,
	APP_BATT_CHK_AF_MOVE,
	APP_BATT_CHK_STROBE_CHARGE,
	APP_BATT_CHK_OPTION_TOTAL
} appBatteryCheckOption_e;

typedef enum {
	APP_BATT_LEVEL_0  = 0,  /* battery empty */
	APP_BATT_LEVEL_1,         /* 1/3  */
	APP_BATT_LEVEL_2,         /* 2/3 */
	APP_BATT_LEVEL_3,        /* battery full */
	APP_BATT_LEVEL_TOTAL
} appBatteryLevel_e;

typedef enum {
	BATTERY_STATUS_NON_FULL = 0,
	BATTERY_STATUS_FULL
} appBatteryStatus_e;

typedef enum {
	BATTERY_DCSRC_PC = 0x0000,
	BATTERY_DCSRC_ADAPTER
} appDCSRC_e;


#define MAX_BATTERY_LEVEL_IN_USED   APP_BATT_LEVEL_2

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct appBattery_s {
	UINT16 sysBoot; 	/* min voltage for boot */
	UINT16 lensBoot;	/* min voltage for power on lens init  */
	UINT16 lensMove;/* min voltage for normal len move */
	UINT16 shutDown;/* cut of voltage for shut down */
	UINT16 level[APP_BATT_LEVEL_TOTAL];	/* voltage of each battery level */
	UINT16 sampleNum; /* amount of sample point */
	UINT16 reserved;
} appBattery_t;

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
void appBatteryCheckInit(void);
void appBatteryCheckEnableSet(BOOL enable);
UINT32 appBatteryLevelGet(void);
UINT32 appLowBatteryCheck( appBatteryCheckOption_e option);
UINT32 appBatteryTypeChange(battType_e battType);
UINT32 appLensBootBatteryLevelGet( void);
UINT32 appBatteryAdcValueGet(void);
/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/

#endif  /* APP_BATTERY_H*/
