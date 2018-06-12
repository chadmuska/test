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
#define HOST_DBG 0
#include "app_com_def.h"

#include "sp5k_global_api.h"
#include "sp5k_rsvblk_api.h"
#include "sp5k_aud_api.h"
#include "sp5k_gfx_api.h"
#include "sp5k_fs_api.h"
#include "sp5k_os_api.h"
#include "sp5k_utility_api.h"
#include "sp5k_modesw_api.h"

#include "app_video.h"
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
static BOOL appModeWaitFlag = FALSE;

extern UINT8 gVideoSizeTag ;

void
appModeWaitCfg(
	BOOL wait
)
{
	appModeWaitFlag = wait;
}

void
appModeSet(
	UINT32 mode
)
{
	#if (SP5K_SENSOR_OV2715 || SP5K_SENSOR_AR0330CS || SP5K_SENSOR_AR0330CS_PLCC || SP5K_SENSOR_AR0330 || SP5K_SENSOR_AR0230 || SP5K_SENSOR_OV4689 \
		|| SP5K_SENSOR_AR0330_SPCA7002 || SP5K_SENSOR_OV4689_SPCA7002 || SP5K_SENSOR_SONYIMX078 || SP5K_SENSOR_SONYIMX206 || SP5K_SENSOR_DUAL_SONYIMX206)
	sp5kModeCfgSet(SP5K_MODE_CFG_PV_DISP_FIT, 1);
	#endif

	sp5kModeSet(mode);
	if (appModeWaitFlag)
	{
		sp5kModeWait(mode);
	}
}
#if SP5K_SENSOR_DUAL_MODE
UINT8
appDualModeCfgSet(
	UINT8 cfg
)
{
	static UINT8 PreDualModeCfg = 0xff;
	if (PreDualModeCfg != cfg) {
		static UINT8 DualModeCfg[4] = { SP5K_MODE_PV_SENSOR_DUAL_PRI_SENSOR,
                                    SP5K_MODE_PV_SENSOR_DUAL_SEC_SENSOR,
                                    SP5K_MODE_PV_SENSOR_DUAL_PRI_SENSOR,
                                    SP5K_MODE_PV_SENSOR_DUAL_SEC_SENSOR};
		HOST_PROF_LOG_PRINT(LEVEL_INFO, "modeset: dualmode cfg %d", cfg);
		PreDualModeCfg = cfg;
		sp5kModeCfgSet(SP5K_MODE_CFG_PV_SENSOR_OPT, DualModeCfg[cfg]);
	}
	return PreDualModeCfg;
}
#endif
