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
/* DO NOT EDIT the next 2 lines for auto revision by subversion
$Rev: 29002 $
$Date:: 2017-08-23 10:49:54#$
 */

#ifndef _MODESW_DEF_H_
#define _MODESW_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

/**< frame buffer addr must align to 16 byte and width must be multiple of 8 */
typedef struct {
	UINT8 *yuvaddr;
	UINT16 yuvw;
	UINT16 yuvh;
	UINT16 dispx;
	UINT16 dispy;
	UINT16 dispw;
	UINT16 disph;
} modeswFrameBuf_t;


/** video roi must align 2 */
typedef struct {
	UINT32 videoSrcPath; /* 0 : from pv main buf, 1 : from pv 2nd buf */
	UINT32 videoSrcRoiX; /* video src roi X in pv yuv buf domain */
	UINT32 videoSrcRoiY; /* video src roi X in pv yuv buf domain */
	UINT32 videoSrcRoiW; /* video src roi X in pv yuv buf domain */
	UINT32 videoSrcRoiH; /* video src roi X in pv yuv buf domain */
} modePvVideoSrcRoi_t;


#ifdef __cplusplus
}
#endif

#endif /* _MODESW_DEF_H_ */
