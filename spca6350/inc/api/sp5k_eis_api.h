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
 * @file sp5k_eis_api.h
 *
 * @author LinJie Cheng
 */
#ifndef _SP5K_EIS_API_H_
#define _SP5K_EIS_API_H_

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "middleware/eis_def.h"



/**************************************************************************
 *                          C O N S T A N T S
 **************************************************************************/
/*
eis en
*/
typedef enum sp5kEisEn_e {
	SP5K_EIS_EN_ENABLE = EIS_EN_ENABLE,
	SP5K_EIS_EN_DISABLE = EIS_EN_DISABLE,
} sp5kEisEn_t;

/*
eis paramter setting
*/
typedef enum sp5kEisCfg_e {
	SP5K_EIS_CFG_ENABLE = EIS_CFG_ENABLE,     /* 0: disable eis, 1: enable eis */
	SP5K_EIS_CFG_DST_WIDTH = EIS_CFG_DST_WIDTH,	/* eis destination width size */
	SP5K_EIS_CFG_DST_HEIGHT = EIS_CFG_DST_HEIGHT,	/* eis destination height size */
	SP5K_EIS_CFG_DEGREE = EIS_CFG_DEGREE,	/* eis rotation degree */
	SP5K_EIS_CFG_NORMALIZED_COORD  = EIS_CFG_NORMALIZED_COORD,	/* eis coordinate transform config; 0: disable eis, 1: enable */
	SP5K_EIS_CFG_CALC_RANGE_PERCENTAGE = EIS_CFG_CALC_RANGE_PERCENTAGE,
	SP5K_EIS_CFG_X_COMP_RANGE_MAX = EIS_CFG_X_COMP_RANGE_MAX,
	SP5K_EIS_CFG_Y_COMP_RANGE_MAX = EIS_CFG_Y_COMP_RANGE_MAX,
	SP5K_EIS_CFG_MAX = EIS_CFG_MAX, 
} sp5kEisCfg_t;

/*
eis cb setting
*/
typedef enum sp5kEisModuleCbSvcId_e {
	SP5K_EIS_CB_SVC_ID_AFFINE_SET = EIS_CB_SVC_ID_AFFINE_SET,
	SP5K_EIS_CB_SVC_ID_CDSP_SOF_CB = EIS_CB_SVC_ID_CDSP_SOF_CB,
	SP5K_EIS_CB_SVC_ID_CDSP_EOF_CB = EIS_CB_SVC_ID_CDSP_EOF_CB,
	SP5K_EIS_CB_SVC_ID_TOTAL = EIS_CB_SVC_ID_TOTAL,
} sp5kEisModuleCbSvcId_t;

typedef UINT32 (*fp_sp5kEisCallback_t)(UINT32, UINT32, void *);


/* rectangle data */
typedef eis_AffineRect sp5kEis_AffineRect;


/* 
data structure for eis affine flow control
*/
typedef eisAffineCtrl_t sp5kEisAffineCtrl_t;



/**************************************************************************
 *                          D A T A   T Y P E S
 **************************************************************************/



/**************************************************************************
 *                              M A C R O S
 **************************************************************************/

/**************************************************************************
 *              F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
 
UINT32 sp5kEisCfgSet (sp5kEisCfg_t id,UINT32 value);
UINT32 sp5kEisCfgGet (sp5kEisCfg_t id,UINT32 *value);

UINT32 sp5kEisCbSet (sp5kEisModuleCbSvcId_t id,fp_sp5kEisCallback_t pfunc);



#endif  /* _SP5K_EIS_API_H_ */

