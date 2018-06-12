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

#include "common.h"
#include <stdio.h>
#include <string.h>
#include "api/sp5k_cdsp_api.h"
#include "app_dbg_api.h"
#include "app_aaa.h"
#include "app_awbalg_api.h"
#include "app_aealg_api.h"
#if SP5K_DIQ_FRMWRK
#include "api/sp5k_dq_api.h"
#endif
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

void appAwbAdjustCB(UINT8 sensorsel, UINT32 mode5k, UINT16 * rs, UINT16 * grs, UINT16 * bs, UINT16 * gbs )
{
#if SP5K_DIQ_FRMWRK
	UINT32 ClrTemp;
	appAWBALG_WBParamGet (AWB_PARA_CLRTMP, &ClrTemp );
	sp5kDynIqCfgSet(SP5K_DYN_IQ_CFG_AWB_CT_SET,ClrTemp,1);
#endif
}

void appAwbLVGet(UINT32* pLVx1000)
{
    AEStatus_t AEStatus;

    appAEALG_ParamGet(AE_PARA_STATUS,(SINT32*)&AEStatus);

    *pLVx1000 = (UINT32)AEStatus.LVx1000;
}

void appAwb_SensorWbGainSet(UINT32 Rgain, UINT32 Ggain, UINT32 Bgain )
{
    extern void sensorCustomHdrSenWBGainCfg(UINT32 a_udwRGain, UINT32 a_udwBGain,UINT32 a_udwGrGain, UINT32 a_udwGbGain);
    sensorCustomHdrSenWBGainCfg(Rgain,Bgain,Ggain,Ggain);
}

void appAwbInit(
	void
)
{
	appAWBALG_Init();
	appAWBALG_WBParamSet(AWB_PARA_LV_CB_REGISTER, (UINT32)appAwbLVGet);
	appAWBALG_WBParamSet ( AWB_PARA_SW_WHITE_CLIP_EN, 1 );
#if SP5K_SENSOR_AR0230
	extern void sensor_ar0230_wbgain_set(UINT32 Rgain, UINT32 Ggain, UINT32 Bgain );
	appAWBALG_WBParamSet(AWB_PARA_SENSORWBGAIN_CB_REGISTER, (UINT32)sensor_ar0230_wbgain_set);
#elif SP5K_SENSOR_OV2718  
       appAWBALG_WBParamSet(AWB_PARA_SENSORWBGAIN_CB_REGISTER, (UINT32)appAwb_SensorWbGainSet);
#endif

	appAWBALG_WBParamSet(AWB_PARA_CB_REGISTER, (UINT32)appAwbAdjustCB);
	sp5kAwbModeSet(SP5K_AWB_MODE_INFO_ONLY);

}


void
cmdAWB(
	UINT32 argc,
	UINT8 *arg[],
	UINT32 v[]
)
{
	appAWBALG_Cmd(argc,arg, v);
}

