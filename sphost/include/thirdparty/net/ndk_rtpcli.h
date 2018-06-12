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

/* RTP Media Client */

#ifndef __NDK_MEDIACLI_RTP_H__
#define __NDK_MEDIACLI_RTP_H__

#include <ndk_mediacli.h>

#ifdef __cplusplus
extern "C" {
#endif

/* RTP Media Client Options */

enum {
	/* Options with boolean value: 0(default) or 1. */
	NDK_MEDIACLI_RTP_OPT_TCP         = 0, /* 0: UDP; 1: TCP. */
	NDK_MEDIACLI_RTP_OPT_AUDIO_ONLY  = 1, /* 1: Audio Only. Cannot coexist with option NDK_RTPCLI_OPT_VIDEO_ONLY. */
	NDK_MEDIACLI_RTP_OPT_VIDEO_ONLY  = 2, /* 1: Video Only. */

	/* Options with integer value. */
	NDK_MEDIACLI_RTP_OPT_TIMEOUT     = 0x1000, /* Data arrival timeout in milli-seconds. 0: disable timeout checking. Default 10000. */
	NDK_MEDIACLI_RTP_OPT_CACHETIME   = 0x1001, /* Cache time for audio/video receiving buffer. Default: 500. */
	NDK_MEDIACLI_RTP_OPT_AVSYNCTIME  = 0x1002, /* Unit: ms. */
	NDK_MEDIACLI_RTP_OPT_DEBUG       = 0x1003,

	/* Set value for the SP5K media attributes.
	   Real media attribute id = value - NDK_MEDIACLI_OPT_MEDIA_ATTR.
	   e.g. To set SP5K_MEDIA_ATTR_H264_GOP_NO to 120, the option id is:
	     NDK_MEDIACLI_RTP_OPT_MEDIA_ATTR + SP5K_MEDIA_ATTR_H264_GOP_NO
	 */
	NDK_MEDIACLI_RTP_OPT_MEDIA_ATTR  = 0x2000
};

/*
 * RTP Media Client Message Code
 */
typedef enum {
	// State is changed
	NDK_MEDIACLI_RTP_MSG_STARTED     = 0x0000,
	NDK_MEDIACLI_RTP_MSG_STOPPED,

	// Unrecoverable errors
	NDK_MEDIACLI_RTP_MSG_EGENERIC    = 0x0100,
	NDK_MEDIACLI_RTP_MSG_ECONN,
	NDK_MEDIACLI_RTP_MSG_TIMEOUT
} NDKMediaCliRtpMsg;

typedef void NDKMediaCliRtpMsgHandler(NDKMediaCliHandle h, void *udata, NDKMediaCliRtpMsg msg, long param);

/* \function ndk_mediacli_rtp_init
 * \brief Create and initialize a RTP media client object.
 */
int ndk_mediacli_rtp_init(
	NDKMediaCliHandle *handle,
	const char *source_url,
	NDKMediaCliAVSink *avsink,
	NDKMediaCliRtpMsgHandler *msg_handler,
	void *msg_udata,
	int optnum,
	NDKMediaCliOpt *opts);

#ifdef __cplusplus
}
#endif

#endif

