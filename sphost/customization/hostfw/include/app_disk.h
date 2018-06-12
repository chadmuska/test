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
#ifndef APP_DISK_H
#define APP_DISK_H

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

#define APP_DISK_DPOF_RSVD_SIZE_KB		512

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct appDiskInfo_s {
	UINT32 actDsk;	/* active disk : default 0 (not init yet) */
	UINT32 dskMbSz;	/* disk size : default 0 */
	UINT32 freeSz;	/* disk remain free size in k byte : default 0 */
	UINT32 cluSz;	/* disk cluster size : default 0*/
} appDiskInfo_t;

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
UINT32 appActiveDiskSet(UINT32 dskId);
UINT32 appActiveDiskGet(void);
UINT32 appDiskMountReady(void);
UINT32 appDiskMountFailed(void);
BOOL appDiskMountQuery(UINT32 dskId);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
/*Isaac add for pass SD card Mountd*/
UINT32 appDiskSDMountReady(void);
#endif
appDiskInfo_t* appCurDiskInfoGet(void) ;
void appDiskMount(UINT32 driver);
void appDiskUnMount(UINT32 driver);
void appDiskMountDirect( UINT32 driver );
UINT32 appDiskFormat(UINT32 driver);

UINT32 appDiskFreeClusterGet(UINT32 actDsk);
UINT32 appBytes2ClusterCalculate(UINT32 bytes);

appDiskCapability_t* appSdInfoGet(void);
UINT32 appCardSpeedGet(UINT32 actDsk);

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/

#define IS_DISK_MOUNT_READY     appDiskMountReady()
#define IS_DISK_MOUNT_FAIL      appDiskMountFailed()

#define IS_DISK_NAND_MOUNT	    appDiskMountQuery(SP5K_DRIVE_NAND)
#define IS_DISK_SD_MOUNT	    appDiskMountQuery(SP5K_DRIVE_SD)

#if USB_DISK_SUPPORT && !SP5K_SENSOR_DUAL_MODE
#define IS_DISK_USB_HOST_MOUNT  appDiskMountQuery(SP5K_DRIVE_USB)
#else
#define IS_DISK_USB_HOST_MOUNT 0
#endif
#endif  /* APP_DISK_H*/

