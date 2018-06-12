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
 *	File name    : App_callback_setting.c
 *	Function     : About plural callback funtion(cbfunc) management by index, 
                   you can use callbackFuncTbl_t to call plural cbfunc.
                   You can expand this feature.
 *	First editor :   
 *	First date   :   
 *	History      :
 *               : 2018.05.31	Edward.Lu  "Add description and modify code"
 **************************************************************************/

#include "common.h"
#include <stdio.h>
#include "app_com_def.h"
#include "app_com_api.h"

#include "sp5k_capture_api.h"
#include "app_callback_setting.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

typedef void ( *callbackFunc )();
typedef struct callbackFuncTbl_s {
	UINT32 size;				//callback array size
	callbackFunc* funcDelegate;		//pointer to callback func array
}callbackFuncTbl_t, *pcallbackFuncTbl_t;

typedef UINT32 (*fpsnapCallback_t)(UINT32, UINT32, void *);

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

static callbackFuncTbl_t yuvCallbackFuncTbl = { 2, NULL }; //size is 2


/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/*--------------------------------------------------------------------------*
* Function name    : yuvCallbackFunc
* Function         : SP5K_CAPTURE_CALLBACK_YUV_WITH_JPEG used, 
                     will call all yuvCallbackFuncTbl function.
* Return value	   : void
* Parameter1	   : yuvAddr (UINT32 destination address)
* Parameter2	   : qvAddr (UINT32 no use)
* Parameter3	   : imgSize (sp5kYuvCbImgSize_t*)
* History          :
*                  : 2018.06.01  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32 yuvCallbackFunc( UINT32 yuvAddr, UINT32 qvAddr, void* imgSize ) {
	UINT32 idx = 0;
	callbackFunc* pfunc = yuvCallbackFuncTbl.funcDelegate;

	//peter++ 2013/08/28, if all function is unregistered, return directly
	if( pfunc == NULL ) {
		return FAIL;
	}

	for( ; idx < yuvCallbackFuncTbl.size; idx++ ) {
		if( *pfunc != NULL ) {
			( *pfunc )( yuvAddr, qvAddr, imgSize );
		}
		pfunc++;
	}

	return SUCCESS;
}

/*--------------------------------------------------------------------------*
* Function name    : isFuncRegistered
* Function         : Judge pfunc is registered in funcTbl.
* Return value	   : result (BOOL TRUE/FALSE)
* Parameter1	   : function table (pcallbackFuncTbl_t plural cbfunc pointer)
* Parameter2	   : pfunc (void* new function)
* History          :
*                  : 2018.06.01  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

BOOL isFuncRegistered( pcallbackFuncTbl_t funcTbl, void* pfunc ) {
	UINT32 idx = 0;
	callbackFunc* func = ( callbackFunc* )funcTbl->funcDelegate;	
	for( ; idx < funcTbl->size; idx++ ) {
		if( *func == pfunc )
			return TRUE;
		else
			func++;
	}
	return FALSE;
}

/*--------------------------------------------------------------------------*
* Function name    : registerFuncwithIdx
* Function         : register new function with index
* Return value	   : result (UINT32 SUCCESS/FAIL)
* Parameter1	   : function table (pcallbackFuncTbl_t plural cbfunc pointer)
* Parameter2	   : pfunc (void* new function)
* Parameter3	   : func Index (UINT32 0~(funcTbl size-1))
* History          :
*                  : 2018.06.01  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32 registerFuncwithIdx( pcallbackFuncTbl_t funcTbl, void* pfunc, UINT32 funcIdx ) {
	if( funcIdx >= funcTbl->size ) {
        HOST_PROF_LOG_PRINT(LEVEL_ERROR,"cbset: selector is wrong %s", __FUNCTION__);
		return FAIL;
	}

	callbackFunc* func = ( callbackFunc* )funcTbl->funcDelegate;
	func[ funcIdx ] = pfunc;

	return SUCCESS;
}

/*--------------------------------------------------------------------------*
* Function name    : registerFunc
* Function         : register cbfunc
* Return value	   : result (UINT32 SUCCESS/FAIL)
* Parameter1	   : cbfunc table (pcallbackFuncTbl_t)
* Parameter2	   : pointer function (void* new cbfunc)
* History          :
*                  : 2018.06.01  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32 registerFunc( pcallbackFuncTbl_t funcTbl, void* pfunc ) {
	UINT32 idx = 0;
	callbackFunc* func = ( callbackFunc* )funcTbl->funcDelegate;
	for( ; idx < funcTbl->size; idx++ ) {
		if( *func == NULL ) {
			*func = pfunc;
			return SUCCESS;
		} else {
			func++;
		}
	}

	return FAIL;	//func array is full
}

/*--------------------------------------------------------------------------*
* Function name    : unregisterFunc
* Function         : unregister cdfunc
* Return value	   : result (UINT32 SUCCESS:unregister OK/(-1):funcTbl->funcDelegate free)
* Parameter1	   : cbfunc table (pcallbackFuncTbl_t)
* Parameter2	   : pointer function (void* cbfunc you want remove)
* History          :
*                  : 2018.06.01  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32 unregisterFunc( pcallbackFuncTbl_t funcTbl, void* pfunc ) {
	UINT32 idx = 0, nullCnt = 0;
	BOOL unregistered_done = FALSE;
	callbackFunc* func = ( callbackFunc* )funcTbl->funcDelegate;
	for( ; idx < funcTbl->size; idx++ ) {
		if( *func == NULL ) {
			nullCnt++;
		} else {
			if ( (!unregistered_done) && (*func == pfunc) ) {
				*func = NULL;
				unregistered_done = TRUE;
				nullCnt++;
			}
		}
		func++;
	}

	if( nullCnt == funcTbl->size ) {
		UINT32 ret = (-1);
		sp5kFree( funcTbl->funcDelegate );
		funcTbl->funcDelegate = NULL;
		return ret;
	}

	return SUCCESS;
}

/*--------------------------------------------------------------------------*
* Function name    : getFuncTbl
* Function         : get FuncTbl by selector
                     If pfuncTbl is null, will malloc.
* Return value	   : void
* Parameter1	   : elapsed time (UINT32 5s/10s/20s (unit second))
* History          :
*                  : 2018.06.01  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

pcallbackFuncTbl_t getFuncTbl( UINT32 selector ) {
	pcallbackFuncTbl_t pfuncTbl = NULL;

	switch( selector ) {
		case SP5K_CAPTURE_CALLBACK_YUV_WITH_JPEG: //YUV with JPEG
			pfuncTbl = &yuvCallbackFuncTbl;
			if( pfuncTbl->funcDelegate == NULL ) {
				UINT32 size = sizeof( callbackFunc ) * pfuncTbl->size;
				pfuncTbl->funcDelegate = sp5kMalloc( size );	
				memset( pfuncTbl->funcDelegate, 0, size ); 
			}
			break;
		default:
			break;
	}

	return pfuncTbl;
}

/*--------------------------------------------------------------------------*
* Function name    : appCallbackRegisterwithIdx
* Function         : The flow which register new function with index.
* Return value	   : result (UINT32 SUCCESS/FAIL)
* Parameter1	   : selector (UINT32 switch case, you can expand)
* Parameter2	   : pfunc (void* new function)
* Parameter3	   : func Index (UINT32 0~(funcTbl size-1))
* History          :
*                  : 2018.06.01  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32 appCallbackRegisterwithIdx( UINT32 selector, void* pfunc, UINT32 idx ) {
	
	pcallbackFuncTbl_t pfuncTbl = getFuncTbl( selector );	
	if( pfuncTbl == NULL ) {
        HOST_PROF_LOG_PRINT(LEVEL_ERROR,"cbset: selector is wrong %s", __FUNCTION__);
		return FAIL;
	}
	
	if( !isFuncRegistered( pfuncTbl, pfunc ) ) {
		UINT32 ret;
		ret = registerFuncwithIdx( pfuncTbl, pfunc, idx );
		if( ret == FAIL ) {
            HOST_PROF_LOG_PRINT(LEVEL_ERROR,"cbset: callback func register: array is full %s"\
                                , __FUNCTION__);
			return ret;
		}
	}

	return SUCCESS;
}

/*--------------------------------------------------------------------------*
* Function name    : appCallbackRegister
* Function         : register new function direct
* Return value	   : result (UINT32 SUCCESS/FAIL)
* Parameter1	   : selector (UINT32 switch case, you can expand)
* Parameter2	   : pfunc (void* new function)
* History          :
*                  : 2018.06.01  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32 appCallbackRegister( UINT32 selector, void* pfunc ) {
	

	pcallbackFuncTbl_t pfuncTbl = getFuncTbl( selector );	
	if( pfuncTbl == NULL ) {
		HOST_PROF_LOG_PRINT(LEVEL_ERROR,"cbset: selector is wrong %s", __FUNCTION__);
		return FAIL;
	}
	
	if( !isFuncRegistered( pfuncTbl, pfunc ) ) {
		UINT32 ret;
		ret = registerFunc( pfuncTbl, pfunc );
		if( ret == FAIL ) {
            HOST_PROF_LOG_PRINT(LEVEL_ERROR,"cbset: callback func register: array is full %s"\
                                , __FUNCTION__);
			return ret;
		}
	}

	return SUCCESS;
}

/*--------------------------------------------------------------------------*
* Function name    : appCallbackUnregister
* Function         : unregister pfunc function
* Return value	   : result 
  (UINT32 SUCCESS / FAIL:pfuncTbl=NULL / (-1):funcTbl->funcDelegate free)
* Parameter1	   : selector (UINT32 switch case, you can expand)
* Parameter2	   : pfunc (void* pointer function)
* History          :
*                  : 2018.06.01  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32 appCallbackUnregister( UINT32 selector, void* pfunc ) {

	pcallbackFuncTbl_t pfuncTbl = getFuncTbl( selector );	
	if( pfuncTbl == NULL ) {
		HOST_PROF_LOG_PRINT(LEVEL_ERROR,"cbset: selector is wrong %s", __FUNCTION__);
		return FAIL;
	}

	return unregisterFunc( pfuncTbl, pfunc );
}

/*--------------------------------------------------------------------------*
* Function name    : appCallbackFuncGet
* Function         : Get cbfunc point
* Return value	   : pfunc (void* pointer function, you can expand)
* Parameter1	   : selector
                     (UINT32 SP5K_CAPTURE_CALLBACK_YUV_WITH_JPEG, you can expand)
* History          :
*                  : 2018.06.01  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void* appCallbackFuncGet( UINT32 selector ) {
	void* pfunc = NULL;

	switch( selector ) {
		case SP5K_CAPTURE_CALLBACK_YUV_WITH_JPEG:
			pfunc = yuvCallbackFunc;
			break;
		default:
			break;
	}

	return pfunc;
}
