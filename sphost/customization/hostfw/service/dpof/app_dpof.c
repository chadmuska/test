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
 *	File name	: app_dpof.c
 *	Function	: dpof function
 *	First editor: Weikai Hsu
 *	First date	: 
 *	History		: 	
 *	            : 2018.06.01  Weikai Hsu "Added File Header"
 *	
 *	
 *	
 *--------------------------------------------------------------------------*/
#define HOST_DBG 0
#include "app_com_def.h"
#include "app_com_api.h"
#include "app_dpof.h"
#include "sp5k_dpof_api.h"
#include "sp5k_global_api.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

#if SP5K_CDFS_OPT
#define APP_DPOF_EN 0
#else
#define APP_DPOF_EN 1
#endif
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
/*typedef struct appDpofCtrl_s {
	BOOL isInit;
	BOOL isUpdated;
} appDpofCtrl_t;*/  /*cppcheck Removed:struct is never used*/
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
#if APP_DPOF_EN
static appDpofCtrl_t dpofCtrl = {FALSE, FALSE};
#endif

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

#if APP_DPOF_EN
/*--------------------------------------------------------------------------*
* Function name : _dpofJobQuery 
* Function		: dpof job query
* Return value	: void 
* Parameter1	: appDpofMode_e mode
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
BOOL
_dpofJobQuery(
	appDpofMode_e mode
)
{	
	if(sp5kDpofJobQuery(mode) == SUCCESS) {
		return TRUE;
	}
	
	return FALSE;
}

/*--------------------------------------------------------------------------*
* Function name : _dpofHeaderUpdate 
* Function		: dpof header update
* Return value	: void 
* Parameter1	: void
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void
_dpofHeaderUpdate(
	void
)
{
	UINT8 dpofTm[32];
	tmx_t time;
	sp5kRtcDateTimeGet(SP5K_DATE_TIME_OPTION, &time);
	sprintf((char *)dpofTm,"%04d:%02d:%02d:%02d:%02d:%02d",
						time.tmx_year+1900,
						time.tmx_mon,
						time.tmx_mday,
						time.tmx_hour,
						time.tmx_min,
						time.tmx_sec
	);
	sp5kDpofFileAttrSet(SP5K_DPOF_PARAMID_GEN_DTM,dpofTm);
}

/*--------------------------------------------------------------------------*
* Function name : appDpofInit 
* Function		: dpof init
* Return value	: void 
* Parameter1	: void
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void
appDpofInit(
	void
)
{
	DBG_PRINT("dpof: %s enter\n",__FUNCTION__);
	
	if(dpofCtrl.isInit) {
        HOST_PROF_LOG_PRINT(LEVEL_DEBUG,"dpof: %s(%d)",__FUNCTION__,__LINE__);
		return;
	}	
	
	sp5kDpofInit((UINT8 *)APP_DPOF_CRT_NAME);
	sp5kDpofFileAttrSet(SP5K_DPOF_PARAMID_GEN_CRT,(UINT8 *)APP_DPOF_CRT_NAME);
	dpofCtrl.isInit = TRUE;
}

/*--------------------------------------------------------------------------*
* Function name : appDpofClose 
* Function		: dpof file remove
* Return value	: void 
* Parameter1	: BOOL write2File
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void
appDpofClose(
	BOOL write2File
)
{
	DBG_PRINT("dpof: %s(%d) enter\n\n",__FUNCTION__,write2File);

	if(!dpofCtrl.isInit) 
	{
		HOST_PROF_LOG_PRINT(LEVEL_DEBUG,"dpof: %s(%d)",__FUNCTION__,__LINE__);
		return;
	}		
	if(write2File) 
	{
		sp5kDpofFileWrite();			
		dpofCtrl.isUpdated = FALSE;	
	}
	sp5kDpofClose();
	dpofCtrl.isInit = FALSE;
}

/*--------------------------------------------------------------------------*
* Function name : appDpofFileRemove 
* Function		: dpof file remove
* Return value	: void 
* Parameter1	: UINT32 isCardUsed
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void
appDpofFileRemove(
	UINT32 isCardUsed
)
{
	DBG_PRINT("dpof: %s enter",__FUNCTION__);

	if( isCardUsed )	{
		sp5kFsFileDelete((const UINT8 *)DPOF_FILENAME_SDCARD);
	}
	else {
		sp5kFsFileDelete((const UINT8 *)DPOF_FILENAME_NAND);
	}
}

/*--------------------------------------------------------------------------*
* Function name : appDpofJobExistQuery 
* Function		: dpof job exist query
* Return value	: bool 
* Parameter1	: appDpofMode_e dpofMode
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
BOOL
appDpofJobExistQuery(
	appDpofMode_e dpofMode
)
{		
	//DBG_PRINT("dpof: %s enter",__FUNCTION__);

	return (_dpofJobQuery(dpofMode));
}

/*--------------------------------------------------------------------------*
* Function name : appDpofPrintQuantityGet 
* Function		: get dpof print quantity
* Return value	: UINT32 
* Parameter1	: appDpofMode_e dpofMode
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32
appDpofPrintQuantityGet(
	appDpofMode_e dpofMode
)
{
	UINT32 printQty = 0;

	//DBG_PRINT("dpof: %s enter",__FUNCTION__);
	
	if(_dpofJobQuery(dpofMode)) {
		sp5kDpofJobAttrGet(dpofMode,SP5K_DPOF_PARAMID_PRT_QTY,(void *)&printQty);		
	}
	return printQty;
}

/*--------------------------------------------------------------------------*
* Function name : appDpofDateStampExistQuery 
* Function		: dpof date stamp exist query
* Return value	: BOOL 
* Parameter1	: appDpofMode_e dpofMode
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
BOOL
appDpofDateStampExistQuery(
	appDpofMode_e dpofMode
)
{
	//DBG_PRINT("dpof: %s enter",__FUNCTION__);

	if (_dpofJobQuery(dpofMode)){
		UINT8 dateStamp[32];
		sp5kDpofJobAttrGet(dpofMode,SP5K_DPOF_PARAMID_CFG_DSC_AD,(void *)dateStamp);		
		DBG_PRINT("dateStamp=[%s]@0x%08x \n", dateStamp, dateStamp);
		return (dateStamp[0]=='\0' ? FALSE : TRUE);
	}
	return FALSE;
}

/*--------------------------------------------------------------------------*
* Function name : appDpofDateStampGet 
* Function		: get dpof date stamp 
* Return value	: UINT32 
* Parameter1	: appDpofMode_e dpofMode
* Parameter2	: UINT8 *pszDTBuf
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32
appDpofDateStampGet(
	appDpofMode_e dpofMode,
	UINT8 *pszDTBuf
)
{
	
	//DBG_PRINT("dpof: %s enter",__FUNCTION__);

	if (_dpofJobQuery(dpofMode)){
		int i;
		UINT8 dateStamp[32];
		sp5kDpofJobAttrGet(dpofMode,SP5K_DPOF_PARAMID_CFG_DSC_AD,(void *)dateStamp);
		DBG_PRINT("dpof: dateStamp=[%s] \n", dateStamp);
        i = strlen((const char *)dateStamp);
		if (i>=0) {			
			if (i<24) {
				memcpy(pszDTBuf, dateStamp, i);
				*(pszDTBuf+i)='\0';
				return SUCCESS;
			}
		}
	}
	*pszDTBuf='\0';
	return FAIL;
}

/*--------------------------------------------------------------------------*
* Function name : appDpofPrintSizeGet 
* Function		: get dpof print size
* Return value	: UINT32 
* Parameter1	: void
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32
appDpofPrintSizeGet(
	void
)
{
	UINT32 size = 0;

	//DBG_PRINT("dpof: %s enter",__FUNCTION__);

	if (_dpofJobQuery(APP_DPOF_MODE_SIZ))
	{
		sp5kDpofJobAttrGet(APP_DPOF_MODE_SIZ,SP5K_DPOF_PARAMID_PRT_TYP,(void *)&size);				
	}
	return size;
}

/*--------------------------------------------------------------------------*
* Function name : appDpofJobDelete 
* Function		: dpof job delete
* Return value	: UINT32 
* Parameter1	: appDpofMode_e dpofMode
* Parameter2	: appDpofJobOption_e jobOption
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32 
appDpofJobDelete(
	appDpofMode_e dpofMode,
	appDpofJobOption_e jobOption
)
{
	dpofCtrl.isUpdated = TRUE;

	DBG_PRINT("dpof: %s enter",__FUNCTION__);

	return(sp5kDpofJobDelete(dpofMode,jobOption));
}

/*--------------------------------------------------------------------------*
* Function name : appDpofStdJobAdd 
* Function		: dpof std job add
* Return value	: UINT32 
* Parameter1	: UINT32 printQty
* Parameter2	: UINT8 *dateStamp
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32
appDpofStdJobAdd(
	UINT32 printQty,
	UINT8 *dateStamp
)
{
	DBG_PRINT("dpof: %s enter",__FUNCTION__);

	_dpofHeaderUpdate();
	if (_dpofJobQuery(APP_DPOF_MODE_STD)== 0) 
	{
		if(sp5kDpofJobAdd(SP5K_DPOF_PRINT_MODE_STD) != SUCCESS)
		{
			return FAIL;
		}
	}
	sp5kDpofJobAttrSet(SP5K_DPOF_PRINT_MODE_STD,SP5K_DPOF_PARAMID_CFG_DSC_AD,(void*)dateStamp);
	sp5kDpofJobAttrSet(SP5K_DPOF_PRINT_MODE_STD, SP5K_DPOF_PARAMID_PRT_QTY, (void*)&printQty);
	dpofCtrl.isUpdated = TRUE;
	return SUCCESS;
}

/*--------------------------------------------------------------------------*
* Function name : appDpofFileWrite 
* Function		: dpof file write
* Return value	: UINT32 
* Parameter1	: void
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32
appDpofFileWrite(
	void
)
{
	return sp5kDpofFileWrite;
}

#else
/*--------------------------------------------------------------------------*
* Function name : appDpofJobDelete 
* Function		: dpof job delete
* Return value	: UINT32 
* Parameter1	: appDpofMode_e dpofMode
* Parameter2	: appDpofJobOption_e jobOption
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32 
appDpofJobDelete(
	appDpofMode_e dpofMode,
	appDpofJobOption_e jobOption
) { return 0; }

UINT32
appDpofFileWrite(
	void
) { return 0; }

#endif
