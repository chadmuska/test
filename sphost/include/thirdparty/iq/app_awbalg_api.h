/**************************************************************************
 *
 *	Copyright (c) 2006-2011 by Sunplus mMedia Inc., Ltd.
 *
 *	This software is copyrighted by and is the property of Sunplus
 *	mMedia Inc., Ltd. All rights are reserved by Sunplus mMedia
 *	Inc., Ltd. This software may only be used in accordance with the
 *	corresponding license agreement. Any unauthorized use, duplication,
 *	distribution, or disclosure of this software is expressly forbidden.
 *
 *	This Copyright notice MUST not be removed or modified without prior
 *	written consent of Sunplus mMedia Inc., Ltd.
 *
 *	Sunplus mMedia Inc., Ltd. reserves the right to modify this
 *	software without notice.
 *
 *	Sunplus mMedia Inc., Ltd.
 *	19-1, Innovation First Road, Science-Based Industrial Park,
 *	Hsin-Chu, Taiwan, R.O.C.
 *
 **************************************************************************/
#ifndef APP_AWBALG_APIH
#define APP_AWBALG_APIH

#include "middleware/aastat_def.h"


typedef enum _tagAWBALG_SENSOR_SEL {
    AWB_SENSOR_PRI= 0x0,         
    AWB_SENSOR_SEC, 
    AWB_SENSOR_PRI_SEC,
    AWB_SENSOR_Max
} _AWBALG_SENSOR_SEL;

typedef enum _tagAWBALG_MODE_SEL {
    AWB_MODE_Auto = 0x0,         
    AWB_MODE_DayLight, 
    AWB_MODE_Cloudy,
    AWB_MODE_Tungsten,
    AWB_MODE_Fluorescent_H,
    AWB_MODE_Fluorescent_L,
    AWB_MODE_Fluorescent,
    AWB_MODE_Custom,
    AWB_MODE_Custom_Set,
    AWB_MODE_Sunny,
    AWB_MODE_Sunset,
    AWB_MODE_Flash,
    AWB_MODE_Auto_Water,
    AWB_MODE_Region0,
    AWB_MODE_Region1,
    AWB_MODE_Region2,
    AWB_MODE_Region3,
    AWB_MODE_Region4,
    AWB_MODE_GW,
    AWB_MODE_Max
} _AWBALG_MODE_SEL;

typedef enum _tagWBAlg_PARA_VAL {
    AWB_PARA_STABLE,
    AWB_PARA_INTERVAL_FRAME,
    AWB_PARA_HIGH_SPEED,
    AWB_PARA_MEDIUM_SPEED,
    AWB_PARA_LOW_SPEED,
    AWB_PARA_HIGH_SPEED_WP_THR,
    AWB_PARA_LOW_SPEED_WP_THR,
    AWB_PARA_CONV_PERCENTAGE,
    AWB_PARA_UPDTREFE_FRAME_NUM,
    AWB_PARA_UPDTREFE_FRAME_THR,
    AWB_PARA_UPDTREFE_WP_THR,
    AWB_PARA_SAMPING_X_RATIO,
    AWB_PARA_SAMPING_Y_RATIO,
    AWB_PARA_CALIB_TYPE,
    AWB_PARA_CALIB_DATA_UPDATE,
    AWB_PARA_CALIB_DATA_INFO,
    AWB_PARA_CALIB_DATA_RGB_GAIN,
    AWB_PARA_GD_DATA_RGB_GAIN,    
    AWB_PARA_CWB_CB_SET,
    AWB_PARA_CUSTOMGET_FINISH,
    AWB_PARA_CUSTOMGET_GAIN,
    AWB_PARA_GAINS_RANGE,
    AWB_PARA_PREVIEW_IMMEDIATELY,
    AWB_PARA_CAPTURE_IMMEDIATELY,
    AWB_PARA_LV,
    AWB_PARA_CLRTMP,
    AWB_PARA_GW,
    AWB_PARA_SW_WHITE_CLIP_EN,
    AWB_PARA_LIB_VERSION,
    AWB_PARA_RES_VERSION,
    AWB_PARA_REG_VERSION,
    AWB_PARA_CVR_NIGHT_SCENE_EN,
    AWB_PARA_SENSOR_SEL,
    AWB_PARA_GAIN_APPLY_EN,
    AWB_PARA_SPEED,
    AWB_PARA_RGAIN_RATIO,
    AWB_PARA_BGAIN_RATIO,	
    AWB_PARA_SENSORWBGAIN_CB_REGISTER,	
    AWB_PARA_LV_CB_REGISTER,
    AWB_PARA_CB_REGISTER,
    AWB_PARA_IMMED_BREAK_CNT,

	
	AWB_PARA_FPTR_SECTION = 0x70000000,
	AWB_PARA_FPTR_AEINFO = AWB_PARA_FPTR_SECTION,
    
	AWB_PARA_DEBUG_SECTION = 0x80000000,
	AWB_PARA_DEBUG_INFO = AWB_PARA_DEBUG_SECTION,
	AWB_PARA_DEBUG_SAMP,
	AWB_PARA_DEBUG_BINS,
	AWB_PARA_DEBUG_MAX,
	
    AWB_PARA_MAX
} _WBAlg_PARA_VAL;

typedef enum _tagWBAlg_CALIB_TYPE {
	AWB_CALIB_TYPE_WILD = 0, /* not init */
    AWB_CALIB_TYPE_1WP,
    AWB_CALIB_TYPE_1WP_SCALE,
    AWB_CALIB_TYPE_2WP,
    AWB_CALIB_TYPE_2WP_SCALE,
    AWB_CALIB_TYPE_3WP,
    AWB_CALIB_TYPE_MAX
} _WBAlg_CALIB_TYPE_e;


typedef struct tagWbCalibGain {
	UINT16 rg, bg, gg;
} WbAlgCalibGain_t;

typedef struct tagWbAlgCalibLightSource {
	WbAlgCalibGain_t D;			/* D65 */
	WbAlgCalibGain_t LightSlab;	/* Light Slab */
	WbAlgCalibGain_t CW;		/* CWF */
	WbAlgCalibGain_t A;			/* INCA */
	WbAlgCalibGain_t Flash;		/* Flash mode */
} WbAlgCalibLightSource_t;

typedef struct tagWbAlgCalibDataInfo {
	UINT8 * ptr;
	UINT32 size;
} WbAlgCalibDataInfo_t;

typedef struct tagWbAlgCustomWbGain{
	UINT16 rgain;
	UINT16 grgain;
	UINT16 bgain;
	UINT16 gbgain;
} WbAlgCustomWbGain_t;

typedef struct tagWbAlgUsrGainsRange {
	float rgain_min;
	float rgain_max;
	float bgain_min;
	float bgain_max;
} WbAlgUsrGainsRange_t;

/**************************************************************************
 * SP5K for V35 Test Struct
 **************************************************************************/
typedef struct {
	UINT16 rgain;
	UINT16 grgain;
	UINT16 bgain;
	UINT16 gbgain;
}sp5kWbGain_t;


/**************************************************************************
 * awb alg init
 * value: invalid
 **************************************************************************/
void appAWBALG_Init( void );

/**************************************************************************
 * Get Awb Mode
 * mode: (type:_AWBALG_MODE_SEL)
 * value: invalid
 **************************************************************************/
void appAWBALG_WBModeGet( UINT32 * mode );

/**************************************************************************
 * Set Awb Mode
 * selector: (type:_AWBALG_MODE_SEL)
 * value: invalid
 **************************************************************************/
void appAWBALG_WBModeSet( UINT32 selector );

/**************************************************************************
 * Get Awb Param
 * selector: (type:_WBAlg_PARA_VAL)
 * pvalue: 		
 **************************************************************************/
void appAWBALG_WBParamGet( UINT32 selector, UINT32 * pvalue );

/**************************************************************************
 * Set Awb Param
 * selector: (type:_WBAlg_PARA_VAL)
 * value: 		
 **************************************************************************/
void appAWBALG_WBParamSet( UINT32 selector, UINT32 value );

/**************************************************************************
 * Awb Snap Post flow
 * addr: raw addr(addr or pt)
 * RawFmt: raw bitNumber
 *			0:8 bit, 1:10bit, 2:12bit, 3:14bit, 4:16bit
 * rawinfo: (type:sp5kImgWbInfo_t*) raw info
 **************************************************************************/
void appAWBALG_CapPostWBSet( UINT32 addr, UINT32 RawFmt, void * rawInfo );

void appAWBALG_CapPostWBSet_SEC( UINT32 addr, UINT32 RawFmt, void * rawInfo );

/**************************************************************************
 * Awb Pv Post flow
 * pinfo: raw info
 * presult: return awb algorithm result
 **************************************************************************/
void appAWBALG_PvCb( const aaaProcInfo_t * pinfo, aaaProcResult_t * presult );

/**************************************************************************
 * create awb info, fill to addr
 * addr: a buf addr for save awb info
 * size: buf max size
 * return: actual data size
 **************************************************************************/
UINT32 appAWBALG_WriteAppn( UINT32 addr, UINT32 size );

/**************************************************************************
 * cmd for awb debug
 * argc: param number
 * arg: cmds
 * v: params
 **************************************************************************/
void appAWBALG_Cmd( UINT32 argc, UINT8 * argv[], UINT32 v[] );
void appAWBALG_4K2KApplyWB(void );

#endif	/* APP_ICATCH_AWB_H */

