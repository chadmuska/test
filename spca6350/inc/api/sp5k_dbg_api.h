/**************************************************************************
 *
 *       Copyright (c) 2004-2013 by iCatch Technology, Inc.
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
/**
 * @file sp5k_dbg_api.h
 * @brief Debug related pubilc apis for host to use.
 */

#ifndef __SP5K_DBG_API_H__
#define __SP5K_DBG_API_H__

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/

/**************************************************************************
 *                            C O N S T A N T S                           *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#ifndef HOST_ASSERT
#define HOST_ASSERT(expr) do { \
	if (!(expr)) { \
		dbgAssert(__FILE__, __LINE__, NULL); \
	} \
} while(0)
#endif

#ifndef HOST_ASSERT_MSG
#define HOST_ASSERT_MSG(expr, fmtstr, args...) do { \
	if (!(expr)) { \
		dbgAssert(__FILE__, __LINE__, fmtstr, ##args); \
	} \
} while(0)
#endif

/**************************************************************************
 *                          D A T A   T Y P E S
 **************************************************************************/

/**************************************************************************
 *                  E X T E R N A L   R E F E R E N C E
 **************************************************************************/
/* Assert */
extern void dbgAssert(const char *fileName, UINT32 lineNum, const char *msgStr, ...)
	__attribute__ ((format (printf, 3, 4)));

/* Profile log */
extern UINT32 profLogInit(UINT32 id, UINT32 size);
extern UINT32 profLogAdd(UINT32 id, const char *msg);
extern UINT32 profLogAddN(UINT32 id, const char *msg, UINT32 n);
extern UINT32 profLogPrintf(UINT32 slot, const char *format, ...)
			__attribute__ ((format (printf, 2, 3)));
extern UINT32 profLogCallStack(UINT32 id);
extern UINT32 profLogDump(UINT32 id);
extern void   profLogDumpAll(void);


/* Call stack */
extern void callStack(void);

/* Process information */
extern unsigned _dbg_proc_ps_en(unsigned en);
extern void _dbg_proc_ps_dump(unsigned n);
extern void _dbg_proc_ps_simple_dump(void);

/**************************************************************************
 *              F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/

 
#endif  /* __SP5K_DBG_API_H__ */

