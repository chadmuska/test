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
#ifndef _APP_DBG_API_H_
#define _APP_DBG_API_H_

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
/*#include "sp5k_dbg_api.h"*/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern void dbgAssert(const char *fileName, UINT32 lineNum, const char *msgStr,...);
extern UINT32 profLogAddN(UINT32 id, const char *msg, UINT32 n);
extern UINT32 profLogPrintf(UINT32 slot, const char *format, ...);

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#if !defined(HOST_DBG)
#define HOST_DBG 0
#endif

#if HOST_DBG
#define DBG_ASSERT_EXT_EN   1
#define DBG_PRINT_EXT_EN    1
#endif

/*
 * force turn off debug code generating.
 */
#define DBG_NO_DBG  0  /* if 1, force turn off debug code generating, for testing (need clean and rebuild all). */
#if DBG_NO_DBG == 1
#undef DBG_FUNC_TRACE_EXT_EN
#undef DBG_ASSERT_EXT_EN
#undef DBG_PRINT_EXT_EN

#define DBG_FUNC_TRACE_EXT_EN  0
#define DBG_ASSERT_EXT_EN      0
#define DBG_PRINT_EXT_EN       0
#endif

/*
 * check and set DBG_ASSERT_EN.
 */
#ifdef DBG_ASSERT_EXT_EN  /* if defined in makefile exists, use it. */
#undef DBG_ASSERT_EN
#define DBG_ASSERT_EN  DBG_ASSERT_EXT_EN  /* suppressed by external. */
#endif

#ifndef DBG_ASSERT_EN  /* if not defined, set off as default. */
#define DBG_ASSERT_EN  0
#endif

#if ( DBG_ASSERT_EN != 0 && DBG_ASSERT_EN != 1 )  /* invalid setting? */
#error Invalid DBG_ASSERT_EN setting.
#endif

/*
 * check and set DBG_PRINT_EN.
 */
#ifdef DBG_PRINT_EXT_EN  /* if defined in makefile exists, use it. */
#undef DBG_PRINT_EN
#define DBG_PRINT_EN  DBG_PRINT_EXT_EN  /* suppressed by external. */
#endif

#ifndef DBG_PRINT_EN  /* if not defined, set off as default. */
#define DBG_PRINT_EN  0
#endif

#if ( DBG_PRINT_EN != 0 && DBG_PRINT_EN != 1 )  /* invalid setting? */
#error Invalid DBG_PRINT_EN setting.
#endif

/*
 * asserting.
 *
 * macros:
 *   DBG_ASSERT().
 */
#if DBG_ASSERT_EN == 0
#define DBG_ASSERT(expr)

#elif DBG_ASSERT_EN == 1
#define DBG_ASSERT(expr) \
	if ( !(expr) ) { \
		dbgAssert((char *)__FILE__, __LINE__, NULL); \
	} \

#endif

#ifdef DBG_PRINT
 #undef DBG_PRINT
#endif

/* debug the host */
/* level setting */
#define LEVEL_ERROR                 0 /*min print info*/
#define LEVEL_WARN                  1
#define LEVEL_INFO                  2
#define LEVEL_DEBUG                 3 /*max print info*/
#define LEVEL_TRACE                 (LEVEL_DEBUG+1) /*for DBG_PRINT use*/

/* color setting */
#define DEBUG_COLOR_NORMAL          "\033[m"
#define DEBUG_COLOR_BLACK           "\033[30m"
#define DEBUG_COLOR_RED             "\033[31m"
#define DEBUG_COLOR_GREEN           "\033[32m"
#define DEBUG_COLOR_YELLOW          "\033[33m"
#define DEBUG_COLOR_BLUE            "\033[34m"
#define DEBUG_COLOR_PURPLE          "\033[35m"
#define DEBUG_COLOR_BKRED           "\033[41;37m"

/* tag setting */
#define STRINGIFY(x, s)             #x s
#define ADDTRACECTAG(s)             STRINGIFY([host][trace], s)
#define ADDDBGTAG(s)                STRINGIFY([host][dbg], s)
#define ADDTAG(s)                   STRINGIFY([host], s)
#define ADDWARNTAG(s)               STRINGIFY([host][warn], s)
#define ADDERRTAG(s)                STRINGIFY([host][err], s)

/* debug setting */
#if SP5K_MULTI_FAST_BOOT
#define HOST_PROF_LOG_EN            1
#else
#define HOST_PROF_LOG_EN            0
#endif
#define HOST_PROF_ID                0

/* debug marco */
#if HOST_PROF_LOG_EN
#if SP5K_MULTI_FAST_BOOT
#define DEBUG_DEF_LEVEL             LEVEL_INFO  //use level
#else
#define DEBUG_DEF_LEVEL             LEVEL_DEBUG //use level
#endif

#define HOST_PROF_LOG_ADD(level, s)                                                     \
        if(level <= DEBUG_DEF_LEVEL)                                                    \
            profLogAddN(HOST_PROF_ID, ADDTAG(s), sizeof(ADDTAG(s))-1)

#define HOST_PROF_LOG_PRINT(level, s, args...)                                          \
        if(level <= DEBUG_DEF_LEVEL)                                                    \
            profLogPrintf(HOST_PROF_ID, ADDTAG(s),##args)
#else

#define DEBUG_DEF_LEVEL             LEVEL_INFO  //use level

#define HOST_PROF_LOG_ADD(level, s)                                                     \
        if(level <= DEBUG_DEF_LEVEL)                                                    \
        {                                                                               \
            if(level <= LEVEL_WARN)                                                     \
            {                                                                           \
              if(level == LEVEL_WARN)                                                   \
                printf(DEBUG_COLOR_GREEN ADDWARNTAG(s) DEBUG_COLOR_NORMAL "\n");        \
              else                                                                      \
              {                                                                         \
                printf(DEBUG_COLOR_BKRED ADDERRTAG(s) DEBUG_COLOR_NORMAL "\n");         \
                callStack();                                                            \
              }                                                                         \
            }                                                                           \
            else if(level == LEVEL_INFO)                                                \
                printf(DEBUG_COLOR_YELLOW ADDTAG(s) DEBUG_COLOR_NORMAL "\n");           \
            else                                                                        \
                printf(DEBUG_COLOR_PURPLE ADDDBGTAG(s) DEBUG_COLOR_NORMAL "\n");        \
        }                                                                               \
        if(level == LEVEL_TRACE)                                                        \
                printf(DEBUG_COLOR_BLUE ADDTRACECTAG(s) DEBUG_COLOR_NORMAL);

#define HOST_PROF_LOG_PRINT(level, s, args...)                                          \
        if(level <= DEBUG_DEF_LEVEL)                                                    \
        {                                                                               \
            if(level <= LEVEL_WARN)                                                     \
            {                                                                           \
              if(level == LEVEL_WARN)                                                   \
                printf(DEBUG_COLOR_GREEN ADDWARNTAG(s) DEBUG_COLOR_NORMAL "\n", ##args);\
              else                                                                      \
              {                                                                         \
                printf(DEBUG_COLOR_BKRED ADDERRTAG(s) DEBUG_COLOR_NORMAL "\n", ##args); \
                callStack();                                                            \
              }                                                                         \
            }                                                                           \
            else if(level == LEVEL_INFO)                                                \
                printf(DEBUG_COLOR_YELLOW ADDTAG(s) DEBUG_COLOR_NORMAL "\n", ##args);   \
            else                                                                        \
                printf(DEBUG_COLOR_PURPLE ADDDBGTAG(s) DEBUG_COLOR_NORMAL "\n", ##args);\
        }                                                                               \
        if(level == LEVEL_TRACE)                                                        \
                printf(DEBUG_COLOR_BLUE ADDTRACECTAG(s) DEBUG_COLOR_NORMAL, ##args);
#endif

#if DBG_PRINT_EN == 0
    #define DBG_PRINT(strFmt, args...)
#elif DBG_PRINT_EN == 1
    #define DBG_PRINT(strFmt, args...) \
	    HOST_PROF_LOG_PRINT(LEVEL_TRACE, strFmt, ##args)
#endif

#define BPrintf(str)  {\
	printf("\033[44;30m");\
	printf(ADDTAG(str));\
	printf("\033[0m\n");\
}

#define GPrintf(str)  {\
	printf("\033[42;30m");\
	printf(ADDTAG(str));\
	printf("\033[0m\n");\
}

#define YPrintf(str)  {\
	printf("\033[43;30m");\
	printf(ADDTAG(str));\
	printf("\033[0m\n");\
}

#define RPrintf(str)  {\
	printf("\033[41;30m");\
	printf(ADDTAG(str));\
	printf("\033[0m\n");\
}

#define HLPrintf1(str, a)  {\
	printf("\033[43;30m");\
	printf(ADDTAG(str), a);\
	printf("\033[0m\n");\
}

#define HLPrintf2(str, a, b)  {\
	printf("\033[43;30m");\
	printf(ADDTAG(str), a, b);\
	printf("\033[0m\n");\
}

#define HLPrintf3(str, a, b, c)  {\
	printf("\033[43;30m");\
	printf(ADDTAG(str), a, b, c);\
	printf("\033[0m\n");\
}

#define __FUNC_TRACK__  HOST_PROF_LOG_PRINT(LEVEL_INFO,"Function:%s Line:%u",__FUNCTION__,__LINE__)
#if SP5K_USB_UVC_SUPPORT
#else
void dbgPrint(UINT32 sw, const UINT8 *strFmt, ...);
#endif
void breakpoint(void);

void sioRedirect(void *pf);
void siofilterPatternDump(void);
UINT32 siofilterPatternSet(BOOL fExclude, char *pattern);
extern void callStack(void);
#endif  /* _APP_DBG_API_H_ */

