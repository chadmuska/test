/**************************************************************************
 *
 *       Copyright (c) 2015-2016 by iCatch Technology, Inc.
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
 *  Author: sf.wu                                                    *
 *                                                                        *
 **************************************************************************/

#ifndef _ALG_EFFECT_DEF_H_
#define _ALG_EFFECT_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif






typedef struct fbFaceInfo_s {
	UINT32 x;
	UINT32 y;
	UINT32 size;
} fbFaceInfo_t;


typedef struct {
	int valid;
	int srcHOfs;
	int srcVOfs;
	int srcHSize;
	int srcVSize;
	int dstHOfs;
	int dstVOfs;
	int dstHSize;
	int dstVSize;
	int scaleFac; /* 27.4b */
} fbGmSclInfo_t;




#ifdef __cplusplus
}
#endif

#endif /* _ALG_EFFECT_DEF_H_ */
