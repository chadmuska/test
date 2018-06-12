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
#ifndef _APP_PHOTOFRAME_H_
#define _APP_PHOTOFRAME_H_

#include "app_playback_uiparam.h"
#include "app_playback_def.h"

#define APP_PHOTOFRAME_IDX_INVALID	0xFF

UINT32 appPbPhotoframeCreate( pbImgCb_t *pimgCb );
void appPbPhotoframeClear();
UINT32 appPbEffectPhotoFrameProc( UINT32 srcAddr, UINT32 width, UINT32 height ); 
void appSnapPhotoFrameProc( UINT8* yuv, UINT8* qv, UINT32* sz );
void appPbPhotoframeDestroy( pbImgCb_t *pimgCb );

#endif

