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
 ****************************************************************************/
 /*--------------------------------------------------------------------------*
 *  File name	: 	App_i2c_lib_icatch.c
 *  Function 	: 	i2c library	from icatch  	
 *  First editor	: 	
 *  First date  	: 	
 *  History		:
 *  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

#include "app_i2c_lib_icatch.h"
#include "app_i2c_util.h"


/****************************************************************************
 *						C O N S T A N T S										*
 ****************************************************************************/

/****************************************************************************
 *						M A C R O S											*
 ****************************************************************************/

/****************************************************************************
 *						D A T A    T Y P E S									*
 ****************************************************************************/

/****************************************************************************
 *						G L O B A L    D A T A									*
 ****************************************************************************/

/****************************************************************************
 *						E X T E R N A L    R E F E R E N C E S						*
 ****************************************************************************/
/*
extern void sensorSingleWrite(UINT16 addr,UINT16 data);
extern UINT16 sensorRead(UINT16 addr);
*/
/****************************************************************************
 *						F U N C T I O N    D E C L A R A T I O N S					*
 ****************************************************************************/

/*--------------------------------------------------------------------------*
* Function name : ISPI2C_BandSelectionSet
* Function		: I2C BandSelection Set
* Return value	: void
* Parameter1	: ucParam (UINT8 )
*			  0x00	Reserved
*     			  0x01	50
*     			  0x02	60
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_BandSelectionSet(UINT8 ucParam)
{
	BB_I2CDataWrite(SP7K_MSG_BANDING_SELECTION, ucParam);
}

/*--------------------------------------------------------------------------*
* Function name : ISPI2C_ColorEffectSet
* Function		: I2C Color Effect Set
* Return value	: void
* Parameter1	: ucParam (UINT8 )
*			      0x00	Normal
*     			      0x01	Aqua
*     			      0x02	Negative
*     			      0x03	Sepia
*   			      0x04	Grayscale
*			      0x05	Vivid
*			      0x06	Aura
*			      0x07	Vintage
*			      0x08	Vintage2
*			      0x09	Lomo
*			      0x0A	Red
*			      0x0B	Blue
*			      0x0C	Green
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_ColorEffectSet(UINT8 ucParam)
{
	BB_I2CDataWrite(SP7K_MSG_COLOR_EFFECT, ucParam);
}

/*--------------------------------------------------------------------------*
* Function name :  ISPI2C_EVSet
* Function		: I2C EV Set
* Return value	: void
* Parameter1	: ucParam (UINT8 )
*			     0x00	+2.0
*			     0x01	+1.7
*			     0x02	+1.3
*			     0x03	+1.0
*			     0x04	+0.7
*			     0x05	+0.3
*			     0x06	0
*			     0x07	-0.3
*			     0x08	-0.7
*			     0x09	-1.0
*			     0x0A	-1.3
*			     0x0B	-1.7
*			     0x0C	-2.0
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_EVSet(UINT8 ucParam)
{
	BB_I2CDataWrite(SP7K_MSG_EV_COMPENSATION, ucParam);
}

/*--------------------------------------------------------------------------*
* Function name :  ISPI2C_FlashModeSet
* Function		: I2C Flash Mode Set
* Return value	: void
* Parameter1	: ucParam (UINT8 )
*			     0x00	Auto
*			     0x01	Off
*			     0x02	On
*			     0x03	Reserved
*			     0x04	Torch
*			     0xFF	Main Flash(100%, once)
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_FlashModeSet(UINT8 ucParam)
{
	BB_I2CDataWrite(SP7K_MSG_FLASH_MODE, ucParam);
}

/*--------------------------------------------------------------------------*
* Function name :  ISPI2C_FocusModeSet
* Function		: I2C Focus Mode Set
* Return value	: void
* Parameter1	: ucParam (UINT8 )
*			     0x00	Auto
*			     0x01	Macro
*			     0x02	Infinity Fixed
*			     0x03	Continuous AF
*			     0x04	Full Search
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_FocusModeSet(UINT8 ucParam)
{
	BB_I2CDataWrite(SP7K_MSG_FOCUS_MODE, ucParam);
}

/*--------------------------------------------------------------------------*
* Function name :  ISPI2C_PreviewSizeSet
* Function		: I2C Preview Size Set
* Return value	: void
* Parameter1	: ucParam (UINT8 )
*			     0x00	1280x960
*			     0x01	3264x2448
*			     0x02	1920x1080
*			     0x03	320x240(reserved)
*			     0x04	1280x720(reserved)
*			     0x05	1040x780
*			     0x06	2080x1560
*			     0x07	3648x2736
*			     0x08	4160x3120
*			     0x09	3360x1890
*			     0x0A	2592x1944
*			     0x0B	640x480
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_PreviewSizeSet(UINT8 ucParam)
{
	BB_I2CDataWrite(SP7K_MSG_PV_SIZE_SETTING, ucParam);
}
#if 0
/*-------------------------------------------------------------------------
 *  Function Name : ISPI2C_CaptureSizeSet
 *  ucParam:
     0x00	ID0
     0x01	ID1
     0x02	ID2
     0x03	ID3
     0x04	ID4
     0x05	ID5
 *  Return : None
 *------------------------------------------------------------------------*/
void ISPI2C_CaptureSizeSet(UINT8 ucParam)
{
	BB_I2CDataWrite(SP7K_MSG_CP_SIZE_SETTING, ucParam);
}
#endif

/*--------------------------------------------------------------------------*
* Function name :  ISPI2C_SceneModeSet
* Function		: I2C Scene Size Set
* Return value	: void
* Parameter1	: ucParam (UINT8 )
*			     0x00	Reserved
*			     0x01	Action
*			     0x02	Text
*			     0x03	Beach
*			     0x04	Candle light
*			     0x05	Firework
*			     0x06	Landscape
*			     0x07	Night
*			     0x08	Night Portrait
*			     0x09	Party
*			     0x0A	Portrait
*			     0x0B	Snow
*			     0x0C	Sport
*			     0x0D	Anti-shake
*			     0x0E	Sunset
*			     0x0F	High Sensitivity
*			     0x10	Landscape Portrait
*			     0x11	Kids
*			     0x12	Pet
*			     0x13	Flower
*			     0x14	Soft Flowing Water
*			     0x15	Food
*			     0x16	Backlight
*			     0x17	Indoor
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_SceneModeSet(UINT8 ucParam)
{
	BB_I2CDataWrite(SP7K_MSG_SCENE_SETTING, ucParam);
}

/*--------------------------------------------------------------------------*
* Function name :  ISPI2C_WhiteBalanceSet
* Function		: I2C White Balance Set
* Return value	: void
* Parameter1	: ucParam (UINT8 )
* 			     0x00	Auto
* 			     0x01	Daylight
* 			     0x02	Cloudy
* 			     0x03	Shade
* 			     0x04	Fluorescent_L
* 			     0x05	Fluorescent_H
* 			     0x06	Tungsten
* 			     0x07	Reserved
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/
void ISPI2C_WhiteBalanceSet(UINT8 ucParam)
{
	BB_I2CDataWrite(SP7K_MSG_WB_SETTING, ucParam);
}

/*--------------------------------------------------------------------------*
* Function name :  ISPI2C_ZoomRatioSet
* Function		: I2C Zoom Ratio Set
* Return value	: void
* Parameter1	: usValue (UINT8 zoom factor value)
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/
void ISPI2C_ZoomRatioSet(UINT16 usValue)
{
	BB_I2CDataWrite(SP7K_MSG_PV_DIGITAL_ZOOM_H, usValue>>8);
	BB_I2CDataWrite(SP7K_MSG_PV_DIGITAL_ZOOM_L, usValue&0x00FF);
	//sensor_event_wait(1UL<<16 /* SENSOR_EVENT_CDSP_EOF */);
}

/*--------------------------------------------------------------------------*
* Function name :  ISPI2C_SensorSelectSet
* Function		: I2C Sensor Select Set
* Return value	: void
* Parameter1	: ucParam (UINT8)
*			     0x00	Back sensor
*			     0x01	Front sensor
*			     0x02	Calibration
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_SensorSelectSet(UINT8 ucParam)
{
	UINT32 temp;
	/*BB_I2CDataWrite(SP7K_MSG_SENSOR_SELECT, ucParam);*/
	BB_I2CDataWrite(0x1011, 1); /* CPU reset */

	BB_I2CDataWrite(0x101C, 0x1A);
	BB_I2CDataWrite(0x1140, 0x00);
	BB_I2CDataWrite(0x1150, 0x00);
	BB_I2CDataWrite(0x1160, 0x00);
	BB_I2CDataWrite(0x1170, 0x00);
	BB_I2CDataWrite(0x1180, 0x00);
	BB_I2CDataWrite(0x1190, 0x00);
	BB_I2CDataWrite(0x1200, 0x02);
	BB_I2CDataWrite(0x1310, 0x02);
	BB_I2CDataWrite(0x1008, 0x00);
	BB_I2CDataWrite(0x1009, 0x00);
	BB_I2CDataWrite(0x100C, 0x00);
	BB_I2CDataWrite(0x100D, 0x00);
	BB_I2CDataWrite(0x100E, 0x00);
	BB_I2CDataWrite(0x100F, 0x00);
	BB_I2CDataWrite(0x1018, 0x00);
	BB_I2CDataWrite(0x1004, 0xFF);
	BB_I2CDataWrite(0x1005, 0xFF);
	BB_I2CDataWrite(0x1006, 0xFF);
	BB_I2CDataWrite(0x1007, 0xFF);

	BB_I2CDataWrite(0x7010, 0x07);
	BB_I2CDataWrite(0x7008, 0x00);
	BB_I2CDataWrite(0x7009, 0x00);
	BB_I2CDataWrite(0x700C, 0x00);
	BB_I2CDataWrite(0x700F, 0x00);

	BB_I2CDataWrite(0x941c, 0x00); /* disable LVDS */
	BB_I2CDataWrite(0x9010, 1); /* sensor control reset */
	BB_I2CDataWrite(0x9010, 0);
	BB_I2CDataWrite(0x1306, ucParam); /* Set which firmware will be loaded */
	/* turn off unused sensor, this pare can be omitted, since 7002 fw has covered this part*/
	if (ucParam == 0) { /* switch to back sensor, make front sensor power down */
		temp = BB_I2CDataRead(0x9033);
		temp &= ~0x04;
		BB_I2CDataWrite(0x9033, temp);
	}
	else { /* switch to front sensor, make back sensor power down */
		temp = BB_I2CDataRead(0x9032);
		temp &= ~0x30;
		BB_I2CDataWrite(0x9032, temp);
	}
	BB_I2CDataWrite(0x1011, 0);
}

/*--------------------------------------------------------------------------*
* Function name :  ISPI2C_AEModeSet
* Function		: I2C AE Mode Set
* Return value	: void
* Parameter1	: ucParam (UINT8)
*			     0x00	Multi
*			     0x01	Center
*			     0x02	Spot
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/
void ISPI2C_AEModeSet(UINT8 ucParam)
{
	BB_I2CDataWrite(SP7K_MSG_AE_MODE, ucParam);
}

/*--------------------------------------------------------------------------*
* Function name :  ISPI2C_CaptureModeSet
* Function		: I2C Capture Mode Set
* Return value	: void
* Parameter1	: ucParam (UINT8)
*			     0x00	Single
*			     0x01	HDR
*			     0x02	Reserved
*			     0x03	Burst capture(Unlimited)(ZSL)
*			     0x04	Burst capture(Unlimited)(mode switch)
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_CaptureModeSet(UINT8 ucParam)
{
	BB_I2CDataWrite(SP7K_MSG_CAPTURE_MODE, ucParam);
}

/*--------------------------------------------------------------------------*
* Function name :  ISPI2C_ISOSet
* Function		: I2C ISO Set
* Return value	: void
* Parameter1	: ucParam (UINT8)
*			     0x00	Auto
*			     0x01	ISO 50
*			     0x02	ISO 100
*			     0x03	ISO 200
*			     0x04	ISO 400
*			     0x05	ISO 800
*			     0x06	ISO 1600
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_ISOSet(UINT8 ucParam)
{
	BB_I2CDataWrite(SP7K_MSG_CAPTURE_ISO, ucParam);
}
#if 0
/*-------------------------------------------------------------------------
 *  Function Name : ISPI2C_BrightnessSet
 *  ucValue: the brightness level
 *  Return : None
 *------------------------------------------------------------------------*/
void ISPI2C_BrightnessSet(UINT8 ucValue)
{
	BB_I2CDataWrite(SP7K_MSG_BRIGHTNESS, ucValue);
}

/*-------------------------------------------------------------------------
 *  Function Name : ISPI2C_ContrastSet
 *  ucValue: the contrast level
 *  Return : None
 *------------------------------------------------------------------------*/
void ISPI2C_ContrastSet(UINT8 ucValue)
{
	BB_I2CDataWrite(SP7K_MSG_CONTRAST, ucValue);
}

/*-------------------------------------------------------------------------
 *  Function Name : ISPI2C_SaturationSet
 *  ucValue: the saturation level
 *  Return : None
 *------------------------------------------------------------------------*/
void ISPI2C_SaturationSet(UINT8 ucValue)
{
	BB_I2CDataWrite(SP7K_MSG_SATURATION, ucValue);
}
#endif

/*--------------------------------------------------------------------------*
* Function name :  ISPI2C_AuraColorIndexSet
* Function		: I2C Aura Color Index Set
* Return value	: void
* Parameter1	: ucParam (UINT8)
*			     Effect bit : bit 0~5
*			     1 : Color enable
*			     0 : Color disable
*			     bit 0 : Red
*			     bit 1 : Orange
*			     bit 2 : Yellow
*			     bit 3 : Green
*			     bit 4 : Blue
*			     bit 5 : Violet
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_AuraColorIndexSet(UINT8 ucParam)
{
	BB_I2CDataWrite(SP7K_MSG_AURA_COLOR_INDEX, ucParam);
}

/*--------------------------------------------------------------------------*
* Function name : ISPI2C_OpticalZoomSet
* Function		: I2C Optical Zoom Set
* Return value	: void
* Parameter1	: ucParam (UINT8)
*			     0x00	Zoom in
*			     0x01	Zoom out
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_OpticalZoomSet(UINT8 ucParam)
{
	BB_I2CDataWrite(SP7K_MSG_OPTICAL_ZOOM, ucParam);
}

/*--------------------------------------------------------------------------*
* Function name : ISPI2C_IspFuncSet
* Function		: I2C Isp Function Set
* Return value	: void
* Parameter1	: ucParam (UINT8)
*			     Effect bit : bit 0~2
*			     1 : function enable
*			     0 : function disable
*			     bit 0 : DWDR
*			     bit 1 : Edge information
*			     bit 2 : Re-focus
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_IspFuncSet(UINT8 ucParam)
{
	BB_I2CDataWrite(SP7K_MSG_ISP_FUNCTION, ucParam);
}

/*--------------------------------------------------------------------------*
* Function name : ISPI2C_PreviewCapModeSet
* Function		: I2C  Preview Capture Mode Set
* Return value	: void
* Parameter1	: ucParam (UINT8)
*			     0x00	Preview mode
*			     0x01	Capture mode(mode switch)
*			     0x02	Capture mode(ZSL)
*			     0x03	Capture mode(Non-ZSL)
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_PreviewCapModeSet(UINT8 ucParam)
{
	BB_I2CDataWrite(SP7K_MSG_PV_MODE_SET, ucParam);
}

/*--------------------------------------------------------------------------*
* Function name : ISPI2C_PreviewStreamSet
* Function		: I2C Preview Stream Set
* Return value	: void
* Parameter1	: ucParam (UINT8)
*			     0x00	Stream off
*			     0x01	Stream on
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_PreviewStreamSet(UINT8 ucParam)
{
	BB_I2CDataWrite(SP7K_MSG_PV_STREAM_SETTING, ucParam);
}

/*--------------------------------------------------------------------------*
* Function name : ISPI2C_AbortBurstSet
* Function		: I2C Abort Burst Set
* Return value	: void
* Parameter1	: ucParam (UINT8)
*			     0x00	Burst abort(mode switch)
*			     0x01	Burst abort(ZSL)
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_AbortBurstSet(UINT8 ucParam)
{
	BB_I2CDataWrite(SP7K_MSG_BURST_ABORT, ucParam);
}

/*--------------------------------------------------------------------------*
* Function name : ISPI2C_FvFixFrameRateSet
* Function		: I2C Fv Fix Frame Rate Set
* Return value	: void
* Parameter1	: ucValue (UINT8)
*			     0x00	Disable Fix Frame Rate function
*			     0x01~0xFF : Frame Rate
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_FvFixFrameRateSet(UINT8 ucValue)
{
	BB_I2CDataWrite(SP7K_MSG_FV_FIX_FRAME_RATE, ucValue);
}

/*--------------------------------------------------------------------------*
* Function name : ISPI2C_PvMaxExposureTimeSet
* Function		: I2C Preview Max Exposure Time Set
* Return value	: void
* Parameter1	: ucValue (UINT8)
*			     N=0 : Disable Max. Exposure Time function
*			     N=0x01~0xFF : Max. Exposure time
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_PvMaxExposureTimeSet(UINT8 ucValue)
{
	BB_I2CDataWrite(SP7K_MSG_PV_MAX_EXPOSURE_TIME_SET, ucValue);
}

/*--------------------------------------------------------------------------*
* Function name : ISPI2C_GyroStatusSet
* Function		: I2C Gyro Status Set
* Return value	: void
* Parameter1	: ucParam (UINT8)
*			     0x00	Stop
*			     0x01	Moving
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_GyroStatusSet(UINT8 ucParam)
{
	BB_I2CDataWrite(SP7K_MSG_GYRO_STATUS_SET, ucParam);
}

/*--------------------------------------------------------------------------*
* Function name : ISPI2C_HdrPositiveEvSet
* Function		: I2C HDR Positive Ev Set
* Return value	: void
* Parameter1	: ucValue (UINT8 2.4bits)
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_HdrPositiveEvSet(UINT8 ucValue)
{
	BB_I2CDataWrite(SP7K_MSG_HDR_POSITIVE_EV_SET, ucValue);
}

/*--------------------------------------------------------------------------*
* Function name :  ISPI2C_HdrNegativeEvSet
* Function		: I2C HDR Negative Ev Set
* Return value	: void
* Parameter1	: ucValue (UINT8 2.4bits)
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_HdrNegativeEvSet(UINT8 ucValue)
{
	BB_I2CDataWrite(SP7K_MSG_HDR_NEGATIVE_EV_SET, ucValue);
}
#if 0	/*	not implement rolling shutter */
/*-------------------------------------------------------------------------
 *  Function Name : ISPI2C_RollingShutterSet
 *  Input : None
 *  Return : None
 *------------------------------------------------------------------------*/
void ISPI2C_RollingShutterSet(void)
{
	BB_I2CDataWrite(SP7K_MSG_ROLLING_SHUTTER_HSTARTH, 0x00);
	BB_I2CDataWrite(SP7K_MSG_ROLLING_SHUTTER_HSTARTL, 0x00);
	BB_I2CDataWrite(SP7K_MSG_ROLLING_SHUTTER_HSTEPINIH, 0x00);
	BB_I2CDataWrite(SP7K_MSG_ROLLING_SHUTTER_HSTEPINIL, 0x00);
	BB_I2CDataWrite(SP7K_MSG_ROLLING_SHUTTER_HSTEPH, 0x00);
	BB_I2CDataWrite(SP7K_MSG_ROLLING_SHUTTER_HSTEPL, 0x00);
	BB_I2CDataWrite(SP7K_MSG_ROLLING_SHUTTER_HSIZEH, 0x00);
	BB_I2CDataWrite(SP7K_MSG_ROLLING_SHUTTER_HSIZEL, 0x00);
	BB_I2CDataWrite(SP7K_MSG_ROLLING_SHUTTER_VSTARTH, 0x00);
	BB_I2CDataWrite(SP7K_MSG_ROLLING_SHUTTER_VSTARTL, 0x00);
	BB_I2CDataWrite(SP7K_MSG_ROLLING_SHUTTER_VSIZEH, 0x0F);
	BB_I2CDataWrite(SP7K_MSG_ROLLING_SHUTTER_VSIZEL, 0xFF);
}
#endif

/*--------------------------------------------------------------------------*
* Function name :  ISPI2C_AFROIInfoSet
* Function		: I2C AF ROI Info Set
* Return value	: void
* Parameter1	: usSize (UINT16 )
* Parameter2	: usX AF ROI x (UINT16 )
* Parameter3	: usY AF ROI y (UINT16 )
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/
void ISPI2C_AFROIInfoSet(UINT16 usSize, UINT16 usX, UINT16 usY)
{
	BB_I2CDataWrite(SP7K_MSG_AF_ROI_SIZE_H, usSize>>8);
	BB_I2CDataWrite(SP7K_MSG_AF_ROI_SIZE_L, usSize&0x00FF);
	BB_I2CDataWrite(SP7K_MSG_AF_ROI_X_H, usX>>8);
	BB_I2CDataWrite(SP7K_MSG_AF_ROI_X_L, usX&0x00FF);
	BB_I2CDataWrite(SP7K_MSG_AF_ROI_Y_H, usY>>8);
	BB_I2CDataWrite(SP7K_MSG_AF_ROI_Y_L, usY&0x00FF);
}

/*--------------------------------------------------------------------------*
* Function name :  ISPI2C_AFROITriggerSet
* Function		: I2C AF ROI Trigger Set
* Return value	: void
* Parameter1	: void
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_AFROITriggerSet(void)
{
	BB_I2CDataWrite(SP7K_MSG_AF_ROI_TRIGGER, 0x01);
}

/*--------------------------------------------------------------------------*
* Function name :  ISPI2C_AEROIInfoSet
* Function		: I2C AE ROI Info Set
* Return value	: void
* Parameter1	: usSize (UINT16 )
* Parameter2	: usX AE ROI x (UINT16 )
* Parameter3	: usY AE ROI y (UINT16 )
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_AEROIInfoSet(UINT16 usSize, UINT16 usX, UINT16 usY)
{
	BB_I2CDataWrite(SP7K_MSG_AE_ROI_SIZE_H, usSize>>8);
	BB_I2CDataWrite(SP7K_MSG_AE_ROI_SIZE_L, usSize&0x00FF);
	BB_I2CDataWrite(SP7K_MSG_AE_ROI_X_H, usX>>8);
	BB_I2CDataWrite(SP7K_MSG_AE_ROI_X_L, usX&0x00FF);
	BB_I2CDataWrite(SP7K_MSG_AE_ROI_Y_H, usY>>8);
	BB_I2CDataWrite(SP7K_MSG_AE_ROI_Y_L, usY&0x00FF);
}

/*--------------------------------------------------------------------------*
* Function name :  ISPI2C_AEROITriggerSet
* Function		: I2C AE ROI Trigger Set
* Return value	: void
* Parameter1	: ucParam (UINT8 )
*			     0x00	TAE off
*			     0x01	TAE on (Use TAE ROI : 0x48~0x4D)
*			     0x02	TAE on (Use TAF ROI : 0x40~0x45)
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_AEROITriggerSet(UINT8 ucParam)
{
	BB_I2CDataWrite(SP7K_MSG_AE_ROI_TRIGGER, ucParam);
}

/*--------------------------------------------------------------------------*
* Function name : ISPI2C_AFAbortSet
* Function		: I2C AF Abort Set
* Return value	: void
* Parameter1	: void
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_AFAbortSet(void)
{
	BB_I2CDataWrite(SP7K_MSG_AF_ABORT, 0x00);
}

/*--------------------------------------------------------------------------*
* Function name : ISPI2C_VcmStepSet
* Function		: I2C Vcm Step Set
* Return value	: void
* Parameter1	: usValue- VCM step value (UINT16 0~1023)
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_VcmStepSet(UINT16 usValue)
{
	BB_I2CDataWrite(SP7K_MSG_VCM_STEP_L, usValue&0x00FF);
	BB_I2CDataWrite(SP7K_MSG_VCM_STEP_H, usValue>>8);
}

/*--------------------------------------------------------------------------*
* Function name : ISPI2C_CalibrationRawPathSet
* Function		: I2C Calibration Raw Path Set
* Return value	: void
* Parameter1	: ucParam (UINT8)
*			     0x00	After Front
*			     0x01	After WB
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_CalibrationRawPathSet(UINT8 ucParam)
{
	BB_I2CDataWrite(SP7K_MSG_CALIBRATION_RAW_PATH, ucParam);
}

/*--------------------------------------------------------------------------*
* Function name : ISPI2C_CalibrationRawFormatSet
* Function		: I2C Calibration Raw Format Set
* Return value	: void
* Parameter1	: ucParam (UINT8)
*			     0x00	RAW8
*			     0x01	RAW10
*			     0x02	RAW12
*			     0x03	YUV8
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_CalibrationRawFormatSet(UINT8 ucParam)
{
	BB_I2CDataWrite(SP7K_MSG_CALIBRATION_RAW_FORMAT, ucParam);
}

/*--------------------------------------------------------------------------*
* Function name : ISPI2C_CalibrationRawLinearSet
* Function		: I2C Calibration Raw Linear Set
* Return value	: void
* Parameter1	: ucParam (UINT8)
*			     0x00	Off
*			     0x01	On
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_CalibrationRawLinearSet(UINT8 ucParam)
{
	BB_I2CDataWrite(SP7K_MSG_CALIBRATION_RAW_LINEAR, ucParam);
}

/*--------------------------------------------------------------------------*
* Function name : ISPI2C_CalibrationRawObSet
* Function		: I2C Calibration Raw Ob Set
* Return value	: void
* Parameter1	: ucParam (UINT8)
*			     0x00	Off
*			     0x01	On
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_CalibrationRawObSet(UINT8 ucParam)
{
	BB_I2CDataWrite(SP7K_MSG_CALIBRATION_RAW_OB, ucParam);
}

/*--------------------------------------------------------------------------*
* Function name : ISPI2C_CalibrationRawBpSet
* Function		: I2C Calibration Raw Bp Set
* Return value	: void
* Parameter1	: ucParam (UINT8)
*			     0x00	Off
*			     0x01	On
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/
void ISPI2C_CalibrationRawBpSet(UINT8 ucParam)
{
	BB_I2CDataWrite(SP7K_MSG_CALIBRATION_RAW_BP, ucParam);
}

/*--------------------------------------------------------------------------*
* Function name : ISPI2C_CalibrationRawLscSet
* Function		: I2C Calibration Raw Lsc Set
* Return value	: void
* Parameter1	: ucParam (UINT8)
*			     0x00	Off
*			     0x01	On
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/
void ISPI2C_CalibrationRawLscSet(UINT8 ucParam)
{
	BB_I2CDataWrite(SP7K_MSG_CALIBRATION_RAW_LSC, ucParam);
}

/*--------------------------------------------------------------------------*
* Function name : ISPI2C_CalibrationRawCaSet
* Function		: I2C Calibration Raw Ca Set
* Return value	: void
* Parameter1	: ucParam (UINT8)
*			     0x00	Off
*			     0x01	On
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/
void ISPI2C_CalibrationRawCaSet(UINT8 ucParam)
{
	BB_I2CDataWrite(SP7K_MSG_CALIBRATION_RAW_CA, ucParam);
}


/*--------------------------------------------------------------------------*
* Function name : ISPI2C_CalibrationRawArdSet
* Function		: I2C Calibration Raw Ard Set
* Return value	: void
* Parameter1	: ucParam (UINT8)
*			     0x00	Off
*			     0x01	On
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/
void ISPI2C_CalibrationRawArdSet(UINT8 ucParam)
{
	BB_I2CDataWrite(SP7K_MSG_CALIBRATION_RAW_ARD, ucParam);
}

/*--------------------------------------------------------------------------*
* Function name :ISPI2C_CalibrationRawDepeakSet
* Function		: I2C Calibration Raw Depeak Set
* Return value	: void
* Parameter1	: ucParam (UINT8)
*			     0x00	Off
*			     0x01	On
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_CalibrationRawDepeakSet(UINT8 ucParam)
{
	BB_I2CDataWrite(SP7K_MSG_CALIBRATION_RAW_DEPEAK, ucParam);
}

/*--------------------------------------------------------------------------*
* Function name :ISPI2C_CalibrationRawWbSet
* Function		: I2C Calibration Raw Wb Set
* Return value	: void
* Parameter1	: ucParam (UINT8)
*			     0x00	Off
*			     0x01	On
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_CalibrationRawWbSet(UINT8 ucParam)
{
	BB_I2CDataWrite(SP7K_MSG_CALIBRATION_RAW_WB, ucParam);
}

/*--------------------------------------------------------------------------*
* Function name : ISPI2C_AeAwbThreadModeSet
* Function		: I2C Ae/Awb ThreadMode Set
* Return value	: void
* Parameter1	: ucParam (UINT8)
* 			     0x00	AE/AWB thread off
* 			     0x01	AE/AWB thread on
* 			     0x02	AWB thread off
* 			     0x03	AWB thread on
* 			     0x04	AE thread off
* 			     0x05	AE thread on
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_AeAwbThreadModeSet(UINT8 ucParam)
{
	BB_I2CDataWrite(SP7K_MSG_AEAWBTHREAD_MODE, ucParam);
}


/*--------------------------------------------------------------------------*
* Function name : ISPI2C_DqModeSet
* Function		: I2C DQ Mode Set
* Return value	: void
* Parameter1	: ucParam (UINT8)
* 			     0x00	Pause on
* 			     0x01	Pause off
* 			     0x02	Interpolation Off
* 			     0x03	Interpolation On
* 			     0x04	Fix Color Temperature On
* 			     0x05	Fix Color Temperature Off
* 			     0x06	Fix ISO On
* 			     0x07	Fix ISO Off
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_DqModeSet(UINT8 ucParam)
{
	BB_I2CDataWrite(SP7K_MSG_DQ_MODE, ucParam);
}

/*--------------------------------------------------------------------------*
* Function name : ISPI2C_CdspOnOffModeSet
* Function		: I2C Cdsp On/Off Mode Set
* Return value	: void
* Parameter1	: ucParam (UINT8)
*			     0x00	CDSP off
*			     0x01	CDSP on
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_CdspOnOffModeSet(UINT8 ucParam)
{
	BB_I2CDataWrite(SP7K_MSG_CDSP_MODE, ucParam);
}

/*--------------------------------------------------------------------------*
* Function name : ISPI2C_CalibrationCmd1Set
* Function		: I2C Calibration Command 1 Set
* Return value	: void
* Parameter1	: ucParam (UINT8)
*			     bit 0 : Reserved
*			     bit 1 : OB calibration
*			     bit 2 : WB calibration
*			     bit 3 : AF calibration
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_CalibrationCmd1Set(UINT8 ucParam)
{
	BB_I2CDataWrite(SP7K_MSG_CALIBRATION_CMD1, ucParam);
}

/*--------------------------------------------------------------------------*
* Function name : ISPI2C_CalibrationCmd2Set
* Function		: I2C Calibration Command 2 Set
* Return value	: void
* Parameter1	: ucParam (UINT8)
*			     bit 0 : LSC_D50 calibration
*			     bit 1 : LSC_A calibration
*			     bit 2 : LSC_CW calibration
*			     bit 3 : LSC_TL84 calibration
*			     bit 4 : LSC_D65 calibration
*			     bit 5 : LSC_H calibration
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_CalibrationCmd2Set(UINT8 ucParam)
{
	BB_I2CDataWrite(SP7K_MSG_CALIBRATION_CMD2, ucParam);
}

/*--------------------------------------------------------------------------*
* Function name : ISPI2C_CalibrationInitCmdSet
* Function		: I2C Calibration Command Initial Set
* Return value	: void
* Parameter1	: ucParam (UINT8)
*			     bit 0, 1: Calibration data target
*			     0x02	EEPROM
*			     0x03	SPI
*			     bit 2, 3: Band selection
*			     0x00	50Hz
*			     0x04	60Hz
*			     bit 4, 5: LSC compression method
*			     0x00	Differential
*			     0x10	Non-compression
*			     bit 6, 7: LSC table number
*			     0x00	4 table
*			     0x40	5 table
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_CalibrationInitCmdSet(UINT8 ucParam)
{
	BB_I2CDataWrite(SP7K_MSG_CALIBRATION_INIT_CMD, ucParam);
}


/*--------------------------------------------------------------------------*
* Function name : ISPI2C_CalibrationLscMaxGainSet
* Function		: I2C Calibration Lsc MaxGain Set
* Return value	: void
* Parameter1	: ucVaule (UINT8)
*			     ucVaule : 6.2bits
*			     Range : 0x00~0xFF
*			     0x01	0.25%
*			     0x02	0.50%
*			     0x03	0.75%
*			     0xFF	63.75%
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_CalibrationLscMaxGainSet(UINT8 ucVaule)
{
	BB_I2CDataWrite(SP7K_MSG_CALIBRATION_LENS_MAX_LUM_DECAY, ucVaule);
}


/*--------------------------------------------------------------------------*
* Function name : ISPI2C_DqIdxFixSet
* Function		: I2C Dq Index Fix Set
* Return value	: void
* Parameter1	: ucParam (UINT8)
*		     bit 0~3: Color Temperature Index
*		     0x00	None
*		     0x01	SPI flash is 1MB to load golden LSC & LSC_DQ table, auto
*		     0x02	SPI flash is 1MB to load golden LSC & LSC_DQ table, fix A light table
*		     0x03	SPI flash is 1MB to load golden LSC & LSC_DQ table, fix TL84 table
*		     0x04	SPI flash is 1MB to load golden LSC & LSC_DQ table, fix CW table
*		     0x05	SPI flash is 1MB to load golden LSC & LSC_DQ table, fix D50 table
*		     0x06	SPI flash is 1MB to load golden LSC & LSC_DQ table, fix D65 table
*		     0x07	SPI flash is 1MB to load golden LSC & LSC_DQ table, fix H table
*		     0x08	SPI flash is 1MB to load calibrated LSC & LSC_DQ table, auto
*		     0x09	SPI flash is 1MB to load calibrated LSC & LSC_DQ table, fix A light table
*		     0x0A	SPI flash is 1MB to load calibrated LSC & LSC_DQ table, fix TL84 table
*		     0x0B	SPI flash is 1MB to load calibrated LSC & LSC_DQ table, fix CW table
*		     0x0C	SPI flash is 1MB to load calibrated LSC & LSC_DQ table, fix D50 table
*		     0x0D	SPI flash is 1MB to load calibrated LSC & LSC_DQ table, fix D65 table
*		     0x0E	SPI flash is 1MB to load calibrated LSC & LSC_DQ table, fix H table
*		     bit 4~7: ISO index (0~5)
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_DqIdxFixSet(UINT8 ucParam)
{
	BB_I2CDataWrite(SP7K_MSG_DQ_IDX_FIX, ucParam);
}

/*--------------------------------------------------------------------------*
* Function name : ISPI2C_CaliUtilizationOptionSet
* Function		: I2C CaliUtilization Option Set
* Return value	: void
* Parameter1	: ucParam (UINT8)
*			     bit 0: rear sensor(3ACALI)
*			     0: Use Calibration data(default)
*			     1: Use Golden data
*			     bit 1: rear sensor(LSC)
*			     0: Use Calibration data(default)
*			     1: Use Golden data
*			     bit 2: front sensor(3ACALI)
*			     0: Use Calibration data(default)
*			     1: Use Golden data
*			     bit 3: front sensor(LSC)
*			     0: Use Calibration data(default)
*			     1: Use Golden data
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_CaliUtilizationOptionSet(UINT8 ucParam)
{
	BB_I2CDataWrite(SP7K_MSG_CALI_UTILIZATION_OPTION, ucParam);
}

/*--------------------------------------------------------------------------*
* Function name : ISPI2C_ROISwitchSet
* Function		: I2C ROI Switch Set
* Return value	: void
* Parameter1	: ucParam (UINT8)
*			     0x00	Off
*			     0x01	On
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_ROISwitchSet(UINT8 ucParam)
{
	BB_I2CDataWrite(SP7K_MSG_ROI_SWITCH, ucParam);
}


/*--------------------------------------------------------------------------*
* Function name : ISPI2C_VQVendreqIdSet
* Function		: I2C VQ Vend request Id Set
* Return value	: void
* Parameter1	: ucParam (UINT8)
*			     0x03	AF command
*			     0x04	AE command
*			     0x05	AWB command
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_VQVendreqIdSet(UINT8 ucParam)
{
	BB_I2CDataWrite(APP_VQ_MSG_VENDREQ_ID, ucParam);
}

/*--------------------------------------------------------------------------*
* Function name : ISPI2C_VQVendreqParamSet
* Function		: I2C VQ Vend request Param Set
* Return value	: void
* Parameter1	: ucParam (UINT8)
*			     0x01	AE or AWB lock
*			     0x02	AE or AWB unlock
*			     0x04	Set exposure line
*			     0x06	Set agc
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_VQVendreqParamSet(UINT8 ucParam)
{
	BB_I2CDataWrite(APP_VQ_MSG_VENDREQ_PARAM, ucParam);
}


/*--------------------------------------------------------------------------*
* Function name :  ISPI2C_VQVendreqValuesSet_L
* Function		: I2C VQ Vend request the low byte Values Set
* Return value	: void
* Parameter1	: ucVaule -the low byte of value(UINT8)
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_VQVendreqValuesSet_L(UINT8 ucVaule)
{
	BB_I2CDataWrite(APP_VQ_MSG_VENDREQ_VALUES, ucVaule);
}

/*--------------------------------------------------------------------------*
* Function name :  ISPI2C_VQVendreqProcessSet
* Function		: I2C VQ Vend process Set
* Return value	: void
* Parameter1	: void
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_VQVendreqProcessSet(void)
{
	BB_I2CDataWrite(APP_VQ_MSG_VENDREQ_PROCESS, 0x00);
}

/*--------------------------------------------------------------------------*
* Function name :  ISPI2C_VQVendreqValuesSet_H
* Function		: I2C VQ Vend request the High byte Values Set
* Return value	: void
* Parameter1	: ucVaule -the High byte of value(UINT8)
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_VQVendreqValuesSet_H(UINT8 ucVaule)
{
	BB_I2CDataWrite(APP_VQ_MSG_VENDREQ_VALUES_H, ucVaule);
}

/*--------------------------------------------------------------------------*
* Function name :   ISPI2C_VQVendreqAEAWB_Unlock
* Function		: I2C VQ Vend request AE/AWB Unlock
* Return value	: void
* Parameter1	: ucParam (UINT8)
*			     0x03	AE lock
*			     0x04	AWB lock
*			     0x05	AE unlock
*			     0x06	AWB unlock
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void ISPI2C_VQVendreqAEAWB_Unlock(UINT8 ucParam)
{
	BB_I2CDataWrite(APP_VQ_MSG_UNLOCK_AEAWB, ucParam);
}


/*--------------------------------------------------------------------------*
* Function name :   ISPI2C_GetAFStatus
* Function		: I2C AF status get
* Return value	: AF status(UINT8)
*			     0x00	Idle
*			     0x01	Busy
* Parameter1	: void
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

UINT8 ISPI2C_GetAFStatus(void)
{
	return BB_I2CDataRead(SP7K_RDREG_AF_STATUS);
}

/*--------------------------------------------------------------------------*
* Function name :   ISPI2C_GetAFResult
* Function		: I2C AF status get
* Return value	: AF status(UINT8)
*			     0x00	Focus success
*			     0x01	Focus fail
* Parameter1	: void
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

UINT8 ISPI2C_GetAFResult(void)
{
	return BB_I2CDataRead(SP7K_RDREG_AF_RESULT);
}

/*--------------------------------------------------------------------------*
* Function name :   ISPI2C_GetExposureTimeNumerator
* Function		: I2C Get Exposure Time Numerator
* Return value	: exposure time numerator(UINT8)
* Parameter1	: void
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

UINT8 ISPI2C_GetExposureTimeNumerator(void)
{
	return BB_I2CDataRead(SP7K_RDREG_EXPTIME_NUMERATOR);
}

/*--------------------------------------------------------------------------*
* Function name :   ISPI2C_GetExposureTimeDenominator
* Function		: I2C Get Exposure Time Denominator
* Return value	: exposure time denominator(UINT8)
* Parameter1	: void
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

UINT32 ISPI2C_GetExposureTimeDenominator(void)
{
	return ((BB_I2CDataRead(SP7K_RDREG_EXPTIME_DENOMINATOR_H)<<16) |
			(BB_I2CDataRead(SP7K_RDREG_EXPTIME_DENOMINATOR_M)<<8) |
			 BB_I2CDataRead(SP7K_RDREG_EXPTIME_DENOMINATOR_L));
}

/*--------------------------------------------------------------------------*
* Function name :   ISPI2C_GetExposureTimeCompensation
* Function		: I2C Get Exposure Time Compensation
* Return value	: exposure time compensation(UINT8)
* Parameter1	: void
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

UINT8 ISPI2C_GetExposureTimeCompensation(void)
{
	return BB_I2CDataRead(SP7K_RDREG_EXPTIME_COMPENSATION);
}

/*--------------------------------------------------------------------------*
* Function name :   ISPI2C_GetFocalLength
* Function		: I2C Get Focal Length
* Return value	: lens focal length(UINT16)
* Parameter1	: void
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

UINT16 ISPI2C_GetFocalLength(void)
{
	return ((BB_I2CDataRead(SP7K_RDREG_FOCAL_LENGTH_H)<<8) | BB_I2CDataRead(SP7K_RDREG_FOCAL_LENGTH_L));
}
/*--------------------------------------------------------------------------*
* Function name :  ISPI2C_GetISO
* Function		: I2C Get Focal Length
* Return value	: ISO value(UINT16)
* Parameter1	: void
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

UINT16 ISPI2C_GetISOValue(void)
{
	return ((BB_I2CDataRead(SP7K_RDREG_ISO_H)<<8) | BB_I2CDataRead(SP7K_RDREG_ISO_L));
}


/*--------------------------------------------------------------------------*
* Function name :  ISPI2C_GetFlashMode
* Function		: I2C Get Flash Mode
* Return value	: flash mode(UINT8)
* 			     0x00	Off
* 			     0x01	On
* Parameter1	: void
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

UINT8 ISPI2C_GetFlashMode(void)
{
	return BB_I2CDataRead(SP7K_RDREG_FLASH_MODE);
}

/*--------------------------------------------------------------------------*
* Function name :  ISPI2C_GetBackSensorId
* Function		: I2C Get Back Sensor Id
* Return value	: back sensor id(UINT16)
* Parameter1	: void
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

UINT16 ISPI2C_GetBackSensorId(void)
{
	return ((BB_I2CDataRead(SP7K_RDREG_BACK_SENSORID_H)<<8) | BB_I2CDataRead(SP7K_RDREG_BACK_SENSORID_L));
}


/*--------------------------------------------------------------------------*
* Function name : ISPI2C_GetFrontSensorId
* Function		: I2C Get Front Sensor Id
* Return value	: Front sensor id(UINT16)
* Parameter1	: void
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

UINT16 ISPI2C_GetFrontSensorId(void)
{
	return ((BB_I2CDataRead(SP7K_RDREG_FRONT_SENSORID_H)<<8) | BB_I2CDataRead(SP7K_RDREG_FRONT_SENSORID_L));
}


/*--------------------------------------------------------------------------*
* Function name :ISPI2C_GetFWVersion
* Function		: I2C Get FW Version
* Return value	: FW Version (UINT32)
* Parameter1	: void
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

UINT32 ISPI2C_GetFWVersion(void)
{
	return ((BB_I2CDataRead(SP7K_RDREG_FW_VER_H)<<16) |
			(BB_I2CDataRead(SP7K_RDREG_FW_VER_M)<<8) |
			 BB_I2CDataRead(SP7K_RDREG_FW_VER_L));
}

/*--------------------------------------------------------------------------*
* Function name :ISPI2C_GetVendorId
* Function		: I2C Get Vendor Id
* Return value	: Vendor Id (UINT16)
* Parameter1	: void
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

UINT16 ISPI2C_GetVendorId(void)
{
	return ((BB_I2CDataRead(SP7K_RDREG_VENDOR_ID_H)<<8) | BB_I2CDataRead(SP7K_RDREG_VENDOR_ID_L));
}


/*--------------------------------------------------------------------------*
* Function name :ISPI2C_GetCalibrationExecStatus1
* Function		: I2C Get Calibration Exec Status1
* Return value	: OB/WB/AF calibration execution status (UINT8)
* Parameter1	: void
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

UINT8 ISPI2C_GetCalibrationExecStatus1(void)
{
	return BB_I2CDataRead(SP7K_RDREG_CALIBRATION_EXEC_STATUS1);
}


/*--------------------------------------------------------------------------*
* Function name :ISPI2C_GetCalibrationExecStatus2
* Function		: I2C Get Calibration Exec Status2
* Return value	: calibration execution status (UINT8)
* 				LSC_D50/LSC_A/LSC_CW/LSC_TL84/LSC_D65/LSC_H
* Parameter1	: void
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

UINT8 ISPI2C_GetCalibrationExecStatus2(void)
{
	return BB_I2CDataRead(SP7K_RDREG_CALIBRATION_EXEC_STATUS2);
}


/*--------------------------------------------------------------------------*
* Function name :ISPI2C_GetCalibrationErrStatus1
* Function		: I2C Get Calibration Error Status1
* Return value	: calibration error status (UINT8)
* 				OB/WB/AF
* Parameter1	: void
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

UINT8 ISPI2C_GetCalibrationErrStatus1(void)
{
	return BB_I2CDataRead(SP7K_RDREG_CALIBRATION_ERR_STATUS1);
}


/*--------------------------------------------------------------------------*
* Function name :ISPI2C_GetCalibrationErrStatus2
* Function		: I2C Get Calibration Error Status2
* Return value	: calibration error status (UINT8)
* 				LSC_D50/LSC_A/LSC_CW/LSC_TL84/LSC_D65/LSC_H
* Parameter1	: void
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

UINT8 ISPI2C_GetCalibrationErrStatus2(void)
{
	return BB_I2CDataRead(SP7K_RDREG_CALIBRATION_ERR_STATUS2);
}


/*--------------------------------------------------------------------------*
* Function name :ISPI2C_GetCalibrationErrCode
* Function		: I2C Get Calibration Error code
* Return value	: calibration error code (UINT8)
* Parameter1	: void
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

UINT8 ISPI2C_GetCalibrationErrCode(void)
{
	return BB_I2CDataRead(SP7K_RDREG_CALIBRATION_ERR_CODE);
}


/*--------------------------------------------------------------------------*
* Function name :ISPI2C_GetLoadTableStatus
* Function		: I2C Get Load Table Status
* Return value	: load table status (UINT8)
* Parameter1	: void
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

UINT8 ISPI2C_GetLoadTableStatus(void)
{
	return BB_I2CDataRead(SP7K_RDREG_LOAD_TABLE_STATUS);
}

