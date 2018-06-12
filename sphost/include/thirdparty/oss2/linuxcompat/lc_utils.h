/**************************************************************************
 *
 *       Copyright (c) 2009-2017 by iCatch Technology, Inc.
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
 **************************************************************************/

#ifndef __LC_UTILS_H__
#define __LC_UTILS_H__

#include <cmd.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OSS_DEFINE_SUBCMDS(name, help, subcmds) \
static cmd_t subcmds[]; \
static cmd_t *oss_subcmds_list_##name = NULL; \
static void oss_subcmds_entry_func_##name (int argc, char *argv[]) { \
	if (!oss_subcmds_list_##name) oss_subcmds_list_##name = cmdSubBatchRegister(0, subcmds); \
	cmdSubProcess(argc, argv, oss_subcmds_list_##name, NULL); \
} \
static cmd_t oss_subcmds_entry_array_##name [] = { \
	{#name, oss_subcmds_entry_func_##name, help, NULL}, \
	{NULL, NULL, NULL, NULL} \
}

#define OSS_REGISTER_SUBCMDS(name) do { \
	oss_register_main_cmd(); \
	oss_cmd_list = cmdSubBatchRegister(oss_cmd_list, oss_subcmds_entry_array_##name); \
} while(0)

extern void oss_register_main_cmd();
extern cmd_t *oss_cmd_list;

#ifdef __cplusplus
}
#endif

#endif
