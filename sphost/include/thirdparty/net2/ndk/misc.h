/**************************************************************************
 *
 *       Copyright (c) 2015 by iCatch Technology, Inc.
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

#ifndef __NDK_MISC_FUNCS_H__
#define __NDK_MISC_FUNCS_H__

/* The functions in this header file are not released.
 * Host Codes should not use these functions.
 * It may be changed in the future.
 */

#include <stdarg.h>
#include <string.h>
#include <sys/time.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <sp5k_os_api.h>
#include <sp5k_dbg_api.h>
#include <oss_ros_api.h>
#include <oss_sp5k_api.h>
#include <linux/assert.h>

/******************************************************************************
 * Time
 ******************************************************************************/
long ndk_time(long *t);
void ndk_msleep(UINT32 millisecs);
void ndk_usleep(UINT32 microsecs);
int  ndk_gettimeofday(struct timeval *tv);
/* time unit: milli-seconds */
int  ndk_timeping(const char *host, int timeout, SINT64 *hostime, SINT64 *loctime);
/* Can only be used between our system. */
BOOL ndk_gethosttimediff(const char *host, int *diff, int timeout);

/******************************************************************************
 * memory & string
 ******************************************************************************/
char * ndk_strdup(const char *s); /* Use ndk_mem_free to free the returned string. */
char * ndk_strndup(const char *str, unsigned int n);
int    ndk_sscanf(const char *ibuf, const char *fmt, ...);
const char *ndk_strerror(int e);

/******************************************************************************
 * Timeval
 ******************************************************************************/
void   timeval_sub2(struct timeval *minuend, struct timeval const *subtrahend);
void   timeval_sub3(struct timeval *result, struct timeval const *minuend, struct timeval const *subtrahend);
void   timeval_add2(struct timeval *summand, struct timeval const *addend);
void   timeval_add3(struct timeval *result, struct timeval const *summand, struct timeval const *addend);
void   timeval_add_ms(struct timeval *summand, long diff_ms);
long   timeval_diff_ms(struct timeval const *minuend, struct timeval const *subtrahend);
long   timeval_diff_now_ms(struct timeval const *tv);
SINT64 timeval_diff_us(struct timeval const *minuend, struct timeval const *subtrahend);

static inline SINT64 timeval2mtime(struct timeval const *tv) { return ((SINT64)tv->tv_sec)*1000 + ((SINT64)tv->tv_usec)/1000; }
static inline SINT64 timeval2utime(struct timeval const *tv) { return ((SINT64)tv->tv_sec)*1000000 + (SINT64)tv->tv_usec; }
static inline void mtime2timeval(struct timeval *tv, SINT64 t) { tv->tv_sec = t/1000; tv->tv_usec = (t%1000) * 1000; }
static inline void utime2timeval(struct timeval *tv, SINT64 t) { tv->tv_sec = t/1000000; tv->tv_usec = t%1000000; }
static inline SINT64 mtime_now_get() { struct timeval tv; ndk_gettimeofday(&tv); return timeval2mtime(&tv); }
static inline SINT64 utime_now_get() { struct timeval tv; ndk_gettimeofday(&tv); return timeval2utime(&tv); }

/******************************************************************************
 * Thread
 ******************************************************************************/
void*  ndk_thread_create(const char *name, UINT32 priority, void (*start_routine)(void *param), void *param, UINT32 stack_size);
void*  ndk_thread_identify();
void   ndk_thread_destroy(void *thread);
void   ndk_thread_yield();
BOOL   ndk_thread_run_in(void *thread);
/* \brief The default thread priority level used by NDK to create threads.
 * Custom can reference this priority level to create new threads.
 */
int    ndk_thread_get_priority();

/******************************************************************************
 * Critical Section
 ******************************************************************************/
#define ndk_global_lock(p)	(*(p) = ros_enter_critical())
#define ndk_global_unlock(p)	ros_exit_critical(*(p))

/******************************************************************************
 * Debug & Log
 ******************************************************************************/
enum {
	NDK_DBG_FATAL = 0,
	NDK_DBG_ERROR,
	NDK_DBG_WARN,
	NDK_DBG_YELL,
	NDK_DBG_INFO,
	NDK_DBG_NOISY,
	NDK_DBG_CUSTOM,

	NDK_DBG_ALL
};
void ndk_dbgout_line_varg(int lvl, const char* prompt, int line, const char* fmt, va_list ap);
void ndk_dbgout_line(int lvl, const char* prompt, int line, const char* fmt, ...) __attribute__ ((format (printf, 4, 5)));

#define profLogPuts(slot, msg)  sp5kProfLogAddN(slot, msg, strlen(msg))
#define ndk_fatal(args...)      ndk_dbgout_line(NDK_DBG_FATAL, __func__, __LINE__, "" args)
#define ndk_error(args...)      ndk_dbgout_line(NDK_DBG_ERROR, __func__, __LINE__, "" args)
#define ndk_warning(args...)    ndk_dbgout_line(NDK_DBG_WARN,  __func__, __LINE__, "" args)
#define ndk_yell(args...)       ndk_dbgout_line(NDK_DBG_YELL,  __func__, __LINE__, "" args)
#define ndk_info(args...)       ndk_dbgout_line(NDK_DBG_INFO,  __func__, __LINE__, "" args)

#define ndk_err_return(...)     do { \
	ndk_dbgout_line(NDK_DBG_ERROR, __func__, __LINE__, "erret"); \
	return __VA_ARGS__; \
} while(0)

#define NDK_ASSERT(v)           do { if (!(v)) { oss_assert(__FILE__, __LINE__, NULL); } } while(0)
#define NDK_ASSERT_EXPR(v, e)   do { if (!(v)) { e; oss_assert(__FILE__, __LINE__, NULL); } } while(0)

/******************************************************************************
 * Spin Lock
 ******************************************************************************/
#include <linux/mutex.h>
typedef struct mutex	NDKSpinLock;

#define NDK_SPINLOCK_DEFINE(name)	DEFINE_MUTEX(name)

#define ndk_spinlock_init(sl)		mutex_init(sl)
#define ndk_spinlock_get(sl)		mutex_lock(sl)
#define ndk_spinlock_put(sl)		mutex_unlock(sl)
#define ndk_spinlock_destroy(sl)	mutex_destroy(sl)

/******************************************************************************
 * Alarm
 ******************************************************************************/
struct NDKAlarmInfo;
typedef void (*NDKAlarmHandler)(struct NDKAlarmInfo* alarm);
typedef struct NDKAlarmInfo
{
	UINT32 handler;
	UINT32 expires;
	NDKAlarmHandler func;
	unsigned long user_data;
} NDKAlarm;

#define NDK_ALARM_DECLARE(name, pfunc, udata) NDKAlarm name = { 0, 0, pfunc, udata }

static inline void ndk_alarm_init(NDKAlarm *alarm) { memset(alarm, 0, sizeof(NDKAlarm)); }

// Set func and data. The alarm will be stopped.
int  ndk_alarm_set(NDKAlarm *alarm, NDKAlarmHandler func, unsigned long data);
// expires: > 0, expired time; 0: execute alarm function immediately
int  ndk_alarm_start(NDKAlarm *alarm, UINT32 expires/*milli-seconds*/);
void ndk_alarm_stop(NDKAlarm *alarm);
void ndk_alarm_delete(NDKAlarm *alarm);

/******************************************************************************
 * Byte Swap
 ******************************************************************************/
UINT64 bswap_64(UINT64 v);
UINT32 bswap_32(UINT32 v);
UINT16 bswap_16(UINT16 v);

void   bswap_write_32(UINT8 *p, UINT32 a);
UINT16 bswap_read_16(const UINT8 *p);
UINT32 bswap_read_32(const UINT8 *p);

#define CONST_SWAP_16(x) ((UINT16)(			\
	(((UINT16)(x) & (UINT16)0x00ffU) << 8) |	\
	(((UINT16)(x) & (UINT16)0xff00U) >> 8)))

#define CONST_SWAP_32(x) ((UINT32)(			\
	(((UINT32)(x) & (UINT32)0x000000ffUL) << 24) |	\
	(((UINT32)(x) & (UINT32)0x0000ff00UL) <<  8) |	\
	(((UINT32)(x) & (UINT32)0x00ff0000UL) >>  8) |	\
	(((UINT32)(x) & (UINT32)0xff000000UL) >> 24)))

#if __BYTE_ORDER == __LITTLE_ENDIAN
#define platform_htons(a)  CONST_SWAP_16(a)
#define platform_ntohs(a)  CONST_SWAP_16(a)
#define platform_htonl(a)  CONST_SWAP_32(a)
#define platform_ntohl(a)  CONST_SWAP_32(a)
#else
#define platform_htons(a)  (a)
#define platform_ntohs(a)  (a)
#define platform_htonl(a)  (a)
#define platform_ntohl(a)  (a)
#endif

/***********************************************************************************
 *  Variable Options
 ***********************************************************************************/
typedef void* NDKVaOptHandle;

enum {
	NDK_VAOPT_LONG    = 'l',
	NDK_VAOPT_DOUBLE  = 'd',
	NDK_VAOPT_PTR     = 'p',
	NDK_VAOPT_STR     = 's'
};

int  ndk_vaopt_init(NDKVaOptHandle *h);
void ndk_vaopt_reset(NDKVaOptHandle h);
void ndk_vaopt_destroy(NDKVaOptHandle h);
void ndk_vaopt_dump(NDKVaOptHandle h);
void ndk_vaopt_foreach(NDKVaOptHandle h,
	void (*cb)(UINT32 id, int type, void *val, int len, void *udata),
	void *udata);

int ndk_vaopt_push_long(NDKVaOptHandle h, UINT32 id, long val);
int ndk_vaopt_push_double(NDKVaOptHandle h, UINT32 id, double val);
int ndk_vaopt_push_pointer(NDKVaOptHandle h, UINT32 id, void *ptr);
int ndk_vaopt_push_string(NDKVaOptHandle h, UINT32 id, const char *s);

long ndk_vaopt_get_long(NDKVaOptHandle h, UINT32 id, long defval);
double ndk_vaopt_get_double(NDKVaOptHandle h, UINT32 id, double defval);
void *ndk_vaopt_get_pointer(NDKVaOptHandle h, UINT32 id, void *defptr);
const char *ndk_vaopt_get_string(NDKVaOptHandle h, UINT32 id, const char *defstr);

/******************************************************************************
 * From BASEFW
 ******************************************************************************/
void memdump(const void *p, UINT32 size);

/******************************************************************************
 * misc
 ******************************************************************************/
/* Get global temporary buffer. Buffer size 64, buffer count 8. */
#define NDK_TEMP_BUFFER_SIZE	252
char *ndk_temp_buffer_get();

const char *f2str(double d, int precision);
const char *tv2str(struct timeval const* tv);
const char *tv2str_now();
const char *u642xstr(UINT64 *val);

#define NDK_ALIGN_TO(x, n)      ( ((x)+((n)-1)) & ~((n)-1) )
#define NDK_ARRAY_SIZE(a)       (sizeof(a)/sizeof((a)[0]))

#define NDK_MIN(a, b) ({ \
	typeof(a) _123a = (a); \
	typeof(b) _123b = (b); \
	_123a < _123b ? _123a : _123b; \
})

#define NDK_MAX(a, b) ({ \
	typeof(a) _123a = a; \
	typeof(b) _123b = b; \
	_123a < _123b ? _123b : _123a; \
})

/******************************************************************************
 * VFP
 ******************************************************************************/
#if defined(SPCA6500) && defined(TX_ENABLE_VFP_SUPPORT)
extern void tx_thread_vfp_enable(void);
extern void tx_thread_vfp_disable(void);
#else
#define tx_thread_vfp_enable()		((void)0)
#define tx_thread_vfp_disable()		((void)0)
#endif /*#ifdef TX_ENABLE_VFP_SUPPORT*/

#ifdef __cplusplus
}
#endif

#endif
