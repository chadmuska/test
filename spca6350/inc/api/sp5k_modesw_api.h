/**************************************************************************
 *
 *       Copyright (c) 2004-2014 by iCatch Technology, Inc.
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
 **************************************************************************/

/**
 * @file sp5k_modesw_api.h
 * @brief header file for sp5k mode.
 */

#ifndef _SP5K_MODESW_API_H_
#define _SP5K_MODESW_API_H_

#include "middleware/global_def.h" /* power save mode */
#include "middleware/sensor_def.h"
#include "middleware/modesw_def.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define SP5K_MODE_SEC_SENSOR_SEL 0x8000
 
typedef enum {
	SP5K_MODE_STILL_PREVIEW  = 0x11,
	SP5K_MODE_STILL_SNAP     = 0x13,
	SP5K_MODE_VIDEO_PREVIEW  = 0x21,
	SP5K_MODE_VIDEO_RECORD   = 0x23,
	SP5K_MODE_AUDIO_RECORD   = 0x33,
	SP5K_MODE_MULTI_PLAYBACK = 0x41,
	SP5K_MODE_STILL_PLAYBACK = 0x42,
	SP5K_MODE_VIDEO_PLAYBACK = 0x43,
	SP5K_MODE_AUDIO_PLAYBACK = 0x44,
	SP5K_MODE_STANDBY        = 0x88,
	SP5K_MODE_POWER_SAVE     = 0x90,

	SP5K_MODE_DIQ			 = 0xff	
} sp5kMode_e;

typedef enum {
	SP5K_MODE_CFG_FILE_TYPE             = 0x01,	/* obsolete */
	SP5K_MODE_CFG_STANDBY_DUP	        = 0x02, /*param1: 1= backup last frame, 0= black frame */
	SP5K_MODE_CFG_STANDBY_FORCE         = 0x03, /*param1: 1= enter standby even already in */
	SP5K_MODE_CFG_PV_FREEZE		        = 0x04, /*param1: 1=enable freeze, 0=disable freeze */
	SP5K_MODE_CFG_PV_DISP_FIT	        = 0x05, /*param1: 1=enable pv disp fit by ratio, 2=pv fit by snapimg ratio, 0=disable */
	SP5K_MODE_CFG_STANDBY_FRAME_SIZE    = 0x06, /*param1: width, param2: height*/
	SP5K_MODE_CFG_STILL_PV_SIZE	        = 0x07, /*param1: width, param2: height, param3: num */
	SP5K_MODE_CFG_STILL_PV_DELAY        = 0x08, /*param1: delay frame cnt */
	SP5K_MODE_CFG_STILL_PV_MV		    = 0x09, /*param1: pv mv enable */
	SP5K_MODE_CFG_BAND_DETECT_ENABLE    = 0x0A, /*param1: band detect enable */
	SP5K_MODE_CFG_PV_PAN_ENABLE         = 0x0B, /*param1: pv pan center enable */
	SP5K_MODE_CFG_STNDBY_FRAME_BUF_SIZE = 0x0C, /*param1,param2: Idle buffer frame size */
	SP5K_MODE_CFG_PV_DIST_OUTSIDE       = 0x0D, /*param1: dist outside mode enable */
	SP5K_MODE_CFG_PV_SENSOR_OPT 		= 0x0F, /*param1: sp5kModePvSensorOpt_e */
	SP5K_MODE_CFG_PV_2ND_PATH_ENABLE    = 0x10, /*param1: 1: enable, 0: disable */
	/*param1: en, param2: org clk, param3: new clk */
	SP5K_MODE_CFG_VIDEO_PV_CLK_SWITCH   = 0x11,
	SP5K_MODE_CFG_POWER_SAVE_MODE       = 0x12, /*param1: sp5kModePowerSaveMode_e*/
	SP5K_MODE_CFG_POWER_SAVE_WAKEUP_PIN = 0x13,
	SP5K_MODE_CFG_POWER_SAVE_INDICATOR  = 0x14,
	SP5K_MODE_CFG_SNAP_CLK_SWITCH		= 0x15,
	SP5K_MODE_CFG_PV_DISP_SCALE_FACTOR  = 0x16, /*param1, param2: disp Pv scale factor */
	SP5K_MODE_CFG_PV_IMAGE_FORMAT       = 0x17, /*param1: 1: YUV420, 0: YUV422 */
	SP5K_MODE_CFG_PV_CDSP_SCALING_OPTION= 0x18, /*param1: 0: default(spv rgb, vpb yuv), 1:rgb scaling, 2: yuv scaling */
	SP5K_MODE_CFG_VIDEO_PB_CLK_SWITCH	= 0x19,
	SP5K_MODE_CFG_POWER_SAVE_TIMER_EN   = 0x1a, /*param1: 0: disable, 1: enable. param2: timeout (ms)*/
	SP5K_MODE_CFG_POWER_SAVE_WAKEUP_GET = 0x1b,
	SP5K_MODE_CFG_PREVIEW_DISP_CROP_GET = 0x1c,
	SP5K_MODE_CFG_PV_BUF_ALLOC_BOTTOM	= 0x1d,
	SP5K_MODE_CFG_PV_SCALE_OPTION    	= 0x1e, /* 0: gpe 1: gpe bilinear 2: jpg */
	SP5K_MODE_CFG_PV_CAP_GET			= 0x1f,
	SP5K_MODE_CFG_SNAP_CAP_GET			= 0x20,
	SP5K_MODE_CFG_STILL_PV_IMG_ROI		= 0x21,/*param1: roiX, param2: roiY, param3: roiW,  param3: roiH if SP5K_MODE_CFG_PV_DISP_FIT=3 */
	SP5K_MODE_CFG_SEC_PV_CAP_GET		= 0x22,
	SP5K_MODE_CFG_VIDEO_PV_SIZE			= 0x23,
	SP5K_MODE_CFG_PV_DISP_CHNL			= 0x24,
	SP5K_MODE_CFG_PV_UVC_DEVICE         = 0x25,
	SP5K_MODE_CFG_PV_RAW_DO_OPT 		= 0x26, /*param1:  sp5kModePvDoRawOpt_e */
	SP5K_MODE_CFG_PV_RAW_DO_PARAM       = 0x27,
	SP5K_MODE_CFG_PV_VIDEO_SRC_ROI      = 0x28, /*param1 : videoId ;  param2 : sp5kModePvVideoSrcRoi_t */
} sp5kModeCfg_e;

typedef enum {
	SP5K_MODE_PV_DISP_SINGLE = (1 << 0),
	SP5K_MODE_PV_DISP_DUAL = (1 << 1), 
}sp5kModePvDispChnl_e;

typedef enum {
	SP5K_MODE_PV_RAW_DO_DISABLE  = 0,
	SP5K_MODE_PV_RAW_DO_UHD_MODE = 1,
	SP5K_MODE_PV_RAW_DO_CUST_CB_MODE = 2, 
}sp5kModePvDoRawOpt_e;

typedef enum {
	SP5K_MODE_PV_RAW_DO_UHD_MODE_PARAM_RAW_NUM = 0x01,
	SP5K_MODE_PV_RAW_DO_CUST_RAW_TO_YUV_CB     = 0x101,
	SP5K_MODE_PV_RAW_DO_CUST_POST_YUV_CB       = 0x102,
}sp5kModePvDoRawParam_e;


typedef enum { /* obsolete */
	SP5K_MODE_FILE_TYPE_JPG  = 0x01,
	SP5K_MODE_FILE_TYPE_AVI  = 0x02,
	SP5K_MODE_FILE_TYPE_WAV  = 0x03,
} sp5kModeFileType_e;

typedef enum {
	SP5K_MODE_STILL_PREVIEW_NORMAL  = 0x00,
	SP5K_MODE_STILL_PREVIEW_FAKE    = 0x01,
} sp5kModeEx_e;

typedef enum {
	SP5K_MODE_PV_FRZ_DISABLE       = 0x00, /* Noraml flow, enter Idle mode */
	SP5K_MODE_PV_FRZ_NEXT_MODE     = 0x01, /* freeze All Next Mode, No enter Idle mode */
	SP5K_MODE_PV_FRZ_MODESW        = 0x02, /* freeze at ModeSw, No enter Idle mode */
	SP5K_MODE_PV_FRZ_KEEP_BUF      = 0x03, /* Not free ori buf, Keep Original buf size */
	SP5K_MODE_PV_FORCE_IDLE_FRAME  = 0x04, /* Force use idle transit buffer */
} sp5kModePvFreezeType_e;


typedef enum {
	SP5K_MODE_POWER_SAVE_STANDBY,
	SP5K_MODE_POWER_SAVE_SUSPEND,
	SP5K_MODE_POWER_SAVE_HIBERNATE,
	SP5K_MODE_POWER_SAVE_MAX = SP5K_MODE_POWER_SAVE_HIBERNATE,
} sp5kModePowerSaveMode_e;

typedef enum {
	SP5K_MODE_POWER_SAVE_WAKEUP_SRC_TIMER		= POWER_SAVE_WAKEUP_SRC_TIMER,
	SP5K_MODE_POWER_SAVE_WAKEUP_SRC_GRP_GPIO	= POWER_SAVE_WAKEUP_SRC_GRP_GPIO,
} sp5kModePowerSaveWakeupSrc;

typedef enum {
	SP5K_MODE_POWER_SAVE_GPIO_ACT_MASTER = 0,
	SP5K_MODE_POWER_SAVE_GPIO_ACT_LOW_2_HIGH = 0x11,
	SP5K_MODE_POWER_SAVE_GPIO_ACT_HIGH_2_LOW = 0x12,
	
} sp5kModePowerSaveGpioCond_e;

typedef sensorCapabilityPreview_t	sp5kCapabilityPreview_t;
typedef sensorCapabilitySnap_t		sp5kCapabilitySnap_t;
typedef modePvVideoSrcRoi_t         sp5kModePvVideoSrcRoi_t;

/*SP5K_MODE_CFG_PV_SENSOR_OPT*/
typedef enum {
	SP5K_MODE_PV_SENSOR_DEFAULT      	  = 0x00,
	SP5K_MODE_PV_SENSOR_HDR_SEQ_MODE	  = 0x01,
	SP5K_MODE_PV_SENSOR_LDC_STRETCH_H     = 0x02,
	SP5K_MODE_PV_SENSOR_DUAL_PRI_SENSOR   = 0x10,
	SP5K_MODE_PV_SENSOR_DUAL_SEC_SENSOR   = 0x11,
	SP5K_MODE_PV_SENSOR_DUAL_TOP_DOWN     = 0x12,  /*Only disp combine, but video separate  */
	SP5K_MODE_PV_SENSOR_DUAL_LEFT_RIGHT   = 0x13,
	SP5K_MODE_PV_SENSOR_DUAL_TOP_DOWN_REC = 0x14,  /*video and disp combine */
	SP5K_MODE_PV_SENSOR_DUAL_360_STITCH   = 0x16,
} sp5kModePvSensorOpt_e;

#define SP5K_MODE_PV_CFG_IS_SRC_BUF_FIT_SENSOR_RATIO (1<<8)

/**************************************************************************
 *                           D A T A   T Y P E S                          *
 **************************************************************************/
#define sp5kModePowerSaveWakeupPin_t powerSaveWakeupPin_t
#define sp5kModePowerSaveWakeupCfg_t powerSaveWakeupCfg_t
#define sp5kModePowerSaveIndicator_t powerSaveIndicator_t

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
UINT32 sp5kModeSet(UINT32 mode);
UINT32 sp5kModeGet(UINT32 *pmode);
UINT32 sp5kModeGetPeek(UINT32 *pmode);

void sp5kModeWait(UINT32 newmode);
UINT32 sp5kModeCfgSet(sp5kModeCfg_e cfg, ...);
UINT32 sp5kModeCfgGet(sp5kModeCfg_e cfg, ...);
UINT32 sp5kModeGetEx(UINT32 *pmode,	UINT32 *pEx);

void sp5kModeMultiPbInit(void);
void sp5kModeStillPbInit(void);
void sp5kModeAvPlayInit(void);
void sp5kModeStillPrevInit(void);
void sp5kModeStillSnapInit(void);
void sp5kModeVideoPrevInit(void);
void sp5kModeVideoRecInit(void);
void sp5kModeAudioRecInit(void);
void sp5kModeUSBInit(void);
void sp5kModePowerSaveInit(void);

#endif  /* _SP5K_MODESW_API_H_ */

