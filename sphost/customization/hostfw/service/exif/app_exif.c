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
 /*--------------------------------------------------------------------------*
 *	File name	: app_exif.c
 *	Function	: exif function
 *	First editor: Weikai Hsu
 *	First date	: 
 *	History		: 	
 *	            : 2018.06.01  Weikai Hsu "Added File Header"
 *	
 *	
 *	
 *--------------------------------------------------------------------------*/
#define HOST_DBG 0
#include <stdarg.h>
#include "middleware/pwrlog.h"
#include "api/sp5k_ae_api.h"
#include "api/sp5k_capture_api.h"
#include "api/sp5k_media_api.h"

#include "app_com_def.h"
#include "app_com_api.h"
#include "app_exif.h"
#include "app_aaa.h"
#include "app_view_param_def.h"
#include "app_view_param.h"
#include "app_still.h"
#include "app_lens_api.h"
#include "app_zoom_api.h"
#include "app_aaa.h"
#include "app_awbalg_api.h"
#include "app_aealg_api.h"

#include "../../app_ver.h"
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
#include "app_infohandler_func.h"
#endif

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
#define APP_EXIF_MAKE	    "Panasonic"
#define APP_EXIF_MODEL	    " "
#define APP_EXIF_COPYRIGHT  "Copyright 2008"
#define APP_EXIF_MAKERNOTE  " "
#define APP_EXIF_VERSION    "0220"

#define MAKERNOTE_MAKER	    "Panasonic"
#define MODEL_NAME	        " "
#define MAKE_MODEL			" " MODEL_NAME
#else
#define APP_EXIF_MAKE	    "iCatch"
#define APP_EXIF_MODEL	    "SP5K Series Digital Camera"
#define APP_EXIF_COPYRIGHT  "Copyright 2008"
#define APP_EXIF_MAKERNOTE  "Sunplus Maker Note"
#define APP_EXIF_VERSION    "0220"

#define MAKERNOTE_MAKER	    "ICATCHTEK"
#define MODEL_NAME	        "CVR"
#define MAKE_MODEL			"Icatch" MODEL_NAME
#endif
unsigned char RiffStrdStart[12] = { 0x41, 0x56, 0x49, 0x46, 0x01, 0x1E, 0x02, 0x02, 0x07, 0x00};	/*AVIF....*/

#define RIFF_PROFILE_IMAGESIZE_INDEX	4
#define RIFF_PROFILE_FRAMERATE_INDEX	5
#define RIFF_PROFILE_SOUNDINFO_INDEX    6
#define RIFF_MAKERNOTE_IFD_NUM 		    6

#define RIFF_PROFILE_IMAGESIZE_320		0x01
#define RIFF_PROFILE_IMAGESIZE_640		0x02
#define RIFF_PROFILE_IMAGESIZE_1280     0x03
#define RIFF_PROFILE_IMAGESIZE_1920     0x04

#define RIFF_PROFILE_SOUNDINFO     	    0x01

#define RIFF_PROFILE_FRAMERATE_30FPS    0x1E
#define RIFF_PROFILE_FRAMERATE_60FPS    0x3C

#define STRD_TAG_TYPE_BYTE              1
#define STRD_TAG_TYPE_ASCII             2
#define STRD_TAG_TYPE_SHORT             3
#define STRD_TAG_TYPE_LONG              4

#define F_NO_X100		280
#define	MAX_APER_X100	300 /* 3.0=F#2.8 */
#define FOCAL_LEN_X100	460
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct {
	UINT32 bufSize;
	UINT8 *bufBase;
	UINT8 *bufWPtr;
} exifMNBufCB_t;

typedef struct _Exif_tag
{
	unsigned short TagCode;
	unsigned short Type;
	UINT32	Count;
	UINT32	Value;
}SSTRDTAG, *PSSTRDTAG;

enum
{
	POTHIFD_MAKE,
	POTHIFD_MODEL,
	POTHIFD_VIDEODATESTAMP,
	POTHIFD_MAX,
};

typedef struct _Strd_Chuck
{
	unsigned char StrdStart[12];
	SSTRDTAG P0thIfd[POTHIFD_MAX];
	unsigned char Makenote[sizeof(MAKERNOTE_MAKER)];
	unsigned char MakeModel[sizeof(MAKE_MODEL)];
}SSTRDCHUNK, *PSTRDCHUNK;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
#if SPCA5110 && !SPCA5210
#define EXIF_MN_IQ_BUF_SIZE		128
#else
#define EXIF_MN_IQ_BUF_SIZE		128
#endif
#define EXIF_MN_PFH_BUF_SIZE	(128)
#define EXIF_MN_AF_BUF_SIZE		(128)
#define EXIF_MN_AE_BUF_SIZE		(512)
#define EXIF_3A_DEBUG_INFO_SIZE (32*1024)
#define EXIF_MN_BUF_SIZE	(EXIF_MN_IQ_BUF_SIZE+EXIF_MN_PFH_BUF_SIZE+EXIF_MN_AF_BUF_SIZE+EXIF_MN_AE_BUF_SIZE+EXIF_3A_DEBUG_INFO_SIZE)
static UINT8 mnBuf[EXIF_MN_BUF_SIZE]={0}; // maker note can record the 3a/iq data for debug

UINT32 aperture_x100[7][2]={
		{280, 560},
		{310, 630},
		{340, 690},
		{380, 760},
		{420, 850},
		{470, 950},
		{520, 1040}
};

UINT32 shutterSpeed[257][2]={
{4096	,	1	},
{3840	,	1	},
{3600	,	1	},
{2700	,	1	},
{2048	,	1	},
{1920	,	1	},
{1800	,	1	},
{1200	,	1	},
{1024	,	1	},
{960	,	1	},
{900	,	1	},
{600	,	1	},
{512	,	1	},
{480	,	1	},
{360	,	1	},
{300	,	1	},
{256	,	1	},
{240	,	1	},
{210	,	1	},
{180	,	1	},
{128	,	1	},
{120	,	1	},
{90	,	1	},
{80	,	1	},
{64	,	1	},
{60	,	1	},
{48	,	1	},
{45	,	1	},
{32	,	1	},
{30	,	1	},
{24	,	1	},
{20	,	1	},
{16	,	1	},
{15	,	1	},
{12	,	1	},
{10	,	1	},
{8	,	1	},
{7	,	1	},
{6	,	1	},
{5	,	1	},
{4	,	1	},
{7	,	2	},
{3	,	1	},
{5	,	2	},
{2	,	1	},
{7	,	4	},
{3	,	2	},
{5	,	4	},/*table_idx=47 , exp_idx=0*/
{1	,	1	},/*table_idx=48 , exp_idx=1*/
{1	,	1	},
{1	,	1	},
{1	,	1	},
{1	,	1	},
{1	,	1	},
{1	,	1	},
{1	,	1	},
{1	,	1	},
{1	,	1	},
{1	,	2	},
{1	,	2	},
{1	,	2	},
{1	,	2	},
{1	,	2	},
{1	,	2	},
{1	,	2	},
{1	,	2	},
{1	,	2	},
{1	,	2	},
{1	,	2	},
{1	,	2	},
{1	,	3	},
{1	,	3	},
{1	,	3	},
{1	,	3	},
{1	,	3	},
{1	,	3	},
{1	,	3	},
{1	,	4	},
{1	,	4	},
{1	,	4	},
{1	,	4	},
{1	,	4	},
{1	,	4	},
{1	,	5	},
{1	,	5	},
{1	,	5	},
{1	,	5	},
{1	,	5	},
{1	,	6	},
{1	,	6	},
{1	,	6	},
{1	,	6	},
{1	,	7	},
{1	,	7	},
{1	,	7	},
{1	,	8	},
{1	,	8	},
{1	,	8	},
{1	,	9	},
{1	,	9	},
{1	,	10	},
{1	,	10	},
{1	,	10	},
{1	,	11	},
{1	,	11	},
{1	,	12	},
{1	,	12	},
{1	,	13	},
{1	,	13	},
{1	,	14	},
{1	,	15	},
{1	,	15	},
{1	,	16	},
{1	,	17	},
{1	,	17	},
{1	,	18	},
{1	,	19	},
{1	,	20	},
{1	,	21	},
{1	,	22	},
{1	,	23	},
{1	,	24	},
{1	,	24	},
{1	,	25	},
{1	,	27	},
{1	,	28	},
{1	,	29	},
{1	,	30	},/*table_idx=127 , exp_idx=80*/
{1	,	32	},
{1	,	33	},
{1	,	35	},
{1	,	36	},
{1	,	38	},
{1	,	40	},
{1	,	41	},
{1	,	43	},
{1	,	45	},
{1	,	47	},
{1	,	50	},
{1	,	52	},
{1	,	54	},
{1	,	56	},
{1	,	59	},
{1	,	60	},
{1	,	64	},
{1	,	67	},
{1	,	70	},
{1	,	73	},
{1	,	76	},
{1	,	79	},
{1	,	83	},
{1	,	87	},
{1	,	91	},
{1	,	95	},
{1	,	100	},
{1	,	103	},
{1	,	108	},
{1	,	112	},
{1	,	117	},
{1	,	120	},
{1	,	128	},
{1	,	134	},
{1	,	140	},
{1	,	146	},
{1	,	152	},
{1	,	159	},
{1	,	166	},
{1	,	173	},
{1	,	181	},
{1	,	189	},
{1	,	200	},
{1	,	206	},
{1	,	215	},
{1	,	225	},
{1	,	235	},
{1	,	240	},
{1	,	256	},
{1	,	267	},
{1	,	279	},
{1	,	292	},
{1	,	304	},
{1	,	318	},
{1	,	332	},
{1	,	347	},
{1	,	362	},
{1	,	378	},
{1	,	395	},
{1	,	400	},
{1	,	431	},
{1	,	450	},
{1	,	470	},
{1	,	480	},
{1	,	512	},
{1	,	535	},
{1	,	558	},
{1	,	583	},
{1	,	609	},
{1	,	636	},
{1	,	664	},
{1	,	693	},
{1	,	724	},
{1	,	756	},
{1	,	790	},
{1	,	800	},
{1	,	861	},
{1	,	899	},
{1	,	939	},
{1	,	960	},
{1	,	1024	},
{1	,	1069	},
{1	,	1117	},
{1	,	1166	},
{1	,	1218	},
{1	,	1272	},
{1	,	1328	},
{1	,	1387	},
{1	,	1448	},
{1	,	1512	},
{1	,	1579	},
{1	,	1600	},
{1	,	1722	},
{1	,	1798	},
{1	,	1878	},
{1	,	1920	},
{1	,	2048	},
{1	,	2139	},
{1	,	2233	},
{1	,	2332	},
{1	,	2435	},
{1	,	2543	},
{1	,	2656	},
{1	,	2774	},
{1	,	2896	},
{1	,	3025	},
{1	,	3158	},
{1	,	3200	},
{1	,	3444	},
{1	,	3597	},
{1	,	3756	},
{1	,	3840	},
{1	,	4096	},
{1	,	4277	},
{1	,	4467	},
{1	,	4664	},
{1	,	4871	},
{1	,	5087	},
{1	,	5312	},
{1	,	5547	},
{1	,	5793	},
{1	,	6049	},
{1	,	6317	},
{1	,	6400	},
{1	,	6889	},
{1	,	7194	},
{1	,	7512	},
{1	,	7845	},
{1	,	8192	},
};

static exifMNBufCB_t mnBufCB[EXIF_MN_TOTAL];
SSTRDTAG P0thIfd[sizeof(SSTRDCHUNK)] = {{0}};
unsigned char pAviStrdChuck[sizeof(SSTRDCHUNK)];

UINT32	u32VariableDataOffset = sizeof(SSTRDTAG)*POTHIFD_MAX;
UINT32	uVariableDataGlobalOffset = 2;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
void aeExif_DbgLogInit(void);
void aeExif_DbgLogSave(void);
extern void exifAscRTCGet(UINT8 *ptimeBuf);
extern UINT32 isoValue[];
extern void flashExif_DbgLogInit(void);
extern void flashExif_DbgLogSave(void);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
/*paul 2018.04.16 add for modify compiler warning*/
extern void jet_LatiLogi_string_to_rational(char *str, UINT32 *dwbuf);
#endif
extern UINT8* awb_db_buf;

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
/*--------------------------------------------------------------------------*
* Function name : AppExifWriteAppnCb 
* Function		: exif write
* Return value	: UINT32 
* Parameter1	: UINT32 addr
* Parameter2	: UINT32 size
* Parameter3	: sp5kExifAppnCbInfo_t *pinfo
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32
AppExifWriteAppnCb(
	UINT32 addr,
	UINT32 size,
	sp5kExifAppnCbInfo_t *pinfo
)
{
	static UINT32 flag = 0;
	static UINT32 leftSize, saveSize;
	
	if (flag == 2) {
		UINT32 awbSize;
		//write app6 for wb info.
		pinfo->index = 6;
		//awb_db_buf = (UINT8 *)sp5kMalloc(32*1024);

		awbSize = appAWBALG_WriteAppn((UINT32)addr, 32*1024);

		appAEALG_WriteAppn((UINT32)(addr+awbSize), 32*1024-awbSize);
		//memcpy(addr, awb_db_buf, (1024*32));
		flag = 3;
		//sp5kFree(awb_db_buf);
		return (1024*32);

	}

	if (flag == 3)
	{
		flag = 0;
		saveSize = 0;
		return 0;
	}

	/* In case the internal buffer is not enough */
	if (pinfo->nailSize == 0)
	{
		flag = 0;
		return 0;
	}
	pinfo->index = 5;//APP5
	if (flag == 0)
	{
		leftSize = pinfo->nailSize;
		flag = 1;
	}
	if (leftSize < size)
	{
		size = leftSize;
	}
	leftSize -= size;
	if (leftSize == 0)
	{
		flag = 2;
	}
	memcpy((UINT8 *)addr, (UINT8 *)pinfo->nailAddr + saveSize, size);
	saveSize += size;
	return size;
}


/*--------------------------------------------------------------------------*
* Function name : appExifInit 
* Function		: exif init
* Return value	: void 
* Parameter1	: void
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void
appExifInit(
	void
)
{
	UINT8* fwVer;
	UINT8 buf[8];
	UINT32* dwbuf=(UINT32*)buf;
	sp5kImgSize_t size;

	DBG_PRINT("exif: __APP_EXIF_INIT__\n");
	fwVer = appBSPVersionGet();
	size.width = 640;
	size.height = 480;

	sp5kStillCapCfgSet(SP5K_CAPTURE_QV_RESOLUTION, (UINT32)&size);
	sp5kStillCapCfgSet(SP5K_CAPTURE_MAX_QV_COMPRESSION_RATIO, 1000);
	sp5kStillCapCfgSet(SP5K_CAPTURE_MAX_THM_COMPRESSION_RATIO, 1000);
	sp5kExifCfgSet(SP5K_EXIF_CFG_MPF_SUPPORT, 1); /* support mpf big thumb */
	sp5kSystemCfgSet(SP5K_EXIF_MAKERNOTE_SIZE_CFG, EXIF_MN_BUF_SIZE);
	#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	sp5kExifCfgSet(SP5K_EXIF_CFG_GPS_IFD_SUPPORT, 1);
	#endif
	sp5kExifInit();
	sp5kExifAppnParamSet(SP5K_EXIF_APPN_WRITE_CALLBACK, 0x00);

	// These tas we don't need for DSC
	sp5kExifEncodeTagDelete(SP5K_IFD_TYPE_ZEROIFD, SP5K_IFD0_TAG_IMGDESC);
	sp5kExifEncodeTagDelete(SP5K_IFD_TYPE_EXIFIFD, SP5K_EXIF_TAG_AUDIOFILE);

	// These new tags will not change after poweron.
	sp5kExifEncodeTagAdd(SP5K_IFD_TYPE_ZEROIFD, APP_EXIF_TAG_FW_VER, SP5K_IFD_DATATYPE_UNDEFINED, strlen((const char *)fwVer), fwVer);
	sp5kExifEncodeTagAdd(SP5K_IFD_TYPE_ZEROIFD, APP_EXIF_TAG_COPYRIGHT, SP5K_IFD_DATATYPE_UNDEFINED, strlen((const char *)APP_EXIF_COPYRIGHT), (UINT8 *)APP_EXIF_COPYRIGHT);

	// These new tags will change after poweron.
	sp5kExifEncodeTagAdd(SP5K_IFD_TYPE_EXIFIFD, SP5K_EXIF_TAG_MAKERNOTE, SP5K_IFD_DATATYPE_UNDEFINED, EXIF_MN_BUF_SIZE, mnBuf);
	sp5kExifEncodeTagAdd(SP5K_IFD_TYPE_EXIFIFD, APP_EXIF_TAG_EXP_MODE, SP5K_IFD_DATATYPE_SHORT, 1, buf);
	sp5kExifEncodeTagAdd(SP5K_IFD_TYPE_EXIFIFD, APP_EXIF_TAG_FOCALLENGTH_32MMFILE, SP5K_IFD_DATATYPE_SHORT, 1, buf);
	sp5kExifEncodeTagAdd(SP5K_IFD_TYPE_EXIFIFD, APP_EXIF_TAG_SCENE_MODE, SP5K_IFD_DATATYPE_SHORT, 1, buf);
	sp5kExifEncodeTagAdd(SP5K_IFD_TYPE_EXIFIFD, APP_EXIF_TAG_SHARPNESS, SP5K_IFD_DATATYPE_SHORT, 1, buf);
	sp5kExifEncodeTagAdd(SP5K_IFD_TYPE_EXIFIFD, APP_EXIF_TAG_WB_MODE, SP5K_IFD_DATATYPE_SHORT, 1, buf);
	sp5kExifEncodeTagAdd(SP5K_IFD_TYPE_EXIFIFD, APP_EXIF_TAG_APERTURE, SP5K_IFD_DATATYPE_RATIONAL, 1, buf);
	sp5kExifEncodeTagAdd(SP5K_IFD_TYPE_EXIFIFD, APP_EXIF_TAG_SHUTTER_SPEED, SP5K_IFD_DATATYPE_SRATIONAL, 1, buf);
	sp5kExifEncodeTagAdd(SP5K_IFD_TYPE_EXIFIFD, APP_EXIF_TAG_DIGITAL_ROOM_RATION, SP5K_IFD_DATATYPE_SHORT, 1, buf);

	#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	UINT8 uVersion[4] = { 0x02, 0x03, 0x00, 0x00 }; 
	sp5kExifEncodeTagValueSet(SP5K_IFD_TYPE_GPSIFD, SP5K_GPS_TAG_VERSIONID, SP5K_IFD_DATATYPE_BYTE, sizeof(uVersion), uVersion ); 
    sp5kExifEncodeTagAdd(SP5K_IFD_TYPE_GPSIFD, SP5K_GPS_TAG_LATITUDEREF, SP5K_IFD_DATATYPE_ASCII, 2, buf);
	sp5kExifEncodeTagAdd(SP5K_IFD_TYPE_GPSIFD, SP5K_GPS_TAG_LONGITUDEREF, SP5K_IFD_DATATYPE_ASCII, 2, buf);
	sp5kExifEncodeTagAdd(SP5K_IFD_TYPE_GPSIFD, SP5K_GPS_TAG_LATITUDE, SP5K_IFD_DATATYPE_RATIONAL, 3, buf);
	sp5kExifEncodeTagAdd(SP5K_IFD_TYPE_GPSIFD, SP5K_GPS_TAG_LONGITUDE, SP5K_IFD_DATATYPE_RATIONAL, 3, buf);
	sp5kExifEncodeTagAdd(SP5K_IFD_TYPE_GPSIFD, SP5K_GPS_TAG_SPEED, SP5K_IFD_DATATYPE_RATIONAL, 1, buf);
	sp5kExifEncodeTagAdd(SP5K_IFD_TYPE_GPSIFD, SP5K_GPS_TAG_IMGDIR, SP5K_IFD_DATATYPE_RATIONAL, 1, buf);
	sp5kExifEncodeTagAdd(SP5K_IFD_TYPE_GPSIFD, SP5K_GPS_TAG_TRACK, SP5K_IFD_DATATYPE_RATIONAL, 1, buf);
	#endif

	// These sp5k tags will not change.
	appExifSetMake(APP_EXIF_MAKE);
	appExifSetModel(APP_EXIF_MODEL);
	appExifSetExifVersion(APP_EXIF_VERSION);
	dwbuf[0]=0x120;
	dwbuf[1]=0x003;
	appExifSetXResolution(buf);
	appExifSetYResolution(buf);

	// These is default value after power on.
	appExifSetWBMode(buf);
	dwbuf[0] = MAX_APER_X100;
	dwbuf[1] = 100;
	appExifSetMaxAperture(buf);

	appExif_MN_Init();
}
/*--------------------------------------------------------------------------*
* Function name : appExifAscRTCGet 
* Function		: get exif asc rtc
* Return value	: void 
* Parameter1	: UINT8 *ptimeBuf
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void
appExifAscRTCGet(UINT8 *ptimeBuf)
{
	exifAscRTCGet(ptimeBuf);
}
/*--------------------------------------------------------------------------*
* Function name : appExif_SetExpProgram 
* Function		: exif set exp program
* Return value	: void 
* Parameter1	: UINT32 curMode
* Parameter2	: UINT32 curScene
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void
appExif_SetExpProgram(
	UINT32 curMode,
	UINT32 curScene
)
{
	UINT8 buf[4];
	UINT16* uwbuf = (UINT16*)buf;

	switch(curMode)
	{
	case PASM_PROGRAM:
		uwbuf[0] = SP5K_EXIF_TAGDATA_EXPPROG_NORMAL;
		break;
	default:
		uwbuf[0] = SP5K_EXIF_TAGDATA_EXPPROG_NOTDEFINED;
		break;
	}
	appExifSetExpProgram(buf);
	DBG_PRINT("exif: %s: %d\n",__FUNCTION__, uwbuf[0]);
}
/*--------------------------------------------------------------------------*
* Function name : appExif_SetExpBias 
* Function		: exif set exp bias
* Return value	: void 
* Parameter1	: UINT32 expBiasIdx
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void
appExif_SetExpBias(
	UINT32 expBiasIdx
)
{
	UINT8 buf[8];
	SINT32* lbuf = (SINT32*)buf;
	static SINT16 expBiasLut[]=
	{
		[UI_EXP_BIAS_N200]=SP5K_EXIF_TAGDATA_EXP_BIAS_N200,
		[UI_EXP_BIAS_N166]=SP5K_EXIF_TAGDATA_EXP_BIAS_N166,
		[UI_EXP_BIAS_N133]=SP5K_EXIF_TAGDATA_EXP_BIAS_N133,
		[UI_EXP_BIAS_N100]=SP5K_EXIF_TAGDATA_EXP_BIAS_N100,
		[UI_EXP_BIAS_N066]=SP5K_EXIF_TAGDATA_EXP_BIAS_N066,
		[UI_EXP_BIAS_N033]=SP5K_EXIF_TAGDATA_EXP_BIAS_N033,
		[UI_EXP_BIAS_ZERO]=SP5K_EXIF_TAGDATA_EXP_BIAS_ZERO,
		[UI_EXP_BIAS_P033]=SP5K_EXIF_TAGDATA_EXP_BIAS_P033,
		[UI_EXP_BIAS_P066]=SP5K_EXIF_TAGDATA_EXP_BIAS_P066,
		[UI_EXP_BIAS_P100]=SP5K_EXIF_TAGDATA_EXP_BIAS_P100,
		[UI_EXP_BIAS_P133]=SP5K_EXIF_TAGDATA_EXP_BIAS_P133,
		[UI_EXP_BIAS_P166]=SP5K_EXIF_TAGDATA_EXP_BIAS_P166,
		[UI_EXP_BIAS_P200]=SP5K_EXIF_TAGDATA_EXP_BIAS_P200,
	};

	lbuf[0]=expBiasLut[expBiasIdx];
	lbuf[1]=100;
	appExifSetExpBias(buf);
	DBG_PRINT("exif: %s: %s%d/%d\n",__FUNCTION__, lbuf[0]>0? "+":"-", lbuf[0]>0? lbuf[0]:(-lbuf[0]), lbuf[1]);
}
/*--------------------------------------------------------------------------*
* Function name : appExif_SetMeterMode 
* Function		: exif set meter mode
* Return value	: void 
* Parameter1	: UINT32 Metering
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void
appExif_SetMeterMode(
	UINT32 Metering
)
{
	UINT8 buf[4];
	UINT16 *wbuf = (UINT16*)buf;

	if (Metering == UI_METERING_CENTER) {
		wbuf[0] = SP5K_EXIF_TAGDATA_METERMODE_CENTERWEIGHTED;
	}
	else if (Metering == UI_METERING_MULTI) {
		wbuf[0] = SP5K_EXIF_TAGDATA_METERMODE_MULTISPOT;
	}
	else if (Metering == UI_METERING_SPOT) {
		wbuf[0] = SP5K_EXIF_TAGDATA_METERMODE_SPOT;
	}
	appExifSetMeterMode(buf);
	DBG_PRINT("exif: %s: %d\n",__FUNCTION__, wbuf[0]);
}
/*--------------------------------------------------------------------------*
* Function name : appExif_SetFlash 
* Function		: exif set flash mode
* Return value	: void 
* Parameter1	: UINT32 flashMode
* Parameter2	: UINT32 isFired
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void
appExif_SetFlash(
	UINT32 flashMode,
	UINT32 isFired

)
{
	UINT8 buf[4];
	UINT16 *wbuf = (UINT16*)buf;

	switch (flashMode)
	{
	case UI_FLASH_AUTO:
		wbuf[0] = (isFired)? 25: 24;
		break;
	case UI_FLASH_RED_EYE:
		wbuf[0] = (isFired)? 89: 0;
		break;
	case UI_FLASH_ON:
		wbuf[0] = 13;
		break;
	case UI_FLASH_SLOW_SYNC:
		wbuf[0] = (isFired)? 29: 0;
		break;
	case UI_FLASH_OFF:
		wbuf[0] = 16;
		break;
	}
	appExifSetFlash(buf);
	DBG_PRINT("exif: %s: %d\n",__FUNCTION__, wbuf[0]);
}
/*--------------------------------------------------------------------------*
* Function name : appExif_SetLightSource 
* Function		: exif set light source
* Return value	: void 
* Parameter1	: UINT32 wbMode
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void
appExif_SetLightSource(
	UINT32 wbMode
)
{
	UINT8 buf[4];
	UINT16 *wbuf = (UINT16*)buf;

	switch (wbMode)
	{
		default:
		case UI_WB_AUTO:
		case UI_WB_CUSTOM:
			wbuf[0] = SP5K_EXIF_TAGDATA_LIGHTSOURCE_UNKNOWN;
			break;
		case UI_WB_DAYLIGHT:
			wbuf[0] = SP5K_EXIF_TAGDATA_LIGHTSOURCE_DAYLIGHT;
			break;
		case UI_WB_CLOUDY:
			wbuf[0] = SP5K_EXIF_TAGDATA_LIGHTSOURCE_CLOUDY_WEATHER;
			break;
		case UI_WB_TUNGSTEN:
			wbuf[0] = SP5K_EXIF_TAGDATA_LIGHTSOURCE_TUNGSTEN;
			break;
		case UI_WB_FLUORESCENT_H:
			wbuf[0] = SP5K_EXIF_TAGDATA_LIGHTSOURCE_DAYLIGHT_FLUORESCENT;
			break;
		case UI_WB_FLUORESCENT_L:
			wbuf[0] = SP5K_EXIF_TAGDATA_LIGHTSOURCE_DAYLIGHT_FLUORESCENT;
			break;
		case UI_WB_FLUORESCENT:
			wbuf[0] = SP5K_EXIF_TAGDATA_LIGHTSOURCE_DAY_WHITE_FLUORESCENT;
			break;
	}

	appExifSetLightSource(buf);
	DBG_PRINT("exif: %s: %d\n",__FUNCTION__, wbuf[0]);
}
/*--------------------------------------------------------------------------*
* Function name : appExif_SetSceneCaptureType 
* Function		: exif set scene capture type
* Return value	: void 
* Parameter1	: UINT32 curMode
* Parameter2	: UINT32 curScene
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void
appExif_SetSceneCaptureType(
	UINT32 curMode,
	UINT32 curScene
)
{
	UINT8 buf[4];
	UINT16 *wbuf = (UINT16*)buf;

	wbuf[0] = SP5K_EXIF_TAGDATA_SCENECAPTURETYPE_STANDARD;

	appExifSetSceneCaptureType(buf);
	DBG_PRINT("exif: %s: %d\n",__FUNCTION__, wbuf[0]);
}
/*--------------------------------------------------------------------------*
* Function name : appExif_SetSharpness 
* Function		: exif set sharpness
* Return value	: void 
* Parameter1	: UINT32 sharpness
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void
appExif_SetSharpness(
	UINT32 sharpness
)
{
	UINT8 buf[4];
	UINT16 *wbuf = (UINT16*)buf;

	switch (sharpness)
	{
	case UI_SHARP_MORE:
		wbuf[0] = SP5K_EXIF_TAGDATA_SHARPNESS_HARD;
		break;
	case UI_SHARP_NORMAL:
		wbuf[0] = SP5K_EXIF_TAGDATA_SHARPNESS_SOFT;
		break;
	case UI_SHARP_LESS:
		wbuf[0] = SP5K_EXIF_TAGDATA_SHARPNESS_NORMAL;
		break;
	default:
		break;
	}
	appExifSetSharpness(buf);
	DBG_PRINT("exif: %s: %d\n",__FUNCTION__, wbuf[0]);
}
/*--------------------------------------------------------------------------*
* Function name : appExif_SetSharpness 
* Function		: exif encode
* Return value	: UINT32 
* Parameter1	: UINT32 imgId
* Parameter2	: void *reserved2
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32
appExifEncodeCb(
	UINT32 imgId,
	UINT32 reserved1,
	void *reserved2
)
{
	/*s Modify by Aries 10/05/19*/
	//UINT32 agc;
	UINT8 buf[32];
	UINT32 *dwbuf=(UINT32 *)buf;
	SINT32 *lbuf=(SINT32 *)buf;
	UINT16 *wbuf=(UINT16 *)buf;
	//sp5kEvUnit_t Ev;
	appAeSet_t* pAe;
	UINT32 awbSize=0;

	profLogAdd(0, "Exif CB -s");
	DBG_PRINT("exif: @@@@@@@@@@@@@\n");
	DBG_PRINT("exif: __APP STILL EXIF CB__\n");


	dwbuf[0] = shutterSpeed[gStillCB.ae.shutter][0];
	dwbuf[1] = shutterSpeed[gStillCB.ae.shutter][1];
	appExifSetExpTime(buf);

	DBG_PRINT("exif: @@@@@@@@@@@@@\n");
	DBG_PRINT("exif: exp time =%d/%d\n", dwbuf[0], dwbuf[1]);
	// TV = - log  (shutter)
	//			 2
	// 4.9 = - log	(1/30)
	//			  2
	dwbuf[2] = dwbuf[0];
	dwbuf[3] = dwbuf[1];
	log2xyND((SINT32)dwbuf[2],(SINT32)dwbuf[3],(SINT32*)&lbuf[0],(SINT32*)&lbuf[1]);
	lbuf[0] = -lbuf[0];
	appExifSetShutSpeed(buf);
	DBG_PRINT("exif: TV =%d/%d\n", lbuf[0], lbuf[1]);

	dwbuf[0] = aperture_x100[gStillCB.lensZoomSeg][gStillCB.ae.iris-APERTURE_2_8];
	dwbuf[1] = 100;
	appExifSetFNum(buf);
	DBG_PRINT("exif: f num =%d/%d\n", dwbuf[0], dwbuf[1]);

	// AV = 2* log (f number)
	//			  2
	// 5.67 = 2* log (7.1)
	//			    2
	dwbuf[2] = dwbuf[0];
	dwbuf[3] = dwbuf[1];
	log2xyND((SINT32)dwbuf[2],(SINT32)dwbuf[3],(SINT32*)&dwbuf[0],(SINT32*)&dwbuf[1]);
	dwbuf[0] = dwbuf[0]*2;
	appExifSetAperture(buf);
	DBG_PRINT("exif: AV =%d/%d\n", dwbuf[0], dwbuf[1]);

	wbuf[0] = isoValue[gStillCB.ae.iso];
	appExifSetIsoSpeed(buf);
	DBG_PRINT("exif: ISO =%d\n", wbuf[0]);

	pAe = appAeSetGet();
	appExif_SetExpBias(pAe->evbias);

	appExif_SetLightSource(pViewParam->wb);

	appExif_SetFlash(UI_FLASH_OFF, gStillCB.needFlash);

	dwbuf[0] = (UINT16)appLensQuery(LENS_QUERY_ZOOM_STEP_POS); //lensZoomCurPosGet();
	dwbuf[1] = 1000;
	appExifSetFocalLength(buf);

	dwbuf[0] = appDZoomGetRatio();
	dwbuf[1] = 1000;
	appExifSetDigitalZoomRatio(buf);

	#if !defined(SPCA6350_DEFAWB_MOVEOUT)
	// user may want to add the information for testing.
	SP_IQLogInit();

	//SP_IQAWBLog();
	#endif

	aeExif_DbgLogInit();
	aeExif_DbgLogSave();

	#if SP5K_STROBE_ENAABLE
	flashExif_DbgLogInit();
	flashExif_DbgLogSave();
	#endif

	awbSize = appAWBALG_WriteAppn((UINT32)(&mnBuf[EXIF_MN_IQ_BUF_SIZE+EXIF_MN_PFH_BUF_SIZE+EXIF_MN_AF_BUF_SIZE+EXIF_MN_AE_BUF_SIZE]), EXIF_3A_DEBUG_INFO_SIZE);

	appAEALG_WriteAppn((UINT32)((&mnBuf[EXIF_MN_IQ_BUF_SIZE+EXIF_MN_PFH_BUF_SIZE+EXIF_MN_AF_BUF_SIZE+EXIF_MN_AE_BUF_SIZE])+awbSize), EXIF_3A_DEBUG_INFO_SIZE-awbSize);

	appExifSetMakerNote(EXIF_MN_BUF_SIZE, mnBuf);

	#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	UINT8 buf1[32];
	UINT32 *dwbuf1=(UINT32 *)buf1;
	appExifSetGpsLatitudeRef((UINT8 *)appInfoPositionNIndicatorGet());
	appExifSetGpsLongitudeRef((UINT8 *)appInfoPositionEIndicatorGet());
	jet_LatiLogi_string_to_rational(appInfoPositionLatitudeGet(),dwbuf1);
	appExifSetGpsLatitude(buf1);
	jet_LatiLogi_string_to_rational(appInfoPositionLongitudeGet(),dwbuf1);
	appExifSetGpsLongitude(buf1);
	//printf("[%s] CarSpeed = %s, %d\n",__FUNCTION__, appInfoPositionSpeedGet(), (int)(atof(appInfoPositionSpeedGet())*100));
	dwbuf1[0] = (int)(atof(appInfoPositionSpeedGet())*100);
	dwbuf1[1] = 100;
	appExifSetGpsSpeed(buf1);
	//printf("[%s] CarAngle = %s, %d\n",__FUNCTION__, appInfoPositionAngleGet(), atoi(appInfoPositionAngleGet()));
	dwbuf1[0] = atoi(appInfoPositionAngleGet());
	dwbuf1[1] = 1;
	appExifSetGpsDirection(buf1);
	appExifSetGpsTrack(buf1);
	#endif
	profLogAdd(0, "Exif CB -e");
	return SUCCESS;
	/*e Modify by Aries 10/05/19*/
}
/*--------------------------------------------------------------------------*
* Function name : appExif_MN_Init 
* Function		: exif MN init
* Return value	: void 
* Parameter1	: void
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appExif_MN_Init(void)
{
	UINT8 *pBuf;

	pBuf = (UINT8 *)mnBuf;

	/*-- Initialize EXIF Maker Note buffer control block
	*/
	mnBufCB[EXIF_MN_IQ].bufBase = pBuf;
	mnBufCB[EXIF_MN_IQ].bufWPtr = mnBufCB[EXIF_MN_IQ].bufBase;
	mnBufCB[EXIF_MN_IQ].bufSize = EXIF_MN_IQ_BUF_SIZE;
	pBuf += EXIF_MN_IQ_BUF_SIZE;

	mnBufCB[EXIF_MN_PFH].bufBase = pBuf;
	mnBufCB[EXIF_MN_PFH].bufWPtr = mnBufCB[EXIF_MN_PFH].bufBase;
	mnBufCB[EXIF_MN_PFH].bufSize = EXIF_MN_PFH_BUF_SIZE;
	pBuf += EXIF_MN_PFH_BUF_SIZE;

	mnBufCB[EXIF_MN_AF].bufBase = pBuf;
	mnBufCB[EXIF_MN_AF].bufWPtr = mnBufCB[EXIF_MN_AF].bufBase;
	mnBufCB[EXIF_MN_AF].bufSize = EXIF_MN_AF_BUF_SIZE;
	pBuf += EXIF_MN_AF_BUF_SIZE;

	mnBufCB[EXIF_MN_AE].bufBase = pBuf;
	mnBufCB[EXIF_MN_AE].bufWPtr = mnBufCB[EXIF_MN_AE].bufBase;
	mnBufCB[EXIF_MN_AE].bufSize = EXIF_MN_AE_BUF_SIZE;
	/*cppcheck Removed:Variable 'pBuf' is assigned a value that is never used.*/
	/*pBuf += EXIF_MN_AE_BUF_SIZE;*/

	#if HOST_DBG
	{
		int i;
		DBG_PRINT("\n");
		DBG_PRINT("----------------------------------\n");
		for (i=0; i<EXIF_MN_TOTAL; i++)
		{
			DBG_PRINT("blk[%d]: base(0x%x), wptr(0x%x), size(%d) \n",
					i, mnBufCB[i].bufBase,
					mnBufCB[i].bufWPtr,
					mnBufCB[i].bufSize);
		}
		DBG_PRINT("----------------------------------\n");
		DBG_PRINT("\n");
	}
	#endif
}
/*--------------------------------------------------------------------------*
* Function name : appExif_MN_BufClear 
* Function		: exif MN buf clear
* Return value	: void 
* Parameter1	: UINT8 bufID
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appExif_MN_BufClear(UINT8 bufID)
{
	UINT8 *bufBase;
	UINT32 bufSize;

	DBG_PRINT("exif: %s ...", __FUNCTION__);

	/*-- Check if bufID is valid */
	if (bufID >= EXIF_MN_TOTAL)
	{
		DBG_PRINT("exif: MN_Print: Invalid ID(%d) !!! \n", bufID);
		return;
	}

	/*-- Initialize EXIF Maker Note buffer control block
	*/
	bufBase = mnBufCB[bufID].bufBase;
	mnBufCB[bufID].bufWPtr = bufBase;
	bufSize = mnBufCB[bufID].bufSize;
	memset(bufBase, 0, bufSize);
}
/*--------------------------------------------------------------------------*
* Function name : appExif_MN_Printf 
* Function		: exif MN printf
* Return value	: UINT32 
* Parameter1	: UINT8 bufID
* Parameter2	: const char *szFmt
* Parameter3	: C - Variable Arguments
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32  appExif_MN_Printf(UINT8 bufID, const char *szFmt, ...)
{
    va_list vaPtr;
    unsigned char szBuffer[128];
	UINT32 buflen,ret;

	UINT8 *bufWPtr, *bufBase;
	UINT32 bufSize;

	DBG_PRINT("exif: %s ...", __FUNCTION__);

	/*-- Check if bufID is valid */
	if (bufID >= EXIF_MN_TOTAL)
	{
		DBG_PRINT("exif: MN_Print: Invalid ID(%d) !!! \n", bufID);
		return FAIL;
	}
	memset(szBuffer, 0, 128);

    va_start(vaPtr, (const char *)szFmt);
    vsprintf((char *)szBuffer, (const char *)szFmt, vaPtr);
	va_end(vaPtr);
	buflen=strlen((const char *)szBuffer);

	/*-- strings size checkup */
	if (buflen>(128-1))
	{
		DBG_PRINT("exif: MN_Print: Strings size overflow(%d) !!! \n", buflen);
		return FAIL;
	}

	bufWPtr = mnBufCB[bufID].bufWPtr;
	bufBase = mnBufCB[bufID].bufBase;
	bufSize = mnBufCB[bufID].bufSize;

	if((bufWPtr - bufBase + buflen) > bufSize )
	{
		DBG_PRINT("exif: MN_Print: Buf overflow(%d) !!! \n", bufSize);
		return FAIL;
	}
	memcpy(bufWPtr, szBuffer, buflen);
	bufWPtr += buflen;
	mnBufCB[bufID].bufWPtr = bufWPtr;


	ret=appExifSetMakerNote(buflen, mnBufCB[bufID].bufWPtr);

    if(ret!=SUCCESS)
       HOST_PROF_LOG_PRINT(LEVEL_ERROR, "set: %s %d",__FUNCTION__,__LINE__);

	return SUCCESS;
}
/*--------------------------------------------------------------------------*
* Function name : appExif_MN_WriteBytes 
* Function		: exif MN write bytes
* Return value	: UINT32 
* Parameter1	: UINT8 bufID
* Parameter2	: UINT8 *pdata
* Parameter3	: UINT32 size
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32 appExif_MN_WriteBytes(UINT8 bufID, UINT8 *pdata, UINT32 size)
{
	UINT8 *bufWPtr, *bufBase;
	UINT32 bufSize;

	DBG_PRINT("exif: %s ...", __FUNCTION__);

	/*-- Check if bufID is valid */
	if (bufID >= EXIF_MN_TOTAL)
	{
		DBG_PRINT("exif: MN_Print: Invalid ID(%d) !!! \n", bufID);
		return FAIL;
	}

	bufWPtr = mnBufCB[bufID].bufWPtr;
	bufBase = mnBufCB[bufID].bufBase;
	bufSize = mnBufCB[bufID].bufSize;

	if((bufWPtr - bufBase + size) > bufSize )
	{
		DBG_PRINT("exif: MN_Print: Buf overflow(%d) !!! \n", bufSize);
		DBG_PRINT("exif: base(%x), wptr(%x), size(%d) \n", bufBase, bufWPtr, size);
		return FAIL;
	}

	memcpy(bufWPtr, pdata, size);

	bufWPtr += size;
	mnBufCB[bufID].bufWPtr = bufWPtr;

	return SUCCESS;
}
/*--------------------------------------------------------------------------*
* Function name : appExif_MN_Write8 
* Function		: exif MN write 1 byte
* Return value	: UINT32 
* Parameter1	: UINT8 bufID
* Parameter2	: UINT8 data
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32 appExif_MN_Write8(UINT8 bufID, UINT8 data)
{
	return appExif_MN_WriteBytes(bufID, &data, 1);
}
/*--------------------------------------------------------------------------*
* Function name : appExif_MN_Write16 
* Function		: exif MN write 2 byte
* Return value	: UINT32 
* Parameter1	: UINT8 bufID
* Parameter2	: UINT8 data
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32 appExif_MN_Write16(UINT8 bufID, UINT16 data)
{
	return appExif_MN_WriteBytes(bufID, (UINT8 *)&data, 2);
}
/*--------------------------------------------------------------------------*
* Function name : appExif_MN_Write32 
* Function		: exif MN write 4 byte
* Return value	: UINT32 
* Parameter1	: UINT8 bufID
* Parameter2	: UINT8 data
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32 appExif_MN_Write32(UINT8 bufID, UINT32 data)
{
	return appExif_MN_WriteBytes(bufID, (UINT8 *)&data, 4);
}
/*--------------------------------------------------------------------------*
* Function name : appExif_MN_WriteUID 
* Function		: exif MN write UID
* Return value	: UINT32 
* Parameter1	: UINT8 bufID
* Parameter2	: UINT32 uid
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32 appExif_MN_WriteUID(UINT8 bufID, UINT32 uid)
{
	UINT32 bigendian;

	bigendian =   ((uid & 0xFF000000)>>24)
				+ ((uid & 0x00FF0000)>>8)
				+ ((uid & 0x0000FF00)<<8)
				+ ((uid &0x000000FF)<<24);
	return appExif_MN_WriteBytes(bufID, (UINT8 *)&bigendian, 4);
}
/*--------------------------------------------------------------------------*
* Function name : appExif_SetAviVariableDataOffset 
* Function		: set avi variable data offset
* Return value	: void 
* Parameter1	: UINT32 offset
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appExif_SetAviVariableDataOffset(UINT32 offset)
{
	u32VariableDataOffset = offset;
	uVariableDataGlobalOffset += offset;
}
/*--------------------------------------------------------------------------*
* Function name : appExif_SetAviRiffASCII 
* Function		: set avi riff ascii
* Return value	: void 
* Parameter1	: PSSTRDTAG pointer
* Parameter2	: unsigned short offset
* Parameter3	: unsigned short tagcode
* Parameter4	: UINT32 count
* Parameter5	: char* pcStr
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appExif_SetAviRiffASCII (PSSTRDTAG pointer, unsigned short offset, unsigned short tagcode, UINT32 count,  char* pcStr)
{
	pointer[offset].TagCode = tagcode;
	pointer[offset].Type 	= STRD_TAG_TYPE_ASCII;
	pointer[offset].Count	= count;
	pointer[offset].Value	= uVariableDataGlobalOffset;

	if ( *pcStr == 0 ) return;

	memcpy( ((unsigned char*)pointer) + u32VariableDataOffset, pcStr, count);

	u32VariableDataOffset += count;
	uVariableDataGlobalOffset  += count;

}
/*--------------------------------------------------------------------------*
* Function name : appExif_SetAviRiffShort 
* Function		: set avi riff short
* Return value	: void 
* Parameter1	: PSSTRDTAG pointer
* Parameter2	: unsigned short offset
* Parameter3	: unsigned short tagcode
* Parameter4	: UINT32 count
* Parameter5	: UINT32 value
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appExif_SetAviRiffShort ( PSSTRDTAG pointer, unsigned short offset, unsigned short tagcode, UINT32 count, UINT32 value)
{
	pointer[offset].TagCode = tagcode;
	pointer[offset].Type 	= STRD_TAG_TYPE_SHORT;
	pointer[offset].Count 	= count;
	pointer[offset].Value 	= value;

}
/*--------------------------------------------------------------------------*
* Function name : appExif_AddAviPaddingData 
* Function		: add avi padding data
* Return value	: void 
* Parameter1	: unsigned char* pbPtr
* Parameter2	: unsigned char* pbData
* Parameter3	: UINT32 uSize
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appExif_AddAviPaddingData ( unsigned char* pbPtr, unsigned char* pbData, UINT32 uSize)
{
	memcpy( pbPtr+u32VariableDataOffset, pbData, uSize );
	u32VariableDataOffset += uSize;
	uVariableDataGlobalOffset += uSize;
}
/*--------------------------------------------------------------------------*
* Function name : appExif_SetAviChunk 
* Function		: set avi chunk
* Return value	: void 
* Parameter1	: void
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appExif_SetAviChunk(void)
{


	unsigned char* pbAviExifPtr;
	view_param_t* puiView;

	uVariableDataGlobalOffset = 2;	/*dw++@20120529. Bugfix #44981.*/

	puiView = appViewParamGet();

	pbAviExifPtr = pAviStrdChuck;

	/*ImageSize*/
	/*RiffStrdStart[RIFF_PROFILE_IMAGESIZE_INDEX] = RIFF_PROFILE_IMAGESIZE_1920;*/
	RiffStrdStart[RIFF_PROFILE_IMAGESIZE_INDEX] = (puiView->videoSize==UI_VIDEO_SIZE_FHD_60FPS)?RIFF_PROFILE_IMAGESIZE_1920:RIFF_PROFILE_IMAGESIZE_1280;

	/*sound INFO*/
	/*RiffStrdStart[RIFF_PROFILE_FRAMERATE_INDEX] = RIFF_PROFILE_FRAMERATE_30FPS;*/
	RiffStrdStart[RIFF_PROFILE_FRAMERATE_INDEX] =  (puiView->videoSize==UI_VIDEO_SIZE_FHD_60FPS)?RIFF_PROFILE_FRAMERATE_60FPS:RIFF_PROFILE_FRAMERATE_30FPS;

	/*sound INFO*/
	RiffStrdStart[RIFF_PROFILE_SOUNDINFO_INDEX] = RIFF_PROFILE_SOUNDINFO;

	memcpy( pbAviExifPtr, RiffStrdStart, sizeof(RiffStrdStart) );
	pbAviExifPtr += sizeof(RiffStrdStart);

	/* 0TH-IFD*/
	appExif_SetAviVariableDataOffset(sizeof(SSTRDTAG)*POTHIFD_MAX);
	appExif_SetAviRiffASCII(P0thIfd, 0, 0x0100, sizeof(MAKERNOTE_MAKER), MAKERNOTE_MAKER);
	appExif_AddAviPaddingData( (unsigned char*)P0thIfd, (unsigned char*)"\x0", 1);

	appExif_SetAviRiffASCII(P0thIfd, 1, 0x0101, sizeof(MAKE_MODEL), MAKE_MODEL);
	appExif_AddAviPaddingData( (unsigned char*)P0thIfd, (unsigned char*)"\x0", 1);

	if(puiView->videoStamp){
		appExif_SetAviRiffShort(P0thIfd, 2, 0x0102, 1, 1); /*0 :Datestamp Off;1 Datestamp On*/
	}else{
		appExif_SetAviRiffShort(P0thIfd, 2, 0x0102, 1, 0); /*0 :Datestamp Off;1 Datestamp On*/
	}
	memcpy( pbAviExifPtr, P0thIfd,u32VariableDataOffset);
	uVariableDataGlobalOffset += sizeof (RiffStrdStart);

	sp5kMediaRecAviChunkSet(MEDIA_AVI_CHUNK_STRD, pAviStrdChuck, uVariableDataGlobalOffset);
}
/*--------------------------------------------------------------------------*
* Function name : appExif_GetAviChunkInfo 
* Function		: get avi chunk information
* Return value	: void 
* Parameter1	: UINT32 *poffset
* Parameter2	: UINT32 *size
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appExif_GetAviChunkInfo(UINT32 *poffset,UINT32 *size)
{

	UINT32 strdsize;
	UINT32 strdOffset;

	sp5kMediaPlayAviChunkInfoGet(MEDIA_AVI_CHUNK_STRD,&strdOffset,&strdsize);
	*poffset =strdOffset;
	*size = strdsize;
	#if 0
	printf("strdChunksize:%d\n",strdsize);
	printf("strdChunkOffset:%d\n",strdOffset);
	#endif

}
/*--------------------------------------------------------------------------*
* Function name : appExif_GetAviChunk 
* Function		: get avi chunk 
* Return value	: void 
* Parameter1	: void
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appExif_GetAviChunk(void)
{

	UINT32 strdChunksize;
	UINT32 strdChunkOffset;
	UINT8 *pbuf;

	appExif_GetAviChunkInfo(&strdChunkOffset,&strdChunksize);

	pbuf = (UINT8 *)sp5kMalloc(strdChunksize);
	memset(pbuf, 0, strdChunksize);
	sp5kMediaPlayAviChunkBufSet(MEDIA_AVI_CHUNK_STRD, pbuf, strdChunksize);

	#if 0
	printf("strdChunksize:%d\n",strdChunksize);
	printf("strdChunkOffset:0x%x\n",strdChunkOffset);
	memdump(pbuf,strdChunksize);
	#endif

	sp5kFree(pbuf);

}
/*--------------------------------------------------------------------------*
* Function name : appMpf_setIfdValue 
* Function		: set ifd value
* Return value	: void 
* Parameter1	: UINT32 snapCnt
* Parameter2	: UINT32 rsvd2
* Parameter3	: void *rsvd3
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appMpf_setIfdValue(
	UINT32 snapCnt,
	UINT32 rsvd2,
	void *rsvd3
)
{
	/*MPO host sample code*/
	#if 0
	UINT32 defaultVal;
	UINT32 imgNumber;
	UINT8 angleVal[8] = {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
	UINT8 blLenVal[8] = {0x4d, 0x00, 0x00, 0x00, 0xe8, 0x03, 0x00, 0x00};
	switch(snapCnt) {
	case 1:
		imgNumber = 2;
		/*index IFD*/
		sp5kMpfIfdValueSet(SP5K_MPF_IFD_TYPE_INDEX, SP5K_MPF_IDX_TAG_VERSION, SP5K_MPF_DATATYPE_UNDEFINED, 4,"0100");
		sp5kMpfIfdValueSet(SP5K_MPF_IFD_TYPE_INDEX, SP5K_MPF_IDX_TAG_NUMOFIMG, SP5K_MPF_DATATYPE_LONG, 1, &imgNumber);
		UINT8 *mpEntryVal = (UINT8 *)osMemAlloc(imgNumber * 16);
		UINT8 *uniIdVal = (UINT8 *)osMemAlloc(imgNumber * 33);
		memset(mpEntryVal,0x00,imgNumber * 16);
		memset(uniIdVal,0x00,imgNumber * 33);

		UINT32 hostMpType;
		hostMpType = (0x20 << 24) | SP5K_MPF_MP_ENTRY_TYPE_MVDISPARITY;
		memcpy(mpEntryVal,&hostMpType,4);
		hostMpType = (0x0 << 24) | SP5K_MPF_MP_ENTRY_TYPE_MVDISPARITY;
		memcpy(mpEntryVal+16,&hostMpType,4);

		sp5kMpfIfdValueSet(SP5K_MPF_IFD_TYPE_INDEX, SP5K_MPF_IDX_TAG_MPENTRY, SP5K_MPF_DATATYPE_UNDEFINED, imgNumber * 16, mpEntryVal);

		memset(uniIdVal,0x31,33);
		memset(uniIdVal+33,0x32,33);
		sp5kMpfIfdValueSet(SP5K_MPF_IFD_TYPE_INDEX, SP5K_MPF_IDX_TAG_IMGUNID, SP5K_MPF_DATATYPE_UNDEFINED, imgNumber * 33, uniIdVal);

		/*attribute IFD*/
		defaultVal = 1;
		sp5kMpfIfdValueSet(SP5K_MPF_IFD_TYPE_ATTRIBUTE, SP5K_MPF_ATTR_TAG_IDXOFIMG, SP5K_MPF_DATATYPE_LONG, 1, &snapCnt);
		sp5kMpfIfdValueSet(SP5K_MPF_IFD_TYPE_ATTRIBUTE, SP5K_MPF_ATTR_TAG_BASEVIEWPOINTNUM, SP5K_MPF_DATATYPE_LONG, 1, &defaultVal);
		sp5kMpfIfdValueSet(SP5K_MPF_IFD_TYPE_ATTRIBUTE, SP5K_MPF_ATTR_TAG_CONVERGENCEANGLE, SP5K_MPF_DATATYPE_SRATIONAL, 1, angleVal);
		sp5kMpfIfdValueSet(SP5K_MPF_IFD_TYPE_ATTRIBUTE, SP5K_MPF_ATTR_TAG_BASELINELENGTH, SP5K_MPF_DATATYPE_RATIONAL, 1, blLenVal);

		break;
	case 2:
		imgNumber = 2;

		/*attribute IFD*/
		defaultVal = 1;
		angleVal[0] = 0x16;
		angleVal[4] = 0x0a;
		sp5kMpfIfdValueSet(SP5K_MPF_IFD_TYPE_ATTRIBUTE, SP5K_MPF_ATTR_TAG_VERSION, SP5K_MPF_DATATYPE_UNDEFINED, 4,"0100");
		sp5kMpfIfdValueSet(SP5K_MPF_IFD_TYPE_ATTRIBUTE, SP5K_MPF_ATTR_TAG_IDXOFIMG, SP5K_MPF_DATATYPE_LONG, 1, &snapCnt);
		sp5kMpfIfdValueSet(SP5K_MPF_IFD_TYPE_ATTRIBUTE, SP5K_MPF_ATTR_TAG_BASEVIEWPOINTNUM, SP5K_MPF_DATATYPE_LONG, 1, &defaultVal);
		sp5kMpfIfdValueSet(SP5K_MPF_IFD_TYPE_ATTRIBUTE, SP5K_MPF_ATTR_TAG_CONVERGENCEANGLE, SP5K_MPF_DATATYPE_SRATIONAL, 1, angleVal);
		sp5kMpfIfdValueSet(SP5K_MPF_IFD_TYPE_ATTRIBUTE, SP5K_MPF_ATTR_TAG_BASELINELENGTH, SP5K_MPF_DATATYPE_RATIONAL, 1, blLenVal);
		break;
	}
	#endif
}

