/**************************************************************************
 *
 *       Copyright (c) 2004-2016 by iCatch Technology, Inc.
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
#ifndef _SP5K_DISK_FSM_H_
#define _SP5K_DISK_FSM_H_

#include "common.h"


/**
*  Data type define.
*  Must keep the same with definition in type.h
*/

#define SP5K_POSIX_API_BIT 64

#if  (SP5K_POSIX_API_BIT == 64)
	typedef signed long long psssize_t ;
	typedef unsigned  long long psusize_t ;
#else
	typedef signed long  psssize_t ;
	typedef unsigned  long  psusize_t ;
#endif

#define S32 SINT32
#define S64 SINT64
#define U08 UINT8
#define U16 UINT16
#define U32 UINT32

#define FAT_MAX_LONG_NAME_LEN 512
#define FAT_MAX_SHORTNAME_LEN 13

#define DRIVE_NAND	1
#define DRIVE_SD	2
#define DRIVE_OTG	3
#define DRIVE_SDIO	4
#define DRIVE_MS	5
#define DRIVE_RAM	6

/* merit define for mount */
#define MERIT_PREFERRED (0x800000)
#define MERIT_NORMAL (0x600000)
#define MERIT_UNLIKELY (0x400000)
#define MERIT_DO_NOT_USE (0x200000)
#define MERT_UNDEFINED (0x0)

/**
*  Directory info structure.
*  Must keep the same with definition in posix_fs.h
*/
typedef struct diskInfo_s {
	unsigned int uiFreeClus;
	unsigned int uiTotalSector;
	unsigned int uiTotalDataClus;
	unsigned int uiBytesPerClus;
	unsigned int uiBytesPerSector;
	unsigned int uiFreeSector;
	unsigned char ucFsType;
	unsigned char pucDevName[32];
} diskInfo_t;

struct dirent {
	char d_name[FAT_MAX_LONG_NAME_LEN];
	char d_shortname[FAT_MAX_LONG_NAME_LEN];
	unsigned int d_ino;
	int d_off;
	int d_size;
	unsigned int d_reclen;
	unsigned int d_type;
	int d_openid;
};

struct sp5k_file_stat {
	unsigned int  st_mode;     /* File mode */
	unsigned int   st_ino;      /* File serial number */
	unsigned int   st_dev;      /* ID of device containing file */
	unsigned int st_nlink;    /* Number of hard links */
	unsigned int   st_uid;      /* User ID of the file owner */
	unsigned int   st_gid;      /* Group ID of the file's group */
	psusize_t st_size;     /* File size (regular files only) */
	unsigned int  st_atime;    /* Last access time */
	unsigned int  st_mtime;    /* Last data modification time */
	unsigned int  st_ctime;    /* Last file status change time */
	unsigned int st_attr;
};

struct dirent_Ex {
	char d_name[FAT_MAX_LONG_NAME_LEN];
	char d_shortname[FAT_MAX_LONG_NAME_LEN];
	int d_off;
	int d_openid;
	struct sp5k_file_stat stStat;
};


/**
*  function set define for subfs.
*  must keep the same with FsInterfaceSet_S defined in posix_resourc.h
*/
typedef struct TagSp5kFsInterfaceSet {
	char          name[10];          /* filesystem name*/
	unsigned int  merit;            /* filesystem merit for use when mount */
	int           driveMask;           /* drive banding bitmap. Default is 0x03ffffff(A:bit0~Z:bit25). */
	int (*pfsp5kopendir)(S32 iMountID, char *dirname);


	int (*pfsp5kreaddir_r)(S32 iMountID, S32 dirp,
					   struct dirent *entry);

	int (*pfsp5kreaddirEx)(S32 iMountID, S32  dirp, struct dirent_Ex *entry);

	/*!\brief close a directory
	 * \param DIR* dirp [in] DIR pointer
	 * \return returns 0 on success or -1 on failure */
	int (*pfsp5kclosedir)(S32 iMountID, S32  dirp);

	/*!\brief set the position of the next readdir()
	 *	      call in the directory stream.
	 * \param DIR* dirp [in] DIR pointer
	 * \param off_t pos [in] offset
	 * \return returns no value */
	S32 (*pfseekdir)(S32 iMountID, S32  dirp, psssize_t pos);

	/*!\brief change working directory
	 * \param char *path [in] path string
	 * \return On success, zero is returned.  On error, -1 is returned */
	int (*pfsp5kchdir)(S32 iMountID, char *path, int fd);

	/*!\brief deletes a directory, which must be empty
	 * \param char *path [in] path string
	 * \return On success, zero is returned.  On error, -1 is returned */
	int (*pfsp5krmdir)(S32 iMountID, char *path);

	/*!\brief create a directory
	 * \param char *path [in] path string
	 * \return On success, zero is returned.  On error, -1 is returned */
	int (*pfsp5kmkdir)(S32 iMountID, char *pathname);

	char* (*pfsp5kgetcwd)(S32 iMountID, char *buf, int size);

	/*!\brief open and possibly create a file
	 * \param char *pathname [in] pathname
	 * \param int flags [in] open flag,is  one  of  O_RDONLY,
	 *        O_WRONLY or O_RDWR which request opening the
	 *		  file read-only, write-only or  read/write
	 * \return return  the  new  file  descriptor, or -1 if an error */
	int (*pfsp5kopen)(S32 iMountID, char *pathname, int flags);

	/*!\brief get file status
	 * \param char *file_name [in] file name
	 * \param struct sp5k_file_stat *buf [out] file status buffer
	 * \return On success, zero is returned.  On error, -1 is returned */
	int (*pfsp5kstat)(S32 iMountID, char *file_name, struct sp5k_file_stat *buf);

	/*!\brief get file status
	 * \param int fd [in] file identification
	 * \param struct sp5k_file_stat *buf [out] file status buffer
	 * \return On success, zero is returned.  On error, -1 is returned */
	int (*pfsp5kfstat)(S32 iMountID, int fd, struct sp5k_file_stat *buf);

	int (*pfsp5kchmod)(S32 iMountID, char* file_name, unsigned int mode);

	/*!\brief close the open file
	 * \param int fd [in] file identification
	 * \return On success, zero is returned.  On error, -1 is returned */
	int (*pfsp5kclose)(S32 iMountID, int fd);

	/*!\brief read from a file descriptor
	 * \param int fd [in] file identification
	 * \param void* buf [out] data buffer
	 * \param size_t nbytes [in] read data length by bytes
	 * \return On success, the number of bytes read is returned
	 *         (zero indicates end of file),On error,-1 is returned */
	psssize_t (*pfsp5kread)(S32 iMountID, int fd, void* buf, psusize_t nbytes);

	/*!\brief write to a file descriptor
	 * \param int fd [in] file identification
	 * \param void* buf [in] data buffer
	 * \param size_t nbytes [in] read data length by bytes
	 * \return On  success,  the  number of bytes written are
	 *         returned (zero indicates nothing was written).  On error,
	 *	       -1  is  returned, */
	psssize_t (*pfsp5kwrite)(S32 iMountID, int fd, void* buf, psusize_t nbytes);

	/*!\brief reposition read/write file offset
	 * \param int fd [in] file identification
	 * \param off_t offset [in] seek offset
	 * \param int whence [in] is one of SEEK_SET.SEEK_CUR and SEEK_END
	 * \return On  success,  returns the resulting offset location
	 *         as measured in bytes from the beginning  of  the  file.
	 *		   On error, -1  is  returned, */
	psusize_t (*pfsp5klseek)(S32 iMountID, int fd, psusize_t  offset, int whence);

	/*!\brief delete a name and possibly the file it refers to
	 * \param char *path [in]
	 * \return On  success,  zero is returned.  On error, -1 is returned */
	int (*pfsp5kunlink)(S32 iMountID, char *path);

	/*!\brief mount device
	 * \param char* devname [in] device name
	 * \param char* dir [in]
	 * \param char* fsnamt [in] file system type name
	 * \return return device id, or -1 if an error occurred */
	int (*pfsp5kmount)(char* pucDevName);

	int (*pfsp5kformat)(char *pucDevName,char *fsType);

	int (*pfsp5kgetdiskinfo)(S32 iMountID, diskInfo_t* pstdevInfo);

	/*!\brief duplicate a file descriptor
	 * \param char* devname [in] device name
	 * \return return device id, or -1 if an error occurred */
	int (*pfsp5kumount)(S32 iMountID);

	int (*pfsp5kConcatefile)(S32 iMountID, int uiDstHandle, int uiSrcHandle);/*moddfy by sunbo 2006/3/22*/

	S64 (*pfsp5ktell)(S32 iMountID, int fd);/*moddfy by sunbo 2006/3/22*/

	/*!\brief change the name or location of a file
	 * \param char *oldpath [in]
	 * \param char *newpath [in]
	 * \return On  success,  zero is returned.  On error, -1 is returned */
	int (*pfsp5krename)(S32 iMountID, char *oldpath, char *newpath);

	/*!\brief get file info
	 * \param int fd [in] file identification
	 * \param struct dirent_Ex *buf [out] file info buffer
	 * \return On success, zero is returned.  On error, -1 is returned */
	int (*pfsp5kfinfo)(S32 iMountID, int fd, struct dirent_Ex *entry);
	int (*pfsp5kfileinfo)(S32 iMountID, char *file_name, struct dirent_Ex *entry);

	int (*pfsp5kfcntl)(S32 iMountID, int fd, int cmd, long long arg);

	int (*pfsp5kdel)(S32 iMountID, int fd);

	int (*pfsp5kflush)(S32 iMountID, int fd);

	int (*pfsp5ksetlabel)(S32 iMountID, unsigned char* pucDiskLabelBuf,
					  int iDiskLabelLen, unsigned char ucIsGetLabel);

	int (*pfsp5kdircacheset)(unsigned char *dirPath, unsigned int opt); /* add by kinreven */

	/*eric@20101129 for attr*/
	int (*pfsp5kinfoset)(int fd, struct dirent_Ex *); /* add by eric.su@20101129 */

	/*dw@20120814 for info get with no lock */
	int (*pfsp5kinfoget)(S32 iMountID, U32 uInfoId, void *iInfoValue); /* add by dw@20120814 */

	/*file recovery*/
	int (*pfsp5kfileRecover)(U08 * PathName);
	struct TagFsInterfaceSet *pstNext;
} Sp5kFsInterfaceSet_S;



/**
*  storage function set define.
*  must keep the same with FatDriverFun_S defined in posix_fs.h
*/
typedef struct tagSp5kFatDriverFun {
	/*!\brief get the disk total sector number 512Bytes per sector
	 * \param uiDeviceID [in] device ID
	 * \param uiTotalSector [out] totoal sector number
	 * \return 1 for success,-1 for failure */
	long (*pfDriverDiskSectorCount_C)(unsigned long uiDeviceID,
									 unsigned long* puiRetTotalSector);

	/*!\brief get the physical partition count
	 * \param uiDeviceID [in] device ID
	 * \param uiPartitionCount [out] total partition count
	 * \return 1 for success,-1 for failure */
	long (*pfDriverGetPartitCount_C)(unsigned long uiDeviceID,
									unsigned long* puiPartitionCount);
	/*!\brief get the space size of physical partition by sector, 512Bytes per sector
	 * \param uiDeviceID [in] device ID
	 * \param uiPartitonNo [in] partition number(Base 0)
	 * \param uiStartSector [out] start number for physical partition(Base 0)
	 * \param uiSectorCount [out] total sector number for the partition
	 * \return 1 for success,-1 for failure*/
	long (*pfDriverGetPartitionSize_C)(unsigned long uiDeviceID,
									  unsigned long  uiPartitonNo,
									  unsigned long* puiStartSector,
									  unsigned long* puiSectorCount);

	/*!\brief wirte data,data length caculated by sector,512Bytes per sector
	 * \param uiDeviceID [in] device ID
	 * \param uiStartSector [in] start sector number for data writing in(Base 0)
	 * \param uiSectorNum [in] total sector number for writeing
	 * \param pucBuffer [in] data buffer for writing
	 * \return 1 for success, -1 for failure*/
	long (*pfDriverWriteSector_C)(unsigned long uiDeviceID,
								 unsigned long  uiStartSector,
								 unsigned long  uiSectorNum,
								 unsigned char* pucBuffer);

	/*!\brief read data,,data length caculated by sector,512Bytes per sector
	 * \param uiDeviceID [in] device ID
	 * \param uiStartSector [in] start sector number for data reading in(Base 0)
	 * \param uiSectorNum [in]total sector number for reading
	 * \param pucBuffer [in] data buffer for reading
	 * \return 1 for success, -1 for failure*/
	long (*pfDriverReadSector_C)(unsigned long uiDeviceID,
								unsigned long  uiStartSector,
								unsigned long  uiSectorNum,
								unsigned char* pucBuffer);

	/*!\brief open device
	 * \param pucDeviceName [in] start address of the specified disk device
	 * \param uiDeviceNameLen [in] device name length
	 * \param puiRetDevicdID [out] the opened device ID
	 * \return 1 for success, -1 for failure*/
	long (*pfDriverOpenDevice_C)(unsigned char* pucDeviceName,
								unsigned long  uiDeviceNameLen,
								unsigned long* puiRetDevicdID);

	/*!\brief close device
  * \param uiDeviceID [in] device ID
  * \return 1 for success, -1 for failure */
	long (*pfDriverSynchDevice_C)(unsigned long uiDeviceID);

	/*!\brief close device
	 * \param uiDeviceID [in] device ID
	 * \return 1 for success, -1 for failure */
	long (*pfDriverCloseDevice_C)(unsigned long uiDeviceID);

}SP5kFatDriverFun_S;

/**
*   API interface define
*/

/**
*	This function let user register they sub-filesystem implement into posix.
*/
UINT32 sp5kDiskFsRegister(
	Sp5kFsInterfaceSet_S * pFsInterface
);

/**
*	This function change merit of sub-filesystem, so that they will mount in different order
*/
UINT32 sp5kDiskFsChgMerit(
	UINT8 * pucName,
	UINT32  merit
);

/**
*	This function get special storage driver's function interface, so that host can r/w storage directly.
*/
SP5kFatDriverFun_S* sp5kDiskFsStorageGet(
	UINT8* pucName
);


#endif	/* _SP5K_DISK_FSM_H_ */

