/**************************************************************************
 *
 *       Copyright (c) 2009-2013 by iCatch Technology, Inc.
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
 *  Hsin-Chu, Taiwan, R.O.C.
 *
 **************************************************************************/
/**
 * @file sp5k_eis_rsc_algo_api.h
 *
 * @author LinJie Cheng
 */
#ifndef _SP5K_EIS_RSC_ALGO_API_H_
#define _SP5K_EIS_RSC_ALGO_API_H_

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/

/**************************************************************************
 *                          C O N S T A N T S
 **************************************************************************/
/*
eis algo version
*/
typedef enum sp5kEisAlgoVersion_e {
	SP5K_EIS_ALGO_VERSION_1,
	SP5K_EIS_ALGO_VERSION_2,
	SP5K_EIS_ALGO_VERSION_MAX,
} sp5kEisAlgoVersion_t;

/*
eis algo comp type
*/
typedef enum sp5kEisAlgoCompType_e {
	SP5K_EIS_ALGO_COMP_TYPE_IME,
	SP5K_EIS_ALGO_COMP_TYPE_DIST,	
	SP5K_EIS_ALGO_COMP_TYPE_MAX,
} sp5kEisAlgoCompType_t;

/*
eis algo comp mode
*/
typedef enum sp5kEisAlgoCompMode_e {
	SP5K_EIS_ALGO_COMP_MODE_XY,
	SP5K_EIS_ALGO_COMP_MODE_XYZ,
	SP5K_EIS_ALGO_COMP_MODE_MAX,
} sp5kEisAlgoCompMode_t;

/*
eis algo smoothness mode
*/
typedef enum sp5kEisAlgoSmoothnessMode_e {
	SP5K_EIS_ALGO_SMOOTHNESS_MODE_V1,
	SP5K_EIS_ALGO_SMOOTHNESS_MODE_MAX,
} sp5kEisAlgoSmoothnessMode_t;

/*
rsc algo version
*/
typedef enum sp5kRscAlgoVersion_e {
	SP5K_RSC_ALGO_VERSION_1,
	SP5K_RSC_ALGO_VERSION_MAX,
} sp5kRscAlgoVersion_t;

/*
rsc algo comp mode
*/
typedef enum sp5kRscAlgoCompMode_e {
	SP5K_RSC_ALGO_COMP_MODE_H,
	SP5K_RSC_ALGO_COMP_MODE_V,
	SP5K_RSC_ALGO_COMP_MODE_HV,
	SP5K_RSC_ALGO_COMP_MODE_MAX,
} sp5kRscAlgoCompMode_t;

/* gyro data format , same as gyroData_t*/
typedef struct sp5kGyroData {
	int log_x;
	int log_y;	
	int log_z;
	UINT32 log_time;
	UINT32 gyroSensorIdx;
} sp5kGyroData_t;

typedef enum sp5kEisRscAlgoDebugLogFmt_e {
	SP5K_EIS_RSC_ALGO_DEBUG_GYRO_LOG_IN_TEXT = (1<<8),
} sp5kEisRscAlgoDebugLogFmt_t;

/*
eis rsc algo config setting
*/
typedef enum sp5kEisRscAlgoCfg_e {
	/* eis algo config setting */
	SP5K_EIS_ALGO_CFG_COMP_TYPE,
	SP5K_EIS_ALGO_CFG_COMP_MODE,
	SP5K_EIS_ALGO_CFG_SMOOTHNESS_MODE,
	SP5K_EIS_ALGO_CFG_ENABLE, 
	SP5K_EIS_ALGO_CFG_DEBUG_LOG,
	SP5K_EIS_ALGO_CFG_DST_WIDTH,
	SP5K_EIS_ALGO_CFG_DST_HEIGHT,
	SP5K_EIS_ALGO_CFG_COMP_RANGE_PERCENTAGE,
	SP5K_EIS_ALGO_CFG_SRC_ROI,
	SP5K_EIS_ALGO_CFG_SMOOTH_LEVEL,
	SP5K_EIS_ALGO_CFG_RAW_TO_YUV_PARAM_UPD,
	SP5K_EIS_ALGO_CFG_MAX,

	/* rsc algo config setting */
	SP5K_RSC_ALGO_CFG_COMP_MODE = SP5K_EIS_ALGO_CFG_MAX,
	SP5K_RSC_ALGO_CFG_ENABLE,
	SP5K_RSC_ALGO_CFG_DEBUG_LOG,
	SP5K_RSC_ALGO_CFG_COMP_RANGE_PERCENTAGE,
	SP5K_RSC_ALGO_CFG_MAX,

	/* gyro data parameter setting */
	SP5K_EIS_RSC_GYRO_SCAN_US = SP5K_RSC_ALGO_CFG_MAX,		/* this is gyro sample interval */
	SP5K_EIS_RSC_GYRO_SCAN_CALLBACK,		/* gyro data sampling and gyro data input and RSC compensate callback */
	SP5K_EIS_RSC_GYRO_PARAM_FL,				/* this is gyro value transform to pixel value parameters */
	SP5K_EIS_RSC_GYRO_PARAM_WGAIN,			/* this is gyro value transform to pixel value parameters */
	SP5K_EIS_RSC_GYRO_PARAM_THR,			/* used for filter the gyro value */
	SP5K_EIS_RSC_GYRO_PARAM_X_BIAS,			/* Gyro X zero value */
	SP5K_EIS_RSC_GYRO_PARAM_Y_BIAS,			/* Gyro Y zero value */
	SP5K_EIS_RSC_GYRO_PARAM_Z_BIAS,			/* Gyro Z zero value */
	SP5K_EIS_RSC_GYRO_PARAM_DELAY_SLOT,
	SP5K_EIS_RSC_GYRO_PARAM_SENSOR_TOT,
	SP5K_EIS_RSC_GYRO_PARAM_MAX,

	/* input gyro data setting */
	SP5K_EIS_RSC_GYRO_INPUT_DATA = SP5K_EIS_RSC_GYRO_PARAM_MAX,
	SP5K_EIS_RSC_GYRO_INPUT_MAX,

	/* input auto exposure/frame rate ctrl information */
	SP5K_EIS_RSC_SENSOR_INFO_EXP_TIME = SP5K_EIS_RSC_GYRO_INPUT_MAX,
	SP5K_EIS_RSC_SENSOR_INFO_FPS,
	SP5K_EIS_RSC_SENSOR_INFO_MAX,

	/* calibration */
	SP5K_EIS_RSC_CALIB_PARAM = SP5K_EIS_RSC_SENSOR_INFO_MAX,
	SP5K_EIS_RSC_CALIB_MAX,
} sp5kEisRscAlgoCfg_t;



/**************************************************************************
 *                          D A T A   T Y P E S
 **************************************************************************/
/* data structure for SP5K_EIS_RSC_GYRO_SCAN_CALLBACK cb*/
typedef UINT32 (*fp_sp5kTimerSamplingIntCb_t)(UINT32, UINT32, void *);


/**************************************************************************
 *                              M A C R O S
 **************************************************************************/

/**************************************************************************
 *              F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/

void sp5kEisAlgoInit (sp5kEisAlgoVersion_t version);
void sp5kRscAlgoInit (sp5kRscAlgoVersion_t version);

UINT32 sp5kEisRscAlgoCfgSet (sp5kEisRscAlgoCfg_t id,...);
UINT32 sp5kEisRscAlgoCfgGet (sp5kEisRscAlgoCfg_t id,...);


#endif  /* _SP5K_EIS_RSC_ALGO_API_H_ */

