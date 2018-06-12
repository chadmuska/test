/**************************************************************************
 *
 *       Copyright (c) 2010-2016 by iCatch Technology, Inc.
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
 *  Author: Anmin Deng
 *
 **************************************************************************/
/* DO NOT EDIT the next 2 lines for auto revision by subversion
$Rev: 26010 $
$Date:: 2016-04-20 16:06:10#$
 */
#ifndef _MULTIBOOT_API_H_
#define _MULTIBOOT_API_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "common.h"


/** \note
 * the definitions of the following multi boot stage level constants
 * are referred by host/Makefile and multiboot.def too during project building.
 * the format of the following constant #define's is not allowed to change.
 * the value of "MULTI_BOOT_STAGE_ALL" must be 32
 */
#define MULTI_BOOT_STAGE_OPEN        1
#define MULTI_BOOT_STAGE_PROF        1
#define MULTI_BOOT_STAGE_PWR_GPIO    1
#define MULTI_BOOT_STAGE_VFS         4
#define MULTI_BOOT_STAGE_DEV_START   5
#define MULTI_BOOT_STAGE_DEV_RTC     6
#define MULTI_BOOT_STAGE_DEV_TEST    9
#define MULTI_BOOT_STAGE_DISK_RSV    10
#define MULTI_BOOT_STAGE_RESOURCE    10
#define MULTI_BOOT_STAGE_DISP        11
#define MULTI_BOOT_STAGE_SENSOR      12
#define MULTI_BOOT_STAGE_DEV_SECOND  13
#define MULTI_BOOT_STAGE_DISK_MEDIA  14
#define MULTI_BOOT_STAGE_DEV_AUDIO   15
#define MULTI_BOOT_STAGE_DEV_USB     16
#define MULTI_BOOT_STAGE_MEDIA       21
#define MULTI_BOOT_STAGE_MODESW      22
#define MULTI_BOOT_STAGE_SYS_RES     23
#define MULTI_BOOT_STAGE_APP_START   24
#define MULTI_BOOT_STAGE_CMD         25
#define MULTI_BOOT_STAGE_HOST        28
#define MULTI_BOOT_STAGE_APP_DELAY   29
#define MULTI_BOOT_STAGE_SLIDE       32
#define MULTI_BOOT_STAGE_ALL         32



/** \note the max level of multi boot stage is 32, where it is represented
 * by a UINT32 flags in the multiboot system */


/** \brief to wait for specific boot stage image loaded to DRAM ready */
extern void multibootLoadWait(UINT32 stage);

/** \brief to assert for specific boot stage image loaded to DRAM ready */
extern void multibootLoadAssert(UINT32 stage);

/** \brief to run the specific stage of multi boot callback */
extern void multibootCb(UINT32 stage);

/** \brief to run the specific stage of custom multi boot callback */
extern void multibootCustomCb(UINT32 stage);

/**
 * \brief to compare the current system timestamp to the specific timestamp
 * \param timestamp, the specific timestamp, in ms
 * \return the difference from the specific timestamp to
 * the system current timestamp, unit in ms. return value < 0 means the system
 * time had "timestamp" ms passed the specific timestamp; > 0 means it still
 * takes "timestamp" more ms for the specific timestamp to come
 * \example
 * SINT32 diff = multibootTimestampCmp(150);
 * if (diff > 0) ros_thread_sleep(diff);
 */
extern SINT32 multibootTimestampCmp(UINT32 timestamp);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef _MULTIBOOT_API_H_ */
