/**************************************************************************
 *
 *       Copyright (c) 2005-2013 by iCatch Technology, Inc.
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

/* DO NOT EDIT the next 2 lines for auto revision by subversion
$Rev: 30228 $
$Date:: 2018-05-11 15:51:53#$
 */

#ifndef _FS_DEF_H_
#define _FS_DEF_H_


#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
#include "common.h"

/**
 * @file fs_def.h
 * @brief Declaration of fs wrapper functions.
 */

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
/*
 * fsFileOpen(), definition here MUST manually sync to vfs_api.h.
 */
#if !defined(__linux__)
typedef enum {
	FS_OPEN_APPEND = (1 << 0),
	FS_OPEN_BINARY = (1 << 1),
	FS_OPEN_CREATE = (1 << 2),
	FS_OPEN_RDONLY = (1 << 4),
	FS_OPEN_RDWR   = (1 << 5),
	FS_OPEN_NOFIND = (1 << 11),
} fsOpenFlag_e;
#else
#include <fcntl.h>
typedef enum {
	FS_OPEN_APPEND = O_APPEND,
	FS_OPEN_BINARY = O_RDWR,
	FS_OPEN_CREATE = O_CREAT,
	FS_OPEN_RDONLY = O_RDONLY,
	FS_OPEN_RDWR   = O_RDWR,
	FS_OPEN_NOFIND = 0, /*not supported by linux*/
} fsOpenFlag_e;
#endif

/*
 * fsFileSeek(), definition here MUST manually sync to vfs_api.h.
 */
#if !defined(__linux__)
typedef enum {
	FS_SEEK_SET = 0,
	FS_SEEK_CUR = 1,
	FS_SEEK_END = 2,
} fsSeekMode_e;
#else
#include <linux/fs.h>
typedef enum {
	FS_SEEK_SET = SEEK_SET,
	FS_SEEK_CUR = SEEK_CUR,
	FS_SEEK_END = SEEK_END,
} fsSeekMode_e;
#endif
/*
 * fsFileAttrSet().
 */
typedef enum {
	FS_ATTR_READONLY = (1 << 0),
	FS_ATTR_HIDDEN   = (1 << 1),
	FS_ATTR_SYSTEM   = (1 << 2),
	FS_ATTR_LABEL    = (1 << 3),
	FS_ATTR_DIR      = (1 << 4),
	FS_ATTR_DIRTY    = (1 << 5),
} fsAttr_e;

/*
 * fsAliasNameGet().
 */
typedef enum {
	FS_STR_NATIVE  = 0,
	FS_STR_UNICODE = 1,
} fsCodeType_e;


/**************************************************************************
 *                           D A T A   T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
/*
 * fs_alias_name.c
 */
UINT8 *fsAliasNameGet(const UINT8 *fileName, UINT32 codeType);

/*
 * fs_dir_change.c
 */
UINT32 fsDirChange(const UINT8 *pathName);

/*
 * fs_dir_make.c
 */
UINT32 fsDirMake(const UINT8 *pathName);

/*
 * fs_dir_remove.c
 */
UINT32 fsDirRemove(const UINT8 *pathName);

/*
 * fs_file_attr_get.c
 */
UINT32 fsFileAttrGet(const UINT8 *fileName, UINT32 *pattr);

/*
 * fs_file_attr_set.c
 */
UINT32 fsFileAttrSet(const UINT8 *fileName, UINT32 attr);

/*
 * fs_file_close.c
 */
UINT32 fsFileClose(UINT32 fd);

/*
 * fs_file_delete.c
 */
UINT32 fsFileDelete(const UINT8 *fileName);

/*
 * fs_file_open.c
 */
UINT32 fsFileOpen(const UINT8 *fileName, SINT32 flag);

/*
 * fs_file_read.c
 */
UINT32 fsFileRead(UINT32 fd, UINT8 *buf, UINT32 size);

/*
 * fs_file_seek.c
 */
UINT32 fsFileSeek(UINT32 fd, SINT64 offset, UINT32 mode);
UINT32 fsFileSeekI64(UINT32 fd, SINT64 offset, UINT32 mode);
UINT32 fsFileQseek(UINT32 fd, SINT64 offset);
UINT32 fsFileQseekI64(UINT32 fd, SINT64 offset);
UINT32 fsFilePreLseek(UINT32 fd);

/*
 * fs_file_size_get.c
 */
UINT32 fsFileSizeGet(UINT32 fd);
UINT64 fsFileSizeGetI64(UINT32 fd);

/*
 * fs_file_tell.c
 */
UINT32 fsFileTell(UINT32 fd);
UINT64 fsFileTellI64(UINT32 fd);

/*
 * fs_file_write.c
 */
UINT32 fsFileWrite(UINT32 fd, UINT8 *buf, UINT32 size);

/*
 * fs_next_file.c
 */
UINT32 fsNextFileNameGet(UINT8 *fileName, UINT32 *nameLength);

/*
 * fs_fif.c
 */
UINT32 fsFifOpen(const char *fifname);
UINT32 fsFifOpenExt(const char *fifname, UINT32 fifsel);
UINT32 fsFifClose(UINT32 hFif);
UINT32 fsFifOpenCust(const char *fifname);
UINT32 fsFifCloseCust(UINT32 hFif);

void   fsFifDefSelInit(void);
UINT32 fsFifDefSelGet(void);

/*
 * fs_simple.c
 */
UINT32 fsSimpleWrite(UINT8 *fname, void *buf, UINT32 size);
UINT32 fsSimpleRead(UINT8 *fname, void *buf, UINT32 size);
UINT32 fsSimpleReadEx(UINT8 *fname, void**pbuf, UINT32 *psize);
UINT32 fsFileCompare(UINT8 *psrcfname, UINT8 *pdstfname);
UINT32 fsFileCopy(UINT8 *psrcfname, UINT8 *pdstfname);
UINT32 fsIsFileExists(UINT8 *fname);

/*
 * fs_fic.c
 */
#ifndef _FIC_HDR_T_
#define _FIC_HDR_T_
struct ficHdr_s;
typedef struct ficHdr_s ficHdr_t;
#endif /*#ifndef _FIC_HDR_T_*/

ficHdr_t *fsIsFicData(void *p);

/*
 * fs_direct_get.c
 */
void *fsFileDirectGet(const char *fname, void *pbuf, UINT32 buflmt,
		UINT32 *pfsize, UINT32 *phandle, UINT32 fifSel);

void fsFileDirectFree(UINT32 handle);

/*
 *fs_file_rename.c
 */
UINT32 fsFileRename(UINT8 *srcFile,UINT8 *dstFile);

/*
 *fs_volume_lable_set.c
 */
UINT32 fsFatVolLabSet(void *para0,void *para1);

/*
 *fs_bs_volume_lable_set.c
 */
UINT32 fsFatBSVolLabSet(void *para0,void *para1);
/*
 *fs_dirname_get.c
 */
UINT8 *fsGetCurrDirName(void);

/*
 *fs_cluster_size_get.c
 */
UINT32 fsGetClusterSize(void);

UINT32 fsGetDiskClusterSize(UINT8* diskPath);

/*
 *fs_file_cnt_get.c
 */
UINT32 fsDirFileCnt(UINT8 *dirPath);

/*
 *fs_dir_cache_set.c
 */
SINT32 fsDirCacheSet(UINT8 *pDirPath,UINT32 option);


#if 0

/*=============================================*/
UINT32 sp5kFsLongFileNameGet();

UINT8 *vfsLongFileNameGet(UINT8 *pname);

/*=============================================*/
UINT32 sp5kFsShortFileNameGet();

UINT8 *vfsAliasNameGet(UINT8 *pname, UINT32 uni);

/*=============================================*/
UINT32 sp5kFsFileCopy();

UINT32 vfsFileCopy(UINT8 *psrcFile, UINT8 *pdstFile);

#endif


#ifdef __cplusplus
}
#endif

#endif  /* _FS_DEF_H_ */
