/**************************************************************************
 *
 *       Copyright (c) 2005-2016 by iCatch Technology, Inc.
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
 *                                                                        *
 *  Author:phancia Lai                                                    *
 *                                                                        *
 **************************************************************************/
/* DO NOT EDIT the next 2 lines for auto revision by subversion
$Rev: 26010 $
$Date:: 2016-04-20 16:06:10#$
 */

#ifndef _RSVBLK_DEF_H_
#define _RSVBLK_DEF_H_

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
/* Resource file type which are available for sp5kResourceFilePlay() */
typedef enum {
	RES_TYPE_JPG = 0x00,
	RES_TYPE_WAV,
	RES_TYPE_WAV_MIX,
	RES_TYPE_MP3,
	RES_TYPE_WMA,
	RES_TYPE_JPG_PIP,
	RES_TYPE_JPG_PF,
	RES_TYPE_AVI,
	RES_TYPE_AVI_PIP,
	RES_TYPE_AVI_PF,
	RES_TYPE_WAV_BUF,
	RES_TYPE_EXTEND,
	RES_FILE_TYPE_NUM,
} resFileType_e;

typedef enum {
	RES_SUBTYPE_JPG_MAIN = 0x10000,
	RES_SUBTYPE_JPG_QV = 0x20000,
	RES_SUBTYPE_JPG_THM = 0x30000,
	RES_SUBTYPE_AVI_THM = 0x40000,
} resFileSubType_e;

/*!< should sync with sp5kRotFlip_e in sp5k_pb_api.h */
typedef enum {
	RSV_ROTATE_0 = 0x00,
	RSV_ROTATE_90,
	RSV_ROTATE_180,
	RSV_ROTATE_270,
	RSV_ROTATE_FLIP_H,
	RSV_ROTATE_FLIP_V,
	RSV_ROTATE_MAX
} rsvImageRotate_t;

typedef struct {
	UINT32 width;
	UINT32 height;
	UINT32 roiX;
	UINT32 roiY;
	UINT32 roiW;
	UINT32 roiH;
} rsvImageRoi_t;

typedef enum {
	RSV_DRAW_THUMB_ROTATE_BY_THUMB = 0x00,
	RSV_DRAW_THUMB_ROTATE_BY_PRIMARY,
	RSV_DRAW_THUMB_ROTATE_BY_NONE,
} rsvThmRotateBase_t;

typedef enum {
	RSV_QV_ROTATE_NONE,
	RSV_QV_ROTATE_BY_EXIF,
} rsvQvRotateBase_t;

typedef enum {
	RSV_MAIN_ROTATE_NONE,
	RSV_MAIN_ROTATE_BY_PRIMARY,
} rsvMainRotateBase_t;

typedef enum {
	RSV_PRELOAD_THUMB_MAXSIZE = 0x00,
	RSV_PRELOAD_QV_MAXSIZE,
	RSV_PRELOAD_MAXSIZE_NONE,
} rsvPreloadMaxSize_t;
#endif /* _RSVBLK_DEF_H_ */

