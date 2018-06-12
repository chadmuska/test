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
 /*--------------------------------------------------------------------------*
 *	File name	: app_script.c
 *	Function	: app script function 
 *	First editor: Weikai Hsu
 *	First date	:
 *	History		: 	
 *	            : 2018.06.01  Weikai Hsu "Added File Header"
 *	
 *	
 *	
 *--------------------------------------------------------------------------*/
#define HOST_DBG 0
#include "common.h"
#include <stdio.h>
#include <string.h>

#include "api/sp5k_global_api.h"
#include "api/sp5k_fs_api.h"
#include "api/sp5k_os_api.h"
#include "api/sp5k_disk_api.h"

#include "app_com_api.h"
#include "app_com_def.h"
#include "app_script.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
enum 
{
	TK_LOOP, 
	TK_KEY, 
	TK_KEYDOWN,
	TK_KEYUP, 
	TK_SLEEP, 
	TK_LOOPEND,
	TK_WAKEUP,
	TK_NUMBER,
	TK_UNKNOWN,
	TK_EOS,
};

enum 
{
	PCODE_NOP,
	PCODE_HALT,
	PCODE_JZ,
	PCODE_JNZ,
	PCODE_MOV,
	PCODE_ADD,
	PCODE_SUB,
	PCODE_SLEEP=0x80,
	PCODE_KEY,
	PCODE_WAKEUP,
};


/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define LOOP_DEPTH_MAX	4

#define LOOP_REG	0

#define SKIPPABLE(ch)	((ch)==' ' || (ch)=='\t' || (ch)==0xd || (ch)==0xa)
#define TOLOWER(ch)		((ch)-'A'+'a')
#define ISDIGIT(ch)		('0'<=(ch) && (ch)<='9')

#define TIMER_BASE_MS	50
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

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
 
/* ========================================================== */
/* tokenizer */
/* Inplace token parsing replaces delimiter with \0 so we need to recover it later. */
static char *ppatch=NULL,patchCh;
static char *tokenLast=NULL;
/*--------------------------------------------------------------------------*
* Function name : parserTokenNullPatch 
* Function		: parser Token Null Patch
* Return value	: static void 
* Parameter1	: void
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static void parserTokenNullPatch(void)
{
	if (ppatch) 
	{
		*ppatch = patchCh;
		ppatch = NULL;
	}
}
/*--------------------------------------------------------------------------*
* Function name : parserTokenGet 
* Function		: get parser Token
* Return value	: static char*
* Parameter1	: char **pbuf
* History	    :
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static char *
parserTokenGet(
	char **pbuf
)
{
	char *buf=*pbuf;
	char *token;
	while (SKIPPABLE(*buf))
	{
		buf++;
	}
	
	if (*buf==0) 
	{
		token = NULL;
		if (ppatch)
		{
			*ppatch = patchCh;
		}
	} 
	else 
	{
		token = buf;
		while (*buf && !SKIPPABLE(*buf))
		{
			if ('A'<=*buf && *buf<='Z')
			{
				*buf = TOLOWER(*buf);
			}
			buf++;
		}
		
		if (*buf) 
		{ /* set NULL for end of token, remember pos & ch for later recover */
			if (ppatch)
			{
				*ppatch = patchCh;  /* ugly, recover char replace by null char previously */
			}
			ppatch = buf; 
			patchCh = *buf;
			*buf++ = 0;
		}
	}
	*pbuf = buf;
	return tokenLast=token;
}

/* one token unget only */
/*--------------------------------------------------------------------------*
* Function name : parserTokenUnget 
* Function		: Unget parser Token
* Return value	: static char*
* Parameter1	: char **pbuf
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static char *
parserTokenUnget(
	char **pbuf
)
{
	if (tokenLast && *tokenLast) 
	{
		*pbuf = tokenLast;
		parserTokenNullPatch();
	}
	return *pbuf;
}

/* ========================================================== */
/* keyword for our script */
static struct 
{
	UINT8 id;
	char *name;
} tokenTable[]=
{
	{ TK_LOOP,		"loop" },
	{ TK_LOOPEND, 	"loopend" },
	{ TK_KEY, 		"key" },
	{ TK_KEYDOWN,	"keydown" },
	{ TK_KEYUP,		"keyup" },
	{ TK_SLEEP, 	"sleep" },
	{ TK_WAKEUP, 	"wakeup" },
};
/*--------------------------------------------------------------------------*
* Function name : parserTokenLookup 
* Function		: parser Token Lookup
* Return value	: static UINT8
* Parameter1	: char *token
* Parameter2	: UINT32 *pvalue
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static UINT8
parserTokenLookup(
	char *token,
	UINT32 *pvalue
)
{ /* hash it if lots of cmd */
	int i;
	if (token==NULL)
	{
		return TK_EOS;
	}
	
	for (i=0 ; i<COUNT_OF(tokenTable) ; i++) 
	{
		if (strcmp(token, tokenTable[i].name)==0) 
		{
			return tokenTable[i].id;
		}
	}
	
	if (ISDIGIT(*token)) 
	{ /* no negative number for now */
		UINT32 v;
		v = *token - '0';
		token++;
		while (ISDIGIT(*token)) 
		{
			v = v*10 + (*token - '0');
			token++;
		}
		if (*token!=0)
		{
			return TK_UNKNOWN;
		}
		if (pvalue)
		{
			*pvalue = v;
		}
		return TK_NUMBER;
	}
	return TK_UNKNOWN;
}

/* ========================================================== */
/* small stack for loop/loopend */

static int loopdepth;
static UINT16 loopaddr[LOOP_DEPTH_MAX];
/*--------------------------------------------------------------------------*
* Function name : LoopAddrPush 
* Function		: loop address push
* Return value	: static UINT32
* Parameter1	: UINT16 ip
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static UINT32 LoopAddrPush(UINT16 ip)
{
	if (loopdepth>=LOOP_DEPTH_MAX)
	{
		return FAIL;
	}
	loopaddr[loopdepth++] = ip;
	return SUCCESS;
}
/*--------------------------------------------------------------------------*
* Function name : LoopAddrPop 
* Function		: loop address pop
* Return value	: static UINT32
* Parameter1	: UINT16 *pip
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static UINT32 LoopAddrPop(UINT16 *pip)
{
	if (loopdepth==0)
	{
		return FAIL;
	}
	*pip = loopaddr[--loopdepth];
	return SUCCESS;
}
/*--------------------------------------------------------------------------*
* Function name : LoopAddrStackInit 
* Function		: loop address stack init
* Return value	: static void
* Parameter1	: void
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static void LoopAddrStackInit(void)
{
	loopdepth = 0;
}

/* ========================================================== */
/* script pcode gen */

#define ADD_PCODE(cmd, arg, value)	{ 	pcodebuf[ip++] = (cmd); pcodebuf[ip++] = (arg);\
									  		*(UINT16*)(pcodebuf+ip) = (UINT16)(value); ip += 2; }

/*	loop 8
	...
	loopend
	
	wr	loop_reg[loopdepth], 8
   	jz	loopdepth_exit <- need to be patch later
loopdepth:	   ...
   	sub	loop_reg[loopdepth], 1
   	jnz	loopdepth
loopdepth_exit:	
*/

/*--------------------------------------------------------------------------*
* Function name : PCodeGenLoop 
* Function		: pc code gen loop
* Return value	: static UINT16
* Parameter1	: char **psrcbuf
* Parameter2	: UINT8 *pcodebuf
* Parameter3	: UINT16 ip
* Parameter4	: UINT32 *perr
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static UINT16
PCodeGenLoop(
	char **psrcbuf,
	UINT8 *pcodebuf,
	UINT16 ip,
	UINT32 *perr
)
{
	UINT32 v;
	if (parserTokenLookup(parserTokenGet(psrcbuf), &v) != TK_NUMBER)
	{
		*perr = SCRIPT_ERR_SYNTAX_EXPECT_NUMBER;
		return 0;
	}
	ADD_PCODE(PCODE_MOV, LOOP_REG+loopdepth, (UINT16)v);
	ADD_PCODE(PCODE_JZ, LOOP_REG+loopdepth, 0);
	if (LoopAddrPush(ip)!=SUCCESS) 
	{
		*perr = SCRIPT_ERR_SYNTAX_TOO_MANY_NESTED_LOOP;
		return 0;
	}
	return 8;	
}
/*--------------------------------------------------------------------------*
* Function name : PCodeGenLoopend 
* Function		: pc code gen loop end
* Return value	: static UINT16
* Parameter1	: UINT8 *pcodebuf
* Parameter2	: UINT16 ip
* Parameter3	: UINT32 *perr
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static UINT16
PCodeGenLoopend(
	UINT8 *pcodebuf,
	UINT16 ip,
	UINT32 *perr
)
{
	UINT16 iploop;
	if (LoopAddrPop(&iploop)!=SUCCESS)
	{
		*perr = SCRIPT_ERR_SYNTAX_LOOP_UNMATCH;
		return 0;
	}
	ADD_PCODE(PCODE_SUB, LOOP_REG+loopdepth, 1);
	ADD_PCODE(PCODE_JNZ, LOOP_REG+loopdepth, iploop);
	*(UINT16*)(pcodebuf+iploop-2) = ip;
	return 8;
}
/*--------------------------------------------------------------------------*
* Function name : PCodeGenKey 
* Function		: pc code gen key
* Return value	: static UINT16
* Parameter1	: char **psrcbuf
* Parameter2	: UINT8 *pcodebuf
* Parameter3	: UINT16 ip
* Parameter4	: UINT32 *perr
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
/* key xxx N */
static UINT16
PCodeGenKey(
	char **psrcbuf,
	UINT8 *pcodebuf,
	UINT16 ip,
	UINT32 *perr
)
{
	UINT32 key,v;
	if ((key=appKeyLookup(parserTokenGet(psrcbuf))) == APP_KEY_ERROR)
	{
		*perr = SCRIPT_ERR_SYNTAX_INVALID_KEY;
		return 0;
	}
	if (parserTokenLookup(parserTokenGet(psrcbuf), &v) != TK_NUMBER)
	{
		parserTokenUnget(psrcbuf);
		v = 1;
	}
	if ((key | 0x0)==0)
	{ /* key w/o up/down state, just send key msg & wait */
		ADD_PCODE(PCODE_KEY, 0, key);
		if (v>=2) 
		{
			ADD_PCODE(PCODE_SLEEP, 0, v-1);
		}
		return v ? 8 : 4;
	} 
	else 
	{ /* key with state, send key down msg & wait, then send key up msg */
		ADD_PCODE(PCODE_KEY, 0, (IS_GPIO_PRESS_MSG(key)==(SP5K_MSG_GPIO_BUTTON_PRESS<<4)) ? key & 0xFFFFFF0F : key & 0xFFFFFF3F);
		if (v>=2) 
		{
			ADD_PCODE(PCODE_SLEEP, 0, v-1);
		}
		ADD_PCODE(PCODE_KEY, 0, (IS_GPIO_RELEASE_MSG(key)==(SP5K_MSG_GPIO_BUTTON_RELEASE<<4)) ? key & 0xFFFFFF2F : key & 0xFFFFFF5F);
		return v ? 12 : 8;
	}
}
/*--------------------------------------------------------------------------*
* Function name : PCodeGenKeyUpDown 
* Function		: pc code gen key up down
* Return value	: static UINT16
* Parameter1	: char **psrcbuf
* Parameter2	: UINT8 *pcodebuf
* Parameter3	: UINT16 ip
* Parameter4	: UINT8 tokentype
* Parameter5	: UINT32 *perr
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static UINT16
PCodeGenKeyUpDown(
	char **psrcbuf,
	UINT8 *pcodebuf,
	UINT16 ip,
	UINT8 tokentype, /* must be TK_DOWN or TK_UP */
	UINT32 *perr
)
{
	UINT32 key;
	DBG_ASSERT(tokentype==TK_KEYDOWN || tokentype==TK_KEYUP);
	if ((key=appKeyLookup(parserTokenGet(psrcbuf))) == APP_KEY_ERROR)
	{
		*perr = SCRIPT_ERR_SYNTAX_INVALID_KEY;
		return 0;
	}
	ADD_PCODE(PCODE_KEY, 0, key);
	return 4;
}
/*--------------------------------------------------------------------------*
* Function name : PCodeGenWakeUp 
* Function		: pc code gen wakeup
* Return value	: static UINT16
* Parameter1	: char **psrcbuf
* Parameter2	: UINT8 *pcodebuf
* Parameter3	: UINT16 ip
* Parameter4	: UINT32 *perr
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static UINT16
PCodeGenWakeUp(
	char **psrcbuf,
	UINT8 *pcodebuf,
	UINT16 ip,
	UINT32 *perr
)
{
	UINT32 v;
	if (parserTokenLookup(parserTokenGet(psrcbuf), &v) != TK_NUMBER)
	{
		*perr = SCRIPT_ERR_SYNTAX_EXPECT_NUMBER;
		return 0;
	}
	
	if (v)
	{
		ADD_PCODE(PCODE_WAKEUP, 0, v);
		return 4;
	} 
	else
	{
		return 0;
	}
}
/*--------------------------------------------------------------------------*
* Function name : PCodeGenSleep 
* Function		: pc code gen sleep
* Return value	: static UINT16
* Parameter1	: char **psrcbuf
* Parameter2	: UINT8 *pcodebuf
* Parameter3	: UINT16 ip
* Parameter4	: UINT32 *perr
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static UINT16
PCodeGenSleep(
	char **psrcbuf,
	UINT8 *pcodebuf,
	UINT16 ip,
	UINT32 *perr
)
{
	UINT32 v;
	if (parserTokenLookup(parserTokenGet(psrcbuf), &v) != TK_NUMBER) 
	{
		*perr = SCRIPT_ERR_SYNTAX_EXPECT_NUMBER;
		return 0;
	}
	if (v)
	{
		ADD_PCODE(PCODE_SLEEP, 0, v);
		return 4;
	}
	else
	{
		return 0;
	}
}
/*--------------------------------------------------------------------------*
* Function name : PCodeGenHalt 
* Function		: pc code gen halt
* Return value	: static UINT16
* Parameter1	: UINT8 *pcodebuf
* Parameter2	: UINT16 ip
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static UINT16
PCodeGenHalt(
	UINT8 *pcodebuf,
	UINT16 ip
)
{
	ADD_PCODE(PCODE_HALT, 0, 0);
	return 4;
}

/* ========================================================== */
/* script parser main entry & err report */
/*--------------------------------------------------------------------------*
* Function name : scriptErrorReport 
* Function		: report script error
* Return value	: static void
* Parameter1	: UINT32 err
* Parameter2	: char *code
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static void
scriptErrorReport(
	UINT32 err,
	char *code
)
{
	if (err>=SCRIPT_ERR_SYNTAX) 
	{
		UINT16 l,i,idx=err&0xffff;
		char *p,ch;
		for (i=0,l=1 ; i<idx ; i++) 
		{
			if (code[i]==0xa)
				l++;
		}
		for (p=code+idx ; *p!=0 && *p!=0xd && *p!=0xa ; p++)
		{
			;
		}
		ch = *p; *p = 0;
        HOST_PROF_LOG_PRINT(LEVEL_ERROR, "scrp: err[%x] line:%d [%s]", err, l, code+idx);   
		*p = ch;
	}
}
/*--------------------------------------------------------------------------*
* Function name : scriptCompile 
* Function		: script compile
* Return value	: static UINT32 
* Parameter1	: char *srcbuf
* Parameter2	: UINT8 *pcodebuf
* Parameter3	: UINT16 pcodemax
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static UINT32 
scriptCompile(
	char *srcbuf,
	UINT8 *pcodebuf,
	UINT16 pcodemax
)
{
	char *token,*src0=srcbuf;
	UINT16 ip=0;
	UINT32 err=SCRIPT_ERR_OK;
	LoopAddrStackInit();
	while ((token=parserTokenGet(&srcbuf)) != NULL) 
	{
		switch (parserTokenLookup(token, NULL))
		{
		case TK_LOOP:
			ip += PCodeGenLoop(&srcbuf, pcodebuf, ip, &err);
			break;
		case TK_LOOPEND:
			ip += PCodeGenLoopend(pcodebuf, ip, &err);
			break;
		case TK_KEY:
			ip += PCodeGenKey(&srcbuf, pcodebuf, ip, &err);
			break;
		case TK_KEYDOWN:
			ip += PCodeGenKeyUpDown(&srcbuf, pcodebuf, ip, TK_KEYDOWN, &err);
			break;
		case TK_KEYUP:
			ip += PCodeGenKeyUpDown(&srcbuf, pcodebuf, ip, TK_KEYUP, &err);
			break;
		case TK_SLEEP:
			ip += PCodeGenSleep(&srcbuf, pcodebuf, ip, &err);
			break;
		case TK_WAKEUP:
			ip += PCodeGenWakeUp(&srcbuf, pcodebuf, ip, &err);
			break;
		case TK_EOS:
			break;
		default:
			err = SCRIPT_ERR_SYNTAX_INVALID_CMD;
			break;
		}
		if (ip+4>pcodemax) /* keep 4 byte for halt instruction */ 
		{
			err = SCRIPT_ERR_CODE_SIZE_EXCEED;
		}
		if (err)
		{
			break;
		}
	}
	parserTokenNullPatch();
	if (err==SCRIPT_ERR_OK) 
	{
		ip += PCodeGenHalt(pcodebuf, ip);
		if (1) 
		{
			UINT16 i;
			for (i=0 ; i<ip ; i+=4) 
			{
				DBG_PRINT("scrp: %04x:%x %x %x %x\n", i, pcodebuf[i], pcodebuf[i+1], pcodebuf[i+2], pcodebuf[i+3]);
				if (pcodebuf[i]==PCODE_HALT)
				{
					break;
				}
			}
		}
	}
	if (err>=SCRIPT_ERR_SYNTAX) 
	{
		err |= (token-src0);
		scriptErrorReport(err, src0);
	}
	return err;
}

/* ========================================================== */
/* script execution context */
struct 
{
	UINT8 *code;
	UINT16 ip;
	UINT32 sleepCount;
	UINT16 reg[256];
} script;

typedef enum 
{
	SCRIPT_RUN_CORE, /* multiple core instruction can be run in same cycle */
	SCRIPT_RUN_WAIT, /* should run next instruction in next cycle */
	SCRIPT_RUN_HALT, /* halt */
} scriptStepRun_e;
/*--------------------------------------------------------------------------*
* Function name : scriptStepRun 
* Function		: script Step Run
* Return value	: static scriptStepRun_e
* Parameter1	: void
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static scriptStepRun_e
scriptStepRun(
	void
)
{
	scriptStepRun_e r=SCRIPT_RUN_CORE;
	DBG_PRINT("scrp: %x:%x sleep=%d\n", script.ip, script.code[script.ip], script.sleepCount);
	if (script.sleepCount)
	{
		script.sleepCount--;
		return SCRIPT_RUN_WAIT;
	}
	switch (script.code[script.ip]) 
	{
	case PCODE_JZ:
		if (script.reg[script.code[script.ip+1]] == 0)
		{
			script.ip = *(UINT16*)(script.code+script.ip+2);
			return r;
		}
		break;
	case PCODE_JNZ:
		if (script.reg[script.code[script.ip+1]] != 0) 
		{
			script.ip = *(UINT16*)(script.code+script.ip+2);
			return r;
		}
		break;

	case PCODE_MOV:
		script.reg[script.code[script.ip+1]] = *(UINT16*)(script.code+script.ip+2);
		break;
	case PCODE_ADD:
		script.reg[script.code[script.ip+1]] += *(UINT16*)(script.code+script.ip+2);
		break;
	case PCODE_SUB:
		script.reg[script.code[script.ip+1]] -= *(UINT16*)(script.code+script.ip+2);
		break;

	case PCODE_SLEEP:
		script.sleepCount = *(UINT16*)(script.code+script.ip+2) * 100/TIMER_BASE_MS;
		if (script.sleepCount)
		{
			script.sleepCount--;
		}
		r = SCRIPT_RUN_WAIT;
		break;
	case PCODE_KEY:
		sp5kHostMsgSend(APP_KEY_MSG|*(UINT16*)(script.code+script.ip+2), 0);
		r = SCRIPT_RUN_WAIT;
		break;
	case PCODE_WAKEUP:
		sp5kHostMsgSend(APP_UI_MSG_RTC_WAKEUP, *(UINT16*)(script.code+script.ip+2));
		break;
	case PCODE_HALT:
		HOST_PROF_LOG_ADD(LEVEL_INFO, "scrp: script stop");   
		return SCRIPT_RUN_HALT; /* stop */
	case PCODE_NOP: default:
		break;
	}
	script.ip += 4;
	return r;
}

/* ========================================================== */
/* script API */

UINT8 pcode[1024];
/*--------------------------------------------------------------------------*
* Function name : appScriptCompile 
* Function		: script compile
* Return value	: UINT32
* Parameter1	: char *srcbuf
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32 
appScriptCompile(
	char *srcbuf
)
{
	return scriptCompile(srcbuf, pcode, sizeof(pcode));
}
/*--------------------------------------------------------------------------*
* Function name : appScriptLoad 
* Function		: script load
* Return value	: UINT32
* Parameter1	: char *fname
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32
appScriptLoad(
	char *fname
)
{
	UINT32 fd,srclen;
	UINT32 err=SCRIPT_ERR_OK;
	char *srcbuf;

	fd = sp5kFsFileOpen((const UINT8 *)fname, SP5K_FS_OPEN_RDONLY);
	if (fd==0) 
	{
		return SCRIPT_ERR_FILE;
	}
	
	srclen = sp5kFsFileSizeGet(fd);
	srcbuf = sp5kMalloc(srclen+1);
	if (srcbuf==NULL)
	{
		sp5kFsFileClose(fd);
		return SCRIPT_ERR_MALLOC;
	}
	sp5kFsFileRead(fd, (UINT8 *)srcbuf, srclen);
	sp5kFsFileClose(fd);
	srcbuf[srclen] = 0;

	err = scriptCompile(srcbuf, pcode, sizeof(pcode));

	sp5kFree(srcbuf);
	return err;
}

static UINT32 scriptTimer=TIMER_NULL;
/*--------------------------------------------------------------------------*
* Function name : scriptTimerIsr 
* Function		: script timer isr
* Return value	: static void
* Parameter1	: void
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static void scriptTimerIsr(void)
{
	int i;
	scriptStepRun_e r;
	for (i=0 ; i<3 ; i++) 
	{ /* allow running 3 continuous core instruction in one cycle */
		r = scriptStepRun();
		if (r==SCRIPT_RUN_HALT) 
		{
			appTimerIsrUnreg(&scriptTimer);
			break;
		}
		else if (r==SCRIPT_RUN_WAIT) 
		{
			break;
		}
	}
}
/*--------------------------------------------------------------------------*
* Function name : appScriptStart 
* Function		: script start
* Return value	: UINT32
* Parameter1	: UINT8 delaySec
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32 
appScriptStart(
	UINT8 delaySec
)
{
	if (scriptTimer!=TIMER_NULL) 
	{
        HOST_PROF_LOG_ADD(LEVEL_ERROR, "scrp: Override prev script");        
		appTimerIsrUnreg(&scriptTimer);
	}
	script.code = pcode;
	script.ip = 0;
	script.sleepCount = delaySec*1000/TIMER_BASE_MS;
	scriptTimer = appTimerIsrReg(scriptTimerIsr, TIMER_BASE_MS/10); /* isr is 10ms-based */
	return scriptTimer==TIMER_NULL ? FAIL : SUCCESS;
}
/*--------------------------------------------------------------------------*
* Function name : appScriptAutoRun 
* Function		: script auto run
* Return value	: UINT32
* Parameter1	: UINT32 drive
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32 appScriptAutoRun(UINT32 drive)
{
	static UINT8 fname[]="C:\\ATSCRIPT.TXT",inited=0;
	UINT32 err;
	if (inited)
	{
        HOST_PROF_LOG_PRINT(LEVEL_ERROR, "scrp: inited=%d",inited);
		return FAIL;
	}
	fname[0] = drive==SP5K_DRIVE_NAND ? 'C' : 'D';
	err = appScriptLoad((char *)fname);

	if (err==0)
	{
		err = appScriptStart(10);
		inited = 1;
	} 
	else
	{
		err = FAIL;
	}
	return err;
}
/*--------------------------------------------------------------------------*
* Function name : appScriptCIPARun 
* Function		: script cipa run
* Return value	: UINT32
* Parameter1	: UINT32 drive
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32 appScriptCIPARun(UINT32 drive)
{
	static UINT8 fname[]="C:\\CPSCRIPT.TXT",inited=0;
	UINT32 err;
	if (inited) 
	{
        HOST_PROF_LOG_PRINT(LEVEL_ERROR, "scrp: inited=%d",inited);
		return FAIL;
	}
	fname[0] = drive==SP5K_DRIVE_NAND ? 'C' : 'D';
	err = appScriptLoad((char *)fname);

	if (err==0) 
	{
		err = appScriptStart(10);
		inited = 1;
	}
	else 
	{
		err = FAIL;
	}
	return err;
}
/*--------------------------------------------------------------------------*
* Function name : appScriptQuickRun 
* Function		: script quick run
* Return value	: UINT32
* Parameter1	: char *code
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32
appScriptQuickRun(
	char *code
)
{
	UINT32 r;
	r = appScriptCompile(code);
	return r!=SCRIPT_ERR_OK ? r : appScriptStart(0);
}

#if 0
/*--------------------------------------------------------------------------*
* Function name : appScriptKeyLogSave 
* Function		: script key log save
* Return value	: UINT32
* Parameter1	: UINT32 *keyLogBuf
* Parameter2	: UINT32 keyLogCount
* Parameter3	: char *fname
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32
appScriptKeyLogSave(
	UINT32 *keyLogBuf,
	UINT32 keyLogCount,
	char *fname
)
{
	UINT32 fd,i;
	UINT16 key,sleep;
	UINT8 keystate0;
	char buf[32],*keyname;
	fd = sp5kFsFileOpen(fname, FS_OPEN_CREATE);
	if (fd==0)
		return SCRIPT_ERR_FILE;
	keystate0 = KEY_PRESS;
	for (i=0 ; i<keyLogCount ; i++) {
		sleep = (UINT16)(keyLogBuf[i]>>16);
		key = (UINT16)(keyLogBuf[i] & 0xffff);
		keyname = appKeyNameGet(key);
		DBG_PRINT("%d:%d %x %s\r\n", i, sleep, key, keyname);
		if (sleep) {
			sprintf(buf, "sleep %d ", sleep);
			sp5kFsFileWrite(fd, buf, strlen(buf));
		}
		if (keyname) {
			/* insert CRLF when: prev key=release/none & cur key=press/none */
			if (keystate0!=KEY_PRESS && KEY_STATE(key)!=KEY_RELEASE)
				sp5kFsFileWrite(fd, "\xd\xa", 2);
			keystate0 = KEY_STATE(key);
			sprintf(buf, "%s %s ", keystate0==KEY_RELEASE?"keyup":"keydown", keyname);
			sp5kFsFileWrite(fd, buf, strlen(buf));
		}
	}
	sp5kFsFileClose(fd);
	return SCRIPT_ERR_OK;
}

#endif
/*--------------------------------------------------------------------------*
* Function name : cmdScript 
* Function		: cmd Script
* Return value	: void
* Parameter1	: UINT32 argc
* Parameter2	: UINT8 *arg[]
* Parameter3	: UINT32 v[]
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void 
cmdScript(
	UINT32 argc,
	UINT8 *arg[], 
	UINT32 v[]
)
{
	if (argc==1) 
	{
		printf("script usage:\n"
			" script lib n : run built-in script, n=0-2\n"
			" script filename : run script file\n"
			" script stop : stop current script\n"
			" ;cmd_list : quick run script commands\n"
			" script rec : record quick run script\n"
				" script rec stop: stop record & save recorded script to C:\\KEYLOG.TXT\n" );
	} 
	else if (strcmp((void*)arg[1], "stop")==0)
	{
		appScriptQuickRun("");
	}
	else if (strcmp((void*)arg[1], "lib")==0) 
	{
		appScriptLibRun(v[2]);
	} 
	else if (strcmp((void*)arg[1], "rec")==0) 
	{
		#if 0
		if (strcmp(arg[2],"stop")==0)
			appKeyLogClose("C:\\KEYLOG.TXT");
		else
			appKeyLogOpen(2048);
		#endif
	} 
	else if (appScriptLoad((char *)arg[1])==0)
	{
		appScriptStart(v[2]);
	}
}

