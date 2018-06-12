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
#include <math.h>
#include "app_com_def.h"
#include "app_com_api.h"
#include "app_sys_def.h"

#include "sp5k_cal_api.h"
#include "sp5k_ae_api.h"
#include "sp5k_sensor_api.h"

#include "app_aaa.h"
#include "app_still.h"
#include "app_calib_api.h"
#include "app_lens_api.h"
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
#if KIT_SHUT_CTRL_TIMER
UINT32 CapExpTime;
UINT32 CapShutColseTime;
#endif

UINT32 CapWithFaceCheck=0;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void
_stillUtil_ShutterDelaySet(
	void
)
{


}

void 
appStillUtil_CaptureSet(
	void
)
{
	_stillUtil_ShutterDelaySet();
    
	sp5kModeSet(SP5K_MODE_STILL_SNAP);	
}

void 
appStillUtil_CalibCaptureSet(
	BOOL enable,
	BOOL saveRaw
)
{
	if ( enable ) {
		
		_stillUtil_ShutterDelaySet();
		
		if ( saveRaw ) {
			sp5kCalibrationCfgSet(SP5K_CALIBRATION_CALLBACK, SP5K_CALIB_CB_STILL_RAW_SAVE);
		}
		else {
			sp5kCalibrationCfgSet(SP5K_CALIBRATION_CALLBACK, SP5K_CALIB_CB_STILL_RAW);
		}
	}		
	else {
		if ( saveRaw ) {
			sp5kCalibrationCfgSet(SP5K_CALIBRATION_OFF, SP5K_CALIB_CB_STILL_RAW_SAVE);
		}
		else {
			sp5kCalibrationCfgSet(SP5K_CALIBRATION_OFF, SP5K_CALIB_CB_STILL_RAW);
		}
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appStillUtil_CaptureWithFaceCheck
 *------------------------------------------------------------------------*/
void 
appStillUtil_CaptureWithFaceCheck(
	void
)
{

}

