/**************************************************************************
 *
 *       Copyright (c) 2009-2014 by iCatch Technology, Inc.
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
 * @file sp5k_moe_api.h
 *
 * @author Matt Wang
 */
#ifndef _SP5K_MOE_API_H_
#define _SP5K_MOE_API_H_

/**************************************************************************
 *                          C O N S T A N T S
 **************************************************************************/
typedef enum _sp5kMoeComponentTot_e {
	SP5K_MOE1_SRC_TOT    = 4,
	SP5K_MOE1_DST_TOT    = 3,
	SP5K_MOE1_ACC_TOT    = 3,
	SP5K_MOE1_COUNT_TOT  = 2,
	SP5K_MOE1_CONST_TOT  = 6,
	SP5K_MOE1_RANDOM_TOT = 1,
	SP5K_MOE1_CLAMP_TOT  = 4,
	SP5K_MOE1_MULT_TOT   = 4,
	SP5K_MOE1_DIV_TOT    = 2,
	SP5K_MOE1_SHIFT_TOT  = 8,
} sp5kMoeComponentTot_e;

typedef enum _sp5kMoeHwLimitation_e {
	SP5K_MOE2_FILTER_SRC_W_MAX =  672,
	SP5K_MOE2_FILTER_REG_W_MAX = 1344,
} sp5kMoeHwLimitation_e;

typedef enum _sp5kMoeDataFmt_e {
	SP5K_MOE_UNSIGNED_8BIT      = ((0 << 8) | (0 << 4)),
	SP5K_MOE_UNSIGNED_16BIT     = ((0 << 8) | (1 << 4)),
	SP5K_MOE_UNSIGNED_YUV422_Y  = ((0 << 8) | (2 << 4) | (0 << 0)),
	SP5K_MOE_UNSIGNED_YUV422_U  = ((0 << 8) | (2 << 4) | (1 << 0)),
	SP5K_MOE_UNSIGNED_YUV422_V  = ((0 << 8) | (2 << 4) | (2 << 0)),
	SP5K_MOE_UNSIGNED_YUV422_UV = ((0 << 8) | (2 << 4) | (3 << 0)),
	SP5K_MOE_UNSIGNED_YUV420_Y  = ((0 << 8) | (3 << 4) | (0 << 0)),
	SP5K_MOE_UNSIGNED_YUV420_U  = ((0 << 8) | (3 << 4) | (1 << 0)),
	SP5K_MOE_UNSIGNED_YUV420_V  = ((0 << 8) | (3 << 4) | (2 << 0)),
	SP5K_MOE_UNSIGNED_YUV420_UV = ((0 << 8) | (3 << 4) | (3 << 0)),
	SP5K_MOE_SIGNED_8BIT        = ((1 << 8) | (0 << 4)),
	SP5K_MOE_SIGNED_16BIT       = ((1 << 8) | (1 << 4)),
	SP5K_MOE_SIGNED_YUV422_Y    = ((1 << 8) | (2 << 4) | (0 << 0)),
	SP5K_MOE_SIGNED_YUV422_U    = ((1 << 8) | (2 << 4) | (1 << 0)),
	SP5K_MOE_SIGNED_YUV422_V    = ((1 << 8) | (2 << 4) | (2 << 0)),
	SP5K_MOE_SIGNED_YUV422_UV   = ((1 << 8) | (2 << 4) | (3 << 0)),
	SP5K_MOE_SIGNED_YUV420_Y    = ((1 << 8) | (3 << 4) | (0 << 0)),
	SP5K_MOE_SIGNED_YUV420_U    = ((1 << 8) | (3 << 4) | (1 << 0)),
	SP5K_MOE_SIGNED_YUV420_V    = ((1 << 8) | (3 << 4) | (2 << 0)),
	SP5K_MOE_SIGNED_YUV420_UV   = ((1 << 8) | (3 << 4) | (3 << 0)),
} sp5kMoeDataFmt_e;  /* plz keep sync to halMoeCqDataFmt_e. */

typedef enum _sp5kMoe2UvExp_e {
	SP5K_MOE2_UV_DUP       =             (0 << 0),   /**< Duplicate. */
	SP5K_MOE2_UV_AVG_TRUNC = ((0 << 1) | (1 << 0)),  /**< Average and truncate. */
	SP5K_MOE2_UV_AVG_ROUND = ((1 << 1) | (1 << 0)),  /**< Average and round up. */
} sp5kMoe2UvExp_e;  /* plz keep sync to halMoeCqMoe2UvExp_e. */

#if 0
typedef enum _sp5kMoe2ScaleDownFunc_e {
	SP5K_MOE2_ACCU_SCALE_DOWN = 0,  /**< Accumulated scale down. */
	SP5K_MOE2_DROP_SCALE_DOWN = 1,  /**< Dropped scale down. */
} sp5kMoe2ScaleDownFunc_e;  /* plz keep sync to halMoeCqMoe2ScaleDownFunc_e. */

typedef enum _sp5kMoe2FilterFunc_e {
	SP5K_MOE2_GEN_IMG_FILTER = 0,  /**< General image filter. */
	SP5K_MOE2_MAX_IMG_FILTER = 1,  /**< Maximum image filter. */
	SP5K_MOE2_MIN_IMG_FILTER = 2,  /**< Minimum image filter. */
	SP5K_MOE2_MED_IMG_FILTER = 3,  /**< Median image filter. */
} sp5kMoe2FilterFunc_e;  /* plz keep sync to halMoeCqMoe2FilterFunc_e. */
#endif

typedef enum _sp5kMoe2EfltVal_e {
	SP5K_MOE2_EFLT_VAL_DIFF = 0,  /**< E-filter input/output use difference between pixel and center pixel. */
	SP5K_MOE2_EFLT_VAL_ORG  = 1,  /**< E-filter input/output use pixel original value. */
} sp5kMoe2EfltVal_e;  /* plz keep sync to halMoeCqMoe2EfltVal_e. */

typedef enum _sp5kMoe2EfilterSRange_e {
	SP5K_MOE2_EFILTER_S_1S7P0 = 0,  /**< Parameter output slop range in 1s7.0b format. */
	SP5K_MOE2_EFILTER_S_1S6P1 = 1,  /**< Parameter output slop range in 1s6.1b format. */
	SP5K_MOE2_EFILTER_S_1S5P2 = 2,  /**< Parameter output slop range in 1s5.2b format. */
	SP5K_MOE2_EFILTER_S_1S4P3 = 3,  /**< Parameter output slop range in 1s4.3b format. */
	SP5K_MOE2_EFILTER_S_1S3P4 = 4,  /**< Parameter output slop range in 1s3.4b format. */
	SP5K_MOE2_EFILTER_S_1S2P5 = 5,  /**< Parameter output slop range in 1s2.5b format. */
	SP5K_MOE2_EFILTER_S_1S1P6 = 6,  /**< Parameter output slop range in 1s1.6b format. */
	SP5K_MOE2_EFILTER_S_1S0P7 = 7,  /**< Parameter output slop range in 1s0.7b format. */
} sp5kMoe2EfilterSRange_e;  /* plz keep sync to halMoeCqMoe2EfilterSRange_e. */

typedef enum _sp5kMoe2Mirror_e {
	SP5K_MOE2_MIRROR_DIS  = (0x0),  /**< Disable mirror function. */
	SP5K_MOE2_MIRROR_T    = (0x1),  /**< Enable top boundary mirror. */
	SP5K_MOE2_MIRROR_B    = (0x2),  /**< Enable bottom boundary mirror. */
	SP5K_MOE2_MIRROR_L    = (0x4),  /**< Enable left boundary mirror. */
	SP5K_MOE2_MIRROR_R    = (0x8),  /**< Enable right boundary mirror. */
	SP5K_MOE2_MIRROR_TB   = (SP5K_MOE2_MIRROR_T  | SP5K_MOE2_MIRROR_B),
	SP5K_MOE2_MIRROR_TL   = (SP5K_MOE2_MIRROR_T  | SP5K_MOE2_MIRROR_L),
	SP5K_MOE2_MIRROR_TR   = (SP5K_MOE2_MIRROR_T  | SP5K_MOE2_MIRROR_R),
	SP5K_MOE2_MIRROR_BL   = (SP5K_MOE2_MIRROR_B  | SP5K_MOE2_MIRROR_L),
	SP5K_MOE2_MIRROR_BR   = (SP5K_MOE2_MIRROR_B  | SP5K_MOE2_MIRROR_R),
	SP5K_MOE2_MIRROR_LR   = (SP5K_MOE2_MIRROR_L  | SP5K_MOE2_MIRROR_R),
	SP5K_MOE2_MIRROR_TBL  = (SP5K_MOE2_MIRROR_TB | SP5K_MOE2_MIRROR_L),
	SP5K_MOE2_MIRROR_TBR  = (SP5K_MOE2_MIRROR_TB | SP5K_MOE2_MIRROR_R),
	SP5K_MOE2_MIRROR_TLR  = (SP5K_MOE2_MIRROR_T  | SP5K_MOE2_MIRROR_LR),
	SP5K_MOE2_MIRROR_BLR  = (SP5K_MOE2_MIRROR_B  | SP5K_MOE2_MIRROR_LR),
	SP5K_MOE2_MIRROR_TBLR = (SP5K_MOE2_MIRROR_TB | SP5K_MOE2_MIRROR_LR),
} sp5kMoe2Mirror_e;  /* plz keep sync to halMoeCqMoe2Mirror_e. */

#if 0
typedef enum _sp5kMoe2DividerFunc_e {
	SP5K_MOE2_DIV_REG    = (0 << 8),  /**< Register defined divisor. */
	SP5K_MOE2_DIV_FILTER = (1 << 8),  /**< Sum of mask parameters as divisor, only for SP5K_MOE2_GEN_IMG_FILTER. */
} sp5kMoe2DividerFunc_e;  /* plz keep sync to halMoeCqMoe2DividerFunc_e. */
#endif

typedef enum _sp5kMoeAttr_e {
	SP5K_MOE_DIRECT_WAIT = (0 << 0),  /**< Directly wait for execution finished. */
	SP5K_MOE_ASYNC_WAIT  = (1 << 0),  /**< Async wait for execution finished. */
} sp5kMoeAttr_e;  /* plz keep sync to halMoeCqAttr_e. */

/**************************************************************************
 *                          D A T A   T Y P E S
 **************************************************************************/
/* moe1. */
typedef struct _sp5kMoe1SrcCfg_t {
	UINT32 en;    /**< 0: disable, 1: enable src component. */
	void  *pbuf;  /**< Frame buffer address (unit in byte). */
	UINT32 bufW;  /**< Frame buffer width (unit in pixel). */
	UINT32 bufH;  /**< Frame buffer height (unit in pixel). */
	UINT32 fmt;   /**< Data format, one of moeDataFmt_e. */
	SINT32 roiX;  /**< Frame buffer ROI offset x (unit in pixel). */
	SINT32 roiY;  /**< Frame buffer ROI offset y (unit in pixel). */
	SINT32 stpY;  /**< Step y for filter mask (unit in pixel). */
} sp5kMoe1SrcCfg_t;  /* plz keep sync to halMoeCqMoe1SrcCfg_t. */

typedef struct _sp5kMoe1DstCfg_t {
	UINT32 en;    /**< 0: disable, 1: enable dst component. */
	void  *pbuf;  /**< Frame buffer address (unit in byte). */
	UINT32 bufW;  /**< Frame buffer width (unit in pixel). */
	UINT32 bufH;  /**< Frame buffer height (unit in pixel). */
	UINT32 fmt;   /**< Data format, one of moeDataFmt_e. */
	SINT32 roiX;  /**< Frame buffer ROI offset x (unit in pixel). */
	SINT32 roiY;  /**< Frame buffer ROI offset y (unit in pixel). */
	SINT32 stpY;  /**< Step y for filter mask (unit in pixel). */
} sp5kMoe1DstCfg_t;  /* plz keep sync to halMoeCqMoe1DstCfg_t. */

typedef struct _sp5kMoe1CountCfg_t {
	UINT32 en;     /**< Each bit, 0: disable, 1: enable function, see description of each element. */
	UINT32 intrv;  /**< (When en[1]==1) Number of pixels per interval, 0~65535. */
	UINT32 end;    /**< (When en[2]==1) Count end for resetting counter to 0, 0~65535. */
} sp5kMoe1CountCfg_t;  /* plz keep sync to halMoeCqMoe1CountCfg_t. */

typedef struct _sp5kMoe1ConstCfg_t {
	UINT32 en;   /**< Each bit, 0: disable, 1: enable function, see description of each element. */
	SINT32 val;  /**< (When en[1]==1) Constant value, 0~65535. */
} sp5kMoe1ConstCfg_t;  /* plz keep sync to halMoeCqMoe1ConstCfg_t. */

typedef struct _sp5kMoe1RandomCfg_t {
	UINT32 en;    /**< Each bit, 0: disable, 1: enable function, see description of each element. */
	UINT32 msk;   /**< (When en[1]==1) Random number bit mask, [15:0]. */
	UINT32 seed;  /**< (When en[2]==1) Random seed, 0~65535. */
} sp5kMoe1RandomCfg_t;  /* plz keep sync to halMoeCqMoe1RandomCfg_t. */

typedef struct _sp5kMoe1ClampCfg_t {
	UINT32 en;     /**< Each bit, 0: disable, 1: enable function, see description of each element. */
	SINT32 loThr;  /**< (When en[1]==1) Low threshold, -32768~32767. */
	SINT32 hiThr;  /**< (When en[2]==1) High threshold, -32768~32767. */
} sp5kMoe1ClampCfg_t;  /* plz keep sync to halMoeCqMoe1ClampCfg_t. */

typedef struct _sp5kMoe1MultCfg_t {
	UINT32 en;     /**< Each bit, 0: disable, 1: enable function, see description of each element. */
	UINT32 quant;  /**< (When en[1]==1) Quantization for quotient, 0~16, output quotient will be right shift for quant bits. */
	UINT32 round;  /**< (When en[2]==1) 0: disable, 1: enable rounding off for quotient. */
	UINT32 clamp;  /**< (When en[3]==1) 0: disable, 1: enable clamping quotient to -32768~32767. When enable, quant must be 0. */
} sp5kMoe1MultCfg_t;  /* plz keep sync to halMoeCqMoe1MultCfg_t. */

typedef struct _sp5kMoe1DivCfg_t {
	UINT32 en;    /**< Each bit, 0: disable, 1: enable function, see description of each element. */
	SINT32 div0;  /**< (When en[1]==1) Data for quotient when divisor is zero, -32768~32767. */
} sp5kMoe1DivCfg_t;  /* plz keep sync to halMoeCqMoe1DivCfg_t. */

typedef struct _sp5kMoe1ShiftCfg_t {
	UINT32 en;   /**< Each bit, 0: disable, 1: enable function, see description of each element. */
	SINT32 bit;  /**< (When en[1]==1) 0: no shift, 1~15: left shift 1~15 bits, -1~-16: right shift 1~16 bits. */
} sp5kMoe1ShiftCfg_t;  /* plz keep sync to halMoeCqMoe1ShiftCfg_t. */

typedef struct _sp5kMoe1AccRet_t {
	SINT64 sum;  /**< Summation of pixels in ROI. */
	SINT32 max;  /**< Maximum of pixels in ROI. */
	SINT32 min;  /**< Minimum of pixels in ROI. */
} sp5kMoe1AccRet_t;  /* plz keep sync to halMoeCqMoe1AccRet_t. */

/* moe2. */
typedef struct _sp5kMoe2SrcCfg_t {
	UINT32 en;     /**< 0: disable, 1: enable src component. */
	void  *pbuf;   /**< Frame buffer address (unit in byte). */
	UINT32 bufW;   /**< Frame buffer width (unit in pixel). */
	UINT32 bufH;   /**< Frame buffer height (unit in pixel). */
	UINT32 fmt;    /**< Data format, one of moeDataFmt_e. */
	SINT32 roiX;   /**< Frame buffer ROI offset x (unit in pixel). */
	SINT32 roiY;   /**< Frame buffer ROI offset y (unit in pixel). */
	UINT32 uvExp;  /**< Expanding YUV422/YUV420 to YUV444 method, one of sp5kMoe2UvExp_e. */
} sp5kMoe2SrcCfg_t;  /* plz keep sync to halMoeCqMoe2SrcCfg_t. */

typedef struct _sp5kMoe2ScaleDown_t {
	UINT32 en;    /**< Each bit, 0: disable, 1: enable function, see description of each element. */
	#if 0
	UINT32 func;  /**< (When en[1]==1) Scale down function selection, one of sp5kMoe2ScaleDownFunc_e. */
	#endif
	UINT32 srcW;  /**< (When en[2]==1) Source width before scaling. */
	UINT32 srcH;  /**< (When en[3]==1) Source height before scaling. */
} sp5kMoe2ScaleDown_t;  /* plz keep sync to halMoeCqMoe2ScaleDown_t. */

typedef struct _sp5kMoe2EfilterCfg_t {
	UINT32 en;      /**< Each bit, 0: disable, 1: enable function, see description of each element. */
	UINT32 inVal;   /**< (When en[1]==1) Input value selection, one of sp5kMoe2EfltVal_e. */
	UINT32 outVal;  /**< (When en[2]==1) Output value selection, one of sp5kMoe2EfltVal_e. */
	SINT32 *xAry;   /**< (When en[3]==1) Parameter input threshold array (5 SINT32 entries). Note xAry[0] is not used. */
	SINT32 *yAry;   /**< (When en[4]==1) Parameter output offset array (5 SINT32 entries). */
	SINT32 *sAry;   /**< (When en[5]==1) Parameter output slop array (5 SINT32 entries). */
	UINT32 sRng;    /**< (When en[6]==1) Parameter output slop range, one of halMoeCqMoe2EfilterSRange_e. */
} sp5kMoe2EfilterCfg_t;  /* plz keep sync to halMoeCqMoe2EfilterCfg_t. */

typedef struct _sp5kMoe2Filter_t {
	UINT32 en;       /**< Each bit, 0: disable, 1: enable function, see description of each element. */
	#if 0
	UINT32 func;     /**< (When en[1]==1) Filter function selection, one of sp5kMoe2FilterFunc_e. */
	#endif
	UINT32 mir;      /**< (When en[2]==1) Boundary mirror, one of sp5kMoe2Mirror_e. */
	UINT32 mskStp;   /**< (When en[3]==1) Filter mask step, 0 means no gap between adjacent pixels. */
	UINT32 prmEn;    /**< (When en[4]==1) Bitmap [24:0] enable for filter mask parameter 25 slots. */
	UINT32 prmSgn;   /**< (When en[5]==1) Bitmap [24:0] sign bit for filter mask parameter 25 slots. */
	UINT32 *prmAry;  /**< (When en[6]==1) Filter mask parameter array set to registers (9 UINT32 entries). */
} sp5kMoe2Filter_t;  /* plz keep sync to halMoeCqMoe2Filter_t. */

typedef struct _sp5kMoe2ShifterCfg_t {
	UINT32 en;   /**< Each bit, 0: disable, 1: enable function, see description of each element. */
	SINT32 bit;  /**< (When en[1]==1) 0: no shift, 1~15: left shift 1~15 bits, -1~-16: right shift 1~16 bits. */
} sp5kMoe2ShifterCfg_t;  /* plz keep sync to halMoeCqMoe2ShifterCfg_t. */

typedef struct _sp5kMoe2Divider_t {
	UINT32 en;      /**< Each bit, 0: disable, 1: enable function, see description of each element. */
	#if 0
	UINT32 func;    /**< (When en[1]==1) Divider function selection, one of sp5kMoe2DividerFunc_e. */
	#endif
	UINT32 clmpEn;  /**< (When en[2]==1) 0: use 1 sign bit and LSB 15 bits, 1: clamp to -65536~65535 as dividend. */
	SINT32 dvsr;    /**< (When en[3]==1) Divisor, 0~8191. */
} sp5kMoe2Divider_t;  /* plz keep sync to halMoeCqMoe2Divider_t. */

typedef struct _sp5kMoe2ClamperCfg_t {
	UINT32 en;     /**< Each bit, 0: disable, 1: enable function, see description of each element. */
	SINT32 loThr;  /**< (When en[1]==1) Low threshold, -32768~32767. */
	SINT32 hiThr;  /**< (When en[2]==1) High threshold, -32768~32767. */
} sp5kMoe2ClamperCfg_t;  /* plz keep sync to halMoeCqMoe2ClamperCfg_t. */

/* common. */
typedef struct _sp5kMoeExecCfg_t {
	sp5kMoe1SrcCfg_t      moe1Src[SP5K_MOE1_SRC_TOT];      /**< MOE1 src component. */
	sp5kMoe1DstCfg_t      moe1Dst[SP5K_MOE1_DST_TOT];      /**< MOE1 dst component. */
	sp5kMoe1CountCfg_t    moe1Cnt[SP5K_MOE1_COUNT_TOT];    /**< MOE1 count component. */
	sp5kMoe1ConstCfg_t    moe1Cnst[SP5K_MOE1_CONST_TOT];   /**< MOE1 const component. */
	sp5kMoe1RandomCfg_t   moe1Rand[SP5K_MOE1_RANDOM_TOT];  /**< MOE1 random component. */
	sp5kMoe1ClampCfg_t    moe1Clmp[SP5K_MOE1_CLAMP_TOT];   /**< MOE1 clamp component. */
	sp5kMoe1MultCfg_t     moe1Mul[SP5K_MOE1_MULT_TOT];     /**< MOE1 mult component. */
	sp5kMoe1DivCfg_t      moe1Div[SP5K_MOE1_DIV_TOT];      /**< MOE1 div component. */
	sp5kMoe1ShiftCfg_t    moe1Shf[SP5K_MOE1_SHIFT_TOT];    /**< MOE1 shift component. */
	sp5kMoe2SrcCfg_t      moe2SrcImg;                      /**< MOE2 image src component. */
	sp5kMoe2SrcCfg_t      moe2SrcPrm;                      /**< MOE2 parameter src component. */
	sp5kMoe2ScaleDown_t   moe2SclDn;                       /**< MOE2 scale down component. */
	sp5kMoe2EfilterCfg_t  moe2Eflt;                        /**< MOE2 e-filter component. */
	sp5kMoe2Filter_t      moe2Flt;                         /**< MOE2 filter component. */
	sp5kMoe2ShifterCfg_t  moe2Shf;                         /**< MOE2 shifter component. */
	sp5kMoe2Divider_t     moe2Div;                         /**< MOE2 divider component. */
	sp5kMoe2ClamperCfg_t  moe2Clmp;                        /**< MOE2 clamper component. */
	UINT32 roiW;                                           /**< ROI width on MOE1 dst. */
	UINT32 roiH;                                           /**< ROI height on MOE1 dst. */
	UINT32 attr;                                           /**< Miscellaneous attributes, combination of sp5kMoeAttr_e. */
} sp5kMoeExecCfg_t;  /* plz keep sync to halMoeExecCfg_t. */

typedef struct _sp5kMoeExecRet_t {
	sp5kMoe1AccRet_t acc[SP5K_MOE1_ACC_TOT];  /**< MOE1 acc component. */
} sp5kMoeExecRet_t;  /* plz keep sync to halMoeExecRet_t. */

/**************************************************************************
 *                              M A C R O S
 **************************************************************************/
/** @brief Clear variable in data type of sp5kMoeExecCfg_t. */
#define SP5K_MOE_EXEC_CFG_CLEAR(pcfg)  CFG_CLEAR(pcfg, sizeof(sp5kMoeExecCfg_t))

/**************************************************************************
 *              F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
UINT32 sp5kMoeCommandQueueExec(sp5kMoeExecCfg_t *pcfg, sp5kMoeExecRet_t *pret, void *pcqTbl, UINT32 len);
UINT32 sp5kMoeCommandQueueAsyncWait(sp5kMoeExecRet_t *pret);
SINT32 sp5kMoeFilterMaskSumGet(UINT32 signBit, UINT32 *prmAry);

#endif  /* _SP5K_MOE_API_H_ */

