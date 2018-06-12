/**************************************************************************
 *
 *       Copyright (c) 2005-2016 by iCatch Technology, Inc.
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
 *  Author: Anthony Yu                                                    *
 *                                                                        *
 **************************************************************************/
#ifndef _SP5K_AWB_API_H_
#define _SP5K_AWB_API_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include "middleware/aastat_def.h"
#include "middleware/aaa_def.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
typedef enum {
	SP5K_AWB_CONVERGE_RANGE = AWB_CONVERGE_RANGE,
	SP5K_AWB_CONVERGE_SPEED = AWB_CONVERGE_SPEED,
	SP5K_AWB_ADAPT_DEGREE = AWB_ADAPT_DEGREE,		/* obsolete */
	SP5K_AWB_PURE_COLOR_AREA = AWB_PURE_COLOR_AREA,

	SP5K_AWB_CUSTOM_RGAIN_SHIFT=AWB_CUSTOM_RGAIN_SHIFT,	/*4*/
	SP5K_AWB_CUSTOM_BGAIN_SHIFT=AWB_CUSTOM_BGAIN_SHIFT,

	SP5K_AWB_AE_DARK_LUMA=AWB_DARK_LUMA,			/*6*/
	SP5K_AWB_AE_BRIGHT_AELV=AWB_BRIGHT_AELV,
	SP5K_AWB_AE_BRIGHT_CONTRAST=AWB_BRIGHT_CONTRAST,

	SP5K_AWB_ALGORITHM = AWB_ALGORITHM,

	SP5K_AWB_CALIBRATION_RGAIN=AWB_CALIBRATION_RGAIN,	/*90*/
	SP5K_AWB_CALIBRATION_BGAIN=AWB_CALIBRATION_BGAIN,
	SP5K_AWB_CALIBRATION_PERCENTAGE=AWB_CALIBRATION_PERCENTAGE,
	SP5K_AWB_CALIBRATION_FLASH_RGAIN=AWB_CALIBRATION_FLASH_RGAIN,
	SP5K_AWB_CALIBRATION_FLASH_BGAIN=AWB_CALIBRATION_FLASH_BGAIN,

	SP5K_AWB_MODE_SET=AWB_MODE_SET,			/*100*/
	SP5K_AWB_PROCESS_MODE=AWB_MODE_PROCESS,
	SP5K_AWB_ARITHMETIC_MODE=AWB_MODE_ARITHMETIC,
	SP5K_AWB_FLOW_MODE=AWB_MODE_FLOW,

	SP5K_AWB_PARAM_RAW_BITNUM = AWB_PARAM_RAW_BITNUM,/*200*/

	SP5K_AWB_AVG_RGAIN=AWB_AVG_RGAIN,	/*210*/
	SP5K_AWB_AVG_BGAIN=AWB_AVG_BGAIN,

	SP5K_AWB_SET_RGAIN=AWB_SET_RGAIN,	/*210*/
	SP5K_AWB_SET_BGAIN=AWB_SET_BGAIN,

	SP5K_AWB_ALightMode = AWB_ALightMode,
	SP5K_AWB_JumpStep = AWB_JumpStep,
} sp5kAwbParamSel_t;

#define SP5K_AWB_PARAM_GET AWB_PARAM_GET
#define sp5kAwbParamGet(selector) sp5kAwbParamSet(selector,SP5K_AWB_PARAM_GET)

typedef enum {
	/* auto */
	SP5K_AWB_FLOW_AUTO=awbMode_5spot,

	/* standard */
	SP5K_AWB_FLOW_A_EXT=awbMode_A_ext,
	SP5K_AWB_FLOW_A=awbMode_A,
	SP5K_AWB_FLOW_CW=awbMode_CW,
	SP5K_AWB_FLOW_D=awbMode_D,
	SP5K_AWB_FLOW_D_EXT=awbMode_D_ext,

	/* indoor */
	SP5K_AWB_FLOW_INDOOR=awbmode_indoor,
	SP5K_AWB_FLOW_TUNGSTEN=awbMode_tungsten,
	SP5K_AWB_FLOW_FLUORESCENT_H=awbMode_fluorescent_H,
	SP5K_AWB_FLOW_FLUORESCENT_L=awbMode_fluorescent_L,

	/* outdoor */
	SP5K_AWB_FLOW_OUTDOOR=awbMode_outdoor,
	SP5K_AWB_FLOW_SUNNY=awbMode_sunny,
	SP5K_AWB_FLOW_CLOUDY=awbMode_cloudy,
	SP5K_AWB_FLOW_SUNSET=awbMode_sunset,
	SP5K_AWB_FLOW_SHADE=awbMode_shade,
	SP5K_AWB_FLOW_GRASSLAND=awbMode_grassland,

	SP5K_AWB_FLOW_FLASH=awbMode_flash,
	SP5K_AWB_FLOW_CALIB=awbMode_calib,

	SP5K_AWB_FLOW_CUSTOM=awbMode_custom,
	SP5K_AWB_FLOW_MSPOT=awbMode_mspot,
	SP5K_AWB_FLOW_UNDERWATER=awbMode_custom1,
	SP5K_AWB_FLOW_CUSTOM2=awbMode_custom2,
	SP5K_AWB_FLOW_CUSTOM3=awbMode_custom3,
	SP5K_AWB_FLOW_CUSTOM4=awbMode_custom4,
}sp5kAwbFlowModeVal_t;

typedef enum{
	SP5K_AWB_PROC_PREVIEW=awbMode_preview,
	SP5K_AWB_PROC_SNAP=awbMode_snap,
	SP5K_AWB_PROC_AVI=awbMode_avi,
}sp5kAwbProcModeVal_t;

typedef enum{
	SP5K_AWB_ARITH_MUTIL_SPOT_WP_AVG=awbMode_mutilSpotsWpAvg,
	SP5K_AWB_ARITH_ALL_SPOTS_AVG=awbMode_allSpotsAvg,
	SP5K_AWB_ARITH_ALL_POINTS_AVG=awbMode_allPointsAvg,
}sp5kAwbArithmeticModeVal_t;

typedef enum {
	SP5K_AWB_ACCUM_PERIOD,
	SP5K_AWB_INVALID_MSG_SEND,
	SP5K_AWB_WIN_PATH,
	SP5K_AWB_USE_BAYER_PATTERN,
	SP5K_AWB_CFG_TOTAL,
} sp5kAwbCfgSel_t;

typedef enum {
	SP5K_AWB_SPEED_SLOW = 0x00000000,
	SP5K_AWB_SPEED_NORMAL,
	SP5K_AWB_SPEED_FAST,
} sp5kAwbSpeed_t;

typedef enum {
	SP5K_AWB_MODE_OFF = 0x00000000,
	SP5K_AWB_MODE_AUTO_SET,
	SP5K_AWB_MODE_INFO_ONLY,
} sp5kAwbMode_t;

typedef enum {
	SP5K_AWB_WIN_PATH_AFTER_CA = 0,
	SP5K_AWB_WIN_PATH_PREPROCESS,
	SP5K_AWB_WIN_PATH_2ND_PATH,
} sp5kAwbWinPath_t;

typedef enum{
	SP5K_AWB_ALGORITHM_V1 = AWB_ALGORITHM_V1,
	SP5K_AWB_ALGORITHM_V2 = AWB_ALGORITHM_V2,
}sp5kAwbAlgorithm_t;

typedef struct {
	UINT16 hTotal, vTotal;
	UINT16 hOffset, vOffset;
	UINT16 hSize, vSize;
} sp5kAwbWinCfg_t;

#define sp5kAwbDefaultFilter awbDefaultFilter/*(UINT32 R,UINT32 G,UINT32 B);*/
#define sp5kAwbDefaultReset awbDefaultReset/*();*/
#define sp5kAwbDefaultDecider awbDefaultDecider/*();*/
#define sp5kAwbDefaultDeciderPost awbDefaultDeciderPost/*();*/
#define sp5kAwbDefaultInfo awbDefaultInfo/*(UINT8* pt, UINT32 w,UINT32 h,UINT32 size);*/

UINT32 sp5kAwbResourceSet(UINT32 awbTblIdx);

UINT32 sp5kAwbCTTblSet(const UINT16 *pctTbl);
UINT32 sp5kAwbCTTblGet(UINT16 *pctTbl);
UINT32 sp5kAwbModeSet(UINT32 mode);
UINT32 sp5kAwbParamSet(UINT32 selector,UINT32 value);
UINT32 sp5kAwbCfgSet(UINT32 selector,UINT32 value);

typedef awbResult_t sp5kAwbResult_t;
SINT32 sp5kAwbPreviewResultGet(UINT32 param, sp5kAwbResult_t *presult);

typedef fpaaaProc_t fpsp5kAwbCallback_t;

#define SP5K_AEAWB_INFO_BIT_AEWIN_Y   AASTAT_INFO_FLAG_AEWIN_Y
#define SP5K_AEAWB_INFO_BIT_AEWIN_RGB AASTAT_INFO_FLAG_AEWIN_RGB
#define SP5K_AEAWB_INFO_BIT_AEWIN_R   AASTAT_INFO_FLAG_AEWIN_R
#define SP5K_AEAWB_INFO_BIT_AEWIN_G   AASTAT_INFO_FLAG_AEWIN_G
#define SP5K_AEAWB_INFO_BIT_AEWIN_B   AASTAT_INFO_FLAG_AEWIN_B
#define SP5K_AEAWB_INFO_BIT_AWBWIN    AASTAT_INFO_FLAG_AWBWIN
#define SP5K_AEAWB_INFO_BIT_HIS_Y     AASTAT_INFO_FLAG_HIS_Y
#define SP5K_AEAWB_INFO_BIT_AWB_RGB   AASTAT_INFO_FLAG_AWB_RGB
#define SP5K_AEAWB_INFO_BIT_HIS_R     AASTAT_INFO_FLAG_HIS_R
#define SP5K_AEAWB_INFO_BIT_HIS_G     AASTAT_INFO_FLAG_HIS_G
#define SP5K_AEAWB_INFO_BIT_HIS_B     AASTAT_INFO_FLAG_HIS_B

UINT32 sp5kAwbCustomCbSet(UINT32 infoflag, fpsp5kAwbCallback_t pfunc);
UINT32 sp5kAwbCustomAwbwinSizeSet(UINT32 hgrid,UINT32 vgrid);
UINT32 sp5kAwbWinCfgGet(sp5kAwbWinCfg_t *awbWinCfg);
UINT32 sp5kAwbCustomWinRoiSet(UINT32 hstart, UINT32 vstart, UINT32 hsize, UINT32 vsize);
UINT32 sp5kAwbCustomMaxWinSizeSet(UINT32 hcnt,UINT32 vcnt);

#ifdef __cplusplus
}
#endif

#endif /* _SP5K_AWB_API_H_ */
