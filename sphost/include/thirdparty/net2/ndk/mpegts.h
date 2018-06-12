/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2013 by iCatch Technology Co., Ltd.              *
 *                                                                        *
 *  This software is copyrighted by and is the property of Sunplus        *
 *  Technology Co., Ltd. All rights are reserved by Sunplus Technology    *
 *  Co., Ltd. This software may only be used in accordance with the       *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice MUST not be removed or modified without prior   *
 *  written consent of Sunplus Technology Co., Ltd.                       *
 *                                                                        *
 *  Sunplus Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  Sunplus Technology Co., Ltd.                                          *
 *  19, Innovation First Road, Science-Based Industrial Park,             *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 **************************************************************************/
#ifndef __NDK_MPEGTS_ENC_H__
#define __NDK_MPEGTS_ENC_H__

#include <middleware/media_def.h>
#include <ndk_types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	NDK_TS_ATTR_VIDEO_CODEC = 0,
	NDK_TS_ATTR_AUDIO_CODEC,
	NDK_TS_ATTR_AUDIO_SAMPLE_RATE,
	NDK_TS_ATTR_AUDIO_CHANNELS,

	NDK_TS_ATTR_NUM
} NDKTsEncAttrId;

/* \brief output callback function
 * \param seqno Sequence No. Start from 0.
 * \param buf buffer address. NULL means current segmentation is finished.
 * \param size buffer size.
 * \param udata user data.
 * \return TRUE: continue. FALSE: stop mpeg-ts encoding.
 */
typedef BOOL (*NDKTsEncOutFunc)(UINT seqno, UINT8 *buf, UINT size, void *udata);

typedef struct {} *NDKTsEncHandle;

typedef struct {
	UINT32 value[NDK_TS_ATTR_NUM];
} NDKTsEncAttrs;

/*
 * \param duration 0: no segment
 * \abort set abort to TRUE to stop the segmentation procedure
 * \return mpegts encoder handler.
 */
NDKTsEncHandle
ndk_tsenc_create(
	NDKTsEncAttrs   *attrs,
	UINT            duration,
	UINT            initial_seqno,
	NDKTsEncOutFunc ofunc,
	void            *udata
);

void ndk_tsenc_destroy(NDKTsEncHandle h);
/* Packetize AV Buffer */
int  ndk_tsenc_pack_buffer(NDKTsEncHandle h, mediaSrcBuffer_t *avbuf);

/* dst_file_format: NULL, don't saving. It is for testint the segment speed. */
int  ndk_tsenc_pack_file(const char *src_file, const char *dst_file_format, int duration);
int  ndk_tsenc_pack_live(NDKTsEncAttrs *attrs, UINT stream_id, UINT32 seconds, const char *filename);

#ifdef __cplusplus
}
#endif

#endif

