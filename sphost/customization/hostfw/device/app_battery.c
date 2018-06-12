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
 *	File name    : App_battery.c
 *	Function     : About battery implementation.
 *	First editor :   
 *	First date   :   
 *	History      :
 *               : 2018.06.06  Edward.Lu  "Add description and modify code"
 **************************************************************************/
#define HOST_DBG 0
#include "app_com_def.h"
#include "app_com_api.h"
#include "gpio_custom.h"

#include "app_timer.h"
#include "app_battery.h"
#include "app_errorosd.h"
#include "app_view_osd.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

#define BATTERY_CHECK_TIME_OUT  appBackgroundTimerTickIntervalGet() //20 ms

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

static appBattery_t battSetting[BATT_TYPE_MAX] = {
  /* sysBoot-lensBoot-lensMove-shutDown-L0-  L1-  L2-  L3-  sampleNum-reserved */
  {  6000,      6000,      6000,      6000,     { 6688, 7410, 7980,  0 },      15,      0 }, /* akaline */
  {  6000,      6000,      6000,      6000,     { 6688, 7410, 7980,  0 },      15,      0 }, /* NiMH */
  {  6000,      6000,      6000,      6000,     { 6688, 7410, 7980,  0 },      15,      0 }, /* Lithium */
  {  6000,      6000,      6000,      6000,     { 6688, 7410, 7980,  0 },      15,      0 }, /* Oxyride */
};

static appBattery_t *pBattSetting = &battSetting[0];
UINT32 isBatteryCheckEnable = 1;
static UINT32 samplingCount = 0;
static UINT32 battVoltageSum = 0;
static UINT32 latestBattVoltageLevel = MAX_BATTERY_LEVEL_IN_USED;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/*--------------------------------------------------------------------------*
* Function name    : _batteryAdcValueGet
* Function         : read battery value by adc
* Return value	   : result (UINT32 SUCCESS/FAIL)
                     If the return value is not SUCCESS, *pvalue is meaningless.
* Parameter1	   : pvalue (UINT32*)
* History          :
*                  : 2018.06.06  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32
_batteryAdcValueGet(
	UINT32 *pvalue
)
{
	if(sp5kAdcRead(ADC_BATTERY_CH, pvalue)== SUCCESS){
		return SUCCESS;
	}
	else {
		printf("[ERR] sp5kAdcRead(ADC_BATTERY_CH) failed !!!\n");
		return FAIL;
	}
}

/*--------------------------------------------------------------------------*
* Function name    : appBatteryLevelPolling
* Function         : battery level polling handle (APP_BATT_LEVEL_0~APP_BATT_LEVEL_3)
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.06.06  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appBatteryLevelPolling(
	void
)
{
	UINT32 battAdcValue;
	UINT32 battVoltageAvg = 0;

	#if BATTERY_CHECK_DISABLE
	return;
	#endif

	if(IS_USB_IN)
		return;

	if(!isBatteryCheckEnable)
	{
		return;
	}

	if (_batteryAdcValueGet(&battAdcValue) != SUCCESS)
	{
		return;
	}
	samplingCount++;
	battVoltageSum = battVoltageSum + battAdcValue;

	if(samplingCount < pBattSetting->sampleNum)
	{
		return;
	}

	// calculate average voltage
	battVoltageAvg = battVoltageSum / pBattSetting->sampleNum;
	samplingCount = 0;
	battVoltageSum = 0;

	//shut down check
	if (battVoltageAvg < pBattSetting->shutDown)
	{
        HOST_PROF_LOG_PRINT(LEVEL_ERROR, "baty: ShutDown %d :%d",battVoltageAvg ,pBattSetting->shutDown);
		appViewOsd_AllClean();
		appOsdMessageBoxShow(TRUE,STRTK_OUT_OF_BATTERY);
		appTimeDelayMs(2500);
		appPowerOffUiMsgSend(APP_POWER_OFF_BATT_EMPTY);
		return;
	}

	/* (battVoltageAvg > shutDown voltage) and (battVoltageAvg < APP_BATT_LEVEL_0) */
	if (latestBattVoltageLevel == APP_BATT_LEVEL_0)
	{
        HOST_PROF_LOG_ADD(LEVEL_INFO, "baty: level 0");
        //return;
	}

	/* battVoltageAvg > (latestBattVoltageLevel) */
	if (battVoltageAvg > pBattSetting->level[latestBattVoltageLevel])
	{
	    if(latestBattVoltageLevel < MAX_BATTERY_LEVEL_IN_USED)
        {
	    latestBattVoltageLevel = latestBattVoltageLevel + 1;
		sp5kHostMsgSend(APP_UI_MSG_BATT_LEVEL_CHANGE, latestBattVoltageLevel);
        }
		return;
	}

	if (battVoltageAvg <= pBattSetting->level[latestBattVoltageLevel - 1] )
	{
	    if(latestBattVoltageLevel > APP_BATT_LEVEL_0)
        {
		latestBattVoltageLevel = latestBattVoltageLevel - 1;
		sp5kHostMsgSend(APP_UI_MSG_BATT_LEVEL_CHANGE, latestBattVoltageLevel);
        }
		return;
	}
}

/*--------------------------------------------------------------------------*
* Function name    : appBatteryLevelLoad
* Function         : load battery calibration setting
* Return value	   : result (UINT32 SUCCESS/FAIL)
* Parameter1	   : pbuf (UINT8* appBattery_t batt[BATT_TYPE_MAX])
* History          :
*                  : 2018.06.06  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32
appBatteryLevelLoad(
	UINT8 *pbuf
)
{
	if(pbuf == NULL)
	{
        HOST_PROF_LOG_PRINT(LEVEL_ERROR, "baty: load battery setting file error %s(%d)"\
                            ,__FUNCTION__,__LINE__);
		return FAIL;
	}
	if(ReadFile(RES_CALIB_BATTERY_SET, (UINT8*)(pbuf), BATT_TYPE_MAX*sizeof(appBattery_t))!= SUCCESS)
	{
        HOST_PROF_LOG_PRINT(LEVEL_ERROR, "baty: read file[%s] fail"\
                            ,RES_CALIB_BATTERY_SET);
		/* set default */
		return FAIL;
	}
	return SUCCESS;
}

/*--------------------------------------------------------------------------*
* Function name    : appBatteryLevelSave
* Function         : save battery calibration setting
* Return value	   : result (UINT32 SUCCESS/FAIL)
* Parameter1	   : pbuf (UINT8* appBattery_t battSetting[BATT_TYPE_MAX])
* History          :
*                  : 2018.06.06  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32
appBatteryLevelSave(
	UINT8 *pbuf
)
{
	if(pbuf == NULL)
	{
        HOST_PROF_LOG_PRINT(LEVEL_ERROR, "baty: save battery setting file error %s(%d)"\
                            ,__FUNCTION__,__LINE__);
		return FAIL;
	}
	if(WriteFile(RES_CALIB_BATTERY_SET, (UINT8*)(pbuf), BATT_TYPE_MAX*sizeof(appBattery_t))!=SUCCESS)
	{
        HOST_PROF_LOG_PRINT(LEVEL_ERROR, "baty: write file[%s] fail"\
                            ,RES_CALIB_BATTERY_SET);
		return FAIL;
	}

	return SUCCESS;
}

/*--------------------------------------------------------------------------*
* Function name    : appBatteryCheckInit
* Function         : Do battery setting init.
                     load SUCCESS, use this setting
                     load FAIL, use default setting
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.06.06  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appBatteryCheckInit(
	void
)
{
	appBattery_t batt[BATT_TYPE_MAX];

	profLogAdd(0, "baty: Check init");

	if(appBatteryLevelLoad((UINT8*)(&batt[0])) == SUCCESS) {
		memset(battSetting,0,BATT_TYPE_MAX*sizeof(appBattery_t));
		memcpy(&battSetting[0],(UINT8*)(&batt),BATT_TYPE_MAX*sizeof(appBattery_t));
	}
	else {
		appBatteryLevelSave((UINT8*)(&battSetting[0]));
	}

	#if HOST_DBG
	UINT32 i;
	DBG_PRINT("baty: sysBoot : lensBoot : lensMove : shutDown : level0 : level1 : level2 : level3 : sampleNum \n");
	for(i=0; i<BATT_TYPE_MAX;i++ )
	{
	        printf("%d %d %d %d %d %d %d %d %d\n",
			battSetting[i].sysBoot, battSetting[i].lensBoot, battSetting[i].lensMove,
			battSetting[i].shutDown, battSetting[i].level[0], battSetting[i].level[1],
			battSetting[i].level[2], battSetting[i].level[3], battSetting[i].sampleNum);
	}
	DBG_PRINT("\n\n");
	#endif

	//load battery level setting
	if(pUiSetting->batteryType >= BATT_TYPE_MAX)
	{
		pUiSetting->batteryType = BATTERY_TYPE_DEFAULT;
	}
	pBattSetting = &battSetting[pUiSetting->batteryType];
}

/*--------------------------------------------------------------------------*
* Function name    : appBatteryCheckEnableSet
* Function         : Set battery check feature
* Return value	   : void
* Parameter1	   : enable (BOOL ENABLE/DISABLE)
* History          :
*                  : 2018.06.06  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appBatteryCheckEnableSet(
	BOOL enable
)
{
	isBatteryCheckEnable = enable;
}

/*--------------------------------------------------------------------------*
* Function name    : appBatteryLevelGet
* Function         : Get battery level.
* Return value	   : latestBattVoltageLevel
                     (UINT32 appBatteryLevel_e APP_BATT_LEVEL_0~APP_BATT_LEVEL_3)
* Parameter1	   : void
* History          :
*                  : 2018.06.06  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32
appBatteryLevelGet(
	void
)
{
	return latestBattVoltageLevel;
}

/*--------------------------------------------------------------------------*
* Function name    : appBatteryTypeChange
* Function         : Change battery type
* Return value	   : result (UINT32 SUCCESS/FAIL)
* Parameter1	   : battType (battType_e)
* History          :
*                  : 2018.06.06  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32
appBatteryTypeChange(
	battType_e battType
)
{
	UINT32 flag = isBatteryCheckEnable;

	if(battType >= BATT_TYPE_MAX)
	{
		return FAIL;
	}
	isBatteryCheckEnable = FALSE;
	pBattSetting = &battSetting[battType];
	isBatteryCheckEnable = flag;
	return SUCCESS;
}

/*--------------------------------------------------------------------------*
* Function name    : appLowBatteryCheck
* Function         : If battery lower minimum limit setting by case, check it.
* Return value	   : result (UINT32 TRUE/FALSE)
                     TRUE : power off
                     FALSE: don't mind
* Parameter1	   : option (appBatteryCheckOption_e)
* History          :
*                  : 2018.06.06  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32
appLowBatteryCheck(
	 appBatteryCheckOption_e option
)
{
	UINT32 ret = FALSE;
	UINT32 battAdcValue;

	#if BATTERY_CHECK_DISABLE
	return FALSE;
	#endif
    if(IS_USB_IN)
		return FALSE;

	if (_batteryAdcValueGet(&battAdcValue) != SUCCESS)
	{
		return FALSE;
	}

	switch(option)
	{
		case APP_BATT_CHK_SYS_BOOT:
			if (battAdcValue < pBattSetting->sysBoot)
			{
				ret = TRUE;
			}
			break;
		case APP_BATT_CHK_LENS_BOOT:
			if (battAdcValue < pBattSetting->lensBoot)
			{
				ret = TRUE;
			}
			break;
		case APP_BATT_CHK_LENS_MOVE:
			if (battAdcValue < pBattSetting->lensMove)
			{
				ret = TRUE;
			}
			break;
		case APP_BATT_CHK_AF_MOVE:
			if (battAdcValue < pBattSetting->lensMove)
			{
				ret = TRUE;
			}
			break;
		case APP_BATT_CHK_STROBE_CHARGE:
			if (battAdcValue < pBattSetting->lensMove)
			{
				ret = TRUE;
			}
			break;
		default:
            HOST_PROF_LOG_PRINT(LEVEL_WARN, "baty: %s (%d)",__FUNCTION__, __LINE__);
			break;
	}
	if (ret)
	{
        HOST_PROF_LOG_PRINT(LEVEL_WARN, "baty: appLowBatteryCheck(%d) = %d",option,battAdcValue);
		appPowerOffUiMsgSend(APP_POWER_OFF_BATT_EMPTY);
	}
	return ret;
}

/*--------------------------------------------------------------------------*
* Function name    : appLensBootBatteryLevelGet
* Function         : Get battery level which lens boot.
* Return value	   : pBattSetting->lensBoot (UINT32)
* Parameter1	   : void
* History          :
*                  : 2018.06.06  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32
appLensBootBatteryLevelGet(
	 void
)
{
	return pBattSetting->lensBoot;
}

/*--------------------------------------------------------------------------*
* Function name    : appBatteryAdcValueGet
* Function         : Get battery adc value
* Return value	   : value, battery adc value (UINT32 0=Fail)
* Parameter1	   : void
* History          :
*                  : 2018.06.06  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32
appBatteryAdcValueGet(
	void
)
{
	UINT32 value;

	if(_batteryAdcValueGet(&value) != SUCCESS)
	{
		return 0x00;
	}
	return value;
}

