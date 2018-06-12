/**************************************************************************
 *
 *        Copyright (c) 2006-2011 by Sunplus mMedia Inc., Ltd.
 *
 *  This software is copyrighted by and is the property of Sunplus
 *  mMedia Inc., Ltd. All rights are reserved by Sunplus mMedia
 *  Inc., Ltd. This software may only be used in accordance with the
 *  corresponding license agreement. Any unauthorized use, duplication,
 *  distribution, or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of Sunplus mMedia Inc., Ltd.
 *
 *  Sunplus mMedia Inc., Ltd. reserves the right to modify this
 *  software without notice.
 *
 *  Sunplus mMedia Inc., Ltd.
 *  19-1, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan, R.O.C. 
 *
 **************************************************************************/
#ifndef APP_ICATCH_AWB_H
#define APP_ICATCH_AWB_H

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
#include "api/sp5k_ae_api.h"
#include "api/sp5k_awb_api.h"
#include "api/sp5k_cdsp_api.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

#define AE_WINDOW_H_SIZE	16
#define AE_WINDOW_V_SIZE	16
#define AE_TARGET_LV_RANGE 18/*LV0~LV17*/
#define AE_PROGRAM_CHART_SIZE 30 


#define IRIS_STEP_1			48
#define IRIS_STEP_2			68


/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/


typedef enum _AEALG_PARAM_SEL{
	AE_PARA_PROGRAMCHART_PV,
	AE_PARA_PROGRAMCHART_CAP,	
	AE_PARA_WINDOW_LV,
	AE_PARA_WINDOW,	
	AE_PARA_TARGET,
       AE_PARA_PRE_CB_REGISTER,	
       AE_PARA_CB_REGISTER,
       AE_PARA_LOCK,
	AE_PARA_STABLE,
	AE_PARA_PERIOD,
	AE_PARA_HDR,    
	AE_PARA_SENSOR_ID,  
	AE_PARA_SENSORSEL,   
	AE_PARA_EVSET,
       AE_PARA_CONFIG,	
       AE_PARA_STATUS,
       AE_PARA_LIB_VERSION,       
	AE_PARA_CAL, 	
	AE_PARA_DEBUG,
} AEALG_PARAM_SEL;


typedef enum _AEALG_PARA_SENSOR_ID{
AE_SENSOR_ID_PRI,
AE_SENSOR_ID_SEC,
AE_SENSOR_ID_BOTH,    
} AEALG_PARA_SENSOR_ID;

typedef enum _AEALG_PARA_SENSOR_SEL{
AE_SENSOR_PRI_ONLY,
AE_SENSOR_SEC_ONLY,    
AE_SENSOR_PRI_SEC_SINGLE,
AE_SENSOR_PRI_SEC_DUAL,
} AEALG_PARA_SENSOR_SEL;


typedef struct {
	UINT32 cbcnt;
	UINT32 frame_id;
	SINT16 LVx1000;
	UINT32 Luma;
       UINT16 Target;
       SINT32 TargetStep;
       SINT32 LastBV; /*LastBV+TaregtStep = RealBV*/
	UINT8  Stable;
	
	UINT32 aperidx;
	SINT32 expidx;
	SINT32 expidxset;
	UINT32 agcidx;
	UINT32 accuridx;

	UINT32 aperidxCAP;
	SINT32 expidxCAP;
	UINT32 agcidxCAP;
	UINT32 accuridxCAP;

	UINT8 BLStatus; 
	SINT32 BLEVx1000Diff;
	SINT32 DeltaEVx1000;
	SINT16 UpLVx1000;
	UINT16 UpY;
	UINT16 DownY;
} AEStatus_t;

typedef struct {
	UINT8 AV;
	SINT16 TV;
	UINT8 SV;
} AEProgram_t;

typedef struct {
	SINT16 Ver;
	SINT16 ISO100AgcIdx;
	SINT16 ISO100AgcIdxCAP;
	SINT16 LVErrx1000;
	SINT16 DefaultBV;
	SINT16 ConvergeSpeed;/*1= fastest, bigger = slowest */
	SINT16 ConvergeSlope;
	SINT16 ConvergeLimit;
	SINT16 ConvergeSpeedFast;/*1= fastest, bigger = slowest */
	SINT16 ConvergeSlopeFast;
	SINT16 ConvergeLimitFast;
	SINT16 ConvergeRange;
	SINT16 StableRange;
	SINT16 StableCNT;
	SINT16 StillEqualPV;
	SINT16 OBError;	
	SINT16 BLCompEnable;
	SINT16 BLUpHighRatio;
	SINT16 BLDownLowRatio;
	SINT16 BLUpLVHighBound;
	SINT16 BLUpLVLowBound;
	SINT16 BLEVx1000HighBound;
	SINT16 BLEVx1000LowBound;
	SINT16 BLDownAETarget;
	SINT16 FlashTriggerBV;
	SINT16 Reserve[139];
} AEConfig_t;/*AE Behavior*/


typedef struct aeInfo_node{
	UINT32 frame_id;
	UINT8  aewin_hgrid;
	UINT8  aewin_vgrid;
	UINT16 *paewin_y;
    UINT32 *pyhis;
    SINT32 lvidx;
    struct aeInfo_node *next;
}__attribute__((packed))aeInfo_list; /* ae info backup linklist */

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/
/* check API doc:sp5kAeEvUnitGet() */



/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/



/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
/*AE */
void  appAEALGInit(void);
UINT32 appAEALG_WriteAppn(UINT32 addr, UINT32 size);
void appAEALG_ParamGet(UINT32 selector , SINT32* pValue);
void appAEALG_ParamSet(UINT32 selector,SINT32 value);


/*LIB */
SINT32 pv_agcidx_to_svidx(SINT32 agcidx);
SINT32 cap_agcidx_to_svidx(SINT32 agcidx);
SINT32 cap_svidx_to_agcidx(SINT32 svidx);
SINT32 pv_svidx_to_agcidx(SINT32 svidx);
void appAeAlgPreviewReset(SINT32 expIdx, UINT32 agcIdx);
SINT32 appAEALGLib_ResultGet(UINT32 param, aeResult_t *presult);
UINT32 AEALGFastStableCheck(void);
UINT32 AEALGFlashTriggerCheck(void);
void AEALGCurrentBVtoPreview(void);
void AEALGCurrentBVtoCapture(void);
void appAEStillCapExpAgcGet(SINT32 * pExp_Idx,UINT32 *pAgc_Idx);
void appAEStillCapExpAgcSet(SINT32 Exp_Idx,UINT32 Agc_Idx,UINT32 Accur_Agc_Idx);
void appAEPreviewExpAgcGet(SINT32 * pExp_Idx,UINT32 *pAgc_Idx);
void appAEPreviewExpAgcSet(SINT32 Exp_Idx,UINT32 Agc_Idx,UINT32 Accur_Agc_Idx);
void appAEStillCapExpAgcNGet(UINT32 Sen_Idx,SINT32 * pExp_Idx,UINT32 *pAgc_Idx);
void appAEStillCapExpAgcNSet(UINT32 Sen_Idx,SINT32 Exp_Idx,UINT32 Agc_Idx,UINT32 Accur_Agc_Idx);
void appAEPreviewExpAgcNGet(UINT32 Sen_Idx,SINT32 * pExp_Idx,UINT32 *pAgc_Idx);
void appAEPreviewExpAgcNSet(UINT32 Sen_Idx,SINT32 Exp_Idx,UINT32 Agc_Idx,UINT32 Accur_Agc_Idx);
UINT32 appAEPreviewExpIdxtoAccurateUs(SINT32 Exp_Idx);
aeInfo_list *appAEALGLib_BkInfo_Get(void);
UINT32 appAeALG_Log2x1000(SINT32 val);
/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/



#endif  /* APP_ICATCH_AWB_H */


