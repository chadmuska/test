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
 * @file eis_def.h
 *
 * @author LinJie Cheng
 */



#ifndef EIS_DEF_H
#define EIS_DEF_H

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "middleware/common_types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**************************************************************************
 *                          C O N S T A N T S
 **************************************************************************/
  

/**************************************************************************
 *                          D A T A   T Y P E S
 **************************************************************************/
/*
eis en
*/
typedef enum eisEn_e {
	EIS_EN_ENABLE = 1,
	EIS_EN_DISABLE = 0,	
} eisEn_t;

/*
eis paramter setting
*/
typedef enum eisCfg_e {
	EIS_CFG_ENABLE = 0,	 /* 0: disable eis, 1: enable eis */
	EIS_CFG_DST_WIDTH,
	EIS_CFG_DST_HEIGHT,
	EIS_CFG_DEGREE,
	EIS_CFG_CALC_RANGE_PERCENTAGE,
	EIS_CFG_NORMALIZED_COORD,
	EIS_CFG_X_COMP_RANGE_MAX,
	EIS_CFG_Y_COMP_RANGE_MAX,
	EIS_CFG_PROJECT_MODE,
	/* internal use */
	EIS_CFG_DETECT_RECT_WIDTH,
	EIS_CFG_DETECT_RECT_HIGHT,
	EIS_CFG_SRC_WIDTH,
	EIS_CFG_SRC_HEIGHT,
	/* for debug */
	EIS_CFG_SRC_SAVE,
	EIS_CFG_DST_SAVE,
	/*for resource buffer*/
	EIS_CFG_RES_BUF, /*load from sp5kResourceFileLoad(SP5K_RES_EIS_TABLE)*/
	EIS_CFG_LDC_PARAM, /*For advance EIS*/
	EIS_CFG_MAX, 
} eisCfg_t;

/*
eis cb setting
*/
typedef enum eisModuleCbSvcId_e {
	EIS_CB_SVC_ID_AFFINE_SET = 0,
	EIS_CB_SVC_ID_AFFINE_PREPROC_SET,
	EIS_CB_SVC_ID_CDSP_SOF_CB,
	EIS_CB_SVC_ID_CDSP_EOF_CB,
	EIS_CB_SVC_ID_TOTAL,
} eisModuleCbSvcId_t;

/*
eis internal cb setting
*/
typedef enum eisModuleInternalCbSvcId_e {
	EIS_INTERNAL_CB_SVC_ID_ALGO_START = 0,
	EIS_INTERNAL_CB_SVC_ID_ALGO_STOP,
	EIS_INTERNAL_CB_SVC_ID_TOTAL,
} eisModuleInternalCbSvcId_t;


/* rectangle data */
typedef struct {
    UINT16 x[4]; /* X-coordinate value of the upper-left, upper-right, lower-right, lower-left of the rectangle  */
    UINT16 y[4]; /* Y-coordinate value of the upper-left, upper-right, lower-right, lower-left of the rectangle  */
} eis_AffineRect;


/* 
data structure for eis affine flow control
*/
typedef struct eisAffineCtrl_s {
	eis_AffineRect center;
	eis_AffineRect current;
	eis_AffineRect next;
} eisAffineCtrl_t;

/* 
data structure for eis ime Prm table
need same as imeProjTbl struct
*/

typedef union eisImeProjTbl_s{
	float f;
	UINT32 k;
} eisImeProjTbl_t;

/* 
data structure for eis Prm flow control
*/
typedef struct eisPrmCtrl_s {
	eisImeProjTbl_t center[10];
	eisImeProjTbl_t current[10];
	eisImeProjTbl_t next[10];
} eisPrmCtrl_t;


/**************************************************************************
 *                              M A C R O S
 **************************************************************************/



/**************************************************************************
 *              F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/




#ifdef __cplusplus
}
#endif


#endif /* #ifndef EIS_DEF_H */
