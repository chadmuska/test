/**************************************************************************
 *
 *	 Copyright (c) 2009-2016 by iCatch Technology, Inc.
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
 *									  *
 **************************************************************************/
#ifndef __NDK_MEDIA_API_H__
#define __NDK_MEDIA_API_H__

#include <sys/time.h>
#include <sp5k_os_api.h>
#include <sp5k_media_api.h>

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************\
 * Constants
\**************************************************************************/
/* Attributes */
enum {
	NDK_MEDIA_ATTR_VIDEO_CODEC = 0,
	NDK_MEDIA_ATTR_DURATION,		/* milli-seconds */
	NDK_MEDIA_ATTR_WIDTH,
	NDK_MEDIA_ATTR_HEIGHT,
	NDK_MEDIA_ATTR_VIDEO_BRC_TYPE,
	NDK_MEDIA_ATTR_VIDEO_AVG_BITRATE,
	NDK_MEDIA_ATTR_VIDEO_FRAME_RATE,	/* float* */
	NDK_MEDIA_ATTR_H264_GOP_NO,

	NDK_MEDIA_ATTR_AUDIO_CODEC = 1000,
	NDK_MEDIA_ATTR_AUDIO_SAMPLE_RATE,
	NDK_MEDIA_ATTR_AUDIO_SAMPLE_BITS,
	NDK_MEDIA_ATTR_AUDIO_CHANNELS,
	NDK_MEDIA_ATTR_AUDIO_AVG_BITRATE,
};

/* Codec */
enum {
	NDK_MEDIA_CODEC_UNKNOWN = 0,

	NDK_MEDIA_CODEC_VIDEO_START = 1000,
	NDK_MEDIA_CODEC_H264 = NDK_MEDIA_CODEC_VIDEO_START + MEDIA_VIDEO_H264,
	NDK_MEDIA_CODEC_MJPG = NDK_MEDIA_CODEC_VIDEO_START + MEDIA_VIDEO_MJPG,
	NDK_MEDIA_CODEC_HEVC = NDK_MEDIA_CODEC_VIDEO_START + MEDIA_VIDEO_HEVC,

	NDK_MEDIA_CODEC_AUDIO_START = 2000,
	NDK_MEDIA_CODEC_PCM	= NDK_MEDIA_CODEC_AUDIO_START + MEDIA_AUDIO_PCM,
	NDK_MEDIA_CODEC_AAC	= NDK_MEDIA_CODEC_AUDIO_START + MEDIA_AUDIO_AAC,
	NDK_MEDIA_CODEC_MULAW,
	NDK_MEDIA_CODEC_ALAW,
	NDK_MEDIA_CODEC_IMA_ADPCM,
};

/* Codec Convert */
#define NDK_AUDC_FROM_FW(codec)		((codec) + NDK_MEDIA_CODEC_AUDIO_START)
#define NDK_VIDC_FROM_FW(codec)		((codec) + NDK_MEDIA_CODEC_VIDEO_START)

static inline UINT32 NDK_CODEC_TO_FW(UINT32 codec) { return codec - (codec > 2000 ? NDK_MEDIA_CODEC_AUDIO_START : NDK_MEDIA_CODEC_VIDEO_START); }

/* Media Buffer Flags*/
enum {
	/* bit[0:3]: Elementary Stream type */
	NDK_MEDIA_BUF_ES_UNKNOW		= 0,

	NDK_MEDIA_BUF_ES_VIDEO		= 1,
	NDK_MEDIA_BUF_ES_AUDIO		= 2,
	NDK_MEDIA_BUF_ES_TEXT		= 3,

	NDK_MEDIA_BUF_ES_NR		= 3,
	NDK_MEDIA_BUF_ES_BITMASK	= 0x0F,

	/* bit[4]: Key-frame flag */
	NDK_MEDIA_BUF_KEY_FRAME		= 0x01 << 4,
	NDK_MEDIA_BUF_PARAM_FRAME	= 0x01 << 5,
};

#define NDK_MEDIA_BUF_ES_TYPE(flags)	((flags)&NDK_MEDIA_BUF_ES_BITMASK)

/**************************************************************************\
 * Stream Source
\**************************************************************************/
/* Control Codes */
typedef enum {
	NDK_SS_CTRL_PAUSE		= 0,	/* no arg */
	NDK_SS_CTRL_RESUME,			/* no arg */
	NDK_SS_CTRL_SEEK,			/* arg: double seconds */
	NDK_SS_CTRL_DUMP,			/* no arg */
} NDKStreamSrcCtrlCode;

typedef struct StreamBufObj {
	void *sbo_data;
	void (*sbo_ref)(void *data);
	void (*sbo_unref)(void *data);
} NDKStreamBufObj;

typedef struct {
	NDKStreamBufObj	bufobj;			/* the internal buffer struct */
	unsigned int	bufflags;
	unsigned char	*data;
	unsigned int	length;
	unsigned int	duration; /* in ms */
	struct timeval	pts; /* Presentation timestamp. */
	struct timeval	enctime; /* Encoding time. */
} NDKStreamSrcBuf;

typedef struct StreamSrc NDKStreamSrc;
typedef void (*NDKStreamSrcBufHandler)(NDKStreamSrcBuf *buf, void* udata);

struct StreamSrc {
	void *ss_priv;
	/* \brief Open the StreamSource. Support reopen after close.
	 * \return 0: succeed; <0: error code.
	 */
	int  (*ss_open)(NDKStreamSrc *ss);
	/* \brief Start receive data. Must be called after ss_open.
	 * \param handler: A callback function sent to the StreamSource to handle the buffer.
	 * \param udata: user data for the handler.
	  */
	void (*ss_start_receive)(NDKStreamSrc *ss, NDKStreamSrcBufHandler handler, void *udata);
	/* \brief Close the StreamSource
	  */
	void (*ss_close)(NDKStreamSrc *ss);
	/* \brief Free the StreamSource data structure
	 */
	void (*ss_free)(NDKStreamSrc *ss);
	/* \brief Is the StreamSource open or not.
	 * \return TRUE: succeed; FALSE: failed
	 */
	BOOL (*ss_is_open)(NDKStreamSrc *ss);
	/* \brief Return the media attribute.
	 * \param attr: Media attribute ID. See the enumerate NDK_MEDIA_ATTR_XXX.
	 * \return 0: succeed; <0: error code.
	 */
	int  (*ss_get_attribute)(NDKStreamSrc *ss, unsigned int attr, void *val);
	/* \brief Is the source ended or not. It is used for file streaming.
	 * \return TRUE: source is end; FALSE: not end.
	 */
	BOOL (*ss_end_of_source)(NDKStreamSrc *ss);
	/* \brief Control the StreamSource.
	 * \prarm code: Control code. See the enumerate NDK_SS_CTRL_XXX.
	 * \return 0: succeed; <0: error code.
	 */
	int  (*ss_control)(NDKStreamSrc *ss, NDKStreamSrcCtrlCode code, void *param);
};

/**************************************************************************\
 * Stream Sink
\**************************************************************************/
/* TODO: Added in the future. */
typedef struct StreamSink {
} NDKStreamSink;

/**************************************************************************\
 * Stream Creator
\**************************************************************************/
typedef struct StreamSrcCreator {
	void *udata;
	NDKStreamSrc* (*create)(const char *stream_name, void *udata);
} NDKStreamSrcCreator;

typedef struct StreamSinkCreator {
	void *udata;
	NDKStreamSink* (*create)(const char *stream_name, void *udata);
} NDKStreamSinkCreator;

/**************************************************************************\
 * API functions/macros
\**************************************************************************/

#define ndk_stream_buf_init(sbobj)	memset(&(sbobj), 0, sizeof(sbobj))
#define ndk_stream_buf_ref(sbobj)	do { if ((sbobj).sbo_data) { (sbobj).sbo_ref((sbobj).sbo_data); }} while(0)
#define ndk_stream_buf_unref(sbobj)	do { if ((sbobj).sbo_data) { (sbobj).sbo_unref((sbobj).sbo_data); }} while(0)

#ifdef __cplusplus
}
#endif

#endif /* __NDK_MEDIA_API_H__ */
