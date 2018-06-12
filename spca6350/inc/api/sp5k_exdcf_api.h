/**************************************************************************
 *
 *       Copyright (c) 2005-2014 by iCatch Technology, Inc.
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
 *  Hsin-Chu, Taiwan, R.O.C.
 *
 **************************************************************************/
/**
 * @file sp5k_exdcf_api.h
 *
 * @author rx.liao
 */
#ifndef _SP5K_DCF_API_H_
#define _SP5K_DCF_API_H_

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "common.h"
#include "middleware/disp_def.h"

/**************************************************************************
 *                          C O N S T A N T S
 **************************************************************************/
/*define macro for dcf file type the dcf module could recognize*/
#define SP5K_EXDCF_FILETYPE_JPG	0x0001
#define SP5K_EXDCF_FILETYPE_ASF	0x0002
#define SP5K_EXDCF_FILETYPE_AVI	0x0004
#define SP5K_EXDCF_FILETYPE_MOV	0x0008
#define SP5K_EXDCF_FILETYPE_MP3	0x0010
#define SP5K_EXDCF_FILETYPE_WAV	0x0020
#define SP5K_EXDCF_FILETYPE_WMA	0x0040
#define SP5K_EXDCF_FILETYPE_MP4	0x0080
#define SP5K_EXDCF_FILETYPE_AAC	0x0100
#define SP5K_EXDCF_FILETYPE_MPO	0x0200
#define SP5K_EXDCF_FILETYPE_THM	0x0400
#define SP5K_EXDCF_FILETYPE_MTS	0x0800
#define SP5K_EXDCF_FILETYPE_EXT 	0x8000

#define SP5K_EXDCF_FILETYPE_JPG_MEMO (SP5K_EXDCF_FILETYPE_JPG \
                                  | SP5K_EXDCF_FILETYPE_WAV)
#define SP5K_EXDCF_FILETYPE_JPG_MOV  (SP5K_EXDCF_FILETYPE_JPG \
                                  | SP5K_EXDCF_FILETYPE_MOV)
#define SP5K_EXDCF_FILETYPE_MASK (SP5K_EXDCF_FILETYPE_JPG \
	                           | SP5K_EXDCF_FILETYPE_ASF \
							   | SP5K_EXDCF_FILETYPE_AVI \
							   | SP5K_EXDCF_FILETYPE_MOV \
                               | SP5K_EXDCF_FILETYPE_MP3 \
                               | SP5K_EXDCF_FILETYPE_WAV \
                               | SP5K_EXDCF_FILETYPE_WMA \
                               | SP5K_EXDCF_FILETYPE_MP4 \
                               | SP5K_EXDCF_FILETYPE_AAC \
                               | SP5K_EXDCF_FILETYPE_MPO\
                               | SP5K_EXDCF_FILETYPE_THM\
                               | SP5K_EXDCF_FILETYPE_MTS\
                               | SP5K_EXDCF_FILETYPE_EXT)

/*define value for folder option when delete object*/
#define SP5K_EXDCF_DEL_EMPTY_DIR 0x0000
#define SP5K_EXDCF_DEL_FILE_ONLY 0x0100

/*define for the sub sys ID*/
#define SP5K_EXDCF_SUBSYS_ID_UNDEFINE			-1
#define SP5K_EXDCF_CUS_SYS_ID					0
#define SP5K_EXDCF_DCF_SYS_ID					1

/*define for sort mode*/
#define SP5K_EXDCF_SORT_BY_TIME					0x00001
#define SP5K_EXDCF_SORT_BY_KEY					0x00002

/*define for the config option,the config is set before init*/
#define SP5K_EXDCF_FILETYPE_CONFIG				0x0000
#define SP5K_EXDCF_MAX_TOTAL_FILE_CONFIG		0x0001
#define SP5K_EXDCF_MAX_FOLDERFILE_CONFIG		0x0002
#define SP5K_EXDCF_FILESIZE_ZERO_DEL_CONFIG		0x0003
#define SP5K_EXDCF_DUP_DIRKEY_CONFIG			0x0004
#define SP5K_EXDCF_DUP_FILEKEY_CONFIG			0x0005
#define SP5K_EXDCF_LOCKFILE_SHOW_CONFIG			0x0006
#define SP5K_EXDCF_HIDDENFILE_SHOW_CONFIG		0x0007
#define SP5K_EXDCF_RECOVER_FILETYPE_CONFIG		0x0008
#define SP5K_EXDCF_VFS_TIME_CONFIG				0x0009


/*define for option set,these option will be set after sys init */
#define SP5K_EXDCF_OPT_DUP_FILEKEY				0x0001
#define SP5K_EXDCF_OPT_DIR_FREECHAR				0x0002
#define SP5K_EXDCF_OPT_FILE_FREECHAR			0x0003
#define SP5K_EXDCF_OPT_DUP_DIRKEY				0x0004
#define SP5K_EXDCF_OPT_USE_MOV_MEMO				0x0005
#define SP5K_EXDCF_OPT_DIR_CLUDE_FILTER			0x0006
#define SP5K_EXDCF_OPT_DUPKEY_FILTER			0x0007
#define SP5K_EXDCF_OPT_DIR_DELETE				0x0008
#define SP5K_EXDCF_OPT_FILE_TYPE_EXTEND  		0x0009

/*define for exDcf info Get*/
#define SP5K_EXDCF_INFO_CUR_TOTALFILE			0x0002
#define SP5K_EXDCF_SUBINFO_TOTAL_DIRNUM			0x0003
#define SP5K_EXDCF_SUBINFO_TOTALFILE			0x0004
#define SP5K_EXDCF_SUBINFO_CUR_DIRNUM			0x0005
#define SP5K_EXDCF_SUBINFO_CUR_FILENUM			0x0006
#define SP5K_EXDCF_INFO_ACTIVE_STORAGE			0x1000

/*define for operation*/
/*define for dcf file operation type*/
#define SP5K_EXDCF_FILEOP_COPY	         		0x0001
#define SP5K_EXDCF_FILEOP_COPYALL	         	0x0002
#define SP5K_EXDCF_FILEOP_DEL              		0x0003
#define SP5K_EXDCF_FILEOP_DELALL	         	0x0004
#define SP5K_EXDCF_FILEOP_LOCK	         		0x0005
#define SP5K_EXDCF_FILEOP_LOCKALL	         	0x0006
#define SP5K_EXDCF_FILEOP_UNLOCK	         	0x0007
#define SP5K_EXDCF_FILEOP_UNLOCKALL        		0x0008
#define SP5K_EXDCF_FILEOP_FORMAT	         	0x0009
#define SP5K_EXDCF_FILEOP_ABORT	         		0x000A
#define SP5K_EXDCF_FILEOP_COPY_BY_DIR      		0x000B
#define SP5K_EXDCF_FILEOP_COPYALL_BY_DIR   		0x000C
#define SP5K_EXDCF_FILEOP_DEL_BY_DIR			0x000D
#define SP5K_EXDCF_FILEOP_RECOVER_FAT      		0x000F
#define SP5K_EXDCF_FILEOP_RECOVER_FILE     		0x0010


/*define macro for filter option*/ 
#define SP5K_FILTER_ALL		0x0000
#define SP5K_FILTER_SUBSYS	0x0001

/*define for the dir attr*/
#define SP5K_EXDCF_DIRATTR_NONE			0
#define SP5K_EXDCF_DIRATTR_DUP			1
#define SP5K_EXDCF_DIRATTR_HIDE			2
#define SP5K_EXDCF_DIRATTR_NORMAL		3

/*define dcf file attribution*/
#define SP5K_EXDCF_FILE_ATTR_LOCK			0x0001
#define SP5K_EXDCF_FILE_ATTR_MEMO			0x0002
#define SP5K_EXDCF_FILE_ATTR_HIDE			0x0004

/*define for sp5kExDcfDirInfoGet*/
#define SP5K_EXDCF_DIRINFO_MAX_FILEKEY	0x0000
#define SP5K_EXDCF_DIRINFO_TOTAL_CHILD	0x0001
#define SP5K_EXDCF_DIRINFO_FREECHAR		0x0002

/*define for the option,the set is after init*/

#define SP5K_ERR_SUCCESS	             0x00000000 /*Function is implemented without any errors*/
#define SP5K_ERR_EXDCF_GROUP             0x06000000 /*all dcf error group*/

#define SP5K_EXDCF_FULL_PATH_BUF_SIZE 	50
/**************************************************************************
 *                          D A T A   T Y P E S
 **************************************************************************/
/*define call back type for file operate*/
typedef void (*pfsp5kExdcfCallBack_t)(UINT32, UINT32, void *);
typedef void (*sp5kExDcfCustomSortKeySetCb_t)(void*, UINT32*);

typedef struct sp5kExDcfTime
{
	UINT16 sec;
	UINT16 min;
	UINT16 hour;
	UINT16 day;  /*day of month: 1..31*/
	UINT16 mon;  /*month of year: 1..12*/
	UINT16 year; /*calendar year, eg, 2013*/
}sp5kExDcfTime_t;

typedef struct sp5kExDcfAttrElm
{
  UINT16 dirNum;
  UINT16 fileNum;
  sp5kExDcfTime_t time;
  UINT8 dirname[9];
  UINT8 filename[32];
  UINT32 fileType;
  UINT32 filesize;
  UINT32 attribute;
} sp5kExDcfAttrElm_t;

typedef struct sp5kExDcfAppCbFuncSet_s{
	UINT32 (*pfnsp5kCbUpdateNextKey)();
	UINT32 (*pfnsp5kCbSynchDelDir)(UINT8 *rootDir,UINT32 dirKey);
	UINT32 (*pfnsp5kCbSaveDirConfirm)(UINT8 *rootDir, UINT32 fileType);
}sp5kExDcfAppCbFuncSet_t;

typedef struct sp5kExDcfCustomSortInfo_s{
	UINT32 sortKey;
	UINT32 uiStartIdx;
	UINT32 uiCount;
	struct sp5kExDcfCustomSortInfo_s *Next;
}sp5kExDcfCustomSortInfo_t;

typedef struct sp5kExDcfRecoverFileList
{
    UINT32 	fileType;
    UINT8	pathName[SP5K_EXDCF_FULL_PATH_BUF_SIZE];
    struct 	sp5kExDcfRecoverFileList * pnext;
}sp5kExDcfRecoverFileList_t;

typedef struct sp5kExDcfCustomSortKeySetCbInfo_s
{
	UINT16 dirNum;
	UINT16 fileNum;
	UINT8* fileFreeChar;
} sp5kExDcfCustomSortKeySetCbInfo_t;

/**************************************************************************
 *              F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
void sp5kExDcfStdSysInit(sp5kExDcfAppCbFuncSet_t* pDcfCbObj);
void sp5kExDcfCusSysInit(sp5kExDcfAppCbFuncSet_t* pDcfCbObj);
UINT32 sp5kExDcfSubsysReg(UINT8* subSysName,UINT32 subSysId);
UINT32 sp5kExDcfPreInit(UINT32 maxNumItem);
UINT32 sp5kExDcfInit(UINT32 storageId,UINT32 reserved,UINT32 async);
UINT32 sp5kExDcfTerm(UINT32 storageId);
UINT32 sp5kExDcfActive(UINT32 storageID);
UINT32 sp5kExDcfDirConfig(UINT8 *subSysName, UINT8 *curRootPath, UINT8 *dirFreeChar, UINT32 minDirKey, UINT32 maxDirKey);
UINT32 sp5kExDcfFileConfig(UINT8 *subSysName, UINT8 *fileFreeChar, UINT32 minFileKey, UINT32 maxFileKey);
UINT32 sp5kExDcfConfig(UINT8 *subSysName, UINT32 option, UINT32 value);
UINT32 sp5kExDcfOptionSet(UINT8 * dirName, UINT32 option, ...);
UINT32 sp5kExDcfIdxGet(UINT32 *pfileIdx);
UINT32 sp5kExDcfIdxSet(UINT32 fileIdx);
UINT32 sp5kExDcfNextKeyGet(UINT8* rootDir,UINT32* dirkey,UINT32* filekey);
UINT32 sp5kExDcfNextKeySet(UINT8* rootDir,UINT32 dirkey,UINT32 filekey);
UINT32 sp5kExDcfInfoGet(UINT8 *subSysName,UINT32 option,UINT32* pvalue);
UINT32 sp5kExDcfFileFilter(UINT32 uiOption,...);
UINT32 sp5kExDcfGetIdxByFn(UINT8 *pfilename,UINT32 *pIndx);
UINT32 sp5kExDcfSortModeSet(UINT32 sortMode);
UINT32 sp5kExDcfFileOperate(UINT32 optCode,UINT32 param,UINT32 async,pfsp5kExdcfCallBack_t pcallback,UINT32 length);
UINT32 sp5kExDcfFileAttrGet(UINT32 fileIndex, sp5kExDcfAttrElm_t *pfileAttr, UINT8 * pfilename);
UINT32 sp5kExDcfDelFilesByDir(UINT8 * rootDir,  UINT32 dirKey);
UINT32 sp5kExDcfGetMaxDir(UINT8 *subSysName,UINT16 *usKey,UINT8 *aucName,UINT8 *dirAttr);
UINT32 sp5kExDcfDirInfoGet(UINT8 *subSysName,UINT16 dirKey,UINT32 option,void *pValue);
UINT32 sp5kExDcfCurKeyGet(UINT8 *subSysName,UINT16 *pDirKey, UINT16 *pFileKey);
UINT32 sp5kExDcfFindMaxKey(UINT8 *subSysName,UINT8 *dirStr,UINT16 *pMaxDirKey,UINT16 *pMaxFileKey);
UINT32 sp5kExDcfFreeCharGet(UINT8 *subSysName,UINT8 *pDirFreeChar,UINT8 *pFileFreeChar);
UINT32 sp5kExDcfSubFreeCharSet(UINT8 *subSysName,UINT32 subFreeCharNum,UINT8 **fileFreeChar);
UINT32 sp5kExDcfGetIdxByKey(UINT8 *subSysName,UINT16 dirKey,UINT16 fileKey,UINT32 *pIndex);
UINT32 sp5kExDcfCustomSortKeySet(UINT8 * subSysName, UINT16 usDirKey, UINT16 usFileKey, UINT32 uiSortKey);
UINT32 sp5kExDcfCustomSortKeySetByIdx(UINT8 * subSysName, UINT32 uiStartIdx, UINT32 uiCount, UINT32 uiSortKey);
UINT32 sp5kExDcfCustomSortKeySetByUserCallback(UINT8 *subSysName, void* pReserve, sp5kExDcfCustomSortKeySetCb_t callback);
UINT32 sp5kExDcfCustomSortModeSet(UINT8 * subSysName, UINT32 filterMask);
UINT32 sp5kExDcfCustomSortInfoGet(UINT8 *subSysName,sp5kExDcfCustomSortInfo_t ** ppCustomInfo,UINT32 *pnum);
UINT32 sp5kExDcfMemoAttrSet(void);
#endif  /* _SP5K_DISP_API_H_ */

