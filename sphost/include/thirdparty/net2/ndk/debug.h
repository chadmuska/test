/**************************************************************************
 *
 *       Copyright (c) 2012 by iCatch Technology, Inc.
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
#ifndef __NDK_DEBUG_H__
#define __NDK_DEBUG_H__

#ifdef __cplusplus
//#define __FUNCNAME__  __PRETTY_FUNCTION__
# define __FUNCNAME__    __FILE__
extern "C" {
#else
# define __FUNCNAME__    __func__
#endif

///////////////////////////////////////////////////////////////////////////////
// Debug and Log Functions.
extern const char *err_goto;

extern UINT32 callStackMem(UINT32 *buf, UINT32 bufsize);

/* print lock. debug only  */
#define NDK_PRLOCK_EN	1

#if NDK_PRLOCK_EN
extern void ndk_prlock_get();
extern void ndk_prlock_put();
#else
#define ndk_prlock_get()	((void)0)
#define ndk_prlock_put()	((void)0)
#endif

extern void ndk_hexput(unsigned char ch);
extern int  ndk_puts(const char* s); // No new-line at the end
extern int  ndk_nputs(const char* s, int n);
extern int  ndk_vputs(const char* s, ...); // End with NULL

///////////////////////////////////////////////////////////////////////////////
// Print prefix string at the head.
extern unsigned ndk_preprint_level;
extern void ndk_preprint_setup(const char *prefix, unsigned width);
extern void ndk_preprint(const char *format, ...);
static inline void ndk_preprint_push() { ++ndk_preprint_level; }
static inline void ndk_preprint_pop(unsigned n) { ndk_preprint_level = ndk_preprint_level < n ? 0 : ndk_preprint_level - n; }

///////////////////////////////////////////////////////////////////////////////
#define ndk_println(fmt, args...)	do{ ndk_prlock_get(); printf(fmt "\n", ##args); ndk_prlock_put(); }while(0)

#define ndk_callstack_println(args...)	do{ \
	ndk_prlock_get(); \
	ndk_dbgout_line(NDK_DBG_INFO,__func__,__LINE__,""args); \
	callStack(); \
	ndk_prlock_put(); \
}while(0)

extern void ndk_dbg_set_level(int dbglvl);
extern void ndk_memdump(const void *mem, size_t len);
extern void ndk_hexdump(const char* prompt, const void *mem, size_t mem_len, size_t disp_len);
extern void ndk_hexprint(const void *buf, size_t len, int cols, int separator);

/* unit of bufsize is sizeof(void*). */
extern void ndk_callstackmem(void *buf, unsigned bufsiz, unsigned start);
extern void ndk_callstack();

#if NDK_DEBUG > 0
	#define ndk_noisy(args...)      ndk_dbgout_line(NDK_DBG_NOISY, __FUNCNAME__, __LINE__, " " args)
	#define ndk_trace(args...)      ndk_dbgout_line(NDK_DBG_YELL, __FUNCNAME__, __LINE__, "__TRACE__ " args)
	#define ndk_enter() 		ndk_dbgout_line(NDK_DBG_NOISY, __FUNCNAME__, __LINE__, "+++");
	#define ndk_leave() 		ndk_dbgout_line(NDK_DBG_NOISY, __FUNCNAME__, __LINE__, "---");

	#define ndk_errno_return(expr_or_val, fmt, args...) do { \
		int _tmp_e = (expr_or_val); \
		if (_tmp_e != 0) sp5kProfLogPrintf(0, "%s:%d> err %d:" fmt, __func__, __LINE__, _tmp_e, ##args); \
		if (_tmp_e < 0) ndk_errno = -_tmp_e; \
		return _tmp_e; \
	} while(0)

	#define ndk_err_goto(label)     do { \
		ndk_dbgout_line(NDK_DBG_ERROR, __FUNCNAME__, __LINE__, err_goto); \
		goto label; \
	} while(0)

	#define NDK_ASSERT_PTR(ptr)     \
		do { \
			if ( (UINT32)(ptr) < 0x80000000U || (UINT32)(ptr) > 0xAFFFFFFFU || \
			     ((UINT32)(ptr) > 0x8FFFFFFFU && (UINT32)(ptr) < 0xA0000000U) ) { \
				printf("Inv PTR "#ptr" = %p", (ptr)); \
				dbgAssert(__FILE__, __LINE__, NULL); \
			} \
		} while (0)

#else
	#define ndk_noisy(...)          ((void)0)
	#define ndk_trace(...)          ((void)0)
	#define ndk_enter()             ((void)0)
	#define ndk_leave()             ((void)0)

	#define ndk_errno_return(expr_or_val, ...) return ({ \
		int _tmp_e = (expr_or_val); \
		if (_tmp_e < 0) \
			ndk_errno = -_tmp_e; \
		_tmp_e; })

	#define ndk_err_goto(label)     goto label

	#define NDK_ASSERT_PTR(ptr)     ((void)0)
#endif

#ifdef NDK_NOASSERT
	#define NDK_WARN_ON(x)          ((void)0)
	#define NDK_BUG_ON(x)           ((void)0)
#else
	#define NDK_WARN_ON(x)          NDK_ASSERT(!(x))
	#define NDK_BUG_ON(x)           NDK_ASSERT(!(x))
#endif

#undef  assert
#define assert NDK_ASSERT

///////////////////////////////////////////////////////////////////////////////
// Time Chart Debug Macros
/* Predefined Tracks */
enum {
	NDK_TCHART_TRACK_DRVTX = 0,
	NDK_TCHART_TRACK_DRVRX,
	NDK_TCHART_TRACK_IFTX,
	NDK_TCHART_TRACK_IFRX,
	NDK_TCHART_TRACK_SOCKSND,
	NDK_TCHART_TRACK_SOCKRCV,
	NDK_TCHART_TRACK_LWIP,

	NDK_TCHART_TRACK_NUM,
	NDK_TCHART_TRACK_ALL = (1<<NDK_TCHART_TRACK_NUM) - 1,

	NDK_TCHART_ALL = 0xFFFFFFFFU
};

#ifdef NDK_DEBUG_TCHART
extern int    ndk_tchart_en;
extern UINT32 ndk_tchart_bits;

# define NDK_TCHART_SETBIT(TRACK, tracks)      (tracks) |= (1 << NDK_TCHART_TRACK_##TRACK)
# define NDK_TCHART_EN(enabled, tracks)        do { ndk_tchart_en = (enabled); ndk_tchart_bits = tracks; } while (0)

# define NDK_TCHART_S(TRACK, args...)          if (ndk_tchart_en && ((1 << NDK_TCHART_TRACK_##TRACK) & ndk_tchart_bits)) sp5kProfLogPrintf(0, "#S#" #TRACK "#N#", ##args)
# define NDK_TCHART_E(TRACK)                   if (ndk_tchart_en && ((1 << NDK_TCHART_TRACK_##TRACK) & ndk_tchart_bits)) sp5kProfLogPrintf(0, "#E#" #TRACK "#N#")
# define NDK_TCHART_P(TRACK, args...)          if (ndk_tchart_en && ((1 << NDK_TCHART_TRACK_##TRACK) & ndk_tchart_bits)) sp5kProfLogPrintf(0, "#P#" #TRACK "#N#", ##args)

# define NDK_TCHART_CUSTOM_S(track, args...)   if (ndk_tchart_en) sp5kProfLogPrintf(0, "#S#" track "#N#" ##args)
# define NDK_TCHART_CUSTOM_E(track)            if (ndk_tchart_en) sp5kProfLogPrintf(0, "#E#" track "#N#")
# define NDK_TCHART_CUSTOM_P(track, args...)   if (ndk_tchart_en) sp5kProfLogPrintf(0, "#P#" track "#N#" ##args)
#else /* NDK_DEBUG_TCHART */
# define NDK_TCHART_SETBIT(TRACK, bits)        ((void)0)
# define NDK_TCHART_EN(n, bits)                ((void)0)

# define NDK_TCHART_S(TRACK, args...)          ((void)0)
# define NDK_TCHART_E(TRACK)                   ((void)0)
# define NDK_TCHART_P(TRACK, args...)          ((void)0)

# define NDK_TCHART_CUSTOM_S(track, args...)   ((void)0)
# define NDK_TCHART_CUSTOM_E(track)            ((void)0)
# define NDK_TCHART_CUSTOM_P(track, args...)   ((void)0)
#endif /* NDK_DEBUG_TCHART */

#define ndk_tchart_begin(track)                sp5kProfLogPrintf(0, "#S#" #track "#N#")
#define ndk_tchart_end(track)                  sp5kProfLogPrintf(0, "#E#" #track "#N#")
#define ndk_tchart_info(track, args...)        sp5kProfLogPrintf(0, "#P#" #track "#N#", ##args)

#ifdef __cplusplus
}
#endif

#endif

