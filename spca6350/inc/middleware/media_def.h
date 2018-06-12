/**************************************************************************
 *
 *       Copyright (c) 2005-2012 by iCatch Technology, Inc.
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
 *  Author:
 *
 **************************************************************************/
#ifndef _MEDIA_DEF_H_
#define _MEDIA_DEF_H_

#include <sys/time.h> /* for struct timeval */

/**************************************************************************
 *                           C O N S T A N T S
 **************************************************************************/
#define CUSTOMER_TAG_SUPPORT          1
#define MUX_PERIODIC_SYNC_SUPPORT     1
#define REC_FRAME_RATE_SWITCH_SUPPORT 1
#define REC_AUDIO_RESAMPLE 1

typedef enum _mediaCallbackId_e {
	MEDIA_CALLBACK_REC_START = 0,
	MEDIA_CALLBACK_REC_STOP  = 1,
	MEDIA_CALLBACK_PLAY_STOP = 2,
	MEDIA_CALLBACK_SEAMLESS  = 3,
	MEDIA_CALLBACK_TOT,
} mediaCallbackId_e;

typedef enum _mediaPlayControl_e {
	MEDIA_PLAY_START         = 0,
	MEDIA_PLAY_ABORT         = 1,
	MEDIA_PLAY_PAUSE         = 2,
	MEDIA_PLAY_RESUME        = 3,
	MEDIA_PLAY_SEEK          = 4,
	MEDIA_PLAY_FORWARD       = 5,
	MEDIA_PLAY_REWIND        = 6,
	MEDIA_PLAY_SLOW_MOTION   = 7,
	MEDIA_PLAY_FRAME_CAPTURE = 8,
	MEDIA_PLAY_RESOURCE_AVI  = 9,/* @startAV */
	MEDIA_PLAY_TRIM     = 10,
	MEDIA_PLAY_MERGE    = 11,
	MEDIA_PLAY_SEAMLESS_EN   = 12,
	MEDIA_PLAY_SEAMLESS_LOAD = 13,
	MEDIA_PLAY_VIDEO_FRM_APPEND = 14,
} mediaPlayControl_e;

typedef enum _mediaRecControl_e {
	MEDIA_REC_PAUSE           = 0,
	MEDIA_REC_RESUME,
	MEDIA_REC_PIV_CAPTURE,
	MEDIA_REC_JFIF_CAPTURE,
	MEDIA_REC_EXIF_CAPTURE,
	MEDIA_REC_SWTRIG_START,
	MEDIA_REC_SWTRIG_STOP,
	MEDIA_REC_PREREC_MS,
	MEDIA_REC_PREREC_INIT,
	MEDIA_REC_PREREC_CTRL,
	MEDIA_REC_APPEND_NEXT_VIDEO,
	/* Stream Controller */
	MEDIA_REC_SC_CREATE,
	MEDIA_REC_SC_DESTROY,
	MEDIA_REC_SC_ATTACH,
	MEDIA_REC_SC_DETACH,
#if REC_FRAME_RATE_SWITCH_SUPPORT
	MEDIA_REC_FRAME_RATE_SWITCH,
#endif
	/* Stream */
	MEDIA_REC_STREAM_START,
	MEDIA_REC_STREAM_STOP,
	MEDIA_REC_STREAM_RESTART,
	/* Info */
	MEDIA_REC_GET_STREAM_INFO,
	MEDIA_REC_GET_SC_INFO,

	MEDIA_REC_SC_CREATE_EX,
} mediaRecControl_e;

typedef enum _mediaRecPivCfg_e {
	MEDIA_REC_PIV_MODE0,
	MEDIA_REC_PIV_MODE1,
	MEDIA_REC_PIV_MODE2_OPT1,
	MEDIA_REC_PIV_MODE2_OPT2,
} mediaRecPivCfg_e;

typedef enum _mediaRecScType_e {
	MEDIA_REC_SC_TYPE_FILE,
	MEDIA_REC_SC_TYPE_RTP,
	MEDIA_REC_SC_TYPE_SIMPLE,
#if defined(LINUX_STREAM_IF)
	MEDIA_REC_SC_TYPE_ES,
	MEDIA_REC_SC_TYPE_TS,
#endif
	MEDIA_REC_SC_TYPE_CUSTOM,
	MEDIA_REC_SC_TYPE_UVC,
	MEDIA_REC_SC_TYPE_FRAMEWORK,
	MEDIA_REC_SC_TYPE_MAX,
} mediaRecScType_e;
typedef enum _mediaPreRecMode_e {
	MEDIA_REC_PREREC_TRIG_MODE = 1,
	MEDIA_REC_PREREC_NORMAL_MODE,
} mediaPreRecMode_e;
typedef enum _mediaPreRecOpt_e {
	MEDIA_REC_PREREC_TRIG_FILE = 1,
	MEDIA_REC_PREREC_CLOSE_FILE,
} mediaPreRecOpt_e;
typedef enum _mediaRecStreamId_e {
	MEDIA_REC_STREAM_1ST	= 1<<0,
	MEDIA_REC_STREAM_2ND	= 1<<1,
	MEDIA_REC_STREAM_3RD	= 1<<2,
	MEDIA_REC_ENCODER		= 1<<3,
} mediaRecStreamId_e;

typedef enum _mediaPlayStreamId_e {
	MEDIA_PLAY_STREAM_1ST	= 1<<0,
	MEDIA_PLAY_STREAM_2ND	= 1<<1,
} mediaPlayStreamId_e;


enum {
	MEDIA_REC_STREAM_NUM  = 3,
	MEDIA_REC_STREAMS_H264_ALL = 3, /* id 0, id 1 */
	MEDIA_REC_STREAMS_ALL = (1 << MEDIA_REC_STREAM_NUM) - 1,
	MEDIA_REC_SC_NUM_MAX  = 8
};

typedef enum _mediaEdit_e {
	MEDIA_EDIT_TRIM = 0,
} mediaEdit_e;

typedef enum _mediaType_e {
	MEDIA_AAC = 0x05,
	MEDIA_MOV = 0x06,
	MEDIA_MP4 = 0x07,
	MEDIA_MTS = 0x08,
	MEDIA_TS  = 0x09,
} mediaType_e;

typedef enum _entropyType_e {
	MEDIA_H264_ENTROPY_CABAC = 0x0,
	MEDIA_H264_ENTROPY_CAVLC = 0x1
} entropyType_e;

typedef enum _gopStructure_e {
	MEDIA_H264_GOP_IBBP  = 0x00,
	MEDIA_H264_GOP_IPPP  = 0x10,
	MEDIA_H264_GOP_IIII  = 0x20,
} gopStructure_e;


typedef enum _profileType_e {
	MEDIA_H264_PROFILE_BASE_MAIN = 0x0,
	MEDIA_H264_PROFILE_HIGH = 0x1
} profileType_e;

typedef enum _intraMode_e {
	MEDIA_H264_INTRA_4_16 = 0x0,
	MEDIA_H264_INTRA_8_16 = 0x1,
	MEDIA_H264_INTRA_4_8 = 0x2
} intraMode_e;

typedef enum _h264VuiInfo_e {
	MEDIA_H264_VUI_VIDEO_SIGNAL_TYPE      = 1 <<  0,
	MEDIA_H264_VUI_ASPECT_RATIO_INFO      = 1 <<  1,
	MEDIA_H264_VUI_OVERSCAN_INFO          = 1 <<  2,
	MEDIA_H264_VUI_CHROMA_LOC_INFO        = 1 <<  3,
	MEDIA_H264_VUI_TIMING_INFO            = 1 <<  4,
	MEDIA_H264_VUI_NAL_HRD_PARAMETERS     = 1 <<  5,
	MEDIA_H264_VUI_VCL_HRD_PARAMETERS     = 1 <<  6,
	MEDIA_H264_VUI_PIC_STRUCT             = 1 <<  7,
	MEDIA_H264_VUI_BITSTREAM_RESTRICTION  = 1 <<  8
} h264VuiInfo_e;

typedef enum _mediaAttr_e {
	MEDIA_ATTR_FILE_NAME         = 0,
	MEDIA_ATTR_FILE_TYPE         = 1, /* modified by Edward 20070521 */
	MEDIA_ATTR_REC_STATE         = 2,
	MEDIA_ATTR_WIDTH             = 3,
	MEDIA_ATTR_HEIGHT            = 4,
	MEDIA_ATTR_DURATION          = 5,
	MEDIA_ATTR_ELAPSED_TIME      = 6,
	MEDIA_ATTR_SEEKABLE          = 7,

	MEDIA_ATTR_VIDEO_CODEC       = 8,
	MEDIA_ATTR_VIDEO_BRC_TYPE    = 9,
	MEDIA_ATTR_VIDEO_AVG_BITRATE = 10,
	MEDIA_ATTR_VIDEO_FRAME_RATE  = 11,

	MEDIA_ATTR_AUDIO_CODEC       = 12,
	MEDIA_ATTR_AUDIO_BRC_TYPE    = 13,
	MEDIA_ATTR_AUDIO_AVG_BITRATE = 14,
	MEDIA_ATTR_AUDIO_SAMPLE_RATE = 15,
	MEDIA_ATTR_AUDIO_SAMPLE_BITS = 16,
	MEDIA_ATTR_AUDIO_CHANNELS    = 17,
	MEDIA_ATTR_AUDIO_AAC_PLUS    = 18, /*@aac plus support*/
	MEDIA_ATTR_AUDIO_AAC_PROFILE = 19, /*@aac profile setting*/
	MEDIA_ATTR_AUDIO_AAC_HAS_ADTS = 20,

	MEDIA_ATTR_VIDEO_OPEN_DML    = 20,
	MEDIA_ATTR_VIDEO_DIVX        = 21,

	/**
	 WARNING:
	 Please keep enum value of "MEDIA_ATTR_FILE_NAME_BUF" ALIGN4.
	 Then, the address of "filename buffer" ALIGN16.
	 Otherwise, codeSentry CANNOT watch unknown program modify filename issue
	 */
	MEDIA_ATTR_FILE_NAME_BUF     = 24,
	MEDIA_ATTR_FILE_NAME_RSV     = MEDIA_ATTR_FILE_NAME_BUF + (256/4), /* 256 bytes - reserve for file name */
	MEDIA_ATTR_FILE_WIDTH,
	MEDIA_ATTR_FILE_HEIGHT,
	MEDIA_ATTR_H264_ENTROPY_MODE, /* 0 for CABAC, 1 for CAVLC */
	MEDIA_ATTR_H264_GOP_STRUCTURE,/* 0x00 for IBBP, 0x10 for IPPP, 0x20 for IIII */
	MEDIA_ATTR_H264_GOP_NO,       /* # of gop */
	/*
	 * b[0:7] qp ini value or qp value in fixed qp mode.
	 * b[8:15] qp max value (the worst qp value).
	 * b[16:23] qp min value (the best qp value).
	 * b[31] 0 - RC, 1 fixed QP.
	 */
	MEDIA_ATTR_H264_QP,		      /* CBR : for I/P/B, VBR : for P/B */
	MEDIA_ATTR_H264_QP_I,         /* VBR only : for I */
	MEDIA_ATTR_H264_PROFILE,      /* 0: base/main, 1: high profile */
	MEDIA_ATTR_H264_INTRA_MODE,   /* 0: I4+I16, 1: I8+I16, 2: I4+I8 */
	MEDIA_ATTR_H264_INTERLACE,    /* 0: progressive, 1: interlace mode */
	MEDIA_ATTR_H264_SLICE_NO,     /* 0~1: normal, 2~4: multi-slices */
	MEDIA_ATTR_H264_VUI_TAG,      /* b[0]: video_signal_type_present_flag */
	MEDIA_ATTR_H264_IDR_INTERVAL, /* 0:first I only, 1:every I frame, 2:two I frame, ... */
	MEDIA_ATTR_H264_AUD_SEI_INFO, /* 0: Not Available, 1: Available */
	MEDIA_ATTR_H264_SLICE_TYPE_OFFSET, /**< Offset of slice type, value may be 0, 5*/
	MEDIA_ATTR_H264_COLOR_PARAM,  /* 0: BT.709, 1: BT.601 */
	MEDIA_ATTR_H264_SPS_PPS_SIZE,
	MEDIA_ATTR_H264_SPS_PPS_DATA,
	MEDIA_ATTR_H264_MOTIONLESS,

	MEDIA_ATTR_MJPG_INITIAL_Q_FACTOR,	/* don't use it */

	MEDIA_ATTR_WITH_SUBTITLE,	/* 1 for subtitle embedded, 0 for no subtitle.  */
	MEDIA_ATTR_VIDEO_PTS_RATE,
	#if CUSTOMER_TAG_SUPPORT
	MEDIA_ATTR_REC_TAGS,
	MEDIA_ATTR_USER_DATA_SIZE,
	MEDIA_ATTR_USER_DATA,
	#endif
	MEDIA_ATTR_IDENTIFY_TAGS,
	MEDIA_ATTR_YUV_FORMAT,

	MEDIA_ATTR_2ND_WIDTH,
	MEDIA_ATTR_2ND_HEIGHT,
	MEDIA_ATTR_2ND_VIDEO_CODEC,
	MEDIA_ATTR_2ND_VIDEO_BRC_TYPE,
	MEDIA_ATTR_2ND_VIDEO_AVG_BITRATE,
	MEDIA_ATTR_2ND_VIDEO_FRAME_RATE,

	MEDIA_ATTR_2ND_H264_ENTROPY_MODE,
	MEDIA_ATTR_2ND_H264_GOP_STRUCTURE,
	MEDIA_ATTR_2ND_H264_GOP_NO,
	MEDIA_ATTR_2ND_H264_QP,
	MEDIA_ATTR_2ND_H264_QP_I,
	MEDIA_ATTR_2ND_H264_PROFILE,
	MEDIA_ATTR_2ND_H264_INTERLACE,
	MEDIA_ATTR_2ND_H264_SLICE_NO,
	MEDIA_ATTR_2ND_H264_IDR_INTERVAL,
	MEDIA_ATTR_MERGE_DST_FILE,
	MEDIA_ATTR_MERGE_DST_AUD_CODEC,
	MEDIA_ATTR_SW_TRIGGER_DST_FILE,
	MEDIA_ATTR_TRIM_DST_FILE,
	MEDIA_ATTR_VIDEO_MAX_BITRATE,
	MEDIA_ATTR_VIDEO_MIN_BITRATE,
	MEDIA_ATTR_2ND_VIDEO_MAX_BITRATE,
	MEDIA_ATTR_2ND_VIDEO_MIN_BITRATE,
	MEDIA_ATTR_2ND_ELAPSED_TIME,

	MEDIA_ATTR_REC_STATE_VIDEO_FIFO_USED,
	MEDIA_ATTR_REC_STATE_AUDIO_FIFO_USED,

	MEDIA_ATTR_INIT_QVALUE,
	MEDIA_ATTR_CUR_QVALUE,

	MEDIA_ATTR_SPHERICAL_VIDEO_TAG_ENABLE,
	MEDIA_ATTR_AUDIO_BLOCK_ALIGN,

	MEDIA_ATTR_VIDEO_VARIABLE_DURATION_EN,

	MEDIA_ATTR_MAX
} mediaAttr_e;

typedef enum {
	MEDIA_PLAY_THUMB_SCALE    = 0,
	MEDIA_PLAY_THUMB_BGCOLOR  = 1,
	MEDIA_PLAY_THUMB_BGFRAME  = 2,
	MEDIA_PLAY_THUMB_COLORKEY = 3,
	MEDIA_PLAY_THUMB_SKIP     = 4,
	MEDIA_PLAY_THUMB_BGFRAME_FILENAME = 5,
	MEDIA_PLAY_THUMB_CFG_MAX
} mediaPlayThumbCfg_e;

typedef enum {
	MEDIA_MOV_USER_DATA = 0,
	MEDIA_AVI_CHUNK_STRD = 0,
	MEDIA_AVI_CHUNK_STRD_2ND = 1,
	MEDIA_AVI_CHUNK_STRN = 2,
	MEDIA_AVI_CHUNK_STRN_2ND = 3,
	MEDIA_AVI_CHUNK_ISFT = 4,
	MEDIA_AVI_CHUNK_JUNK = 5,
	MEDIA_AVI_CHUNK_SUBTITLE = 6,
	MEDIA_AVI_CHUNK_MAX = 7,
	MEDIA_AVI_CHUNK_INFO = 0x80,
} mediaAviChunk_e;

typedef enum _audAlcParam_e {
	ALC_PARAM_ALC_OPT = 0x0,

	#if SPCA6350
	/*function control*/
	ALC_PARAM_NOISEGATE_EN,
	ALC_PARAM_ANTICLIP_EN,
	ALC_PARAM_QUICKRELEASE_EN,
	/*Gain Limit (GL)*/
	ALC_PARAM_MAX_GAIN,
	ALC_PARAM_MIN_GAIN,
	ALC_PARAM_NOISEGATE_MIN_GAIN,
	/*Static Curve (SC)*/
	ALC_PARAM_BASE_OUTPUT_VOL = 0x10, /**< Base output level for input match with THD1. Also called y1 in DRC spec */
	ALC_PARAM_THD1, /**< Threashold 1 for input audio level */
	ALC_PARAM_THD2, /**< Threashold 2 for input audio level */
	ALC_PARAM_THD3, /**< Threashold 3 for input audio level */
	ALC_PARAM_THD4, /**< Threashold 4 for input audio level */
	ALC_PARAM_THD5, /**< Threashold 5 for input audio level */
	ALC_PARAM_SLOP1, /**< Slop 1: between 0db ~ THD1 */
	ALC_PARAM_SLOP2, /**< Slop 2: between THD1 ~ THD2 */
	ALC_PARAM_SLOP3, /**< Slop 3: between THD2 ~ THD3 */
	ALC_PARAM_SLOP4, /**< Slop 4: between THD3 ~ THD4 */
	ALC_PARAM_SLOP5, /**< Slop 5: between THD4 ~ THD5 */
	ALC_PARAM_SLOP6, /**< Slop 6: for input data < THD5. Also called Noise Gate area. */
	/*Level Measurement (LM)*/
	ALC_PARAM_LM_ATTACK_TIME = 0x20,
	ALC_PARAM_LM_RELEASE_TIME,
	ALC_PARAM_LM_AVG_TIME,
	/*Gain Smooth (GS)*/
	ALC_PARAM_GS_ATTACK_TIME,
	ALC_PARAM_GS_DECAY_TIME,
	ALC_PARAM_GS_DECAY_QR_TIME,
    /* SW ALC control PGA */
	ALC_PARAM_MAX_VOL_DB,
	ALC_PARAM_HOLD_TIME_MS,

	#elif SPCA6330
	ALC_PARAM_ALC_WIN_SIZE,
	ALC_PARAM_ALC_MAX_VOL,
	ALC_PARAM_ALC_MIN_VOL,
	ALC_PARAM_ALC_MAX_GAIN_IDX,
	ALC_PARAM_ALC_MIN_GAIN_IDX,
	ALC_PARAM_ALC_DEFAULT_GAIN_IDX,
	ALC_PARAM_ALC_GAIN_UP_SPEED,
	ALC_PARAM_ALC_GAIN_DOWN_SPEED,
	ALC_PARAM_ALC_MAX_VOL_GAP,
	ALC_PARAM_SILENCE_GAIN_DOWN_SPEED,
	ALC_PARAM_ALC_GAIN_UP_STEP,
	ALC_PARAM_ALC_GAIN_DOWN_STEP,
	#endif

	ALC_PARAM_ALC_CUR_ENGENY,
	ALC_PARAM_ALC_MAX

} audAlcParam_e;

#define MEDIA_COM_CFG_SELECTOR_OFFSET 0x1000
typedef enum _mediaComCfg_e {
	MEDIA_AVI_INTERLEAVE_INTERVAL = MEDIA_COM_CFG_SELECTOR_OFFSET,
	MEDIA_COM_CFG_MAX,
} mediaComCfg_e;

#define MEDIA_REC_CFG_SELECTOR_OFFSET 0x2000
typedef enum _mediaRecCfg_e {
	MEDIA_REC_FLIP_ROTATE = MEDIA_REC_CFG_SELECTOR_OFFSET,
	MEDIA_REC_ALC,
	MEDIA_REC_DIS_LEVEL,
	MEDIA_REC_PAUSE_CALLBACK,
	MEDIA_REC_DISK_THR_KB,
	MEDIA_REC_DIS_GMV_FILTER,
	MEDIA_REC_SEAMLESS_CALLBACK,
	MEDIA_REC_FILE_CLOSE_CALLBACK,
	MEDIA_REC_FILE_CHANGED_CALLBACK,
	MEDIA_REC_USRDATA_CALLBACK,
	MEDIA_REC_FILENAME_CALLBACK,
	MEDIA_REC_ALC_MODE,
	MEDIA_REC_VLC_BUF_SIZE,
	MEDIA_REC_VLC_BUF_CNT,
	MEDIA_REC_MAX_QVALUE,
	MEDIA_REC_DIS_COMP_BYPASS, /* Bypass DIS compenstation */
	MEDIA_REC_DIS_COMP_TYPE, /* DIS compenstation Type: fast, smooth */
	MEDIA_REC_DIS_COMPENSATE_CALC_CB,/* DIS compenstation Type: 3, compenstation offset calculate customer cb */
	MEDIA_REC_DIS_GMV_CALC_CB,/* DIS  gmv calculate customer cb */
	/*
	 * DIS compensation range in %, this defines the available compensate region
	 * the MEDIA_REC_DIS_COMP_RANGE_WIDTH is limited by this and the
	 * MEDIA_REC_DIS_COMP_RANGE_MAX_W.
	 */
	MEDIA_REC_DIS_COMP_RANGE,
	MEDIA_REC_DIS_COMP_RANGE_MAX_W, /* DIS compensation range max in pixel*/
	MEDIA_REC_DIS_COMP_RANGE_MAX_H, /* DIS compensation range max in pixel*/
	MEDIA_REC_DIS_COMP_RANGE_WIDTH, /* DIS compensation range in pixel*/
	MEDIA_REC_DIS_COMP_RANGE_HEIGHT, /* DIS compensation range in pixel*/
	MEDIA_REC_DIS_EDGE_THRESHOLD, /*DIS edgewin threshold*/
	MEDIA_REC_DIS_SEARCH_RANGE,
	MEDIA_REC_DIS_EDGE_WIN_H_COUNT,
	MEDIA_REC_DIS_EDGE_WIN_V_COUNT,
	MEDIA_REC_PIV_CONFIG,
	MEDIA_REC_PIV_AVAILABLE_JOB,
	MEDIA_REC_PIV_FRAMES,
	MEDIA_REC_PIV_FRAMES_WIDTH,
	MEDIA_REC_PIV_FRAMES_HEIGHT,
	/*
	 * Mode 0:
	 * UINT32 frmCb(UINT32 id, UINT32 addr, sp5kYuvCbImgSize_t *p, void *rsv);
	 * Mode 1/2:
	 * UINT32 frmCb(UINT32 id, UINT32 addr, sp5kImgWbInfo_t *p, void *rsv);
	 */
	MEDIA_REC_PIV_FRAME_CB,
	/*
	 * UINT32 procCb(UINT32 id, UINT32 addr, sp5kYuvCbImgSize_t *p, void *rsv);
	 * return TRUE(1) for save file.
	 */
	MEDIA_REC_PIV_PROC_CB,
	/*
	 * UINT32 iqCb(UINT32 param1, UINT32 param2, void *param3);
	*/
	MEDIA_REC_PIV_IQ_CB,
	MEDIA_REC_INPUT_FRAME_RATE,
	MEDIA_REC_2ND_INPUT_FRAME_RATE,
#if REC_FRAME_RATE_SWITCH_SUPPORT
	MEDIA_REC_FRAME_RATE_SWITCH_EN,
#endif
#if REC_AUDIO_RESAMPLE
	MEDIA_REC_AUDIO_RESAMPLE_EN,
#endif
	MEDIA_REC_VIDEO_FIFO_BUF_NO,
	MEDIA_REC_AUDIO_FIFO_BUF_NO,
	MEDIA_REC_SWTRIG_CB,
	MEDIA_REC_SWTRIG_CFG_PTR_CB,
	MEDIA_REC_DUP_L_TO_R_ENABLE,
	MEDIA_REC_LR_SWAP_ENABLE,
	MEDIA_REC_AUD_FILTERMUX,
	MEDIA_REC_SEAMLESS_TIME_SLOT, /*seamless */
	MEDIA_REC_SEAMLESS_SIZE_SLOT, /*seamless */
	MEDIA_REC_SEAMLESS_FULL_OPTION,
	MEDIA_REC_SEAMLESS_CHANGE_FILE,
	MEDIA_REC_SEAMLESS_SIZE_LIMIT_CB,
	MEDIA_REC_TRIGGER_TIME_SLOT,  /*trigger mode*/
	MEDIA_REC_TRIGGER_FRONT_TIME,
 	MEDIA_REC_TRIGGER_BEHIND_TIME,
	MEDIA_REC_EVENT_TRIGER_EN,
	MEDIA_REC_EVENT_TRIGGER_MODE,
	MEDIA_REC_TRIGGER_FILE_NAME,
	MEDIA_REC_2ND_TRIGGER_FILE_NAME,
	MEDIA_REC_EVENT_TRIGGER_TYPE,

	MEDIA_REC_RECOVERY_EN,
	MEDIA_REC_RECOVERY_OVER4G_EN,
	MEDIA_REC_RECOVERY_FLUSH_INTERVAL,

	MEDIA_REC_MULTI_STREAM_EN, /**< enable of multi stream feature 0: en, 1: dis */
	MEDIA_REC_MULTI_STREAMS,   /**< enable of which stream. b[0]: 1st, b[1]: 2nd */

	MEDIA_REC_END_CLIP_MS,
	MEDIA_REC_START_CLIP_MS,

	/*
	 * Pointer to the callbacks for custom stream controller.
	 * (mediaRecScCustomCbList_t), should not be local variable.
	 */
	MEDIA_REC_SC_CUSTOM_CBLIST,

	MEDIA_REC_DATA_CHUNK_ALIGN_FLOW,/**< enable of video and audio data chunk align flow 1: en, 0: dis (MEDIA_REC_END_CLIP_MS need enable)*/
	MEDIA_REC_2ND_VLC_BUF_SIZE,
	MEDIA_REC_2ND_VLC_BUF_CNT,

	MEDIA_REC_PERIODIC_SYNC_EN,	/* om-702 MOOV Periodic Sync Function */
	MEDIA_REC_PERIODIC_SYNC_CALLBACK_SET, /* om-702 MOOV Periodic Sync Function */

	MEDIA_REC_H264_PFRAME_SKIP,     /**< insert dummy p frame for IPPP gop */
	MEDIA_REC_H264_SEI_CB_INIT_CB,
	MEDIA_REC_EXTENSION_TYPE_EN, /*Enable record extension type file, support file size over 4GB */
	MEDIA_REC_AUD_FRAME_DURATION,

	MEDIA_REC_MJPG_USED,

	MEDIA_REC_VIDEO_STREAM_ORDER,
	MEDIA_REC_H264_STOP_MODE,

	MEDIA_REC_MIN_QVALUE,
	MEDIA_REC_MJPG_Q_TABLE_Y,
	MEDIA_REC_MJPG_Q_TABLE_UV,

	MEDIA_REC_VIDEO_VFRC_ENABLE	,	/*[0]1st,[1]2nd,[2]3rd*/
	MEDIA_REC_VIDEO_VFRC_BITRATE,
	MEDIA_REC_VIDEO_VFRC_ATTR,		/*[23~16]step,[15~8]minfr,[7~0]maxfr*/
	MEDIA_REC_2ND_VIDEO_VFRC_BITRATE,
	MEDIA_REC_2ND_VIDEO_VFRC_ATTR,	/*[23~16]step,[15~8]minfr,[7~0]maxfr*/
	MEDIA_REC_H264_IFRAME_SHIELD_EN, /** insert dummy p frame before the same YUV I frame for I frame backup under decode process. */
	MEDIA_REC_H264_CLK,
	MEDIA_REC_CFG_MAX,
} mediaRecCfg_e;

typedef enum _eventTrigType_e {
	MEDIA_REC_INTERRUPT_SEAMLESS   	= 0,
	MEDIA_REC_UNINTERRUPT_SEAMLESS 	= 1,
}eventTrigType_e;

typedef void (*pfMediaRecHdrSyncCallback_t)(UINT32 fd, const UINT8 *buf, UINT32 size);
typedef void (*pfMediaRecAviHdrSyncCallback_t)(UINT32 fd, const UINT8 *buf, UINT32 size, UINT32 idx1_ofs);


#define MEDIA_PLAY_CFG_SELECTOR_OFFSET 0x3000
typedef enum _mediaPlayCfg_e {
	MEDIA_PLAY_MUTE_VIDEO = MEDIA_PLAY_CFG_SELECTOR_OFFSET,
	MEDIA_PLAY_MUTE_AUDIO,
	MEDIA_PLAY_FLIP_ROTATE,
	MEDIA_PLAY_VIDEO_FF_END_PAUSE,
	MEDIA_PLAY_TARGET_FRAME_RATE,
	MEDIA_PLAY_DISP_FRAME_CB,
	MEDIA_PLAY_VIDEO_DATA_CB,
	MEDIA_PLAY_AUDIO_DATA_CB,
	MEDIA_PLAY_VIDEO_FRAME_DATA_CB,
	MEDIA_PLAY_AUDIO_FRAME_DATA_CB,
	MEDIA_PLAY_START_TIME,
	MEDIA_PLAY_END_TIME,
	MEDIA_PLAY_BG_MUSIC_VOLUME,
	MEDIA_PLAY_SOUND_EFFECT_CB,
	MEDIA_PLAY_SOUND_EFFECT_SRC,
	MEDIA_PLAY_USER_DATA_BUF,
	MEDIA_PLAY_USRDATA_CALLBACK,
	MEDIA_PLAY_VIDEO_SPEED_CTRL_CB,
	/*
	 * mode:
	 *	0 - show decoding error and no further decode.
	 *	1 - ignore the decoding error and continues from next frame.
	 *	2 - ignore the decoding error and continues from next key frame.
	 */
	MEDIA_PLAY_ERROR_HANDLE_MODE,
	MEDIA_PLAY_AFFINE,
	MEDIA_PLAY_HEADER_BUF_SIZE,
	MEDIA_PLAY_MAX_DEC_BUF_NUM,
	MEDIA_PLAY_YUV_SCALE_DIV,
	MEDIA_PLAY_AUDIO_CH_MIX_EN,
	MEDIA_PLAY_FILE2_START_MERGE_TIME,
	MEDIA_PLAY_DISP_UPD_DIS,
	MEDIA_PLAY_VIDEO_STREAM,
	MEDIA_PLAY_DECODE_FRAME_RATE,
	MEDIA_PLAY_DISP_FRAME_RATE,
	MEDIA_PLAY_DATA_SOURCE,
	MEDIA_PLAY_YUV_SCALE_OPT,
	MEDIA_PLAY_SEAMLESS_FILENAME_CB,
	MEDIA_PLAY_CFG_MAX,
} mediaPlayCfg_e;

typedef enum _mediaPlaySndEffectSrc_e {
	MEDIA_PLAY_SOUND_EFFECT_SRC_BG_MUSIC,
	MEDIA_PLAY_SOUND_EFFECT_SRC_VIDEO,
	MEDIA_PLAY_SOUND_EFFECT_SRC_MERGED
} mediaPlaySndEffectSrc_e;

typedef enum _mediaPlayDataSrc_e {
	MEDIA_PLAY_DATA_SRC_FILE,
	MEDIA_PLAY_DATA_SRC_CUSTOM
} mediaPlayDataSrc_e;

typedef enum _mediaPlayDataType_e {
	MEDIA_PLAY_DATA_AUDIO,
	MEDIA_PLAY_DATA_VIDEO
} mediaPlayDataType_e;



#define MEDIA_REC_CTRL_CFG_SELECTOR_OFFSET 0x4000
typedef enum _mediaRecCtrlCfg_e {
	MEDIA_REC_CTRL_PREREC_MS = MEDIA_REC_CTRL_CFG_SELECTOR_OFFSET,
	MEDIA_REC_CTRL_APPEND_NEXT_VIDEO,
	MEDIA_REC_CTRL_FRAME_RATE_SWITCH,
	MEDIA_REC_CTRL_STAMP_PAUSE,
	MEDIA_REC_CTRL_CFG_MAX,
} mediaRecCtrlCfg_e;


typedef enum {
	/* encode operation */
	SP5K_MEDIA_SYS_CFG_H264_MB_QP_DIFF, /* &qparr, type */
	SP5K_MEDIA_SYS_CFG_H264_MV_CTRL,    /* mode, termination, shift */
	/* obsolete */
	SP5K_MEDIA_SYS_CFG_h264_MV_CTRL = SP5K_MEDIA_SYS_CFG_H264_MV_CTRL,
	SP5K_MEDIA_SYS_CFG_H264_QT_AC_SKIP,
	SP5K_MEDIA_SYS_CFG_H264_RC_SCALER,
	SP5K_MEDIA_SYS_CFG_H264_DYNAMIC_BITRATE,
	SP5K_MEDIA_SYS_CFG_H264_STRICT_SLICE_TYPE,
	SP5K_MEDIA_SYS_CFG_H264_SPS_LENGTH,
	SP5K_MEDIA_SYS_CFG_H264_SPS_LEVEL_IDC,
	SP5K_MEDIA_SYS_CFG_H264_SPS_FORCE_FRAME_CROPPING,
	SP5K_MEDIA_SYS_CFG_H264_SEQ_SCALING_MATRIX_PRESENT,
	SP5K_MEDIA_SYS_CFG_H264_LOG2_MAX_FRAME_NUM_MINUS4,
	SP5K_MEDIA_SYS_CFG_H264_LOG2_MAX_PIC_ORDER_CNT_LSB_MINUS4,
	SP5K_MEDIA_SYS_CFG_H264_PPS_PIC_INIT_QP,
	SP5K_MEDIA_SYS_CFG_H264_DEBLOCKING_FILTER_CTRL_PRESENT,
	SP5K_MEDIA_SYS_CFG_IDENTIFY_TAGS_POS,
} mediaSystemCfg_e;


typedef enum {
	MEDIA_SEAMLESS_SLOT_TYPE_NONE,
	MEDIA_SEAMLESS_SLOT_TYPE_TIME,
	MEDIA_SEAMLESS_SLOT_TYPE_SIZE
} mediaSeamlessSlotType_e;

typedef enum {
	MEDIA_REC_SEAMLESS_FULL_OPTION_DEL_OLDEST_FILE,
	MEDIA_REC_SEAMLESS_FULL_OPTION_STOP,
} mediaSeamlessFullOption_e;

typedef enum {
	MEDIA_H264_UNUSED,
	MEDIA_H264_USE_BY_ENCODER,
	MEDIA_H264_USE_BY_DECODER
} mediaH264Owner_e;

#define MEDIA_REC_YUV422              0x01
#define MEDIA_REC_YUV420              0x00

#define MEDIA_REC_ROTATE_0            0x00
#define MEDIA_REC_ROTATE_90           0x01
#define MEDIA_REC_ROTATE_180          0x02
#define MEDIA_REC_ROTATE_270          0x03
#define MEDIA_REC_FLIP_VERTICAL       0x04
#define MEDIA_REC_FLIP_HORIZONTAL     0x05

#define MEDIA_UBR                     0x00
#define MEDIA_CBR                     0x01
#define MEDIA_VBR                     0x02

#define MEDIA_AUDIO_UNKNOWN           0x0000
#define MEDIA_AUDIO_PCM               0x0001
/*#define MEDIA_AUDIO_ADPCM             0x0002*/
#define MEDIA_AUDIO_ALAW              0x0006
#define MEDIA_AUDIO_MULAW             0x0007
#define MEDIA_AUDIO_IMA_ADPCM         0x0011
#define MEDIA_AUDIO_G726              0x0045
#define MEDIA_AUDIO_MPEGLAYER3        0x0055
#define MEDIA_AUDIO_WMA1              0x0160
#define MEDIA_AUDIO_AAC               0x00FF

#define MEDIA_VIDEO_UNKNOWN           0x0000
#define MEDIA_VIDEO_MPEG4             0x5634504d /* LE "MP4V" */
#define MEDIA_VIDEO_MJPG              0x47504a4d /* LE "MJPG" */
#define MEDIA_VIDEO_H264              0x34363248 /* LE "H264" */
#define MEDIA_VIDEO_CUSTOM            0x54535543 /* LE "CUST" */

#define MEDIA_REC_DIS_OFF             0x00 /* DIS off */
#define MEDIA_REC_DIS_LOW             0x01 /* not use, compatibility, will be removed sometime later */
#define MEDIA_REC_DIS_MID             0x02 /* DIS enable, record only */
#define MEDIA_REC_DIS_HIGH            0x03 /* not use, compatibility, will be removed sometime later */
#define MEDIA_REC_DIS_REC_ONLY        0x02 /* DIS enable, record only */
#define MEDIA_REC_DIS_REC_PV          0x01 /* DIS enable, pv and record */
#define MEDIA_REC_RSC_OFF             0x10 /* not use, compatibility, will be removed sometime later */
#define MEDIA_REC_RSC_LOW             0x11 /* not use, compatibility, will be removed sometime later */
#define MEDIA_REC_RSC_MID             0x12 /* not use, compatibility, will be removed sometime later */
#define MEDIA_REC_RSC_HIGH            0x13 /* not use, compatibility, will be removed sometime later */
#define MEDIA_REC_RSC_DIS_OFF         0x20 /* not use, compatibility, will be removed sometime later */
#define MEDIA_REC_RSC_DIS_LOW         0x21 /* not use, compatibility, will be removed sometime later */
#define MEDIA_REC_RSC_DIS_MID         0x22 /* not use, compatibility, will be removed sometime later */
#define MEDIA_REC_RSC_DIS_HIGH        0x23 /* not use, compatibility, will be removed sometime later */

#define MEDIA_MSG_PARAM_OKAY          0x00
#define MEDIA_MSG_PARAM_FAIL          0x01
#define MEDIA_MSG_PARAM_NOHANDLE      0x02	/*add for mux open fail*/

#define MEDIA_MSG_PLAY_START          0x500
#define MEDIA_MSG_PLAY_STOP           0x501
#define MEDIA_MSG_PLAY_END            0x502
#define MEDIA_MSG_REC_START           0x503
#define MEDIA_MSG_REC_STOP            0x504
#define MEDIA_MSG_REC_DISK_FULL       0x505
#define MEDIA_MSG_FF_TO_END           0x506
#define MEDIA_MSG_RW_TO_END           0x507
#define MEDIA_MSG_SOUND_FINISH        0x508
#define MEDIA_MSG_PLAY_FILE_ERROR     0x50b
#define MEDIA_MSG_REC_PAUSE_FINISH    0x50c
#define MEDIA_MSG_FILE_SIZE_NOTIFY 	  0x50d
#define MEDIA_MSG_REC_DROP_FRAME      0x50e
#define MEDIA_MSG_AVI_REC_FPS_LOW     0x50f
#define MEDIA_MSG_REC_AUD_HW_STOP     0x510
#define MEDIA_MSG_TRIM_END            0x511/* @mediaTrim */
#define MEDIA_MSG_SEMALESS_CNT_NOTIFY 0x512/* @seamless */
#define MEDIA_MSG_URGENT_CB_STOP      0x513/* @urgentCB */
#define MEDIA_MSG_SEMALESS_DELETE     0x514/* @seamless */
#define MEDIA_MSG_REC_FRAME_CAPUTRE_END 0x515
#define MEDIA_MSG_REC_RESUME_FINISH    0x516
#define MEDIA_MSG_SEAMLESS_CHANGE_FILE 0x517/* @seamless video tag*/
#define MEDIA_MSG_REC_CDSP_SKIP_FRAME  0x518
#define MEDIA_MSG_CODEC_FPS_LOW       0x519

#define MEDIA_MSG_PLAY_SEAMLESS_LOAD_NOTIFY  0x520
#define MEDIA_MSG_PLAY_SEAMLESS_LOAD_FINISH  0x521
#define MEDIA_MSG_PLAY_SEAMLESS_LOAD_FAILED  0x522
#define MEDIA_MSG_REC_FIRST_FRAME_READY      0x523
#define MEDIA_MSG_PLAY_DONE           0x524

#define MEDIA_MSG_REC_VLC_FULL        0x528

#define MEDIA_MAX_FILE_NAME    ( 30 )

#define MEDIA_REC_ALC_CST_MODE      0x00  /* SPCA6350: None */
#define MEDIA_REC_ALC_DRC_MODE      0x01  /* SPCA6350: Dynamic control digital gain by HW */
#define MEDIA_REC_ALC_DRC_PGA_MODE  0x02  /* SPCA6350: Dynamic control digital gain by HW and PGA by SW  */


#define MEDIA_REC_H264_NORMAL_STOP    0x00
#define MEDIA_REC_H264_FAST_STOP      0x01

/**************************************************************************
 *                Media Buffer for Source and Sink
 **************************************************************************/
typedef struct mediaBufferObject_s {} *mediaBufferObject_t;

/* Buffer Flags*/
enum {
	/* bit[0:3]: Elementary Stream type */
	MEDIA_BUF_ES_UNKNOW   = 0,

	MEDIA_BUF_ES_VIDEO    = 1,
	MEDIA_BUF_ES_AUDIO    = 2,
	/* MEDIA_SRC_ES_TEXT    = 3, Not used now. */

	MEDIA_BUF_ES_NR       = 2,
	MEDIA_BUF_ES_BITMASK  = 0x0F,

	/* bit[4]: Key-frame flag */
	MEDIA_BUF_KEY_FRAME   = 0x01 << 4,
	MEDIA_BUF_PARAM_FRAME = 0x01 << 5,	
};

typedef enum {
	MEDIA_BUF_CODEC_UNKNOW = 0,
	MEDIA_BUF_CODEC_H264   = MEDIA_VIDEO_H264,
	MEDIA_BUF_CODEC_JPEG   = MEDIA_VIDEO_MJPG,
	MEDIA_BUF_CODEC_PCM    = MEDIA_AUDIO_PCM,
	MEDIA_BUF_CODEC_AAC    = MEDIA_AUDIO_AAC,
} mediaBufCodecType_e;

#define MEDIA_BUF_ES_TYPE(flags)        ((flags)&MEDIA_BUF_ES_BITMASK)
#define MEDIA_BUF_ES_IDX(es)            (((es)&MEDIA_BUF_ES_BITMASK) - 1)  /* Start from 0. */
#define MEDIA_BUF_CODEC_IS_AUDIO(c)     ((c) == MEDIA_BUF_CODEC_PCM || (c) == MEDIA_BUF_CODEC_AAC)
#define MEDIA_BUF_CODEC_IS_VIDEO(c)     ((c) == MEDIA_BUF_CODEC_H264 || (c) == MEDIA_BUF_CODEC_JPEG)

/**************************************************************************
 *                Media Source
 **************************************************************************/

enum {
	/* Flags for mediaSrcOpenXXX functions. */
	MEDIA_SRC_NO_AUDIO_SWAP = 1 << 0,
	MEDIA_SRC_ELEMENT_AUDIO = 1 << 1,
	MEDIA_SRC_ELEMENT_VIDEO = 1 << 2,

	MEDIA_SRC_ATTR_AVDIFF_TIME = MEDIA_ATTR_MAX + 1000 /* PTS of first audio - PTS of first video. Unit: MS. */
};

typedef enum {
	MEDIA_SRC_CTRL_PAUSE   = 0, /* No arg */
	MEDIA_SRC_CTRL_RESUME,      /* No arg */
	MEDIA_SRC_CTRL_SEEK,        /* arg: UINT32 param*/
	MEDIA_SRC_CTRL_DUMP,        /* No arg */
	/* Return all supported attribute IDs.
	 * arg: mediaAttr_e **attrs, UINT32 *num. The last element is (UINT32)-1
	 */
	MEDIA_SRC_CTRL_GET_ATTRS,
	/* Video Dynamic BitRate */
	MEDIA_SRC_CTRL_VDBR_SET,

	MEDIA_SRC_CTRL_NR
} mediaSrcCtrlCode_e;

typedef void *mediaSrcHandle_t; /* 0 is invalid handle */

typedef struct mediaSrcBuffer_s
{
	mediaBufferObject_t bufobj; /* For releasing buffer */
	UINT32 bufflags;
	UINT8  *data;
	UINT32 length;
	UINT32 duration; /* in ms */
	struct timeval pts; /* Presentation timestamp. */
	struct timeval enctime; /* Encoding time. */
} mediaSrcBuffer_t;

typedef void (*mediaSrcOnGotBuffer_t)(mediaSrcBuffer_t *buf, unsigned long user_data);

/**************************************************************************
 *                Media Sink
 **************************************************************************/
/* Media Sink Types */
enum {
	MEDIA_SNK_TYPE_UNKNOW  = 0,
	MEDIA_SNK_TYPE_DEMUX   = 'D',
	MEDIA_SNK_TYPE_FILE    = 'F',
	MEDIA_SNK_TYPE_RAWFILE = 'R',

	MEDIA_SNK_TYPE_NUM     = 3
};

/* Flags for media sink functions. */
enum {
	MEDIA_SNK_FLG_NO_AUDIO_SWAP = 1 << 0,
	MEDIA_SNK_FLG_LIVE          = 1 << 1,
	MEDIA_SNK_FLG_GOPCHK        = 1 << 2,
	MEDIA_SNK_FLG_DEBUG         = 1 << 3,
	MEDIA_SNK_FLG_CACHE_FIRST_ONLY = 1 << 4,
};

typedef enum {
	/* Set */
	MEDIA_SNK_CTRL_START = 0, /* No Argument */
	MEDIA_SNK_CTRL_STOP,      /* No Argument */

	/* Get */
	MEDIA_SNK_CTRL_ISSTARTED, /* Argument: BOOL*. */
	MEDIA_SNK_CTRL_GET_STATS, /* Statistics. Arguments: mediaSnkStats_t*. */

	MEDIA_SNK_CTRL_NR
} mediaSnkCtrlCode_e;

typedef enum {
	/* No argument. */
	MEDIA_SNK_MSG_PAUSE = 0,
	MEDIA_SNK_MSG_PAUSE_WAIT,
	/* long: -1, resume from current position; >= 0: new position. */
	MEDIA_SNK_MSG_RESUME,
	MEDIA_SNK_MSG_RESUME_WAIT,

	MEDIA_SNK_MSG_NR
} mediaSnkMsg_e;

/* Additional Attributes */
enum {
	MEDIA_SNK_ATTR_CACHETIME        = 0x7FFFFFU,
	MEDIA_SNK_ATTR_DMXDROPTIME,     /* UINT32. Demux frame drop time.*/
	MEDIA_SNK_ATTR_SRVTIMEDIFF,     /* INT32. Server time = local time + diff.  */
};

typedef void *mediaSnkHandle_t; /* 0 is invalid handle */

typedef struct mediaSnkBuffer_s
{
	mediaBufferObject_t bufobj; /* For releasing buffer */
	void (*free)(mediaBufferObject_t bufobj);

	UINT32 bufflags;
	UINT8  *data;
	UINT32 length;
	UINT32 frame_index;
	struct timeval pts;
	struct timeval enc_time;
	struct timeval alloc_time;
} mediaSnkBuffer_t;

typedef struct {
	UINT64 bytes;
	UINT32 frames;
	UINT32 frames_dropped;
	UINT32 frames_late;
	int    avg_delay_time;
	struct timeval recent_pts;
} mediaSnkAVStatsInf_t;

typedef struct mediaSnkStats_s {
	mediaSnkAVStatsInf_t audio, video;
	int cache_time;
} mediaSnkStats_t;

/* \return 0: succeed; > 0: error code.*/
typedef UINT32 (*mediaSnkOnMsg_t)(mediaSnkHandle_t h, mediaSnkMsg_e msg, long param, void *user_data);

/**************************************************************************
 *                          D A T A    T Y P E S
 **************************************************************************/
typedef UINT32 (*pmediaRecPauseCB_t)(UINT32);
typedef void   (*pmediaRecSeamlessCB_t)(void);
typedef void   (*pmediaRecFileCloseCB_t)(UINT32, UINT32);
typedef void   (*pmediaRecFileChangedCB_t)(UINT32);
typedef void   (*pmediaRecHsModeCB_t)(UINT32, UINT32);
typedef UINT32 (*pmediaPlaySoundEffectCb_t)(UINT32 timeStamp, UINT8* pBuf,
	UINT32 sampleRate, UINT32 channelCnt, UINT32 sampleCnt, UINT32 sampleSize);




#if CUSTOMER_TAG_SUPPORT
/*
typedef enum {
	MEDIA_TAG_UPDT_TIME = 0,
	MEDIA_TAG_UPDT_GPS,
	MEDIA_TAG_UPDT_SENSOR,
	MEDIA_TAG_UPDT_EVT
} mediaVideoUsrDatUpdtType_e;
*/

typedef struct mediaVideoTagUserDataGet_s {
	UINT8* pData;
	UINT64 nDataSize;
	SINT64 nOffSet;
	UINT8  szFilename[256];
}	mediaVideoTagUserDataGet_t;

typedef struct mediaVideoTagUserData_s {
	UINT32	name;			/**< Name for user data type atom, by spec it's "udta" */
	UINT32	location;   	/**< In order to meet customer's request
								 0: default value, default location by spec, under moov atom
								 1: one level up to on par with ftyp atom */
	UINT32	update_mode; 	/**< How user data is updated
								 0: write all data into atom when close
								 (if moov periodic sync function enabled, the pData will be written to user box when mov
                                    					   file created)
								 1: write when mux control is called
								 When setting 0 or 1 we don't care about data just write.
								 Below Not Implemened Yet:
								 2: Customzied for RDV-1000 to support 4 kinds of user atom
								 3: a special mode for MOOV periodic sync function
							 */
	UINT8*	pData;			/**< Used when update mode is 0*/
	UINT32	nDataSize;		/**< Used when update mode is 0*/
	UINT32	pCallBack;		/**< Used when there is something host need to pass to mux task when done */
} mediaVideoTagUserData_t;

typedef struct mediaVideoTags_s {
	/**
	  *		MOV specific tags
	  */
	UINT32  major_brand;
	UINT32  minor_version;
	UINT32* compatible_brands;
	UINT32  compatible_brands_cnt;
	mediaVideoTagUserData_t udta_atom;          /**< udta customer user data */
	UINT16  pref_volume;
	UINT16  video_track_lang_id;                /**< video track language ID */
	UINT16  audio_track_lang_id;                /**< audio track languaed ID */
	UINT16  audio_has_edts;                     /**< audio track has edts box */
	UINT32  vendor;
	UINT32  component_manuf;
	UINT32  data_ref_type;
	UINT32  movie_time_scale;                   /**< movie time scale */
	UINT32  tkhd_flags;
	UINT8   stbl_order;                         /**< put stss table before stsc when set */
	UINT8   no_udta;                            /**< no write udta box */
	UINT8   no_freebox;                         /**< no free box generated*/
	UINT8   moov_first;                         /**< moov atom comes before mdat atom */

	/** Video Related Fields */
	UINT32  time_scale;                         /**< video track media time scale */
	UINT32  video_track_matrix[9];
	UINT16  gfx_mode;
	UINT16  opcolor[3];
	UINT16  video_dscp_ver;
	UINT16  video_dscp_rev_lvl;
	UINT8   video_compressor_name[32];          /**< A 32-byte Pascal string */
	UINT32  video_slice_type_offset;            /**< H264 specific */
	UINT32  temporal_quality;
	UINT32  spatial_quality;
	UINT32  video_sample_per_chunk;             /**< 4 bytes integer indicate how much samples stored per chunk*/
	UINT8   AVCProfileIndication;
	UINT8   profile_compatibility;
	UINT8   AVCLevelIndication;
	UINT8   video_has_color_param;              /**< boolean, TRUE if host wants to add color parameter atom to video sample description atom*/
	UINT8   video_has_fiel_box;                 /**< boolean, TRUE if host wants to add field handling atom to video sample description atom */
	UINT8   video_handler_minf_present;         /**< boolean, video handler presence */
	UINT8   video_handler_comp_name[64];        /**< UTF8 null terminated string in MP4, counted string in MOV */
	UINT8   video_handler_ref_comp_name[64];    /**< MP4 doesn't have it, counted string in MOV */
	UINT32 pasp_hSpacing;	/*pixel aspect ration setting*/
	UINT32 pasp_vSpacing;	
	
	/** Audio Related Fields */
	UINT16  audio_dscp_ver;
	UINT16  audio_compression_id;
	UINT32  audio_stsz_sample_size;
	UINT16  audio_ES_ID;
	UINT16  audio_streamPrority;
	UINT32  audio_bufferSizeDB;
	UINT32  audio_sample_per_chunk;             /**< 4 bytes integer indicate how much samples stored per chunk*/
	UINT32  audio_handler_minf_present;         /**< boolean, audio handler presence */
	UINT8   audio_handler_comp_name[64];        /**< UTF8 null terminated string in MP4, counted string in MOV */
	UINT8   audio_handler_ref_comp_name[64];    /**< MP4 doesn't have it, counted string in MOV */
	UINT8   audio_mdhd_same_duration;           /**< Whether audio duration is set to the same as video */
	UINT8   audio_chunk_first;
	UINT8   av_chunk_align; /**< Set to 1 for Sony a/v chunk interleave rule,
							  * audio chunk start time should be within video chunk GOP start frame interval.
							  * audio chunk will have variable length */

	/** UUID */
	UINT8   uuid_present;                       /**< TRUE if uuid box is needed */

						  
} mediaVideoTags_t;

typedef void (*pmediaRecUsrDataCB_t)(void);
typedef void (*pmediaPlayUsrDataCB_t)(UINT8** pBuf, UINT32* pSize);
#endif


typedef struct {
	UINT32 (*fpOpen)(void);
	UINT32 (*fpClose)(void);
	UINT32 (*fpVideoFrameGot)(UINT8 *pbuf, UINT32 size, struct timeval stamp,
		void *type);
	UINT32 (*fpAudioFrameGot)(UINT8 *pbuf, UINT32 size, struct timeval stamp,
		void *rsvd);
} mediaRecScCustomCbList_t;

typedef struct {
	UINT8* pData;
	UINT32 size;
} mediaRecScCustomData_t;

typedef struct {
	mediaRecScCustomData_t adts;	/*AAC only*/
} mediaRecScCustomAudBlkGotCbInfo_t;

typedef struct {
	UINT32 attrId; /* MEDIA_ATTR_XXX, not 2nd attributes. -1 means end */
	UINT32 val;
} mediaAttrVal_t;

typedef struct mediaStreamAttr_s {
	/* general */
	UINT16 width;
	UINT16 height;
	UINT32 codec;
	UINT32 bitrateAvg;
	UINT32 bitrateMax;
	UINT32 bitrateMin;
	UINT32 fpsSave;
	/* record config */
	UINT32 vlcBufSize;
	UINT32 vlcBufCnt;
	UINT32 fpsInput;
	/* Variable Frame Rrate Control */
	UINT32 vfrcBitRate;
	UINT32 vfrcAttr;
	/* BRC type */
	UINT32 brcType;
#if 0
	/* h264 */
	UINT8  h264EntropyMode; /* 0 for CABAC, 1 for CAVLC */
	UINT8  h264Gop;         /* 0x00 for IBBP, 0x10 for IPPP, 0x20 for IIII */
	UINT16 h264GopNo;
	UINT32 h264Qp;
	UINT8  h264Profile;     /* 0: base/main, 1: high profile */
	UINT8  h264IntraMode;   /* 0: I4+I16, 1: I8+I16, 2: I4+I8 */
	UINT8  h264Interlace;   /* 0: progressive, 1: interlace mode */
	UINT8  h264SliceNo;     /* 0~1: normal, 2~4: multi-slices */
	UINT32 h264VuiTag;      /* b[0]: video_signal_type_present_flag */
	UINT32 h264IdrInv;      /* 0:first I only, 1:every I frame, 2:two I frame, ... */
	UINT8  h264AudSei;      /* 0: Not Available, 1: Available */
	UINT8  h264SliceTypeOff; /* Offset of slice type, value may be 0, 5 */
	UINT8  h264ColorParam;  /* 0: BT.709, 1: BT.601 */
#endif
	/* mjpg */
	UINT8  mjpgInitQ;
	UINT8  mjpgMaxQ;
	UINT8  mjpgMinQ;
} mediaStreamAttr_t;

/* Dual-Stream, SPCA6330 */
typedef enum {
	MEDIA_REC_DUALSTREAM_PAUSE,
	MEDIA_REC_DUALSTREAM_RESUME
} mediaRecDualStreamCtrlCode_e;

typedef enum {
	MEDIA_REC_DUALSTREAM_OFF,
	MEDIA_REC_DUALSTREAM_ON_NORMAL,
	MEDIA_REC_DUALSTREAM_ON_EXT,
} mediaRecDualStreamState_e;

typedef struct {
	UINT32 dataFmt; /* 0 : EXIF_YUV420, 1: EXIF_YUV422 */
	UINT32 width, height;
	UINT32 vlcPos;
	UINT32 vlcSize;
	UINT32 qIdx;
	UINT8  qtable[128];
} mediaRecDualStreamJpegAttr_t;

typedef void (*mediaRecDualStreamCbFunc)(mediaSrcBuffer_t *buf
	, mediaRecDualStreamJpegAttr_t const *jpegattr
	, UINT32 udata);

typedef struct mediaRecDualStreamClient_s {
	mediaRecDualStreamCbFunc cbfunc;
	UINT32 udata;
	/* Frame rate control. Don't modify the following members directly. */
	UINT32 frc_frmrate;   /* 0: no frame-rate control */
	UINT32 frc_shiftbits; /* 0 <==> 29 */
	UINT32 frc_maskbits;
	struct mediaRecDualStreamClient_s *next;
} mediaRecDualStreamClient_t;

typedef struct {
	UINT8* pData;
	UINT32 size;
} mediaPlayAudFrmData_t;

typedef struct {
	mediaPlayAudFrmData_t adts;	/*AAC only*/
} mediaPlayAudFrmDataCbInfo_t;

/* Dual-Stream End, SPCA6330 */

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S
 **************************************************************************/

#define MEDIA_REC_FLIP_ROTATE_MODE		((mediaRecCfgGet(MEDIA_REC_FLIP_ROTATE)>> 8) & 0xFF)
#define MEDIA_REC_FLIP_ROTATE_ANGLE		(mediaRecCfgGet(MEDIA_REC_FLIP_ROTATE) & 0xFF)


#endif /* _MEDIA_DEF_H_ */

