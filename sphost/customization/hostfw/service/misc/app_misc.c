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
 *	File name    : App_misc.c
 *	Function     : The implementation which is miscellaneous feature.
 *	First editor :   
 *	First date   :   
 *	History      :
 *               : 2018.06.06  Edward.Lu  "Add description and modify code"
 **************************************************************************/
#define HOST_DBG 0
#include "app_com_def.h"

#include "middleware/pwrlog.h"
#include "sp5k_global_api.h"
#include "sp5k_rsvblk_api.h"
#include "sp5k_aud_api.h"
#include "sp5k_gfx_api.h"
#include "sp5k_fs_api.h"
#include "sp5k_os_api.h"
#include "sp5k_utility_api.h"

#include "app_osd.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef UINT32 (*bkfunc_t)(UINT32);

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/*--------------------------------------------------------------------------*
* Function name    : appTimeStrMake
* Function         : Simple use to get time string(hour,minute,second) by total second,
                     you can printf it.
* Return value	   : void
* Parameter1	   : buf (char* time string buffer)
* Parameter2	   : sec (UINT32 total second)
* History          :
*                  : 2018.06.06  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appTimeStrMake(
	char *buf,
	UINT32 sec
)
{
	if (sec>=3600)
	{
		sprintf(buf, "%02u:%02u:%02u", sec/3600, sec%3600/60, sec%60);
	}
	else
	{
		sprintf(buf, "%02u:%02u", sec/60, sec%60);
	}
}

/*--------------------------------------------------------------------------*
* Function name    : appPower2Get
* Function         : brief rational base 2 power, 
                     where *pResultNumerator / *pResultDenominator := power(2, numerator/denominator)
* Return value	   : return 0: success;
                     PWR2_DOMAIN_ERR: denom==0 not supported;
                     PWR2_OVERFLOW: result range *pwr2X, *pwr2Y not fits in UINT32;
* Parameter1	   : numerator (SINT32)
* Parameter2	   : denominator (UINT32)
* Parameter3	   : pResultNumerator (UINT32)
* Parameter4	   : pResultDenominator (UINT32)
* History          :
*                  : 2018.06.06  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

#define POWER2_ARG_MIN	-16
#define POWER2_ARG_MAX	16

SINT32
appPower2Get(
	SINT32 numerator,
	UINT32 denominator,
	UINT32 *pResultNumerator,
	UINT32 *pResultDenominator
)
{
	SINT32 val;
	if (numerator <= POWER2_ARG_MIN * (SINT32)denominator) 
	{
		val = -1;
		*pResultNumerator = 1;
		*pResultDenominator = (1UL << (UINT32)-POWER2_ARG_MIN) - 1;
	} 
	else if (numerator >= POWER2_ARG_MAX * (SINT32)denominator) 
	{
		val = 1;
		*pResultNumerator = (1UL << POWER2_ARG_MAX) - 1;
		*pResultDenominator = 1;
	}
	else 
	{
		val = power2xyND(numerator, denominator, pResultNumerator, pResultDenominator);
	}
	return val;
}

/*--------------------------------------------------------------------------*
* Function name    : WriteFile
* Function         : Simple use to write file. You can use fsSimpleWrite, too.
* Return value	   : result (UINT32 SUCCESS/FAIL)
* Parameter1	   : fname (UINT8*)
* Parameter2	   : buf (UINT8)
* Parameter3	   : size (UINT32)
* History          :
*                  : 2018.06.06  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32 WriteFile(UINT8 *fname, UINT8 *buf, UINT32 size)
{
	UINT32 fd;
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "misc: %s %s from %x l=%d", __FUNCTION__, fname, (UINT32)buf, size);

	fd = sp5kFsFileOpen(fname, SP5K_FS_OPEN_CREATE);
	if (fd) 
	{
		sp5kFsFileWrite(fd, buf, size);
		sp5kFsFileClose(fd);
		return SUCCESS;
	}
	return FAIL;
}

/*--------------------------------------------------------------------------*
* Function name    : ReadFile
* Function         : Simple use to read file. You can use fsSimpleRead, too.
* Return value	   : result (UINT32 SUCCESS/FAIL)
* Parameter1	   : fname (UINT8*)
* Parameter2	   : buf (UINT8*)
* Parameter3	   : size (UINT32)
* History          :
*                  : 2018.06.06  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32 ReadFile(UINT8 *fname, UINT8 *buf, UINT32 size)
{
	UINT32 fd;
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "misc: %s %s from %x l=%d", __FUNCTION__, fname, (UINT32)buf, size);
    
	fd = sp5kFsFileOpen(fname, SP5K_FS_OPEN_RDONLY);
	if (fd) 
	{
		sp5kFsFileRead(fd, buf, size);
		sp5kFsFileClose(fd);
		return SUCCESS;
	}
	return FAIL;
}

/*--------------------------------------------------------------------------*
* Function name    : ReadFileEx
* Function         : Simple use to read all data from file. You can use fsSimpleReadEx, too.
* Return value	   : result (UINT32 SUCCESS/FAIL)
* Parameter1	   : fname (UINT8*)
* Parameter2	   : pbuf (UINT8**)
* Parameter3	   : psize (UINT32*)
* History          :
*                  : 2018.06.06  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32 ReadFileEx(UINT8 *fname, UINT8**pbuf, UINT32 *psize )
{
	UINT32 fd,ret=FAIL;
	UINT8 *buf;
	fd = sp5kFsFileOpen(fname, SP5K_FS_OPEN_RDONLY);
	if (fd) 
	{
		UINT32 size;
		size = sp5kFsFileSizeGet(fd);
		buf = sp5kMalloc(size);
		if (buf)
		{
            HOST_PROF_LOG_PRINT(LEVEL_INFO, "misc: %s %s from %x l=%d", __FUNCTION__, fname, (UINT32)buf, size);
            
			sp5kFsFileRead(fd, buf, size);
			*pbuf = buf;
			*psize = size;
			ret = SUCCESS;
		}
		sp5kFsFileClose(fd);
	}
	return ret;
}

/*--------------------------------------------------------------------------*
* Function name    : appFileExistCheck
* Function         : Simple use to check is file exist or not. You can use fsIsFileExists, too.
* Return value	   : result (BOOL TRUE:file exist/FALSE:file not exist)
* Parameter1	   : filename (UINT8*)
* History          :
*                  : 2018.06.06  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

BOOL
appFileExistCheck(
	UINT8 *filename
)
{
	UINT32 fd;	
	
	fd = sp5kFsFileOpen(filename, FS_OPEN_RDONLY);
	if (fd != 0)
	{
		UINT32 readLen;
		readLen = sp5kFsFileSizeGet(fd);	
		sp5kFsFileClose(fd);
		if (readLen) 
		{
			return TRUE;
		}
	}		
	return FALSE;
}
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)

/*--------------------------------------------------------------------------*
* Function name    : jet_file_size_get
* Function         : Simple use to get file size
* Return value	   : readLen (UINT32 fail:readLen=0)
* Parameter1	   : filename (UINT8*)
* History          :
*                  : 2018.06.06  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32 jet_file_size_get(UINT8 *filename)
{
	UINT32 fd, readLen = 0;	
	fd = sp5kFsFileOpen(filename, FS_OPEN_RDONLY);
	if (fd != 0){
		readLen = sp5kFsFileSizeGet(fd);	
		sp5kFsFileClose(fd);
	}		
	return readLen;
}
#endif

/*--------------------------------------------------------------------------*
* Function name    : appBackgroundTask
* Function         : Background Task implementation
* Return value	   : void
* Parameter1	   : param
                     param[0]:pfunc (bkfunc_t)
                     param[1]:function input parameter
                     param[2]:event flag pointer
                     param[3]:input:blocking, output:function return
* History          :
*                  : 2018.06.06  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

static void
appBackgroundTask(
	UINT32 param
)
{
	UINT32 *par = (UINT32 *)param;
	bkfunc_t pfunc = (bkfunc_t)par[0];
	UINT32 blocking = par[3];

	profLogPrintf(0, "#S#HostBk%d#N#%p(%x)", blocking, pfunc, par[1]);
	par[3] = pfunc(par[1]);
	profLogPrintf(0, "#E#HostBk%d#N#", blocking);

	if (blocking) {
		sp5kOsEventFlagsSet((SP5K_EVENT_FLAGS_GROUP *)par[2], 1, TX_OR);
		while (1)	sp5kOsThreadSleep(1000);
	}
	else {
		sp5kFree((void *)param);
		sp5kOsThreadDelete(NULL);
	}
}

/*--------------------------------------------------------------------------*
* Function name    : appBackgroundFuncExec
* Function         : Create another thread to do background task with blocking.
                     You use this feature to do background feature.
* Return value	   : result (UINT32 SUCCESS/function return)
* Parameter1	   : pfunc (void* function pointer)
* Parameter2	   : param (UINT32 function input parameter)
* Parameter3	   : priority (UINT32 background thread priority)
* Parameter4	   : blocking (UINT32)
					 blocking=0:Original thread process don't wait appBackgroundTask done.
                     blocking=1:Original thread process wait appBackgroundTask done.
* History          :
*                  : 2018.06.06  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32
appBackgroundFuncExec(
	void *pfunc,
	UINT32 param,
	UINT32 priority,
	UINT32 blocking
)
{
	UINT32 flags;
	UINT32 ret = SUCCESS;

	if (blocking == 0) {
		UINT32 *par = sp5kMallocCache(4*sizeof(UINT32));
		par[0] = (UINT32)pfunc;
		par[1] = param;
		par[3] = 0;
		sp5kOsThreadCreate("appBkTasks", appBackgroundTask, (ULONG)par, priority, 0, 0, 1);
	}
	else {
		SP5K_EVENT_FLAGS_GROUP bkDone;

		sp5kOsEventFlagsCreate(&bkDone, "appBkDone");

		UINT32 par[4] = {(UINT32)pfunc, param, (UINT32)&bkDone, 1};
		SP5K_THREAD *tid = sp5kOsThreadCreate("appBkTask", appBackgroundTask, (ULONG)par, priority, 0, 0, 1);
		sp5kOsEventFlagsGet(&bkDone, 1, TX_OR_CLEAR, &flags, TX_WAIT_FOREVER);
		sp5kOsEventFlagsDelete(&bkDone);
		sp5kOsThreadDelete(tid);
		ret = par[3];
	}

	return ret;
}

#if SUPPORT_RUNTIME_SWITCH_DRAM_CLK

/*--------------------------------------------------------------------------*
* Function name    : appSysClkSwitch
* Function         : Switch system clock 300MHz or 450MHz.
* Return value	   : void
* Parameter1	   : select (UINT8)
                     0:RUNTIME_SWITCH_DRAM_CLK_DEF_KHZ, 300MHz
                     1:RUNTIME_SWITCH_DRAM_CLK_MAX_KHZ, 450MHz
* History          :
*                  : 2018.06.06  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

static UINT32 curClkSetting = -1;	/* 0: normal clk, 1: max clk */
void appSysClkSwitch(UINT8 sel)
{
	UINT32 mode;

    printf("%u,%d\n", curClkSetting ,sel);
    callStack();

	if (curClkSetting != sel) {
		curClkSetting = sel;
		profLogPrintf(0, "#S#APPCLKSW#N#%d", sel);
		sp5kModeGet(&mode);
		if(mode!=SP5K_MODE_STANDBY) {
		    sp5kModeCfgSet(SP5K_MODE_CFG_STANDBY_DUP,1);
			sp5kModeSet(SP5K_MODE_STANDBY);
			sp5kModeWait(SP5K_MODE_STANDBY);
		}
		sp5kOsThreadSleep(10);
		sp5kSystemPllSet(sel?RUNTIME_SWITCH_DRAM_CLK_MAX_KHZ:RUNTIME_SWITCH_DRAM_CLK_DEF_KHZ);
		sp5kOsThreadSleep(10);
		#if TEST_STANDBY
		if(mode!=SP5K_MODE_STANDBY) {
			sp5kModeSet(mode);
			sp5kModeWait(mode);
		}
		#endif
		profLogPrintf(0, "#E#APPCLKSW#N#%d", mode);
	}
	else {
		printf("sys clk are %d already\n", sel);
	}
}
#endif

/*--------------------------------------------------------------------------*
* Function name    : appModeCfgPvRawSet
* Function         : When set this flag.
                     CDSP will output raw first and then do yuv from raw. 
                     Use at Multi-View or 4K2K flow that yuv can not be from CDSP directly. 
* Return value	   : void
* Parameter1	   : doMode (UINT32 sp5kModePvDoRawOpt_e)
                     SP5K_MODE_PV_RAW_DO_DISABLE : disable preview do raw flow
                     SP5K_MODE_PV_RAW_DO_UHD_MODE : enable preview Do raw flow to support 4k/2k rec.
                     SP5K_MODE_PV_RAW_DO_CUST_CB_MODE : enable preview do raw flow to support 360cam or Eis 
* History          :
*                  : 2018.06.06  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

static UINT32 doPvMode = SP5K_MODE_PV_RAW_DO_DISABLE;
void
appModeCfgPvRawSet(
	UINT32 doMode
)
{
	doPvMode = doMode;
	sp5kModeCfgSet( SP5K_MODE_CFG_PV_RAW_DO_OPT, doMode );
}

/*--------------------------------------------------------------------------*
* Function name    : appModeCfgPvRawGet
* Function         : Get current sp5kModePvDoRawOpt_e mode
* Return value	   : doPvMode (sp5kModePvDoRawOpt_e)
* Parameter1	   : void
* History          :
*                  : 2018.06.06  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32
appModeCfgPvRawGet(
	void
)
{
	return doPvMode;
}

