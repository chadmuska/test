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
 *	File name	: app_dcf.c
 *	Function	: dcf function
 *	First editor: Weikai Hsu
 *	First date	: 	
 *	History		: 	
 *	            : 2018.06.01  Weikai Hsu "Added File Header"
 *	
 *	
 *	
 *--------------------------------------------------------------------------*/
#define HOST_DBG 1
#include "sp5k_media_api.h"
#include "app_com_def.h"
#include "app_com_api.h"
#include "app_dcf.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define MAX(a, b)		( (a) > (b) ? (a) : (b) )
#define IS_DCF_KEY_MAX(dir, file)  ((dir == DCF_DIR_NUM_MAX) && (file == DCF_FILE_NUM_MAX))
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct dcfKeyInfo_s
{
	BOOL dcfKeyFull;	//999-9999
	BOOL createDir;
	UINT16 nextDirKey;
	UINT16 nextFileKey;
} dcfKeyInfo_t;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static dcfKeyInfo_t dcfKey = {FALSE,FALSE,0,0};

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/*--------------------------------------------------------------------------*
* Function name : appDcfDefaultFreeCharGet 
* Function		: get default free char 
* Return value	: void 
* Parameter1	: UINT8 *szdir
* Parameter2	: UINT8 *szfile
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void
_dcfDefaultFreeCharGet(
	UINT8 *szdir,
	UINT8 *szfile
)
{
	if ((szdir == NULL) ||(szfile == NULL))
	{
        DBG_PRINT("dcf: %s (%d)\n",__FUNCTION__,__LINE__);
		return;
	}

	memcpy(szdir,  DCF_DIR_FREE_CHAR,  6);
	memcpy(szfile, DCF_FILE_FREE_CHAR, 6);
	szdir[5] = 0;
	szfile[5] = 0;
}
/*--------------------------------------------------------------------------*
* Function name : dcfKeyGet 
* Function		: get dcf key
* Return value	: void 
* Parameter1	: UINT16 *dirKey
* Parameter2	: UINT16 *fileKey
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void
_dcfKeyGet(
	UINT16 *dirKey,
	UINT16 *fileKey
)
{

/*paul 2018.04.16 modify for compiler warning*/
#if !defined(HW_17DVR)&& !defined(HW_19DVR)
	UINT32 totalFile;
       sp5kDcfAttrElm_t fileAttr;

       /* find the last file and get last folder number, and file number */
	sp5kDcfFsInfoGet(SP5K_DCF_INFO_TOTAL_FILENUM,&totalFile);
	if(totalFile)
	{
		UINT8  fname[32];
		sp5kDcfFsFileAttrGet(totalFile, &fileAttr,fname );
		*dirKey =  fileAttr.dirNum;
		*fileKey =  fileAttr.fileNum;
	}
	else
#endif
	{
		*dirKey = 0;
		*fileKey = 0;
	}
}

/*--------------------------------------------------------------------------*
* Function name : appDcfInit 
* Function		: dcf init
* Return value	: UINT32 
* Parameter1	: UINT32 drive
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32
appDcfInit(
	UINT32 drive
)
{
	UINT32 totalFile;
	UINT8 dirChar[6],fileChar[6];
    sp5kDcfAttrElm_t fileAttr;

	sp5kMediaRecCfgSet(SP5K_MEDIA_REC_RECOVERY_EN, 0);
	sp5kDcfFsOptionSet(SP5K_DCF_OPT_FILE_SIZE_ZERO_DEL, 0);

    #if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
    char *freechar[5] = {DCF_DUAL_FREE_CHARA, DCF_DUAL_FREE_CHARB};
	sp5kDcfFsSubFreeCharSet(2, (UINT8 **)freechar);
    #endif

	//dcf init
	UINT32 ret = sp5kDcfFsInit(drive,0,0);
	if (SUCCESS != ret )
	{
		return ret;
	}

	ret = sp5kDcfFsActive(drive);
	if( SUCCESS != ret ) {
		return ret;
	}
	//free char set
	_dcfDefaultFreeCharGet(dirChar,fileChar);
	sp5kDcfFsFreeCharSet(dirChar, fileChar, SP5K_DCF_NEXTKEY_MAX);
	//
	sp5kDcfFsInfoGet(SP5K_DCF_INFO_TOTAL_FILENUM,&totalFile);
	if(totalFile)
	{
		UINT16 dirKey, fileKey;
		UINT8  fname[32], tmpBuff[6];
		memset(tmpBuff ,0,sizeof(tmpBuff ));
		sp5kDcfFsFileAttrGet(totalFile, &fileAttr,fname );
		memcpy(tmpBuff,&fileAttr.dirname[3],5);
		dirKey =  fileAttr.dirNum;
		fileKey =  fileAttr.fileNum;
		DBG_PRINT("dcf: exist DCF key (%d,%d) - (%s)\n",dirKey,fileKey,fileAttr.dirname);
		DBG_PRINT("dcf: exist DCF dir char (%s,%s)\n",dirChar,fileAttr.dirname);

		//check if 999-9999 already?
		if(IS_DCF_KEY_MAX(dirKey,fileKey))
		{
			dcfKey.dcfKeyFull = TRUE;
			DBG_PRINT("dcf: No more free DCF key!!\n");
		}
		//check free char of max dir
		else if(strcmp((const char *)dirChar, (const char *)tmpBuff) != 0)
		{
			if (dirKey == DCF_DIR_NUM_MAX)
			{
				dcfKey.dcfKeyFull = TRUE;
				DBG_PRINT("dcf: No more free DCF key!!\n");
			}
			else{
				dcfKey.createDir = TRUE;
				DBG_PRINT("dcf: Create new dir!!!\n");
			}
		}
	}
	//
	/*
	appDcfUserSettingGet(&fnumRule,&dirKey,&fileKey);
	appDcfNexDcfKeySet((fnumRule == FILENUM_RESET),dirKey,fileKey);
	*/

	return SUCCESS;
}

/*--------------------------------------------------------------------------*
* Function name : appDcfNexDcfKeySet 
* Function		: next dcf key set
* Return value	: UINT32 
* Parameter1	: UINT32 isReset
* Parameter2	: UINT16 dirKey
* Parameter3	: UINT16 fileKey
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32
appDcfNexDcfKeySet(
	UINT32 isReset,
 	UINT16 dirKey,
 	UINT16 fileKey
)
{
	UINT16 existDirKey,existFileKey;
	UINT16 newDirKey,newFileKey;


	DBG_PRINT("dcf: %s (%d,%d,%d)\n",__FUNCTION__,isReset,dirKey,fileKey);

	newDirKey = dirKey;
	newFileKey = fileKey;

	if (dcfKey.dcfKeyFull)
	{
		return FAIL;
	}

	_dcfKeyGet(&existDirKey,&existFileKey);

	//if exist dcf key is larger than input config key
	if (newDirKey < existDirKey)
	{
		newDirKey = existDirKey;
		newFileKey = existFileKey;
	}
	else if (newDirKey == existDirKey)
	{
		newFileKey = MAX(newFileKey,existFileKey);
	}
	else if (isReset)
	{ //if file num setting is RESET and exist dcf key is smaller than input config key
		newDirKey = existDirKey;
		newFileKey = existFileKey;
	}

	if (newDirKey< DCF_DIR_NUM_MIN)
	{
		newDirKey = DCF_DIR_NUM_MIN;
	}

	if(IS_DCF_KEY_MAX(newDirKey,newFileKey))
	{
		dcfKey.dcfKeyFull = TRUE;
		DBG_PRINT("dcf: No more free DCF key!!\n");
		return FAIL;

	}

	newFileKey ++;
	if((newFileKey > DCF_FILE_NUM_MAX) || dcfKey.createDir)
	{
		dcfKey.createDir = FALSE;
		newDirKey ++;
		newFileKey = DCF_FILE_NUM_MIN;
	}

	if(IS_DCF_KEY_MAX(newDirKey,newFileKey))
	{
		dcfKey.dcfKeyFull = TRUE;
	}

	DBG_PRINT("dcf: next KEY set (%d,%d)\n",newDirKey, newFileKey);

	dcfKey.nextDirKey = newDirKey;
	dcfKey.nextFileKey = newFileKey;

	sp5kDcfFsNextKeySet(newDirKey, newFileKey);

	return SUCCESS;

}

/*--------------------------------------------------------------------------*
* Function name : appDcfKeyFullCkeck 
* Function		: dck key full check
* Return value	: UINT32 
* Parameter1	: void
* History	    :
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32
appDcfKeyFullCkeck(
	void
)
{
	UINT16 existDirKey,existFileKey;

    	_dcfKeyGet(&existDirKey,&existFileKey);

	if(IS_DCF_KEY_MAX(existDirKey,existFileKey))
	{
		return TRUE;
	}
	else if (dcfKey.dcfKeyFull)
	{
		return TRUE;
	}
	return FALSE;
}

/*--------------------------------------------------------------------------*
* Function name : appDcfKeyFullCkeck 
* Function		: dcf remain key query
* Return value	: bool 
* Parameter1	: UINT32 requestNum
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
BOOL
appDcfRemainKeyQuery(
 	UINT32 requestNum
)
{
	UINT16 existDirKey,existFileKey;
	_dcfKeyGet(&existDirKey, &existFileKey);
	if (((existFileKey + requestNum -1) > DCF_FILE_NUM_MAX)
		&& (existDirKey == DCF_DIR_NUM_MAX))
	{
		return FALSE;
	}
	return TRUE;
}

/*--------------------------------------------------------------------------*
* Function name : appDcfKeySettingUpdate 
* Function		: dcf key setting update
* Return value	: void 
* Parameter1	: UINT16 dirKey
* Parameter2	: UINT16 fileKey
* History	    : 
*			    : 2018.06.01  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void
appDcfKeySettingUpdate(
	UINT16 *dirKey,
	UINT16 *fileKey
)
{
	UINT16 nextDirKey,nextFileKey;
	sp5kDcfFsNextKeyGet(&nextDirKey, &nextFileKey);
	DBG_PRINT("dcf: next KEY get (%d,%d)",nextDirKey, nextFileKey);
	if ((nextDirKey == dcfKey.nextDirKey) && (nextFileKey == dcfKey.nextFileKey))
	{
		DBG_PRINT("dcf: key setting update : do nothing, no file is created\n");
		return;
	}
	_dcfKeyGet(dirKey,fileKey);
	DBG_PRINT("dcf: key setting update (%d,%d)\n",*dirKey,*fileKey);
}

