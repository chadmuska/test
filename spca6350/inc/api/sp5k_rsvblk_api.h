/**************************************************************************
 *
 *       Copyright (c) 2005-2011 by iCatch Technology, Inc.
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
 *  Hsin-Chu, Taiwan, R.O.C.
 *
 *  Author: Phacnia Lai
 *
 **************************************************************************/
#ifndef _SP5K_RSVBLK_API_H_
#define _SP5K_RSVBLK_API_H_

/**************************************************************************
 *                         H E A D E R   F I L E S
 **************************************************************************/
#include "common.h"
#include "middleware/rsvblk_def.h"
#include "middleware/disk_def.h"
#include "api/sp5k_gfx_api.h"

/**************************************************************************
 *                           C O N S T A N T S
 **************************************************************************/
/* Resource file type which are available for sp5kResourceFilePlay() */
typedef enum {
	SP5K_RES_TYPE_JPG         = RES_TYPE_JPG,
	SP5K_RES_TYPE_WAV         = RES_TYPE_WAV,
	SP5K_RES_TYPE_MP3         = RES_TYPE_MP3,
	SP5K_RES_TYPE_JPG_PIP     = RES_TYPE_JPG_PIP,
	SP5K_RES_TYPE_JPG_PF      = RES_TYPE_JPG_PF,
	SP5K_RES_TYPE_AVI         = RES_TYPE_AVI,
	SP5K_RES_TYPE_AVI_PIP     = RES_TYPE_AVI_PIP,
	SP5K_RES_TYPE_AVI_PF      = RES_TYPE_AVI_PF,
	SP5K_RES_TYPE_WAV_BUF	  = RES_TYPE_WAV_BUF,
	SP5K_RES_TYPE_EXTEND      = RES_TYPE_EXTEND,
	SP5K_RES_FILE_TYPE_NUM    = RES_FILE_TYPE_NUM,
} sp5kResFileType_e;

typedef enum {
	SP5K_RES_SUBTYPE_JPG_MAIN = RES_SUBTYPE_JPG_MAIN,
	SP5K_RES_SUBTYPE_JPG_QV   = RES_SUBTYPE_JPG_QV,
	SP5K_RES_SUBTYPE_JPG_THM  = RES_SUBTYPE_JPG_THM,
	SP5K_RES_SUBTYPE_AVI_THM  = RES_SUBTYPE_AVI_THM,
} sp5kResFileSubType_e;

/* Resource file type which are available for sp5kResourceFileLoad() */
typedef enum {
	/* 3A/sensor */
	SP5K_RES_EXPOUSRE = 0x00,
	SP5K_RES_AGC,
	SP5K_RES_AE_WINDOW,
	SP5K_RES_AWB_CTT,
	SP5K_RES_AWB_WINDOW,

	/* MISC */
	SP5K_RES_OSD_FONT,
	SP5K_RES_OSD_STRING,
	SP5K_RES_DATE_STAMP_OSD,

 	SP5K_RES_OSD_ICON,
	SP5K_RES_OSD_ICON_2,
	SP5K_RES_OSD_ICON_3,
	SP5K_RES_OSD_ICON_4,
	SP5K_RES_OSD_ICON_5,

	SP5K_RES_DISP_COLOR_MATRIX,
	SP5K_RES_DISP_GAMMA,

	SP5K_RES_USB_STRING,
	SP5K_RES_CDSP_PARAM,

	SP5K_RES_NEXT_GAMMA,        /* 5330 */
	SP5K_RES_USB_CONF_BIN,

	/* IQ */
	SP5K_RES_LSC,
	SP5K_RES_CA,
	SP5K_RES_ARD,
	SP5K_RES_ARD_CORNER,
	SP5K_RES_R_GAMMA,
	SP5K_RES_G_GAMMA,
	SP5K_RES_B_GAMMA,

	SP5K_RES_3D_LUT,

	SP5K_RES_BP_VIEW,
	SP5K_RES_BP_CAPTURE,

	SP5K_RES_PURPLE_FRINGE,
	SP5K_RES_HUE,
	SP5K_RES_Y_GAMMA,
	SP5K_RES_WDR,

	SP5K_RES_AA_Y,
	SP5K_RES_AA_UV,

	SP5K_RES_AHD_AA,

	SP5K_RES_AHD_CORNER_THR_Y,
	SP5K_RES_AHD_CORNER_THR_UV,

	SP5K_RES_YNF,

	SP5K_RES_Y_EDGE_GAIN,
	SP5K_RES_Y_EDGE_OSC,
	SP5K_RES_GFX_MAP_MORPHING,

	SP5K_RES_Y_CORNER_EDGE,

	SP5K_RES_LDC,

	/* cdsp redo */
	SP5K_RES_REDO_PURPLE_FRINGE,
	SP5K_RES_REDO_HUE,
	SP5K_RES_REDO_Y_GAMMA,
	SP5K_RES_REDO_WDR,

	SP5K_RES_REDO_AA_Y,
	SP5K_RES_REDO_AA_UV,

	SP5K_RES_REDO_AHD_AA,

	SP5K_RES_REDO_AHD_CORNER_THR_Y,
	SP5K_RES_REDO_AHD_CORNER_THR_UV,

	SP5K_RES_REDO_YNF,

	SP5K_RES_REDO_Y_EDGE_GAIN,
	SP5K_RES_REDO_Y_EDGE_OSC,

	SP5K_RES_REDO_Y_CORNER_EDGE,

	/* cdsp sdo */
	SP5K_RES_SDO_PURPLE_FRINGE,
	SP5K_RES_SDO_HUE,
	SP5K_RES_SDO_Y_GAMMA,
	SP5K_RES_SDO_WDR,

	SP5K_RES_SDO_AA_Y,
	SP5K_RES_SDO_AA_UV,

	SP5K_RES_SDO_YNF,

	SP5K_RES_SDO_Y_EDGE_GAIN,
	SP5K_RES_SDO_Y_EDGE_OSC,

	SP5K_RES_SDO_Y_CORNER_EDGE,

	/* cdsp s2do */
	SP5K_RES_S2DO_PURPLE_FRINGE,
	SP5K_RES_S2DO_HUE,
	SP5K_RES_S2DO_Y_GAMMA,
	SP5K_RES_S2DO_WDR,

	SP5K_RES_S2DO_AA_Y,
	SP5K_RES_S2DO_AA_UV,

	SP5K_RES_S2DO_YNF,

	SP5K_RES_S2DO_Y_EDGE_GAIN,
	SP5K_RES_S2DO_Y_EDGE_OSC,

	SP5K_RES_S2DO_Y_CORNER_EDGE,
	
	SP5K_RES_OSD_ICON_6,
	SP5K_RES_OSD_ICON_7,
	SP5K_RES_OSD_ICON_8,
	SP5K_RES_OSD_ICON_9,
	SP5K_RES_OSD_ICON_10,
 	SP5K_RES_OSD_ICON_11,
	SP5K_RES_OSD_ICON_12,
	SP5K_RES_OSD_ICON_13,
	SP5K_RES_OSD_ICON_14,
	SP5K_RES_OSD_ICON_15,
	SP5K_RES_OSD_ICON_16,
	
	SP5K_RES_PTPIP_STRING,
	
	SP5K_RES_DIGI_EFF,	
	SP5K_RES_ALC,
	SP5k_RES_EIS_PARAM_TABLE,
	SP5K_RES_LOAD_TYPE_MAX,  /* new added item please add before this one. */
} sp5kRsvBlkResLoadType_e;

#define SP5K_RES_AUTO	(-1)

typedef enum {
	SP5K_RES_PRELOAD_NUM = 0x00000000,
	SP5K_RES_CFG_THUMB_ROTATE_BASE,
	SP5K_RES_CFG_QV_ROTATE_BASE,
	SP5K_RES_CFG_MAIN_ROTATE_BASE,
	SP5K_RES_CFG_THUMB_DRAW_WITH_OUTPUT_SIZE_FIT,
	SP5K_RES_CFG_QV_3_2_PATCH = 0x80000000,
} sp5kRsvPreloadCfgSelector_e;

typedef enum {
	SP5K_RES_PRELOAD_THUMB_ROTATE_BY_THUMB = 0x00,
	SP5K_RES_PRELOAD_THUMB_ROTATE_BY_PRIMARY,
	SP5K_RES_PRELOAD_THUMB_ROTATE_BY_NONE,
} sp5kRsvBlkThmRotateBase_t;

typedef enum {
	SP5K_RES_PRELOAD_QV_ROTATE_NONE, /* no rotation */
	SP5K_RES_PRELOAD_QV_ROTATE_BY_EXIF, /* rotate by exif (main or thumbnail) */
} sp5kRsvBlkQvRotateBase_t;

typedef enum {
	SP5K_RES_PRELOAD_MAIN_ROTATE_NONE,
	SP5K_RES_PRELOAD_MAIN_ROTATE_BY_PRIMARY,
} sp5kRsvBlkMainRotateBase_t;

typedef struct {
	void * pbuf;
	UINT32 size;
} sp5kRsvPreloadMemObj_t;

typedef enum {
	SP5K_RSV_PRELOAD_THUMB_MAXSIZE = 0x00,
	SP5K_RSV_PRELOAD_QV_MAXSIZE,
	SP5K_RSV_PRELOAD_MAXSIZE_NONE,
} sp5kRsvPreloadMaxSize_t;

#define SP5K_RES_DRAM_PLAY	( 0x00 )
#define SP5K_RES_DRAM_ABORT ( 0x01 )

#define SP5K_RES_FILE_OVERFLOW 0x050A0002 /*exif file overflow */


#define sp5kRawSegmentPreCb_t	pfDiskPreCb_t
#define sp5kRsvImageRotate_t	rsvImageRotate_t
#define sp5kRsvImageRoi_t		rsvImageRoi_t

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
UINT32 sp5kUDFRead(void);
UINT32 sp5kADFRead(void);
UINT32 sp5kUDFWrite(void);
UINT32 sp5kADFWrite(void);
UINT32 sp5kUDFParamGet(UINT32 offset,UINT32 length,UINT16 *pdata);
UINT32 sp5kUDFParamSet(UINT32 offset,UINT32 length,UINT16 *pdata);
UINT32 sp5kUserResFileAdd(UINT32 resType,UINT32 resIndex,char *pfilename);
UINT32 sp5kUserResFileDelete(UINT8 *pfilename);
UINT32 sp5kUserResFileCountGet(UINT32 resType,UINT32 *pcount);
/**
 * \note
 * ctrl:  SP5K_RES_DRAM_PLAY or SP5K_RES_DRAM_ABORT
 * pfilename: input WAV filename
 * phandle: output handle for PLAY and input handle for ABORT
 */
UINT32 sp5kResWavPlay(UINT32 ctrl, const char *pfilename, UINT32 *phandle);
UINT32 sp5kResourceFilePlay(UINT32 resType,const char *pfilename,UINT32 param[4]);
/**\note
 * opt: support SP5K_RES_BP_VIEW & SP5K_RES_BP_CAP only
 * resName: sunplus resource file
 * zRes: original res file
 * zResType: size of bp in byte, support 2 & 4 only
 */
UINT32 sp5kResourceFileLoadExtZ(UINT32 opt, char *resName, char *zRes, UINT32 zResType);
/**
 * \note
 * param[0]: offset x in the selected page layer, must be multiple of 2
 * param[1]: offset y in the selected page layer
 * param[2]: roi width in the selected page layer, must be multiple of 2
 * param[3]: roi height in the selected page layer
 */
UINT32 sp5kResourceFilePagePlay(UINT32 resType,const char *pfilename,UINT32 pageId,UINT32 param[4], UINT16 bgColor);
/**
 * \note
 * srcParam[0]: offset x in the input file, must be multiple of 2
 * srcParam[1]: offset y in the input file
 * srcParam[2]: roi width in the input file, must be multiple of 2
 * srcParam[3]: roi height in the input file
 * dstParam[0]: offset x in the selected page layer, must be multiple of 2
 * dstParam[1]: offset y in the selected page layer
 * dstParam[2]: roi width in the selected page layer, must be multiple of 2
 * dstParam[3]: roi height in the selected page layer
 */
UINT32 sp5kResourceFileRoiPlay(UINT32 resType,const char *filename,UINT32 srcParam[4],UINT32 pageId,UINT32 dstParam[4], UINT16 bgColor);
/**
 * \note
 * pyuvBuf: input YUV buffer
 * bufW   : width of YUV buffer, must be multiple of 16
 * bufH   : height of YUV buffer, must be multiple of 8
 * jpgW   : width of output JPG, must be multiple of 16
 * jpgH   : height of output JPG, must be multiple of 8
 * pfname : filename of output JPG
 */
UINT32 sp5kResourceJpgSave(UINT8 *pyuvBuf,UINT32 bufW,UINT32 bufH,UINT32 jpgW,UINT32 jpgH,const char *pfname);
UINT32 sp5kResourceFileLoad(UINT32 option,const char *pfilename);
UINT32 sp5kResFileDebugLoad(UINT32 option);
UINT32 sp5kResourceFileLoadWithHdrId(const char *pfilename);
UINT32 sp5kResourceBufSizeSet(UINT32 option, UINT32 size);
UINT32 sp5kResDRAMPreLoad( UINT32 resType, const char *pfilename, UINT32 param1, UINT32 param2, UINT32 param3, UINT32 param4 );
UINT32 sp5kResDRAMControl( UINT32 control, UINT32 resHandle, UINT32 param1, UINT32 param2, UINT32 param3, UINT32 param4 );
UINT32 sp5kResDRAMUnload( UINT32 resHandle );
UINT32 sp5kRsvBlkFormat(UINT32 driver);
UINT32 sp5kRawSegmentWrite(UINT32 offset, UINT32 num, UINT8 *pbuf);
UINT32 sp5kRawSegmentRead(UINT32 offset, UINT32 num, UINT8 *pbuf);
void sp5kRawSegmentPreCbSet(sp5kRawSegmentPreCb_t pfunc);

UINT32 sp5kRsvPreloadCfg(UINT32 num);
UINT32 sp5kRsvPreloadDrawCfgSet(UINT32 selector, UINT32 value);
/**
 * \note
 * width: output width in the preload buffer, must be multiple of 8
 * height: output height in the preload buffer
 */
UINT32 sp5kRsvPreloadFile(UINT32 fileType, char *pfileName, UINT32 loadId, UINT32 width, UINT32 height, UINT16 bgColor);
UINT32 sp5kRsvPreloadGfxObjGet(UINT32 loadId, sp5kGfxObj_t *pgfxobj);
UINT32 sp5kRsvPreloadInfoGet(UINT32 loadId, char *pfilename, UINT32 *pfileType, UINT32 *pwidth, UINT32 *pheight, UINT32 *plinebyte);
/**
 * \note
 * param[0]: offset x in the selected page layer, must be multiple of 2
 * param[1]: offset y in the selected page layer
 * param[2]: roi width in the selected page layer, must be multiple of 2
 * param[3]: roi height in the selected page layer
 */
UINT32 sp5kRsvPreloadDraw(UINT32 loadId, UINT32 pageId, UINT32 param[4], UINT16 bgColor);
/**
 * \note
 * param[0]: offset x in the selected page layer, must be multiple of 2
 * param[1]: offset y in the selected page layer
 * param[2]: roi width in the selected page layer, must be multiple of 2
 * param[3]: roi height in the selected page layer
 * rotate: rotation
 */
UINT32 sp5kRsvPreloadDrawRotate(UINT32 loadId, UINT32 pageId, UINT32 param[4], UINT16 bgColor, sp5kRsvImageRotate_t rotate);
/**
 * \note
 * srcParam[0]: offset x in the input file, must be multiple of 2
 * srcParam[1]: offset y in the input file
 * srcParam[2]: roi width in the input file, must be multiple of 2
 * srcParam[3]: roi height in the input file
 * dstParam[0]: offset x in the selected page layer, must be multiple of 2
 * dstParam[1]: offset y in the selected page layer
 * dstParam[2]: roi width in the selected page layer, must be multiple of 2
 * dstParam[3]: roi height in the selected page layer
 */
UINT32 sp5kRsvPreloadDrawRoi(UINT32 loadId, UINT32 srcParam[4], UINT32 pageId, UINT32 dstParam[4], UINT16 bgColor);
/**
 * \note
 * srcParam[0]: offset x in the input file, must be multiple of 2
 * srcParam[1]: offset y in the input file
 * srcParam[2]: roi width in the input file, must be multiple of 2
 * srcParam[3]: roi height in the input file
 * dstParam[0]: offset x in the selected page layer, must be multiple of 2
 * dstParam[1]: offset y in the selected page layer
 * dstParam[2]: roi width in the selected page layer, must be multiple of 2
 * dstParam[3]: roi height in the selected page layer
 * rotate: rotation
 */
UINT32 sp5kRsvPreloadDrawRoiRotate(UINT32 loadId, UINT32 srcParam[4], UINT32 pageId, UINT32 dstParam[4], UINT16 bgColor,
								   sp5kRsvImageRotate_t rotate);
UINT32 sp5kRsvPreloadRelease(void);
#if 0/* obsolete it */
UINT32 sp5kResPhotoframeAlphaMskBlend(sp5kGfxObj_t *pPhotoObj, sp5kGfxObj_t *pImageObj);
#endif
UINT32 sp5kResPhotoframeAlphaMskBlendRoi(sp5kGfxObj_t *pPhotoObj, sp5kGfxObj_t *pImageObj);

#if 0
/**
 * \note    save the roi of the input YUV buffer to a JPG file with specified size and filename
 * pyuvBuf  : input YUV buffer
 * width    : width of input YUV buffer
 * height   : height of input YUV buffer
 * proi     : pointer to the roi info of the input YUV buffer
 * jpegW    : width of output JPG file
 * jpegH    : height of output JPG file
 * pfname   : pointer to output filename, use dcf name rule if NULL
 */
UINT32 sp5kRsvYuvJpegSave(UINT8 *pyuvBuf, UINT32 width, UINT32 height, sp5kRsvImageRoi_t *proi, UINT32 jpegW, UINT32 jpegH, const char *pfname);
#endif
 UINT32 sp5kRsvPreLoadReadMaxSet( UINT32 preloadType, UINT32 size);
UINT32 resSysgIdGet( UINT32 res5kId );

#endif /* _SP5K_RSVBLK_API_H_ */

