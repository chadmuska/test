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
 /*--------------------------------------------------------------------------*
 *  File name	: 	app_power_hibernate.c
 *  Function 	: 	power hibernate	 	
 *  First editor	: 	
 *  First date  	: 	
 *  History		:
 *  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

#include "sp5k_global_api.h"
#include "sp5k_disp_api.h"
#include "app_com_def.h"
#include "app_tvlcd.h"
#include "app_sensor.h"
#include "app_osd_api.h"
#include "app_disk.h"

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
extern UINT32 appInitStateGet(void);
extern void appInitStateUnset(UINT32 state);
#if SP5K_NDK2
extern void sdhc_deinitialize(void);
#endif
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
  /*--------------------------------------------------------------------------*
 * Function name	: appPowerHibernateInit
 * Function	       	: Initial function for power hibernate
 * Return value   	: void
 * Parameter1    	: void
 * History	   :
 *                :  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

void appPowerHibernateInit(void)
{
	UINT8 cardExist = 0;
	extern void net_system_init();
	/* turn off sensor */
	appSensorPowerOff();
	sp5kModeSet(SP5K_MODE_STANDBY);
	sp5kModeWait(SP5K_MODE_STANDBY);

	/* unmount SD card */
	if(IS_CARD_EXIST){
		cardExist = TRUE;
		sp5kDiskUnMount(SP5K_DRIVE_REMOVE | SP5K_DRIVE_SD);
	}

	/* turn off display device*/
	appTvLcdStop();

	/* disable SARADC */
	sp5kAdcEnable(0);
	sp5kAdcDeInit();

	/* turn on DRAM LDO power */
	#if	defined(HW_SBC) || defined(HW_JETDVR) || defined(HW_PDVR)||defined(HW_17DVR)||defined(HW_19DVR)
	SET_DRAM_LDO_CLR(1);
	SET_DRAM_LDO_EN(1);
	sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 10);
	SET_DRAM_LDO_EN(0);
	#endif

	/* enter power save hibernate mode */
	sp5kModeCfgSet(SP5K_MODE_CFG_POWER_SAVE_MODE,SP5K_MODE_POWER_SAVE_HIBERNATE);
	sp5kModeSet(SP5K_MODE_POWER_SAVE);
	sp5kModeWait(SP5K_MODE_POWER_SAVE);

	/* exit power save hibernate mode from here by any power on source. */
	/* turn off DRAM LDO power */
	#if	defined(HW_SBC) || defined(HW_JETDVR) || defined(HW_PDVR)||defined(HW_17DVR)||defined(HW_19DVR)
	SET_DRAM_LDO_CLR(0);
	#endif

	/* enable SARADC */
	sp5kAdcInit();
	sp5kAdcEnable(1);

	/* mount card here when card is exist before hibernate. */
	if(cardExist && IS_CARD_EXIST){
		appDiskMount(SP5K_DRIVE_SD);
	}

	/* turn on display device */
	sp5kDispChnlSrcSet(SP5K_DISP_CHNL_0, SP5K_DISP_SRC_0);
	sp5kDispPowerOn(SP5K_DISP_CHNL_0, SP5K_DISP_CHNL_0_PANEL, 0, 0);
	LCD_BACKLIGHT_ON;

	/* OSD and display color palette initialization. */
	appOsdInit_LCDModeSet(OSD_DEFAULT_FRAMSIZE_W, OSD_DEFAULT_FRAMSIZE_H);
	appOsdInit();
	
	appStateChange(appPreviousStateGet(),STATE_PARAM_NORMAL_INIT);

}

  /*--------------------------------------------------------------------------*
 * Function name	: appPowerHibernateState
 * Function	       	: power hibernate state process
 * Return value   	: void
 * Parameter1    	: msg (UINT32)
 * Parameter2    	: param (UINT32)
 * History	   :
 *                :  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

void
appPowerHibernateState(
	UINT32 msg,
	UINT32 param
)
{
	switch (msg)
	{
		case APP_STATE_MSG_INIT:
			appPowerHibernateInit();
			break;
		case APP_STATE_MSG_CLOSE:
		    appStateCloseDone();
		    break;
		default:
			break;
	}
}

