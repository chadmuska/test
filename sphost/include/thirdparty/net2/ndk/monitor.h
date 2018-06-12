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
#ifndef __NDK_MONITOR_H__
#define __NDK_MONITOR_H__

#include <ndk/common.h>

#ifdef __cplusplus
extern "C" {
#endif

enum { NDK_MON_VAR_NR_MAX = 20, NDK_MON_VAR_LEN_MAX = 32 };
enum { NDK_MON_RES_OK = 0, NDK_MON_RES_INVALID, NDK_MON_RES_BREAK };

void ndk_mon_reset();
void ndk_mon_add_var(const char* var_name, UINT32 var_id, int width,
	int (*getval_func)(UINT32 var_id, char *val, int width, unsigned long param),
	unsigned long param);
void ndk_mon_start(int expires);
void ndk_mon_stop();

#ifdef __cplusplus
}
#endif

#endif
