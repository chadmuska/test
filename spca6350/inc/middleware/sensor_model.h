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
 *
 *  Author: Anmin Deng
 *
 **************************************************************************/
#ifndef _SENSOR_MODEL_H_
#define _SENSOR_MODEL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include "sensor_def.h"
#include "gpio_def.h"
/*>>sensor LVDS config*/

typedef struct {
	UINT16 sensel;/*0: primary 1: secondary 2: both*/
	UINT16 wordcntsel	   ;
	UINT16 Lvdslaneswap    ;
	UINT16 sofcarryline    ;/*Hispi => 1*/
	UINT16 Lvdsoutsel	   ;
	UINT16 Lvdseofsel	   ;
	UINT16 Lvdseolsel	   ;
	UINT16 Ctrlword_sol    ;/*lvds same cw*/
	UINT16 Ctrlword_sof    ;/*lvds same cw*/
	UINT16 Ctrlword_eol    ;/*lvds same cw*/
	UINT16 Ctrlword_eof    ;/*lvds same cw*/
	UINT16 Lvds_cw_sol[12] ;/*lvds diff cw*/
	UINT16 Lvds_cw_sof[12] ;/*lvds diff cw*/
	UINT16 Lvds_cw_eol[12] ;/*lvds diff cw*/
	UINT16 Lvds_cw_eof[12] ;/*lvds diff cw*/
	UINT16 syncwrod0	   ;
	UINT16 syncwrod1	   ;
	UINT16 syncwrod2	   ;
	UINT16 t;/*mipiRxBiasCurrent_t*/
	UINT16 rt_sel;/*mipiRtSel_t*/
	UINT16 dual_clock;
	UINT16 fifodectype; /*for dual clock channel, 0: [0,2,4,6] [1,3,6,7]  ,1: [0,1,2,3] [4,5,6,7]*/
	UINT16 samectrlword;
	UINT16 mipi;
	UINT16 spsel;
	UINT16 lvdslaneen;
	UINT16 settle;
	UINT16 eofsel;
	UINT16 wordwidth;
	UINT16 decpixelnum;
	UINT16 Ctrlword01_sol ;/*mipi*/
	UINT16 Ctrlword02_sol ;/*mipi*/
	UINT16 Ctrlword03_sol ;/*mipi*/
	UINT16 Ctrlword04_sol ;/*mipi*/
	UINT16 SenPinSelNum  ;
	UINT8  SenPinSel[12] ;
} sensorLVDSCfg_t;


/*>>sensor Sync Sig Gen config*/
typedef struct {
	UINT32 linetotal       : 16;
	UINT32 lineblank       : 16;
	UINT32 frametotal      : 16;
	UINT32 frameblank      : 16;
/*resahpe +++*/	
	UINT32 hreshape_fall   : 16;
	UINT32 hreshape_rise   : 16;
	UINT32 vreshape_fall   : 16;
	UINT32 vreshape_rise   : 16;
/*resahpe ---*/	
	UINT16 hoffset;
	UINT16 voffset;
	UINT16 hsize;
	UINT16 vsize;
	UINT32 lastlinepix     : 16;
	UINT32 hdvdopos        : 16;
	UINT16 conf_hdpol      : 1; /*hd 0:active low, 1:active hi*/
	UINT16 conf_vdpol      : 1; /*vd 0:active low, 1:active hi*/
	UINT16 conf_totalsync  : 1; /*obsolute*//*Xtotal/blank sync 0:reshape,1:orig vd*/
	UINT16 conf_hdpclkedge : 1; /*hd output at pclk 0: pos, 1: neg edge, pclk phase invert*/
	UINT16 conf_hvldbypass : 1; /*obsolute*/
	UINT16 conf_vvldbypass : 1; /*obsolute*/
	UINT16 frameblank_base : 1; /*obsolute*//*0:hd, 1:pixel*/ /*revoked, always in hd*/
	UINT16 vreshape_base   : 1; /*obsolute*//*0:hd, 1:pixel*/
	UINT16 conf_yuvhdpol   : 1; /*obsolute*//*yuvhd just before hdres 0:active low*/
	UINT16 conf_yuvvdpol   : 1; /*obsolute*//*yuvvd just before vdres 0:active low*/
	UINT16                 : 6;
	UINT16 hdnum_frame_start;/*obsolute*/
	UINT16 hdnum_frame_end;  /*obsolute*/
	UINT16 hdnum_cmdport;
	/*V35 new*/
	SINT16 hdnum_diff_precmd; /*different between cmdport and precmd signed*/
	UINT16 ext_hdnum;		  /*V35 extension line */
	UINT32 fpclk;             /*fpclk : for pclknx clock convert(linetotal)  unit (khz)*/
	UINT32 pclk1x;			  /*pclk1x: k(Hz) */
	UINT16 ssg_hblank; 		  
	UINT16 ssg_htotal;
	UINT16 ssg_vblank; 		  
	UINT16 ssg_vtotal;
	UINT16 ssg_vblank_base;
	UINT32 ssg_pclk;   /*ssg_pclk : for sync signal generator (master mode) */
	const sensorLVDSCfg_t *pLVDScfg;
	UINT16 lvds_hsize	   ;
	UINT16 lvds_vsize	   ;
} sensorSyncSigCfg_t;
typedef struct {
	UINT32 hoffset         : 16;
	UINT32 voffset         : 16;
	UINT32 hsize           : 16;
	UINT32 vsize           : 16;
} sensorSyncSigSizeCfg_t;

typedef struct {
	UINT16 hblank;
	UINT16 htotal;
	UINT16 vblank;
	UINT16 vtotal;
	UINT16 vblank_base;
	UINT32 pclk;
} sensorSyncSigGenSizeCfg_t;

typedef struct {
	UINT16 hdnum_frame_start;
	UINT16 hdnum_frame_end;
	UINT16 hdnum_cmdport;
	UINT16 hdnum_diff_precmd;	
} sensorSyncSigHdnumCfg_t;



typedef enum {
	SENSOR_SYNC_IMM,/*imm update*/
	SENSOR_SYNC_VVALID,/*sync to valid end*/
} sensorSyncOption_t;

void sensorSnapFldSelSet(UINT32 fldsel);
void sensorSyncSigInit(const sensorSyncSigCfg_t *pcfg);
void sensorSyncSigSet(const sensorSyncSigCfg_t *pcfg);
void sensorSyncSigAsyncSet(const sensorSyncSigCfg_t *pcfg,sensorSyncOption_t sync);
void sensorSyncSigGenInit(const sensorSyncSigGenSizeCfg_t *pcfg);
void sensorSyncExpSigInit(const sensorSyncSigGenSizeCfg_t *pscfg1,UINT32 v1_count,const sensorSyncSigGenSizeCfg_t *pscfg2,UINT32 v2_count,const sensorSyncSigCfg_t *pcfg,UINT32 v3_count);
void sensorSyncVsyncRestart(void);
void sensorSyncSigSizeSet(const sensorSyncSigSizeCfg_t *pcfg);
void sensorSyncSigStop(void);
void sensorSyncSigStart(void);
const sensorSyncSigCfg_t *sensorSyncSigGet();
void sensorSyncSigGenStop(void);
void sensorSyncSigClockCheck(void);/*fine tune linetotal if dram clock changes*/


/*Dual sensor +++*/
void sensorSecSyncSigInit(const sensorSyncSigCfg_t *pcfg);
void sensorSecSyncSigSizeSet(const sensorSyncSigSizeCfg_t *pcfg);
void sensorSecSyncSigStop(void);
void sensorSecSyncSigStart(void);
const sensorSyncSigCfg_t * sensorSecSyncSigGet();
void sensorSecSyncSigClockCheck(void);/*fine tune linetotal if dram clock changes*/


/*Dual sensor ---*/


void sensorSyncSigSizeSet(const sensorSyncSigSizeCfg_t *pcfg);
void sensorSyncSigHdnumSet(const sensorSyncSigHdnumCfg_t *pcfg);
void sensorSyncSigHdnumFrameEndCustomSet(	UINT32 frame_end_hdmun);
void sensorSyncSigReshapeModeSet(UINT32 mode);
void sensorLVDSCfgSet(const sensorLVDSCfg_t *pcfg);
const sensorLVDSCfg_t* sensorLVDSCfgGet(void);

void sensorLVDSEnSet(UINT32 enable);
void sensorLVDSPhaseCfg(UINT32 t,UINT32 rt_sel,UINT32 phase[13]);


/*MODE:  MODE_CCIR601 / MODE_CCIR656 */
/*UVSEL:  UVSEL_CB / UVSEL_Y1 / UVSEL_CR/ UVSEL_Y2 */
/*YUVrange(128~-128): If range is 0~255, turn on SENSOR_RANGE_YS128|SENSOR_RANGE_US128|SENSOR_RANGE_VS128 or .....*/

typedef enum {
	SENSOR_MODE_CCIR601,
	SENSOR_MODE_CCIR656,
	SENSOR_MODE_TOTAL,
} sensorYUVmode_t;

/*UVSel*/
typedef enum {
	SENSOR_UVSEL_0, /*U0 Y0 V0 Y1 */
	SENSOR_UVSEL_1, /*Y0 V0 Y1 U0 */
	SENSOR_UVSEL_2, /*V0 Y0 U0 Y1 */
	SENSOR_UVSEL_3, /*Y0 U0 Y1 V0 */
	SENSOR_UVSEL_4, /*U0 Y1 V0 Y0 */
	SENSOR_UVSEL_5, /*Y1 V0 Y0 U0 */
	SENSOR_UVSEL_6, /*V0 Y1 U0 Y0 */
	SENSOR_UVSEL_7, /*Y1 U0 Y0 V0 */
	SENSOR_UVSEL_TOTAL,
} sensorUVsel_t;

/*YUVrange*/
typedef enum {
	SENSOR_RANGE_YS128 =1,
	SENSOR_RANGE_US128 =2,
	SENSOR_RANGE_VS128 =4,
} sensorYUVrange_t;

void sensorYUVCfgSet(UINT32 YUVmode, UINT32 UVsel, UINT32 YUVrange);
void sensorYUVEnSet(UINT32 enable);
void sensorSecYUVCfgSet(UINT32 YUVmode, UINT32 UVsel, UINT32 YUVrange);
void sensorSecYUVEnSet(UINT32 enable);

/*<<sensor Sync Sig Gen config*/


/*>>sensor TG config*/
typedef enum {
	SENSORMODEL_0, SENSORMODEL_1, SENSORMODEL_2, SENSORMODEL_3,
	SENSORMODEL_4, SENSORMODEL_5, SENSORMODEL_6, SENSORMODEL_7,
	SENSORMODEL_8, SENSORMODEL_9,
} sensorModelType_t;

typedef enum {
	TYPE_CMOS          = SENSORMODEL_0,
	TYPE_SHARPCCD      = SENSORMODEL_0,
	TYPE_SONYCCD       = SENSORMODEL_0,
	TYPE_PANACCD       = SENSORMODEL_0,
	TYPE_SONYNEWCCD    = SENSORMODEL_1, /*Z-TYPE*/
	TYPE_SHARPNEWCCD   = SENSORMODEL_1, /*Z-TYPE*/
	TYPE_PANANEWCCD    = SENSORMODEL_3, /*PANA VGA -TYPE*/
	TYPE_SONY2DIVCCD   = SENSORMODEL_7,
	TYPE_SONY3DIVCCD   = SENSORMODEL_4,
	TYPE_SONY4DIVCCD   = SENSORMODEL_5,
	TYPE_SONY4DIVNEWCCD= SENSORMODEL_2,/*SONY NEW  4-DIV TYPE ICX 682*/
} sensorModel_t;

typedef enum {
	SENSORMODEL_UNKNOWN			=0x00, /*unknow type for backward compatible */
	SENSORMODEL_RAW				=0x01, 
	SENSORMODEL_YUV				=0x02,	
	SENSORMODEL_MIPI_RAW		=0x03, 
	SENSORMODEL_MIPI_YUV		=0x04, 
	SENSORMODEL_LVDS_RAW		=0x05, 
	SENSORMODEL_LVDS_YUV		=0x06, 
	SENSORMODEL_YUV_BT656		=0x07,	
	SENSORMODEL_YUV_BT1120		=0x08, 
	SENSORMODEL_MIPI_RAW_YUV	=0x09,	
	SENSORMODEL_DUAL_RAW		=0x10|SENSORMODEL_RAW, 
	SENSORMODEL_DUAL_YUV		=0x10|SENSORMODEL_YUV, 
	SENSORMODEL_DUAL_MIPI_RAW	=0x10|SENSORMODEL_MIPI_RAW, 
	SENSORMODEL_DUAL_MIPI_YUV	=0x10|SENSORMODEL_MIPI_YUV, 
	SENSORMODEL_DUAL_LVDS_RAW	=0x10|SENSORMODEL_LVDS_RAW, 
    SENSORMODEL_FAKE_RAW        =0x20|SENSORMODEL_RAW,
    SENSORMODEL_FAKE_YUV        =0x20|SENSORMODEL_YUV,
	SENSORMODEL_TOTAL
} sensorModelInfo_t;


typedef enum {
	BAYER_GRBG, BAYER_RGGB, BAYER_BGGR, BAYER_GBRG,
	BAYER_TOTAL,
} sensorCfaPattern_t;

#define FRONT_RESHAPE_AUTO_MODE   0
#define FRONT_RESHAPE_MANUAL_MODE 1

/*already in hal_front.h but not opened to api +++*/
#ifndef _HAL_FRONT_H_

typedef enum {
	FRONT_IMG_SRC_SENSOR, /* sensor with normal AFE */
	FRONT_IMG_SRC_SENSOR_GRAY,
	FRONT_IMG_SRC_SENSOR_DIFF,
	FRONT_IMG_SRC_SENSOR_DIFF_GRAY,
	FRONT_IMG_SRC_TV = 0x08,
} frontSensorSrc_t;

/* when front image src is TV decoder in (FRONT_IMG_SRC_TV),
 * add these bitmap to subtract 128 from Y, U, V values.
 * for example,
 * .img_src_type = FRONT_IMG_SRC_TV + TV_LEVEL_SHIFT_U + TV_LEVEL_SHIFT_V
 */
#define  TV_LEVEL_SHIFT_Y  1
#define  TV_LEVEL_SHIFT_U  2
#define  TV_LEVEL_SHIFT_V  4

typedef enum {
	FRONT_IMG_SENSOR_BIT_8 =0,
	FRONT_IMG_SENSOR_BIT_10 =1,
	FRONT_IMG_SENSOR_BIT_12 =3,
} frontSensorBit_t;

#define TG_CMOS_SP5K_SLAVE      0
#define TG_CMOS_SP5K_MASTER     1
#define TG_CCD_EXT_SP5K_SLAVE   2
#define TG_CCD_EXT_SP5K_MASTER  3
#define TG_TVIN_DEC             8

#define TG_BMAP_SP5K_MASTER  0x01
#define TG_BMAP_CCD_EXT      0x02
#define TG_BMAP_TVIN_DEC     0x08

typedef enum {   /* CPU clock = 48MHz */
FRONT_SSC_CLK_24K = 0,
FRONT_SSC_CLK_48K,
FRONT_SSC_CLK_96K,
FRONT_SSC_CLK_192K,
FRONT_SSC_CLK_384K,
FRONT_SSC_CLK_768K,
FRONT_SSC_CLK_1500K,
FRONT_SSC_CLK_3M
} sensorSerialSscClk_t;
#endif
/*already in hal_front.h but not opened to api ---*/

#undef FRONT_SSC_RESTART_ON
#undef FRONT_SSC_READ_HAS_SUBADDR
#define FRONT_SSC_RESTART_ON		0x02
#define FRONT_SSC_READ_HAS_SUBADDR	0x40


/*
V35 2013.12.10
For data rate < 700Mbps	:  FRONT_RX_BIAS_8UA
For data rate 700~900Mbps	:  FRONT_RX_BIAS_10UA
For data rate > 900Mbps	:  FRONT_RX_BIAS_12UA
*/

typedef enum {	 /* 8uA~26uA */
	FRONT_RX_BIAS_26UA =0x10,/*10000*/
	FRONT_RX_BIAS_24UA =0x11,/*10001*/
	FRONT_RX_BIAS_22UA =0x12,/*10010*/
	FRONT_RX_BIAS_20UA =0x13,/*10011*/
	FRONT_RX_BIAS_18UA =0x18,/*11000*/
	FRONT_RX_BIAS_16UA =0x19,/*11001*/
	FRONT_RX_BIAS_14UA =0x1C,/*11010*/
	FRONT_RX_BIAS_12UA =0x1D,/*11101*/
	FRONT_RX_BIAS_10UA =0x1E,/*11110*/
	FRONT_RX_BIAS_8UA  =0x1F,/*11111*/
} mipiRxBiasCurrent_t;

typedef enum {	 /*Termination Resister */
	FRONT_RT_SEL_R0   =0x10,/*10000*/
	FRONT_RT_SEL_R1   =0x11,/*10001*/
	FRONT_RT_SEL_R2   =0x12,/*10010*/
	FRONT_RT_SEL_R3   =0x13,/*10011*/
	FRONT_RT_SEL_R4   =0x14,/*10100*/
	FRONT_RT_SEL_R5   =0x15,/*10101*/
	FRONT_RT_SEL_R6   =0x16,/*10110*/
	FRONT_RT_SEL_R7   =0x17,/*10111*/
} mipiRtSel_t;

typedef struct {
	UINT8 tg_type; /*CMOS/CCD + ITG/ XTG_SSG_TG/ XTG_SSG_DSP*/
	UINT8 n_preview_submode; /*preview, af, and other front digi zoom*/
	UINT8 n_snap_submode; /*snap front digi zoom*/
	UINT8 vshutter_need_resend; /*sony FALSE;panasonic/ITG TRUE*/
	/*how many evset sync exp takes to effect at readout*/
	/*0=always valid (post wb);1=effect at next readout;...*/
	UINT8 exp_effect_frame_count; /*CCD typically 2*/
	UINT8 cmdportid_xtg; /*ITG == undefined*/
	UINT8 cmdportid_exp; /*ITG == undefined*/
	/*anmin-20090514 some CMOS must have preview frames between snap*/
	UINT8 n_snap_pv_delay;
	UINT8 conf_has_abort; /*if TG supports stream off*/
	UINT8 cmdportid_xtg2; /*for secondary sensor */
} sensorTgCfg_t;

typedef struct {
	void (*resload)(void);
	void (*resstop)(void);
	void (*init)(void);
	void (*stop)(void);
	void (*pwrsave)(UINT32 mode);
	void (*expwrite)(UINT32 nreg,const UINT8 *pdata);
	void (*exp2write)(UINT32 nreg,const UINT8 *pdata);
	void (*expvtbldo)(UINT32 vtype,UINT32 vcount);
	void (*crop)(UINT32 hoff,UINT32 voff,UINT32 hsize,UINT32 vsize);
	UINT32 (*devcfg)(UINT32 cfg,UINT32 val); /*obsolete*/
	/** \param abort: 1=auto, 2=manual */
	void (*streamoff)(UINT32 abort);
	void (*streamon)(UINT32 abort);
	void (*cmdportBegin)(UINT32 smode,UINT32 expIdx,UINT32 agcIdx,UINT32 exp2Idx,UINT32 agc2Idx);
	void (*cmdportEnd)(void);
	void (*expwriteRedo)(UINT32 smode,SINT32 *expIdx,UINT32 *vcount,UINT8 *vtbl_type,UINT32 *int_us);
	UINT32 (*senstschk)(void);/*check sensor sts*/
	UINT32 (*regSeqCtrl)(UINT32 id);/*control sensor powerup sequence */
	void (*frmwrite)(UINT32 smode,UINT32 kfps_max,UINT32 kfps_min,UINT32 nreg,const UINT8 *pdata);
	UINT32 (*connect)(void);
} sensorTgSvc_t;

typedef void (*sensorDoFunc_t)(void);

typedef struct {
	sensorDoFunc_t previewBegin;
	sensorDoFunc_t previewShortExp;
	sensorDoFunc_t previewLongExpBegin;
	sensorDoFunc_t previewLongExpFollow;
	sensorDoFunc_t previewLongExpEnd;
	sensorDoFunc_t previewExpEnd;
	void (*previewNext)(UINT32 next_mode);
} sensorPreviewDo_t;

typedef struct {
	sensorDoFunc_t *psnapEarly;
	sensorDoFunc_t snapExpBegin;
	sensorDoFunc_t snapShortExp;
	sensorDoFunc_t snapBshutterBegin;
	sensorDoFunc_t snapBshutterFollow;
	sensorDoFunc_t snapBshutterEnd;
	sensorDoFunc_t snapLongExpBegin;
	sensorDoFunc_t snapLongExpFollow;
	sensorDoFunc_t snapLongExpEnd;
	sensorDoFunc_t snapExpEnd;
	sensorDoFunc_t *psnapDummy;
	sensorDoFunc_t *psnapReadoutField;
	sensorDoFunc_t snapFieldEnd;
	sensorDoFunc_t snapReadoutEnd;
	void (*snapNext)(UINT32 next_mode);
	sensorDoFunc_t snapAbort;
} sensorSnapDo_t;

/*if crop_type is 0, supporting dims (for hoff,voff,hsize,vsize) are..
 * supporting dim = dim_scale x N + dim_offset;
 * where N = dim_min .. dim_max;
 *else if crop_type is 1, supporting dims are listed in the series
 */
typedef struct {
	UINT16 crop_type : 8; /*0:this struct;1:series of supporting numbers*/
	UINT16 n_entry   : 8; /*crop_type==1,how many numbers in the series*/
	UINT16 dim_scale;
	UINT16 dim_offset;
	UINT16 dim_idxmin;
	UINT16 dim_idxmax;
} sensorTgCropDim_t;
typedef struct {
	const UINT16 xOffset; 
	const UINT16 yOffset;
	const UINT16 xSize;
	const UINT16 ySize;
	const UINT16 hratio;
	const UINT16 vratio;
	const UINT16 yuvW;
	const UINT16 yuvH;
} sensorTgCrop_t;

typedef struct {
	void (*init)(void);
	void (*modeswBegin)(UINT32 s0_mode,UINT32 s1_mode);
	void (*modeswNext)(UINT32 s0_next_mode,UINT32 s1_next_mode);
} sensorDualSvc_t;


typedef struct {
	const sensorPreviewDo_t *fpdo;
	const sensorTgCrop_t *crop;
	UINT16 hsize;
	UINT16 vsize;
	UINT8  sensor_model; /*CMOS,Sony,FFM,SonyVGA,etc*/
	UINT8  cfa_pattern; /*bayer rggb, grbg,..., ffm honey, strip, etc*/
	UINT8  model_info;/*SPCA5330 add */
	UINT8  fhscale;/*SPCA5330 add */
	UINT16 pix_100khz;
	UINT16 hratio;
	UINT16 vratio;
	const sensorTgDummy_t *pdummy; /* FIXME: is it okay here? */
	const sensorTgWin_t *pobwin[2]; /* for V35 auto OB , 2 OB window*/
	UINT32 fps_mhz; /* fps * 1000000 */
	UINT32 sensor_pclk;  /*Unit:Hz , sensor pixel clock for auto banding caculation*/
	UINT32 sensor_htotal;/* sensor pixel clock per line  for auto banding caculation*/
	UINT8  max_view_flag;/*flag to check if it is max view size*/
	UINT32 bayerhs_size;/*Bayer Hscale size*/
	const sensorTgBinCor_t *pbincorr;/*bining correction */
} sensorTgPreviewCfg_t;

typedef struct {
	const sensorSnapDo_t *fpdo;
	const sensorTgCrop_t *crop;
	UINT16 hsize;
	UINT16 vsize;
	UINT8  sensor_model; /*CMOS,Sony,FFM,SonyVGA,etc*/
	UINT8  cfa_pattern; /*bayer rggb, grbg,..., ffm honey, strip, etc*/
	UINT8  n_dummy;
	UINT8  n_readout;
	UINT8  n_capvd; /*obsolete, left for backward compatible*/
	UINT8  n_snap_early;
	/** \member .skip_snap_readout_vd:
	 * 0: typical snap flow with early,exp,dummy,ro frames, all have VDs;
	 * 1: CMOS frame snap, no VD btwn exp to ro, has VD after exp GPIO trg;
	 * 2: CMOS frame snap, no VD btwn exp to ro, no VD after exp trg;
	 * 3: CMOS rolling consecutive snap (snap readout->snap readout)
	 * 4: CMOS no hd in snap exp frame
	 */
	UINT8  skip_snap_readout_vd;
	UINT8  fldsel_start;
	UINT8  fldsel_end;
	UINT8  rsv[1];
	UINT8  op_mode; /*Normal mode.  Burst snap mode.*/
	UINT8  model_info;/*SPCA5330 add */
	UINT8  fhscale;/*SPCA5330 add */
	UINT16 pix_100khz;
	UINT16 hratio;
	UINT16 vratio;
	const sensorTgWin_t *pobwin[2]; /* for V35 auto OB , 2 OB window*/
	UINT32 sensor_pclk; 	/*Unit:kHz , sensor pixel clock for auto banding caculation*/
	UINT32 sensor_htotal;	/* sensor pixel clock per line	for auto banding caculation*/
	const sensorTgFieldSel_t *pfieldsel;
} sensorTgSnapCfg_t;

typedef struct {
	sensorTgCfg_t *tgCfg;
	const sensorTgSvc_t *tgSvc;
	const sensorTgPreviewCfg_t **previewCfg;
	const sensorTgSnapCfg_t **snapCfg;
	const sensorSyncSigCfg_t *defSyncSigCfg;
	UINT32 rsv[2];
	const char *verStr;
} sensorTgDev_t;

typedef struct {
	const sensorDualSvc_t *Svc;
	const char *verStr;
} sensorDualDev_t;


typedef enum {
	SENSOR_MAIN=0,
	SENSOR_BK1,
	SENSOR_BK2,
} sensorListId_t;

void sensorTgDevInstall(UINT32 devid,const sensorTgDev_t *pdev);
/*<<sensor TG config*/


/*>>sensor AFE config*/
/**
 * \a .img_src_type:  one of frontSensorSrc_t,
 *     in case of FRONT_IMG_SRC_TV, adds on bitmaps TV_LEVEL_SHIFT_Y/_U/_V;
 * \a .img_src_cfg:   config img src according to .img_src_type,
 *     if .img_src_type is FRONT_IMG_SRC_TV, .img_src_cfg bits defined as
 *     bit[1:0] uvsel: 0=Cb, 1=Y1, 2=Cr, 3=Y2;
 *     bit[2]   selffielden: to generate 601 mode field signal (5110 revoked)
 *     bit[3]   fieldpol:  invert field signal to CDSP (5210 revoked)
 *     bit[4]   bt656en:   0=601 mode, 1=656 mode;
 *     bit[6:5] tvsyncen:  0=TV decoding v:hvalid, 3=SSG v:hvalid;
 *     bit[7]   fldsvden:  field signal transition sync to VD; (5110 revoked)
 */
typedef struct {
	/******TODO*****/
	UINT8 img_src_type; /*sensor:normal,gray,diff:int;siggen;tv:yuvS*/
	UINT8 img_src_cfg; /*cfg for gray,diff,tv*/
	UINT8 nbit_rgb; /**< one of frontSensorBit_t */
	/*how many evset sync afe takes to effect at readout*/
	/*0=always valid (post wb);1=effect at next readout;...*/
	UINT8 afe_effect_frame_count; /*CCD typically 1*/
	/*agc range*/
	/*dcoffset range*/
	UINT8 cmdportid_afe;
	UINT8 cmdportid_agc;
	UINT8 cmdportid_dcoffset;
	UINT32 rsv[2];
} sensorAfeCfg_t;

typedef struct {
	void (*resload)(void);
	void (*resstop)(void);
	void (*init)(void);
	void (*stop)(void);
	void (*pwrsave)(UINT32 mode);
	void (*agcwrite)(UINT32 nreg,const UINT8 *pdata);
	void (*agc2write)(UINT32 nreg,const UINT8 *pdata);
	void (*agcetbldo)(UINT32 sv_error, UINT32 err_unit );
	void (*dcoffsetwrite)(UINT32 idx);
	UINT32 (*devcfg)(UINT32 cfg,UINT32 val); /*obsolete*/
} sensorAfeSvc_t;

typedef struct {
	/******TODO*****/
	/*analog separate channel gain, range*/
	/*analog separate channel dcoffset, range*/
	/*cfa pattern matching with sensor cfa if separate channel available*/
} sensorAfeRGGB_t;

typedef struct {
	sensorAfeCfg_t *afeCfg;
	const sensorAfeSvc_t *afeSvc;
	const sensorAfeRGGB_t *afeRGGB; /*NULL if not supporting RGGB afe*/
	UINT32 rsv[4];
	const char *verStr;
} sensorAfeDev_t;

void sensorAfeDevInstall(UINT32 devid,const sensorAfeDev_t *pdev);

/*<<sensor AFE config*/

/*sensor Dual config >>*/
void sensorDualDevInstall(const sensorDualDev_t *pddev);
/*<<sensor Dual config*/

/*>>sensor timer shutter config*/
/*MUST call before sensor_exptbl_init(); if device supports bshut*/
void sensor_bshut_init(void);
void sensor_bshut_delete(void);
void sensor_bshut_vcount_offset_set(UINT32 us);	
UINT32 sensor_bshut_vcount_offset_get(void);
void sensor_snap_msh_delay_cfg_get(UINT32 cfg, UINT32 *frame, UINT32 *delay);

/*<<sensor timer shutter config*/

/*>>sensor ev table config*/
UINT32 sensor_exptbl_init(void);
UINT32 sensor_exptbl_ext_init(UINT8 sensorNo);
UINT32 sensor_agctbl_init(void);
UINT32 sensor_agctbl_ext_init(UINT8 sensorNo);
void sensor_exptbl_delete(void);
void sensor_agctbl_delete(void);
/*for testing only*/
UINT32 sensor_exptbl_create(const UINT8 *pbuf,UINT32 fsize);
UINT32 sensor_agctbl_create(const UINT8 *pbuf,UINT32 fsize);
/*for exptbl remap mode*/
void sensor_exptbl_remap_cfg(UINT32 mode,UINT8 *lut, UINT32 size);
/*for exptbl index0 unit cfg*/
void sensor_exptbl_offset_cfg(UINT32 enable,UINT8 prev_idx0,UINT8 snap_idx0);
/*for agctbl index0 unit cfg*/
void sensor_agcptbl_offset_cfg(UINT32 enable,UINT8 idx0);
/*<<sensor ev table config*/


/*<<sensor ev table config*/


/*>>sensor pre-gain setting*/
void sensorPreGainEnSet( UINT32 on);
void sensorPreGainSvIdxCfg(UINT32 sverror);
void sensorPreGainCfg( UINT32 gain);
void sensorObBaseGainCfg( UINT32 gain);
void sensorSnapPreGainCfg( UINT32 gain);
UINT32 sensorPreGainEnGet(void);
UINT32 sensorPreGainGet(void);
UINT32 sensorSnapPreGainGet(void);
/*<<sensor pre-gain setting*/

/* \brief if stream off, sensor model driver to call this API to
 * turn on streaming after mode serial code sent */
void sensor_mode_abort_stream_on(void);


/*cmdport and serial port>>>*/
#define SENSOR_CMDPORTID_UNDEFINED 0xff

/*
V35				
<FORMAT3>
Bit 7										
SENSOR_SERIAL_FORMAT3_SPI_SIF2		
Bit6	
SENSOR_SERIAL_FORMAT3_SPI_SCK_IDLE_HI	
Bit [0:5]
GPIOPIN GROUP	
		
<FORMAT2>
SENSOR_SERIAL_FORMAT_SSC_SIF2
Bit 7	
SENSOR_SERIAL_FORMAT_SSC_RDSUB
Bit 6
SENSOR_SERIAL_FORMAT_SSC_RESTART
Bit 5
Bit[0:4]	
GPIOPIN CROUP	

<MODE>
Bit [4:5]	
SENSOR_SERIAL_MODE_SPI_MODE_MASK
Bit [0]
SENSOR_SERIAL_MODE_SPI
*/



#define SENSOR_SERIAL_FORMAT_TYPE_MASK 			0x20
#define SENSOR_SERIAL_FORMAT_PORT_MASK 			0x10
#define SENSOR_SERIAL_FORMAT_SPI_GPIO_MASK 		0x3F
#define SENSOR_SERIAL_FORMAT_SSC_GPIO_MASK		0x1F


/*BIT[5]: SSC/SPI BIT[4]:SIF1/SIF2   BIT[0:3] GPIO GRP*/
/*SSC  PINOUT*/ 																													          
/*,SCK,SDA*/
#define SENSOR_SERIAL_FORMAT2_SIF1_PIN_TG0			0x01  /*FRONT_SSC_SIF1_PIN_TG0	TG 14,15*/ 	
#define SENSOR_SERIAL_FORMAT2_SIF1_PIN_TG1			0x02  /*FRONT_SSC_SIF1_PIN_TG1	TG 12,13*/ 	
#define SENSOR_SERIAL_FORMAT2_SIF1_PIN_TG2			0x03  /*FRONT_SSC_SIF1_PIN_TG2	TG 18,19*/ 	
#define SENSOR_SERIAL_FORMAT2_SIF1_PIN_LMI			0x04  /*FRONT_SSC_SIF1_PIN_LMI	LMI 12,13*/ 	
													                                                          
#define SENSOR_SERIAL_FORMAT2_SIF2_PIN_TG0			0x11  /*FRONT_SSC_SIF2_PIN_TG0	TG 0,1*/	
#define SENSOR_SERIAL_FORMAT2_SIF2_PIN_TG1			0x12  /*FRONT_SSC_SIF2_PIN_TG1	TG 12,13*/	
#define SENSOR_SERIAL_FORMAT2_SIF2_PIN_TG2			0x13  /*FRONT_SSC_SIF2_PIN_TG2	TG 16,17*/	
#define SENSOR_SERIAL_FORMAT2_SIF2_PIN_LMI			0x14  /*FRONT_SSC_SIF2_PIN_LMI	LMI 8,9*/	
#define SENSOR_SERIAL_FORMAT2_SIF2_PIN_RGB			0x15  /*FRONT_SSC_SIF2_PIN_RGB	RGB 6,7*/	
#define SENSOR_SERIAL_FORMAT2_SIF2_PIN_HDMI			0x16  /*FRONT_SSC_SIF2_PIN_HDMI	HDMI */	
#define SENSOR_SERIAL_FORMAT2_SIF2_PIN_USB			0x17  /*FRONT_SSC_SIF2_PIN_USB	USB */  
                                                                                    
                                                                                    
/*SPI PINOUT*/                                                                      
/*SEN,SCK,SDO,SDI*/
#define SENSOR_SERIAL_FORMAT3_SIF1_PIN_TG0			0x21  /*FRONT_SPI_SIF1_PIN_TG0		TG 12,14,15*/
#define SENSOR_SERIAL_FORMAT3_SIF1_PIN_TG1			0x22  /*FRONT_SPI_SIF1_PIN_TG1		TG 4,14,15*/
#define SENSOR_SERIAL_FORMAT3_SIF1_PIN_TG0_SDI		0x23  /*FRONT_SPI_SIF1_PIN_TG0_SDI	TG 12,14,15,(13)*/
#define SENSOR_SERIAL_FORMAT3_SIF1_PIN_TG1_SDI		0x24  /*FRONT_SPI_SIF1_PIN_TG1_SDI	TG 4,14,15,(6)*/
#define SENSOR_SERIAL_FORMAT3_SIF1_PIN_LMI			0x25  /*FRONT_SPI_SIF1_PIN_LMI		LMI 15,12,13*/
#define SENSOR_SERIAL_FORMAT3_SIF1_PIN_LMI_SDI		0x26  /*FRONT_SPI_SIF1_PIN_LMI_SDI	LMI 15,12,13,(14)*/
																																		                
#define SENSOR_SERIAL_FORMAT3_SIF2_PIN_TG0		 	0x31  /*FRONT_SPI_SIF2_PIN_TG0		TG 3,0,1*/
#define SENSOR_SERIAL_FORMAT3_SIF2_PIN_TG1		 	0x32  /*FRONT_SPI_SIF2_PIN_TG1		TG 3,0,1,(2)*/
#define SENSOR_SERIAL_FORMAT3_SIF2_PIN_TG0_SDI 		0x33  /*FRONT_SPI_SIF2_PIN_TG0_SDI	TG 19,16,17*/
#define SENSOR_SERIAL_FORMAT3_SIF2_PIN_TG1_SDI 		0x34  /*FRONT_SPI_SIF2_PIN_TG1_SDI	TG 19,16,17,(18)*/
#define SENSOR_SERIAL_FORMAT3_SIF2_PIN_LMI		 	0x35  /*FRONT_SPI_SIF2_PIN_LMI		LMI 11,8,9*/
#define SENSOR_SERIAL_FORMAT3_SIF2_PIN_LMI_SDI 		0x36  /*FRONT_SPI_SIF2_PIN_LMI_SDI	LMI 11,8,9,(10)*/
#define SENSOR_SERIAL_FORMAT3_SIF2_PIN_RGB		 	0x37  /*FRONT_SPI_SIF2_PIN_RGB		RGB 21,6,7*/
#define SENSOR_SERIAL_FORMAT3_SIF2_PIN_RGB_SDI 		0x38  /*FRONT_SPI_SIF2_PIN_RGB_SDI	RGB 21,6,7,(20)*/
#define SENSOR_SERIAL_FORMAT3_SIF2_PIN_LCD		 	0x39  /*FRONT_SPI_SIF2_PIN_LCD		LCD 13,14,15*/
#define SENSOR_SERIAL_FORMAT3_SIF2_PIN_LCD_SDI 		0x3A  /*FRONT_SPI_SIF2_PIN_LCD_SDI	LCD 13,14,15,(17)*/



#define SENSOR_SERIAL_MODE_SPI_SENSEL 0x80 /********TODO********/
#define SENSOR_SERIAL_MODE_SPI_SENGPIO 0x40 /********TODO********/
#define SENSOR_SERIAL_MODE_SPI_MODE_MASK 0x30 /********TODO********/
#define SENSOR_SERIAL_MODE_SPI_MODE0 0x00 /********TODO********/
#define SENSOR_SERIAL_MODE_SPI_MODE1 0x10 /********TODO********/
#define SENSOR_SERIAL_MODE_SPI_MODE2 0x20 /********TODO********/
#define SENSOR_SERIAL_MODE_SPI_MODE3 0x30 /********TODO********/
#define SENSOR_SERIAL_MODE_SPI 0x01 /********TODO********/
#define SENSOR_CMDPORTID_OP_SEQ 0x40 /********TODO********/
#define SENSOR_CMDPORTID_OP_BURST 0x80 /********TODO********/

#define SENSOR_SERIAL_FORMAT_SPI_SENIDLE   0x80 /********TODO********/
#define SENSOR_SERIAL_FORMAT_SPI_SENOPEN   0x40 /********TODO********/
#define SENSOR_SERIAL_FORMAT_SPI_SENACTIVE 0x20 /********TODO********/
#define SENSOR_SERIAL_FORMAT_SPI_SENCLOSE  0x10 /********TODO********/

#define SENSOR_SERIAL_FORMAT_SSC_SIF2      0x80 /********TODO********/
#define SENSOR_SERIAL_FORMAT3_SPI_SIF2     0x80 /********TODO********/
#define SENSOR_SERIAL_FORMAT2_SSC_SIF2_LMI 0x00 /********TODO********/
#define SENSOR_SERIAL_FORMAT3_SPI_SCK_IDLE_HI 0x40 /********TODO********/
#define SENSOR_SERIAL_FORMAT_SSC_RESTART   0x20 /********TODO********/
#define SENSOR_SERIAL_FORMAT_SSC_RDSUB     0x40 /********TODO********/

/*UINT8 port_id is dynamic allocated by sensor_serial.c::sensorSerialCreate()
 * as SENSOR_CMDPORTID_XTG,XTG_SEQ,AFE,AFE_SEQ,MOTOR,LCD,AUDIO,UCON,etc
 * were actually in sensor{Tg/Afe}Cfg.cmdportid_{xtg,exp,afe,agc,dcoffset}
 */
/*structure of physical definitions
 * mode:[0]H spi/L ssc
 *   ssc [7:1]slave addr
 *   spi [7]sensel;[6]sengpio;[5:4]spi type;[3:1]cdsini(*5110*reserved 0*);
 *     ([7:6]==LL:sensel1,HL:sensel2,LH:sengpio,HH:no sen pin)
 * ssc (mode[0]==L)
 *   format2:[1:0]readCtr(no [4] go)
 *     [5,4,3,2] reserved 0 for hal front sif config usage;
 *     (*5110*[0]rdSubEn;[1]restart;[6]slaveaddr 16b*5210 revoked*;[7]sif1/2*)
 *     (*5210*if [7]==H, sif2 only, [6]L:SIF2 on TG, H:SIF2 on LMI)
 *   format3:(stopdelay and clkien??)(*5110*slaveaddr hi*, 5210 revoked)
 * spi (mode[0]==H)
 *   format2:(mode[6]==H)
 *     sensel gpio [7:4]sensel H/L(idle,open,active,close);[3:0]gpio_grp
 *   format3:
 *     [7]sif1/2;
 *     \limit when format3[7]==H, ie SIF2, SPI on SIF2 must use LMI pins;
 *     [6]idle level of SCK;
 *     [5:0](mode[6]==H only)sensel gpio gpio_pin
 * priority
 *   0 exclusive, flush any specific time and predefined sync time (tg,afe)
 *   1 (msh lens)
 *   2 delay flush when sensor active, flush any specific time otherwise
 */
typedef struct { /*ITG has this struct NULL*/
	UINT8 mode;
	UINT8 format2;
	UINT8 format3;
	UINT8 speed;
	UINT8 priority; /*always 0 for sensor(TG,AFE)*/
	UINT8 nbit_regdata;
	UINT8 nbit_regaddr;
	UINT32 speed_khz;
} sensorCmdport_t;

#define D_CACHE_LINE_SIZE   32
/** \note to support SIF1DMA:
 * . using this new SIF1DMA supporting libsensor.a revision;
 * . adding this macro for all the serial command data in sensor model driver;
 * . using new linker script;
 */
/* all sif1dma bufs used in serial APIs attaching through this macro */
#define SENSOR_SIF1DMA_BUF   __attribute__ (( \
			__aligned__ (D_CACHE_LINE_SIZE), \
			__section__ (".rodata.sif1dma") ))

#define SENSOR_SIF1DMA_VARBUF   __attribute__ (( \
			__aligned__ (D_CACHE_LINE_SIZE), \
			__section__ (".data.sif1dma.var") ))

/** \brief always calls this after CPU manipulating serial command buffer
 * when SIF1DMA is supported */
void sensorSerialDirectBufSnoop(UINT8 *pdata, UINT32 size);

UINT32 sensorSerialCreate(const sensorCmdport_t *pcmdport);
void sensorSerialDelete(UINT32 cmdportid);
void sensorSerialFlush(UINT32 cmdportid);
UINT32 sensorSerialAsyncWrite(UINT32 cmdportid,UINT32 nreg,const UINT8 *pdata);
UINT32 sensorSerialAsyncExtWrite(UINT32 cmdportid,UINT32 nreg,const UINT8 *pdata, sensorSyncOption_t sync);

UINT32 sensorSerialAsyncRead(UINT32 cmdportid,UINT32 nreg,UINT8 *pdata);
UINT32 sensorSerialBulkWrite(UINT32 cmdportid,UINT32 nreg,const UINT8 *pdata,UINT32 sync);
UINT32 sensorSerialBlockRead(UINT32 cmdportid,UINT32 nreg,UINT8 *pdata,UINT32 sync);
void sensorSerialReload(	UINT32 cmdportid, const sensorCmdport_t *pcmdport);

UINT32 sensorSerialDoneForceWaitAll(UINT32 cmdportid);
/*CQ Parser +++ */
/*
SENSOR_SIFPARSER_SEL_OFF   : Force to use CPU write
SENSOR_SIFPARSER_SEL_ON    : Force to use sifparser function
SENSOR_SIFPARSER_SEL_AUTO: Auto , depend on SENSOR_SIF1DMA_BUF section.
*/
typedef enum {
	SENSOR_SIFPARSER_SEL_OFF,
	SENSOR_SIFPARSER_SEL_ON,
	SENSOR_SIFPARSER_SEL_AUTO=0xFF,
} sensorSerialSifParserSel_t;

void sensorSerialSifParserSet(UINT32 on);
UINT32 sensorSerialSifParserGet();

/*CQ Parser --- */

typedef enum {
	SENSOR_CMDPORTID_SEL_XTG,
	SENSOR_CMDPORTID_SEL_EXP,
	SENSOR_CMDPORTID_SEL_AFE = 0x10,
	SENSOR_CMDPORTID_SEL_AGC,
	SENSOR_CMDPORTID_SEL_DCOFFSET,
} sensorCmdportIdSel_t;
UINT32 sensorCmdportIdGet(UINT32 port);
/*cmdport and serial port<<<*/
typedef void (*fpTimeOutCallback)(UINT32 parm);	/* resume Callback*/
void sensorSerialWaitTimeOutCfg(UINT32 ms,fpTimeOutCallback fp);/*Sensor Serial timeout config.*/


/*cmdport and serial port<<<*/


/*sensor tggpio config>>>*/
#define sensorTGGpioCfg(mask,dir)     TGGPIO_MASK_CFG(mask,dir)
#define sensorTGGpioWrite(mask,out)   TGGPIO_MASK_WRITE(mask,out)
#define sensorTGGpioRead(mask)        TGGPIO_MASK_READ(mask)
#define sensorTgGpioPinCfg(pin,dir)   TGGPIO_PIN_CFG(pin,dir)
#define sensorTgGpioPinWrite(pin,out) TGGPIO_PIN_WRITE(pin,out)
#define sensorTgGpioPinRead(pin)      TGGPIO_PIN_READ(pin)

/*sensor tggpio config<<<*/

/*sensor api function>>> : convert hal function to sensor api function */

typedef enum {
	SENSOR_API_PHY0_LANE_REMAP,	/*remap0:8lane  remap1:4lane*//*val1: remap0   val2: remap1 */
	SENSOR_API_PHY1_LANE_REMAP,	/*remap0:8lane  remap1:4lane*//*val1: remap0   val2: remap1 */
	SENSOR_API_SEC_LANE_REMAP=SENSOR_API_PHY1_LANE_REMAP,/*remap0:8lane  remap1:4lane*//*val1: remap0   val2: remap1 */
	SENSOR_API_RGB_PIN_ENABLE, 	/*val1: pin enable   val2: mask */
	SENSOR_API_LVDS_DATA_READ,	/*Check sensor data pin  low or high level(Low power state) */ /*val1: Hi/Low   val2: N/A */
	SENSOR_API_PREVIEW_AE_SET,	/*Convert AeAgc  base on current sensor mode */ /*val1: N/A    val2: N/A */
	SENSOR_API_SIGGEN_ENABLE,	/*siggen on/off*/  /*val1: on/off    val2: N/A */
	SENSOR_API_SIGGEN_VD_PAUSE,	/*siggen vd pause on/off*/  /*val1: on/off    val2: N/A */
	SENSOR_API_TGPLL_ENABLE,	/*TGPLL on/off*/  /*val1: on/off    val2: N/A */
	SENSOR_API_SIF_SPI_MSB_TRAN,/*Select sif spi MSB first , else LSB*//*val1: SIF1/2   val2: on/off */
	SENSOR_API_DUAL_PCLK1X,/*Dual sensor pclk1x settings*//*val1: kHz	val2: N/A */
	SENSOR_API_DUAL_CDSPCLK,/*Dual sensor pclk1x settings*//*val1: kHz	val2: N/A */
	SENSOR_API_SIF_TIMEOUT_CALLBACK,/*config sif timeout ,callback function *//*val1: timeout ms   val2: callback */
	SENSOR_API_DUAL_SENSEL,/* Dual Raw sensor SEL*//*val1: 0=>pri ,1=>sec,2=> autosw	val2: N/A */
	SENSOR_API_FAKE_TG_EN,/* Fake tg enable , tg replacement *//*val1: 0=>pri ,1=>sec	val2: 1: enable  0:disable*/
	SENSOR_API_SIGGEN_HDVDPOL,/*ssg HD/VD polarity *//*val1: hd polarity	val2:  hd polarity*/
	SENSOR_API_VD_PROBE_LMI5_EN,/*output VD probe signal from pin LMI5*//*val1: enable/disable	val2:  N/A*/
	SENSOR_API_SIGGEN_SYNCMODE,/*ssg vd sync to main sensor sof*//*val1: enable/disable	val2:  N/A*/
	SENSOR_API_MAX
} sensorApiSel_t;

UINT32 sensorModelApiCfgSet(UINT32 cfg/*sensorApiSel_t*/,UINT32 val1,UINT32 val2);
UINT32 sensorModelApiCfgGet(UINT32 cfg/*sensorApiSel_t*/,UINT32 *val1,UINT32 *val2);

/*sensor api function <<<*/


/*sensor event services>>>*/
void sensorEventVdWait(UINT32 times);
void sensorEventVvalidWait(UINT32 times);
void sensorEventNewFrameWait(UINT32 times);
void sensorEventCdspFrcWait(UINT32 times);
void sensorEventSsgVvalidWait(void);
void sensorEventPrecmdWait(void);
UINT32 sensorMipiClockLowPowerWait(UINT32 mask,UINT32 timeout_ms);
void sensor_frame_end_cb_reg(void *f, UINT32 param);
void sensor_frame_vd_cb_reg(void *f, UINT32 param);
void sensor_frame_orig_vd_neg_cb_reg(void *f, UINT32 param);

void tmrUsWait(UINT32 us);
void tmrUsHardWait(UINT32 us);

/*sensor event services<<<*/

/*sensor reload/supspend/resume function */

typedef UINT32 (*fpReloadCallback)(UINT32 mode); /* reload TG uCode. for 3-in-1 TG calibraion mode.*/
typedef UINT32 (*fpSuspendCallback)(void);	/* suspend Callback */
typedef UINT32 (*fpResumeCallback)(void);		/* resume Callback*/

void sensorReportSize(UINT32 id,UINT32 count);

void sensorReloadInstall(fpReloadCallback f); 
UINT32 sensorReloadExecute(UINT32 mode);

void sensorSuspendInstall(fpSuspendCallback f);
UINT32 sensorSuspendExecute();

void sensorResumeInstall(fpResumeCallback f);
UINT32 sensorResumeExecute();

UINT8 sensorSuspendStateGet(void);

/*sensor_model_tgpll +++*/
typedef enum {
	SENSOR_TGPLL_CFG_CLOCK,
	SENSOR_TGPLL_CFG_DELAY,
	SENSOR_TGPLL_CFG_DRIVING,
	SENSOR_TGPLL_CFG_CHARGEPUMP,
	SENSOR_TGPLL_CFG_MAX
} sensorTgpllCfg_t;

typedef enum {
	SENSOR_TGPLL_CTRL_EN,
	SENSOR_TGPLL_CTRL_MAX,	
} sensorTgpllCtrl_t;

UINT32 sensorTgPllClkExtSet(UINT32 clkHz);
UINT32 sensorTgPllClkExtGet(void);
void sensorTgPllClkCfgSet(UINT32 sel,UINT32 val);
void sensorTgPllClkCtrl(UINT32 sel,UINT32 val);
/*sensor_model_tgpll ---*/

/*Convert AeAgc  base on current sensor mode */
extern void ae_preview_evset_write();

/*sensorCustInfo*/
typedef enum {
	SENSOR_CUST_INFO_00, 
	SENSOR_CUST_INFO_01, 
	SENSOR_CUST_INFO_02, 
	SENSOR_CUST_INFO_03, 
	SENSOR_CUST_INFO_04, 
	SENSOR_CUST_INFO_05, 
	SENSOR_CUST_INFO_06, 
	SENSOR_CUST_INFO_07, 
	SENSOR_CUST_INFO_TOTAL,
} sensorCustInfo_t;

void sensorInfoSet(UINT32 smode,UINT32 sel,UINT32 param,UINT32 sz,UINT32 opt);
UINT32 sensorInfoGet(UINT32 smode,UINT32 sel);


/** \brief Capture task states */
typedef enum {
	STATE_SENSOR_VWD_IDLE        = 0x00,
	STATE_SENSOR_VWD_REINIT      = 0x01,
	STATE_SENSOR_VWD_WAIT_ORGVD  = 0x10,/*wait  org vd */
} VdwatchDogState_t;


#ifdef __cplusplus
}
#endif

#endif /*_SENSOR_MODEL_H_*/

