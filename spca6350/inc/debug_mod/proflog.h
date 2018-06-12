/**************************************************************************
 *
 *       Copyright (c) 2007-2016 by iCatch Technology, Inc.
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
 *  Author: Paul May                                                      *
 *                                                                        *
 **************************************************************************/
/* DO NOT EDIT the next 2 lines for auto revision by subversion
$Rev: 26010 $
$Date:: 2016-04-20 16:06:10#$
 */

#ifndef _PROF_LOG_H_
#define _PROF_LOG_H_

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
#include "common.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define SYS_PROF_ID         0

#define TIME_CHART_GEN      0
#define TIME_CHART_VFS      0
#define TIME_CHART_DISK     0
#define TIME_CHART_SD       0
#define TIME_CHART_NAND     0
#define TIME_CHART_RSV      0
#define TIME_CHART_MEDIA    0
#define TIME_CHART_5KMODESW 0
#define TIME_CHART_FASTBOOT 0
#define TIME_CHART_PVCB     0
#define TIME_CHART_MODEPV   0
#define TIME_CHART_DZOOM    0

#define TIME_CHART_SEN      0 /*sensor */
#define TIME_CHART_SNAP     1 /*snap flow*/
#define TIME_CHART_EIS      1 /* eis flow */
#define TIME_CHART_RSC      1 /* rsc flow */

#undef TIME_CHART_SNAP_WITH_COLOR
#undef TIME_CHART_SNAP_ALSO_PROF_0

typedef enum {
	PROF_LOG_MODE_KEEP_OLD, /* manual trigger at log init until full */
	PROF_LOG_MODE_KEEP_NEW, /* auto wrapping */
} profLogMode_t;

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/
#undef PROF_LOG_INIT
#undef PROF_LOG_ADD
#undef PROF_LOG_PRINT
#undef PROF_LOG_CALLSTACK
#undef PROF_LOG_MEMDUMP
#undef PROF_LOG_DUMP
#undef PROF_LOG_TERM
#if PROF_LOG_EN
#define PROF_LOG_INIT(id,size)          profLogInit(id,size)
#define PROF_LOG_ADD(id,msg)            profLogAddN(id,msg,sizeof(msg)-1)
#define PROF_LOG_ADDN(id,msg,n)         profLogAddN(id,msg,n)
#define PROF_LOG_PRINT(id,str,args...)  profLogPrintf(id,str,##args)
#define PROF_LOG_CALLSTACK(id)          profLogCallStack(id)
#define PROF_LOG_MEMDUMP(id,p,sz)       profLogMemdump(id,p,sz)
#define PROF_LOG_DUMP(id)               profLogDump(id)
#define PROF_LOG_TERM(id)               profLogTerm(id)
#else
#define PROF_LOG_INIT(id,size)
#define PROF_LOG_ADD(id,msg)
#define PROF_LOG_ADDN(id,msg,n)
#define PROF_LOG_PRINT(id,str,args...)
#define PROF_LOG_CALLSTACK(id)
#define PROF_LOG_MEMDUMP(id,p,sz)
#define PROF_LOG_DUMP(id)
#define PROF_LOG_TERM(id)
#endif

#if SYS_PROF_LOG_OFF
#define SYS_PROF_LOG_ADD(s)
#define SYS_PROF_LOG_ADDN(s,n)
#define SYS_PROF_LOG_PRINT(s,args...)
#define SYS_PROF_LOG_CALLSTACK()
#define SYS_PROF_LOG_MEMDUMP(p,sz)
#define SYS_PROF_LOG_DUMP()
#else  /*#if SYS_PROF_LOG_OFF*/
#define SYS_PROF_LOG_ADD(s)             profLogAddN(SYS_PROF_ID,s,sizeof(s)-1)
#define SYS_PROF_LOG_ADDN(s,n)          profLogAddN(SYS_PROF_ID,s,n)
#define SYS_PROF_LOG_PRINT(s,args...)   profLogPrintf(SYS_PROF_ID,s,##args)
#define SYS_PROF_LOG_CALLSTACK()        profLogCallStack(SYS_PROF_ID)
#define SYS_PROF_LOG_MEMDUMP(p,sz)      profLogMemdump(SYS_PROF_ID,p,sz)
#define SYS_PROF_LOG_DUMP()             profLogDump(SYS_PROF_ID)
#endif /*#if SYS_PROF_LOG_OFF*/

#if TIME_CHART_GEN
#define TIME_CHART_GEN_PROF_ID               3
#define TIME_CHART_GEN_PROF_LOG_PRINT(s,args...)     profLogPrintf(TIME_CHART_GEN_PROF_ID,s,##args)
#else
#define TIME_CHART_GEN_PROF_ID               3
#define TIME_CHART_GEN_PROF_LOG_PRINT(s,args...)
#endif

#if TIME_CHART_VFS
#define TIME_CHART_VFS_PROF_ID              3
#define TIME_CHART_VFS_PROF_LOG_PRINT(s,args...) profLogPrintf(TIME_CHART_VFS_PROF_ID,s,##args)
#else
#define TIME_CHART_VFS_PROF_LOG_PRINT(s,args...)
#endif

#if TIME_CHART_DISK
#define TIME_CHART_DISK_PROF_ID               3
#define TIME_CHART_DISK_PROF_LOG_PRINT(s,args...)     profLogPrintf(TIME_CHART_DISK_PROF_ID,s,##args)
#else
#define TIME_CHART_DISK_PROF_LOG_PRINT(s,args...)
#endif

#if TIME_CHART_SD
#define TIME_CHART_SD_PROF_ID               3
#define TIME_CHART_SD_PROF_LOG_PRINT(s,args...)     profLogPrintf(TIME_CHART_SD_PROF_ID,s,##args)
#else
#define TIME_CHART_SD_PROF_LOG_PRINT(s,args...)
#endif

#if TIME_CHART_NAND
#define TIME_CHART_NAND_PROF_ID               3
#define TIME_CHART_NAND_PROF_LOG_PRINT(s,args...)     profLogPrintf(TIME_CHART_NAND_PROF_ID,s,##args)
#else
#define TIME_CHART_NAND_PROF_LOG_PRINT(s,args...)
#endif

#if TIME_CHART_RSV
#define TIME_CHART_RSV_PROF_ID               3
#define TIME_CHART_RSV_PROF_LOG_PRINT(s,args...)     profLogPrintf(TIME_CHART_RSV_PROF_ID,s,##args)
#else
#define TIME_CHART_RSV_PROF_LOG_PRINT(s,args...)
#endif

#if TIME_CHART_5KMODESW
#define TIME_CHART_MODESW_PROF_ID               0
#define TIME_CHART_MODESW_PROF_LOG_PRINT(s,args...)     profLogPrintf(TIME_CHART_MODESW_PROF_ID,s,##args)
#else
#define TIME_CHART_MODESW_PROF_LOG_PRINT(s,args...)
#endif

#if TIME_CHART_FASTBOOT
#define TIME_CHART_FASTBOOT_PROF_ID               3
#define TIME_CHART_FASTBOOT_PROF_LOG_PRINT(s,args...)     profLogPrintf(TIME_CHART_FASTBOOT_PROF_ID,s,##args)
#else
#define TIME_CHART_FASTBOOT_PROF_LOG_PRINT(s,args...)
#endif

#if TIME_CHART_SEN
#define TIME_CHART_SEN_PROF_ID               1
#define TIME_CHART_SEN_PROF_LOG_PRINT(s,args...)     profLogPrintf(TIME_CHART_SEN_PROF_ID,s,##args)
#else
#define TIME_CHART_SEN_PROF_LOG_PRINT(s,args...)
#endif

#if TIME_CHART_SNAP
#define TIME_CHART_SNAP_PROF_ID               99
#define TIME_CHART_SNAP_PROF_HIJACK_ID 1

#ifdef TIME_CHART_SNAP_ALSO_PROF_0
#define TIME_CHART_SNAP_PROF_LOG_PRINT(s,args...)  do { \
		profLogPrintf(TIME_CHART_SNAP_PROF_ID,s,##args); \
		profLogPrintf(0,s,##args); \
	} while (0)
#else
#define TIME_CHART_SNAP_PROF_LOG_PRINT(s,args...) \
		profLogPrintf(TIME_CHART_SNAP_PROF_ID,s,##args)
#endif

#ifdef TIME_CHART_SNAP_WITH_COLOR
#define TIME_CHART_SNAP_PROF_LOG_PRINT_BLUE(s,args...)     TIME_CHART_SNAP_PROF_LOG_PRINT(BLUE(s),##args)
#define TIME_CHART_SNAP_PROF_LOG_PRINT_RED(s,args...)     TIME_CHART_SNAP_PROF_LOG_PRINT(RED(s),##args)
#define TIME_CHART_SNAP_PROF_LOG_PRINT_GREEN(s,args...)     TIME_CHART_SNAP_PROF_LOG_PRINT(GREEN(s),##args)

#define TIME_CHART_SNAP_PROF_LOG_PRINT_BG_BLUE(s,args...)   TIME_CHART_SNAP_PROF_LOG_PRINT(BG_BLUE(s),##args)
#define TIME_CHART_SNAP_PROF_LOG_PRINT_BG_RED(s,args...)    TIME_CHART_SNAP_PROF_LOG_PRINT(BG_RED(s),##args)
#define TIME_CHART_SNAP_PROF_LOG_PRINT_BG_GREEN(s,args...)  TIME_CHART_SNAP_PROF_LOG_PRINT(BG_GREEN(s),##args)
#else
#define TIME_CHART_SNAP_PROF_LOG_PRINT_BLUE(s,args...)     TIME_CHART_SNAP_PROF_LOG_PRINT(s,##args)
#define TIME_CHART_SNAP_PROF_LOG_PRINT_RED(s,args...)     TIME_CHART_SNAP_PROF_LOG_PRINT(s,##args)
#define TIME_CHART_SNAP_PROF_LOG_PRINT_GREEN(s,args...)     TIME_CHART_SNAP_PROF_LOG_PRINT(s,##args)

#define TIME_CHART_SNAP_PROF_LOG_PRINT_BG_BLUE(s,args...)   TIME_CHART_SNAP_PROF_LOG_PRINT(s,##args)
#define TIME_CHART_SNAP_PROF_LOG_PRINT_BG_RED(s,args...)    TIME_CHART_SNAP_PROF_LOG_PRINT(s,##args)
#define TIME_CHART_SNAP_PROF_LOG_PRINT_BG_GREEN(s,args...)  TIME_CHART_SNAP_PROF_LOG_PRINT(s,##args)
#endif
#else
#define TIME_CHART_SNAP_PROF_LOG_PRINT(s,args...)
#define TIME_CHART_SNAP_PROF_LOG_PRINT_BLUE(s,args...)
#define TIME_CHART_SNAP_PROF_LOG_PRINT_RED(s,args...)
#define TIME_CHART_SNAP_PROF_LOG_PRINT_GREEN(s,args...)

#define TIME_CHART_SNAP_PROF_LOG_PRINT_BG_BLUE(s,args...)
#define TIME_CHART_SNAP_PROF_LOG_PRINT_BG_RED(s,args...)
#define TIME_CHART_SNAP_PROF_LOG_PRINT_BG_GREEN(s,args...)
#endif

#if TIME_CHART_MEDIA
#define TIME_CHART_MEDIA_PROF_ID               3
#define TIME_CHART_MEDIA_PROF_LOG_PRINT(s,args...)     profLogPrintf(TIME_CHART_MEDIA_PROF_ID,s,##args)
#else
#define TIME_CHART_MEDIA_PROF_LOG_PRINT(s,args...)
#endif

#if TIME_CHART_EIS
#define TIME_CHART_EIS_PROF_ID               1
#define TIME_CHART_EIS_PROF_LOG_PRINT(s,args...)     profLogPrintf(TIME_CHART_EIS_PROF_ID,s,##args)
#else
#define TIME_CHART_EIS_PROF_LOG_PRINT(s,args...)
#endif

#if TIME_CHART_RSC
#define TIME_CHART_RSC_PROF_ID               1
#define TIME_CHART_RSC_PROF_LOG_PRINT(s,args...)     profLogPrintf(TIME_CHART_RSC_PROF_ID,s,##args)
#else
#define TIME_CHART_RSC_PROF_LOG_PRINT(s,args...)
#endif

#if TIME_CHART_PVCB
#define TIME_CHART_PVCB_PROF_ID               0
#define TIME_CHART_PVCB_PROF_LOG_PRINT(s,args...)     profLogPrintf(TIME_CHART_PVCB_PROF_ID,s,##args)
#else
#define TIME_CHART_PVCB_PROF_LOG_PRINT(s,args...)
#endif

#if TIME_CHART_MODEPV
#define TIME_CHART_MODEPV_PROF_ID               0
#define TIME_CHART_MODEPV_PROF_LOG_PRINT(s,args...)     profLogPrintf(TIME_CHART_MODEPV_PROF_ID,s,##args)
#else
#define TIME_CHART_MODEPV_PROF_LOG_PRINT(s,args...)
#endif

#if TIME_CHART_DZOOM
#define TIME_CHART_DZOOM_PROF_ID               0
#define TIME_CHART_DZOOM_PROF_LOG_PRINT(s,args...)     profLogPrintf(TIME_CHART_DZOOM_PROF_ID,s,##args)
#else
#define TIME_CHART_DZOOM_PROF_LOG_PRINT(s,args...)
#endif

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/* usage:
	init: profLogInit(id, 4096) to allocate buffer. id is 0-3

	run:  profLogAdd(id, "start Op A") to keep a message and calling time (us)

	end:  profLogDump(id) to print all log through UART.

	release: profLogTerm(id) to free memory
 */

UINT32 profLogOpen(void);
UINT32 profLogInit(UINT32 id, UINT32 size);
UINT32 profLogModeSet(UINT32 id, UINT32 mode);
UINT32 profLogAdd(UINT32 id, const char *msg);
UINT32 profLogAddN(UINT32 id, const char *msg, UINT32 n);
UINT32 profLogPrintf(UINT32 slot, const char *format, ...)
			__attribute__ ((format (printf, 2, 3)));
UINT32 profLogCallStack(UINT32 id);
UINT32 profLogMemdump(UINT32 id, const void *addr, UINT32 size);
UINT32 profLogDump(UINT32 id);
void   profLogDumpAll(void);
UINT32 profLogTerm(UINT32 id);


#endif  /* _PROF_LOG_H_ */
