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
#define HOST_DBG 1
#include "app_com_def.h"
#include "app_com_api.h"

#include "middleware/sensor_model.h"
#include "customization/dev_init.h"

/*
	Set powersaving timeout before 5k StandbyMode.
	Auto power save after <timeout> (unit in ms).
	<timeout> = 0xffffffff: no auto power save (default);
	<timeout> = 0: immediate power save;
	<timeout> will be clear before entering power save mode.
*/
void appsensorAutoPwrSaveSet(UINT32 timeout){

	sp5kSystemCfgSet(SP5K_SENSOR_SYS_CFG,SENSOR_SYS_CFG_AUTO_PWRSAVE_TIMEOUT, timeout);
}

void 
appSensorPowerOff(
	void
)
{
    appsensorAutoPwrSaveSet(0); 
}

void 
appAfePowerCtrl(
	UINT32 fWakeup
)
{

}

