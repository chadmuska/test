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
#ifndef _SP5K_DISK_API_H_
#define _SP5K_DISK_API_H_

#include "common.h"
#include "middleware/disk_def.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef UINT32 (*pfDir_t)(UINT32 ,UINT32, UINT32);
typedef UINT32 (*pfWr_t)(UINT32 ,UINT32, UINT32);
typedef UINT32 (*pfRd_t)(UINT32 ,UINT32, UINT32 *);
typedef UINT32 (*pfpwr_t)(void);

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
typedef enum {
	SP5K_DRIVE_RSVA    = 0,
	SP5K_DRIVE_NAND    = 1,
	SP5K_DRIVE_SD      = 2,
	SP5K_DRIVE_SMC     = 3,
	SP5K_DRIVE_XD      = 3,
	SP5K_DRIVE_OTG     = 3,
	SP5K_DRIVE_SDIO    = 4,
	SP5K_DRIVE_CF      = 4,
	SP5K_DRIVE_MS      = 5,
	SP5K_DRIVE_RAM     = 6,
	SP5K_DRIVE_RSVB    = 7,
	SP5K_DRIVE_USB     = 11,
	SP5K_DRIVE_FILE    = 12,
	SP5K_DRIVE_FILE1   = 13,
	SP5K_DRIVE_CDROM   = 0x0f,
	SP5K_START_DETECT  = 0xffff,
	SP5K_DRIVE_REMOVE  = 0x10000,
} sp5kDriveId_t;

typedef enum {
	SP5K_DISK_DETECT_PIN             = 0,
	SP5K_DISK_POWER_ON_PIN           = 1,
	SP5K_DISK_BUF_ENABLE_PIN         = 2,
	SP5K_DISK_WRITE_PROTECT_PIN      = 3,
	SP5K_DISK_SD18V_CTRL_PIN         = 4,
	SP5K_DISK_NAND_SPI_WP_CTRL_PIN   = 5,
	SP5K_DISK_PIN_MAX
} sp5kiskFunctionPin_t;

typedef enum {
	SP5K_DISK_STATE               = 0,
	SP5K_DISK_SIZE_BY_MEGA_BYTE   = 1,
	SP5K_DISK_FREE_SIZE_BY_BYTES  = 2,
	SP5K_DISK_CLUSTER_SIZE_BY_BYTES = 3,
	SP5K_DISK_WRITE_PROTECT_QUERY = 4,
	SP5K_DISK_TYPE_QUERY          = 5,
	SP5K_DISK_CLOCK_QUERY         = 6,
	SP5K_DISK_BUS_QUERY           = 7,
	SP5K_DISK_FREE_SIZE_BY_KBYTE  = 8,
	SP5K_DISK_CLASS_SPEED_QUERY   = 9,
	SP5K_DISK_FS_TYPE_INFO        = 10,
	SP5K_DISK_TOTAL_SECTOR        = 11,
	SP5K_DISK_ECC_FAIL_DETECTED   = 12,
	SP5K_DISK_HEALTH_QUERY        = 13,
	SP5K_DISK_VIDEO_SPEED_QUERY   = 14,
} sp5kDiskInfo_t;

/* for code compatibility */
#define SP5K_SIZE_BY_MEGA_BYTE		SP5K_DISK_SIZE_BY_MEGA_BYTE
#define SP5K_FREE_SIZE_BY_BYTES		SP5K_DISK_FREE_SIZE_BY_BYTES
#define SP5K_CLUSTER_SIZE_BY_BYTES	SP5K_DISK_CLUSTER_SIZE_BY_BYTES
#define SP5K_FREE_SIZE_BY_KBYTE		SP5K_DISK_FREE_SIZE_BY_KBYTE
#define SP5K_DISK_WRITE_PROTECT_QUERRY	SP5K_DISK_WRITE_PROTECT_QUERY

typedef enum {
	SP5K_DISK_IS_NOT_WRITE_PROTECT   = 0,
	SP5K_DISK_IS_UNDER_WRITE_PROTECT = 1,
} sp5kDiskProtectInfo_t;

typedef enum {
	SP5K_SD_TYPE                  = 1,
	SP5K_MMC_TYPE                 = 2,
} sp5kSdTypeCap_t;

typedef enum {
	SP5K_1_BIT_BUS                = 1,
	SP5K_4_BIT_BUS                = 2,
	SP5K_8_BIT_BUS                = 3,
} sp5kSdBusCap_t;

typedef enum {
	SP5K_CLOCK_375KHz             = 1,
	SP5K_CLOCK_6MHz               = 2,
	SP5K_CLOCK_12MHz              = 3,
	SP5K_CLOCK_24MHz              = 4,
	SP5K_CLOCK_48MHz              = 5,
	SP5K_CLOCK_96MHz              = 6,
} sp5kSdClockCap_t;

/* for SP5K_DISK_ID_CALLBACK_CFG parameter info */
/* SD */
#define SD_INFO_TYPE_MASK         0x000000FF
#define SD_INFO_TYPE_LSB          0
typedef enum {
	SD_TYPE_UNKNOWN               = 0,
	SD_TYPE_V11                   = 1,
	SD_TYPE_V20                   = 2,
	SD_TYPE_SDHC                  = 3,
	SD_TYPE_SDXC                  = 4,
	SD_TYPE_MMC                   = 5,
	SD_TYPE_MAX
} SdTypeCap_t;

#define SD_INFO_MODE_MASK         0x0000FF00
#define SD_INFO_MODE_LSB          8
typedef enum {
	SD_33V_DS                     = 1,
	SD_33V_HS                     = 2,
	SD_18V_SDR12                  = 3,
	SD_18V_SDR25                  = 4,
	SD_18V_SDR50                  = 5,
	SD_18V_SDR104                 = 6,
	SD_18V_DDR50                  = 7,
} SdModeCap_t;

#define SD_INFO_RESERVED_MASK     0xFFFF0000
#define SD_INFO_RESERVED_LSB      16

/* SPI */
#define SPI_CFG_CMD_LSB         0
#define SPI_CFG_CMD_MASK        0x000000FF
#define SPI_CFG_DATA_LSB        8
#define SPI_CFG_DATA_MASK       0x00FFFF00
#define SPI_CFG_DUMMY_LSB       24
#define SPI_CFG_DUMMY_MASK      0x03000000
#define SPI_CFG_SIZE_LSB        26
#define SPI_CFG_SIZE_MASK       0x0C000000
#define SPI_CFG_NOSTR_LSB       28
#define SPI_CFG_NOSTR_MASK      0x10000000
#define SPI_CFG_NOEND_LSB       29
#define SPI_CFG_NOEND_MASK      0x20000000
#define SPI_CFG_WRITE_LSB       30
#define SPI_CFG_WRITE_MASK      0x40000000
#define SPI_CFG_WAIT_LSB        31
#define SPI_CFG_WAIT_MASK       0x80000000

/* Memory Stick */
#define MS_INFO_TYPE_MASK         0x000000FF
#define MS_INFO_TYPE_LSB          0
typedef enum {
	MS_TYPE_RW                    = 0,
	MS_TYPE_ROM                   = 1,
	MS_TYPE_ROM2                  = 2,
	MS_TYPE_ROM3                  = 3,
	MS_TYPE_MAX
} MsTypeCap_t;

#define MS_INFO_SUBCLASS_MASK     0x0000FF00
#define MS_INFO_SUBCLASS_LSB      8

#define MS_INFO_SPEEDTYPE_MASK    0x00000700
#define MS_INFO_SPEEDTYPE_LSB     8
typedef enum {
	MS_SPEEDTYPE_LESS15           = 1,
	MS_SPEEDTYPE_15               = 2,
	MS_SPEEDTYPE_32               = 3,
} MsSpeedTypeCap_t;

#define MS_INFO_CLASS_MASK        0x00FF0000
#define MS_INFO_CLASS_LSB         16
typedef enum {
	MS_CLASS_MSPRO                = 2,
} MsClassCap_t;

#define MS_INFO_RESERVED_MASK     0xFF000000
#define MS_INFO_RESERVED_LSB      24

/* for SP5K_DISK_ID_CALLBACK_CFG callback returned value */
/* SD */
#define SD_SP_SPEEDMODE_MASK      0x0000000F
#define SD_SP_SPEEDMODE_LSB       0
#define SD_SP_QSCANLATCH_MASK     0x00000010
#define SD_SP_QSCANPHASE_MASK     0x00000020
#define SD_SP_DRIVEADJ_MASK       0x000000C0 /* 1~3 -> 4,6,8 mA */
#define SD_SP_DRIVEADJ_LSB        6
#define SD_SP_MAXCLKMHZ_MASK      0x0000FF00 /* 1~255 MHz */
#define SD_SP_MAXCLKMHZ_LSB       8
#define SD_SP_PHASEADJ_MASK       0x00FF0000
#define SD_SP_PHASEADJ_LSB        16

/* Memory Stick */
#define MS_SP_SPEEDMODE_MASK      0x00000003
#define MS_SP_SPEEDMODE_LSB       0
typedef enum {
	MS_MODE_1BIT                  = 1,
	MS_MODE_4BIT                  = 2
} MsModeCap_t;
#define MS_SP_MAXCLKMHZ_MASK      0x0000FF00 /* 1~255 MHz */
#define MS_SP_MAXCLKMHZ_LSB       8

#define SP5K_FILEDISK_RAWC         ":RAWC" /*DISK_FILEDISK_RAWC*/

#define SP5K_MIRRORDISK_FLUSH      0xffffffff
#define SP5K_MIRRORDISK_NO_BACKUP  0xffffffff /*DISK_MIRROR_NO_BACKUP*/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
UINT32 sp5kDiskMount(UINT32);
UINT32 sp5kDiskMountDirect(UINT32);
UINT32 sp5kDiskDriverPinCfg(UINT32 ,UINT32 ,UINT32 ,UINT32 ,UINT32 );
UINT32 sp5kDiskRamAttrSet( UINT32 );
UINT32 sp5kDiskInfoGet(UINT32 ,UINT32 ,UINT32 *);
UINT32 sp5kDiskUnMount(UINT32);
UINT32 sp5kDiskDiscard(UINT32);
#if 0
UINT32 sp5kDiskNandBadBlkScan(UINT32, UINT32, UINT32 *, UINT32 *);
#endif
UINT32 sp5kDiskOptionSet(UINT32,UINT32);
#endif  /* _SP5K_DISK_API_H_ */

