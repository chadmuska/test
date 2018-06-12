/**************************************************************************
 *
 *       Copyright (c) 2008-2016 by iCatch Technology, Inc.
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
 *  Author: Anmin Deng
 *
 **************************************************************************/
#ifndef _FTRACK_DEF_H_
#define _FTRACK_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include "middleware/modesw_def.h"

/**
 * \brief mode in
 * UINT32 sp5kPatternTrackTargetSet(UINT32 target);
 */
typedef enum {
	PTRACK_TARGET_FACE,
	PTRACK_TARGET_CAR
} ptrackTargetSel_t;


/**
 * \brief mode in
 * UINT32 sp5kPatternTrackModeSet(UINT32 mode);
 */
typedef enum {
	PTRACK_MODE_OFF,
	PTRACK_MODE_PV_ON,
	PTRACK_MODE_PV_SUSPEND,
	PTRACK_MODE_PV_RESUME,
	PTRACK_MODE_PV_DELAY_OFF = 0x80,
} ptrackModeSel_t;


/**< frame buffer addr must align to 16 byte and width must be mutiple of 8 */
typedef modeswFrameBuf_t ptrackFrameBuf_t;


/**
 * \param cfg in
 * UINT32 sp5kPatternTrackCfgSet(UINT32 cfg,UINT32 val);
 */
typedef enum {
	PTRACK_CFG_MIN_PATTERN_SIZE, /*min size of pattern*/
	PTRACK_CFG_Y_BUF_NUM, /*at least 2, max 8*/			
	PTRACK_CFG_YSCALE_BUF_NUM, /*at least 1, max 8*/
	PTRACK_CFG_PROC_PATTERN_BUF_NUM, /*at least 2, max 8*/
	PTRACK_CFG_ACCUM_PERIOD,
	PTRACK_CFG_YUVBUF_BLOCK,
	PTRACK_CFG_TRACK_DONE_CB,
	PTRACK_CFG_LOW_POWER,
	PTRACK_CFG_COPY_FRAME_BUF,
	PTRACK_CFG_PV_SNAP_OFF, /*1:fd close under snap,0:not*/
	PTRACK_CFG_DRAM_PAUSE_PERIOD, /* [0:7] FD/MOE(cdsp3), [8:31] rsvd */
	PTRACK_CFG_FRAME_FLESH_MODE, /* 0:off (default), o/w:on, TODO */
	PTRACK_CFG_FRAME_FLESH_CNT_THRS,
	PTRACK_CFG_FRAME_FLESH_SCORE_THRS,
	PTRACK_CFG_MIN_PATTERN_Y_LEVEL,
	/** hostFW can choice detect list, but baseFW can apply when it need. */
	PTRACK_CFG_PROC_PB_DETECT_LIST_SEL,
	PTRACK_CFG_PROC_COMPACT_BUF_MODE, /*1:enable, 0:disable, set before start*/

	/* shared by framework and algorithm */
	FTRACK_CFG_ALGORITHM_SHARE       = 0x800,
	PTRACK_CFG_PROC_MAX_PATTERNS = FTRACK_CFG_ALGORITHM_SHARE, /* max count of result */	
	PTRACK_CFG_PROC_IISII_OFF, /* skip moe oper. 1: disable II/SII, 0: enable */
	PTRACK_CFG_PROC_EACH_PATTERN_CB,
	PTRACK_CFG_PROC_DETECT_LIST_SEL,
	/* face tracking */
	PTRACK_CFG_FACE_COLOR_TABLE_SET, /*0:real,1:whiter*/
	PTRACK_CFG_FACE_LOVE_PORTRAIT_MODE,
	PTRACK_CFG_FACE_REDEYE_DETECTING,
	

	/* algorithm used */
	PTRACK_CFG_ALGORITHM_STRAT = 0x1000,
	/* face tracking */
	PTRACK_CFG_FACE_SMILE_DETECT = PTRACK_CFG_ALGORITHM_STRAT,
	PTRACK_CFG_FACE_BLINK_DETECT,
	PTRACK_CFG_FACE_BLINK_NUM, /* 1:one eye blink as blink, 2: two eyes */
	PTRACK_CFG_FACE_WINK_DETECT, /* 0: off, 1: enable */
	PTRACK_CFG_FACE_EYE_INFO_DETECT, /* 0: off, 1: enable */
	PTRACK_CFG_FACE_KEY_FACE, /*0:off,1:on,2:smile detect key face only*/
	PTRACK_CFG_FACE_CHECK_COLOR,
	PTRACK_CFG_FACE_RESTART,
	PTRACK_CFG_FACE_CHECK_NOSE,
	PTRACK_CFG_FACE_RES_USE_RAWBUF,
	PTRACK_CFG_FACE_SEARCH_LEVEL, /* 0~15, internal use only */
	PTRACK_CFG_FACE_REF_BUF_INFO,
	PTRACK_CFG_FACE_MOVE_SENSITIVITY, /* 0~100, 0: sensitive, 100: stable */
	PTRACK_CFG_FACE_POST_FILTER_PARAM0, /* 0: normal, 1: skin beauty */
	PTRACK_CFG_FACE_POST_PROCESS_BYPASS, /* disable feature check */
	PTRACK_CFG_FACE_TOTAL,
	
	/* car tracking */
	PTRACK_CFG_CAR_START = 0x2000,
	PTRACK_CFG_CAR_FC_WARN_DISTANCE = PTRACK_CFG_CAR_START,
	PTRACK_CFG_CAR_FC_ROI_CROP_FACTOR,
	PTRACK_CFG_CAR_FC_ROI_CROP_OFFSET,
	PTRACK_CFG_CAR_FC_NIGHT_YUV_LUT_TABLE,
	PTRACK_CFG_CAR_FC_SENSOR_PIXEL_UNIT,
	PTRACK_CFG_CAR_FC_SENSOR_FOCAL,
	PTRACK_CFG_CAR_FC_DAYNIGHT,
	PTRACK_CFG_CAR_FC_REMOVE_DETECT,
	PTRACK_CFG_CAR_FC_CENTER_POINT, /* b[31:16]: y, b[15:0]: x */
	PTRACK_CFG_CAR_FC_IMAGE_SCALE_FACTOR,
	PTRACK_CFG_CAR_FC_CENTER_TOLER_FACTOR,
	PTRACK_CFG_CAR_FC_RESERVED_BUFSIZE, /*buffer reserved in memFd pool*/
	PTRACK_CFG_CAR_FC_CFG_TOTAL,
	
	FTRACK_CFG_TOTAL,
} ptrackCfgSel_t;

/**************************************************************************
 * Face Tracking
 **************************************************************************/
/* face attributes */
/* b[0:5]: angle */
/* b[6]: right */
/* b[7]: left */
/* b[8]: no nose */
/* b[9:11]: smile degree */
/* b[31]: smile, b[30]: blink, b[29]: wink, b[28]: key face */
#define PTRACK_FACE_TYPE_SMILE   0x80000000
#define PTRACK_FACE_TYPE_BLINK   0x40000000
#define PTRACK_FACE_TYPE_WINK    0x20000000
#define PTRACK_FACE_TYPE_KEYFACE 0x10000000
#define PTRACK_FACE_TYPE_FACE    0
#define PTRACK_FACE_TYPE_RIGHT   0x00000040
#define PTRACK_FACE_TYPE_LEFT    0x00000080
#define PTRACK_FACE_TYPE_NO_NOSE 0x00000100

#define PTRACK_FACE_ANGLE_MASK   0x3f
#define PTRACK_FACE_SMILE_DEGREE_MASK 0xe00
typedef enum {
	PTRACK_FACE_SMILE_SMALL = (2 << 9),
	PTRACK_FACE_SMILE_MID   = (3 << 9),
	PTRACK_FACE_SMILE_LARGE = (4 << 9)
} ptrackFaceSmileDegree_e;

/* clockwise */
typedef enum {
	PTRACK_FACE_ANGLE_0 = 0x00,
	PTRACK_FACE_ANGLE_15,/*0x1*/
	PTRACK_FACE_ANGLE_30,/*0x2*/
	PTRACK_FACE_ANGLE_45,/*0x3*/
	PTRACK_FACE_ANGLE_60,/*0x4*/
	PTRACK_FACE_ANGLE_75,/*0x5*/
	PTRACK_FACE_ANGLE_90,/*0x6*/
	PTRACK_FACE_ANGLE_105,/*0x7*/
	PTRACK_FACE_ANGLE_120,/*0x8*/
	PTRACK_FACE_ANGLE_135,/*0x9*/
	PTRACK_FACE_ANGLE_150,/*0xA*/
	PTRACK_FACE_ANGLE_165,/*0xB*/
	PTRACK_FACE_ANGLE_180,/*0xC*/
	PTRACK_FACE_ANGLE_195,/*0xD*/
	PTRACK_FACE_ANGLE_210,/*0xE*/
	PTRACK_FACE_ANGLE_225,/*0xF*/
	PTRACK_FACE_ANGLE_240,/*0x10*/
	PTRACK_FACE_ANGLE_255,/*0x11*/
	PTRACK_FACE_ANGLE_270,/*0x12*/
	PTRACK_FACE_ANGLE_285,/*0x13*/
	PTRACK_FACE_ANGLE_300,/*0x14*/
	PTRACK_FACE_ANGLE_315,/*0x15*/
	PTRACK_FACE_ANGLE_330,/*0x16*/
	PTRACK_FACE_ANGLE_345 /*0x17*/
} ptrackFaceAngle_e;


/* flow selector ID */
typedef enum {
	FTRACK_DETECT_FLOW_SEL_NORMAL,
	FTRACK_DETECT_FLOW_SEL_LOVE_PORTRAIT,
	FTRACK_DETECT_FLOW_SEL_SIMPLE,
	FTRACK_DETECT_FLOW_SEL_PB,
	FTRACK_DETECT_FLOW_SEL_SECOND_PB,
	FTRACK_DETECT_FLOW_VEHICLE_DETECT,
	FTRACK_DETECT_FLOW_SEL_TOTAL
} ftrackDetectFlowSelectId_e;

/* flow selector ID */
typedef enum {
	FTRACK_CAR_DETECT_FLOW_VEHICLE_DETECT,
	FTRACK_CAR_DETECT_FLOW_SEL_TOTAL		
} ftrackCarDetectFlowSelectId_e;

/* DB type*/
typedef enum {
	FTRACK_CUS_DB_TYPE_NORMAL = 0, /*detector = 1:1*/
	FTRACK_CUS_DB_TYPE_CAR, /*detector = 2:1 => 1:1 , therefore the framebuf ratio needs to resize */
	FTRACK_CUS_DB_TYPE_NUMBER
} ftrackCustomDBType_e;


/**< face result. Host should prepare enough buffer or crash might happen */
typedef union {
	struct {
		UINT16 x:10;
		UINT16 y:10;
		UINT16 size:10;
		UINT8  sts:2;
	} elem;
	UINT32 l;
} ptrackFaceEye_t;


typedef struct {
	UINT32 x:10;
	UINT32 y:10;
	UINT32 size:10;
	UINT32 attr;
	ptrackFaceEye_t leye;
	ptrackFaceEye_t reye;
} ptrackFaceRes_t;


/**************************************************************************
 * Car Tracking
 **************************************************************************/
/* face attributes */
/* b[0:5]: angle */
/* b[6]: sedan */
/* b[7]: hb */
/* b[8]: truck */
/* b[9]: bus */
#define PTRACK_CAR_TYPE_VEHICLE_SEDAN 0x00000040
#define PTRACK_CAR_TYPE_VEHICLE_HB    0x00000080
#define PTRACK_CAR_TYPE_VEHICLE_TRUCK 0x00000100
#define PTRACK_CAR_TYPE_VEHICLE_BUS   0x00000200

typedef struct {
	UINT32 x:10;
	UINT32 y:10;
	UINT32 size:10;
	UINT32 attr;
	UINT32 dist;
} ptrackCarInfo_t;

typedef struct {
	UINT32 distance;
	UINT32 warn;
	UINT32 centerCarIdx;
	UINT32 num; /* Same as the return num of sp5kPatternTrackResultGet */
	ptrackCarInfo_t cars[20]; /* note that this is the start of the car array */
} ptrackCarRes_t;


/**************************************************************************
 * Obsolete
 **************************************************************************/
typedef struct ftrackEdgeWinAttr_s {
	/* for FW calculation */
	UINT32	zfacH;
	UINT32	zfacV;
	UINT32	muxHsize;
	UINT32	muxVsize;
	/* for HW assignment */
	UINT16	winThrH;
	UINT16	winThrV;
	UINT16	hOffset;
	UINT16	vOffset;
	UINT8	hSize;
	UINT8	vSize;
	UINT8	winCntH;
	UINT8	winCntV;
} ftrackEdgeWinAttr_t;

typedef struct ftrackSkinParam_s {
	UINT8 uh; /**< U high realm */
	UINT8 ul; /**< U low  threshold */
	UINT8 vh; /**< V high realm */
	UINT8 vl; /**< V low  threshold */
	UINT8 yh; /**< Y high realm */
	UINT8 yl; /**< Y low  threshold */
} ftrackSkinParam_t;

#ifdef __cplusplus
}
#endif

#endif /* _FTRACK_DEF_H_ */
