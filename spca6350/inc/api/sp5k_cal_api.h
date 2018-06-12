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
#ifndef __SP5K_CAL_API_H__
#define __SP5K_CAL_API_H__

#include "common.h"
#include "middleware/aastat_def.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
/* Error code */
#define SP5K_ERR_SUCCESS                        0x00000000
#define SP5K_ERR_CALIBRATION_CFG                0x00000001

enum sp5kCalibrationSelector {
	SP5K_CALIBRATION_VIEW_BP 					= 0x71,
	SP5K_CALIBRATION_VIEW_VERT_LINE				= 0x72,
	SP5K_CALIBRAITON_VIEW_VERT_LINE				= SP5K_CALIBRATION_VIEW_VERT_LINE, /* compatible previous typo */
	SP5K_CALIBRATION_CAP_BP 					= 0x73,
	SP5K_CALIBRATION_CAP_LSC					= 0x74,
	SP5K_CALIBRATION_VIEW_LSC					= 0x75,
	SP5K_CALIBRATION_LSC 						= SP5K_CALIBRATION_CAP_LSC, /* compatible with previous name */
	SP5K_CALIBRATION_LSC_PREVIEW                = SP5K_CALIBRATION_VIEW_LSC, /* compatible with previous name */
	SP5K_CALIBRATION_CAP_VERT_LINE				= 0x76,
	SP5K_CALIBRATION_SAVE_STILL_RAW 			= 0x81,
	SP5K_CALIBRATION_SAVE_PREVIEW_RAW			= 0x82,
	SP5K_CALIBRATION_STILL_INFO 				= 0x90, /* obsolete */
	SP5K_CALIBRATION_CALLBACK 				    = 0x91,
	SP5K_CALIBRATION_PURE_RAW	  	    	    = 0x92,

	SP5K_CALIBRATION_OFF 						= 0xFF
};

enum sp5kCalibCallbackSelector {
	SP5K_CALIB_CB_STILL_RAW						= 1,
	SP5K_CALIB_CB_STILL_RAW_SAVE				= 2,
	SP5K_CALIB_CB_PREVIEW_RAW					= 3,
	SP5K_CALIB_CB_PREVIEW_RAW_SAVE				= 4,
};

enum sp5kCalibBpCfgSelector {
	SP5K_CALIB_BP_MAX							= 1,
	SP5K_CALIB_BP_FILENAME,
	SP5K_CALIB_BP_THR,
	SP5K_CALIB_BP_PVVL_MAX = 0x20,
	SP5K_CALIB_BP_PVVL_R_THR,
	SP5K_CALIB_BP_PVVL_B_THR,
	SP5K_CALIB_BP_PVVL_LOG_BUF,	/* UINT16[66]: lines, coords[64] */
	SP5K_CALIB_BP_PVVL_LOAD_RAW,
	SP5K_CALIB_BP_CPVL_LOAD_RAW=SP5K_CALIB_BP_PVVL_LOAD_RAW,
	SP5K_CALIB_BP_PVVL_SAVE_RAW,
	SP5K_CALIB_BP_CPVL_SAVE_RAW=SP5K_CALIB_BP_PVVL_SAVE_RAW,
	SP5K_CALIB_BP_PVVL_BIN,
	SP5K_CALIB_BP_CPVL_BIN=SP5K_CALIB_BP_PVVL_BIN,
	SP5K_CALIB_BP_CPVL_BP_TYPE,
};

enum sp5kCalibLscCfgSelector {
	SP5K_CALIB_LSC_LUMA_LEVEL					= 1,
	SP5K_CALIB_LSC_CENTER_ENABLE,
	SP5K_CALIB_LSC_VFLIP_ENABLE,
	SP5K_CALIB_LSC_HFLIP_ENABLE,
	SP5K_CALIB_LSC_OUTPUT_NAME,
	SP5K_CALIB_LSC_LOAD_RAW,
	SP5K_CALIB_LSC_SAVE_RAW,
	SP5K_CALIB_LSC_LOG_BUF,	/** \note UINT16 [0]=H center, [1]=V center, [2]=H step, [3]=V step, [4]=H num, [5]=V num, [6]=total num, [7]=warning val, [8]=rsv, [9]=rsv */
	SP5K_CALIB_LSC_GR_AVG_HIGH,
	SP5K_CALIB_LSC_GR_AVG_LOW,
	SP5K_CALIB_LSC_R_LEVEL,
	SP5K_CALIB_LSC_G_LEVEL,
	SP5K_CALIB_LSC_B_LEVEL,
	SP5K_CALIB_LSC_BIT_RESOLUTION,
	SP5K_CALIB_LSC_TBL_HSTEP,
	SP5K_CALIB_LSC_TBL_VSTEP,
	SP5K_CALIB_LSC_IS_4_CH,
};

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define MALLOC( x )     ros_malloc_uc( x )
#define FREE( x )       ros_free( x )

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
/* -- ICAT options -- */
typedef struct snapInf_s {
	UINT32 rawwfmt; 		/* Raw data format (10/12/14/16bits) */
	UINT32 rawwidth; 		/* Raw image width */
	UINT32 rawheight; 		/* Raw image height */
	UINT32 rawaddr; 		/* Raw image starting address */
	UINT32 yuvwidth; 		/* Yuv image width */
	UINT32 yuvheight; 		/* Yuv image height */
	UINT32 yuvaddr;			/* Yuv image starting address */
	UINT32 baytype;			/* Bayer pattern type (0/1/2/3) */
} snapInf_t;

/* -- Still info -- */
typedef struct {
    UINT32  *paewin_y;  		/* 16x16 Byte */
    UINT32  *paewin_r;  		/* 16x16 Byte */
    UINT32  *paewin_g;  		/* 16x16 Byte */
    UINT32  *paewin_b;  		/* 16x16 Byte */
    cdspAWBWinResult_t *prgbsum;
} sp5kCalResult_t;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
UINT8 *sp5kCalSnapStillRawDramOnly( void );

UINT32 sp5kCalibrationCfgSet(UINT32 selector, UINT32 value);
UINT32 sp5kCalibBpCfgSet(UINT32 selector, UINT32 value);
UINT32 sp5kCalibLscCfgSet(UINT32 selector, UINT32 value);
UINT32 sp5kImgCalibrationResultGet(sp5kCalResult_t *presult);
UINT32 sp5kCaliPreviewRawCallbackSet(void *func);

#endif /* _SP5K_CAL_API_H_ */


