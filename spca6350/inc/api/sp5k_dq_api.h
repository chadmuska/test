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
/* DO NOT EDIT the next 2 lines for auto revision by subversion
$Rev: 27351 $
$Date:: 2016-10-26 15:36:41#$
 */

#ifndef _SP5K_DQ_API_H_
#define _SP5K_DQ_API_H_

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
typedef enum {
	SP5K_DYN_IQ_EVT_ISO		= 1<<0,
	SP5K_DYN_IQ_EVT_CT		= 1<<1,
	SP5K_DYN_IQ_EVT_HD_INT	= 1<<2,

	SP5K_DYN_IQ_EVT_AGC_SET	= 1<<3,
	SP5K_DYN_IQ_EVT_EOF		= 1<<4,
	SP5K_DYN_IQ_EVT_ABORT	= 1<<5,

	SP5K_DYN_IQ_EVT_ALL		= (1<<6)-1
} sp5kDnyIqEvent_t;



typedef enum{
	SP5K_DYN_IQ_CFG_FIF_WORK_DIR=0,				/*param 1 : diq resource working path */
	SP5K_DYN_IQ_CFG_FIF_INIT,					/*param 1 : any value */
	SP5K_DYN_IQ_CFG_SYS_INIT,					/*param 1 : any value */
	SP5K_DYN_IQ_CFG_LSC_RATIO,					/*param 1 : ratio (param1/base)for target , base = 1024 */
	SP5K_DYN_IQ_CFG_OB_RATIO,					/*param 1 : ratio (param1/base)for target , base = 1024 */
	SP5K_DYN_IQ_CFG_AWB_CT_SET,					/*param 1 : color temperature , param 2 : sync option */
	SP5K_DYN_IQ_CFG_GLOBAL_RES_FLUSH,			/*param 1 : sync option */
	SP5K_DYN_IQ_CFG_PAUSE_SET,					/*param 1 : 1:pause, 0:run */
	SP5K_DYN_IQ_CFG_CT_IDX_SET,					/*param 1 : CT index ,param 2: sync option*/
	SP5K_DYN_IQ_CFG_ISO_IDX_SET,				/*param 1 : ISO idnex ,param 2: sync option */
	SP5K_DYN_IQ_CFG_FIX_ISO_IDX_SET,			/*param 1 : en , param 2 : idx , param 3: sync option */
	SP5K_DYN_IQ_CFG_FIX_CT_IDX_SET,				/*param 1 : en , param 2 : idx , param 3: sync option */
	SP5K_DYN_IQ_CFG_RGB_GAMMA_LOAD,				/*param 1 : r gamma param 2 : g gamma , param 3: bgamma , param 4: sync option*/
	SP5K_DYN_IQ_CFG_RGB_GAMMA_RATIO,			/*param 1 : ratio (param1/base) for Target, base = 1024 */
	SP5K_DYN_IQ_CFG_ZOOM_IDX_SET,				/*param 1 : ZOOM index ,param 2: sync option*/
	SP5K_DYN_IQ_CFG_FIX_ZOOM_IDX_SET,			/*param 1 : en , param 2 : idx , param 3: sync option */
	SP5K_DYN_IQ_CFG_AE_INFO_SET,				/*param 1 : aaaProcInfo_t , param 2: lvx1000*/
	SP5K_DYN_IQ_CFG_AUTO_RGBGMA_CB_REG,			/*param 1 : callback for auto rgb gamma */
	SP5K_DYN_IQ_CFG_AUTO_YGMA_CB_REG,			/*param 1 : callback for auto y gamma */
	SP5K_DYN_IQ_CFG_WDR_CB_REG,					/*param 1 : callback for auto WDR */
	SP5K_DYN_IQ_CFG_AUTO_GMA_INTVL_SET,			/*param 1 : intervel for auto gamma */
	SP5K_DYN_IQ_CFG_INTPL_INTVL_SET,			/*param 1 : intervel for interpolation */
	SP5K_DYN_IQ_CFG_WDR_INTVL_SET,				/*param 1 : intervel for dyanmic wdr */
	SP5K_DYN_IQ_CFG_Y_GAMMA_LOAD,				/*param 1 : y gamma,  param 2: sync option*/
	SP5K_DYN_IQ_CFG_CAP_ANR_FLOW_OFF_SET,		/*param 1 : turn off ANR*/
	SP5K_DYN_IQ_CFG_DEL_ALL_CQ,					/*param 1 : none*/
	SP5K_DYN_IQ_CFG_AVA_IN_CAP,					/*param 1 : en (DIQ is available for capture mode)*/
	SP5K_DYN_IQ_CFG_BITRATE_CTRL_SET,			/*param 1 : en (bit rate ctrl), param 2 : iq api selector (-1: all), param 3 : level 0~1024, param 4 : sync option, param 5 : to capture */
	SP5K_DYN_IQ_CFG_DUAL_SEN_OPT_SET,			/*param 1 : dual sensor option, same as modeswPreviewDualSensorOpt_e */
	SP5K_DYN_IQ_CFG_GLOBAL_SHARP_SET,			/*param 1 : global sharpness strength (-128 ~ +127 2's complement), param2 : to capture */
	SP5K_DYN_IQ_CFG_GLOBAL_SATURATION_SET,		/*param 1 : global saturation gain(0 ~ 255, 64 for 1x, param2 : to capture */
	SP5K_DYN_IQ_CFG_CDSP_RES_NO_REDUCE,			/*param 1 : 1: don't reduce memory 0: reduce memory */
	SP5K_DYN_IQ_CFG_EXT_CFG_TO_CAP,				/*param 1 : please refer sp5kDynIqApplyParToCapSel_t */
	SP5K_DYN_IQ_CFG_WDR_PAR_SET,				/*param 1 : please refer sp5kDynIqWdrPar_t, param 2 : sync option */
	SP5K_DYN_IQ_CFG_AHD_NP_SET,					/*param 1 : noise profile array UINT32[0:16] Y/ UINT32[17:25] UV, base = 1024*/
													/*param 2 : svd, param 3 : toCap*/
	SP5K_DYN_IQ_CFG_M_R_CFG_SET,				/*param 1 : enable, param 2 : type(please refer sp5kCdspDiqMReplaceType_t, param 3 : iq selector */
	SP5K_DYN_IQ_CFG_YNF_BLUR_STRH_SCALE_SET,	/*param 1 : direction, param 2 : weight, param 3 : to capture, param 4 : svd */
	SP5K_DYN_IQ_CFG_SPC_EFF_CLRMAT1_SET,		/*param 1 : array contains 9 effect matrix coefficients of UINT16 type, param 2 : to capture, param 3 : svd */
	SP5K_DYN_IQ_CFG_RES_REPLACE_SET,			/*param 1 : option(add/del), param 2 : senId(1~n) , param 3 : sensor mode (0x30~0x4f), param 4 : selector (SP5K_DIQ_MODE_CT/SP5K_DIQ_MODE_ISO/SP5K_DIQ_MODE_ZOOM), 
												param 5 : index, param 6 : specific resource index, param 7 : svd*/
	SP5K_DYN_IQ_CFG_SPC_DEFORM_SET,             /*param 1 : enable/disable LDC deform severe hadle*/
} sp5kDynIqCfg_t;

typedef enum{
	SP5K_DYN_IQ_CFG_CT_IDX_GET=0,				/*param 1 : diq ct index pointer */
	SP5K_DYN_IQ_CFG_ISO_IDX_GET,				/*param 1 : diq iso index pointer */
	SP5K_DYN_IQ_CFG_RGB_GAMMA_GET,				/*param 1 : r/g/b gamma pointer to pointer , param 2 : resID */
	SP5K_DYN_IQ_CFG_CAP_ANR_FLOW
} sp5kDynIqCfgGet_t;

typedef enum {
	SP5K_DYN_IQ_UPD_NONE		= 0,
	SP5K_DYN_IQ_UPD_Y_GAMMA		= 1 << 0,
	SP5K_DYN_IQ_UPD_SAT			= 1 << 1,
	SP5K_DYN_IQ_UPD_RGB_GAMMA	= 1 << 2,
	SP5K_DYN_IQ_UPD_WDR			= 1 << 3,
	SP5K_DYN_IQ_UPD_WDR_CAP		= 1 << 4
} sp5kDynIqUpd_t;

typedef enum {
	SP5K_CDSP_CAP_ANR_LITE_FLOW = 0,
	SP5K_CDSP_CAP_ANR_FLOW,
} sp5kDynIqAnrFlowSel_t;

typedef enum {
	SP5K_CDSP_DIQ_M_R_TYPE_BLK = 0,
	SP5K_CDSP_DIQ_M_R_TYPE_CFG,
	SP5K_CDSP_DIQ_M_R_TYPE_RES,
	SP5K_CDSP_DIQ_M_R_TYPE_MAX,
} sp5kCdspDiqMReplaceType_t;

typedef enum sp5kDiqResSupplementOp {
	SP5K_DIQ_RES_SUPP_DEL = 0x0,
	SP5K_DIQ_RES_SUPP_ADD = 0x1,

	SP5K_DIQ_RES_SUPP_MAX = 0x2
}  sp5kDiqResSupplementOp_t;

typedef enum sp5kDiqModeSelector_e {
    SP5K_DIQ_MODE_NA = 0x0,
	SP5K_DIQ_MODE_SRM,
	SP5K_DIQ_MODE_CT,
    SP5K_DIQ_MODE_ISO,
    SP5K_DIQ_MODE_LV,
	SP5K_DIQ_MODE_ZOOM,
	SP5K_DIQ_MODE_SPC,

	SP5K_DIQ_MODE_MAX
} sp5kDiqSelectMode_t;

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct sp5kDynIqWdrData_s{
	UINT32 histBefore[256];
	UINT32 histAfter[256];
	UINT32 yAvg_up;
	UINT32 yAvg_down;
	UINT32 yAvg_left;
	UINT32 yAvg_right;
}sp5kDynIqWdrData_t;

typedef struct sp5kDynIqWdrBuf_s{
	/*** histogram ***/
	UINT32 hist[256];
	UINT32 histPos;
	/*** wdr parameters ***/
	UINT32 wdryga;
	UINT32 wdrqs;
	UINT32 wdres;
	UINT32 wdryoff;
	UINT16 wdrqlut[16];
	UINT16 wdruthrlut[16];
	UINT16 wdrvthrlut[16];
	UINT32 upd;
	/*** WDRFT.BIN ***/
	UINT16 *par;
	UINT32 idxQty;
	UINT32 parQty;
}sp5kDynIqWdrBuf_t;

typedef struct sp5kDynIqCapAnrFlow_t{
	UINT8 redoEn;
	UINT8 anrLiteEn;
	UINT8 anrEn;
	UINT8 rsv[5];
} sp5kDynIqCapAnrFlow_t;

typedef struct sp5kDynIqWdrPar_s{
	UINT32 wdryga;
	UINT32 wdrqs;
	UINT32 wdres;
	UINT32 wdryoff;
	UINT16 wdrqlut[16];
	UINT16 wdruthrlut[16];
	UINT16 wdrvthrlut[16];
} sp5kDynIqWdrPar_t;
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define app_DynIqTask_En_Get 		sp5kDynIqEnGet
#define DynIqPauseSet(val)			sp5kDynIqCfgSet(SP5K_DYN_IQ_CFG_PAUSE_SET,val)
#define app_DIQ_DynIso_Set()
#define DynIqTaskInit()
#define DynIqTaskInstall(en)
#define app_DIQ_DynWdr_LVSet(sel)
#define app_DIQ_DynGamma_Set(sel,val)
#define app_DIQ_DynGamma_Intpl_init(IncBrightness,DecBrightness,SwitchGammaThd)
#define app_DIQ_DynWdr_SizeSet(hgrid,vgrid)

/* Histogram  position (histPos)
	0, after interpolation2
	1: after white/dark clip2
	2: after RGBWDR
	3: after color matrix2
	4: after YUVWDR
	5: after RS offset
	6: after digital effect
} halcdspHistPos_t;
*/

/* upd :  SP5K_DYN_IQ_UPD_RGB_GAMMA, SP5K_DYN_IQ_UPD_NONE */
typedef void (*fpSp5kDiqAutoRgbGmaCb_t)(UINT32 frmId, UINT32 *hist, UINT32 histPos, SINT16 lv, UINT8 *rGma, UINT8 *gGma,UINT8 *bGma, UINT32 *upd);
/* sat :  1s 1.6 format  (2's complement)
   upd :  SP5K_DYN_IQ_UPD_Y_GAMMA / SP5K_DYN_IQ_UPD_SAT, SP5K_DYN_IQ_UPD_NONE */
typedef void (*fpSp5kDiqAutoYGmaCb_t)(UINT32 frmId, UINT32 *hist, UINT32 histPos, SINT16 lv, UINT8 *yGma, UINT32 *sat, UINT32 *upd);

/* upd :  SP5K_DYN_IQ_UPD_WDR, SP5K_DYN_IQ_UPD_NONE */
typedef void (*fpSp5kDiqWdrCb_t)(UINT32 frmId, sp5kDynIqWdrBuf_t *wdrBuf, UINT32 *wdrIdx, UINT32 *upd);

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

UINT32 sp5kDynIqEnGet(UINT32 sel);
void sp5kDynIqResTest(UINT32 mode,UINT32 ctIdx,UINT32 isoIdx);
void sp5kDynIqFromBufCheckIn(UINT16 Select, UINT16 SMode, UINT16 CT, UINT16 ISO, char * FIFName, void * pSrc, UINT32 len );
void sp5kDynIqFromFileCheckIn( UINT16 Select, UINT16 SMode, UINT16 CT, UINT16 ISO, char * FIFName, char * FileName );
UINT32 sp5kDynIqCfgSet(sp5kDynIqCfg_t cfg,...);
void sp5kDynIqCfgGet(sp5kDynIqCfgGet_t cfg,...);
void sp5kDynIqAeInfoLock(UINT32 lock);

#endif /* _SP5K_DQ_API_H_ */
