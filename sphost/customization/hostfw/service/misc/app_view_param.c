/**************************************************************************
 *
 *       Copyright (c) 2006-2018 by iCatch Technology, Inc.
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
#define HOST_DBG 0
#include "app_com_def.h"
#include "app_view_param.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/**************************************************************************
 *                          L O C A L    D A T A                          *
 **************************************************************************/

param_info_t StillVideoParamInfo[VIEW_PRM_COUNT]={
	DEF_VIEW_PRM_INF(stillSize,				PRM_SAVE,				UI_STILL_SIZE_FULL,			0 ),
	DEF_VIEW_PRM_INF(stillQuality,			PRM_SAVE,				UI_QUALITY_FINE,			0 ),
	DEF_VIEW_PRM_INF(stillDriverMode,		PRM_SAVE,				UI_STILL_DRIVERMODE_OFF,	0 ),
	DEF_VIEW_PRM_INF(stillStamp,			PRM_SAVE,				UI_DATESTAMP_OFF,			0 ),
    DEF_VIEW_PRM_INF(videoSize,				PRM_SAVE,				UI_VIDEO_SIZE_FHD_60FPS,    0 ),
	DEF_VIEW_PRM_INF(videoQuality,    		PRM_SAVE,				UI_QUALITY_FINE,			0 ),
	DEF_VIEW_PRM_INF(videoSeamless,			PRM_SAVE,				UI_VIDEO_SEAMLESS_OFF,		0 ),
	DEF_VIEW_PRM_INF(videoStamp,			PRM_SAVE,				UI_DATESTAMP_OFF,			0 ),
	DEF_VIEW_PRM_INF(metering,				PRM_SAVE,				UI_METERING_MULTI,			0 ),
	DEF_VIEW_PRM_INF(wb,					PRM_SAVE,				UI_WB_AUTO,					0 ),
	DEF_VIEW_PRM_INF(iso,					PRM_SAVE,				UI_ISO_AUTO,				0 ),
	DEF_VIEW_PRM_INF(ev,					PRM_SAVE,				UI_EXP_BIAS_ZERO,			0 ),
	DEF_VIEW_PRM_INF(edge,					PRM_SAVE,				UI_SHARP_NORMAL,			0 ),
	DEF_VIEW_PRM_INF(stablizer,				PRM_SAVE,				UI_STABILIZER_OFF,			0 ),
	DEF_VIEW_PRM_INF(pvflickermode,			PRM_SAVE,				UI_PV_FLICKERMODE_60HZ,		0 ),
	DEF_VIEW_PRM_INF(viddist,				PRM_SAVE,				UI_STABILIZER_OFF,			0 ),
	DEF_VIEW_PRM_INF(vidrsc,				PRM_SAVE,				UI_ROLLING_SHUTTER_OFF,		0 ),
};


ui_param_t ViewParamSet[]={
	{ "UISTILLVIDEO.BIN", 	NULL,		StillVideoParamInfo },
};

