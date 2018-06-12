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
#ifndef __APP_SCRIPT_H__
#define __APP_SCRIPT_H__

#include "common.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
enum {
	SCRIPT_ERR_OK,
	SCRIPT_ERR_FILE 						= 0x100000,
	SCRIPT_ERR_MALLOC 						= 0x110000,
	SCRIPT_ERR_CODE_SIZE_EXCEED 			= 0x120000,
	SCRIPT_ERR_SYNTAX 						= 0xf00000,
	SCRIPT_ERR_SYNTAX_INVALID_CMD			= SCRIPT_ERR_SYNTAX,
	SCRIPT_ERR_SYNTAX_EXPECT_NUMBER			= SCRIPT_ERR_SYNTAX|0x10000,
	SCRIPT_ERR_SYNTAX_TOO_MANY_NESTED_LOOP	= SCRIPT_ERR_SYNTAX|0x20000,
	SCRIPT_ERR_SYNTAX_LOOP_UNMATCH			= SCRIPT_ERR_SYNTAX|0x30000,
	SCRIPT_ERR_SYNTAX_INVALID_KEY			= SCRIPT_ERR_SYNTAX|0x40000,
};
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

UINT32 appScriptCompile(char *srcbuf);
UINT32 appScriptLoad(char *fname);
UINT32 appScriptStart(UINT8 delaySec);
UINT32 appScriptAutoRun(UINT32 drive);
UINT32 appScriptCIPARun(UINT32 drive);
UINT32 appScriptQuickRun(char *code);
UINT32 appScriptKeyLogSave(UINT32 *keyLogBuf, UINT32 keyLogCount, char *fname);
UINT32 appScriptLibRun(UINT32 idx);

#endif /* __APP_SCRIPT_H__ */

