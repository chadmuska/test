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
#ifndef _APP_CDFS_H_
#define _APP_CDFS_H_

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
#include "api/sp5k_capture_api.h"
#include "api/sp5k_dcf_api.h"
/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct CDFS_FILE_TAB_S
{
	UINT8 FileName[32];
	UINT32 attr;                        /*read from fdb*/
	UINT32 dcfattr;                     /*for host*/
	UINT64 FileSize;
	UINT32 FileType;
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	UINT32 folder;
	UINT32 event;
	int    index;
	char mtime[32];
#endif
	struct CDFS_FILE_TAB_S *next;	    /*last->next = NULL*/
	struct CDFS_FILE_TAB_S *pre;	    /*first->pre = last*/
}CDFS_FILE_TAB_T;

typedef struct CDFS_FOLDER_INFO_S
{
	UINT32 file_num;
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	UINT32 file_num_in_subfolder[999];
#endif
	UINT32 cur_file_idx;
}CDFS_FOLDER_INFO_T;

typedef struct CDFS_FOLDER_LIST_S
{
	struct CDFS_FILE_TAB_S *head_node;
	struct CDFS_FOLDER_INFO_S folder_info;
}CDFS_FOLDER_LIST_T;

enum FILE_FORMAT
{
	CDFS_FILE_FOLDER_VIDEO,
	CDFS_FILE_FOLDER_JPG,
	CDFS_FILE_FOLDER_EVENT,
#if defined(PRJ_JETDVR)  ||defined(PRJ_17DVR)||defined(PRJ_19DVR)
	CDFS_FILE_FOLDER_USERRECORD,
	CDFS_FILE_FOLDER_USERSAVE,
#endif
	CDFS_FILE_FOLDER_FAIL,
	CDFS_FILE_FOLDER_MAX = CDFS_FILE_FOLDER_FAIL,
#if defined(PRJ_JETDVR) ||defined(PRJ_17DVR)||defined(PRJ_19DVR)
	CDFS_FILE_FOLDER_ANNIV,
#endif
};


#define CDFS_FILE_FOLDER_VIDEO_BIT   (1 << CDFS_FILE_FOLDER_VIDEO)
#define CDFS_FILE_FOLDER_JPG_BIT     (1 << CDFS_FILE_FOLDER_JPG)
#define CDFS_FILE_FOLDER_EVENT_BIT   (1 << CDFS_FILE_FOLDER_EVENT)
#if defined(PRJ_JETDVR)||defined(PRJ_19DVR)||defined(PRJ_17DVR)
#define CDFS_FILE_FOLDER_USERRECORD_BIT (1 << CDFS_FILE_FOLDER_USERRECORD)
#define CDFS_FILE_FOLDER_USERSAVE_BIT   (1 << CDFS_FILE_FOLDER_USERSAVE)
#endif
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
#define FOLDER_MASK	(0b11111)
#endif
enum EVENT_WARN
{
	EVENT_WARN_STAGE_ONE = 1,
	EVENT_WARN_STAGE_TWO = 2,
};

 enum DEL_METHOD
{
	CDFS_PROTECTED_SKIP,
	CDFS_PROTECTED_DEL,
};

 enum CDFS_EVENT
{
	JPG_EVENT_IN_VIDEO	    = 1<<0,
	VIDEO_EVENT_IN_VIDEO	= 1<<1,
};

#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
typedef BOOL (*cdfs_compare)(CDFS_FILE_TAB_T *pCur, CDFS_FILE_TAB_T *pCondition);
/*paul modify BOOL -> void for gcc warning*/
typedef void (*cdfs_node_update)(CDFS_FILE_TAB_T *pCur);
#endif

typedef enum {
	APP_CDFS_ADD_FILE_CB,
	APP_CDFS_DELETE_FILE_CB,
} appCdfsCbType_t;

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
UINT32 appCdfsFolderInit(UINT32 folder);
UINT32 appCdfsCurrFolderFileListCreate(UINT32 folder,UINT8* dirPath);
UINT32 appCdfsFileAdd(UINT8 *filename, UINT32 *phandle);
UINT32 appCdfsFileSync(UINT8 *filename, UINT32 handle);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
BOOL appCdfsFileDel(CDFS_FILE_TAB_T *pCur);
#else
UINT32 appCdfsFileDel(UINT8 *filename);
#endif
UINT32 appCdfsNextFileNameGet(UINT32 filetype,UINT8 *pFileName);
UINT32 appCdfsFileFolderSet(UINT32 folder);
UINT32 appCdfsFileFolderGet(void);
UINT32 appCdfsCurFileIdxSet(UINT32 fileIdx);
UINT32 appCdfsCurFileIdxGet(UINT32 *fileIdx);
UINT32 appCdfsFileNumGet(UINT32 *filenum);
UINT32 appCdfsFileAttrGet(UINT32 fileIdx, sp5kDcfAttrElm_t* pfileInfoAtr, UINT8* pname);
UINT32 appCdfsInfoGet(UINT32 option, UINT32* pValue);
UINT32 appCdfsFileOperate(UINT32 option, UINT32 param, UINT32 async, fpsp5kCallback_t pcallback, UINT32 length);
UINT64 appCdfsCanDelVideoSizeGet(void);
UINT32 appCdfsTheOldestProtectedFileDel(void);
UINT32 appCdfsEventFolderSizeCheck(void);
UINT32 appCdfsFreeSpaceCheck(UINT32 folder);
UINT32 appCdfsEventFlagGet(void);
UINT32 appCdfsFileNameValidCheck(UINT32 filetype);
UINT32 appCdfsFileNameToFileType(UINT8 *filename);
UINT32 appCdfsFileListTabInit(UINT32 folder);
UINT32 appCdfsEventProcess(UINT8 *filename,UINT32 event);
UINT32 appCdfsFolder2Path(UINT32 folder,UINT8 *path);
UINT32 appCdfsAddFileStsGet(void);
UINT32 appCdfsAbsolutePathToFile(UINT8 *fullpath, UINT8 *filename);
void appCdfsFileListPrintf(UINT32 folder);
void appCdfsEventFlagSet(UINT32 flag);
void appCdfsEventFlagClean(UINT32 flag);
UINT8 * appCdfsLastFileNameGet(void);

#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
void appCdfsLastFileNameSet(UINT8 *fname);/*sc added*/
#endif

UINT8 * appCdfsLastAviNameGet(void);
CDFS_FILE_TAB_T *appCdfsFileListHeadGet(UINT32 folder);
#if !JET_PANA_CDFS
CDFS_FILE_TAB_T * appCdfsFileHandleGet(UINT32 idx);
#endif
UINT32 appCdfsTheOldestFileDel(void);
UINT32 appCdfsFileNameExistCheck(UINT8 *pCheckFileName);
void appCdfsListInfoDump(CDFS_FILE_TAB_T *pHead,CDFS_FILE_TAB_T *pLast);

/*dual cdfs flow*/
#if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
UINT32 appCdfsNextSubFileNameGet(UINT32 fileType,UINT32 subFileType,UINT8 *pFileName);
#endif

#if defined(PRJ_17DVR)||defined(PRJ_19DVR)

CDFS_FILE_TAB_T *appCdfsNextNodeFind(CDFS_FILE_TAB_T *pCur, CDFS_FILE_TAB_T *pCond, cdfs_compare cmp);
CDFS_FILE_TAB_T *appCdfsPrevNodeFind(CDFS_FILE_TAB_T *pCur, CDFS_FILE_TAB_T *pCond, cdfs_compare cmp);
CDFS_FILE_TAB_T *appCdfsFolderNodeFind(UINT32 folder, CDFS_FILE_TAB_T *pCur);
CDFS_FILE_TAB_T *appCdfsTableHeadGet();
UINT32 appCdfsFolderCount(UINT32 folder);
void appCdfsRemoveNode(CDFS_FILE_TAB_T *pCur);
UINT64 appCdfsUsedSpace(UINT32 folder);
UINT32 appCdfsBiggestNmlGet();

void appCdfsNodeUpdateCBSet(cdfs_node_update cb);

UINT32 jet_appCdfsCurrFolderFileListAdd(UINT32 folder,UINT8* dirPath);
void appCdfsFolderNodeCountRemove(CDFS_FILE_TAB_T *pCur);
void appCdfsFolderNodeCountAdd(CDFS_FILE_TAB_T *pCur);
#endif
UINT32 appCdfsNextFileNameByChGet(UINT8 *channelName, UINT32 filetype,UINT8 *pFileName);
void appCdfsCbReg(UINT32 sel, void *pfunc);

typedef UINT32 (*burstTimeGetCb)(tmx_t *burstFileTime, UINT32 bidx);
void appBurstTimeCbReg(void *pfunc, UINT32 num);


#endif  /* _APP_CDFS_H_ */

