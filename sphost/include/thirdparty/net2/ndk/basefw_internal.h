/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2012 by iCatch Technology Co., Ltd.             *
 *                                                                        *
 *  This software is copyrighted by and is the property of Sunplus        *
 *  Technology Co., Ltd. All rights are reserved by Sunplus Technology    *
 *  Co., Ltd. This software may only be used in accordance with the       *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice MUST not be removed or modified without prior   *
 *  written consent of Sunplus Technology Co., Ltd.                       *
 *                                                                        *
 *  Sunplus Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  Sunplus Technology Co., Ltd.                                          *
 *  19, Innovation First Road, Science-Based Industrial Park,             *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 **************************************************************************/
#ifndef __BASEFW_INTERNAL_H__
#define __BASEFW_INTERNAL_H__

#include <ndk_types.h>
#include <sys/time.h>

#include <oss_ros_api.h>

#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////////////
// Internal baseFW Symbols and APIs.
#define ROS_SUCCESS  0

int getch(void);
int puts(const char* s);
void tmrTimeStampGet(struct timeval *tv);
void cmdParseToArgv(char *cmd,int *pargc, char *argv[]);
void callStack(void);

UINT32 tmrTimeClockGet();
UINT32 tmrMsTimeGet(void);
UINT32 vfsFlush(UINT32);
void cmdProcess(char *cmd);

void *osMemRealloc(void *pori, UINT32 size);
void *osMemAlloc(UINT32 size);
void *osMemAllocCache(UINT32 size);
UINT32 osMemFree(void *pointer);

void  *memByteCreate(char *name, void *ptr, UINT32 size);
UINT32 memByteDestroy(void *ptr, UINT32 force);
void  *memByteAlloc(UINT32 size, void *pcb, void *psmall, UINT32 smallthrs, UINT32 isbot);

// Extended 5K API and Symbols by NDK.
#ifndef sp5kOsEventFlagsClear
# define sp5kOsEventFlagsClear(e, f)  sp5kOsEventFlagsSet(e, ~(f), TX_AND)
#endif

#define SP5K_QUEUE_EMPTY              0x0A

// PS-DUMP
unsigned _dbg_proc_ps_en(unsigned en);
void     _dbg_proc_ps_dump(unsigned n);
UINT32   osTaskInfoDisplay(UINT32 mode);
void     swWatchDogDisable(void);

int (*uartGetchRedirect(int (*pf) (void)) ) (void);

#define ENTER_CRITICAL(x)	(x) = ros_enter_critical()
#define EXIT_CRITICAL(x)	ros_exit_critical(x)

void sioCtrl(UINT32 opt);

#ifdef __cplusplus
}
#endif

#endif
