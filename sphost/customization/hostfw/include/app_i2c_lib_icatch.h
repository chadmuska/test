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
#ifndef APP_I2C_LIB_ICATCH_H
#define APP_I2C_LIB_ICATCH_H


/****************************************************************************
 *						C O N S T A N T S 										*
 ****************************************************************************/
#ifndef NULL
#define NULL  ((void *)0)
#endif
#ifndef SUCCESS
#define SUCCESS	1
#endif
#ifndef FAIL
#define FAIL	0
#endif
#ifndef TRUE
#define TRUE	1
#endif
#ifndef FALSE
#define FALSE	0
#endif

#define UINT64	unsigned long long
#define UINT32	unsigned int
#define UINT16	unsigned short
#define UINT8	unsigned char
#define SINT64	signed long long
#define SINT32	signed int
#define SINT16	signed short
#define SINT8	signed char
#define BOOL	unsigned char

#define SP7K_MSG_TAG	0x7100
#define SP7K_RDREG_BASE	0x7280

/****************************************************************************
 *						D A T A    T Y P E S									*
 ****************************************************************************/
typedef enum {
	SP7K_MSG_BANDING_SELECTION				= (SP7K_MSG_TAG | 0x01),
	SP7K_MSG_COLOR_EFFECT					= (SP7K_MSG_TAG | 0x02),
	SP7K_MSG_EV_COMPENSATION				= (SP7K_MSG_TAG | 0x03),
	SP7K_MSG_FLASH_MODE						= (SP7K_MSG_TAG | 0x04),
	SP7K_MSG_FOCUS_MODE						= (SP7K_MSG_TAG | 0x05),
	SP7K_MSG_PV_SIZE_SETTING				= (SP7K_MSG_TAG | 0x06),
	/*SP7K_MSG_CP_SIZE_SETTING				= (SP7K_MSG_TAG | 0x08),*/
	SP7K_MSG_SCENE_SETTING					= (SP7K_MSG_TAG | 0x09),
	SP7K_MSG_WB_SETTING						= (SP7K_MSG_TAG | 0x0A),
	SP7K_MSG_PV_DIGITAL_ZOOM_H				= (SP7K_MSG_TAG | 0x0B),
	SP7K_MSG_PV_DIGITAL_ZOOM_L				= (SP7K_MSG_TAG | 0x0C),
	SP7K_MSG_AE_MODE						= (SP7K_MSG_TAG | 0x0E),
	SP7K_MSG_CAPTURE_MODE					= (SP7K_MSG_TAG | 0x0F),
	SP7K_MSG_CAPTURE_ISO					= (SP7K_MSG_TAG | 0x10),
#if 0
	SP7K_MSG_BRIGHTNESS						= (SP7K_MSG_TAG | 0x15),
	SP7K_MSG_CONTRAST						= (SP7K_MSG_TAG | 0x16),
	SP7K_MSG_SATURATION						= (SP7K_MSG_TAG | 0x17),
#endif
	SP7K_MSG_AURA_COLOR_INDEX				= (SP7K_MSG_TAG | 0x19),
	SP7K_MSG_OPTICAL_ZOOM					= (SP7K_MSG_TAG | 0x1A),
	SP7K_MSG_ISP_FUNCTION					= (SP7K_MSG_TAG | 0x1B),
	SP7K_MSG_PV_MODE_SET					= (SP7K_MSG_TAG | 0x20),
	SP7K_MSG_PV_STREAM_SETTING				= (SP7K_MSG_TAG | 0x21),
	SP7K_MSG_BURST_ABORT					= (SP7K_MSG_TAG | 0x22),
	SP7K_MSG_FV_FIX_FRAME_RATE				= (SP7K_MSG_TAG | 0x24),
	SP7K_MSG_PV_MAX_EXPOSURE_TIME_SET		= (SP7K_MSG_TAG | 0x25),
	SP7K_MSG_GYRO_STATUS_SET				= (SP7K_MSG_TAG | 0x26),
	SP7K_MSG_HDR_POSITIVE_EV_SET			= (SP7K_MSG_TAG | 0x27),
	SP7K_MSG_HDR_NEGATIVE_EV_SET			= (SP7K_MSG_TAG | 0x28),
#if 0	/*	not implement rolling shutter */
	SP7K_MSG_ROLLING_SHUTTER_HSTARTH		= (SP7K_MSG_TAG | 0x23),
	SP7K_MSG_ROLLING_SHUTTER_HSTARTL		= (SP7K_MSG_TAG | 0x24),
	SP7K_MSG_ROLLING_SHUTTER_HSTEPINIH		= (SP7K_MSG_TAG | 0x25),
	SP7K_MSG_ROLLING_SHUTTER_HSTEPINIL		= (SP7K_MSG_TAG | 0x26),
	SP7K_MSG_ROLLING_SHUTTER_HSTEPH			= (SP7K_MSG_TAG | 0x27),
	SP7K_MSG_ROLLING_SHUTTER_HSTEPL			= (SP7K_MSG_TAG | 0x28),
	SP7K_MSG_ROLLING_SHUTTER_HSIZEH			= (SP7K_MSG_TAG | 0x29),
	SP7K_MSG_ROLLING_SHUTTER_HSIZEL			= (SP7K_MSG_TAG | 0x2A),
	SP7K_MSG_ROLLING_SHUTTER_VSTARTH		= (SP7K_MSG_TAG | 0x2B),
	SP7K_MSG_ROLLING_SHUTTER_VSTARTL		= (SP7K_MSG_TAG | 0x2C),
	SP7K_MSG_ROLLING_SHUTTER_VSIZEH			= (SP7K_MSG_TAG | 0x2D),
	SP7K_MSG_ROLLING_SHUTTER_VSIZEL			= (SP7K_MSG_TAG | 0x2E),
#endif
	SP7K_MSG_AF_ROI_SIZE_H					= (SP7K_MSG_TAG | 0x40),
	SP7K_MSG_AF_ROI_SIZE_L					= (SP7K_MSG_TAG | 0x41),
	SP7K_MSG_AF_ROI_X_H						= (SP7K_MSG_TAG | 0x42),
	SP7K_MSG_AF_ROI_X_L						= (SP7K_MSG_TAG | 0x43),
	SP7K_MSG_AF_ROI_Y_H						= (SP7K_MSG_TAG | 0x44),
	SP7K_MSG_AF_ROI_Y_L						= (SP7K_MSG_TAG | 0x45),
	SP7K_MSG_AF_ROI_TRIGGER					= (SP7K_MSG_TAG | 0x46),
	SP7K_MSG_AE_ROI_SIZE_H					= (SP7K_MSG_TAG | 0x48),
	SP7K_MSG_AE_ROI_SIZE_L					= (SP7K_MSG_TAG | 0x49),
	SP7K_MSG_AE_ROI_X_H						= (SP7K_MSG_TAG | 0x4A),
	SP7K_MSG_AE_ROI_X_L						= (SP7K_MSG_TAG | 0x4B),
	SP7K_MSG_AE_ROI_Y_H						= (SP7K_MSG_TAG | 0x4C),
	SP7K_MSG_AE_ROI_Y_L						= (SP7K_MSG_TAG | 0x4D),
	SP7K_MSG_AE_ROI_TRIGGER					= (SP7K_MSG_TAG | 0x4E),
	SP7K_MSG_AF_ABORT						= (SP7K_MSG_TAG | 0x4F),
	SP7K_MSG_CAP_EDGE_GET					= (SP7K_MSG_TAG | 0x5F),
	SP7K_MSG_VCM_STEP_L						= (SP7K_MSG_TAG | 0x5C),
	SP7K_MSG_VCM_STEP_H						= (SP7K_MSG_TAG | 0x5D),
	SP7K_MSG_CALIBRATION_RAW_PATH			= (SP7K_MSG_TAG | 0x60),
	SP7K_MSG_CALIBRATION_RAW_FORMAT			= (SP7K_MSG_TAG | 0x61),
	SP7K_MSG_CALIBRATION_RAW_LINEAR			= (SP7K_MSG_TAG | 0x62),
	SP7K_MSG_CALIBRATION_RAW_OB				= (SP7K_MSG_TAG | 0x63),
	SP7K_MSG_CALIBRATION_RAW_BP				= (SP7K_MSG_TAG | 0x64),
	SP7K_MSG_CALIBRATION_RAW_LSC			= (SP7K_MSG_TAG | 0x65),
	SP7K_MSG_CALIBRATION_RAW_CA				= (SP7K_MSG_TAG | 0x66),
	SP7K_MSG_CALIBRATION_RAW_ARD			= (SP7K_MSG_TAG | 0x67),
	SP7K_MSG_CALIBRATION_RAW_DEPEAK			= (SP7K_MSG_TAG | 0x68),
	SP7K_MSG_CALIBRATION_RAW_WB				= (SP7K_MSG_TAG | 0x69),
	SP7K_MSG_AEAWBTHREAD_MODE				= (SP7K_MSG_TAG | 0x70),
	SP7K_MSG_DQ_MODE						= (SP7K_MSG_TAG | 0x71),
	SP7K_MSG_CDSP_MODE						= (SP7K_MSG_TAG | 0x72),
	SP7K_MSG_CALIBRATION_CMD1				= (SP7K_MSG_TAG | 0x74),
	SP7K_MSG_CALIBRATION_CMD2				= (SP7K_MSG_TAG | 0x75),
	SP7K_MSG_CALIBRATION_INIT_CMD			= (SP7K_MSG_TAG | 0x76),
	SP7K_MSG_CALIBRATION_LENS_MAX_LUM_DECAY	= (SP7K_MSG_TAG | 0x77),
	SP7K_MSG_DQ_IDX_FIX						= (SP7K_MSG_TAG | 0x78),
	SP7K_MSG_CALI_UTILIZATION_OPTION		= (SP7K_MSG_TAG | 0x7F),
	SP7K_MSG_ROI_SWITCH						= (SP7K_MSG_TAG | 0x88),

	APP_VQ_MSG_VENDREQ_ID					= (SP7K_MSG_TAG | 0xE4),
	APP_VQ_MSG_VENDREQ_PARAM				= (SP7K_MSG_TAG | 0xE5),
	APP_VQ_MSG_VENDREQ_VALUES				= (SP7K_MSG_TAG | 0xE6),
	APP_VQ_MSG_VENDREQ_PROCESS				= (SP7K_MSG_TAG | 0xE8),
	APP_VQ_MSG_VENDREQ_VALUES_H				= (SP7K_MSG_TAG | 0xEA),
	APP_VQ_MSG_UNLOCK_AEAWB					= (SP7K_MSG_TAG | 0xEB),
} sp7kHostMsgList_e;

typedef enum {
	SP7K_RDREG_AF_STATUS				= (SP7K_RDREG_BASE | 0xA0),
	SP7K_RDREG_AF_RESULT				= (SP7K_RDREG_BASE | 0xA1),
	SP7K_RDREG_EXPTIME_NUMERATOR		= (SP7K_RDREG_BASE | 0xB0),
	SP7K_RDREG_EXPTIME_DENOMINATOR_L	= (SP7K_RDREG_BASE | 0xB1),
	SP7K_RDREG_EXPTIME_DENOMINATOR_M	= (SP7K_RDREG_BASE | 0xB2),
	SP7K_RDREG_EXPTIME_DENOMINATOR_H	= (SP7K_RDREG_BASE | 0xB3),
	SP7K_RDREG_EXPTIME_COMPENSATION		= (SP7K_RDREG_BASE | 0xB4),
	SP7K_RDREG_FOCAL_LENGTH_L			= (SP7K_RDREG_BASE | 0xB5),
	SP7K_RDREG_FOCAL_LENGTH_H			= (SP7K_RDREG_BASE | 0xB6),
	SP7K_RDREG_ISO_L					= (SP7K_RDREG_BASE | 0xB7),
	SP7K_RDREG_ISO_H					= (SP7K_RDREG_BASE | 0xB8),
	SP7K_RDREG_FLASH_MODE				= (SP7K_RDREG_BASE | 0xB9),
	SP7K_RDREG_BACK_SENSORID_L			= (SP7K_RDREG_BASE | 0xC6),
	SP7K_RDREG_BACK_SENSORID_H			= (SP7K_RDREG_BASE | 0xC7),
	SP7K_RDREG_FRONT_SENSORID_L			= (SP7K_RDREG_BASE | 0xC8),
	SP7K_RDREG_FRONT_SENSORID_H			= (SP7K_RDREG_BASE | 0xC9),
	SP7K_RDREG_FW_VER_L					= (SP7K_RDREG_BASE | 0xCA),
	SP7K_RDREG_FW_VER_M					= (SP7K_RDREG_BASE | 0xCB),
	SP7K_RDREG_FW_VER_H					= (SP7K_RDREG_BASE | 0xCC),
	SP7K_RDREG_VENDOR_ID_L				= (SP7K_RDREG_BASE | 0xCE),
	SP7K_RDREG_VENDOR_ID_H				= (SP7K_RDREG_BASE | 0xCF),
	SP7K_RDREG_CALIBRATION_EXEC_STATUS1	= (SP7K_RDREG_BASE | 0xF0),
	SP7K_RDREG_CALIBRATION_EXEC_STATUS2	= (SP7K_RDREG_BASE | 0xF1),
	SP7K_RDREG_CALIBRATION_ERR_STATUS1	= (SP7K_RDREG_BASE | 0xF2),
	SP7K_RDREG_CALIBRATION_ERR_STATUS2	= (SP7K_RDREG_BASE | 0xF3),
	SP7K_RDREG_CALIBRATION_ERR_CODE		= (SP7K_RDREG_BASE | 0xF4),
	SP7K_RDREG_LOAD_TABLE_STATUS		= (SP7K_RDREG_BASE | 0xF5),
	SP7K_RDREG_INT_STS_REG_0			= (SP7K_RDREG_BASE | 0xF8),
	SP7K_RDREG_INT_MASK_REG_0			= (SP7K_RDREG_BASE | 0xFC),
} sp7kReadRegList_e;

/****************************************************************************
 *						 M A C R O S											*
 ****************************************************************************/

/****************************************************************************
 *						 E X T E R N V A R I A B L E S    D E C L A R A T I O N S		*
 ****************************************************************************/

/****************************************************************************
 *						F U N C T I O N    D E C L A R A T I O N S					*
 ****************************************************************************/
void ISPI2C_BandSelectionSet(UINT8 ucParam);
void ISPI2C_ColorEffectSet(UINT8 ucParam);
void ISPI2C_EVSet(UINT8 ucParam);
void ISPI2C_FlashModeSet(UINT8 ucParam);
void ISPI2C_FocusModeSet(UINT8 ucParam);
void ISPI2C_PreviewSizeSet(UINT8 ucParam);
void ISPI2C_CaptureSizeSet(UINT8 ucParam);
void ISPI2C_SceneModeSet(UINT8 ucParam);
void ISPI2C_WhiteBalanceSet(UINT8 ucParam);
void ISPI2C_ZoomRatioSet(UINT16 usValue);
void ISPI2C_SensorSelectSet(UINT8 ucParam);
void ISPI2C_AEModeSet(UINT8 ucParam);
void ISPI2C_CaptureModeSet(UINT8 ucParam);
void ISPI2C_ISOSet(UINT8 ucParam);
#if 0
void ISPI2C_BrightnessSet(UINT8 ucValue);
void ISPI2C_ContrastSet(UINT8 ucValue);
void ISPI2C_SaturationSet(UINT8 ucValue);
#endif
void ISPI2C_AuraColorIndexSet(UINT8 ucParam);
void ISPI2C_OpticalZoomSet(UINT8 ucParam);
void ISPI2C_IspFuncSet(UINT8 ucParam);
void ISPI2C_PreviewCapModeSet(UINT8 ucParam);
void ISPI2C_PreviewStreamSet(UINT8 ucParam);
void ISPI2C_AbortBurstSet(UINT8 ucParam);
void ISPI2C_FvFixFrameRateSet(UINT8 ucValue);
void ISPI2C_PvMaxExposureTimeSet(UINT8 ucValue);
void ISPI2C_GyroStatusSet(UINT8 ucParam);
void ISPI2C_HdrPositiveEvSet(UINT8 ucValue);
void ISPI2C_HdrNegativeEvSet(UINT8 ucValue);
#if 0	/*	not implement rolling shutter */
void ISPI2C_RollingShutterSet(void);
#endif
void ISPI2C_AFROIInfoSet(UINT16 usSize, UINT16 usX, UINT16 usY);
void ISPI2C_AFROITriggerSet(void);
void ISPI2C_AEROIInfoSet(UINT16 usSize, UINT16 usX, UINT16 usY);
void ISPI2C_AEROITriggerSet(UINT8 ucParam);
void ISPI2C_AFAbortSet(void);
void ISPI2C_VcmStepSet(UINT16 usValue);
void ISPI2C_CalibrationRawPathSet(UINT8 ucParam);
void ISPI2C_CalibrationRawFormatSet(UINT8 ucParam);
void ISPI2C_CalibrationRawLinearSet(UINT8 ucParam);
void ISPI2C_CalibrationRawObSet(UINT8 ucParam);
void ISPI2C_CalibrationRawBpSet(UINT8 ucParam);
void ISPI2C_CalibrationRawLscSet(UINT8 ucParam);
void ISPI2C_CalibrationRawCaSet(UINT8 ucParam);
void ISPI2C_CalibrationRawArdSet(UINT8 ucParam);
void ISPI2C_CalibrationRawDepeakSet(UINT8 ucParam);
void ISPI2C_CalibrationRawWbSet(UINT8 ucParam);
void ISPI2C_AeAwbThreadModeSet(UINT8 ucParam);
void ISPI2C_DqModeSet(UINT8 ucParam);
void ISPI2C_CdspOnOffModeSet(UINT8 ucParam);
void ISPI2C_CalibrationCmd1Set(UINT8 ucParam);
void ISPI2C_CalibrationCmd2Set(UINT8 ucParam);
void ISPI2C_CalibrationInitCmdSet(UINT8 ucParam);
void ISPI2C_CalibrationLscMaxGainSet(UINT8 ucVaule);
void ISPI2C_DqIdxFixSet(UINT8 ucParam);
void ISPI2C_CaliUtilizationOptionSet(UINT8 ucParam);
void ISPI2C_SpiDmaSizeReqSet_L(UINT8 ucValue);
void ISPI2C_SpiDmaSizeReqSet_H(UINT8 ucValue);
void ISPI2C_ROISwitchSet(UINT8 ucParam);
void ISPI2C_DmaTargetSelectSet(UINT8 ucParam);
void ISPI2C_DmaRWIntfSelectSet(UINT8 ucParam);
void ISPI2C_DmaTargetAddrSet_L(UINT8 ucValue);
void ISPI2C_DmaTargetAddrSet_H(UINT8 ucValue);
void ISPI2C_DmaRWDirectionSet(UINT8 ucParam);
void ISPI2C_ResourceIdSet(UINT8 ucParam);

void ISPI2C_VQVendreqIdSet(UINT8 ucParam);
void ISPI2C_VQVendreqParamSet(UINT8 ucParam);
void ISPI2C_VQVendreqValuesSet_L(UINT8 ucVaule);
void ISPI2C_VQVendreqProcessSet(void);
void ISPI2C_VQVendreqValuesSet_H(UINT8 ucVaule);
void ISPI2C_VQVendreqAEAWB_Unlock(UINT8 ucParam);


UINT8 ISPI2C_GetAFStatus(void);
UINT8 ISPI2C_GetAFResult(void);
UINT8 ISPI2C_GetExposureTimeNumerator(void);
UINT32 ISPI2C_GetExposureTimeDenominator(void);
UINT8 ISPI2C_GetExposureTimeCompensation(void);
UINT16 ISPI2C_GetFocalLength(void);
UINT16 ISPI2C_GetISOValue(void);
UINT8 ISPI2C_GetFlashMode(void);
UINT16 ISPI2C_GetBackSensorId(void);
UINT16 ISPI2C_GetFrontSensorId(void);
UINT32 ISPI2C_GetFWVersion(void);
UINT16 ISPI2C_GetVendorId(void);
UINT8 ISPI2C_GetCalibrationExecStatus1(void);
UINT8 ISPI2C_GetCalibrationExecStatus2(void);
UINT8 ISPI2C_GetCalibrationErrStatus1(void);
UINT8 ISPI2C_GetCalibrationErrStatus2(void);
UINT8 ISPI2C_GetCalibrationErrCode(void);
UINT8 ISPI2C_GetLoadTableStatus(void);

#endif  /* APP_I2C_LIB_ICATCH_H */

