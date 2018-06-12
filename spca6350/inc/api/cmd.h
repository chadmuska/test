/**************************************************************************
 *
 *       Copyright (c) 2005-2016 by iCatch Technology, Inc.
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
#ifndef _CMD_H_
#define _CMD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include "common.h"

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
struct cmd_s;
typedef struct cmd_s cmd_t;
typedef void (*cmdHandler)(int argc, char *argv[]);

struct cmd_s {
	const char *cmd;
	cmdHandler phandler;
	const char *phelp;
	cmd_t      *pnext;
};

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
/* extern and listed for legacy interface, calling not recommended */
char *cmdArgvToStr(int argc, char *argv[]);
void cmdParseToArgv(char *cmd, int *pargc, char *argv[]);
void cmdProcess(char *cmd);

void cmdMonitor(void);
void cmdMonitorChar(int c);
char *cmdMonitorGetS(void);

/* ---- 1st level cmd structure ---------------------------------------- */
void cmdBatchRegister(cmd_t *);

/* ---- sub levels cmd structure --------------------------------------- */
/**
 * \brief to register a deeper sub-level command structure list
 * \param pcmds, points to the head of the sub-level command structure list;
 * should be null pointer at initializing 1st time calling;
 * if calling this function again later to merge more command items to a list,
 * the param "pcmds" should be the return value of last call of this function
 * \param pcmd, points to the command structure list to be installed,
 * there must be a tailing member {NULL,NULL,NULL,NULL} to terminate the list
 * \return the pointer which must later be passed to cmdSubProcess() or
 * if more items to be merged in a previous sub command list registered by
 * cmdSubBatchRegister()
 */
cmd_t *cmdSubBatchRegister(cmd_t *pcmds, cmd_t *pcmd);

/**
 * \brief called in a command routine to process deeper sub-level commands
 * \param argc,
 * \param argv, passed by previous level command, argument count and vector,
 * as in standard main()'s style
 * \param pcmds, the sub-level command structure previously registered by
 * cmdSubBatchRegister()
 * \param fp, a failure mode handler if command line processed failed in
 * this level; a null means only to display an error
 */
void cmdSubProcess(int argc, char *argv[], cmd_t *pcmds,
		void (*fp)(int argc, char *argv[]));


void osDeadCmdMonitor(void);


/* ---- multi threaded test "cmd_multi_thr.c" -------------------------- */

#define N_TEST_THR_MAX  30

void multi_test_start_wait(void);
UINT32 multi_test_end_accept(void);
void multi_test_end_set(UINT32 mask);
UINT32 multi_test_thr_run(void (*thrlist[])(UINT32 a),
		UINT32 activemask, UINT32 expirems);
void multi_test_thr_get(void);
void multi_test_thr_put(void);
int test_memcmp(const void *p0, const void *p1, size_t n);


#ifdef __cplusplus
}
#endif

#endif /* _CMD_H_ */
