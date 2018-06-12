/**************************************************************************
 *
 *       Copyright (c) 2006-2013 by iCatch Technology, Inc.
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
 *  Hsin-Chu, Taiwan, R.O.C.
 *
 **************************************************************************/
/**
 * @file sp5k_os_api.h
 *
 * @author Stephen Hung
 */

#ifndef _SP5K_OS_API_H_
#define _SP5K_OS_API_H_

#ifdef   __cplusplus
extern   "C" {
#endif

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "common.h"
#include "os/os_def.h"

/**************************************************************************
 *                          C O N S T A N T S
 **************************************************************************/
/* Error code */
#define SP5K_SUCCESS  0x00000000
#define SP5K_ERROR    0x00000001

typedef UINT32 SP5K_THREAD;
typedef UINT32 SP5K_QUEUE;
typedef UINT32 SP5K_MUTEX;
typedef UINT32 SP5K_SEMAPHORE;
typedef UINT32 SP5K_EVENT_FLAGS_GROUP;

#ifndef TX_INT_ENABLE
/* Following macro have been defined in tx_port.h which is not included normally.
   It happens when customer need to use TX API directly and SP5K OS API in the same module */
#define TX_INT_ENABLE    0x0000FF00  /* Enable interrupt value	 */
#define TX_NO_WAIT          0
#define TX_WAIT_FOREVER     0xFFFFFFFFUL
#define TX_AND              2
#define TX_AND_CLEAR        3
#define TX_OR               0
#define TX_OR_CLEAR         1
#define TX_1_ULONG          1
#define TX_2_ULONG          2
#define TX_4_ULONG          4
#define TX_8_ULONG          8
#define TX_16_ULONG         16
#define TX_NO_TIME_SLICE    0
#define TX_MAX_PRIORITIES   32
#define TX_AUTO_START       1
#define TX_DONT_START       0
#define TX_AUTO_ACTIVATE    1
#define TX_NO_ACTIVATE      0
#define TX_TRUE             1
#define TX_FALSE            0
#define TX_NULL             0
#define TX_LOOP_FOREVER     1
#define TX_INHERIT          1
#define TX_NO_INHERIT       0
typedef void   VOID;
typedef char   CHAR;
typedef unsigned char UCHAR;
typedef long   LONG;
typedef UINT32 ULONG;
typedef UINT32 UINT;
#endif

/**
 * \note The error code is borrowed from ThreadX error code.
 * Host can check the return value from sp5kOs API to realize 
 * what error happened.
 */
#define SP5K_OS_SUCCESS          0x00
#define SP5K_OS_DELETED          0x01
#define SP5K_OS_NO_MEMORY        0x10
#define SP5K_OS_POOL_ERROR       0x02
#define SP5K_OS_PTR_ERROR        0x03
#define SP5K_OS_WAIT_ERROR       0x04
#define SP5K_OS_SIZE_ERROR       0x05
#define SP5K_OS_GROUP_ERROR      0x06
#define SP5K_OS_NO_EVENTS        0x07
#define SP5K_OS_OPTION_ERROR     0x08
#define SP5K_OS_QUEUE_ERROR      0x09
#define SP5K_OS_QUEUE_EMPTY      0x0A
#define SP5K_OS_QUEUE_FULL       0x0B
#define SP5K_OS_SEMAPHORE_ERROR  0x0C
#define SP5K_OS_NO_INSTANCE      0x0D
#define SP5K_OS_THREAD_ERROR     0x0E
#define SP5K_OS_PRIORITY_ERROR   0x0F
#define SP5K_OS_START_ERROR      0x10
#define SP5K_OS_DELETE_ERROR     0x11
#define SP5K_OS_RESUME_ERROR     0x12
#define SP5K_OS_CALLER_ERROR     0x13
#define SP5K_OS_SUSPEND_ERROR    0x14
#define SP5K_OS_TIMER_ERROR      0x15
#define SP5K_OS_TICK_ERROR       0x16
#define SP5K_OS_ACTIVATE_ERROR   0x17
#define SP5K_OS_THRESH_ERROR     0x18
#define SP5K_OS_SUSPEND_LIFTED   0x19
#define SP5K_OS_WAIT_ABORTED     0x1A
#define SP5K_OS_WAIT_ABORT_ERROR 0x1B
#define SP5K_OS_MUTEX_ERROR      0x1C
#define SP5K_OS_NOT_AVAILABLE    0x1D
#define SP5K_OS_NOT_OWNED        0x1E
#define SP5K_OS_INHERIT_ERROR    0x1F

/* Thread's priority */
typedef enum _sp5kOsThreadPriority_e {
	TX_PRIORITY_0  = 0,
	TX_PRIORITY_1  = 1,
	TX_PRIORITY_2  = 2,
	TX_PRIORITY_3  = 3,
	TX_PRIORITY_4  = 4,
	TX_PRIORITY_5  = 5,
	TX_PRIORITY_6  = 6,
	TX_PRIORITY_7  = 7,
	TX_PRIORITY_8  = 8,
	TX_PRIORITY_9  = 9,
	TX_PRIORITY_10 = 10,
	TX_PRIORITY_11 = 11,
	TX_PRIORITY_12 = 12,
	TX_PRIORITY_13 = 13,
	TX_PRIORITY_14 = 14,
	TX_PRIORITY_15 = 15,
	TX_PRIORITY_16 = 16,
	TX_PRIORITY_17 = 17,
	TX_PRIORITY_18 = 18,
	TX_PRIORITY_19 = 19,
	TX_PRIORITY_20 = 20,
	TX_PRIORITY_21 = 21,
	TX_PRIORITY_22 = 22,
	TX_PRIORITY_23 = 23,
	TX_PRIORITY_24 = 24,
	TX_PRIORITY_25 = 25,
	TX_PRIORITY_26 = 26,
	TX_PRIORITY_27 = 27,
	TX_PRIORITY_28 = 28,
	TX_PRIORITY_29 = 29,
	TX_PRIORITY_30 = 30,
	TX_PRIORITY_31 = 31,
} sp5kOsThreadPriority_e;

/* Thread's priority */
typedef enum _sp5kOsThreadCtrlOpt_e {
	SP5K_OS_THREAD_CTRL_SUSPEND,
	SP5K_OS_THREAD_CTRL_RESUME
} sp5kOsThreadCtrlOpt_e;

/**************************************************************************
 *              F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
/* Os Thread */
SP5K_THREAD *sp5kOsThreadCreate(char  *name_ptr, VOID  (*entry_function)(ULONG), ULONG entry_input, UINT  priority, UINT  preempt_threshold, ULONG time_slice_milli_sec, UINT  auto_start);
SP5K_THREAD *sp5kOsThreadCreateEx(char  *name_ptr, VOID  (*entry_function)(ULONG),
	ULONG entry_input, UINT  priority, UINT  preempt_threshold,
	ULONG time_slice_milli_sec, UINT  stack_size, UINT  auto_start);
UINT32 sp5kOsThreadControl(SP5K_THREAD *thread_ptr, UINT thread_ctrl);
UINT32 sp5kOsThreadTerminate(SP5K_THREAD *thread_ptr);
UINT32 sp5kOsThreadDelete(SP5K_THREAD *thread_ptr);
/**
 Put the calling thread to sleep for specific milliseconds.
 @param[in] ms milliseconds to sleep. sleep time accuracy is OS dependent.
 On iCatOS, where 1 osTick = 10 milliseconds, sleeping time would be
   ([-1 .. 0] + ms/10) x osTick milliseconds
   (plus OS scheduling latency to active the calling thread).
 @retVal 0 SUCCESS
 @return o/w errors or being aborted
*/
UINT32 sp5kOsThreadSleep(UINT32 ms);

/* Os queue */
UINT32 sp5kOsQueueCreate(SP5K_QUEUE *queue_ptr, CHAR *name_ptr, UINT message_size, VOID *queue_start, ULONG queue_size);
UINT32 sp5kOsQueueDelete(SP5K_QUEUE *queue_ptr);
UINT32 sp5kOsQueueReceive(SP5K_QUEUE *queue_ptr, VOID *destination_ptr, ULONG wait_option);
UINT32 sp5kOsQueueSend(SP5K_QUEUE *queue_ptr, VOID *source_ptr, ULONG wait_option);
UINT32 sp5kOsQueueCntGet(SP5K_QUEUE *queue_ptr, UINT32 *cnt, UINT32 *size);

/* Os Mutex */
UINT32 sp5kOsMutexCreate(SP5K_MUTEX *mutex_ptr, CHAR *name_ptr, UINT inherit);
UINT32 sp5kOsMutexDelete(SP5K_MUTEX *mutex_ptr);
UINT32 sp5kOsMutexGet(SP5K_MUTEX *mutex_ptr, ULONG wait_option);
UINT32 sp5kOsMutexPut(SP5K_MUTEX *mutex_ptr);

/* Os Event */
UINT32 sp5kOsEventFlagsCreate(SP5K_EVENT_FLAGS_GROUP *event_ptr, CHAR *name_ptr);
UINT32 sp5kOsEventFlagsDelete(SP5K_EVENT_FLAGS_GROUP *event_ptr);
UINT32 sp5kOsEventFlagsSet(SP5K_EVENT_FLAGS_GROUP *event_ptr, ULONG flags, ULONG set_option);
UINT32 sp5kOsEventFlagsGet(SP5K_EVENT_FLAGS_GROUP *event_ptr, ULONG req_flags, UINT get_option, ULONG *flags_ptr, ULONG wait_option);

/* Semaphore */
UINT32 sp5kOsSemaphoreCreate(SP5K_SEMAPHORE *semaphore_ptr, CHAR *name_ptr, UINT32 init_val);
UINT32 sp5kOsSemaphoreGet(SP5K_SEMAPHORE *semaphore_ptr, ULONG wait_option);
UINT32 sp5kOsSemaphoreCntGet(SP5K_SEMAPHORE *semaphore_ptr, UINT32 *pcnt);
UINT32 sp5kOsSemaphorePut(SP5K_SEMAPHORE *semaphore_ptr);
UINT32 sp5kOsSemaphoreDestroy(SP5K_SEMAPHORE *semaphore_ptr);

/* Misc */
UINT32 sp5kOsTimeGet(void);
void*  sp5kMalloc(UINT32 size);
void   sp5kFree(void *pbuf);
void*  sp5kMallocCache(UINT32 size);
void*  sp5kMallocExt(UINT32 size, UINT32 opt);
void*  sp5kMallocCacheExt(UINT32 size);
UINT32 sp5kOsMemBlockTagSet(void *ptr, const char *name, UINT32 line);
#if defined(__linux__)
void*  sp5kPhyAddrGet(void *addr);
#endif

#ifdef   __cplusplus
}
#endif

#endif  /* _SP5K_OS_API_H_ */

