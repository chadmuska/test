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
 * @file rsc_def.h
 *
 * @author LinJie Cheng
 */


#ifndef RSC_DEF_H
#define RSC_DEF_H

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
rsc en
*/
typedef enum rscEn_e {
	RSC_EN_ENABLE = 1,
	RSC_EN_DISABLE = 0,
} rscEn_t;

/*
rsc paramter setting
*/
typedef enum rscCfg_e {
	RSC_CFG_ENABLE = 0,     /* 0: disable eis, 1: enable eis */
	RSC_CFG_X_COMP_RANGE_MAX,	/* pixel */
	RSC_CFG_Y_COMP_RANGE_MAX,	/* pixel */
	RSC_CFG_CDSP_TO_RS,	/* CDSP to RS time 10us */
	RSC_CFG_COMP_RANGE_PERCENT, /* RSC compensate percentage */
	RSC_CFG_DST_WIDTH, /* RSC destination size */
	RSC_CFG_DST_HEIGHT,	/* RSC destination size */
	RSC_CFG_MAX,
} rscCfg_t;

/*
rsc cb setting
*/
typedef enum rscModuleCbSvcId_e {
	RSC_CB_SVC_ID_FRAME_END = 0,
	RSC_CB_SVC_ID_FRAME_START,
	RSC_CB_SVC_ID_TOTAL,
} rscModuleCbSvcId_t;

/*
rsc internal cb setting
*/
typedef enum rscModuleInternalCbSvcId_e {
	RSC_INTERNAL_CB_SVC_ID_ALGO_START = 0,
	RSC_INTERNAL_CB_SVC_ID_ALGO_STOP,
	RSC_INTERNAL_CB_SVC_ID_TOTAL,
} rscModuleInternalCbSvcId_t;




/**************************************************************************
 *                              M A C R O S
 **************************************************************************/



/**************************************************************************
 *              F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/


#ifdef __cplusplus
}
#endif


#endif /* #ifndef RSC_DEF_H */
