/**************************************************************************
 *
 *       Copyright (c) 2012 by iCatch Technology, Inc.
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
#ifndef __NDK_STREAMING_H__
#define __NDK_STREAMING_H__

#include <sys/time.h>
#include <ndk_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************************
 *                         C O S T A N T S
 *******************************************************************************/
/* \enum NDK Events
 */
enum {
	/* \brief Data is RTSP URL string(const char*).
	 * Return: 1, Accept this RTSP request; 0, refuse.
	 */
	NDK_ST_EVT_RTSP_REQUEST          = 0,

	/* \brief Sent before the streaming is about to start. The encoder or meida source
	 *   should be prepared to generate audio/video bistream after receiving this event.
	 * \param (none).
	 * \return TRUE: start streaming; FALSE: abort streaming.
	*/
	NDK_ST_EVT_ON_STARTED,

	/* \brief Sent after the streaming is stopped. The encoder or media source can be
	 * closed after receiving this event.
	 * \param (none).
	 * \return ignored.
	 */
	NDK_ST_EVT_ON_STOPPED,

	/* \brief Send after a frame is dropped.
	 * \param frame index.
	 *  video index = idx
	 *  audio index = idx | 0x10000000,
	 * \return ignored.
	 */
	NDK_ST_EVT_FRAME_DROPPED,

	/* \brief Send when the Wi-Fi driver tx buffer is cleared by media server.
	 * \param (none).
	 * \return ignored.
	 */
	NDK_ST_EVT_TXBUFQ_CLEARED,
};

/* \enum Streaming Mode
 */
typedef enum {
	/* Dual-Streaming */
	NDK_ST_DUAL_H264JPEG             = 0, /* One JPEG and one H264. */
	NDK_ST_DUAL_STREAMING            = 1, /* V33 only. The meaming of dual here is dualstream(jpeg). */

	/* Mono-streaming */
	NDK_ST_MONO_H264                 = 2,
	NDK_ST_MONO_JPEG                 = 3,

	/* Support one or two H264/MJPG streaming. V35 Only. The streaming type is determined by URL or
	 * NDK_ST_ATTR_STREAMING_TYPE. Default, map of URL to streaming-type:
	 * H264S0 -> h264, stream_id = 0;
	 * H264S1 -> h264, stream_id = 1;
	 * JPEGS0 -> jpeg, stream_id = 0;
	 * JPEGS1 -> jpeg, stream_id = 1;
	 * For other URL, like live0/live1, user must provide the streaming-type by NDK_ST_ATTR_STREAMING_TYPE.
	 */
	NDK_ST_MULTI_STREAM              = 4,

	NDK_ST_MODE_NUM
} NDKStMode;

/* \enum Live Streaming Type.
 */
enum {
	NDK_ST_TYPE_INVALID              = 0, /* Invalid Streaming Type. */
	NDK_ST_TYPE_DEFAULT              = 1, /* Judged by URL */
	NDK_ST_TYPE_FILE                 = 2,
	NDK_ST_TYPE_AUDIO                = 3,
	NDK_ST_TYPE_JPEG                 = 4,
	NDK_ST_TYPE_JPEGS0               = NDK_ST_TYPE_JPEG,
	NDK_ST_TYPE_H264S0               = 5,
	NDK_ST_TYPE_H264S1               = 6,
	NDK_ST_TYPE_JPEGS1               = 7,

	NDK_ST_TYPE_NUM
};

/* \brief Values for audio on */
enum {
	NDK_ST_AUDIO_OFF                 = 0,
	NDK_ST_AUDIO_ON_NORMAL           = 0x01 << 0,  /* Marker per RTP packet */
	NDK_ST_AUDIO_ON_MARKER_PER_FRAME = 0x01 << 8,
	NDK_ST_AUDIO_ON_NOSWAP           = 0x01 << 9,
};

/* \brief Streaming Flags */
enum {
	NDK_ST_FLG_TUTK                  = 1 << 0,
};

/* Dual-stream control code. */
typedef enum {
	NDK_DUALSTREAM_PAUSE             = 0, /* Pause dual-streaming. No parameter */
	NDK_DUALSTREAM_RESUME,                /* Resume dual-streaming. No parameter */
	NDK_DUALSTREAM_MAXQ_SET,
	NDK_DUALSTREAM_MINQ_SET,
	NDK_DUALSTREAM_BITRATE_SET,
} NDKDualStreamCtrlCode;

/* attribute code. */
enum {
	/* \brief Timeout to resend last frame if there is no new frame for sending.
	 * Default: 0. Min. 30 ms. 0: don't resend. Unit: ms.
	 */
	NDK_ST_ATTR_TIME_TO_RESEND_FRAME = 0,

	/* \brief Lag time to drop frame.
	 * Default: 150 ms. Min. 60 ms. 0: don't drop frame.
	 */
	NDK_ST_ATTR_TIME_TO_DROP_FRAME,

	/* \brief Network loading measure value.
	 * If network loading is too high, media server will stop sending frames until it
	 * becomes low again.
	 * Default: 80. Set to 0 to disable the network loading checking.
	 */
	NDK_ST_ATTR_NET_LOADING_HIGH,

	/* \brief Clear the TX buffer queue of the WiFi driver .
	 * If the network loading has become high for some time, the media server will
	 * clear the dirver's tx buffer for the (media) data in the buffer may outmode.
	 * Unit: milli-seconds.
	 * Default: 0, this feature is disabled.
	 */
	NDK_ST_ATTR_TIME_CLR_TXBUFQ,

	/* \brief Streaming frame rate. Default: 0. Max.: 30.
	 * The frame rate control is implemented by dropping frames. It is only valid for
	 * Motion-JPEG streaming. For H.264, frame cannot be dropped. Otherwise. Otherwise, the streaming
	 * client may decode fail.
	 * The frame rate can be: 6, 10, 15, 20 or 30.
	 */
	NDK_ST_ATTR_FRAME_RATE,

	/* \brief Video Dynamic BitRate Control
	 * If network congestion is detected, the video bitrate will be reduced automatically.
	 * This value defines the minimum bitrate to be reduced to in percentage.
	 * If set to 0, the dynamic bitrate control is disabled. Maximum value is 100.
	 * Default is 50.
	 */
	NDK_ST_ATTR_VDBRC_MIN,

	/* \brief Video Dynamic BitRate Control
	 * Time of restoring bitrate from min to 100% in milliseconds.
	 * Default value: 1000.
	 */
	NDK_ST_ATTR_VDBRC_RESTORE_TIME,

	/* \brief Audio and Video FIFO size.
	 * Default audio FIFO size is 24, and video is 32.
	 * Note, for H264 streaming, FIFO size must > GOP size.
	 */
	NDK_ST_ATTR_AUDIO_FIFO_SIZE,
	NDK_ST_ATTR_VIDEO_FIFO_SIZE,

	/* \brief Interval time to reset audio and video frame PTS calculation.
	 * Default 0: Disable this feature.
	 */
	NDK_ST_ATTR_PTS_RESET_TIME,

	/* \brief Streaming Type. See NDK_ST_TYPE_XXX */
	NDK_ST_ATTR_STREAMING_TYPE,

	/* \brief Maximum RTP payload size. This controls UDP RTP packet size. Not affect TCP RTP packet. */
	NDK_ST_ATTR_RTP_PAYLOAD_SIZE,

	NDK_ST_ATTR_MAX
};

typedef UINT32 NDKStAttr;

/* Constants for Custom Media Source */
typedef enum {
	NDK_ST_MEDIASRC_FILE             = 0,

	NDK_ST_MEDIASRC_NUM
} NDKStMediaSrcType;

enum {
	NDK_ST_MEDIABUFFER_UNKNOW        = 0,
	NDK_ST_MEDIABUFFER_VIDEO         = 1,
	NDK_ST_MEDIABUFFER_AUDIO         = 2,
};

/********************************************************************************
 *                         D A T A  T Y P E S
 *******************************************************************************/
/* \brief Streaming event handler.
   \return 0: FAILED; > 0: SUCCESS and Return code.
 */
typedef BOOL (*NDKStEvtHandler)(UINT32 event, UINT32 data);

/* Streaming Server Configure */
typedef struct NDKStCfg
{
	const char*     root_dir;       /* root_dir: Root directory for local file streaming. */
	UINT16          port;           /* RTSP listening port. */
	NDKStMode       st_mode;
	UINT32          audio_on;
	UINT32          keepalive_secs; /* Keep alive time in seconds for streaming client. 0: no keep-alive check. */
	NDKStEvtHandler	evt_handler;
	UINT32          flags;
} NDKStCfg;

/* Definitions for Custom Media Source */
typedef void* NDKStMediaSrcHandle;
typedef void* NDKStMediaBufferObject;

typedef struct NDKStMediaBuffer
{
	/* The internal buffer object defined by customer for this media-buffer. */
	NDKStMediaBufferObject bufobj;
	UINT32 f_buftype    : 2, /* NDK_ST_MEDIABUFFER_AUDIO, NDK_ST_MEDIABUFFER_VIDEO */
	       f_keyframe   : 1, /* For JPEG, always 1. For H.264, set to 1 for I-frame; set to 0 for P-frame and B-frame. */
	       f_paramframe : 1; /* Set to 1 for H.264 SPS and PPS frame. Otherwise, set to 0 */
	/* The bitstream data pointer */
	UINT8  *data;
	/* Length of the bitstream data. */
	UINT32 length;
	/* Frame duration in milli-seconds. */
	UINT32 duration;
	/* Presentation timestampe. */
	struct timeval pts;
} NDKStMediaBuffer;

/* Callback function provided by NDK for pushing buffer to NDK for RPT streaming.
 * \param buf: media buffer pointer.
 * \return void. If pushing failed, NDK will discard this buffer.
 */
typedef void (*NDKStMediaSrcPushBufferFunc)(NDKStMediaBuffer *buf, void *param);

typedef struct NDKStMediaSrcCallback
{
	/* \brief Open the custom media-source for streaming.
	 * \param push_func: a callback function for pushing buffers to NDK.
	 * \param push_param: argument of the pushing function.
	 * \param src_arg: media-source argument. Depending on media-source types:
	 *       NDK_ST_MEDIASRC_FILE: const char *filename
	 * \return NULL: failed. Othewise a handle represents the custom media-source object.
	 */
	NDKStMediaSrcHandle (*open)(NDKStMediaSrcPushBufferFunc push_func, void *push_param, void *src_arg, ...);

	/* \brief Close the custom media-source.
	 * \param h: handle of the custom media-source object
	 */
	void   (*close) (NDKStMediaSrcHandle h);
	/* \brief Tell the custom media-source object to pause pushing buffer to NDK.
	 * \param h: handle of the custom media-source object
	 * \return SUCCEED or FAIL.
	 */
	UINT32 (*pause) (NDKStMediaSrcHandle h);
	/* \brief Tell the custom media-source object to continue pushing buffer to NDK.
	 * \param h: handle of the custom media-source object
	 * \return SUCCEED or FAIL.
	 */
	UINT32 (*resume)(NDKStMediaSrcHandle h);
	/* \brief Test if the end of source is met.
	 * \param h: handle of the custom media-source object
	 * \return TRUE: at the end of source. FALSE: not.
	 */
	BOOL   (*end_of_source)(NDKStMediaSrcHandle h);
	/* \brief Seek media source to position.
	 * \param h: handle of the custom media-source object
	 * \param position: unit is milli-seconds.
	 * \return SUCCEED or FAIL.
	 */
	/* Return: 0, succeed; 1, failed; */
	UINT32 (*seek_to)(NDKStMediaSrcHandle h, UINT32 position/*in milli-seconds */);
	/* \breif Get audio/video attributes. The attribute symbols are same with the one defined in enum mediaAttr_e
	   Below are the attributes used by NDK now:
	    SP5K_MEDIA_ATTR_WIDTH
	    SP5K_MEDIA_ATTR_HEIGHT
	    SP5K_MEDIA_ATTR_DURATION
	    SP5K_MEDIA_ATTR_VIDEO_FRAME_RATE
	    SP5K_MEDIA_ATTR_VIDEO_CODEC
	    SP5K_MEDIA_ATTR_AUDIO_CODEC
	    SP5K_MEDIA_ATTR_AUDIO_SAMPLE_BITS
	    SP5K_MEDIA_ATTR_AUDIO_SAMPLE_RATE
	    SP5K_MEDIA_ATTR_AUDIO_CHANNELS
	 */
	UINT32 (*get_attribute)(NDKStMediaSrcHandle h, UINT32 attr, UINT32 *val);
	/* Free the buffer object. */
	void   (*free_buffer_object)(NDKStMediaBufferObject obj);
} NDKStMediaSrcCallback;

/********************************************************************************
 *                         F U N C A T I O N S
 *******************************************************************************/
int ndk_st_global_init(const char *options);

/* \brief Start streaming server
 * \param root_dir: Root directory for local file streaming.
 * \param port: RTSP listening port.
 * \return 0 - succeed; others - failed.
 * \note All streaming files must be in root_dir directory.
 *       Length of root_dir string must be less than 63.
 *       The streaming parameters will be reset to default values in this function. So, change
 *       parameters after calling of this function.
 */
int  ndk_st_start_server(NDKStCfg *cfg);

/* \brief Stop streaming server */
void ndk_st_stop_server();

/* \brief Is server started. */
BOOL ndk_st_is_server_started();

/* \brief Set/Get media server attributes. Must be called after the server is started.
 * \param attr: attribute code.
 * \param value: attribute value.
 * \return 0: succeed or -1: failed.
 */
int  ndk_st_set_attribute(NDKStAttr attr, long value);
int  ndk_st_get_attribute(NDKStAttr attr, long *value);

/* \brief Close existing streaming connections. */
void ndk_st_close_streaming();

/* \brief Test if the streaming is already started */
BOOL ndk_st_is_streaming_started();
BOOL ndk_st_is_dualstream_jpeg();
UINT32 ndk_st_is_audio_on();
NDKStMode ndk_st_get_mode();

int ndk_st_register_mediasrc(NDKStMediaSrcType type, NDKStMediaSrcCallback *cb);

#ifdef __cplusplus
}
#endif

#endif
