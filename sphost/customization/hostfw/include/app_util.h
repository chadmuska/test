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
#ifndef _APP_UTIL_H_
#define _APP_UTIL_H_

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
#if SP5K_ICAT_EIS
#define SUPPORT_RUNTIME_SWITCH_DRAM_CLK (0)/**< For EIS, always 500MHz of EVB_DDR3_2G_500MHz.prm, temp solution */
#else
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
#define SUPPORT_RUNTIME_SWITCH_DRAM_CLK (0)
#else
#define SUPPORT_RUNTIME_SWITCH_DRAM_CLK (1)/**< For V35, DRAM_CLK default 300MHz. But for 1920x1080@60fps, DRAM_CLK MUST be 450MHz. */
#endif
#endif

#define RUNTIME_SWITCH_DRAM_CLK_DEF_KHZ (300000) /*< 300MHz = 300000 KHz */
#define RUNTIME_SWITCH_DRAM_CLK_MAX_KHZ (450000) /*< 450MHz = 4500000 KHz */

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/
#define MS_DIFF(t1, t2) ( (t2)>=(t1) ? (t2)-(t1) : (t2)+(0xffffffff-(t1)) )

/* the macro bound -3 to 5, 11 to 3, by range [3, 10], but -100 might not be bound correctly */
#define BOUND_CYCLING(value, min, max)		( (min) + \
	( ((SINT32)(value)<(SINT32)(min) ? ((max)-(min)+1) : 0) + (value) - (min) ) % ((max)-(min)+1) )

/* the macro bound -3 to 5, 11 to 3, by range [3, 10] */
#define BOUND_CYCLING_EX(value, min, max)	\
	( (min) + (signed)((value)-(min))%(signed)((max)-(min)+1) + \
	( (signed)((value)-(min))%(signed)((max)-(min)+1)<0 ? ((max)-(min)+1) : 0) )

/* the macro bound -3 to 3, 11 to 10, by range [3, 10] */
#define BOUND_SATURATED(value, min, max)	\
	( (SINT32)(value)<(SINT32)(min) ? (min) : (SINT32)(value)>(SINT32)(max) ? (max) : (value) )

#define COUNT_OF(array)	(sizeof(array)/sizeof(array[0]))
/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/

UINT32 WriteFile(UINT8 *fname, UINT8 *buf, UINT32 size);
UINT32 ReadFile(UINT8 *fname, UINT8 *buf, UINT32 size);
UINT32 ReadFileEx(UINT8 *fname, UINT8**pbuf, UINT32 *psize);

/*s Add by Aries 09/10/30*/
void appyuvScale(UINT8 *srcbuf, UINT32 yuvw, UINT32 yuvh,UINT8 *dstbuf, UINT32 qvw, UINT32 qvh);
/*e Add by Aries 09/10/30*/

UINT32 appBackgroundFuncExec(void *pfunc, UINT32 param, UINT32 priority, UINT32 blocking);
#if SUPPORT_RUNTIME_SWITCH_DRAM_CLK
void appSysClkSwitch(UINT8 sel);
#endif
void
appModeCfgPvRawSet(
	UINT32 doMode
);
UINT32
appModeCfgPvRawGet(
	void
);


#endif  /* _APP_UTIL_H_ */


