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
 * @file sp5k_rsc_api.h
 *
 * @author LinJie Cheng
 */
#ifndef _SP5K_RSC_API_H_
#define _SP5K_RSC_API_H_

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "middleware/rsc_def.h"


/**************************************************************************
 *                          C O N S T A N T S
 **************************************************************************/
/*
rsc en
*/
typedef enum sp5kRscEn_e {
	SP5K_RSC_EN_ENABLE = RSC_EN_ENABLE,
	SP5K_RSC_EN_DISABLE = RSC_EN_DISABLE,
} sp5kRscEn_t;

/*
rsc paramter setting
*/
typedef enum sp5kRscCfg_e {
	SP5K_RSC_CFG_ENABLE = RSC_CFG_ENABLE,     /* 0: disable rsc, 1: enable rsc */
	SP5K_RSC_CFG_X_COMP_RANGE_MAX = RSC_CFG_X_COMP_RANGE_MAX,	/* pixel */
	SP5K_RSC_CFG_Y_COMP_RANGE_MAX = RSC_CFG_Y_COMP_RANGE_MAX,	/* pixel */
	SP5K_RSC_CFG_CDSP_TO_RS = RSC_CFG_CDSP_TO_RS,	/* CDSP to RS time 10us */
	SP5K_RSC_CFG_COMP_RANGE_PERCENT  = RSC_CFG_COMP_RANGE_PERCENT,	/* RSC compensate percentage 0~100 */
	SP5K_RSC_CFG_DST_WIDTH = RSC_CFG_DST_WIDTH,	/* RSC destination size */
	SP5K_RSC_CFG_DST_HEIGHT = RSC_CFG_DST_HEIGHT,	/* RSC destination size */
	SP5K_RSC_CFG_MAX = RSC_CFG_MAX,
} sp5kRscCfg_t;

/*
rsc cb setting
*/
typedef enum sp5kRscModuleCbSvcId_e {
	SP5K_RSC_CB_SVC_ID_FRAME_END = RSC_CB_SVC_ID_FRAME_END,
	SP5K_RSC_CB_SVC_ID_FRAME_START = RSC_CB_SVC_ID_FRAME_START,
	SP5K_RSC_CB_SVC_ID_TOTAL = RSC_CB_SVC_ID_TOTAL,
} sp5kRscModuleCbSvcId_t;

typedef UINT32 (*fp_sp5kRscCallback_t)(UINT32, UINT32, void *);


#if 0
/*
data structure for rsc accumulation offset setting input
*/
typedef struct {
	SINT16 x;		 /* horizontal accumulation offset */
	SINT16 y; 		 /* vertical accumulation offset */
} rscAccOffsetInputParam_t;

/*
data structure for rsc accumulation offset setting output
*/
typedef struct {
	UINT32 frameID;
} rscAccOffsetOutputParam_t;
#endif




/**************************************************************************
 *                          D A T A   T Y P E S
 **************************************************************************/


/**************************************************************************
 *                              M A C R O S
 **************************************************************************/

/**************************************************************************
 *              F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/

UINT32 sp5kRscCfgSet (sp5kRscCfg_t id,UINT32 value);
UINT32 sp5kRscCfgGet (sp5kRscCfg_t id,UINT32 *value);

UINT32 sp5kRscCbSet (sp5kRscModuleCbSvcId_t id,fp_sp5kRscCallback_t pfunc);

UINT32 sp5kRscCompensateSet (SINT32 x, SINT32 y, UINT32 interval_ms);


#endif  /* _SP5K_RSC_API_H_ */

