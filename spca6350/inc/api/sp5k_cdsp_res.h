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
 *                                                                        *
 **************************************************************************/
#ifndef __SP5K_CDSP_RES_H__
#define __SP5K_CDSP_RES_H__


/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/


/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct sp5kResId2SysgId_s{
	UINT8 res5kId;
	UINT8 resSysgId;
	UINT8 resCdspId;
}sp5kResId2SysgId_t;

typedef struct sp5kIqResRgbGmaPlainBuf_s{
	UINT8 array[4096];
} sp5kIqResRgbGmaPlainBuf_t;

typedef struct sp5kIqResYGmaPlainBuf_s{
	UINT8 array[256];
} sp5kIqResYGmaPlainBuf_t;

typedef struct sp5kIqResHuePlainBuf_s{
	UINT8 sin[180];	/* -1.0 ~ 1.0 , 1s 1.6 bits*/
	UINT8 cos[180]; /* -1.0 ~ 1.0 , 1s 1.6 bits*/
	UINT8 satL[180];/* 2.6 bits*/
	UINT8 satH[180];/* 2.6 bits*/
} sp5kIqResHuePlainBuf_t;

typedef struct sp5kCdspRes3dLut_s{
	UINT8 r;
	UINT8 g;
	UINT8 b;
} sp5kIqRes3dLut_t;

typedef struct sp5kIqRes3dLutPlainBuf_s{
	sp5kIqRes3dLut_t array[729];
} sp5kIqRes3dLutPlainBuf_t;

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/

 /**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

UINT32 sp5kIqLutBufSizeGet(UINT32 resId,UINT32 *plainFmtBufSize,UINT32 *lutSize,char *fPathName);
UINT32 sp5kIqLutBufSizeSet(UINT32 resId,char *fName);
UINT32 sp5kIqLutEncode(UINT32 resId,void *plain_fmt_buf,UINT32 plain_fmt_buf_size,UINT8 *lut,UINT32 lut_size);
UINT32 sp5kIqLutGet(UINT32 resId, UINT8 *buf, UINT32 size, char *fPathName);
UINT32 sp5kIqLutSet(UINT32 resId, UINT8 *buf, UINT32 size, char *fname);
UINT32 sp5kCdspResBufSizeSet(UINT32 id,	UINT32 size);
UINT32 sp5kIqLdcBufDirectSet(UINT8 *buf, UINT32 size);
UINT32 sp5kIqBufPreset(UINT32 res5kId,UINT32 iqIdx,UINT32 tblBuf);
void sp5kIqLutSizeGet(UINT32 resId,	UINT32 *hdrSize, UINT32 *tblSize);

#endif /* _SP5K_CDSP_RES_H_ */


