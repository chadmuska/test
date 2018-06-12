/**************************************************************************
 *
 *       Copyright (c) 2008-2014 by iCatch Technology, Inc.
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
#ifndef _SP5K_ADAS_API_H_
#define _SP5K_ADAS_API_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "common.h"
#include "middleware/adas_def.h"
#include "middleware/lanedetect_def.h"
#include "middleware/rod_def.h"
#include "middleware/lpd_def.h"
#include "middleware/tsr_def.h"
#include "middleware/fcmd_def.h"
#include "middleware/headlight_def.h"



/**< INTERFACE TO SP5K HOST */

/**
 * called in app_init.c and switch on/off in app_init.h
 * ftrack FW module will not be linked in without calling
 */
/**< calling this if host wants HW face track support for preview */
/*void ftrackPvInit(void);*/



#if 0
/**
 * \param mode disable, enable
 * UINT32 sp5kAdasModeSet(UINT32 mode);
 */
typedef enum {
	SP5K_ADAS_MODE_OFF 		  = ADAS_MODE_OFF,
	SP5K_ADAS_MODE_PV_ON		  = ADAS_MODE_PV_ON,
	SP5K_ADAS_MODE_PV_DELAY_OFF  = ADAS_MODE_PV_DELAY_OFF,
} sp5kAdasModeSel_t;

UINT32 sp5kAdasModeSet(UINT32 mode);
#endif
/*LDWS CFG SET SELECT*/
#define SP5K_ADAS_LDWS_THRESHOLD_SET LANEDET_DEPATURE_THRESHOLD
#define SP5K_ADAS_LDWS_CENTRAL_POINT_SET LANEDET_CENTRAL_POINT
#define SP5K_ADAS_LDWS_IMAGE_SCALE_FACTOR_SET LANEDET_IMAGE_SCALE_FACTOR
#define SP5K_ADAS_LDWS_WARN_MSG_INTERVAL LANEDET_WARN_MSG_INTERVAL
#define SP5K_ADAS_LDWS_SENSOR_FOCAL LANEDET_SENSOR_FOCAL
#define SP5K_ADAS_LDWS_SENSOR_PIXEL_UNIT LANEDET_SENSOR_PIXEL_UNIT
#define SP5K_ADAS_LDWS_THRESHOLD_CENTER_TO_LEFT LANEDET_LEFT_DIST_THRES
#define SP5K_ADAS_LDWS_THRESHOLD_CENTER_TO_RIGHT LANEDET_RIGHT_DIST_THRES


#define SP5K_ADAS_ROD_MAP_PARAM ROD_MAP_PARAM


typedef  UINT32(*sp5kAdasAlgoCallback_t)(UINT32 param, UINT32 param2);
typedef adasInputBufCfg_t sp5kAdasInputBufCfg_t;


typedef struct sp5kAdasLdwsInfo_s {
	UINT32 ftx1;
	UINT32 ftx2;
	UINT32 ftx3;
	UINT32 ftx4;
	UINT32 fty1;
	UINT32 fty2;
	UINT32 fty3;
	UINT32 fty4;
	UINT32 roi_h_end;
} sp5kAdasLdwsInfo_t;

typedef struct sp5kAdasLdwsResult_s {
	sp5kAdasLdwsInfo_t info;
	UINT32 warn_flag;
	UINT32 distance_center_to_lane_l;
	UINT32 distance_center_to_lane_r;
} sp5kAdasLdwsResult_t;

typedef struct sp5kAdasLdwsCfg_s{
	UINT32 angle_thr;
	UINT32 distance2center_thr;
	UINT32 center_x;
	UINT32 center_y;
	UINT32 img_scale_factor;
	UINT32 warn_lock_time;
	UINT32 focal_length;
	UINT32 senser_pixel_size;
	UINT32 left_dist_thres;
	UINT32 right_dist_thres;
}sp5kAdasLdwsCfg_t;


typedef struct sp5kAdasRodCoord_s{
	UINT16 Lx;
	UINT16 Ly;
	UINT16 Rx;
	UINT16 Ry;	
}sp5kAdasRodCoord_t;		

typedef struct sp5kAdasRodResult_s {
	sp5kAdasRodCoord_t warning_coord[3];
	UINT32 warning_flag_near;
	UINT32 warning_flag_far;
}sp5kAdasRodResult_t;

typedef enum {
	SP5K_LPD_RESULT_NOT_FOUND = LPD_RESULT_NOT_FOUND,
	SP5K_LPD_RESULT_LESS_THAN_SIX_DIGITS = LPD_RESULT_LESS_THAN_SIX_DIGITS,
	SP5K_LPD_RESULT_FOUND = LPD_RESULT_FOUND,
}SP5K_ADAS_LPD_RESULT_t;

typedef struct {
	UINT8 *yuvaddr;
	UINT16 yuvw;
	UINT16 yuvh;
	UINT16 dispx;
	UINT16 dispy;
	UINT16 dispw;
	UINT16 disph;
	UINT16 frmid;
} sp5kpadasLPDFrameBuf_t;

typedef struct sp5kAdasLpdResult_s {
	UINT8 words[8];
	UINT32 lp_x;
	UINT32 lp_y;
	UINT32 lp_w;
	UINT32 lp_h;	
	UINT8 update_flag;
	sp5kpadasLPDFrameBuf_t *lpdbuf;
	SP5K_ADAS_LPD_RESULT_t result; /*0-not found , 1-digits <6 , 2 - found*/
}sp5kAdasLpdResult_t;

typedef struct sp5kAdasRodMap_s{
	UINT32 rod_up_coord_y;
	UINT32 rod_mid_coord_y;
	UINT32 rod_low_coord_y;
	UINT32 image_width;
	UINT32 image_Height;
}sp5kAdasRodMap_t;

typedef struct sp5kAdasRodCfg_s{
	sp5kAdasRodMap_t map;
}sp5kAdasRodCfg_t;

typedef struct{
	UINT16 Lx;
	UINT16 Ly;
	UINT16 Rx;
	UINT16 Ry;	
}TSR_Region_Coord_t;

typedef struct sp5kAdasTsrResult_s {
	TSR_Region_Coord_t tsr_coord;
	UINT32 tsr_num;
	UINT32 tsr_estnum;
}sp5kAdasTsrResult_t;

#define SP5K_ADAS_TSR_RESULT_SHOW_TIME TSR_RESULT_SHOW_TIME
#define SP5K_ADAS_TSR_CENTER_Y TSR_CENTER_Y

typedef struct sp5kAdasTsrCfg_s{
	UINT32 result_show_time;
	UINT32 tsr_center_y;
}sp5kAdasTsrCfg_t;

/*FCMD*/
#define SP5K_ADAS_FCMD_FW_CARREMOVE_FLAG FCMD_FW_CARREMOVE_FLAG

typedef struct sp5kAdasfcmdResult_s {
	UINT32 warning_flag;
}sp5kAdasfcmdResult_t;

typedef struct sp5kAdasfcmdCfg_s{
	UINT32 forward_carRemove_flag;
}sp5kAdasfcmdCfg_t;

/*HEADLIGHT*/
#define SP5K_ADAS_HEADLIGHT_CFG_SET1 HEADLIGHT_CFG_SET1

typedef struct sp5kAdasHLResult_s {
	UINT32 warning_flag;
}sp5kAdasHLResult_t;

typedef enum {
	SP5K_ADAS_INPUTBUF_CFG_SENSOR_FRONT = ADAS_INPUTBUF_CFG_SENSOR_FRONT,
	SP5K_ADAS_INPUTBUF_CFG_SENSOR_REAR = ADAS_INPUTBUF_CFG_SENSOR_REAR,
	SP5K_ADAS_INPUTBUF_CFG_SENSOR_LEFT = ADAS_INPUTBUF_CFG_SENSOR_LEFT,
	SP5K_ADAS_INPUTBUF_CFG_SENSOR_RIGHT = ADAS_INPUTBUF_CFG_SENSOR_RIGHT,	
	SP5K_ADAS_INPUTBUF_CFG_SENSOR_MAX = ADAS_INPUTBUF_CFG_SENSOR_MAX
}SP5K_ADAS_InputBuf_Cfg_Sensor;

typedef enum {
	SP5K_ADAS_INPUTBUF_CFG_INPUT_MAIN = ADAS_INPUTBUF_CFG_INPUT_CDSPMAIN,
	SP5K_ADAS_INPUTBUF_CFG_INPUT_SEC = ADAS_INPUTBUF_CFG_INPUT_CDSPSEC,
	SP5K_ADAS_INPUTBUF_CFG_INPUT_PBVDO = ADAS_INPUTBUF_CFG_INPUT_PBVDO,
	SP5K_ADAS_INPUTBUF_CFG_INPUT_MAX = ADAS_INPUTBUF_CFG_INPUT_MAX
}SP5K_ADAS_InputBuf_Cfg_Input;


/**
 * \param cfg in
 * UINT32 sp5kAdasCfgSet(UINT32 cfg,UINT32 val);
 */
typedef enum {
	SP5K_ADAS_CFG_INPUTBUF 			  = ADAS_CFG_INPUTBUF,
	SP5K_ADAS_CFG_ALGO_ENABLE			  = ADAS_CFG_ALGO_ENABLE,
	SP5K_ADAS_CFG_RESULT_CB				  = ADAS_CFG_RESULT_CB,
	SP5K_ADAS_CFG_ALGO_CAR_ROI			  = ADAS_CFG_ALGO_SHARE_CAR_ROI,
	SP5K_ADAS_CFG_ALGO_LDWS				  = ADAS_CFG_ALGO_LDWS,
	SP5K_ADAS_CFG_ALGO_ROD				  = ADAS_CFG_ALGO_ROD,	
	SP5K_ADAS_CFG_ALGO_LPD				  = ADAS_CFG_ALGO_LPD,
        SP5K_ADAS_CFG_ALGO_TSR                           = ADAS_CFG_ALGO_TSR,
	SP5K_ADAS_CFG_ALGO_FCMD                        = ADAS_CFG_ALGO_FCMD,
	SP5K_ADAS_CFG_ALGO_PERIOD                     = ADAS_CFG_ALGO_PERIOD,
	SP5K_ADAS_CFG_ALGO_HEADLIGHT		   = ADAS_CFG_ALGO_HEADLIGHT,
	SP5K_ADAS_CFG_TOTAL 					
} sp5kAdasCfgSel_t;


typedef enum {
	SP5K_ADAS_ALGO_LDWS        		  = ADAS_ALGO_LDWS,
	SP5K_ADAS_ALGO_LPD     	          = ADAS_ALGO_LPD,
	SP5K_ADAS_ALGO_TSR        		  = ADAS_ALGO_TSR,
	SP5K_ADAS_ALGO_BLINDSPOT    	  = ADAS_ALGO_BLINDSPOT,
	SP5K_ADAS_ALGO_ROD      	      = ADAS_ALGO_ROD,
	SP5K_ADAS_ALGO_BIRDVIEW           = ADAS_ALGO_BIRDVIEW,
	SP5K_ADAS_ALGO_FCMD                   = ADAS_ALGO_FCMD,
	SP5K_ADAS_ALGO_HEADLIGHT         = ADAS_ALGO_HEADLIGHT,
	SP5K_ADAS_ALGO_TOTAL						
} sp5kAdasAlgoSel_t;

#define SP5K_ADAS_COORD_HSIZE     ADAS_COORD_HSIZE /*adas coordinate window*/
#define SP5K_ADAS_COORD_VSIZE     ADAS_COORD_VSIZE

#define SP5K_ADAS_RUN	1
#define SP5K_ADAS_STOP	0


/**
 * \param cfg in sp5kAdasCfgSel_t
 */
UINT32 sp5kAdasCtrl(UINT32 enable);
UINT32 sp5kAdasCfgSet(UINT32 selector,...);
UINT32 sp5kAdasCfgGet(UINT32 selector,...);
UINT32 sp5kAdasResultGet(UINT32 selector,...);
void sp5kAdasInit(void);


/**************************************************************************
 * Adas Support
 **************************************************************************/



/**************************************************************************
 * Backward compatible
 **************************************************************************/



#ifdef __cplusplus
}
#endif

#endif /* _SP5K_ADAS_API_H_ */
