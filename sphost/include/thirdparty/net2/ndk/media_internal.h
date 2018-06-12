/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2012 by iCatch Technology Co., Ltd.             *
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
#ifndef __MEDIA_INTERNAL_H__
#define __MEDIA_INTERNAL_H__

#include <ndk_types.h>
#include <sys/time.h>

#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////////////
UINT32 mediaPlayThumbVlcGet(UINT8 *pbuf, UINT32 maxSize, UINT32 *psize, UINT32 *pwidth, UINT32 *pheight);

///////////////////////////////////////////////////////////////////////////////
// API functions got from media_api.h. Must be same with the original declarations.
#include <media_def.h>

/*
 * Media Source APIs
 */
void   mediaSrcGlobalInit(UINT32 flags);

/* If on_get_buffer is NULL, caller can use mediaSrcGetBuffer function to get buffer. */
mediaSrcHandle_t mediaSrcOpenFile(const char *filename
	, mediaSrcOnGotBuffer_t on_got_buffer
	, unsigned long user_data
	, UINT32 flags);

#if defined(SPCA6350)
  mediaSrcHandle_t mediaSrcOpenAudioStream(mediaSrcOnGotBuffer_t on_got_buffer, unsigned long user_data, UINT32 flags);
  mediaSrcHandle_t mediaSrcOpenVideoStream(mediaSrcOnGotBuffer_t on_got_buffer, unsigned long user_data, UINT32 flags, UINT32 streamId, UINT32 codec);
#elif defined(SPCA6330)
  mediaSrcHandle_t mediaSrcOpenRmx(UINT32 es_type, mediaSrcOnGotBuffer_t on_got_buffer, unsigned long user_data, UINT32 flags);
  mediaSrcHandle_t mediaSrcOpenLiveAudio(mediaSrcOnGotBuffer_t on_got_buffer, unsigned long user_data, UINT32 flags);
#else
  #error Wrong Platform
#endif

void   mediaSrcClose(mediaSrcHandle_t h);
/* Always return FAIL if on_got_buffer is not NULL. */
UINT32 mediaSrcGetBuffer(mediaSrcHandle_t h, mediaSrcBuffer_t *buf, UINT32 wait_ms);
BOOL   mediaSrcEndOfSource(mediaSrcHandle_t h);
UINT32 mediaSrcControl(mediaSrcHandle_t h, mediaSrcCtrlCode_e code, ...);
UINT32 mediaSrcGetAttribute(mediaSrcHandle_t h, UINT32 attr, UINT32 *val);
void   mediaSrcHoldBufferObject(mediaBufferObject_t bufobj);
void   mediaSrcReleaseBufferObject(mediaBufferObject_t bufobj);
UINT32 mediaSrcJoinBuffer(mediaSrcBuffer_t *dst, UINT32 n, mediaSrcBuffer_t *srcs[]);

#ifdef SPCA6350
mediaSnkHandle_t mediaSnkOpenDemux(mediaSnkOnMsg_t msg_handler, void *user_data, UINT32 flags);
void   mediaSnkClose(mediaSnkHandle_t h);
UINT32 mediaSnkSetAttribute(mediaSnkHandle_t h, UINT32 attr, UINT32 val);
UINT32 mediaSnkGetAttribute(mediaSnkHandle_t h, UINT32 attr, UINT32 *val);
UINT32 mediaSnkPushBuffer(mediaSnkHandle_t h, mediaSnkBuffer_t *buf);
UINT32 mediaSnkControl(mediaSnkHandle_t h, mediaSnkCtrlCode_e code, ...);

UINT32 mediaAttrIdConv(UINT32 attrId, UINT32 codecId);
#endif

// Note: below types are copied from exif_api.h
//#include <../include/middleware/exif_api.h>
typedef struct exifImageInfo {
	UINT8 *paddr;        /* point to thumbnail SOI */
	UINT32 len;          /* length from thumbnail SOI to EOI */
	UINT32 thmHdrOffset; /* offset to compressed thumnail data from paddr*/
	UINT32 w;       /* actual width */
	UINT32 h;       /* actual height */
	UINT32 pixelX;  /* padding width */
	UINT32 pixelY;  /* padding height */
	UINT32 datafmt; /* 422, 420 */
	UINT8 *pYQT;
	UINT8 *pUVQT;
	UINT32 IsDQTValid;
	UINT8 orient;
	UINT16 rstMCU;
} exifImageInfo_t;

typedef struct exifAttrDesc {
	UINT32 ffmt;            /* file format */
	UINT32 order;           /* little or big endian */
	exifImageInfo_t thumb;
	exifImageInfo_t main;
	UINT8 datetime[20];
	UINT8 *pImageTitle;
	UINT8 *pAudioName;
} exifAttrDesc_t;
UINT32 exifMainDecode(UINT8 *pdata, UINT32 len, exifAttrDesc_t **pattrDesc, UINT32 fBackup);

#ifdef __cplusplus
}
#endif

#endif

