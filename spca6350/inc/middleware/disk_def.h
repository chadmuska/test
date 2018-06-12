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
#ifndef __DISK_DEF_H__
#define __DISK_DEF_H__

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef enum {
	DISK_MOUNT_STATUS_REMOVE	= 0,
	DISK_MOUNT_STATUS_INSERT	= 1,
	DISK_MOUNT_STATUS_START		= 2,
	DISK_MOUNT_STATUS_COMPLETE	= 3,
	DISK_MOUNT_STATUS_FAIL		= 4,
} diskMountStatus_t;

typedef void (*pfDiskPreCb_t)(void);

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
#define DRIVE_NAND  1
#define DRIVE_SD	2
#define DRIVE_OTG	3
#define DRIVE_SDIO	4
#define DRIVE_MS	5
#define DRIVE_RAM	6
#define DRIVE_USBHOST	11
#define DRIVE_FILE	12
#define DRIVE_FILE1 13

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

#endif  /* __DISK_DEF_H__ */

