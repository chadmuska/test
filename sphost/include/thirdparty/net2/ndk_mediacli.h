/**************************************************************************
 *
 *       Copyright (c) 2014 by iCatch Technology, Inc.
 *
 *  This software is copyrighted by and is the property of iCatch Technology,
 *  Inc.. All rights are reserved by iCatch Technology, Inc..
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
 **************************************************************************/

#ifndef __NDK_MEDIACLI_H__
#define __NDK_MEDIACLI_H__

#include <stdarg.h>
#include <ndk_types.h>
#include <media_def.h>
#include <ndk/misc.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * Forward Reference
 *****************************************************************************/
struct NDKMediaCliAVSink;

/******************************************************************************
 * Media Client
 *****************************************************************************/
/*
 * Option ID
 */
enum {
	NDK_MEDIACLI_OPT_STARTTIME = 0, /* Value Type: double. */

	NDK_MEDIACLI_OPT_NUM
};

/*
 * Types
 */
struct NDKMediaCli
{
	void *priv_data;
	int  (*mcli_start)  (struct NDKMediaCli *cli, NDKVaOptHandle opts);
	int  (*mcli_stop)   (struct NDKMediaCli *cli);
	void (*mcli_destroy)(struct NDKMediaCli *cli);
};

typedef struct tagMediaCliHandle {} *NDKMediaCliHandle;

/*
 * API Functions
 */
int  ndk_mediacli_start(NDKMediaCliHandle h, NDKVaOptHandle opts);
int  ndk_mediacli_stop(NDKMediaCliHandle h);
void ndk_mediacli_destroy(NDKMediaCliHandle h);

/******************************************************************************
 * Media Client AV Sink
 *****************************************************************************/
/*
 * Control Code
 */
/* Statistics                                                          Arguments*/
#define NDK_MEDIACLI_AVSNK_CTRL_GET_STATS    MEDIA_SNK_CTRL_GET_STATS  /* UINT32 num, UINT32 *stats */

/*
 * Message ID
 * AV Sink can send message to media-client to pause or resume the stream.
 */
#define NDK_MEDIACLI_AVSNK_MSG_PAUSE         MEDIA_SNK_MSG_PAUSE
#define NDK_MEDIACLI_AVSNK_MSG_RESUME        MEDIA_SNK_MSG_RESUME

/*
 * Statistics
 */
enum {
	NDK_MEDIACLI_AVSNK_STAT_AUD_BYTES = 0,
	NDK_MEDIACLI_AVSNK_STAT_AUD_FRAME_TOTAL,
	NDK_MEDIACLI_AVSNK_STAT_AUD_FRAME_DROPPED,
	NDK_MEDIACLI_AVSNK_STAT_AUD_FRAME_LATE,
	NDK_MEDIACLI_AVSNK_STAT_AUD_AVG_DELAY_TIME,

	NDK_MEDIACLI_AVSNK_STAT_VID_BYTES,
	NDK_MEDIACLI_AVSNK_STAT_VID_FRAME_TOTAL,
	NDK_MEDIACLI_AVSNK_STAT_VID_FRAME_DROPPED,
	NDK_MEDIACLI_AVSNK_STAT_VID_FRAME_LATE,
	NDK_MEDIACLI_AVSNK_STAT_VID_AVG_DELAY_TIME,

	NDK_MEDIACLI_AVSNK_STAT_CACHE_TIME,

	// Number of statistics
	NDK_MEDIACLI_AVSNK_STAT_NUM
};

/*
 * Types
 */
typedef UINT32 (*NDKMediaCliOnSinkMsg)(UINT32 msgId, long param, void *udata);

typedef struct
{
	void   *bufobj;
	void   (*pf_free)(void *bufobj); /* Free buffer function. pf_free(bufobj); */

	UINT32 f_audio      : 1, /* 1: audio. */
	       f_video      : 1, /* 1: video. */
	       f_keyframe   : 1, /* For JPEG, always 1. For H.264, set to 1 for I-frame; set to 0 for P-frame and B-frame. */
	       f_paramframe : 1; /* Set to 1 for H.264 SPS and PPS frame. Otherwise, set to 0 */

	UINT8  *data;
	UINT32 length;
	struct timeval pts;
	struct timeval enc_time;
	struct timeval alloc_time;
	UINT32 frame_index;
} NDKMediaCliAVBuffer;

struct NDKMediaCliAVSink
{
	const  char *name;
	void   *priv_data;
	/* init => set-attribute => start => send-buffer => stop => destroy. */
	/*
	 * \param flags: NDK_MEDIACLI_AVSNK_FLG_LIVE, NDK_MEDIACLI_AVSNK_FLG_NOAUDSWAP
	 */
	UINT32 (*avsnk_init)(struct NDKMediaCliAVSink *avsink, NDKMediaCliOnSinkMsg onSinkMsg, void *udata, UINT32 flags);
	UINT32 (*avsnk_start)(struct NDKMediaCliAVSink *avsink);
	void   (*avsnk_stop)(struct NDKMediaCliAVSink *avsink);
	/* Delete the av sink object. This will be called by the media-client. */
	void   (*avsnk_destroy)(struct NDKMediaCliAVSink *avsink);
	/* The send-buffer callback function is responsible for delete the buffer object. */
	UINT32 (*avsnk_send_buffer)(struct NDKMediaCliAVSink *avsink, NDKMediaCliAVBuffer *buf);
	/* attr:
	 * NDK_MEDIA_ATTR_DURATION
	 * NDK_MEDIA_ATTR_WIDTH
	 * NDK_MEDIA_ATTR_HEIGHT
	 * NDK_MEDIA_ATTR_VIDEO_BRC_TYPE
	 * NDK_MEDIA_ATTR_VIDEO_CODEC
	 * NDK_MEDIA_ATTR_VIDEO_AVG_BITRATE
	 * NDK_MEDIA_ATTR_VIDEO_FRAME_RATE
	 * NDK_MEDIA_ATTR_H264_GOP_NO
	 * NDK_MEDIA_ATTR_AUDIO_CODEC
	 * NDK_MEDIA_ATTR_AUDIO_SAMPLE_RATE
	 * NDK_MEDIA_ATTR_AUDIO_SAMPLE_BITS
	 * NDK_MEDIA_ATTR_AUDIO_CHANNELS
	 * NDK_MEDIA_ATTR_AUDIO_AVG_BITRATE
	 */
	UINT32 (*avsnk_set_attr)(struct NDKMediaCliAVSink *avsink, UINT32 attr, UINT32 val);
	UINT32 (*avsnk_get_attr)(struct NDKMediaCliAVSink *avsink, UINT32 attr, UINT32 *val);
	UINT32 (*avsnk_control)(struct NDKMediaCliAVSink *avsink, UINT32 code, .../* depend on the control code. */);
};

typedef struct NDKMediaCliAVSink NDKMediaCliAVSink;

/*
 * API Functions
 */

/* Create a playback media sink*/
NDKMediaCliAVSink *ndk_mediacli_playback_sink_create();
NDKMediaCliAVSink *ndk_mediacli_nullsink_create();
NDKMediaCliAVSink *ndk_mediacli_filesink_create();

#ifdef __cplusplus
}
#endif

#endif
