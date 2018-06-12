/**************************************************************************
 *
 *       Copyright (c) 2005-2018 by iCatch Technology, Inc.
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
#include "common.h"
#include "os/_os_api.h"
#include "customization/dev_init.h"
#include "customization/app_init.h"

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
/* -- External function -- */
#if !SPCA5310
extern void devFrontEnum(void);
extern UINT32 snapMaxRawBufSizeGet(UINT32 mode);
extern void pbDramMaxAddrSet(UINT8 *);
#endif

#if !SPCA6350
extern UINT32 _bssEnd __attribute__ ((section (".bss")));

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct memProfile_s {
	void *pdataLma;
	void *pdataVma;
	void *pheapStart;
	void *pheapEnd;
} memProfile_t;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
extern memProfile_t memProfile;
#endif /*#if !SPCA6350*/

#if !SPCA5310
static UINT32 maxSnapHeapSize;
#endif

/* For customer to create block pool */
#if CUSTOM_HEAP_BLOCK_SUPPORT
static const UINT32 customblkProf[][2] = {
	/*size small to large*/
	/*customers edit their own desired block size and entry*/
#if 0
	{(128       ), 4}, /* 128 byte x 16 entries */
	{(512       ), 4}, /* 512 byte x 16 entries */
	{(2048      ), 4},  /* 2K  byte x 8 entries */
	{(5120      ), 2},  /* 5K  byte x 8 entries */
	{(16  * 1024), 1},  /* 16K byte x 4  entries */
	{(32  * 1024), 0},  /* 32K byte x 1  entries */
#else
    {(128       ), 1000},
	{(1   * 1024), 1},
	{(2   * 1024), 4},
	{(5   * 1024), 1},

#endif
	{0,0}, /* end, must not remove */
};
#endif

#define OS_MEM_SMALL_POOL_SIZE (3 * 1024 * 1024 )

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/**************************************************************************
 *                                                                        *
 *  Function Name: heapInit                                               *
 *                                                                        *
 *  Purposes: heap initialization function.                               *
 *                                                                        *
 *  Limitations:                                                          *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void
heapInit(
	void
)
{
	/*
	 * front driver enum.
	 */
#if !SPCA5310 /*moved from dev_init.c*/
	profLogAdd(0, "devFrontEnum \n");
	devFrontEnum();
#endif

#if OS_MEM_SMALL_POOL_SIZE /*0: use default size*/
	sp5kSystemCfgSet(SP5K_MEM_SMALL_POOL_SIZE_CFG, OS_MEM_SMALL_POOL_SIZE);
#endif
#if CUSTOM_HEAP_BLOCK_SUPPORT
	osCustomMemBlkInit(customblkProf);
#endif
#if CUSTOM_HEAP_BYTE_POOL_SIZE /*0: no separated byte pool custom heap*/
	osCustomMemByteInit(CUSTOM_HEAP_BYTE_POOL_SIZE);
#endif

#if !SPCA5310
	/* modify to reserved for snap and playback manual buffer */
	if (maxSnapHeapSize == 0) {
		maxSnapHeapSize = snapMaxRawBufSizeGet(SNAP_CAPTURE_MODE);
		pbDramMaxAddrSet( (UINT8 *)memProfile.pheapEnd );
	}
	memProfile.pheapEnd = (UINT8 *)memProfile.pheapEnd - maxSnapHeapSize;
#endif
}

#if (ICAT_WIFI || ICAT_ETHERNET) && APP_RTMP
#define CXX_BUF_POOL	(6*1024*1024)
#elif APP_RTMP
#define CXX_BUF_POOL	(4*1024*1024)
#elif (ICAT_WIFI || ICAT_ETHERNET)
#define CXX_BUF_POOL	(1*1024*1024)
#else	/* what is normal C++ usage? */
#define CXX_BUF_POOL	(512*1024)
#endif

#if CXX_SUPPORT
void
cxxHeapInit(
	void
)
{
	void cxxCustomMemPoolInit(
			UINT32 byteSize, UINT32 blockSize, UINT32 blockNum);
	cxxCustomMemPoolInit(CXX_BUF_POOL, 64, 1024*8);
}
#endif /*#if CXX_SUPPORT*/
