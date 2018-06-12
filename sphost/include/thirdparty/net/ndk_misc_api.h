/**************************************************************************
 *
 *       Copyright (c) 2011 by iCatch Technology, Inc.
 *
 *  This software is copyrighted by and is the property of iCatch Technology,
 *  Inc.. All rights are reserved by iCatch Technology, Inc..
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
 *  Hsin-Chu, Taiwan, R.O.C.
 *
 **************************************************************************/

#ifndef __NDK_MISC_API_H__
#define __NDK_MISC_API_H__

#ifdef __cplusplus
extern "C" {
#endif

enum { NDK_DBG_FATAL = 0, NDK_DBG_ERROR, NDK_DBG_WARN, NDK_DBG_INFO, NDK_DBG_NOISY, NDK_DBG_CUSTOM, NDK_DBG_ALL };

/* Time */
long   ndk_time(long *t);
void   ndk_msleep(ULONG millisecs);
void   ndk_usleep(ULONG microsecs);
int    ndk_gettimeofday(struct timeval *tv);

/* Debug & Log */
void   ndk_dbgout_line(int lvl, const char* prompt, int line, const char* fmt, ...) __attribute__ ((format (printf, 4, 5)));

void   callStack(void);
void   dbgAssert(const char *fileName, unsigned int lineNum, const char *msgStr, ...);
UINT32 callStackMem(UINT32 *buf, UINT32 bufsize);

#define profLogPuts(slot, msg)	profLogAddN(slot, msg, strlen(msg))
#define ndk_fatal(args...)      ndk_dbgout_line(NDK_DBG_FATAL, __func__, __LINE__, "" args)
#define ndk_error(args...)      ndk_dbgout_line(NDK_DBG_ERROR, __func__, __LINE__, "" args)
#define ndk_warning(args...)    ndk_dbgout_line(NDK_DBG_WARN,  __func__, __LINE__, "" args)
#define ndk_info(args...)       ndk_dbgout_line(NDK_DBG_INFO,  __func__, __LINE__, "" args)

#define ndk_err_return(...)     do { \
	ndk_dbgout_line(NDK_DBG_ERROR, __func__, __LINE__, "erret"); \
	return __VA_ARGS__; \
} while(0)

#define NDK_ASSERT(v)           do { if (!(v)) { dbgAssert(__FILE__, __LINE__, NULL); } } while(0)
#define NDK_ASSERT_EXPR(v, e)   do { if (!(v)) { e; dbgAssert(__FILE__, __LINE__, NULL); } } while(0)

/* Byte Swap */
#define CONST_SWAP_16(x) ((UINT16)(			\
	(((UINT16)(x) & (UINT16)0x00ffU) << 8) |	\
	(((UINT16)(x) & (UINT16)0xff00U) >> 8)))

#define CONST_SWAP_32(x) ((UINT32)(			\
	(((UINT32)(x) & (UINT32)0x000000ffUL) << 24) |	\
	(((UINT32)(x) & (UINT32)0x0000ff00UL) <<  8) |	\
	(((UINT32)(x) & (UINT32)0x00ff0000UL) >>  8) |	\
	(((UINT32)(x) & (UINT32)0xff000000UL) >> 24)))

static inline UINT16 platform_htons(UINT16 a)  { return CONST_SWAP_16(a); }
static inline UINT16 platform_ntohs(UINT16 a)  { return CONST_SWAP_16(a); }
static inline UINT32 platform_htonl(UINT32 a)  { return CONST_SWAP_32(a); }
static inline UINT32 platform_ntohl(UINT32 a)  { return CONST_SWAP_32(a); }

#define NDK_ALIGN_TO(x, n)      ( ((x)+((n)-1)) & ~((n)-1) )

#ifdef __cplusplus
}
#endif

#endif
