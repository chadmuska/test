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
 *                                                                        *
 *  Author: alan_chuang                                                   *
 *                                                                        *
 **************************************************************************/
#ifndef SP5K_DCF_API_H__
#define SP5K_DCF_API_H__

#include "api/sp5k_global_api.h"  /*fpsp5kCallback_t*/

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
/* Add this option to file index into sp5kDcfFsFileAttrGet() will get memo info instead JPG info */
#define SP5K_DCF_FILEIDX_MEMO_OPT		0x80000000
#define SP5K_DCF_FULL_PATH_BUF_SIZE 	50

/*define macro for dcf file type the dcf module could recognize*/
#define SP5K_DCF_FILETYPE_JPG	0x0001
#define SP5K_DCF_FILETYPE_ASF	0x0002
#define SP5K_DCF_FILETYPE_AVI	0x0004
#define SP5K_DCF_FILETYPE_MOV	0x0008
#define SP5K_DCF_FILETYPE_MP3	0x0010
#define SP5K_DCF_FILETYPE_WAV	0x0020
#define SP5K_DCF_FILETYPE_WMA	0x0040
#define SP5K_DCF_FILETYPE_MP4	0x0080
#define SP5K_DCF_FILETYPE_AAC	0x0100
#define SP5K_DCF_FILETYPE_MPO	0x0200
#define SP5K_DCF_FILETYPE_THM	0x0400
#define SP5K_DCF_FILETYPE_EXT 0x8000


#define SP5K_DCF_FILETYPE_JPG_MEMO (SP5K_DCF_FILETYPE_JPG \
                                  | SP5K_DCF_FILETYPE_WAV)
#define SP5K_DCF_FILETYPE_JPG_MOV  (SP5K_DCF_FILETYPE_JPG \
                                  | SP5K_DCF_FILETYPE_MOV)
#define SP5K_DCF_FILETYPE_MASK (SP5K_DCF_FILETYPE_JPG \
	                           | SP5K_DCF_FILETYPE_ASF \
							   | SP5K_DCF_FILETYPE_AVI \
							   | SP5K_DCF_FILETYPE_MOV \
                               | SP5K_DCF_FILETYPE_MP3 \
                               | SP5K_DCF_FILETYPE_WAV \
                               | SP5K_DCF_FILETYPE_WMA \
                               | SP5K_DCF_FILETYPE_MP4 \
                               | SP5K_DCF_FILETYPE_AAC \
                               | SP5K_DCF_FILETYPE_MPO\
                               | SP5K_DCF_FILETYPE_THM\
                               | SP5K_DCF_FILETYPE_EXT)

/*define value for dcf next mode,default value is APP_DCF_NEXTKEY_MAX*/
#define SP5K_DCF_NEXTKEY_MAX             0x0000
#define SP5K_DCF_NEXTKEY_MIN             0x0001

/*define value for folder option when delete object*/
#define SP5K_DCF_DEL_EMPTY_DIR 0x0000
#define SP5K_DCF_DEL_FILE_ONLY 0x0100

/*define the FS Type */
#define SP5K_FS_TYPE_FAT                 0x0000
#define SP5K_FS_TYPE_FAT12               0x0003
#define SP5K_FS_TYPE_FAT16               0x0004
#define SP5K_FS_TYPE_FAT32               0x0001
#define SP5K_FS_TYPE_EXFAT               0x0002

/*define for dcf file operation type*/
#define SP5K_DCF_FILEOP_COPY	         0x0001
#define SP5K_DCF_FILEOP_COPYALL	         0x0002
#define SP5K_DCF_FILEOP_DEL              0x0003
#define SP5K_DCF_FILEOP_DELALL	         0x0004
#define SP5K_DCF_FILEOP_LOCK	         0x0005
#define SP5K_DCF_FILEOP_LOCKALL	         0x0006
#define SP5K_DCF_FILEOP_UNLOCK	         0x0007
#define SP5K_DCF_FILEOP_UNLOCKALL        0x0008
#define SP5K_DCF_FILEOP_FORMAT	         0x0009
#define SP5K_DCF_FILEOP_ABORT	         0x000A
#define SP5K_DCF_FILEOP_COPY_BY_DIR      0x000B
#define SP5K_DCF_FILEOP_COPYALL_BY_DIR   0x000C
#define SP5K_DCF_FILEOP_RECOVER_FAT      0x000D
#define SP5K_DCF_FILEOP_RECOVER_FILE     0x000E

/*define for sp5kDcfFsInfoGet operate type*/
#define SP5K_DCF_INFO_TOTAL_DIRNUM       0x0001
#define SP5K_DCF_INFO_TOTAL_FILENUM      0x0002
#define SP5K_DCF_INFO_CUR_DIRNUM         0x0003
#define SP5K_DCF_INFO_CUR_FILENUM        0x0004
#define SP5K_DCF_INFO_ACTIVE_STORAGE     0x1000

/*define dcf file attribution*/
#define SP5K_DCF_FILE_ATTR_LOCK	         0x0001
#define SP5K_DCF_FILE_ATTR_MEMO	         0x0002
#define SP5K_DCF_FILE_ATTR_HIDE          0x0004

/*define sort mode*/
#define SP5K_DCF_SORT_BY_KEY             0x00
#define SP5K_DCF_SORT_BY_DATE            0x01

/*define option and value*/
#define SP5K_DCF_OPT_DUP_KEY             0x0001
#define SP5K_DCF_OPT_FORMAT_METHOD       0x0002
#define SP5K_DCF_OPT_FILE_SIZE_ZERO_DEL  0x0003
#define SP5K_DCF_OPT_HIDDEN_ATTR_CFG     0x0004
#define SP5K_DCF_COPY_FILE_MKTIME_METHOD 0x0005 /* obsolete, current dcf file copy always do this */
#define SP5K_DCF_OPT_LOCK_FILE_SHOW      0x0006
#define SP5K_DCF_OPT_FORMAT_FSTYPE       0x0007
#define SP5K_DCF_OPT_VFS_TIME            0x0008 /* the file time return by vfs: 0=ctime, 1=mtime, 2=min of ctime/mtime */
#define SP5K_DCF_OPT_USE_MODIFY_TIME     0x0008 /* obsolete, replaced by SP5K_DCF_OPT_VFS_TIME */
#define SP5K_DCF_OPT_MAX_FILE_LIMIT      0x0009
#define SP5K_DCF_OPT_JPG_MEMO_BYPASS     0x000a
#define SP5K_DCF_RTC_OFFSET_MINUS        0x000b
#define SP5K_DCF_RTC_OFFSET_TICK         0x000c
#define SP5K_DCF_FS_IGNORE_MTIME         0x000d /* 0:mtime=system time, 1:mtime=ctime */
#define SP5K_DCF_FS_COPY_USE_MTIME       0x000e /* 1:ctime=mtime when copy */
#define SP5K_DCF_OPT_CALLBACK_CFG        0x000f /* 20120516. for CVR host who use custom file manage.*/
#define SP5K_DCF_OPT_RECOVERY_FILETYPE	 0x0010
#define SP5K_DCF_OPT_USE_MOV_MEMO        0x0011 /* 0:treat WAV as memo, 1:treat MOV as memo */
#define SP5K_DCF_OPT_DIR_EXCLUDE_FILTER	 0x0012
#define SP5K_DCF_OPT_FILE_DUPKEY_FILTER	 0x0013
#define SP5K_DCF_OPT_CUSTOM_SORT_METHOD  0x0014 /* 0: by key, 1: by date */
#define SP5K_DCF_OPT_FILE_TYPE_EXTEND  0x0015

/*define dup key opt value*/
#define SP5K_DCF_DENY_DUP                0x0000
#define SP5K_DCF_ALLOW_DUP               0x0001

/*define option for sp5kDcfFsDirInfoGet*/
#define SP5K_DCF_DIRINFO_MAX_FILEKEY	0x0000
#define SP5K_DCF_DIRINFO_TOTAL_CHILD	0x0001
#define SP5K_DCF_DIRINFO_FREECHAR		0x0002

/* define hidden attribute (SP5K_DCF_OPT_HIDDEN_ATTR_CFG) */
/*
 * bit[0]: directory hide, 0 -> hidden, 1 -> show
 * bit[1]: file hide, 0 -> hidden, 1 -> show
 */
#define SP5K_DCF_SORT_HIDDEN_DISABLE             0x0000
#define SP5K_DCF_SORT_HIDDEN_FOLDER_ENABLE       0x0001
#define SP5K_DCF_SORT_HIDDEN_FILE_ENABLE         0x0002
#define SP5K_DCF_SORT_HIDDEN_FOLDER_DISABLE SP5K_DCF_SORT_HIDDEN_DISABLE /* obsolete */

/* define vfs time mode: (SP5K_DCF_OPT_VFS_TIME) */
#define SP5K_DCF_OPT_VFS_CTIME			0x0000
#define SP5K_DCF_OPT_VFS_MTIME			0x0001
#define SP5K_DCF_OPT_VFS_MIN_CMTIME		0x0002

#define SP5K_DCF_FORMAT_NORMAL           0x0000
#define SP5K_DCF_FORMAT_FULL             0x0001

#define SP5K_ERR_SUCCESS	             0x00000000 /*Function is implemented without any errors*/
#define SP5K_ERR_DCF_GROUP               0x06000000 /*all dcf error group*/

#define SP5K_ERR_DCF_PARAM_GROUP         0x06010000 /*Illegal paramter input*/
#define SP5K_ERR_DCF_PARAM_STORAGEID     0x06010001 /*Illegal storage ID */
#define SP5K_ERR_DCF_PARAM_FILENAME      0x06010002 /*Illegal file(folder) name or free char */
#define SP5K_ERR_DCF_PARAM_FILEKEY       0x06010003 /*Illegal dcf file(folder) number*/
#define SP5K_ERR_DCF_PARAM_FILE_TYPE     0x06010004 /*Illegal dcf file type or type set*/
#define SP5K_ERR_DCF_PARAM_FILEIDX       0x06010005 /*Illegal dcf file index*/
#define SP5K_ERR_DCF_PARAM_OPERCMD       0x06010006 /*Illegal dcf api function operation command*/
#define SP5K_ERR_DCF_PARAM_NEXT_MODE     0x06010007 /*Illegal dcf next mode*/
#define SP5K_ERR_DCF_PARAM_NULLPTR       0x06010008 /*Null pointer parameter input*/

#define SP5K_ERR_DCF_FSOP_GROUP          0x06020000 /*Call FS operation function failed*/
#define SP5K_ERR_DCF_FSOP_MOUNT          0x06020001 /*Mount or umount device failed*/
#define SP5K_ERR_DCF_FSOP_FORMAT         0x06020002 /*Format device failed*/
#define SP5K_ERR_DCF_FSOP_CREAT_OPEN     0x06020003 /*Create or open file(or folder) failed*/
#define SP5K_ERR_DCF_FSOP_RDWR           0x06020004 /*Read or write for a file(or folder) failed*/
#define SP5K_ERR_DCF_FSOP_REMOVE         0x06020005 /*Delete a file(or folder) failed*/
#define SP5K_ERR_DCF_FSOP_ATTR           0x06020006 /*Get or set the attribute of file failed*/

#define SP5K_ERR_DCF_RES_GROUP           0x06030000 /*Dcf library resource error*/
#define SP5K_ERR_DCF_RES_UNINIT          0x06030001 /*Dcf library resource not init*/
#define SP5K_ERR_DCF_RES_MAX_MNTPNT      0x06030002 /*Max device mount point reached for dcf lib configure*/
#define SP5K_ERR_DCF_RES_MAX_KEY         0x06030003 /*Dcf file number used up*/
#define SP5K_ERR_DCF_RES_MEM_ALLOC       0x06030004 /*Allocate memery failed*/

#define SP5K_ERR_DCF_MISC_GROUP          0x06040000 /*Miscellaneous error for dcf library.*/
#define SP5K_ERR_DCF_MISC_INIT_FAIL      0x06040001 /*Init dcf library failed*/
#define SP5K_ERR_DCF_MISC_DEV_UNINIT     0x06040002 /*The storage not inited*/
#define SP5K_ERR_DCF_MISC_DEV_INITED     0x06040003 /*The Storage already inited*/
#define SP5K_ERR_DCF_MISC_NONE_FILE      0x06040004 /*None dcf file on the storage*/
#define SP5K_ERR_DCF_MISC_FILE_NOT_EXIST 0x06040005 /*The folder or file not exist on the storage*/
#define SP5K_ERR_DCF_MISC_FILE_DUP       0x06040006 /*The folder or file key duplicated*/
#define SP5K_ERR_DCF_FSOP_NOSPACE        0x06040007 /*Not enough space for copy*/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct sp5kDcfTime
{
	UINT16 sec;
	UINT16 min;
	UINT16 hour;
	UINT16 day;  /*day of month: 1..31*/
	UINT16 mon;  /*month of year: 1..12*/
	UINT16 year; /*calendar year, eg, 2013*/
}sp5kDcfTime_t;

typedef struct sp5kDcfAttrElm
{
  UINT16 dirNum;
  UINT16 fileNum;
  sp5kDcfTime_t time;
  UINT8 dirname[9];
  UINT8 filename[32];
  UINT32 fileType;
  UINT32 filesize;
  UINT32 attribute;
} sp5kDcfAttrElm_t;

/*so date info get from the sample Nikon maybe:
"2008/1/14", 1, 11
"2008/9/22", 12, 26
"2008/9/23", 38, 79
*/
typedef struct sp5kDcfDateInfo
{
  UINT8 fileDay[11];/*"2008/9/23"*/
  UINT32 startIdx; /*38*/
  UINT32 fileCnt;  /*79*/
  struct sp5kDcfDateInfo* pstNext;
}sp5kDcfDateInfo_t;

/* custom sort callback */
typedef struct sp5kDcfCustomSortKeySetCbInfo_s
{
	UINT16 dirNum;
	UINT16 fileNum;
	UINT8* fileFreeChar;
} sp5kDcfCustomSortKeySetCbInfo_t;

/* custom info*/
typedef struct sp5kDcfCustomInfo
{
  UINT32 sortKey;
  UINT32 startIdx;
  UINT32 fileCnt;
  struct sp5kDcfCustomInfo* pstNext;
}sp5kDcfCustomInfo_t;

/*structure for copy all 3rd parameter,
  tell ap the copy stat, then can change what to do*/
typedef struct sp5kDcfCopyStat
{
    UINT16 srcDirNum;
    UINT16 srcFileNum;
    UINT16 dstDirNum;
    UINT16 dstFileNum;
    UINT16 stopCopy;
}sp5kDcfCopyStat_t;

typedef struct sp5kDcfRecoverFileList
{
    UINT32 	fileType;
    UINT8	pathName[SP5K_DCF_FULL_PATH_BUF_SIZE];
    struct 	sp5kDcfRecoverFileList* pnext;
}sp5kDcfRecoverFileList_t;

/*define call back type for file operate*/
/*typedef void (*pfsp5kCallBack_t)(UINT32, UINT32, void *);*/
typedef void (*sp5kDcfCustomSortKeySetCb_t)(void*, UINT32*); /* (sp5kDcfCustomSortKeySetCbInfo_t*, UINT32 *pCustomSortKey) */

/*
*  20120516. This function will set to dcf and let host have chance to change the action of dcf. 
*  Remember these function use for custom file manage only and please keep the definition the same as dcf_api.h
*/
typedef struct sp5kDcfAppCbFuncSet_s{
    UINT32 (*pfnsp5kDcfCbNextFileNameGet)(UINT32 fileType,UINT8 *pfilename);
    UINT32 (*pfnsp5kDcfCbFileNameAdd)(UINT8* pfilename, UINT32 *phandle);
	UINT32 (*pfnsp5kDcfCbFileNameSync)(UINT8 *pfilename, UINT32 handle);
    UINT32 (*pfnsp5kDcfCbCurFileIdxSet)(UINT32 fileIdx);
    UINT32 (*pfnsp5kDcfCbCurFileIdxGet)(UINT32* pfileIdx);
    UINT32 (*pfnsp5kDcfCbFileAttrGet)(UINT32 fileIdx, sp5kDcfAttrElm_t* pfileInfoAtr, UINT8* pname);
    UINT32 (*pfnsp5kDcfCbFsInfoGet)(UINT32 option, UINT32* pValue);
    UINT32 (*pfnsp5kDcfCbFileOperate)(UINT32 option, UINT32 param, UINT32 async, fpsp5kCallback_t pcallback, UINT32 length); 
	UINT32 (*pfnsp5kDcfCbNextSubFileNameGet)(UINT32 fileType,UINT32 subFileType,UINT8 *pfilename);
	UINT32 (*pfnsp5kDcfCbSubFilenameAdd)(UINT8 * pfilename, UINT32 *phandle);
	UINT32 (*pfnsp5kDcfCbSubFileNameSync)(UINT8 *pfilename, UINT32 handle);
}sp5kDcfAppCbFuncSet_t;

typedef struct sp5kStdDcfCallBackFuncSet_s{
	UINT32 (*pfnAppFilenameCheck)(UINT8 *pdirname,UINT8 *pfilename);
}sp5kStdDcfCallBackFuncSet_t;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
UINT32 sp5kDcfFsInit(UINT32 storageId, UINT32 reserved, UINT32 async);
UINT32 sp5kDcfFsActive(UINT32 storageID);
void sp5kDcfFsTerm(UINT32 storageId);
UINT32 sp5kDcfFsFreeCharSet(UINT8* pdirname,UINT8* pfilename,UINT32 option);
UINT32 sp5kDcfFsFreeCharGet(UINT8* pdirname,UINT8* pfilename);
UINT32 sp5kDcfFsNextKeySet(UINT16 dirIdx,UINT16 fileIdx);
UINT32 sp5kDcfFsNextKeyGet(UINT16* pdirIdx,UINT16* pfileIdx);
UINT32 sp5kDcfFsFileTypeSet(UINT32 fileType);
UINT32 sp5kDcfFsCurFileIdxSet(UINT32 fileIdx);
UINT32 sp5kDcfFsCurFileIdxGet(UINT32* pfileIdx);
UINT32 sp5kDcfFsFileOperate(UINT32 option,UINT32 param,UINT32 async,fpsp5kCallback_t pcallback,UINT32 length);
UINT32 sp5kDcfFsInfoGet(UINT32 option,UINT32* pvalue);
UINT32 sp5kDcfFsFileAttrGet(UINT32 fileIdx,sp5kDcfAttrElm_t* pfileInfoAtr,UINT8 *pfilename);
UINT32 sp5kDcfFsFileTimeSet(UINT32 fileIdx, sp5kDcfTime_t *atime, sp5kDcfTime_t *mtime, sp5kDcfTime_t *ctime);
UINT32 sp5kDcfFsMaxKeyGet( UINT8 *pdirStr, UINT16 *pdirKey, UINT16 *pfileKey);
UINT32 sp5kDcfFsDirInfoGet( UINT16 dirKey, UINT32 option, void *pValue);
UINT32 sp5kDcfFsSortModeSet(UINT32 sortMode);
UINT32 sp5kDcfFsDateInfoGet(sp5kDcfDateInfo_t** ppdateInfo, UINT32 *pnum);
UINT32 sp5kDcfFsFileIdxGetByKey(UINT16 usDirKey, UINT16 usFileKey, UINT32 *pfileIdx);
UINT32 sp5kDcfFsFileIdxGetByFilename(UINT8* pfilename, UINT32 *pfileIdx);
UINT32 sp5kDcfFsCurKeyGet(UINT16 *pdirkey, UINT16 *pfilekey);
UINT32 sp5kDcfFsMaxDirGet(UINT16 *pusKey, UINT8* pucName, UINT8 *pucIsDup);
UINT32 sp5kDcfFsOptionSet(UINT32 uiOption, ...);
UINT32 sp5kDcfFsCustomSortModeSet(UINT32 filterMask);
UINT32 sp5kDcfFsCustomSortKeySet(UINT16 usDirKey, UINT16 usFileKey, UINT32 uiSortKey);
UINT32 sp5kDcfFsCustomSortKeySetByIdx(UINT32 uiStartIdx, UINT32 uiCount, UINT32 uiSortKey);
UINT32 sp5kDcfFsCustomSortKeySetByUserCallback(void* pReserve, sp5kDcfCustomSortKeySetCb_t callback);
UINT32 sp5kDcfFsCustomSortInfoGet(sp5kDcfCustomInfo_t** ppCustomInfo, UINT32 *pnum);
UINT32 sp5kDcfRecoverListGet(sp5kDcfRecoverFileList_t **pList);

UINT32 sp5kDcfPreInit(UINT32 maxNumItem);
UINT32 sp5kDcfAppCallbackCfg(sp5kDcfAppCbFuncSet_t* pDcfCbObj);

/* 20120516. Add for custome file manage. */
void sp5kDcfStdSysInit(void);
void sp5kDcfCusSysInit(void);

/**20130929 . Add for support the secondary file name/*/
UINT32 sp5kDcfFsSubFreeCharSet(UINT32 uiFreeCharNum,UINT8 ** uiFreeChar);
#endif /* SP5K_DCF_API_H__ */
