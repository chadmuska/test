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
#ifndef _SENSOR_DEF_H_
#define _SENSOR_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"

typedef enum {
	SENSOR_DEV_ID_SENSOR,
	SENSOR_DEV_ID_TV_IN,
	SENSOR_DEV_ID_SIGGEN,
	SENSOR_DEV_ID_RESERVED,
	SENSOR_DEV_ID_TOTAL,
} sensorDevId_t;

typedef enum {
	SENSOR_MODE_IDLE = 0, /*0,static default*/
	SENSOR_MODE_POWERSAVE = 0x10,
	SENSOR_MODE_STOP = 0x20,
	SENSOR_MODE_PREVIEW = 0x30,
	SENSOR_MODE_SNAP = 0x40,
} sensorMode_t;

#define SENSOR_MCLK_SRC_NOT_CONNECT 0
#define SENSOR_MCLK_SRC_SP5K_TGPLL  1
#define SENSOR_MCLK_SRC_ADCLPI_PAD  2
void sensorMclkSrcCfgSet(UINT32 src);
UINT32 sensorMclkSrcCfgGet(void);


#define SENSOR_SYNC_SIG_SP5K_SLAVE  0
#define SENSOR_SYNC_SIG_SP5K_MASTER 1
void sensorSyncSigMasterCfgSet(UINT32 master);
UINT32 sensorSyncSigMasterCfgGet(void);

typedef enum {
	SENSOR_DEV_POWER_ID_SYS_ON            = 0,
	SENSOR_DEV_POWER_ID_RST_ON            = 1,
	SENSOR_DEV_POWER_ID_SEN_ON            = 2,
	SENSOR_DEV_POWER_ID_HDR_ON            = 3,
	SENSOR_DEV_POWER_ID_AFE_WAKEUP        = 4,
	SENSOR_DEV_POWER_ID_TGPLL_ON          = 5,
	
	SENSOR_DEV_POWER_ID_TGPLL_OFF         = 0x85,
	SENSOR_DEV_POWER_ID_AFE_SLEEP         = 0x84,
	SENSOR_DEV_POWER_ID_HDR_OFF           = 0x83,
	SENSOR_DEV_POWER_ID_SEN_OFF           = 0x82,
	SENSOR_DEV_POWER_ID_RST_OFF           = 0x81,
	SENSOR_DEV_POWER_ID_SYS_OFF           = 0x80,
} sensorDevPowerId_t;

#define SENSOR_DEV_POWER_ID_CMOS_SEC_SEL 0x8000

typedef enum {
    SENSOR_DEV_POWER_ID_CMOS_SYS_ON       = 0x10,    /*  Initial all sensor pin as 0 */
    SENSOR_DEV_POWER_ID_CMOS_PWR_ANA_ON   = 0x11,    /*  Cmos Analog power about 3.5v~3.3v or 2.8~2.7v */
    SENSOR_DEV_POWER_ID_CMOS_PWR_IO_ON    = 0x12,    /*  Cmos IO power about 1.8v    */
    SENSOR_DEV_POWER_ID_CMOS_PWR_CORE_ON  = 0x13,    /*  Cmos Core power about 1.2v */
    SENSOR_DEV_POWER_ID_CMOS_RST_ON       = 0x14,
    SENSOR_DEV_POWER_ID_CMOS_PWDN_ON      = 0x15,    /*  for OV spec PWDN pin */
		
    SENSOR_DEV_POWER_ID_CMOS_PWDN_OFF     = 0x75,    /*  for OV spec PWDN pin  */
    SENSOR_DEV_POWER_ID_CMOS_RST_OFF      = 0x74,
    SENSOR_DEV_POWER_ID_CMOS_PWR_CORE_OFF = 0x73,    /*  Cmos Core power  about 1.2v */
    SENSOR_DEV_POWER_ID_CMOS_PWR_IO_OFF   = 0x72,    /*  Cmos IO power about 1.8v     */
    SENSOR_DEV_POWER_ID_CMOS_PWR_ANA_OFF  = 0x71,    /*  Cmos Analog power about 3.5v~3.3v or 2.8~2.7v */
    SENSOR_DEV_POWER_ID_CMOS_SYS_OFF      = 0x70,

    SENSOR_DEV_POWER_ID_CMOS_PWR_QUERY    = 0x90,    /*  Check if need to turn on sensor power in sensor init flow */

} sensorDevCMOSPowerId_t;

UINT32 sensorDevPowerCustomCb(UINT32 id);

typedef enum {
	SENSOR_DEV_POWER_SEQ_ID_CMOS_PWR_UP 	  = 0x11,	 /*  for sensor power up seq ,firmare control start*/
	SENSOR_DEV_POWER_SEQ_ID_CMOS_PWRSAVE_ON   = 0x12,	 /*  for sensor power saving enter(suspend)*/
	SENSOR_DEV_POWER_SEQ_ID_CMOS_PWRSAVE_OFF  = 0x72,	 /*  for sensor power saving exit  (resume) */
	SENSOR_DEV_POWER_SEQ_ID_CMOS_PWR_DOWN	  = 0x71,	 /*  for sensor power down seq ,firmare control start*/
} sensorDevCMOSPowerSeqId_t;

UINT32 sensorDevPowerCustomSeqCb(UINT32 id);/*sensorDevCMOSPowerSeqId_t*/


typedef enum {
	SENSOR_DEV_SEQ_DRV_SWRESET	  =0x1000,	/*reset enable then disable*/
	SENSOR_DEV_SEQ_DRV_SWRESET_ON ,		/*enable reset , reset mode*/
	SENSOR_DEV_SEQ_DRV_SWRESET_OFF, 	/*disable reset , normal mode*/

	SENSOR_DEV_SEQ_DRV_PWRUP	  ,
	SENSOR_DEV_SEQ_DRV_PWRUP_HDVD_ON,
	SENSOR_DEV_SEQ_DRV_PWRUP1	  ,
	SENSOR_DEV_SEQ_DRV_PWRUP2	  ,
	SENSOR_DEV_SEQ_DRV_PWRUP3	  ,

	SENSOR_DEV_SEQ_DRV_PWRDN	  ,
	SENSOR_DEV_SEQ_DRV_PWRDN1	  ,
	SENSOR_DEV_SEQ_DRV_PWRDN2	  ,
	SENSOR_DEV_SEQ_DRV_PWRDN3	  ,

	SENSOR_DEV_SEQ_DRV_SUSPEND	  ,
	SENSOR_DEV_SEQ_DRV_SUSPEND1   ,
	SENSOR_DEV_SEQ_DRV_SUSPEND2   ,
	SENSOR_DEV_SEQ_DRV_SUSPEND3   ,

	SENSOR_DEV_SEQ_DRV_RESUME	  ,
	SENSOR_DEV_SEQ_DRV_RESUME1	  ,
	SENSOR_DEV_SEQ_DRV_RESUME2	  ,
	SENSOR_DEV_SEQ_DRV_RESUME3	  ,

	SENSOR_DEV_SEQ_DRV_MAX		  ,

	SENSOR_DEV_SEQ_TGPLL_ON 	  =0x3000,
	SENSOR_DEV_SEQ_TGPLL_OFF	  =0x3001,
	SENSOR_DEV_SEQ_HDVD_ON		  =0x3002,
	SENSOR_DEV_SEQ_HDVD_OFF 	  =0x3003,

} sensorDevCtrlSeqId_t;

/*for power up sequence 	  */
/*id = sensorDevCtrlSeqId_t */
UINT32 sensorRegSeqCtrl(UINT32 id);


/*cfg type in tgSvc/afeSvc-> UINT32 (*devcfg)(UINT32 cfg,UINT32 val);*/
typedef enum {
	SENSOR_DEV_CFG_TG_PWR_SAVE, /*val==0:normal;*/
	SENSOR_DEV_CFG_AFE_PWR_SAVE,
	SENSOR_DEV_CFG_RGBOUT_FLOAT, /*val==0:normal;*/
	SENSOR_DEV_CFG_RGBOUT_TYPE, /*val==0:normal binary;1:gray, etc*/
	SENSOR_DEV_CFG_SNAP_READOUT_FIELD_REPEAT,
	SENSOR_DEV_CFG_TOTAL,
} sensorDevCfg_t; /*obsolete*/

typedef enum {
	SENSOR_SYS_CFG_AUTO_PWRSAVE_TIMEOUT,
	SENSOR_SYS_CFG_BSHUT_MAX_TIMEOUT,
	SENSOR_SYS_CFG_EVSET_SMOOTH, /*obsolete*/
	SENSOR_SYS_CFG_SHUTTER_EARLY_FRAME,/* 1 or 0 */
	SENSOR_SYS_CFG_MODE_SYNC_EARLY,/*pre command spot offset*/ 
	SENSOR_SYS_CFG_FLOW_USE_ABORT,/*0 or 1, has abort*/
	SENSOR_SYS_CFG_FLOW_IMMEDIATE_IDLE,/*0 or 1, immediately to idle mode.*/
	SENSOR_SYS_CFG_SENSOR_PAUSE,
	SENSOR_SYS_CFG_DUAL_SENSOR_AE_EN,	
	SENSOR_SYS_CFG_FLOW_EARLY_SWITCH_PREVIEW,
	SENSOR_SYS_CFG_CCD_PV_URGENT_CB_FRAME_LIMIT,
	SENSOR_SYS_CFG_NEWFRM_START_OFFSET,
	SENSOR_SYS_CFG_AGC_DLEAY_SYNC,	
	SENSOR_SYS_CFG_VD_WATCHDOG_EN,
	SENSOR_SYS_CFG_VD_WATCHDOG_PRI_EN,
	SENSOR_SYS_CFG_SLAVE_SENSOR_CMDSYNC,/*0:sync to output  hd/vd(ssg) , 1:sync to sensor org hd/vd  Only for slave sensor*/
	SENSOR_SYS_CFG_PRI_TVIN_SKIPFRAME,/*config if primary sensor need TV in skip frame*/
	SENSOR_SYS_CFG_SEC_TVIN_SKIPFRAME,/*config if secondary sensor need TV in skip frame*/
	SENSOR_SYS_CFG_TOTAL,
} sensorStsCfg_t;

typedef enum {
	SENSOR_MODESW_NO_KEEP,	/* sensor change as ori ModeSw set */
	SENSOR_MODESW_KEEP,		/* sensor no need to change */
	SENSOR_MODESW_CHANGE_AT_PVENTER,
	SENSOR_MODESW_CHANGE_AT_PVEXIT, /* sensor  change early */
	SENSOR_MODESW_TOTAL,
} SENSOR_MODESW_t;


typedef enum {
	SENSOR_IO_VSUBCTRL1,
	SENSOR_IO_VSUBCTRL2,
	SENSOR_IO_VSUBCTRL3,
	SENSOR_IO_RESET,
	SENSOR_IO_PWDN,
	SENSOR_IO_FREX,
	SENSOR_IO_RESET_2ND,
	SENSOR_IO_PWDN_2ND,
	SENSOR_IO_FREX_2ND,
	SENSOR_IO_EXTEND,
} sensorIoCtrl_t;


typedef enum {
	SENSOR_DEV_CFG_CUSTOM_TG_INTERNAL,
} sensorDevCustomCfg_t;

typedef struct {
	/* FIXME: only support vertical line */
	UINT16 voffset;
	UINT16 botStart; /*bottom start */
	UINT16 botLen;   /*bottom length*/
	UINT16 topEnd; /*top end*/
	UINT16 topLen; /*top length*/
	UINT16 viewStart; /*view start from voffset*/
	UINT16 viewEnd; /*view end from voffset*/
} sensorTgDummy_t;

typedef struct {
	UINT16 hoffset;
	UINT16 voffset;
	UINT16 hsize; /*2^hsize, hsize=0~14*/
	UINT16 vsize; /*2^vsize, vsize=0~14, hsize+vsize<=16*/
} sensorTgWin_t;

typedef struct {
	UINT8 fieldnum;
	UINT8 fieldsel[16];
} sensorTgFieldSel_t;


typedef struct {
	UINT8 iBinMode;	    /*: 0~1	    */
	UINT8 htrend;		/*: 0~1	    */
	UINT8 vtrend;	    /*: 0~1	    */
	UINT16 iHBinFac0; 	/*: 0~2048 */
	UINT16 iHBinFac1; 	/*: 0~2048 */
	UINT16 iHBinFac2; 	/*: 0~2048 */
	UINT16 iHBinFac3; 	/*: 0~2048 */
	UINT16 iVBinFac0; 	/*: 0~1024 */
	UINT16 iVBinFac1; 	/*: 0~1024 */
	UINT8 iBinCorrHen; 	/*: 0~ 3     */
} sensorTgBinCor_t;


typedef struct {
	UINT8 tg_type;
	UINT8 n_preview_submode; /*preview, af, and other front digi zoom*/
	UINT8 n_snap_submode; /*snap front digi zoom*/
	UINT8 img_src_type;
	UINT8 img_src_cfg;
	UINT8 nbit_rgb;
} sensorCapability_t;

typedef struct {
	UINT8  sensorMode;
	UINT8  sensor_model; /*sensorModel_t*/
	UINT8  cfa_pattern; /*sensorCfaPattern_t*/
	UINT8  align; /*bitmask 0:center,1:left,2:right,4:top,8:bottom*/
	UINT8  model_info;
	UINT16 hsize; /*size at no zooming*/
	UINT16 vsize; /*size at no zooming*/
	UINT32 zhFac; /*65536 is 1x, related to submode 0*/
	UINT32 zvFac; /*65536 is 1x, related to submode 0*/
	UINT16 zhsize;
	UINT16 zvsize;
	SINT16 zhoff;
	SINT16 zvoff;	UINT16 pix_100khz; /*pixel clock in 100khz*/
	UINT16 hratio; /*logical horizontal size, eg. 4000, 3200*/
	UINT16 vratio; /*logical vertical size, eg. 3000, 1800*/
	UINT16 fhscale; /*logical vertical size, eg. 3000, 1800*/
	UINT32 fps_mhz; /* fps * 1000000 */
	UINT32 sensor_pclk;  /*Unit:kHz , sensor pixel clock for auto banding caculation*/
	UINT32 sensor_htotal;/* sensor pixel clock per line  for auto banding caculation*/
	UINT8  max_view_flag;/*flag to check if it is max view size*/
	UINT32 bayerhs_size;/*Bayer Hscale size*/
} sensorCapabilityPreview_t;

typedef struct {
	UINT8  sensorMode;
	UINT8  sensor_model; /*sensorModel_t*/
	UINT8  cfa_pattern; /*sensorCfaPattern_t*/
	UINT8  model_info;
	UINT8  n_dummy;
	UINT8  n_readout;
	UINT8  n_capvd; /*obsolete, left for backward compatible*/
	UINT8  n_snap_early;
	UINT8  fldsel_start;
	UINT8  fldsel_end;
	UINT8  align; /*bitmask 0:center,1:left,2:right,4:top,8:bottom*/
	UINT16 hsize; /*size at no zooming*/
	UINT16 vsize; /*size at no zooming*/
	UINT32 zhFac; /*65536 is 1x, related to submode 0*/
	UINT32 zvFac; /*65536 is 1x, related to submode 0*/
	UINT16 zhsize;
	UINT16 zvsize;
	SINT16 zhoff;
	SINT16 zvoff;
	UINT16 pix_100khz; /*pixel clock in 100khz*/
	UINT16 hratio; /*logical horizontal size, eg. 4000, 3200*/
	UINT16 vratio; /*logical vertical size, eg. 3000, 1800*/
	UINT16 fhscale; /*logical vertical size, eg. 3000, 1800*/
	UINT32 sensor_pclk; 	/*Unit:kHz , sensor pixel clock for auto banding caculation*/
	UINT32 sensor_htotal;	/* sensor pixel clock per line	for auto banding caculation*/
} sensorCapabilitySnap_t;


typedef enum {
	SENSOR_CAP_EXT_ID_DUMMY, /*sensor preview dummy*/
	SENSOR_CAP_EXT_ID_OBWIN, /*sensor preview ob window*/
	SENSOR_CAP_EXT_ID_OBWIN2, /*sensor preview ob window*/
	SENSOR_CAP_EXT_ID_FIELDSEL, /*sensor capture fieldsel */
	SENSOR_CAP_EXT_ID_CROP, /*sensor preview crop size for 6330 mode 9 pv bp issue */
	SENSOR_CAP_EXT_ID_BINCOR, /*sensor preview capability for bining correction*/
	SENSOR_CAP_EXT_TOTAL,
} sensorCapExtId_t;


UINT32 sensorDevIoCtrl(sensorIoCtrl_t devIo, UINT32 opt);
UINT32 sensorDevCfgCustom(sensorDevCustomCfg_t sel, UINT32 opt2);

/** \param cfg is one of sensorStsCfg_t */
UINT32 sensorStsCfg(UINT32 cfg,UINT32 val);


#ifdef __cplusplus
}
#endif

#endif
