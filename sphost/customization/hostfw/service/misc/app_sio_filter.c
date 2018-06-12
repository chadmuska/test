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
#define HOST_DBG 0
#include "common.h"
/*#include <stdio.h> */ /* stdio define putchar as putc and cause problemm in sio filter */
#include <string.h>
#include "sp5k_dbg_api.h"
#include "app_dbg_api.h"
#include "app_util.h"
#include "app_sio_filter.h"


/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define COUNT_OF(array)	(sizeof(array)/sizeof(array[0]))

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
/* putc with filter */
static char *defaultInclude[]={ "[ERR", "cmd>" };
#define DEFAULT_FILTER_MAX		COUNT_OF(defaultInclude)


#define CUSTOM_FILTER_MAX		4

#define FILTER_BUF_MAX			128
static char incfilterBuf[FILTER_BUF_MAX+1]=""; /* extra byte for ending zero */
static char excfilterBuf[FILTER_BUF_MAX+1]=""; /* extra byte for ending zero */

#define FILTER_MAX		(CUSTOM_FILTER_MAX+CUSTOM_FILTER_MAX+DEFAULT_FILTER_MAX)

#define CUSTOM_INC_BASE		DEFAULT_FILTER_MAX
#define CUSTOM_EXC_BASE		(CUSTOM_INC_BASE+CUSTOM_FILTER_MAX)

static BOOL fCheck[FILTER_MAX];
static char *filter[FILTER_MAX];

static BOOL fSioInited=FALSE;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
static void putc_filter(UINT8 ch);
static int putc_null(int ch);

extern int _putchar(int);
int printf(const char *format, ...);

/*void sioPutcRedirect(void * pf);*/
void * sioPutsRedirect(void *pf);
int bk_puts(const char *str);
int bk_putc(const char c);

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/* eat serial output */
static int 
putc_null(
	int ch
)
{
	return 1;/* do nothing */
} 

/* helper for filter */
#if 0
static void dumphex(UINT8 ch)
{
	UINT8 c0=ch/16;
	ch %= 16;
	_putchar(c0>=10 ? c0+'a'-10 : c0+'0');
	_putchar(ch>=10 ? ch+'a'-10 : ch+'0');
	_putchar(' ');
}
#endif

static void
_sioInit(
	void
)
{
	UINT8 i;
	for (i=0 ; i<DEFAULT_FILTER_MAX ; i++)
		filter[i] = defaultInclude[i];
	for ( ; i< CUSTOM_EXC_BASE ; i++)
		filter[i] = incfilterBuf;
	for ( ; i< FILTER_MAX ; i++)
		filter[i] = excfilterBuf;
}

void
siofilterPatternDump(
	void
)
{
	int i;
	if (!fSioInited) { fSioInited = 1; _sioInit(); }
	printf("include filter:");
	for (i=CUSTOM_INC_BASE ; i<CUSTOM_EXC_BASE ; i++)
		if (*filter[i]) printf(" %s", filter[i]);
	printf("\nexclude filter:");
	for ( ; i<FILTER_MAX ; i++)
		if (*filter[i]) printf(" %s", filter[i]);
	printf("\n");
}

/* use ; as separator */
#define IS_DELIMITER(ch)	((ch)==0 || (ch)==';')

UINT32
siofilterPatternSet(
	BOOL fExclude,
	char *pattern
)
{
	UINT8 i,idx,max;
	UINT32 r=SUCCESS;
	char *buf;
	if (!fSioInited) { fSioInited = 1; _sioInit(); }
	if (fExclude) {
		idx = CUSTOM_EXC_BASE; max = FILTER_MAX;
		buf = excfilterBuf;
	} else {
		idx = CUSTOM_INC_BASE; max = CUSTOM_EXC_BASE;
		buf = incfilterBuf;
	}
	for (i=0 ; idx<max && *pattern; idx++) {
		while (*pattern==';')
			pattern++;
		filter[idx] = buf+i;
		for ( ; i<FILTER_BUF_MAX && !IS_DELIMITER(*pattern) ; i++)
			buf[i] = *pattern++;
		buf[i++] = 0;
		if (i>=FILTER_BUF_MAX && !IS_DELIMITER(*pattern)) /* buf overflow */
			break;
	}
	buf[i] = 0;
	for ( ; idx<max ; idx++)
		filter[idx] = buf+i;
	if (*pattern)
		r = FAIL;
	return r;
}

void 
sioCtrl(
	UINT32 opt
)
{
	static UINT32 prev=0,cur=0;
	
	if (opt==0xffffffff) 
	{
		opt = cur = prev;
	}
	else
	{
		prev = cur; cur = opt;
	}
	profLogPrintf(0,"sio: %d",opt);
	switch (opt) 
	{
		case 3:
			sioPutsRedirect(bk_puts);
			sioPutcRedirect(bk_putc);
			break;
		case 1:
			sioPutsRedirect(NULL);
			sioPutcRedirect(putc_null);
			break;
		case 2:
			sioPutsRedirect(NULL);
			sioPutcRedirect(putc_filter);
			break;
		case 0:
		default:
			sioPutsRedirect(NULL);
			sioPutcRedirect(NULL);
			break;
	}
}

static void 
_sioBufOutput(
	char *buf,
	UINT8 len
)
{
	while (len--) {
		_putchar(*buf++);
	}
}

static BOOL
_sioCheckExcludeGet(
	void
)
{
	UINT8 i;
	for (i=CUSTOM_EXC_BASE ; i<FILTER_MAX ; i++) {
		if (fCheck[i])
			return 1;
	}
	return 0;
}

static UINT8
_sioMatch(
	BOOL fExclude,
	UINT8 chIdx,
	char ch
)
{
	UINT8 idx,max;
	idx = fExclude ? CUSTOM_EXC_BASE : 0;
	max = fExclude ? FILTER_MAX : CUSTOM_EXC_BASE;
	for ( ; idx<max ; idx++) {
		if (fCheck[idx]) {
			if (filter[idx][chIdx]=='*' && chIdx==0 && filter[idx][1]==0) 
				/* match wild card: only 1st & signle * is treated as wild card */
				return TRUE;
			if (filter[idx][chIdx]!=ch) /* this filter fails */
				fCheck[idx] = 0;
			else if (filter[idx][chIdx+1]==0) /* match last char of pattern */
				return TRUE;
		}
	}
	return FALSE;
}

enum { SFS_CHECK, SFS_CHECK_EXCLUDE, SFS_OUTPUT, SFS_IGNORE };

static UINT8
_siofilterCheck(
	UINT8 chIdx,
	char ch
)
{

	if (!fSioInited) { fSioInited = 1; _sioInit(); }
	if (chIdx==0) {
		UINT8 i;
		for (i=0 ; i<FILTER_MAX ; i++) {
			fCheck[i] = filter[i][0] ? 1 : 0;
		}
	}
	if (_sioMatch(1, chIdx, ch)) /* check exclude filter */
		return SFS_IGNORE;
	if (_sioMatch(0, chIdx, ch)) /* check include filter */
		return _sioCheckExcludeGet() ? SFS_CHECK_EXCLUDE : SFS_OUTPUT;
	return SFS_CHECK;
}

static UINT8
_siofilterCheckExclude(
	UINT8 chIdx,
	char ch
)
{
	if (_sioMatch(1, chIdx, ch)) /* check exclude filter */
		return SFS_IGNORE;
	return _sioCheckExcludeGet() ? SFS_CHECK_EXCLUDE : SFS_OUTPUT;
}


static UINT8 siofilterState=SFS_CHECK;
static char sioBuf[FILTER_BUF_MAX];
static UINT8 chIdx=0;

static void 
putc_filter(
	UINT8 ch
)
{
	if (ch==0xa) {
		if (siofilterState==SFS_OUTPUT) {
			_putchar(0xd); _putchar(0xa);
		}
		siofilterState = SFS_CHECK;
		chIdx = 0;
	} else if (siofilterState==SFS_OUTPUT) {
		_putchar(ch);
	} else if (siofilterState==SFS_CHECK) {
		sioBuf[chIdx] = ch;
		siofilterState = _siofilterCheck(chIdx, ch);
		chIdx++;
		if (siofilterState==SFS_OUTPUT) {
			_sioBufOutput(sioBuf, chIdx);
		} else if (chIdx>=COUNT_OF(sioBuf)) {
			siofilterState = SFS_IGNORE;	
		}
	} else if (siofilterState==SFS_CHECK_EXCLUDE) {
		sioBuf[chIdx] = ch;
		siofilterState = _siofilterCheckExclude(chIdx, ch);
		chIdx++;
		if (siofilterState==SFS_OUTPUT) {
			_sioBufOutput(sioBuf, chIdx);
		} else if (chIdx>=COUNT_OF(sioBuf)) {
			siofilterState = SFS_IGNORE;	
		}
	}
}

void 
cmdSioFilter(
	char *arg[],
	UINT32 v[]
)
{
	if (*arg[1]=='+' || *arg[1]=='-') {
		siofilterPatternSet(*arg[1]=='-', (char*)arg[2]);
		sioCtrl(2); /* auto turn on sio filter */
	} else if (strcmp(arg[1], (char *)"dump")==0) {
		siofilterPatternDump();
	} else if (arg[1][0]!=0) {
		sioCtrl(v[1]);
	} else {
		printf("sio usage 1: sio 0|1|2\n"
			" sio 0|1|2 : sio filter switch, 0:enable, 1:disable, 2:filter\n"
			" sio + filters : set include patterns (; as delimiter)\n"
			" sio - filters : set exclude patterns\n"
			" sio dump : dump all patterns\n" );
	}
}

