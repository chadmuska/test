/**************************************************************************
 *
 *       Copyright (c) 2004-2016 by iCatch Technology, Inc.
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
 *  Author: Anmin Deng                                                    *
 *                                                                        *
 **************************************************************************/
#ifndef _AAA_DEF_H_
#define _AAA_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif


typedef enum {
	AF_MODE_OFF,   /* 0x00 */
	AF_MODE_ON,    /* 0x01 */
	AF_MODE_AFWIN = AF_MODE_ON,  /* 0x01 */
	AF_MODE_SINGLE_AF,    /* 0x02 */     
	AF_MODE_AFWIN_AND_SINGLE_AF,  /* 0x03 */
	AF_MODE_VERTICAL_AF,   /* 0x04 */
	AF_MODE_AFWIN_AND_VERTICAL_AF,  /* 0x05 */
	AF_MODE_SINGLE_AND_VERTICAL_AF, /* 0x06 */
	AF_MODE_AFWIN_AND_SINGLE_AND_VERTICAL_AF,  /* 0x07 */
	AF_MODE_TOTAL,
} afMode_t;

typedef enum {
	AF_STAT_R,
	AF_STAT_G,
	AF_STAT_B,
	AF_STAT_RGB_SUM, /*default*/
	AF_STAT_R_SQR,
	AF_STAT_G_SQR,
	AF_STAT_B_SQR,
	AF_STAT_RGB_SQRSUM,
	AF_STAT_TOTAL,
} afStatMode_t;


/**< SINT32 aaa_af_result_get(UINT32 param,UINT32 sel,UINT32 *pafval); */
/**< sel */
typedef enum {
	/* AF */	
	AF_VAL_NOISE_CNT, /* 0 */
	AF_VAL_DC, /* 1 */
	AF_VAL_STAT_WIN, /* 2 */ 

	/* SAF */	
	AF_VAL_SINGLE_AF, /* 3 */
	AF_VAL_SINGLE_AF_NOISE_CNT, /* 4 */
	AF_VAL_SINGLE_AF_DC, /* 5 */
	AF_VAL_SINGLE_AF_HI_PREC, /* 6 */

	/* VAF */
	AF_VAL_VERT_AF, /* 7 */
	AF_VAL_VERT_AF_NOISE_CNT, /* 8 */
	
	AF_VAL64_STAT_WIN,  /* 9 */
	AF_VAL64_SINGLE_AF, /* 10 */
	AF_VAL64_VERT_AF,   /* 11 */
	AF_VAL_SEL_TOTAL,
} afValueSel_t;

typedef enum {
	AWB_CONVERGE_RANGE=0,		/*0*/
	AWB_CONVERGE_SPEED,
	AWB_ADAPT_DEGREE,		/* obsolete */
	AWB_PURE_COLOR_AREA,

	AWB_CUSTOM_RGAIN_SHIFT=4,	/*4*/
	AWB_CUSTOM_BGAIN_SHIFT,

	AWB_DARK_LUMA=6,			/*6*/
	AWB_BRIGHT_AELV,
	AWB_BRIGHT_CONTRAST,

	AWB_ALGORITHM=10,

	AWB_CALIBRATION_RGAIN=90,	/*90*/
	AWB_CALIBRATION_BGAIN,
	AWB_CALIBRATION_PERCENTAGE,
	AWB_CALIBRATION_FLASH_RGAIN,
	AWB_CALIBRATION_FLASH_BGAIN,

	AWB_MODE_SET=100,			/*100*/
	AWB_MODE_PROCESS,
	AWB_MODE_ARITHMETIC,
	AWB_MODE_FLOW,

	AWB_PARAM_RAW_BITNUM=200,

	AWB_AVG_RGAIN = 210,
	AWB_AVG_BGAIN,
	AWB_SET_RGAIN,
	AWB_SET_BGAIN,

	AWB_ALightMode = 410,
	AWB_JumpStep,
} awbParamSel_t;

#define AWB_PARAM_GET 0xffff
#define awbMode_auto 0x0000

typedef enum {
	/* auto */
	awbMode_5spot=0x0000,

	/* standard */
	awbMode_A=0x0001,
	awbMode_CW,
	awbMode_D,

	/* indoor */
	awbmode_indoor=0x0010,
	awbMode_tungsten,
	awbMode_fluorescent_H,
	awbMode_fluorescent_L,

	/* outdoor */
	awbMode_outdoor=0x0030,
	awbMode_sunny,
	awbMode_cloudy,
	awbMode_sunset,
	awbMode_shade,
	awbMode_grassland,

	awbMode_1spot=0x0050,
	awbMode_flash,
	awbMode_calib,
	awbMode_D_ext,
	awbMode_A_ext,

	awbMode_custom=0x0060,
	awbMode_custom1=0x0061,
	awbMode_custom2=0x0062,
	awbMode_custom3=0x0063,
	awbMode_custom4=0x0064,
	awbMode_mspot=0x0080,
}awbFlowModeVal_t;

typedef enum{
	awbMode_preview=0x0000,
	awbMode_snap=0x1000,
	awbMode_avi=0x2000,
}awbProcModeVal_t;

typedef enum{
	awbMode_mutilSpotsWpAvg=0x0000,
	awbMode_allSpotsAvg=0x0100,
	awbMode_allPointsAvg=0x0200,
}awbArithmeticModeVal_t;
typedef enum{
	AWB_ALGORITHM_V1 = 0,
	AWB_ALGORITHM_V2,
}awbAlgorithm_t;

#define AWB_FLAG_EN 1
#if AWB_FLAG_EN
#define awbFlag_outdoor 		0x0100
#define awbFlag_lastWeighting	0x0200
#define awbFlag_spot5Weighting	0x0400
#define awbFlag_sward			0x0800
#define awbFlag_delay			0x1000

#define awbFlag_noEnoughPoints 	0x00ff
#define awbFlag_fixGain		 	0x00f1
#define awbFlag_lstGain		 	0x00f2
#define awbFlag_spot0			0x0000
#define awbFlag_spot4			0x0040
#define awbFlag_spot1			0x0010
#define awbFlag_spot5 			0x0050
#define awbFlag_spot2			0x0020
#endif

typedef struct
{
	UINT16 fileSize;
	UINT16 sysID;
	UINT16 fileVersion;
	UINT16 dataSize;
	UINT16 idxCTC;
	UINT16 idxParam;
	UINT16 idxCustom;
	UINT16 idxOther;

}awb_res_header_t;
typedef struct
{
	UINT16* data;
	awb_res_header_t* header;
	UINT16 *ctc;
	UINT16* param;
	UINT16* custom;
	UINT16* other;
}awb_res_t;

extern void awbDefaultInitCtt();
extern UINT8 awbDefaultFilter(UINT32 R,UINT32 G,UINT32 B);
extern void awbDefaultReset();
extern void awbDefaultDecider();
extern void awbDefaultDeciderPost();
extern void awbDefaultInfo(UINT8* pt, UINT32 w,UINT32 h,UINT32 size);
extern UINT16 awbDefaultModeSet(UINT16 mode);
#ifdef __cplusplus
}
#endif

#endif
