/**************************************************************************
 *
 *       Copyright (c) 2014 -2018 by iCatch Technology, Inc.
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
 *  
 *  History:
 *  SamWang 20140318  Update hal_front.h  basefw ver:12555  
 *
 **************************************************************************/
 
#ifndef _HAL_FRONT_H_
#define _HAL_FRONT_H_

#ifdef __cplusplus
extern "C" {
#endif


/*
 * hal_front.c
 */

/*=================================================================*/
#define HAL_FRONT_SYNCSIG_DONT_CHANGE 0x7FFFFFFF

UINT32 halFrontSyncSigCfgSet (UINT32 cfg, UINT32 val1, UINT32 val2);
UINT32 halFrontSyncSigCfgGet (UINT32 cfg, UINT32 *pval1, UINT32 *pval2);

typedef enum {
	FRONT_SYNC_MODE = 0x00000000,
	FRONT_SYNC_SIZE,
	FRONT_SYNC_HDVD_IN_POLAR,
	FRONT_SYNC_HDVD_OUT_POLAR,
	FRONT_SYNC_EDGE,
	FRONT_SYNC_OFFSET,
	FRONT_SYNC_LINE,
	FRONT_SYNC_DUMMY_PIXEL,
	FRONT_SYNC_FRAME,
	FRONT_SYNC_UPDATE, /*Value1 is halFrontSyncSigCfgUpdateSel_t*/
	FRONT_SYNC_HD_VD_DIFF,
	FRONT_SYNC_BYPASS_CDSP_VALID,
	/*hdvd before reshape pol, useful for vari blank yuv sensors*/
	FRONT_SYNC_HDVD_RESHAPE_POLAR,
	FRONT_SYNC_VALID_FMT, /*val1 is halFrontSyncValidFmt_t*/
	FRONT_SYNC_EXTHD_NUM,
	FRONT_SYNC_EXTHD_POL,
	FRONT_SYNC_HOFFSET0,
	FRONT_SYNC_HOFFSET2,
	FRONT_SYNC_HDIV_SHIFT,
	FRONT_SYNC_SIGGEN_EN,/*val1 is enable/disable  sync signal generator */
	FRONT_SYNC_SIGGEN_HD,/*val1 is Hblank val2 is Htotal */
	FRONT_SYNC_SIGGEN_VD,/*val1 is Vblank val2 is Vtotal */
	FRONT_SYNC_REF_SEL,  /*SPCA6350 reference type select*/
	FRONT_SYNC_YUV_DEC,  /*SPCA6350 yuv decode*/
	FRONT_SYNC_STREAM,	 /*Stream on/off */
	FRONT_SYNC_SENSEL,	 /*Dual sensor Selection */
	FRONT_SYNC_PIXELMUN, /*CDC pixel number */
	FRONT_SYNC_YUV_SEL, /*uv sel , yuv 128en*/
	FRONT_SYNC_FIFO_GATING, /*fifo gating control*/
	FRONT_SYNC_SIGGEN_VD_PAUSE,/*pause ssg VD, HD is normal  */
	FRONT_SYNC_TG_REPLACE,/*replace tg0 or tg 1*/
} halFrontSyncSigCfgSetSel_t;

void   halFrontClkCfg(UINT32 mclkmode, UINT32 pclkmode);
void   halFrontClkPhaseCfg(UINT32 masterphase, UINT32 pixelphase, UINT32 adckphase);

void   halFrontSiggenSet(UINT32 enable,UINT32 sigyuven,	UINT32 sentype,UINT32 mode); /* add sentype by stanliu */
void   halFrontProbeSet(UINT32 enable, UINT32 mode);

/*HAL_FRONT_GPIO*/
#define HAL_FRONT_GPIO_PWM_FUNC 	0x03
#define HAL_FRONT_GPIO_LMI_FUNC 	0x02
#define HAL_FRONT_GPIO_SIF_FUNC 	0x01
#define HAL_FRONT_GPIO_ORI_FUNC 	0x00

void halFrontGpioTgSifSel(UINT32 func,UINT32 mask);
void halFrontGpioVDHDSel(UINT32 master);
void halFrontGpioLMIFuncSel(UINT32 func,UINT32 mask);
void halFrontDataPinSelCfg(UINT32 sensel,UINT8  pinsel[12]);
void halFrontRGBPinEnCfg(UINT32 mask);/*RGB pin input enable*/


#define TG_CMOS_SP5K_SLAVE      0
#define TG_CMOS_SP5K_MASTER     1
#define TG_CCD_EXT_SP5K_SLAVE   2
#define TG_CCD_EXT_SP5K_MASTER  3
#define TG_TVIN_DEC             8

#define TG_BMAP_SP5K_MASTER  0x01
#define TG_BMAP_CCD_EXT      0x02
#define TG_BMAP_TVIN_DEC     0x08

/*=================================================================*/
UINT32 halFrontImgSrcCfgSet(UINT32 cfgSel, UINT32 Value);

typedef enum {
	FRONT_IMG_SRC_SET = 0x00000000,
	FRONT_IMG_SIGGEN_CFG, /* revoked */
	FRONT_IMG_SENSOR_BIT_WIDTH,
	FRONT_IMG_SENSOR_CAP_FIELD,
	FRONT_IMG_SENSOR_FIELD_SEL, /*sync bitmask 0x10*/
	FRONT_IMG_TV_CFG,
	FRONT_IMG_DIFF_GRAY_CODE_CFG,
	FRONT_IMG_SRC_IN_PCLK_EDGE,
} halFrontImgSrcCfgSetSel_t;

typedef enum {
	FRONT_IMG_SRC_SENSOR, /* sensor with normal AFE */
	FRONT_IMG_SRC_TV = 0x08,
} frontSensorSrc_t;

typedef enum {
	FRONT_IMG_SIGGEN_BAYER_GR = 0,
	FRONT_IMG_SIGGEN_BAYER_R  = 1,
	FRONT_IMG_SIGGEN_BAYER_B  = 2,
	FRONT_IMG_SIGGEN_BAYER_GB = 3,
	FRONT_IMG_SIGGEN_SONY     = 4,
} frontSiggenSentype_t;

typedef enum {
	FRONT_IMG_SENSOR_BIT_8 =0,
	FRONT_IMG_SENSOR_BIT_10 =1,
	FRONT_IMG_SENSOR_BIT_12 =3,
} frontSensorBit_t;

/*=================================================================*/
UINT32 halFrontSerialCfgSet(UINT32 cfgSel,  UINT32 Value);

typedef enum {
	FRONT_SSC_PHY_CFG = 0x00000000,
	FRONT_SPI_PHY_CFG,
	FRONT_SIF2_SPI_PHY_CFG, /* similar to FRONT_SPI_PHY_CFG */
	FRONT_SIF1_SYNC_CFG, 	/*6350 revoke*/
	FRONT_SIF2_SYNC_CFG, 	/*6350 revoke*/
	FRONT_SIF1_MODE_CFG,	/* directly set sif1 mode */
	FRONT_SIF2_MODE_CFG,	/* directly set sif2 mode */
	FRONT_SIF1_PIN_CFG, 	/* config SIF1 SSC PIN out */
	FRONT_SIF2_PIN_CFG, 	/* config SIF2 SSC PIN out */
	FRONT_SIF1_PIN_OFF_CFG, 	/* config SIF1 SSC PIN out */
	FRONT_SIF2_PIN_OFF_CFG, 	/* config SIF2 SSC PIN out */
	FRONT_SIF2_SEN_CFG, 	/* polarity & cs *//*6350 move to PIN CFG*/
	FRONT_SIF1_IDLE_CFG,	/*idle time between SSC/SPI*/
	FRONT_SIF2_IDLE_CFG,	/*idle time between SSC/SPI*/
	FRONT_SIF1_PIN_AUTO_OFF,/*Turn off this ,If need output the same data through different pin at the same time */
	FRONT_SIF2_PIN_AUTO_OFF,/*Turn off this ,If need output the same data through different pin at the same time */
	FRONT_SSC_CEN_POS,		/* start & stop cenpos */
	/*Add 6350 */
	/*backward compatible*/
	FRONT_SSC_SYNC_CFG = FRONT_SIF1_SYNC_CFG,
	FRONT_SPI_SYNC_CFG = FRONT_SIF1_SYNC_CFG,
} halFrontSerialCfgSetSel_t;

typedef enum {
	SENSOR_IRQ_SVC_VD,				/*0  int_vd_fall, HAL_FRONT_INT_PRI_RESHAPE_SOF*/
	SENSOR_IRQ_SVC_FRAME_END,		/*1  int_vvalid_end,HAL_FRONT_INT_PRI_RESHAPE_EOF*/
	SENSOR_IRQ_SVC_VVALID_END,		/*2  int_vvalid_end,HAL_FRONT_INT_PRI_RESHAPE_EOF*/
	SENSOR_IRQ_SVC_READOUT_END, 	/*3  int_cdsp_eof,HAL_CDSP_INT_CDSP1_SNAP_EOF,HAL_CDSP_INT_CDSP1_EOF_ORG*/
	SENSOR_IRQ_SVC_DRAM_END,		/*4  int_cdsp_eof,HAL_CDSP_INT_CDSP1_SNAP_EOF,HAL_CDSP_INT_CDSP1_EOF_ORG*/
	SENSOR_IRQ_SVC_CDSP_EOF,		/*5  int_cdsp_eof,HAL_CDSP_INT_CDSP1_SNAP_EOF,HAL_CDSP_INT_CDSP1_EOF_ORG*/
	SENSOR_IRQ_SVC_CDSP_EOF_FRC,	/*6  int_cdsp_eof_frc, HAL_DRAM_INT_CDSP1EOF_MPGSB*/
	SENSOR_IRQ_SVC_CDSP_SOF_FRC,	/*7  HAL_DRAM_INT_CDSP1SOF_MPGSB,sensor_svcid.c*/
	SENSOR_IRQ_SVC_CDSP_SOF,		/*8  HAL_CDSP_INT_CDSP1_SOF,sensor_svcid.c*/
	SENSOR_IRQ_SVC_PRECMD,			/*9  after HAL_FRONT_INT_PRI_RESHAPE_1ST_SOL 50 lines */
	SENSOR_IRQ_SVC_NEG_VD,			/*10 HAL_FRONT_INT_PRI_VD_ORG_FALL,sensor_svcid.c*/
	SENSOR_IRQ_SVC_CDSP_VD, 		/*11 int_vd_fall, HAL_FRONT_INT_PRI_RESHAPE_SOF*/
	SENSOR_IRQ_SVC_ORG_VD,			/*12 HAL_FRONT_INT_PRI_VD_ORG_RISE,sensor_svcid.c*/
	SENSOR_IRQ_SVC_FIFO_SOF,		/*13 HAL_FRONT_INT_PRI_FIFO_SOF,sensor_svcid.c*/
	SENSOR_IRQ_SVC_RESHAPE_EOF, 	/*14 HAL_FRONT_INT_PRI_RESHAPE_EOF,sensor_svcid.c*/
	SENSOR_IRQ_SVC_RESHAPE_DMY_EOL, /*15 HAL_FRONT_INT_PRI_RESHAPE_DMY_EOL,sensor_svcid.c*/
	SENSOR_IRQ_SVC_RESHAPE_SOF_FRC, /*16 HAL_FRONT_INT_PRI_RESHAP_VVALDINTV_SOF,sensor_svcid.c*/
	SENSOR_IRQ_SVC_RESHAPE_EOF_FRC, /*17 HAL_FRONT_INT_PRI_RESHAP_VVALDINTV_EOF,sensor_svcid.c*/
	SENSOR_IRQ_SVC_SNAP_EOF,		/*18 HAL_CDSP_INT_CDSP1_SNAP_EOF,sensor_svcid.c*/	
	SENSOR_IRQ_SVC_SSG_VD,			/*19 HAL_FRONT_INT_TG_VD_FALL,sensor_svcid.c*/ 
	SENSOR_IRQ_SVC_SSG_NEG_VD,		/*20 HAL_FRONT_INT_TG_VD_RISE,sensor_svcid.c*/ 	
	SENSOR_IRQ_SVC_SEC_ORG_VD,		/*21 HAL_FRONT_INT_SEC_VD_ORG_RISE,sensor_svcid.c*/
	SENSOR_IRQ_SVC_PRECMD2,			/*22 before SVC_FRAME_END for 50 lines */
	SENSOR_IRQ_SVC_CDSP2_EOF,		/*23  int_cdsp2_eof,,HAL_CDSP_INT_CDSP2_EOF_ORG*/
	SENSOR_IRQ_SVC_CDSP2_EOF_FRC,	/*24  int_cdsp2_eof_frc, HAL_DRAM_INT_CDSP2EOF_MPGSB*/
	SENSOR_IRQ_SVC_TOTAL,
} sensorIrqSvcId_t;

UINT32 sensor_frame_repeat_cb_reg(UINT32 svcid, void *f, UINT32 param);

#ifdef __cplusplus
}
#endif

#endif  /* _HAL_FRONT_H_ */
