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
#ifndef APP_PLAYBACK_DEF_H
#define APP_PLAYBACK_DEF_H

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
#include "sp5k_global_api.h"
#include "sp5k_dcf_api.h"
#include "sp5k_exif_api.h"
#include "sp5k_disk_api.h"
#include "sp5k_gfx_api.h"
#include "sp5k_pb_api.h"
#include "sp5k_slide_api.h"
#include "sp5k_rsvblk_api.h"
#include "sp5k_media_api.h"
#include "sp5k_dzoom_api.h"
#include "gpio_custom.h"
#include "app_dcf.h"
#include "app_disk.h"
#include "app_playback_uiparam.h"
#include "app_exif.h"
#include "app_jpg_draw.h"
#include "app_res_def.h"
#include "app_error_msg.h"
#include "app_cdfs_api.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define PB_DCF_FILENAME_LEN_MAX		32

#define PB_SILENT_SLIDE_SHOW		0 /* determine if music support for slide show */
#define IS_PB_MUSIC_SLIDE			(!PB_SILENT_SLIDE_SHOW)

#define PB_DISK_HOT_SWAP			0 /* CARD hotplug and UI can select DRIVE
									   * Not fully tested, NO GUARANTEED !!
									   */

#define PB_OP_ASYNC			(1)
#define PB_OP_SYNC			(0)

#define PB_VIDEO_CRITICAL_TIME	2000	/* ms, the prohibited time interval between two successive operation */
#define PB_AUDIO_CRITICAL_TIME	1100	/* ms, the prohibited time interval between two successive operation */

/*-- Histogram */
#define PB_HISTOGRAM_MAX_VALUE	256
#define PB_HISTOGRAM_BAR_NUM	128
#define PB_HISTOGRAM_OSD_HEIGHT	60


#define PB_ERR_SHOW_FOREVER		(ERR_MSG_SHOW_FOREVER/*+ERR_MSG_SHOW_1SEC*/)

#define PLAYBACK_ROTATE_BY_EXIF_TAG 1


/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/*-- APP Backdoor events for PLAYBACK mode
 */
typedef enum {
	PB_APP_EVENT_DISK_FORMAT,
	PB_APP_EVENT_SETUP_RESET,
} pbAppBackdoorEvent_t;

/*-- Storage ID */
typedef enum {
	PB_DISK_NAND = SP5K_DRIVE_NAND,
	PB_DISK_SD	 = SP5K_DRIVE_SD,
	PB_DISK_USB	 = SP5K_DRIVE_USB,
} pbDiskIdDef_t;
#define PB_DISK_CARD	(PB_DISK_SD)

/*-- Playback Disk information
 */
typedef struct {
	appDiskInfo_t	dskInfo;

	BOOL			isDiskReady;
	BOOL			isDiskErr;
	BOOL			isCardLock;
	BOOL			isDiskFull;

	BOOL			isDcfFull;
	BOOL			isCardMounted;
	BOOL			isNandMounted;
	BOOL			r1;
} pbDiskCB_t;

/*-- image aspect ratio definition */
typedef enum {
	PB_IMG_ORIENT_UNKNOWN = ((UINT8)0x00),
	PB_IMG_ORIENT_4X3 = ((UINT8)0x01),
	PB_IMG_ORIENT_3X4 = ((UINT8)0x02),
	PB_IMG_ORIENT_3X2 = ((UINT8)0x04),
	PB_IMG_ORIENT_2X3 = ((UINT8)0x08),
	PB_IMG_ORIENT_16X9 = ((UINT8)0x10),
	PB_IMG_ORIENT_9X16 = ((UINT8)0x20),
	PB_IMG_ORIENT_HORIZONTAL = ((UINT8)(PB_IMG_ORIENT_4X3 | PB_IMG_ORIENT_3X2 | PB_IMG_ORIENT_16X9)),
	PB_IMG_ORIENT_VERTICAL = ((UINT8)(PB_IMG_ORIENT_3X4 | PB_IMG_ORIENT_2X3 | PB_IMG_ORIENT_9X16)),
	PB_IMG_ORIENT_16V9 = ((UINT8)(PB_IMG_ORIENT_16X9 | PB_IMG_ORIENT_9X16)),
	PB_IMG_ORIENT_3V2 = ((UINT8)(PB_IMG_ORIENT_3X2 | PB_IMG_ORIENT_2X3)),
	PB_IMG_ORIENT_4V3 = ((UINT8)(PB_IMG_ORIENT_4X3 | PB_IMG_ORIENT_3X4)),
} pbImageOrientDim_t;

/*-- Playback Image Size Grade
 */
typedef enum
{
	PB_IMG_SIZE_GRADE_0,/* VGA*/
	PB_IMG_SIZE_GRADE_1,/* 1M*/
	PB_IMG_SIZE_GRADE_2,/* 2M*/
	PB_IMG_SIZE_GRADE_3,/* 3M*/
	PB_IMG_SIZE_GRADE_4,/* 4M*/
	PB_IMG_SIZE_GRADE_5,/* 5M*/
	PB_IMG_SIZE_GRADE_6,/* 6M*/
	PB_IMG_SIZE_GRADE_7,/* 7M*/
	PB_IMG_SIZE_GRADE_8,/* 8M*/
	PB_IMG_SIZE_GRADE_9,/* 9M*/
	PB_IMG_SIZE_GRADE_10,/* 10M*/
	PB_IMG_SIZE_GRADE_12,/* 12M*/
	PB_IMG_SIZE_GRADE_14,/* 14M*/
	PB_IMG_SIZE_GRADE_16,/* 16M*/
	PB_IMG_SIZE_GRADE_20,/* 20M*/
	PB_IMG_SIZE_GRADE_TOTAL,/* TOTAL*/
	PB_IMG_SIZE_GRADE_UNKNOWN,	/* UNKNOWN*/

} pbImageSizeGrade_t;

/*-- Playback Video Size Grade
 */
typedef enum {
	PB_VIDEO_SIZE_GRADE_1,	/* 16X9*/
	PB_VIDEO_SIZE_GRADE_2,	/* 640*/
	PB_VIDEO_SIZE_GRADE_3,	/* 320*/
	PB_VIDEO_SIZE_GRADE_UNKNOWN,
	PB_VIDEO_SIZE_GRADE_TOTAL,
} pbVideoSizeGrade_t;


/*-- Playback IMG EXIF Information :
 */
typedef struct pbImgExifInfo_s{
	UINT32 imgWidth;
	UINT32 imgHeight;
	UINT32 expTime[2];
	UINT32 fNum[2];
	UINT16 iso;
	UINT16 flash;
	UINT8  szDate[APP_EXIF_DATE_MAX_LEN];
	SINT32 expBias[2];
	UINT16 wbMode;
	UINT16 lightSource;
	UINT16 meteringMode;
	UINT16 sceneMode;
	UINT16 expProgMode;
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	UINT8 latitudeRef[2];
	UINT8 longitudeRef[2];
	UINT8 latitude[32];		/* Latitude info format: ddmm.mmmm*/
	UINT8 longitude[32];	/* Longitude format:dddmm.mmmm*/
	UINT8 speed[8];	/* speed format:xxx.yy*/
	UINT8 direction[8];	/* speed format:xxx*/
#endif
} pbImgExifInfo_t, *PpbImgExigInfo_t;

/*-- Playback primary control block
 */
 typedef struct {
	UINT16		mainStat;
	UINT16		subStat;

	UINT32		err;
	UINT32		asyncOpTmrId;	/* the timer ID of TIME-OUT check for async Operation, such as IMGEDIT_SAVE */
	UINT32		closeLensTmrId; /* the timer ID to close LENS */

	UINT8		playStat;
	UINT8		osdType;	/* the display mode of UI OSD */
	UINT8		postponeUI;		/* 0:Don_Postpone, 1:Postpone_FAST-DRAW; 2:Postpone_NORMAL-DRAW */

	BOOL		isDelHotKey;	/* entering DLE state from HotKey */
	BOOL		isPlayDone;
	BOOL		isNoImage;
	BOOL		isIniting;		/* the period from stateInit to MSG: PB_INIT */

	UINT32 		totalImg;
	UINT32		curImgIdx;
	UINT32		lastImgIdx;	/* backup currImgIdx while suspending PB_MAIN state */
	UINT8		lastFileType;
 } pbMainCB_t;


/*-- Current Image control block */
typedef struct {
	UINT32 				dcfIdx;		/* Image DCF Index */
	sp5kDcfAttrElm_t	dcfAttr;
	UINT8				szDcfFileName[PB_DCF_FILENAME_LEN_MAX];
	UINT32 				imgWidth;		/* Image Width */
	UINT32 				imgHeight;		/* Image Height */
	UINT32				imgYuvFmt;
	UINT32				mediaTotalTime; /* audio/video total time, in ms */
	UINT32				mediaElapTime;	/* audio/video current play time, in ms */

	UINT8		mediaPlaySpeed;	/* speed control of FF & Rewind */

	UINT8		sp5kImgType;	/* the IMG TYPE to be playback after image checkup */
	UINT8		exifImgFlag;	/* the flag indicating existing of MAIN/QV/THUMB */
	UINT8		rotOrient;		/* current orientation while rotate */
	UINT8 		sizeGrade;		/* indicating the size grade of image while doing resize */
	UINT8		fileType;
	pbImageOrientDim_t 	imgOrientDim;  /* indicating dimension type (4x3/3x2/16x9) and orientation */

	BOOL isMainImgOK;		/* Main IMG playback succeeded */
	BOOL isThumbImgOK;		/* Thumb/QV IMG playback succeeded */
	BOOL isFileErr;
	BOOL isProtected;

	BOOL isDpofSet;
	UINT16 dpofPrintNum;    /* DPOF Print Number */
	BOOL isDpofStamp;		/* DPOF Date Stamp */

	UINT32 *pHistBuf;	/* Histogram Buf ptr */
	pbImgExifInfo_t *pExifInfo;	/* ptr to EXIF Info */

#if 0 /* the following shall be rename prior being used !! */
UINT32 expTime[2];			/*  expTime = wExpTime[0] / wExpTime[1] */
UINT32 fNum[2];			/* F-Num = wFNum[0] / wFNum[1] */
UINT16 isoVal;          // ISO Value
UINT16 nAVIFrameRate;   // If AVI File, frame rate=15, 30... fps
UINT16 dpofPrintNum;    // DPOF Print Number
BOOL isExistingDpof;        // If this Standard,Size or Index Ixist DPOF Index value is TRUE else FALSE.
BOOL isDcfCompatible;        // Is Image DCF Compatible
UINT32 ulMPEG4BitRate;    // For MPEG4,indicate BitRate
#endif
} pbImgCb_t;

/*-- Playback ERROR code :
 * The error code is declared as UINT32.
 * It reserve the posibility to extend the encode format if need.
 * Ex, [31~24]: PB_SUB_STAT, [23~16]:Primary Error, [15~0]:Auxilary error infor
 */
typedef enum {
	PB_ERR_NONE,
	PB_ERR_NAND_NOT_READY,
	PB_ERR_CARD_NOT_READY,
	PB_ERR_DISK_NOT_READY,
	PB_ERR_CARD_ERROR,
	PB_ERR_NAND_ERROR,
	PB_ERR_DISK_ERROR,
	PB_ERR_CARD_LOCKED,
	PB_ERR_DISK_FULL,
	PB_ERR_DCF_KEY_FULL,
	PB_ERR_FILE_ERROR,
	PB_ERR_FILETYPE_ERROR,
	PB_ERR_IMG_PROTECTED,
	PB_ERR_NO_IMAGE,
	PB_ERR_NO_IMAGE_SPLASH,
	PB_ERR_COPY_ERROR,
	PB_ERR_PLAYING_BUSY,
	PB_ERR_MEDIA_ERROR,
	PB_ERR_DEL_ERROR,
	PB_ERR_C2C_ERROR,
	PB_ERR_PROT_ERROR,
	PB_ERR_RESIZE_ERROR,
	PB_ERR_EFFECT_ERROR,
	PB_ERR_ROTATE_ERROR,
	PB_ERR_SLIDE_ERROR,
	PB_ERR_VIDEO_ERROR,
	PB_ERR_AUDIO_ERROR,
	PB_ERR_PHOTOFRAME_ERROR,
	PB_ERR_MEMOREC_ERROR,
	PB_ERR_SAVE_IMG_ERROR,
	PB_ERR_START_IMG_ERROR,
} pbErrorCode_t;

/*-- Playback state info */
typedef enum {
	PB_MAIN_STAT_NULL,
	PB_MAIN_STAT_STANDBY,
	PB_MAIN_STAT_VIDEO_SUSPEND,
	PB_MAIN_STAT_AUDIO_SUSPEND,
	PB_MAIN_STAT_MENU_SUSPEND,
	PB_MAIN_STAT_ZPAN_SUSPEND,
	PB_MAIN_STAT_MPLAY_SUSPEND,
	PB_MAIN_STAT_IMG_PROT_SUSPEND,
	PB_MAIN_STAT_DPOF_SUSPEND,
	PB_MAIN_STAT_C2C_SUSPEND,
	PB_MAIN_STAT_MEMOREC_SUSPEND,
	PB_MAIN_STAT_IMG_DEL_SUSPEND,
	PB_MAIN_STAT_ROTATE_SUSPEND,
	PB_MAIN_STAT_SLIDE_SUSPEND,
	PB_MAIN_STAT_START_IMG_SUSPEND,
	PB_MAIN_STAT_JPG_RESIZE,
	PB_MAIN_STAT_JPG_EFFECT,
	PB_MAIN_STAT_JPG_ROTATE,
	PB_MAIN_STAT_JPG_PHOTOFRAME,
#if SP5K_CDFS_OPT
    PB_MAIN_STAT_CDFS_IMAGE,
    PB_MAIN_STAT_CDFS_VIDEO,
#endif	
	PB_MAIN_STAT_UNKNOWN,
} pbStateDef_t;

/*-- Image Playback State */
typedef enum {
	PB_PLAY_STAT_STANDBY,
	PB_PLAY_STAT_JPG_QV_PLAYING,
	PB_PLAY_STAT_JPG_MAIN_PLAYING,
	PB_PLAY_STAT_JPG_THUMB_PLAYING,
	PB_PLAY_STAT_VIDEO_TITLE_PLAYING,
	PB_PLAY_STAT_AUDIO_TITLE_PLAYING,
	PB_PLAY_STAT_VIDEO_TRIM_P2P,
	PB_PLAY_STAT_VIDEO_TRIM_SEC,
} pbPlayStatDef_t;

/*-- Playback SUB State PRIME
 */
typedef enum {
	PB_SUB_STAT_PRIME_NULL,
	PB_SUB_STAT_PRIME_ZPAN,
	PB_SUB_STAT_PRIME_MPLAY,
	PB_SUB_STAT_PRIME_PROT,
	PB_SUB_STAT_PRIME_DEL,
	PB_SUB_STAT_PRIME_DPOF,
	PB_SUB_STAT_PRIME_C2C,
	PB_SUB_STAT_PRIME_SLIDE,
	PB_SUB_STAT_PRIME_MEMOREC,
	PB_SUB_STAT_PRIME_VIDEO,
	PB_SUB_STAT_PRIME_AUDIO,
	PB_SUB_STAT_PRIME_TOTAL,
} pbSubStatePrimeDef_t;


/*-- Playback Navigation direction */
typedef enum {
	PB_SHIFT_NEXT,
	PB_SHIFT_PRE,
	PB_SHIFT_TOTAL,
} pbImageShiftDir_t;

/*-- Playback Flying Play direction*/
typedef enum {
	PB_MEDIA_FLYING_FF,
	PB_MEDIA_FLYING_REW,
	PB_MEDIA_FLYING_SLOW_MOTION,
} pbMediaFlyingDir_t;

/*-- Zoom-Pan State */
typedef enum {
	PB_ZPAN_STAT_NULL = ((PB_SUB_STAT_PRIME_ZPAN<<8)+0),
	PB_ZPAN_STAT_WAIT_MAIN_IMG,
	PB_ZPAN_STAT_STANDBY,
	PB_ZPAN_STAT_TRIM_CONFIRM_MENU,
	PB_ZPAN_STAT_TRIM_BUSY,
	PB_ZPAN_STAT_MOVING,/*willow ++ for move after zpan*/
} pbZoomPanStatDef_t;

/*-- MPlay State */
typedef enum {
	PB_MPLAY_STAT_NULL = ((PB_SUB_STAT_PRIME_MPLAY<<8)+0),
	PB_MPLAY_STAT_BUSY,
	PB_MPLAY_STAT_EXE_BUSY,
	PB_MPLAY_STAT_STANDBY,
} pbMPlayStatDef_t;

/*-- IMG Protect State */
typedef enum {
	PB_PROT_STAT_NULL = ((PB_SUB_STAT_PRIME_PROT<<8)+0),
	PB_PROT_STAT_CONFIRM_MENU,
	PB_PROT_STAT_ALL_BUSY,
	PB_PROT_STAT_STANDBY,
	PB_PROT_STAT_IMG_SHIFT,
} pbImgProtStatDef_t;

/*-- IMG Protect State */
typedef enum {
	PB_DPOF_STAT_NULL = ((PB_SUB_STAT_PRIME_DPOF<<8)+0),
	PB_DPOF_STAT_CONFIRM_MENU,
	PB_DPOF_STAT_JOB_MENU,
	PB_DPOF_STAT_ALL_BUSY,
	PB_DPOF_STAT_STANDBY,
	PB_DPOF_STAT_IMG_SHIFT,
} pbDpofStatDef_t;

/*-- IMG DEL State */
typedef enum {
	PB_DEL_STAT_NULL = ((PB_SUB_STAT_PRIME_DEL<<8)+0),
	PB_DEL_STAT_ALL_BUSY,
	PB_DEL_STAT_STANDBY,
	PB_DEL_STAT_IMG_SHIFT,
	PB_DEL_STAT_CONFIRM_MENU,
} pbImgDelStatDef_t;

/*-- Playback C2C State */
typedef enum {
	PB_C2C_STAT_NULL = ((PB_SUB_STAT_PRIME_C2C<<8)+0),
	PB_C2C_STAT_ALL_BUSY,
	PB_C2C_STAT_NAND_MOUNT_BUSY,
	PB_C2C_STAT_CONFIRM_MENU,
	PB_C2C_STAT_STANDBY,
	PB_C2C_STAT_IMG_SHIFT,
} pbC2CStatDef_t;

/*-- Playback SlideShow State */
typedef enum {
	PB_SLIDE_STAT_NULL = ((PB_SUB_STAT_PRIME_SLIDE<<8)+0),
	PB_SLIDE_STAT_EFFECT_DONE,
	PB_SLIDE_STAT_EFFECT_SLIDING,
	PB_SLIDE_STAT_IMG_DECODING,
	PB_SLIDE_STAT_TOTAL,
} pbSlideStatDef_t;

/*-- Playback Video State */
typedef enum {
	PB_VIDEO_STAT_STANDBY = ((PB_SUB_STAT_PRIME_VIDEO<<8)+0),
	PB_VIDEO_STAT_PLAY_STARTING,
	PB_VIDEO_STAT_PLAYING,
	PB_VIDEO_STAT_PAUSE,
	PB_VIDEO_STAT_FF,
	PB_VIDEO_STAT_REW,
	PB_VIDEO_STAT_SLOW_MOTION,	
	PB_VIDEO_STAT_TOTAL,
} pbVideoStatDef_t;

/*-- Playback Audio State */
typedef enum {
	PB_AUDIO_STAT_STANDBY = ((PB_SUB_STAT_PRIME_AUDIO<<8)+0),
	PB_AUDIO_STAT_PLAY_STARTING,
	PB_AUDIO_STAT_PLAYING,
	PB_AUDIO_STAT_PAUSE,
	PB_AUDIO_STAT_FF,
	PB_AUDIO_STAT_REW,
	PB_AUDIO_STAT_TOTAL,
} pbAudioStatDef_t;

/*-- Playback AMemoRec State */
typedef enum {
	PB_MEMOREC_STAT_STANDBY = ((PB_SUB_STAT_PRIME_MEMOREC<<8)+0),
	PB_MEMOREC_STAT_STARTING,
	PB_MEMOREC_STAT_RECORDING,
	PB_MEMOREC_STAT_PAUSE,
	PB_MEMOREC_STAT_DEL_CONFIRM_MENU,
	PB_MEMOREC_STAT_TOTAL,
} pbMemoRecStatDef_t;

/*-- Playback ROTATE Cmd */
typedef enum {
	PB_ROT_CW,
	PB_ROT_CCW,
	PB_ROT_H_FLIP,
	PB_ROT_V_FLIP,
	PB_ROT_SAVE,
	PB_ROT_CANCEL,
} pbRotCmdDef_t;

/*-- START IMG State */
typedef enum {
	PB_SIMG_STAT_NULL,
	PB_SIMG_STAT_CONFIRM_MENU,
	PB_SIMG_STAT_BUSY,
	PB_SIMG_STAT_IMG_SHIFT,
	PB_SIMG_STAT_STANDBY,
} pbStartImgStatDef_t;

/*--IMG_EDIT State */
typedef enum {
	PB_ROTATE_STAT_NULL,
	PB_ROTATE_STAT_CONFIRM_MENU,
	PB_ROTATE_STAT_BUSY,
	PB_ROTATE_STAT_IMG_SHIFT,
	PB_ROTATE_STAT_STANDBY,
} pbImgRotateStatDef_t;


/*-- IMG_EDIT Cmd */
typedef enum {
	PB_EDIT_CMD_NONE,
	PB_EDIT_CMD_ROTATE,
} pbImgEditCmdDef_t;

/*-- Current Orientation of IMG */
typedef enum {
	/*-- !! Dont change the order */
	PB_ROT_ORIENT_0		= 0,
	PB_ROT_ORIENT_90	= 1,
	PB_ROT_ORIENT_180	= 2,
	PB_ROT_ORIENT_270	= 3,
} pbRotateOrient_t;

/*-- IMG SAVE Option */
typedef enum {
	PB_SAVE_NEW,
	PB_SAVE_REPLACE,
} pbSaveFileOpt_t;

/*-- Media Play Speed */
typedef enum {
	PB_PLAY_SPEED_NORMAL,
	PB_PLAY_SPEED_ACCELERAT_1,
	PB_PLAY_SPEED_ACCELERAT_MAX,
} pbMediaPlaySpeed_t;

/*-- Playback DONE Msg Parse Result */
typedef enum {
	/*-- DO NOT CHANGE SEQUENCE !! */
	/*---- ERROR ----*/
	PB_DONE_UNKNOWN_ERROR 	= (1<<0),
	PB_DONE_THUMB_ERROR		= (1<<1),
	PB_DONE_MAIN_ERROR		= (1<<2),
	PB_DONE_QV_ERROR		= (1<<3),
	PB_DONE_ERROR			= (PB_DONE_UNKNOWN_ERROR|PB_DONE_THUMB_ERROR|PB_DONE_MAIN_ERROR|PB_DONE_QV_ERROR),
	/*---- ABORT ----*/
	PB_DONE_THUMB_ABORT		= (1<<4),
	PB_DONE_MAIN_ABORT		= (1<<5),
	PB_DONE_QV_ABORT		= (1<<6),
	/*---- FINISH ----*/
	PB_DONE_THUMB_FINISH	= (1<<7),
	PB_DONE_MAIN_FINISH		= (1<<8),
	PB_DONE_QV_FINISH		= (1<<9),
} pbDoneMsgParseResult_t;

typedef enum {
	PB_DISP_BASIC			= (unsigned char) 0x00,
	PB_DISP_FULL			= (unsigned char) 0x01,
	PB_DISP_NONE			= (unsigned char) 0x02,
	PB_DISP_TOTAL			= (unsigned char) 0x03,
} PB_DISP_t;

/*-- Playback File Type enum
 */
typedef enum {
	PB_FT_UNKNOWN,
	PB_FT_JPG,
	PB_FT_JPG_MEMO,
	PB_FT_WAV,
	PB_FT_AVI,
	PB_FT_MOV,
	PB_FT_MP4,
	PB_FT_TOTAL,
} pbFileTypeDef_t;


/*==================================
 */
typedef enum {
	PB_MEDIA_UI_STAT_STANDBY,
	PB_MEDIA_UI_STAT_PLAYING,
	PB_MEDIA_UI_STAT_RECORDING,
	PB_MEDIA_UI_STAT_PAUSE,
	PB_MEDIA_UI_STAT_FF,
	PB_MEDIA_UI_STAT_REW,
	PB_MEDIA_UI_STAT_SLOWMOTION,
	PB_MEDIA_UI_STAT_TOTAL,
} pbMediaUIStateDef_t;


/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/

#define gtotalImgCount 		gpbMainCb.totalImg
#define gcurrDcfIdx			gpbMainCb.curImgIdx

#define PB_SUB_STAT_PRIME_NUM(subStat)	((((UINT16)(subStat))>>8) & 0x00FF)

/*-- OSD related */
#define PB_OSD_0_CLR	do { sp5kGfxPageClear(SP5K_GFX_PAGE_OSD_0,0); } while(0)
#define PB_OSD_1_CLR	do { sp5kGfxPageClear(SP5K_GFX_PAGE_OSD_0+1,0); } while(0)
#define PB_PF_0_CLR		do { sp5kGfxPageClear(SP5K_GFX_PAGE_PF_0,0); } while(0)
#define PB_PIP_0_CLR	do { sp5kGfxPageClear(SP5K_GFX_PAGE_PIP_0,0); } while(0)
#if !defined(PRJ_17DVR)&&!defined(PRJ_19DVR)
#define PB_BG_SHOW		do { appModeSet(SP5K_MODE_STANDBY); appJpgFileDraw(SP5K_RES_TYPE_JPG, RES_JPG_PB_BG, 0, 0, 1280, 720); DBG_PRINT("PB_BG_SHOW !!\n"); } while(0)
#else
#define PB_BG_SHOW		do {  } while(0)
#endif
#define PB_OSD_CLR		do { CLEAR_OSD_SRC; DBG_PRINT("pb: OSD_CLR !!\n"); } while(0)
#define PB_PF_CLR		do { PB_PF_0_CLR; DBG_PRINT("pb: PF_CLR !!\n"); } while(0)
#define PB_PIP_CLR		do { PB_PIP_0_CLR; DBG_PRINT("pb: PIP_CLR !!\n"); } while(0)
#define PB_BLACK_BKGND	do { appModeSet(SP5K_MODE_STANDBY); appJpgFileDraw(SP5K_RES_TYPE_JPG, RES_JPG_BLACK, 0, 0, 0, 0); } while(0)
#define PB_CLR_ALL		do { PB_OSD_CLR; PB_PIP_CLR; PB_PF_CLR; DBG_PRINT("PB_CLR_ALL !!\n"); } while(0)

/*-- General index management :
 * INC/DEC will do circulation operation, while
 * ADD/SUB wont !!
 */
#define PB_INDEX_INC(idx, firstIdx, lastIdx)	do {idx = ((idx)>=(lastIdx)) ? firstIdx : (idx+1);} while(0)
#define PB_INDEX_DEC(idx, firstIdx, lastIdx)	do {idx = ((idx)<=(firstIdx)) ? lastIdx : (idx-1);} while(0)
#define PB_INDEX_ADD(idx, firstIdx, lastIdx)	do {idx = ((idx)>=(lastIdx)) ? lastIdx : (idx+1);} while(0)
#define PB_INDEX_SUB(idx, firstIdx, lastIdx)	do {idx = ((idx)<=(firstIdx)) ? firstIdx : (idx-1);} while(0)

#define SYS_IS_CARD_LOCK	(0) /* %TODO% */
#define SYS_IS_CARD_READY	(1) /* %TODO% */
#define PB_IS_DISK_ERROR	(!!(IS_DISK_MOUNT_FAIL))
#define PB_IS_DISK_LOCK		(0)
#define PB_IS_DISK_READY	(!!(IS_DISK_MOUNT_READY))

/*-- Filetype test */
#define APP_PB_FILETYPE_VIDEO	(SP5K_DCF_FILETYPE_AVI \
								| SP5K_DCF_FILETYPE_ASF \
								| SP5K_DCF_FILETYPE_MOV \
								| SP5K_DCF_FILETYPE_MP4)

#define APP_PB_FILETYPE_AUDIO	(SP5K_DCF_FILETYPE_WAV)

#define APP_PB_FILETYPE_ALL		(SP5K_DCF_FILETYPE_JPG \
                         		| APP_PB_FILETYPE_AUDIO \
                         		| APP_PB_FILETYPE_VIDEO)
#define IS_FILETYPE_JPG(a)		(!!(a & SP5K_DCF_FILETYPE_JPG))
#define IS_FILETYPE_WAV(a)		(!!(a & SP5K_DCF_FILETYPE_WAV))
#define IS_FILETYPE_MP3(a)		(!!(a & SP5K_DCF_FILETYPE_MP3))
#define IS_FILETYPE_ASF(a)		(!!(a & SP5K_DCF_FILETYPE_ASF))
#define IS_FILETYPE_AVI(a)		(!!(a & SP5K_DCF_FILETYPE_AVI))
#define IS_FILETYPE_MOV(a)		(!!(a & SP5K_DCF_FILETYPE_MOV))
#define IS_FILETYPE_MP4(a)		(!!(a & SP5K_DCF_FILETYPE_MP4))

#define IS_FILETYPE_VIDEO(a)	(IS_FILETYPE_AVI(a) | IS_FILETYPE_ASF(a) | IS_FILETYPE_MOV(a)| IS_FILETYPE_MP4(a))
#define IS_FILETYPE_AUDIO(a)	(IS_FILETYPE_WAV(a))
#define IS_SUPPORTED_FILETYPE(a)		(!!(a & APP_PB_FILETYPE_ALL))
#define IS_SUPPORTED_VIDEO_FILETYPE(a)	(!!(a & APP_PB_FILETYPE_VIDEO))
#define IS_SUPPORTED_AUDIO_FILETYPE(a)	(!!(a & APP_PB_FILETYPE_AUDIO))


/*-- Playback Playing test */
#define IS_PB_MAIN_PLAYING	(gpbMainCb.playStat==PB_PLAY_STAT_JPG_MAIN_PLAYING)
#define IS_PB_QV_PLAYING	(gpbMainCb.playStat==PB_PLAY_STAT_JPG_QV_PLAYING)
#define IS_PB_THUMB_PLAYING	(gpbMainCb.playStat==PB_PLAY_STAT_JPG_THUMB_PLAYING)
#define IS_PB_JPG_PLAYING	(IS_PB_MAIN_PLAYING || IS_PB_QV_PLAYING || IS_PB_THUMB_PLAYING)
#define IS_PB_VIDEO_TITLE_PLAYING (gpbMainCb.playStat==PB_PLAY_STAT_VIDEO_TITLE_PLAYING)
#define IS_PB_AUDIO_TITLE_PLAYING (gpbMainCb.playStat==PB_PLAY_STAT_AUDIO_TITLE_PLAYING)

/*-- IMG test */
#define IS_MAIN_IMG_EXIST(flag)	(flag & SP5K_IMG_MAIN_EXIST)
#define IS_THUMB_IMG_EXIST(flag)(flag & SP5K_IMG_THUMBNAIL_EXIST)
#define IS_QV_IMG_EXIST(flag)	(flag & SP5K_IMG_QV_EXIST)

/*-- KEY test */
#define PB_SHIFT_REPEAT_INTERVAL	150 /* ms */

/*-- Media Critical period tests */
#define IS_MEDIA_ENDING_CRITICAL_TIME(elapT, intvl, totalT) ((elapT)+(intvl) >= (totalT))
#define IS_MEDIA_BEGINNING_CRITICAL_TIME(elapT, intvl)  ((elapT) <= (intvl))

/*-- UI & SOUND */
#define PB_BEEP		do { appBeep(TRUE, FALSE); DBG_PRINT("pb: BEEP\n"); } while(0)

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

extern pbMainCB_t	gpbMainCb;
extern pbImgCb_t	gpbImgCb;
extern pbUIParam_t	gpbUiParam;
extern pbDiskCB_t	gpbDiskCb;
extern pbImgExifInfo_t gpbExifInfo;
extern UINT32 gpbHistBuf[PB_HISTOGRAM_BAR_NUM];
extern UINT16 gpbSizeGrade[11][3][2];
/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/

/*--------------------------------
 * Playback Main Session
 *--------------------------------
 */
BOOL appPbYUVPIPStatusGet(void);
void appPbYUVPIPStatusSet(BOOL val);
BOOL appPb_IsViewIdle(void);
pbImgExifInfo_t *appPb_ExifInfoPtrGet(void);
pbMainCB_t *appPb_MainCbPtrGet(void);
pbUIParam_t *appPb_UiParamPtrGet(void);
pbImgCb_t *appPb_ImageCbPtrGet(void);
pbDiskCB_t *appPb_DiskCbPtrGet(void);
void appPreviousViewStateSet(UINT32 state);
UINT32 appPreviousViewStateGet(void);

/*--------------------------------
 * Playback Edit Session
 *--------------------------------
 */
UINT32 appPbEffectRedEyeProc(UINT32 srcAddr, UINT32 width, UINT32 height);
UINT32 appPbImageCustomizeEffect(void);
UINT32 appPbEffectRedEyeReduction(void);
UINT32 appPbEffectCreate(pbImgCb_t *pimgCb);
UINT32 appPbEffectExecute(pbImgCb_t *pimgCb);
void appPbEffectDestroy(pbImgCb_t *pimgCb);
UINT32 appPbPhotoframeCreate(pbImgCb_t *pimgCb);
void appPbPhotoframeClear(void);
UINT32 appPbPhotoframeExecute(pbImgCb_t *pimgCb, UINT8 frameId);
void appPbPhotoframeDestroy(pbImgCb_t *pimgCb);
UINT32 appPbRotateCreate(pbImgCb_t *pimgCb);
UINT32 appPbRotateExecute(pbImgCb_t *pimgCb, pbRotCmdDef_t rotCmd);
UINT32 appPbRotatePreview(pbRotCmdDef_t rotCmd);
void appPbRotateDestroy(pbImgCb_t *pimgCb);
UINT32 appPbResizeCreate(pbImgCb_t *pimgCb);
void appPbResizeDestroy(pbImgCb_t *pimgCb);
void appPbEffectLomo(UINT32 srcAddr, UINT32 width, UINT32 height);
void appPBEffectLomoFunction(void);


/*--------------------------------
 * Playback Library Session
 *--------------------------------
 */
void appPbPIPInit(void); 
void AppPbDestroyshowJPGInPIP(void);
    
UINT32 appPbTotalFileCountGet(void);
UINT32 appPbJpgMainPlayStart(void);
pbImageOrientDim_t appPbImageOrientDimGet(UINT16 width, UINT16 height);
pbImageSizeGrade_t appPbImageSizeGradeGet(UINT16 width,	UINT16 height, UINT8 ratioType);
UINT32 appPbStillPlayDoneParse(UINT32 param);
void playImgPlayReset(void);
UINT32 playImgPlayStart(void);
UINT32 playImgDcfAttrGet (pbImgCb_t *pImgCb);
UINT32 appPbImagePlay(UINT32 dcfIdx);
UINT32 appPbImageShift_PreCheck(void);
void appPbImageShiftRepeatCheck(UINT32	dumbMs);
UINT32 appPbImageShift(UINT8	shiftDir);
UINT32 appPbJpgImageShift(UINT8	shiftDir, BOOL isSkipFileErr);
UINT32 appPbImageDcfSave(UINT8 saveOpt);
UINT32 appPbRotatePreview(pbRotCmdDef_t rotCmd);
UINT32 appPbDiskMount(pbDiskIdDef_t dskId);
UINT32 appPbActiveDiskSet(pbDiskIdDef_t dskId);
void playMusicDRAMPlayStop(void);
void appPbMsg_Clear(void);
void appPbMsg_BusyWait(void);
void appPbMsg_ImgSaved(void);
void appPbMsg_Completed(void);
void appPbErr_DiskFull(void);
void appPbErr_CardNotExist(void);
void appPbErr_FileLocked(void);
void appPbErr_CardLock(void);
void appPbErrorProcess(UINT32 err);
UINT32 appPbScanNextJPG(pbImageShiftDir_t scanDir, BOOL isSkipFileErr);
UINT32 appPbBytes2ClusterCalculate(UINT32 bytes);
UINT32 appPbDiskFreeClusterGet(UINT32 actDsk);
void appPbDiskCheckup(pbDiskCB_t *pdskCb);
UINT32 appPbEditSaveDiskCheckup(pbImgCb_t *pimgCb);
UINT32 appPbBackdoorEventCBF(pbAppBackdoorEvent_t appEvent);
void appPbImgExifDateTimeGet(UINT8 *pszDTBuf);
UINT32 appPbImgExifInfoGet(pbImgExifInfo_t *pexifInfo);
 UINT32 appPbImgHistogramGet(UINT32	*pLumaBuf, UINT32 barNum, UINT32 maxVal);
UINT32 appPbMediaUIStateGet(void);
UINT32 appPbHistogramGet( UINT32 *pbHistoData, UINT32 stepX, UINT32 stepY) ;
void appIsInPBMode(void);
/*--------------------------------
 * Playback OSD Session
 *--------------------------------
 */
/*-- Refer to app_playback_osd.h */
/*--------------------------------
 * Playback Menu Session
 *--------------------------------
 */
void appPlayMenu_RotateMenu(UINT8 *uiSel);

#endif /* APP_PLAYBACK_DEF_H */



