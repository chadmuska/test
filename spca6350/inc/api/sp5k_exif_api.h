/**************************************************************************
 *
 *       Copyright (c) 2002-2016 by iCatch Technology, Inc.
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
 *  Author: Huiling Chou                                                  *
 *                                                                        *
 **************************************************************************/
 #include "common.h"

 /**************************************************************************
*                           C O N S T A N T S                            *
 **************************************************************************/
#ifndef _SP5K_EXIF_API_H_
#define _SP5K_EXIF_API_H_

#define EXIF_FORMAT_JFIF  1
#define EXIF_FORMAT_EXIF  2
#define EXIF_FORMAT_GIF   3

#define EXIF_YUV420       0
#define EXIF_YUV422       1
#define EXIF_YUV411       2
#define EXIF_YUV444       3
#define EXIF_YUV422V      4
#define EXIF_YUV411V      5

enum sp5kIfdDataType {
	SP5K_IFD_DATATYPE_BYTE = 1,
	SP5K_IFD_DATATYPE_ASCII,
	SP5K_IFD_DATATYPE_SHORT,
	SP5K_IFD_DATATYPE_LONG,
	SP5K_IFD_DATATYPE_RATIONAL,
	SP5K_IFD_DATATYPE_UNDEFINED = 7,
	SP5K_IFD_DATATYPE_SLONG = 9,
	SP5K_IFD_DATATYPE_SRATIONAL = 10
};

enum sp5kIfdTypeID{
     SP5K_IFD_TYPE_ZEROIFD = 1,
     SP5K_IFD_TYPE_EXIFIFD,
     SP5K_IFD_TYPE_GPSIFD,
     SP5K_IFD_TYPE_FIRSTIFD,
     SP5K_IFD_TYPE_ITOPIFD,
};

enum sp5kEXIFImgAttrID{
     SP5K_IMG_ATTR_WIDTH = 0,
     SP5K_IMG_ATTR_HEIGHT,
     SP5K_IMG_ATTR_DATA_FMT,
     SP5K_IMG_ATTR_ORIENT,
     SP5K_EXIF_APP2_OFFSET,
     SP5K_EXIF_MPF_OFFSET,
     SP5K_EXIF_MPF_SIZE
};

enum sp5kExifImgTypeID{
     SP5K_IMG_THUMBNAIL = 0,
     SP5K_IMG_PRIMARY
};

#define SP5K_IMG_THUMBNAIL_EXIST   (1 << 0)
#define SP5K_IMG_MAIN_EXIST        (1 << 1)
#define SP5K_IMG_QV_EXIST          (1 << 2)
#define SP5K_IMG_APP1_EXIST        (1 << 3)

#define SP5K_IFD0_TAG_IMGDESC   0x010E
#define SP5K_IFD0_TAG_MAKE      0x010F
#define SP5K_IFD0_TAG_MODEL     0x0110
#define SP5K_IFD0_TAG_ORIENT    0x0112
#define SP5K_IFD0_TAG_XRES      0x011A
#define SP5K_IFD0_TAG_YRES      0x011B
#define SP5K_IFD0_TAG_RESUNIT   0x0128
#define SP5K_IFD0_TAG_SOFTWARE  0x0131
#define SP5K_IFD0_TAG_DATETIME  0x0132
#define SP5K_IFD0_TAG_YCBCRPOS  0x0213
#define SP5K_IFD0_TAG_COPYRIGHT 0x8298
#define SP5K_IFD0_TAG_EXIFPTR   0x8769
#define SP5K_IFD0_TAG_GPSPTR	0x8825
#define SP5K_IFD0_TAG_IPM2      0xC4A5

#define SP5K_EXIF_TAG_EXPTIME        0x829A
#define SP5K_EXIF_TAG_FNUM           0x829D
#define SP5K_EXIF_TAG_EXPPROG        0x8822
#define SP5K_EXIF_TAG_ISOSPEEDRATE   0x8827
#define SP5K_EXIF_TAG_SENSITIVITYTYPE 0x8830
#define SP5K_EXIF_TAG_EXIFVER        0x9000
#define SP5K_EXIF_TAG_DATETIMEORIG   0x9003
#define SP5K_EXIF_TAG_DATETIMEDITI   0x9004
#define SP5K_EXIF_TAG_COMPCONFIGURE  0x9101
#define SP5K_EXIF_TAG_COMPRESSBPP    0x9102
#define SP5K_EXIF_TAG_SHUTTERSPDVAL  0x9201
#define SP5K_EXIF_TAG_APERTUREVAL    0x9202
#define SP5K_EXIF_TAG_BRIGHTVAL      0x9203
#define SP5K_EXIF_TAG_EXPBIAS        0x9204
#define SP5K_EXIF_TAG_MAXAPTURE      0x9205
#define SP5K_EXIF_TAG_METERMODE      0x9207
#define SP5K_EXIF_TAG_LIGHTSOURCE    0x9208
#define SP5K_EXIF_TAG_FLASH          0x9209
#define SP5K_EXIF_TAG_FOCALLEN       0x920A
#define SP5K_EXIF_TAG_MAKERNOTE      0x927C
#define SP5K_EXIF_TAG_USRCOMMENT     0x9286

#define SP5K_EXIF_TAG_FLRESHPIXVER   0xA000
#define SP5K_EXIF_TAG_COLORSPACE     0xA001
#define SP5K_EXIF_TAG_PEXELXDIM      0xA002
#define SP5K_EXIF_TAG_PEXELYDIM      0xA003
#define SP5K_EXIF_TAG_AUDIOFILE      0xA004
#define SP5K_EXIF_TAG_ITOPIFDPTR     0xA005
#define SP5K_EXIF_TAG_SENSINGMETHOD  0xA217
#define SP5K_EXIF_TAG_FILESOURCE     0xA300
#define SP5K_EXIF_TAG_SENCETYPE      0xA301

#define SP5K_IFD1_TAG_COMPRESS           0x0103
#define SP5K_IFD1_TAG_ORIENT             0x0112
#define SP5K_IFD1_TAG_XRES               0x011A
#define SP5K_IFD1_TAG_YRES               0x011B
#define SP5K_IFD1_TAG_RESUINT            0x0128
#define SP5K_IFD1_TAG_JPG_INTERCHGFMT    0x0201
#define SP5K_IFD1_TAG_JPG_INTERCHGFMTLEN 0x0202
#define SP5K_IFD1_TAG_YCBCRPOS           0x0213

#define SP5K_ITOP_TAG_ITOPINDEX   0x0001
#define SP5K_ITOP_TAG_ITOPVERSION 0x0002

#define SP5K_GPS_TAG_VERSIONID		0x0000
#define SP5K_GPS_TAG_LATITUDEREF	0x0001
#define SP5K_GPS_TAG_LATITUDE		0x0002
#define SP5K_GPS_TAG_LONGITUDEREF	0x0003
#define SP5K_GPS_TAG_LONGITUDE		0x0004
#define SP5K_GPS_TAG_ALTITUDEREF	0x0005
#define SP5K_GPS_TAG_ALTITUDE		0x0006
#define SP5K_GPS_TAG_TIMESTAMP		0x0007
#define SP5K_GPS_TAG_SATELLITES		0x0008
#define SP5K_GPS_TAG_STATUS			0x0009
#define SP5K_GPS_TAG_MEASUREMODE	0x000A
#define SP5K_GPS_TAG_DOP			0x000B
#define SP5K_GPS_TAG_SPEEDREF		0x000C
#define SP5K_GPS_TAG_SPEED			0x000D
#define SP5K_GPS_TAG_TRACKREF		0x000E
#define SP5K_GPS_TAG_TRACK			0x000F
#define SP5K_GPS_TAG_IMGDIRREF		0x0010
#define SP5K_GPS_TAG_IMGDIR			0x0011
#define SP5K_GPS_TAG_MAPDATUM		0x0012
#define SP5K_GPS_TAG_DSTLATITUDEREF	0x0013
#define SP5K_GPS_TAG_DSTLATITUDE	0x0014
#define SP5K_GPS_TAG_DSTLONGITUDEREF	0x0015
#define SP5K_GPS_TAG_DSTLONGITUDE	0x0016
#define SP5K_GPS_TAG_DSTBEARINGREF	0x0017
#define SP5K_GPS_TAG_DSTBEARING		0x0018
#define SP5K_GPS_TAG_DSTDISTANCEREF	0x0019
#define SP5K_GPS_TAG_DSTDISTANCE	0x001A
#define SP5K_GPS_TAG_PROCMETHOD		0x001B
#define SP5K_GPS_TAG_AREAINFO		0x001C
#define SP5K_GPS_TAG_DATESTAMP		0x001D
#define SP5K_GPS_TAG_DIFFCORRECT	0x001E
#define SP5K_GPS_TAG_POSITIONERR	0x001F


/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef enum {
	SP5K_EXIF_APPN_WRITE_TOTAL_SIZE,
	SP5K_EXIF_APPN_WRITE_EACH_SIZE,
	SP5K_EXIF_APPN_WRITE_CALLBACK,
	SP5K_EXIF_APPN_PARSE_CALLBACK,
	SP5K_EXIF_APPN_PARSE_DONE_CALLBACK,
	SP5K_EXIF_APPN_PARSE_BUF_SIZE,
} sp5kExifAppnParam_e;

typedef enum {
	SP5K_EXIF_APP1_WRITE_CALLBACK,
	SP5K_EXIF_APP1_PARSE_CALLBACK,
} sp5kExifApp1Param_e;

/**
 * @brief The structure passed information between Host and base firmware.
 * Besides index, the nailAddr and nailSize has different meaning in parse and
 * write callbacks.
 * In parse:
 *   nailAddr - The address of internal qv buffer for host to fill the real
 *              qv data. (Only host knows which part of appn is qv data.)
 *   nailSize - The host changes it according how much data stored into 
 *              nailAddr. If the buffer is not enough to store all the qv data,
 *              please fill it as 0.
 * In write:
 *   nailAddr - The address of internal qv buffer for host to fill data in appn
 *              chunk. (Only host knows the data format of appn chunk.)
 *   nailSize - The size of compressed qv data.
 */
 typedef struct sp5kExifAppnCbInfo_s {
	UINT32 index;    /* !< The Appn index, 2~15 */
	UINT32 nailAddr; /* !< The address of qv (screen nail) */
	UINT32 nailSize; /* !< The size of qv (screen nail) */
} sp5kExifAppnCbInfo_t;

 typedef struct sp5kExifApp1CbInfo_s {
	UINT32 nailAddr; /* !< The address of qv (screen nail) */
	UINT32 nailSize; /* !< The size of qv (screen nail) */
} sp5kExifApp1CbInfo_t;

typedef enum {
	SP5K_EXIF_CFG_ENDIAN_TYPE,
	SP5K_EXIF_CFG_MPF_SUPPORT,
	SP5K_EXIF_CFG_GPS_IFD_SUPPORT,
	SP5K_EXIF_CFG_MPF_HDR_WRITE_CALLBACK,
	SP5K_EXIF_CFG_MPF_USE_REPRESENT_IMAGE,
	SP5K_EXIF_CFG_CREATE_CALLBACK,
	SP5K_EXIF_CFG_READ_EXIF_LENGTH,
	SP5K_EXIF_CFG_FIRST_IFD_SUPPORT,
	SP5K_EXIF_CFG_QV_SUPPORT,
	SP5K_EXIF_CFG_TAG_ADR_ALIGN2_EN,
	SP5K_EXIF_CFG_PREPARE_CALLBACK = 0x1001,
} sp5kExifCfgSel_e;

typedef enum {
	SP5K_MAX_IDX_IFD_NUMBER,
	SP5K_MAX_ATTR_IFD_NUMBER,
	SP5K_MAX_IMG_NUMBER
} mpfCfgSel_e;

typedef enum sp5kMpfIfdType{
	SP5K_MPF_IFD_TYPE_INDEX = 1,
	SP5K_MPF_IFD_TYPE_ATTRIBUTE = 2
} sp5kMpfIfdType_e;

typedef enum sp5kMpfIdxIfdTag{
	SP5K_MPF_IDX_TAG_VERSION    		= 0xb000,
	SP5K_MPF_IDX_TAG_NUMOFIMG   		= 0xb001,
	SP5K_MPF_IDX_TAG_MPENTRY			= 0xb002,
	SP5K_MPF_IDX_TAG_IMGUNID    		= 0xb003,
	SP5K_MPF_IDX_TAG_NUMOFFRAME		  	= 0xb004,
} sp5kMpfIdxIfdTag_e;

typedef enum sp5kMpfDataType{
	SP5K_MPF_DATATYPE_LONG = 4,
	SP5K_MPF_DATATYPE_RATIONAL = 5,
	SP5K_MPF_DATATYPE_UNDEFINED = 7,
	SP5K_MPF_DATATYPE_SRATIONAL = 10
} sp5kMpfDataType_e;

typedef enum sp5kMpfAttrIfdTag{
	SP5K_MPF_ATTR_TAG_VERSION    			= 0xb000,
	SP5K_MPF_ATTR_TAG_IDXOFIMG   			= 0xb101,
	SP5K_MPF_ATTR_TAG_PANORIENTETION 		= 0xb201,
	SP5K_MPF_ATTR_TAG_PANOVERLAPH 			= 0xb202,
	SP5K_MPF_ATTR_TAG_PANOVERLAPV 			= 0xb203,
	SP5K_MPF_ATTR_TAG_BASEVIEWPOINTNUM 		= 0xb204,
	SP5K_MPF_ATTR_TAG_CONVERGENCEANGLE 		= 0xb205,
	SP5K_MPF_ATTR_TAG_BASELINELENGTH 		= 0xb206,
	SP5K_MPF_ATTR_TAG_DIVERGENCEANGLE 		= 0xb207,
	SP5K_MPF_ATTR_TAG_ASIXDISTANCEX 		= 0xb208,
	SP5K_MPF_ATTR_TAG_ASIXDISTANCEY 		= 0xb209,
	SP5K_MPF_ATTR_TAG_ASIXDISTANCEZ 		= 0xb20A,
	SP5K_MPF_ATTR_TAG_YAWANGLE 				= 0xb20b,
	SP5K_MPF_ATTR_TAG_PITCHANGLE 			= 0xb20c,
	SP5K_MPF_ATTR_TAG_ROLLANGLE 			= 0xb20d
} sp5kMpfAttrIfdTag_e;

typedef enum sp5kMpfMpEntryType{
	SP5K_MPF_MP_ENTRY_TYPE_UNDEFINED      =  0x000000,
    SP5K_MPF_MP_ENTRY_TYPE_LARGETHUMBCLS1 =  0x010001,
    SP5K_MPF_MP_ENTRY_TYPE_LARGETHUMBCLS2 =  0x010002,
    SP5K_MPF_MP_ENTRY_TYPE_MVPANORAMA     =  0x020001,
    SP5K_MPF_MP_ENTRY_TYPE_MVDISPARITY    =  0x020002,
    SP5K_MPF_MP_ENTRY_TYPE_MVMULTIANGLE   =  0x020003,     
    SP5K_MPF_MP_ENTRY_TYPE_BLPRIMARYIMG   =  0x030000
} sp5kMpfMpEntryType_e;
/* qv_app1 */
typedef enum {
	SP5K_EXIF_QV_DIS = 0,
	SP5K_EXIF_QV_SAVE_IN_APPN,
	SP5K_EXIF_QV_SAVE_IN_APP1,
} sp5kExifCfgQvSp_e;
/**
 * @brief data type of exif parse/write callback.
 * @param addr The address of current Appn chunk.
 * @param size The available size of current Appn chunk.
 */
typedef UINT32 (*fp_sp5kExifAppnCb_t)(UINT32 addr, UINT32 size,  sp5kExifAppnCbInfo_t *);
typedef UINT32 (*fp_sp5kExifCreateCb_t)(UINT32 rsv1, UINT32 rsv2, void * rsv3);

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/* SP5k Host API */
UINT32 sp5kExifInit(void);
UINT32 sp5kExifCfgSet(UINT32 selector, UINT32 value);

UINT32 sp5kExifEncodeTagAdd(UINT16 idfType,	UINT16 tagId, UINT16 idfDataType, UINT32 ifdCnt, UINT8 *pvalue);
UINT32 sp5kExifEncodeTagDelete(UINT16 ifdType, UINT16 tagId);
UINT32 sp5kExifEncodeTagValueSet(UINT16 ifdType, UINT16 tagId, UINT16 ifDataType, UINT32 ifdCnt, UINT8 *pvalue);

UINT32 sp5kExifDecodeTagSizeGet(UINT16 ifdType,	UINT16 ifdId, UINT16 *pifdDataType,	UINT32 *pifdCnt);
UINT32 sp5kExifDecodeTagValueGet(UINT16 ifdType, UINT16 ifdId, UINT8* pbuf);

UINT32 sp5kExifImageInfoGet(UINT32 imgType, UINT32 attrId, UINT32 *pvalue);
UINT32 sp5kExifImageQuery(UINT8 *pname, UINT8 *pflag);
UINT32 sp5kExifApp1Query(UINT8 *pname, UINT8 *pflag);
UINT32 sp5kExifAppnParamSet(UINT32 selector, UINT32 val);

UINT32 sp5kMpfCfgSet(UINT32 selector, UINT32 value);
UINT32 sp5kMpfIfdValueSet(UINT32 ifdType, UINT16 ifdTag, UINT16 dataType, UINT32 ifdCount, void *ifdValue);
#endif /* _SP5K_EXIF_API_H_ */

