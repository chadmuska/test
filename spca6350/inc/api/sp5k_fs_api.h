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
 *  Author: Matt Wang                                                     *
 *                                                                        *
 **************************************************************************/
#ifndef _SP5K_FS_API_H_
#define _SP5K_FS_API_H_

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
#include "api/sp5k_global_api.h" /*tmx_t*/
#include "middleware/fs_def.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
/*
 * sp5kFsFileOpen().
 */
typedef enum {
	SP5K_FS_OPEN_APPEND = FS_OPEN_APPEND,
	SP5K_FS_OPEN_CREATE = FS_OPEN_CREATE,
	SP5K_FS_OPEN_RDONLY = FS_OPEN_RDONLY,
	SP5K_FS_OPEN_RDWR   = FS_OPEN_RDWR,
	SP5K_FS_OPEN_NOFIND = FS_OPEN_NOFIND,
} sp5kFsOpenFlag_e;

/*
 * sp5kFsFileSeek().
 */
typedef enum {
	SP5K_FS_SEEK_SET = FS_SEEK_SET,
	SP5K_FS_SEEK_CUR = FS_SEEK_CUR,
	SP5K_FS_SEEK_END = FS_SEEK_END,
} sp5kFsSeekMode_e;

/*
 * sp5kFsFileAttrSet().
 */
typedef enum {
	SP5K_FS_ATTR_READONLY = FS_ATTR_READONLY,
	SP5K_FS_ATTR_HIDDEN   = FS_ATTR_HIDDEN,
	SP5K_FS_ATTR_SYSTEM   = FS_ATTR_SYSTEM,
	SP5K_FS_ATTR_LABEL    = FS_ATTR_LABEL,
	SP5K_FS_ATTR_DIR      = FS_ATTR_DIR,
	SP5K_FS_ATTR_DIRTY    = FS_ATTR_DIRTY,
} sp5kFsAttr_e;

/*
 * sp5kFsAliasNameGet().
 */
typedef enum {
	SP5K_FS_STR_NATIVE  = FS_STR_NATIVE,
	SP5K_FS_STR_UNICODE = FS_STR_UNICODE,
} sp5kFsCodeType_e;

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
 *				   E X T E R N A L	  R E F E R E N C E S				  *
**************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
/*
 * fs_alias_name.c
 */
#define sp5kFsAliasNameGet  fsAliasNameGet

/*
 * fs_dir_change.c
 */
#define sp5kFsDirChange  fsDirChange

/*
 * fs_dir_make.c
 */
#define sp5kFsDirMake  fsDirMake

/*
 * fs_dir_remove.c
 */
#define sp5kFsDirRemove  fsDirRemove

/*
 * fs_file_attr_get.c
 */
#define sp5kFsFileAttrGet  fsFileAttrGet

/*
 * fs_file_attr_set.c
 */
#define sp5kFsFileAttrSet  fsFileAttrSet

/*
 * fs_file_close.c
 */
#define sp5kFsFileClose  fsFileClose

/*
 * fs_file_delete.c
 */
#define sp5kFsFileDelete  fsFileDelete

/*
 * fs_file_open.c
 */
#define sp5kFsFileOpen  fsFileOpen

/*
 * fs_file_read.c
 */
#define sp5kFsFileRead  fsFileRead

/*
 * fs_file_seek.c
 */
#define sp5kFsFileSeek  fsFileSeek
#define sp5kFsFileSeekI64  fsFileSeekI64

/*
 * fs_file_size_get.c
 */
#define sp5kFsFileSizeGet  fsFileSizeGet
#define sp5kFsFileSizeGetI64  fsFileSizeGetI64

/*
 * fs_file_tell.c
 */
#define sp5kFsFileTell  fsFileTell
#define sp5kFsFileTellI64  fsFileTellI64

/*
 * fs_file_write.c
 */
#define sp5kFsFileWrite  fsFileWrite

/*
 * fs_next_file.c
 */
#define sp5kFsNextFileNameGet  fsNextFileNameGet

/*
 * fat_vollabel.c
 * volume label for PBR & Root DIR
 */
#define sp5kFsFatVolumeLabelSet fsFatVolLabSet /*(void * active storage root path, void * vollabel);*/
#define sp5kFatVolumeLabelSet	sp5kFsFatVolumeLabelSet /* obsolete, for code compatibility only */

/*
 * fat_vollabel.c
 * volume label for PBR only
 * Under windows platform, volume label only show "Removable disk" in MSDC mode.
 */
#define sp5kFsFatBSVolumeLabelSet  fsFatBSVolLabSet /*(void * active storage root path, void * vollabel);*/
#define sp5kFsFatBSVolLabSet  sp5kFsFatBSVolumeLabelSet /*(void * active storage root path, void * vollabel);*/
#define sp5kFatBSVolLabSet  sp5kFsFatBSVolumeLabelSet /* obsolete, for code compatibility only */

/*
 * vfsDirFileCnt.c
 * Get the file count of targer folder
 */
#define sp5kFsDirFileCntGet fsDirFileCnt   /* This is the right naming for fsDirFileCnt, sp5kDirFileCntGet will be abandoned in future. */
#define sp5kDirFileCntGet   sp5kFsDirFileCntGet /* obsolete, for code compatibility only */

#define sp5kFsFileCompare  fsFileCompare

#define sp5kFsFileCopy  fsFileCopy

#define sp5kFsClusterSizeGet  fsGetClusterSize

#define sp5kFsDiskClusterSizeGet fsGetDiskClusterSize
/*
 * vfs_api.c
 * vfsGetCurrDirName: get current working directory path
 * Caller should not modify the returned path string
*/
#define sp5kFsGetCurPath fsGetCurrDirName

/*
 * vfs_api.c
 * vfsRename: change the name or location of a file or dir
 * the path of src and dest could be a file or dir
*/
#define sp5kFsRename fsFileRename

/*
 * vfs_api.c
 * vfsDirCacheSet: cache the specified directory data to memory for enjoy better performance
 * Caller MUST Clear the data cache in time. sp5kFsDirCacheSet(path, 1) and sp5kFsDirCacheSet(path, 0) aways in pairs.
*/
#define sp5kFsDirCacheSet fsDirCacheSet

/*
 * vfs_api.c
 * vfsFileAllTimeGet: get the file's access time /modify time/status change time
*/
UINT32 sp5kFsFileTimeGet(UINT8 *pname,
			tmx_t *atime, tmx_t *mtime, tmx_t *ctime);

/*
 * vfs_api.c
 * vfsFileAllTimeSet: set the file's access time /modify time/status change time
*/
UINT32 sp5kFsFileTimeSet(UINT8 *pname,
		const tmx_t *atime, const tmx_t *mtime, const tmx_t *ctime);

/*
 * fs_fif.c
 * fsFifOpenCust: load FIF into custom heap
 * fsFifCloseCust: unload FIF and release the memory used
*/
#define sp5kFsFifLoad  fsFifOpenCust
#define sp5kFsFifUnload  fsFifCloseCust


/*
 * fs_dir_nextfile_get.c
 * fsDirNextFileGet: get a directory item in current working folder.
*/
UINT32
sp5kFsDirNextFileGet(
	char *pname, /* file name */
	UINT32 *plen, /* file name buffer length */
	UINT32 *pattr, /* file attribute */
	UINT64 *psize, /* file size */
	tmx_t  *ptmx /* file time. decide by DISK option, 0 for ctime, 1 for mtime, 2 for minimum of ctime/mtime */
	);

#if defined(__linux__)
/*
	for linux case.
	To replace disk symbol(A:\, B:\, D:\) by file system path
*/

#define sp5kFsPathAliasSet vfsPathAliasSet
#endif

#endif  /* _SP5K_FS_API_H_ */

