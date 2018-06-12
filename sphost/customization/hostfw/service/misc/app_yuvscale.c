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
#include "app_com_api.h"
#include "app_com_def.h"

#include "app_General.h"
#include "api/sp5k_utility_api.h"

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

#define LOGI_CACH_BADDR_TO_LOGI_UNCACH_BADDR(addr) \
	((void *)((UINT32)(addr) | 0x20000000))

typedef struct pbScaleParam_s {
	UINT8 *srcAddr;
	UINT32 srcWidth;
	UINT32 srcHeight;
	UINT32 cropOffX;
	UINT32 cropOffY;
	UINT32 cropWidth;
	UINT32 cropHeight;
	UINT8 *destAddr;
	UINT32 destWidth;
	UINT32 destHeight;
	UINT32 roiWidth;    /* of dest. */
	UINT32 roiHeight;   /* of dest. */
} pbScaleParam_t;

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

void pbctrlCdspCropScale(pbScaleParam_t *pScale);
void cpuDCacheFlush(void *addrstart, UINT32 size);

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/*-------------------------------------------------------------------------
 *  File Name : appyuvScale
 *------------------------------------------------------------------------*/
void appyuvScale(
	UINT8 *srcbuf, UINT32 yuvw, UINT32 yuvh,
	UINT8 *dstbuf, UINT32 qvw, UINT32 qvh
)
{
	srcbuf = LOGI_CACH_BADDR_TO_LOGI_UNCACH_BADDR(srcbuf);
	dstbuf = LOGI_CACH_BADDR_TO_LOGI_UNCACH_BADDR(dstbuf);
	cpuDCacheFlush(srcbuf, yuvw*yuvh*2);

	#if SPCA6350
	/** @todo Check if GPE is the proper hardware to do */
#define ROUND_DOWN_TO_DIVISIBLE(num,div) \
	( (UINT32)(num) & -(UINT32)(div) )
#define ROUND_UP_TO_DIVISIBLE(num,div) \
	ROUND_DOWN_TO_DIVISIBLE( (UINT32)(num) + (div) - 1, div )
	
	sp5kGfxObj_t srcObj, dstObj;

	srcObj.pbuf = srcbuf;
	srcObj.bufW = ROUND_UP_TO_DIVISIBLE(yuvw, 16);
	srcObj.bufH = ROUND_UP_TO_DIVISIBLE(yuvh, 16);
	srcObj.roiX = 0;
	srcObj.roiY = 0;
	srcObj.fmt  = SP5K_GFX_FMT_YUV422;
	srcObj.roiW = yuvw;
	srcObj.roiH = yuvh;
	dstObj.pbuf = dstbuf;
	dstObj.bufW = qvw;
	dstObj.bufH = qvh;
	dstObj.roiX = 0;
	dstObj.roiY = 0;
	dstObj.fmt  = SP5K_GFX_FMT_YUV422;
	dstObj.roiW = qvw;
	dstObj.roiH = qvh;
	sp5kGfxObjectBilinearScale(&srcObj, &dstObj);

	#else
	sp5kYuvScale_t s;

	s.psrcAddr = srcbuf;
	s.pdstAddr = LOGI_CACH_BADDR_TO_LOGI_UNCACH_BADDR(dstbuf);
	s.srcWidth = yuvw;
	s.srcHeight = yuvh;
	s.dstWidth = qvw;
	s.dstHeight = qvh;
	s.cropWidth = yuvw;
	s.cropHeight = yuvh;
	s.cropOffX = 0;
	s.cropOffY = 0;

	sp5kUtilYuvScale(&s, SP5K_SCALE_FORMAT_YUV);
	#endif

	#if 0
    UINT8 *psrcBuf=NULL, *pdstBuf=NULL;
	pbScaleParam_t scaleCb;
	srcbuf = LOGI_CACH_BADDR_TO_LOGI_UNCACH_BADDR(srcbuf);
	dstbuf = LOGI_CACH_BADDR_TO_LOGI_UNCACH_BADDR(dstbuf);
	cpuDCacheFlush(srcbuf, yuvw*yuvh*2);
	scaleCb.srcAddr = srcbuf;
	scaleCb.srcWidth = yuvw;
	scaleCb.srcHeight = yuvh;
	scaleCb.cropOffX = 0;
	scaleCb.cropOffY = 0;
	scaleCb.cropWidth = yuvw;
	scaleCb.cropHeight = yuvh;
	scaleCb.destAddr = LOGI_CACH_BADDR_TO_LOGI_UNCACH_BADDR(dstbuf);
	scaleCb.destWidth = qvw;
	scaleCb.destHeight = qvh;
	scaleCb.roiWidth = qvw;
	scaleCb.roiHeight = qvh;
	#if 0  /* old version */
	pbctrlCdspCropScale( &scaleCb );
	#else  /* Ethan spca5330  */
	//pbctrlJpegCropScale( &scaleCb );
	#endif
	#endif
}

