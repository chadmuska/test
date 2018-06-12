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
#define HOST_DBG 0
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include "common.h"
#include "app_com_def.h"
#include "app_com_api.h"
#include "app_battery_ctrl.h"
#include "gpio_custom.h"
#include "app_battery.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/**************************************************************************
 *  Function Name: appBatteryCharge_Start                   *
 **************************************************************************/
void appBatteryCharge_Start(void)
{
    sp5kGpioWrite(SP5K_GPIO_GRP_LMI, GPIO_MASK(CFG_BATCHARGER_ENn),(0x00000000L << (CFG_BATCHARGER_ENn)));
    DBG_PRINT("batychg: %s\n",__FUNCTION__);
}

/**************************************************************************
 *  Function Name: appBatteryCharge_Stop                   *
 **************************************************************************/
void appBatteryCharge_Stop(void)
{
    sp5kGpioWrite(SP5K_GPIO_GRP_LMI, GPIO_MASK(CFG_BATCHARGER_ENn),(0x00000001L << (CFG_BATCHARGER_ENn)));
    DBG_PRINT("batychg: %s\n",__FUNCTION__);
}

/**************************************************************************
 *  Function Name: appBatteryCharge_CheckStatus                   *
 status: BATTERY_STATUS_NON_FULL/BATTERY_STATUS_FULL
 **************************************************************************/
UINT8 appBatteryCharge_CheckStatus(void)
{
    UINT32 ucBatteryStatus = BATTERY_STATUS_NON_FULL;
    UINT32 BATCHARGER_CHARGING_STATE ;
    sp5kGpioRead(SP5K_GPIO_GRP_LMI,1<<CFG_BATCHARGER_CHARGINGn,&BATCHARGER_CHARGING_STATE);
    if( BATCHARGER_CHARGING_STATE != BATTERY_STATUS_NON_FULL)
        ucBatteryStatus = BATTERY_STATUS_FULL ;

    DBG_PRINT("batychg: %s\n",__FUNCTION__);
    return ucBatteryStatus;
}
