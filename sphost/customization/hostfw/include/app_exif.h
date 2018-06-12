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
#ifndef _APP_EXIF_H_
#define _APP_EXIF_H_

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
#include "sp5k_exif_api.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

#define APP_EXIF_DATE_MAX_LEN				20

/* new add tag */
#define APP_EXIF_TAG_FW_VER					0x0131
#define APP_EXIF_TAG_COPYRIGHT				0x8298

#define APP_EXIF_TAG_EXP_MODE				0xA402
#define APP_EXIF_TAG_WB_MODE				0xA403
#define APP_EXIF_TAG_FOCALLENGTH_32MMFILE	0xA405
#define APP_EXIF_TAG_SCENE_MODE				0xA406
#define APP_EXIF_TAG_SHARPNESS				0xA40A

#define APP_EXIF_TAG_APERTURE				0x9202
#define APP_EXIF_TAG_SHUTTER_SPEED			0x9201
#define APP_EXIF_TAG_DIGITAL_ROOM_RATION	0xA404

/* availalbe values */
enum {
	SP5K_EXIF_TAGDATA_EXPPROG_NOTDEFINED,
	SP5K_EXIF_TAGDATA_EXPPROG_MANUAL,
	SP5K_EXIF_TAGDATA_EXPPROG_NORMAL,
	SP5K_EXIF_TAGDATA_EXPPROG_APERTURE,
	SP5K_EXIF_TAGDATA_EXPPROG_SHUTTER,
	SP5K_EXIF_TAGDATA_EXPPROG_CREATIVE,
	SP5K_EXIF_TAGDATA_EXPPROG_ACTION,
	SP5K_EXIF_TAGDATA_EXPPROG_PORTRAIT,
	SP5K_EXIF_TAGDATA_EXPPROG_LANDSCAPE
};

enum {
	SP5K_EXIF_TAGDATA_METERMODE_UNKNOWN,
	SP5K_EXIF_TAGDATA_METERMODE_AVERAGE,
	SP5K_EXIF_TAGDATA_METERMODE_CENTERWEIGHTED,
	SP5K_EXIF_TAGDATA_METERMODE_SPOT,
	SP5K_EXIF_TAGDATA_METERMODE_MULTISPOT,
	SP5K_EXIF_TAGDATA_METERMODE_PATTERN,
	SP5K_EXIF_TAGDATA_METERMODE_PARTIAL,
	SP5K_EXIF_TAGDATA_METERMODE_OTHER=255
};

enum {
	SP5K_EXIF_TAGDATA_LIGHTSOURCE_UNKNOWN,
	SP5K_EXIF_TAGDATA_LIGHTSOURCE_MANUAL,
	SP5K_EXIF_TAGDATA_LIGHTSOURCE_DAYLIGHT,
	SP5K_EXIF_TAGDATA_LIGHTSOURCE_FLUORESCENT,
	SP5K_EXIF_TAGDATA_LIGHTSOURCE_TUNGSTEN,
	SP5K_EXIF_TAGDATA_LIGHTSOURCE_FLASH,
	SP5K_EXIF_TAGDATA_LIGHTSOURCE_FINE_WEATHER=9,
	SP5K_EXIF_TAGDATA_LIGHTSOURCE_CLOUDY_WEATHER,
	SP5K_EXIF_TAGDATA_LIGHTSOURCE_SHADE,
	SP5K_EXIF_TAGDATA_LIGHTSOURCE_DAYLIGHT_FLUORESCENT,
	SP5K_EXIF_TAGDATA_LIGHTSOURCE_DAY_WHITE_FLUORESCENT,
	SP5K_EXIF_TAGDATA_LIGHTSOURCE_COOL_WHITE_FLUORESCENT,
	SP5K_EXIF_TAGDATA_LIGHTSOURCE_WHITE_FLUORESCENT,
	SP5K_EXIF_TAGDATA_LIGHTSOURCE_STANDARD_LIGHT_A=17,
	SP5K_EXIF_TAGDATA_LIGHTSOURCE_STANDARD_LIGHT_B,
	SP5K_EXIF_TAGDATA_LIGHTSOURCE_STANDARD_LIGHT_C,
	SP5K_EXIF_TAGDATA_LIGHTSOURCE_D55,
	SP5K_EXIF_TAGDATA_LIGHTSOURCE_D65,
	SP5K_EXIF_TAGDATA_LIGHTSOURCE_D75,
	SP5K_EXIF_TAGDATA_LIGHTSOURCE_D50,
	SP5K_EXIF_TAGDATA_LIGHTSOURCE_ISO_STUDIO_TUNGSTEN,
	SP5K_EXIF_TAGDATA_LIGHTSOURCE_OTHER=255,
};

enum {
	SP5K_EXIF_TAGDATA_SCENECAPTURETYPE_STANDARD,
	SP5K_EXIF_TAGDATA_SCENECAPTURETYPE_LANDSCANPE,
	SP5K_EXIF_TAGDATA_SCENECAPTURETYPE_PORTRAIT,
	SP5K_EXIF_TAGDATA_SCENECAPTURETYPE_NIGHTSCENE,
	SP5K_EXIF_TAGDATA_SCENECAPTURETYPE_OTHER=255
};

enum {
	SP5K_EXIF_TAGDATA_SHARPNESS_NORMAL,
	SP5K_EXIF_TAGDATA_SHARPNESS_SOFT,
	SP5K_EXIF_TAGDATA_SHARPNESS_HARD
};

/* flashlight exif tag data */
#define SP5K_EXIF_TAGDATA_FLASH_FIRED			0x01
#define SP5K_EXIF_TAGDATA_FLASH_RETURE_DETECTED	0x02
#define SP5K_EXIF_TAGDATA_FLASH_DETECTOR		0x04
#define SP5K_EXIF_TAGDATA_FLASH_MODE_FORCE		0x08
#define SP5K_EXIF_TAGDATA_FLASH_MODE_SUPRESS	0x10
#define SP5K_EXIF_TAGDATA_FLASH_MODE_AUTO		0x18
#define SP5K_EXIF_TAGDATA_FLASH_FUNCTION_EXIST	0x20
#define SP5K_EXIF_TAGDATA_FLASH_REDEYE			0x40

#define EXIF_VALUE_FLASH_AUTO_FIRED  		0x10|0x08|0x01
#define EXIF_VALUE_FLASH_AUTO_NO_FIRED		0x10|0x08
#define EXIF_VALUE_FLASH_RED_EYE			0x40|0x10|0x08|0x01
#define EXIF_VALUE_FLASH_FORCE_ON			0x08|0x04|0x01
#define EXIF_VALUE_FLASH_SLOW_SYNC			0x10|0x08|0x04|0x01
#define EXIF_VALUE_FLASH_FORCE_OFF			0x10
#define EXIF_VALUE_FLASH_OFF				0

/* colorspace exif tag data */
#define SP5K_EXIF_TAGDATA_COLORSPACE_SRGB			0x1
#define SP5K_EXIF_TAGDATA_COLORSPACE_UNCALIBRATED	0xffff

/* ev bias value */
#define SP5K_EXIF_TAGDATA_EXP_BIAS_N233 		-233
#define SP5K_EXIF_TAGDATA_EXP_BIAS_N200 		-200
#define SP5K_EXIF_TAGDATA_EXP_BIAS_N166 		-166
#define SP5K_EXIF_TAGDATA_EXP_BIAS_N133 		-133
#define SP5K_EXIF_TAGDATA_EXP_BIAS_N100 		-100
#define SP5K_EXIF_TAGDATA_EXP_BIAS_N066			-66
#define SP5K_EXIF_TAGDATA_EXP_BIAS_N033			-33
#define SP5K_EXIF_TAGDATA_EXP_BIAS_ZERO			0
#define SP5K_EXIF_TAGDATA_EXP_BIAS_P033			33
#define SP5K_EXIF_TAGDATA_EXP_BIAS_P066			66
#define SP5K_EXIF_TAGDATA_EXP_BIAS_P100			100
#define SP5K_EXIF_TAGDATA_EXP_BIAS_P133			133
#define SP5K_EXIF_TAGDATA_EXP_BIAS_P166			166
#define SP5K_EXIF_TAGDATA_EXP_BIAS_P200			200
#define SP5K_EXIF_TAGDATA_EXP_BIAS_P233			233

/* wb value */
#define SP5K_EXIF_TAGDATA_WB_AUTO 		0
#define SP5K_EXIF_TAGDATA_WB_MANUAL 	1

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

typedef enum {
	EXIF_MN_IQ,
	EXIF_MN_PFH,
	EXIF_MN_AF,
	EXIF_MN_AE,
	EXIF_MN_TOTAL,
} exifMNID_t;

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/
#define appExifSetMakerNote(uwSize, szStr) 	    sp5kExifEncodeTagValueSet(SP5K_IFD_TYPE_EXIFIFD, SP5K_EXIF_TAG_MAKERNOTE, SP5K_IFD_DATATYPE_UNDEFINED, uwSize, szStr);
#define appExifSetImgDescrip(szStr) 			sp5kExifEncodeTagValueSet(SP5K_IFD_TYPE_ZEROIFD, SP5K_IFD0_TAG_IMGDESC, SP5K_IFD_DATATYPE_ASCII, strlen((const char *)szStr), (UINT8 *)szStr);
#define appExifSetMake(szStr) 					sp5kExifEncodeTagValueSet(SP5K_IFD_TYPE_ZEROIFD, SP5K_IFD0_TAG_MAKE, SP5K_IFD_DATATYPE_ASCII, strlen((const char *)szStr), (UINT8 *)szStr);
#define appExifSetModel(szStr) 					sp5kExifEncodeTagValueSet(SP5K_IFD_TYPE_ZEROIFD, SP5K_IFD0_TAG_MODEL, SP5K_IFD_DATATYPE_ASCII, strlen((const char *)szStr), (UINT8 *)szStr);

/* Unsigned Short Tag*/
#define appExifSetExpProgram(uwData)			sp5kExifEncodeTagValueSet(SP5K_IFD_TYPE_EXIFIFD, SP5K_EXIF_TAG_EXPPROG, SP5K_IFD_DATATYPE_SHORT, 1, uwData);
#define appExifSetIsoSpeed(uwData) 	    		sp5kExifEncodeTagValueSet(SP5K_IFD_TYPE_EXIFIFD, SP5K_EXIF_TAG_ISOSPEEDRATE, SP5K_IFD_DATATYPE_SHORT, 1, uwData);
#define appExifSetMeterMode(uwData)    			sp5kExifEncodeTagValueSet(SP5K_IFD_TYPE_EXIFIFD, SP5K_EXIF_TAG_METERMODE, SP5K_IFD_DATATYPE_SHORT, 1, uwData);
#define appExifSetLightSource(uwData) 			sp5kExifEncodeTagValueSet(SP5K_IFD_TYPE_EXIFIFD, SP5K_EXIF_TAG_LIGHTSOURCE, SP5K_IFD_DATATYPE_SHORT, 1, uwData);
#define appExifSetFlash(uwData)       			sp5kExifEncodeTagValueSet(SP5K_IFD_TYPE_EXIFIFD, SP5K_EXIF_TAG_FLASH, SP5K_IFD_DATATYPE_SHORT, 1, uwData);
/* new add*/
#define appExifSetExposureMode(uwData)  		sp5kExifEncodeTagValueSet(SP5K_IFD_TYPE_EXIFIFD, APP_EXIF_TAG_EXP_MODE, SP5K_IFD_DATATYPE_SHORT, 1, uwData);
#define appExifSetWBMode(uwData)        		sp5kExifEncodeTagValueSet(SP5K_IFD_TYPE_EXIFIFD, APP_EXIF_TAG_WB_MODE, SP5K_IFD_DATATYPE_SHORT, 1, uwData);
#define appExifSetFocalLength35mmFilm(uwData)	sp5kExifEncodeTagValueSet(SP5K_IFD_TYPE_EXIFIFD, APP_EXIF_TAG_FOCALLENGTH_32MMFILE, SP5K_IFD_DATATYPE_SHORT, 1, uwData);
#define appExifSetSceneCaptureType(uwData)		sp5kExifEncodeTagValueSet(SP5K_IFD_TYPE_EXIFIFD, APP_EXIF_TAG_SCENE_MODE, SP5K_IFD_DATATYPE_SHORT, 1, uwData);
#define appExifSetSharpness(uwData)				sp5kExifEncodeTagValueSet(SP5K_IFD_TYPE_EXIFIFD, APP_EXIF_TAG_SHARPNESS, SP5K_IFD_DATATYPE_SHORT, 1, uwData);

/* Unsigned Rational Tag*/
#define appExifSetExpTime(Ratio_8byte) 			sp5kExifEncodeTagValueSet(SP5K_IFD_TYPE_EXIFIFD, SP5K_EXIF_TAG_EXPTIME, SP5K_IFD_DATATYPE_RATIONAL, 1, Ratio_8byte);
#define appExifSetFNum(Ratio_8byte) 			sp5kExifEncodeTagValueSet(SP5K_IFD_TYPE_EXIFIFD, SP5K_EXIF_TAG_FNUM, SP5K_IFD_DATATYPE_RATIONAL, 1, Ratio_8byte);
#define appExifSetAperture(Ratio_8byte) 		sp5kExifEncodeTagValueSet(SP5K_IFD_TYPE_EXIFIFD, APP_EXIF_TAG_APERTURE, SP5K_IFD_DATATYPE_RATIONAL, 1, Ratio_8byte);
#define appExifSetMaxAperture(Ratio_8byte) 		sp5kExifEncodeTagValueSet(SP5K_IFD_TYPE_EXIFIFD, SP5K_EXIF_TAG_MAXAPTURE, SP5K_IFD_DATATYPE_RATIONAL, 1, Ratio_8byte);
#define appExifSetFocalLength(Ratio_8byte)  	sp5kExifEncodeTagValueSet(SP5K_IFD_TYPE_EXIFIFD, SP5K_EXIF_TAG_FOCALLEN, SP5K_IFD_DATATYPE_RATIONAL, 1, Ratio_8byte);
#define appExifSetDigitalZoomRatio(Ratio_8byte) sp5kExifEncodeTagValueSet(SP5K_IFD_TYPE_EXIFIFD, APP_EXIF_TAG_DIGITAL_ROOM_RATION, SP5K_IFD_DATATYPE_RATIONAL, 1, Ratio_8byte);
#define appExifSetXResolution(Ratio_8byte) 		sp5kExifEncodeTagValueSet(SP5K_IFD_TYPE_FIRSTIFD, SP5K_IFD1_TAG_XRES, SP5K_IFD_DATATYPE_RATIONAL, 1, Ratio_8byte);
#define appExifSetYResolution(Ratio_8byte) 		sp5kExifEncodeTagValueSet(SP5K_IFD_TYPE_FIRSTIFD, SP5K_IFD1_TAG_YRES, SP5K_IFD_DATATYPE_RATIONAL, 1, Ratio_8byte);

/* Signed Rational Tag*/
#define appExifSetShutSpeed(Ratio_8byte) 		sp5kExifEncodeTagValueSet(SP5K_IFD_TYPE_EXIFIFD, APP_EXIF_TAG_SHUTTER_SPEED, SP5K_IFD_DATATYPE_SRATIONAL, 1, Ratio_8byte);
#define appExifSetExpBias(Ratio_8byte)  		sp5kExifEncodeTagValueSet(SP5K_IFD_TYPE_EXIFIFD, SP5K_EXIF_TAG_EXPBIAS, SP5K_IFD_DATATYPE_SRATIONAL, 1, Ratio_8byte);

/* Undefined Tag*/
#define appExifSetExifVersion(szStr)  			sp5kExifEncodeTagValueSet(SP5K_IFD_TYPE_EXIFIFD, SP5K_EXIF_TAG_EXIFVER, SP5K_IFD_DATATYPE_ASCII, strlen((const char *)szStr), (UINT8 *)szStr);

#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
#define appExifSetGpsLatitudeRef(Ratio_8byte)	sp5kExifEncodeTagValueSet(SP5K_IFD_TYPE_GPSIFD, SP5K_GPS_TAG_LATITUDEREF, SP5K_IFD_DATATYPE_ASCII, 2, Ratio_8byte);
#define appExifSetGpsLatitude(Ratio_8byte)		sp5kExifEncodeTagValueSet(SP5K_IFD_TYPE_GPSIFD, SP5K_GPS_TAG_LATITUDE, SP5K_IFD_DATATYPE_RATIONAL, 3, Ratio_8byte);
#define appExifSetGpsLongitudeRef(Ratio_8byte)	sp5kExifEncodeTagValueSet(SP5K_IFD_TYPE_GPSIFD, SP5K_GPS_TAG_LONGITUDEREF, SP5K_IFD_DATATYPE_ASCII, 2, Ratio_8byte);
#define appExifSetGpsLongitude(Ratio_8byte)		sp5kExifEncodeTagValueSet(SP5K_IFD_TYPE_GPSIFD, SP5K_GPS_TAG_LONGITUDE, SP5K_IFD_DATATYPE_RATIONAL, 3, Ratio_8byte);
#define appExifSetGpsAltitudeRef(Ratio_8byte)	sp5kExifEncodeTagValueSet(SP5K_IFD_TYPE_GPSIFD, SP5K_GPS_TAG_ALTITUDEREF, SP5K_IFD_DATATYPE_BYTE, 1, Ratio_8byte);
#define appExifSetGpsAltitude(Ratio_8byte)		sp5kExifEncodeTagValueSet(SP5K_IFD_TYPE_GPSIFD, SP5K_GPS_TAG_ALTITUDE, SP5K_IFD_DATATYPE_RATIONAL, 1, Ratio_8byte);
#define appExifSetGpsTimestamp(Ratio_8byte)		sp5kExifEncodeTagValueSet(SP5K_IFD_TYPE_GPSIFD, SP5K_GPS_TAG_TIMESTAMP, SP5K_IFD_DATATYPE_RATIONAL, 3, Ratio_8byte);
#define appExifSetGpsStatus(Ratio_8byte)		sp5kExifEncodeTagValueSet(SP5K_IFD_TYPE_GPSIFD, SP5K_GPS_TAG_STATUS, SP5K_IFD_DATATYPE_ASCII, 2, Ratio_8byte);
#define appExifSetGpsSpeed(Ratio_8byte)			sp5kExifEncodeTagValueSet(SP5K_IFD_TYPE_GPSIFD, SP5K_GPS_TAG_SPEED, SP5K_IFD_DATATYPE_RATIONAL, 1, Ratio_8byte);
#define appExifSetGpsDatestamp(Ratio_8byte)		sp5kExifEncodeTagValueSet(SP5K_IFD_TYPE_GPSIFD, SP5K_GPS_TAG_DATESTAMP, SP5K_IFD_DATATYPE_ASCII, 11, Ratio_8byte);
#define appExifSetGpsDirectionRef(Ratio_8byte)	sp5kExifEncodeTagValueSet(SP5K_IFD_TYPE_GPSIFD, SP5K_GPS_TAG_IMGDIRREF, SP5K_IFD_DATATYPE_ASCII, 2, Ratio_8byte);
#define appExifSetGpsDirection(Ratio_8byte)		sp5kExifEncodeTagValueSet(SP5K_IFD_TYPE_GPSIFD, SP5K_GPS_TAG_IMGDIR, SP5K_IFD_DATATYPE_RATIONAL, 1, Ratio_8byte);
#define appExifSetGpsTrack(Ratio_8byte)		sp5kExifEncodeTagValueSet(SP5K_IFD_TYPE_GPSIFD, SP5K_GPS_TAG_TRACK, SP5K_IFD_DATATYPE_RATIONAL, 1, Ratio_8byte);
#endif

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
void appExifInit(void);
UINT32 appExifEncodeCb(UINT32 imgId, UINT32 reserved1, void *reserved2);

void appExif_SetExpProgram(UINT32 curMode, UINT32 curScene);
void appExif_SetMeterMode(UINT32 Metering);
void appExif_SetSharpness(UINT32 sharpness);
void appExif_SetSceneCaptureType(UINT32 curMode, UINT32 curScene);
void appExif_GetMakerNoteBuf(UINT8** buf, UINT32* size);
void appExif_MN_Init(void);
void appExif_MN_BufClear(UINT8 bufID);
UINT32 appExif_MN_WriteBytes(UINT8 bufID, UINT8 *pdata, UINT32 size);
UINT32  appExif_MN_Printf(UINT8 bufID, const char *szFmt, ...);
UINT32 appExif_MN_Write8(UINT8 bufID, UINT8 data);
UINT32 appExif_MN_Write16(UINT8 bufID, UINT16 data);
UINT32 appExif_MN_Write32(UINT8 bufID, UINT32 data);
UINT32 appExif_MN_WriteUID(UINT8 bufID, UINT32 uid);
void appMpf_setIfdValue(UINT32 snapCnt, UINT32 rsvd2, void *rsvd3);
void appExif_SetAviChunk(void);
#endif  /* _APP_EXIF_H_ */

