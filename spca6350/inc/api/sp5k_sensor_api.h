/**************************************************************************
 *
 *       Copyright (c) 2004-2016 by iCatch Technology, Inc.
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
 *                                                                        *
 *  Author: Anmin Deng                                                    *
 *                                                                        *
 **************************************************************************/
#ifndef _SP5K_SENSOR_API_H_
#define _SP5K_SENSOR_API_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include "middleware/cdsp_def.h"
#include "middleware/sensor_def.h"
#include "middleware/aastat_def.h"
#include "api/sp5k_global_api.h"
#include "api/sp5k_ae_api.h"
#include "api/sp5k_awb_api.h"

#define SP5K_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP CDSP_SENSOR_X_SIZE_INCREMENT_FOR_MODE_9_PV_BP

/*UINT32 sp5kSensorInit(UINT32 autostart);*/
typedef enum {
	SP5K_SENSOR_INIT_STANDBY = 0,
	SP5K_SENSOR_INIT_RUN = 1,
	SP5K_SENSOR_INIT_TOTAL,
} sp5kSensorInit_t;

/*UINT32 sp5kSensorDevSwitch(UINT32 devid);*/
typedef enum {
	SP5K_SENSOR_DEV_SENSOR = SENSOR_DEV_ID_SENSOR,
	SP5K_SENSOR_DEV_TV_IN = SENSOR_DEV_ID_TV_IN,
} sp5kSensorDev_t;

/*UINT32 sp5kSensorModeCfgSet(UINT32 mode,UINT32 sensormode);*/
typedef enum {
	SP5K_SENSOR_MODE_PREVIEW = SENSOR_MODE_PREVIEW,
	SP5K_SENSOR_MODE_SNAP = SENSOR_MODE_SNAP,
} sp5kSensorMode_t;

/*UINT32 sp5kFlashlightInit(UINT32 polar);*/
typedef enum {
	SP5K_FLASHLIGHT_INACTIVE_LO = 0,
	SP5K_FLASHLIGHT_INACTIVE_HI = 1,
	SP5K_FLASHLIGHT_POLAR_TOTAL,
} sp5kFlashlightPolar_t;

/*UINT32 sp5kPreflashCfgSet(UINT32 sel,UINT32 value);*/
typedef enum {
	SP5K_PREFLASH_CFG_WIDTH = 1,
	SP5K_PREFLASH_CFG_H_POS = 2,
	SP5K_PREFLASH_CFG_DELAY_TYPE = 3,
	SP5K_PREFLASH_CFG_TIMEOUT = 4,
	SP5K_PREFLASH_CFG_WIDTH_NS = 5,
	SP5K_PREFLASH_CFGSEL_TOTAL,
} sp5kPreflashCfgSel_t;

/*UINT32 sp5kSnapFlashCfgSet(UINT32 sel,UINT32 value);*/
typedef enum {
	SP5K_SNAP_FLASH_CFG_MODE = 0,
	SP5K_SNAP_FLASH_CFG_WIDTH = 1,
	SP5K_SNAP_FLASH_CFG_H_POS = 2,
	SP5k_SNAP_FLASH_CFG_DELAY_TYPE = 3,
	SP5K_SNAP_FLASH_CFG_WIDTH_NS = 4,
	SP5K_SNAP_FLASH_CFG_DRAMCLOCK = 5,
	SP5K_SNAP_FLASH_CFGSEL_TOTAL,
} sp5kSnapFlashCfgSel_t;
/*sp5kSnapFlashCfgSet(SP5K_SNAP_FLASH_CFG_MODE, value);*/
typedef enum {
	SP5K_SNAP_FLASH_MODE_OFF = 0,
	SP5K_SNAP_FLASH_MODE_FORCED_ON = 2,
	SP5K_SNAP_FLASH_MODE_FORCED_ON_REAR_SYNC = 0x82,
} sp5kSnapFlashMode_t;
/*bitwise-OR to sp5kSnapFlashMode_t*/
typedef enum {
	SP5K_SNAP_FLASH_MODE_FRONT_CURTAIN_SYNC = 0,
	SP5K_SNAP_FLASH_MODE_REAR_CURTAIN_SYNC = 0x80,
	SP5K_SNAP_FLASH_MODE_BMAP_CURTAIN_SYNC = 0x80,
} sp5kSnapFlashModeCurtainSync_t;

/*sp5kSnapFlashCfgSet(SP5k_SNAP_FLASH_CFG_DELAY_TYPE, value);*/
/*sp5kPreflashCfgSet(SP5k_PREFLASH_CFG_DELAY_TYPE, value);*/
typedef enum {
	SP5K_FLASH_DELAY_TYPE_H_POS = 0,
	SP5K_FLASH_DELAY_TYPE_VD_TIMER = 1,
	SP5K_FLASH_DELAY_TYPE_IMMEDIATE_TIMER = 2,
	SP5K_FLASH_DELAY_TYPE_NEG_VD_TIMER = 3,
	SP5K_FLASH_DELAY_TYPE_CMOS_TORCH_H_POS = 4,
	SP5K_FLASH_DELAY_TYPE_TOTAL,
} sp5kFlashDelayType_t;

/*UINT32 sp5kRedEyeFlashTrig(UINT32 sync);*/
typedef enum {
	SP5K_REDEYE_TRIG_IMMEDIATELY = 0,
	SP5K_REDEYE_TRIG_SYNC_FRAME = 1,
	SP5K_REDEYE_TRIG_SYNC_TOTAL,
} sp5kRedEyeSync_t;

/*UINT32 sp5kSnapShutterCfgSet(UINT32 sel,fpsp5kCallBack_t pfunc);*/
/*UINT32 sp5kSnapShutterDelaySet(UINT32 sel,UINT32 frame,UINT32 delay);*/
typedef enum {
	SP5K_SNAP_SHUTTER_CLOSE_EXP_END = 0,
	SP5K_SNAP_SHUTTER_OPEN_READOUT_END = 1,
	SP5K_SNAP_SHUTTER_OPEN_EXP_START = 2,
	SP5K_SNAP_SHUTTER_CLOSE_FRAME_EXP_END_EARLY = 3,
	SP5K_SNAP_SHUTTER_OPEN_FRAME_READOUT_END = 4,
	SP5K_SNAP_SHUTTER_OPEN_FRAME_EXP_START = 5,
	SP5K_SNAP_SHUTTER_CLOSE_FRAME_EXP_MIN_DELAY = 6,
	SP5K_SNAP_SHUTTER_CLOSE_OPEN_MIN_DELAY = 7,	
	SP5K_SNAP_SHUTTER_CFGSEL_TOTAL,
} sp5kSnapShutterCfgSel_t;

/*UINT32 sp5kSnapShutterAeTrimCfgSet(UINT32 enable);*/
typedef enum {
	SP5K_SNAP_SHUTTER_AE_TRIM_DISABLE = 0,
	SP5K_SNAP_SHUTTER_AE_TRIM_ENABLE = 1,
	SP5K_SNAP_SHUTTER_AE_TRIM_CFG_TOTAL,
} sp5kSnapShutterAeTrimCfg_t;

typedef enum {
	SP5K_CFA_BAYER_GRBG, 
	SP5K_CFA_BAYER_RGGB,
	SP5K_CFA_BAYER_BGGR, 
	SP5K_CFA_BAYER_GBRG,
	SP5K_CFA_BAYER_TOTAL,
} sp5kSensorCfaPattern_t;

/* UINT32  sp5kSensorCfgSet(UINT32 selector,UINT32 param1,	UINT32 param2);*/
typedef enum {
	SP5K_SENSOR_CFG_PREVIEW_TIMEOUT_ENABLE, 
	SP5K_SENSOR_CFG_PREVIEW_TIMEOUT_INTERVAL,
	SP5K_SENSOR_CFG_PREVIEW_TIMEOUT_CALLBACK,
	SP5K_SENSOR_CFG_PREVIEW_TIMEOUT_TESTPAT,
	SP5K_SENSOR_CFG_PREVIEW_TIMEOUT_PRI_ENABLE, 
	SP5K_SENSOR_CFG_PREVIEW_TIMEOUT_PRI_CALLBACK,
	SP5K_SENSOR_CFG_PREVIEW_FRAME_INTERVAL,
	SP5K_SENSOR_CFG_FRAMERATE_MAX,
	SP5K_SENSOR_CFG_FRAMERATE_MIN,
	SP5K_SENSOR_CFG_EXPBGN_CALLBACK,
	SP5K_SENSOR_CFG_EXPEND_CALLBACK,
	SP5K_SENSOR_CFG_READOUTBGN_CALLBACK,
	SP5K_SENSOR_CFG_READOUTEND_CALLBACK,
	SP5K_SENSOR_CFG_DUAL_RAW_SEL,
	SP5K_SENSOR_CFG_PREVIEW_TIMEOUT_STSCHK_CALLBACK,
	SP5K_SENSOR_CFG_PREVIEW_TIMEOUT_STSCHK_PRI_CALLBACK,
	SP5K_SENSOR_CFG_PREVIEW_FAKE_TG_EN,
	SP5K_SENSOR_CFG_DIQ_DUAL_RAW_AGC_REF,	
	SP5K_SENSOR_CFG_TOTAL,
} sp5kSensorCfg_t;

typedef sensorTgBinCor_t sp5ksensorBinCorr_t;


typedef enum {
	SP5K_SENSOR_INFO_BINCORR=0x10,
	SP5K_SENSOR_INFO_TOTAL,
} sp5kSensorInfo_t;


UINT32 sp5kSensorInit(UINT32 autostart);
UINT32 sp5kSensorStop();
UINT32 sp5kSensorDevSwitch(UINT32 devid);
UINT32 sp5kSensorModeCfgSet(UINT32 mode,UINT32 sensormode);
UINT32 sp5kSensorModeCfgGet(UINT32 mode);
UINT32 sp5kFlashlightInit(UINT32 polar);
UINT32 sp5kPreflashCfgSet(UINT32 sel,UINT32 value);
UINT32 sp5kPreflashCfgGet(UINT32 sel);
UINT32 sp5kPreflashTrig();
SINT32 sp5kPreflashResultGet(UINT32 param,
		sp5kAeResult_t *paeresult,sp5kAwbResult_t *pawbresult);
UINT32 sp5kSnapFlashCfgSet(UINT32 sel,UINT32 value);
UINT32 sp5kSnapFlashCfgGet(UINT32 sel);
UINT32 sp5kRedEyeFlashCfgSet(UINT32 numPulse,
		const UINT16 *pulseWidthArray);
UINT32 sp5kRedEyeFlashTrig(UINT32 sync);
UINT32 sp5kSnapShutterCfgSet(UINT32 sel,fpsp5kCallback_t pfunc);
UINT32 sp5kSnapShutterAeTrimCfgSet(UINT32 enable);
UINT32 sp5kSnapShutterDelaySet(UINT32 sel,UINT32 frame,UINT32 delay);
UINT32 sp5kPreviewIrisCfgSet(fpsp5kCallback_t pfunc);

UINT32 sp5kAgcModeResourceFileLoad(UINT32 mode,const char *fname);
UINT32 sp5kAgcIdxModeDiffSet(UINT32 mode,SINT32 diff);
void sp5kSensorVvalidWait(void);
void sp5kSensorLineCountGet (UINT32 sensormode, UINT32 *linecnt);
UINT32 sp5kSensorCfgSet(UINT32 selector,UINT32 param1,UINT32 param2);
UINT32 sp5kSensorCfgGet(UINT32 selector,UINT32 param1,UINT32 param2);
UINT32 sp5kSensorInfoGet(UINT32 sensorMode,	UINT32 selector,UINT32 *pval);

#ifdef __cplusplus
}
#endif

#endif
