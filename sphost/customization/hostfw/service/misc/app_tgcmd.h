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
/* _APP_TGCMD_H_ */

#ifndef _APP_TGCMD_H_
#define _APP_TGCMD_H_


void tgCmdMake(UINT32 addr, UINT32 data, UINT8 *regdata);
char *tgCmdModeStrGet(UINT32 sp5kmode);
void tgCmdUpdate(UINT32 sp5kmode, UINT8 *reg);
void tgCmdDump(UINT32 sp5kmode);

void tgCmdExec(UINT32 sp5kmode);
void tgCmdSet(UINT8 *buf);
void tgCmdGet(UINT8 *buf);
void tgCmdClear(UINT32 sp5kmode);

#endif // _APP_TGCMD_H_
