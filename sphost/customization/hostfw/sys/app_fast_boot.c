 /*--------------------------------------------------------------------------*
 *  File name	: 	App_fast_boot.c
 *  Function 	: 	Fast Boot up 		 	
 *  First editor	: 	
 *  First date  	: 	
 *  History		:
 *  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

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
<<<<<<< .mine *  Hsin-Chu, Taiwan.
======= *  Hsin-Chu, Taiwan, R.O.C.
>>>>>>> .theirs *
 **************************************************************************/

#include <stdio.h>
#include "common.h"
#include "app_com_def.h"
#include "app_fast_boot.h"	/* for struct appPowerOnInfo_t */
#include "gpio_custom.h"	/* %REVIEW% for gpioInitialCfgSet() */
#include "app_dbg_api.h"	/* for LEVEL_INFO */
#include "app_user_setting.h"	/* for BATT_TYPE_MAX */
#include "app_lens_api.h"

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
appPowerOnInfo_t appPowerOnInfo = {FALSE,FALSE,FALSE,FALSE,FALSE};

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
void   halRtcRegWrite(UINT32 addr, UINT32 data);
UINT32 halRtcRegRead(UINT32 addr);

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
  /*--------------------------------------------------------------------------*
 * Function name	: appFastBootLensInfoGet
 * Function	       	: Get Lens Information
 * Return value   	: Status  (UINT32 SUCESS/FAIL)
 * Parameter1    	: Mode pointer  (--)
 * Parameter2    	: Battery Type pointer  (--)
 * Parameter3    	: Battery Level pointer  (--)
 * History	   :
 *                :  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
UINT32
appFastBootLensInfoGet(
	UINT8 *pMode,
	UINT8 *pBattType,
	UINT8 *pBattLevel
)
{
	*pMode = halRtcRegRead(0x20);
	if (*pMode!=halRtcRegRead(0x21))
	{
		return FAIL;
	}

	*pBattType = halRtcRegRead(0x22);
	if (*pBattType!=halRtcRegRead(0x23))
	{
		return FAIL;
	}

	*pBattLevel = halRtcRegRead(0x24);
	if (*pBattLevel!=halRtcRegRead(0x25))
	{
		return FAIL;
	}
	return SUCCESS;
}

  /*--------------------------------------------------------------------------*
 * Function name	: appFastBootLensModeSet
 * Function	       	: Set Lens Mode
 * Return value   	: void
 * Parameter1    	: Mode (UINT32)
 * History	   :
 *                :  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
void
appFastBootLensModeSet(
	UINT32 mode
)
{
	halRtcRegWrite(0x20, mode);
	halRtcRegWrite(0x21, mode);
}
  
  /*--------------------------------------------------------------------------*
 * Function name	: appFastBootLensBattSet
 * Function	       	: Set Lens Mode
 * Return value   	: void
 * Parameter1    	: Battery Type  (UINT32)
 * Parameter2   	: Battery Level  (UINT32)
 * History	   :
 *                :  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

void
appFastBootLensBattSet(
	UINT32 type,
	UINT32 level
)
{
	halRtcRegWrite(0x22, type);
	halRtcRegWrite(0x23, type);
	halRtcRegWrite(0x24, level);
	halRtcRegWrite(0x25, level);
}

  /*--------------------------------------------------------------------------*
 * Function name	: appPowerOnKeyGet
 * Function	       	: Get Power On Key
 * Return value   	: void
 * Parameter1    	: void
 * History	   :
 *                :  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

void
appPowerOnKeyGet(
	void
)
{
#if	defined(HW_SBC) || defined(HW_JETDVR) || defined(HW_PDVR)|| defined(HW_17DVR)||defined(HW_19DVR)
	if (IS_KEY_LEFT_PRESSED)
	{
		appPowerOnInfo.isTestModePowerOn = TRUE;
		printf(" ADC  KEY get to (CALIBRATION) mode \n");
	}
#else
    extern UINT32 gPowerKey;

    profLogPrintf(0, "[host]fboot: pwr get %x",gPowerKey);

	if (gPowerKey == SP5K_PWR_FALL_EVT_PLAYBACK)
	{
		appPowerOnInfo.isPbPowerOn = TRUE;
	}
	if ((!appPowerOnInfo.isPbPowerOn) && (IS_KEY_LEFT_PRESSED) )
	{
		appPowerOnInfo.isTestModePowerOn = TRUE;
	}
#endif
}

  /*--------------------------------------------------------------------------*
 * Function name	: appLensFastBootInit
 * Function	       	: Initial Fast Boot for Lens
 * Return value   	: void
 * Parameter1    	: void
 * History	   :
 *                :  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

void
appLensFastBootInit(
	void
)
{
    #if FAST_BOOT
	UINT32 ret;
	UINT8 mode,batType,batLevel;

	sp5kAdcInit();
	appLensInit();
	sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 20);

	profLogAdd(0, "[host]fboot: appLensFastBootInit");
	gpioInitialCfgSet();
	appPowerOnKeyGet();

	ret = appFastBootLensInfoGet(&mode, &batType, &batLevel);

	if (ret !=SUCCESS || batType>=BATT_TYPE_MAX)
	{
        HOST_PROF_LOG_PRINT(LEVEL_INFO, "fboot: fast boot err=%x (mode:%d, batType=%d, batLevel=%d)", ret, mode, batType, batLevel);
		return;
	}
    #endif  //end FAST_BOOT
}
