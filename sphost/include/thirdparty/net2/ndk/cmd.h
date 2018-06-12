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

#ifndef __NDK_CMD_SHELL_H__
#define __NDK_CMD_SHELL_H__

#include <ndk_types.h>
#include <cmd.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NDKCmdInfo
{
	const char* name;
	int   bgmode_and_stack_size; /* > 0: Run in background mode.
					Number is the stack size in K bytes.*/
	int   argc_min;
	void  (*func)(int argc, char **argv); // can be NULL. NULL --> seperator
	const char* desc;
	struct NDKCmdInfo* next;
} NDKCmdInfo;

void ndk_cmd_register(NDKCmdInfo *cmds, UINT32 nr);
void ndk_cmd_register_syscmd(cmd_t *cmds, UINT32 nr);

/* Convert argv array to command string. */
char *ndk_cmd_argv2str(char *str, int str_len, int argc, char **argv);
int  ndk_cmd_gets(char *cmdbuf, int buflen);
void ndk_cmd_run(int argc, char *argv[], NDKCmdInfo *cmdList, BOOL allowBGMode);
/* If argc == 0, enter the sub-command shell. Else, execute the command directly. */
void ndk_cmd_shell(const char *prompt, int argc, char *argv[], NDKCmdInfo *cmdList, BOOL allowBGMode);
BOOL ndk_cmd_set_bgmode(BOOL on);

// Will be called in info command
typedef struct NDKInfoFunc
{
	struct NDKInfoFunc *next;
	const char *name;
	void (*func)();
} NDKInfoFunc;

void ndk_cmd_global_init();
void ndk_cmd_add_info_func(NDKInfoFunc *infofunc);

void ndk_cmd_dbgswitch_add(const char *name, long *addr);
void ndk_cmd_dbgswitch_dump();
void ndk_cmd_dbgswitch_sset(const char *name, long val);
void ndk_cmd_dbgswitch_iset(int idx, long val);

#ifdef __cplusplus
}
#endif

#endif

