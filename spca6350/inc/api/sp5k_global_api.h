/**************************************************************************
 *
 *       Copyright (c) 2002-2014 by iCatch Technology, Inc.
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
 * @file sp5k_global_api.h
 *
 * @author
 */
#ifndef _SP5K_GLOBAL_API_H_
#define _SP5K_GLOBAL_API_H_

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include <time.h>
#include "common.h"
#include "middleware/global_def.h"
#include "middleware/sensor_def.h"
#include "middleware/cdsp_def.h"
#include "middleware/snap_def.h"

/**************************************************************************
 *                          C O N S T A N T S
 **************************************************************************/
/*
 * sp5kGpioCfg(), sp5kButtonGpioCfg()
 */
typedef enum _sp5kGpioGrp_e {
	SP5K_GPIO_GRP_TGL         = GPIO_GRP_TGL,
	SP5K_GPIO_GRP_LMI         = GPIO_GRP_LMI,
	SP5K_GPIO_GRP_FML         = GPIO_GRP_FML,
	SP5K_GPIO_GRP_FMH         = GPIO_GRP_FMH,
	SP5K_GPIO_GRP_GEN         = GPIO_GRP_GEN,
	SP5K_GPIO_GRP_DISPL       = GPIO_GRP_DISPL,
	SP5K_GPIO_GRP_DISPH       = GPIO_GRP_DISPH,
	SP5K_GPIO_GRP_POCU        = GPIO_GRP_POCU,
	SP5K_GPIO_GRP_USB         = GPIO_GRP_USB,
	SP5K_GPIO_GRP_SAR	      = GPIO_GRP_SAR,
	SP5K_GPIO_GRP_AUD_LINEOUT = GPIO_GRP_AUD_LINEOUT,
	SP5K_GPIO_GRP_CEC         = GPIO_GRP_CEC,
	SP5K_GPIO_GRP_RGB         = GPIO_GRP_RGB,
	SP5K_GPIO_GRP_TOT,
	SP5K_GPIO_GRP_NO_USE      = GPIO_GRP_NO_USE,
} sp5kGpioGrp_e;
/* for backward compatible. */
#define SP5K_GPIO_GRP_DISP  SP5K_GPIO_GRP_DISPL

/*
 * sp5kGpioCfgSet()
 */
typedef enum _sp5kGpioDir_e {
	SP5K_GPIO_DIR_INPUT,
	SP5K_GPIO_DIR_OUTPUT,
} sp5kGpioDir_e;

/*
 * sp5kGpioIsrReg(), sp5kGpioIntEnable()
 */
typedef enum _sp5kGpioInt_e {
	SP5K_GPIO_INT_LMI08  =  0,
	SP5K_GPIO_INT_LMI09  =  1,
	SP5K_GPIO_INT_LMI16  =  2,
	SP5K_GPIO_INT_LMI17  =  3,
	SP5K_GPIO_INT_LMI18  =  4,
	SP5K_GPIO_INT_LMI19  =  5,
	SP5K_GPIO_INT_LMI20  =  6,
	SP5K_GPIO_INT_LMI21  =  7,
	SP5K_GPIO_INT_FM00   =  8,
	SP5K_GPIO_INT_FM01   =  9,
	SP5K_GPIO_INT_FM24   = 10,
	SP5K_GPIO_INT_FM25   = 11,
	SP5K_GPIO_INT_FM26   = 12,
	SP5K_GPIO_INT_FM27   = 13,
	SP5K_GPIO_INT_FM28   = 14,
	SP5K_GPIO_INT_FM29   = 15,
	SP5K_GPIO_INT_FM30   = 16,
	SP5K_GPIO_INT_FM31   = 17,
	SP5K_GPIO_INT_FM45   = 18,
	SP5K_GPIO_INT_FM46   = 19,
	SP5K_GPIO_INT_FM51   = 20,
	SP5K_GPIO_INT_DISP17 = 21,
	SP5K_GPIO_INT_GEN08  = 22,
	SP5K_GPIO_INT_GEN10  = 23,
	SP5K_GPIO_INT_TG02   = 24,
	SP5K_GPIO_INT_TG03   = 25,
	SP5K_GPIO_INT_TOT    = 26,

	/* source level compatible with K33/V33 as possible. */
	SP5K_GPIO_INT0  = SP5K_GPIO_INT_TG02,
	SP5K_GPIO_INT1  = SP5K_GPIO_INT_TG03,
	SP5K_GPIO_INT7  = SP5K_GPIO_INT_DISP17,
	SP5K_GPIO_INT8  = SP5K_GPIO_INT_FM45,
	SP5K_GPIO_INT10 = SP5K_GPIO_INT_FM51,
} sp5kGpioInt_e;

typedef enum {
	SP5K_GPIO_ACTIVE_LOW           = 0,
	SP5K_GPIO_ACTIVE_HIGH          = 1,
} sp5kDiskGpioPolarity_t;

#define SP5K_GPIO_POL_UNKNOWN		0xffff

/*
 * sp5kCntrCfg()
 */
typedef enum _sp5kCntr_e {
	SP5K_CNTR0 = 0,
	SP5K_CNTR1,
	SP5K_CNTR_TOT,
} sp5kCntr_e;

/*
 * sp5kButtonEnable()
 */
typedef enum _sp5kButtonType_e {
	SP5K_BUTTON_TYPE_GPIO   = 0,
	SP5K_BUTTON_TYPE_ADC    = 1,
	SP5K_BUTTON_TYPE_MATRIX = 2,
	SP5K_BUTTON_TYPE_ALL    = 0xffff,
} sp5kButtonType_e;

/*to enable/disable all the buttons in that button type*/
#define SP5K_BUTTON_ID_ALL  0xffff

typedef enum {
	SP5K_CNTR_DEV_0,
	SP5K_CNTR_DEV_1,
} sp5kCntrDev_t;

typedef enum {
	SP5K_CNTR_LMI16,
	SP5K_CNTR_LMI17,
	SP5K_CNTR_LMI18,
	SP5K_CNTR_LMI19,
	SP5K_CNTR_LMI20,
	SP5K_CNTR_LMI21,
	SP5K_CNTR_LMI_NO_USE=0xFF,
} sp5kCntrPin_t;

typedef enum {
	SP5K_CNTR_EDGE_RISE,
	SP5K_CNTR_EDGE_FALL,
	SP5K_CNTR_EDGE_BOTH,
	SP5K_CNTR_EDGE_PHASE,
} sp5kCntrEdge_t;

typedef enum {
	SP5K_CNTR_MODE_DOWN,
	SP5K_CNTR_MODE_UP,
} sp5kCntrMode_t;

typedef enum {
	SP5K_CNTR_CFG_PIN, 		/* pin */
	SP5K_CNTR_CFG_EDGE, 	/* Edge */
	SP5K_CNTR_CFG_MODE,		/* Count up/down*/
	SP5K_CNTR_CFG_PERIOD, 	/* PERIOD */
	SP5K_CNTR_CFG_CALLBACK, /* ISR */
	SP5K_CNTR_CFG_INT_VAL0,	/* isr val 0 */
	SP5K_CNTR_CFG_INT_VAL1, /* isr val 1 */
} sp5kCntrCfg_t;

typedef enum {
	SP5K_CNTR_CTRL_ENABLE,
	SP5K_CNTR_CTRL_DISABLE,
	SP5K_CNTR_CTRL_CLEAR,
	SP5K_CNTR_CTRL_READ,
} sp5kCntrCtrl_t;

typedef enum {
	SP5K_CNTR_ISR_VAL0,
	SP5K_CNTR_ISR_VAL1,
} sp5kCntrIsr_t;

/*
 * sp5kGpioIsrReg(), sp5kAdcIntEnable()
 */
typedef enum _sp5kTrigLvl_e {
	SP5K_TRIG_LVL_RISE = 1,
	SP5K_TRIG_LVL_FALL = 2,
	SP5K_TRIG_LVL_BOTH = 3,
} sp5kTrigLvl_e;

/*
 * sp5kHostMsgSend()
 */
typedef enum _sp5kHostMsg_e {
	SP5K_MSG_USER0      = 0,
	SP5K_MSG_USER_MAX   = SP5K_MSG_USER0,
	SP5K_MSG_USER_TOTAL = SP5K_MSG_USER_MAX + 1,
} sp5kHostMsg_e;

typedef enum _sp5kHostInitMsg_e {
	SP5K_MSG_HOST_INIT = 0,
} sp5kHostInitMsg_e;

/*
 * rtc
 */
typedef enum _sp5kRtcOption_e {
	SP5K_DATE_TIME_OPTION     = 0,
	SP5K_ALARM_OPTION         = 1,
	SP5K_WAKEUP_OPTION        = 2,
	SP5K_DATE_TIME_BLOCK_WAIT = 0x80000000,
} sp5kRtcOption_e;

typedef enum _sp5kRtcCurrent_e {
	SP5K_RTC_CURRENT_0_5_UA,
	SP5K_RTC_CURRENT_0_9_UA,
	SP5K_RTC_CURRENT_1_41_UA,
	SP5K_RTC_CURRENT_2_41_UA,
	SP5K_RTC_CURRENT_4_41_UA,
	SP5K_RTC_CURRENT_8_43_UA,
	SP5K_RTC_CURRENT_12_94_UA,
	SP5K_RTC_CURRENT_19_96_UA,
	SP5K_RTC_CURRENT_MAX,
} sp5kRtcCurrent_e;

/*
 * timer
 */
typedef enum _sp5kTimerId_e {
	SP5K_TIMER_ID_0  = 0,
	SP5K_TIMER_ID_1  = 1,
	SP5K_TIMER_ID_2  = 2,
	SP5K_TIMER_ID_3  = 3,
	SP5K_TIMER_ID_4  = 4,
	SP5K_TIMER_ID_5  = 5,
	SP5K_TIMER_ID_6  = 6,
	SP5K_TIMER_ID_7  = 7,
	SP5K_TIMER_MAX   = SP5K_TIMER_ID_7,
	SP5K_TIMER_TOTAL = SP5K_TIMER_MAX + 1,
} sp5kTimerId_e;

typedef enum _sp5kTimerUsIsrId_e {
	SP5K_TIMER_US_ISR_ID_0 = 0,
	SP5K_TIMER_US_ISR_ID_1 = 1,
	SP5K_TIMER_US_ISR_ID_2 = 2,
} sp5kTimerUsIsrId_e;

/*
 * time delay
 */
typedef enum _sp5kTimeDelay_e {
	SP5K_TIME_DELAY_100NS = 0,
	SP5K_TIME_DELAY_1US   = 1,
	SP5K_TIME_DELAY_10US  = 2,
	SP5K_TIME_DELAY_1MS   = 3,
} sp5kTimeDelay_e;

typedef enum _sp5kPowerCtrl_e {
	SP5K_POWER_OFF         = 0,
	SP5K_POWER_ON          = 1,
	SP5K_POWER_OFF_POLLING = 2,
} sp5kPowerCtrl_e;

#define SP5K_SCAN_IO_MAX  4

/*
 * sp5kPwrDetectGet()
 */
typedef enum _sp5kPwrDetectSrc_e {
	SP5K_PWR_SRC_PWRON         = (1 <<  0),  /* pwron0. */
	SP5K_PWR_SRC_USB           = (1 <<  1),  /* pwron1. */
	SP5K_PWR_SRC_PLAYBACK      = (1 <<  2),  /* pwron2. */
	SP5K_PWR_SRC_CUSTOM        = (1 <<  3),  /* pwron3. */
	SP5K_PWR_RISE_EVT_PWRON    = (1 <<  5),  /* pwron0. */
	SP5K_PWR_RISE_EVT_USB      = (1 <<  6),  /* pwron1. */
	SP5K_PWR_RISE_EVT_PLAYBACK = (1 <<  7),  /* pwron2. */
	SP5K_PWR_RISE_EVT_CUSTOM   = (1 <<  8),  /* pwron3. */
	SP5K_PWR_FALL_EVT_PWRON    = (1 << 10),  /* pwron0. */
	SP5K_PWR_FALL_EVT_USB      = (1 << 11),  /* pwron1. */
	SP5K_PWR_FALL_EVT_PLAYBACK = (1 << 12),  /* pwron2. */
	SP5K_PWR_FALL_EVT_CUSTOM   = (1 << 13),  /* pwron3. */
	SP5K_PWR_SRC_BAK_PWRON     = (1 << 15),  /* pwron0. */
	SP5K_PWR_SRC_BAK_USB       = (1 << 16),  /* pwron1. */
	SP5K_PWR_SRC_BAK_PLAYBACK  = (1 << 17),  /* pwron2. */
	SP5K_PWR_SRC_BAK_CUSTOM    = (1 << 18),  /* pwron3. */
	SP5K_PWR_SRC_RTC           = (1 << 24),
} sp5kPwrDetectSrc_e;

/*
 * sp5kAdcRead(ch,*pVal)
 */
typedef enum _sp5kAdcCh_e {
	SP5K_ADC_SARIN0 = 0,
	SP5K_ADC_SARIN1,
	SP5K_ADC_SARIN2,
	SP5K_ADC_SARIN3,

	SP5K_ADC_SARIN4,
	SP5K_ADC_SARIN5,
	SP5K_ADC_SARIN6,
	SP5K_ADC_TPXP = SP5K_ADC_SARIN6,
	SP5K_ADC_SARIN7,
	SP5K_ADC_TPXN = SP5K_ADC_SARIN7,
	SP5K_ADC_SARIN8,
	SP5K_ADC_TPYP = SP5K_ADC_SARIN8,
	SP5K_ADC_SARIN9,
	SP5K_ADC_TPYN = SP5K_ADC_SARIN9,
	SP5K_ADC_SARIN10,
	SP5K_ADC_TV_DAC_BANDGAP = SP5K_ADC_SARIN10,
	SP5K_ADC_SARIN11,
	SP5K_ADC_BATTERY_FEEDBACK = SP5K_ADC_SARIN11,
	SP5K_ADC_SARIN12,
	SP5K_ADC_USB_CHARGER_DETECT = SP5K_ADC_SARIN12,
	SP5K_ADC_SARIN13,
	SP5K_ADC_TEMPERATURE_SENSOR = SP5K_ADC_SARIN13,
	SP5K_ADC_SARIN_MAX
} sp5kAdcCh_e;

/*
 *
 */
typedef enum _sp5kSysHaltOperate_e {
	SP5K_SYS_HALT_DEBUG, /* software watchdog timer */
	SP5K_SYS_HALT_GOTO_CMDLINE,
	SP5K_SYS_HALT_DELAY_POWEROFF,
	SP5K_SYS_HALT_IMMEDIATE_POWEROFF,
	SP5K_SYS_HALT_DELAY_REBOOT,
	SP5K_SYS_HALT_IMMEDIATE_REBOOT,
	SP5K_SYS_HALT_POWEROFF_REBOOT,
} sp5kSysHaltOperate_e;

/*
 * sp5k_sys_cfg_xxx.c
 */
/*paramId*/
typedef enum _sp5kSystemCfg_e {
	SP5K_SERVICE_TIME_COUNT,         /* obsolete */
	SP5K_CDSP_INFO_CFG_GROUP,        /* param1:cdspInfoCfg_t in "cdsp_def.h"*/
	SP5K_SYS_CFG_CDSP_RGB_GAMMA_TYPE,
	SP5K_CNTR_ISR_LPF,               /* obsolete */
	SP5K_SENSOR_DEV_MODEL_CFG,       /* param1:sensorDevCfg_t in "sensor_def.h"*/
	SP5K_SENSOR_SYS_CFG,             /* param1:sensorStsCfg_t in "sensor_def.h"*/
	_SP5K_SNAP_SYS_CFG_OBSOLETE,               /* param1:snapStsCfg_t in "snap_def.h"*/ /* obsolete */
	SP5K_FIXED_PREV_DZ_CDSP_ASPECT,
	SP5K_USB_FULL_SPEED_CFG,         /* param1: 1 => USB full speed, 0 => USB high speed*/
	SP5K_DISK_CALLBACK_CFG,          /* param1: 1 => enable disk mount callback, 0 => disable*/
	SP5K_CDSP_FLIP_CFG,              /* param1:cdspFlipVal_t in "cdsp_def.h",0: OFF, 1: H-flip, 2: V-flip, 3: HV-flip */
	SP5K_CDSP_AEAF_SW_FLIP_CFG,      /* param1:cdspAaSwFlipSel_t, param2:cdspFlipVal_t in "cdsp_def.h"*/
	SP5K_DISP_TV_WINDOW_CFG,         /* obsolete *//*param1:x0, param2:y0, param3:x1, param4:y1*/
	SP5K_CDSP_SYS_CFG,
	SP5K_BKPV_SKIP_FRAME_CFG,        /* param1: frame count to skip*/
	SP5K_CDSP_COFILTER_EOF_CFG,      /* obsolete from 5310 */
	SP5K_COLOR_KEY_Y_TH_SET,         /* param1: low threshold of y value*/
	SP5K_COLOR_KEY_Y_TH_GET,         /* param1: low threshold of y value*/
	SP5K_EXIF_CUSTOM_RTC_CFG,        /* param1: pointer to datetime string*/
	SP5K_NAND_WP_DELAY_CFG,          /* param1: delay count*/
	SP5K_MODESW_DUP_DEFAULT_CFG,     /* param1: 0 => clear standby, 1 => backup last frame*/
	SP5K_VFS_FAT32_2GB_SD_CFG,       /* param1: 1 => support FAT32 on 2GB SD, 0 => not support*/
	SP5K_EXIF_MAKERNOTE_SIZE_CFG,    /* param1: max size of exif makernote*/
	SP5K_CDSP_MAX_LINE_WIDTH_CFG,    /* obsolete?*/
	SP5K_DISP_SYS_CFG,
	SP5K_DISP_2ND_PATH_CFG,          /* no function?*/
	SP5K_DISK_SD_HW_PIN_CFG,
	SP5K_MEM_SMALL_POOL_SIZE_CFG,    /* param1: max size of small pool in bytes*/
	SP5K_RSV_THM_SIZE_CHECK_CFG,     /* obsolete *//*param1: 1 => check thm size (default), 0 => not */
	SP5K_SOC_HW_VER_CFG,             /* param1: 0=ver A, 1=ver B, only for cfg get*/
	SP5K_FAST_SENSOR_INIT_CFG,       /* obsolete *//*param1: 0=normal, 1=fast sensor init*/
	SP5K_PWRMOS_SWITCH_CFG,          /* param1: 0=disable, 1=enable card power MOS switch*/
	SP5K_MEM_ALLOC_DEBUG_PTR_CFG,    /* param1: the address dump by os mem */
	SP5K_PERIPHERAL_SYS_CFG,
	SP5K_STDANDBY_FRAME_SIZE_CFG,    /* param1: standby frame width, param2: standby frame height*/
	SP5K_BUTTON_SYS_CFG,             /* param1: Set ADC Button release debounce,unit 20ms*/
	SP5K_DISK_SECTOR_NUMS_CFG,
	SP5K_NAND_MAP_TABLE_CFG,
	SP5K_SD_RW_DELAY_CFG,
	SP5K_SD_CLK_PHASE_CFG,
	SP5K_SD_BUS_SPEED_CFG,
	SP5K_SD_BUS_WIDTH_CFG,
	SP5K_SD_MEDIA_TYPE_CFG,
	SP5K_SD_IO_LDO_CFG,
	SP5K_SD_IO_MODE_CFG,
	SP5K_CLK_CPU_DIV_CFG,
	SP5K_CLK_CPU_CLK_CFG,
	SP5K_SYS_HALT_OPERAT_CFG,        /* param1: sp5kSysHaltOperate_e, param2: watchdog timer interval (us), param3: delay time (s) for power off. */
	SP5K_CDSP_2NDPATH_FACTOR_CFG,    /* param1[0:1]width factor, 0 - 1x, 1 - 1/2, 2 - 1/4, param1[16:17] height factor, 0 - 1x, 1 - 1/2, 2 - 1/4. */
	SP5K_AEAWB_WIN_NOCROP,
	SP5K_SDIO_CLK_PHASE_CFG,
	SP5K_SDIO_FUNC_NUM,
	SP5K_SDIO_CMD53_READ_NORSP, /*obsoleted*/
	SP5K_SDIO_SEL_PIN_MUX_GRP,
	SP5K_SDIO_IRQ_THRD_PRI,
	SP5K_DISK_DETECT_PIN_CFG,
	SP5K_DISK_POWER_ON_PIN_CFG,
	SP5K_DISK_BUF_ENABLE_PIN_CFG,
	SP5K_DISK_WRITE_PROTECT_PIN_CFG,
	SP5K_DISK_SD18V_CTRL_PIN_CFG,
	SP5K_DISK_NAND_SPI_WP_CTRL_PIN_CFG,
	SP5K_DISK_PWR_RECYCLE_CALLBACK_CFG,
	SP5K_NAND_FLOW_MODE_CFG,
	SP5K_NAND_BCH_EN_CFG,
	SP5K_NAND_COPYBACK_CFG,
	SP5K_NAND_FAKEID_CFG,
	SP5K_NAND_EXT_NAND_CALLBACK_CFG,
	SP5K_CLK_FD_CLK_CFG, /* param1: clock in KHz. */
	SP5K_MEDIA_SYS_CFG, /* defined in media_def.h */
	SP5K_FS_OPTION_CFG, /* define for FS option set. It's the same as original sp5kDiskOptionSet. */
	SP5K_SYS_PWRONSRC_PINMASK_CFG, /* param1: [3..0] mapping to SP5K_PWR_SRC_CUSTOM | SP5K_PWR_SRC_PLAYBACK | SP5K_PWR_SRC_USB | SP5K_PWR_SRC_PWRON */
	SP5K_SYS_PWROFFSRC_PINMASK_CFG, /* param1: [3..0] mapping to SP5K_PWR_SRC_CUSTOM | SP5K_PWR_SRC_PLAYBACK | SP5K_PWR_SRC_USB | SP5K_PWR_SRC_PWRON */
	SP5K_SYS_CARD_UPDATE_PRESERVE_C_CFG, /* param1: 1:preserve C, 0:no preserve C */
	SP5K_DISK_DEV_CFG, /* device specific configuration like ramdisk size and filedisk name ... */
	SP5K_DISK_ID_CALLBACK_CFG,       /* param1: storage ID, param2: function pointer of (UINT32)func(UINT32 type,UINT8 *pCid,UINT32 num), 0 => disable */
	SP5K_MEDIA_INPUT_HEIGHT_ALIGN16, /* param1: force video src align 16 */
	SP5K_SDIO_EN,
	SP5K_DISK_RSVA_PROTECT, /* param1: write protect A */
	SP5K_DISK_ECC_FAIL_CFG, /* param1: storage ID, param2: 1:treat ECC fail as fail, 0:ignore ECC fail (default) */
	SP5K_FIF_SAVE_OPT, /* param1 = 0:write on any FIF entry write, 1:write on fsFifClose only */
	SP5K_SD_SPEEDCLASS_CFG, /*obsoleted*/
	SP5K_SYS_CARD_UPDATE_BACKUP_DEV_CFG, /* param1: storage ID */
	SP5K_SPI_1BIT_MAX_FREQ, /* param1: maximum freq limit in Khz */
	SP5K_SPI_4BIT_MAX_FREQ, /* param1: maximum freq limit in Khz */
	SP5K_DISK_SD_IO_ERR_CFG, /* param1: SD IO Err */
	SP5K_DISK_SD_RE_DETECT_CFG, /* param1: 0: not allow SD re-detect, 1: allow SD re-detect without power cycle, 2: allow SD re-detect with power cycle */
	SP5K_SD_WR_SLOW_RESET_THR_CFG, /* param1: rel_threshold(%), 1~99; para2: abs_threshold(KB/s); para3: avg_duration(ms); set thr = 0 means disable, set -1 (0xFFFFFFFF) means not change */
	SP5K_DISK_WRITE_PROTECT_CFG, /* param1: storage ID, param2: WRITE_PROTECT */
	SP5K_DISK_PASSWORD_LOCK_CFG, /* param1: storage ID, param2: (sp5kSdLock_t), param3: psw_len, param4: password */
	SP5K_FS_ANTI_FRAGMENT_CFG, /* param1: storage ID, param2: size in bytes */
	SP5K_DISK_RESET, /* param1: storage ID, param2: power reset */
	SP5K_DISK_BUSY_MEAS_CFG, /* param1: storage ID, param2: period (ms), param3: max_busy_percent (%) */
	SP5K_DISK_BUSY_CALLBACK_CFG, /* param1: function pointer of (void)func(UINT32 busy_percent), param2: threshold (%) */
	SP5K_JPG_DECODE_FAST_CFG,/* param1: 1 => Enable, 0 => Disable*/
	SP5K_CLK_DISP_EN,	/* param1: 1 => Enable, 0 => Disable */
	SP5K_SPI_WRITE_MAX_FREQ, /* param1: maximum freq limit in Khz */
	SP5K_SYS_CFG_BOOT_FW_INFO, /* get only. param1: 0=cold boot FW1, 0x80000000=normal boot FW1, 0x80000001=normal boot FW2 */
	SP5K_DISK_MIRROR_CFG, /* param1: storage ID, param2: backup rawC offset */
	SP5K_SPI_CUSTOM_CFG, /* param1: cmdBuf, param2: len */
} sp5kSystemCfg_e;

/*
 * sp5kSystemCfgGet(SP5K_CDSP_FLIP_CFG,...)
 */
typedef enum _sp5kFlipCfg_e {
	SP5K_FLIP_VAL_NOFLIP = CDSP_FLIP_VAL_NOFLIP,
	SP5K_FLIP_VAL_HFLIP,
	SP5K_FLIP_VAL_VFLIP,
	SP5K_FLIP_VAL_BOTHFLIP,
	SP5K_FLIP_VAL_TOTAL,
} sp5kFlipCfg_t;

/*
 * sp5kSystemCfgGet(SP5K_CDSP_AEAF_SW_FLIP_CFG,...)
 */
typedef enum _sp5kAaSwFlipCfg_e {
	SP5K_AA_SW_FLIP_CFG_ALL = CDSP_AA_SW_FLIP_CFG_ALL,
	SP5K_AA_SW_FLIP_CFG_AEWIN,
	SP5K_AA_SW_FLIP_CFG_AFWIN,
	SP5K_AA_SW_FLIP_CFG_TOTAL
} sp5kAaSwFlipCfg_t;


/*define FS option and value*/
#define SP5K_FS_OPT_FORMAT_METHOD           0x0001
#define SP5K_FS_OPT_FORMAT_TYPE             0x0002
#define SP5K_FS_OPT_READ_TIME               0x0003
#define SP5K_FS_OPT_USE_MODIFY_TIME         0x0004
#define SP5K_FS_OPT_RTC_OFFSET_MINUS        0x0005
#define SP5K_FS_OPT_RTC_OFFSET_TICK         0x0006
#define SP5K_FS_OPT_IGNORE_MTIME            0x0007
#define SP5K_FS_OPT_COPY_USE_MTIME          0x0008
#define SP5K_FS_OPT_GETREMAINCLUS_ON_MOUNT  0x0009
#define SP5K_FS_OPT_FAT_BITMAP              0x0010 /*obsoleted*/

#define SP5K_SDIO_PIN_MUX_GRP_CLK20 0
#define SP5K_SDIO_PIN_MUX_GRP_CLK29 1
#define SP5K_SDIO_PIN_MUX_GRP_CLK46 2
/*
 * sp5kDiskCfgSet(SP5K_SDIO_CLK_PHASE_CFG,...)
 */
typedef struct _sp5kDiskCfg_SdioPhase_s {
	int clk_freq; /* khz, phase control works on this clock speed */
	char mmcmode; /* 0: SD mode 1: MMC mode */
	char clk_delay; /* ns, 0 ~ 7 */
	char clk_rvs; /* 1: reverse clock */
	char pos_latch; /* 1: pos edge latch */
	char data_delay; /* ns, 0 ~ 7 */
} sp5kDiskCfg_SdioPhase_t;

/*
 * sp5k_log.c
 */
typedef enum {
	SP5K_LOG_DISABLE,
	SP5K_LOG_ENABLE,
	SP5K_LOG_BACKGROUND,
	SP5K_LOG_BUFFER,
	SP5K_LOG_CUSTOM,
	SP5K_LOG_MAX
} sp5kLogControl_t;

/* for SP5K_DISK_PASSWORD_LOCK_CFG */
typedef enum {
	SP5K_SD_PASSWORD_UNLOCK        = (0),
	SP5K_SD_PASSWORD_LOCK          = (1),
	SP5K_SD_PASSWORD_SET           = (2),
	SP5K_SD_PASSWORD_ERASE         = (3),
} sp5kSdLock_t;

/*For cmdCdspSysCfgDump() dump internal/public info*/
typedef enum {
	CDSP_SYS_CFG_INTERNAL_INFO=0,
	CDSP_SYS_CFG_PUBLIC_INFO,
} sp5kUserSysCfgOpt_e;

/**************************************************************************
 *                          D A T A   T Y P E S
 **************************************************************************/
typedef struct tmx_s {
	int tmx_sec;  /* seconds, 0-60 */
	int tmx_min;  /* minutes, 0-59 */
	int tmx_hour; /* hours, 0-23 */
	int tmx_mday; /* day of the month, 1-31 */
	int tmx_mon;  /* months since Jan, 1-12 <---------------------------(not 0-11) */
	int tmx_year; /* years from 1900 */
	int tmx_wday; /* days since Sunday, 0-6 */
	int tmx_yday; /* days since Jan 1, 0-365 */
	int tmx_isdst; /* Daylight Saving Time indicator */
} tmx_t;

/* For SP5K callback phototype */
typedef UINT32 (*fpsp5kCallback_t)(UINT32, UINT32, void *);
typedef UINT32 (*fpsp5kVoidCallback_t)(void);
typedef UINT32 (*sp5kCntrCallback_t)(void);
typedef UINT32 (*fpSp5kMsgCallback_t)(UINT32 *msgId, UINT32 *param); /* it should be with same type of fpMsgCallback_t. The def should be from middleware .h */

typedef struct sp5kIoScan_s {
	UINT32 group;
	UINT32 pinNum;
	UINT32 valIni;
	UINT32 expireMs;
	UINT32 msgCmd[2];
	UINT32 msgParam[2];
	void (*pfunc[2])(UINT32);
} sp5kIoScan_t;

/**************************************************************************
 *              D A T A  D E C L A R A T I O N S
 **************************************************************************/

/**************************************************************************
 *              F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
/* sp5k_adc.c */
UINT32 sp5kAdcInit(void);
UINT32 sp5kAdcDeInit(void);
UINT32 sp5kAdcRead(UINT32 chnlId, UINT32 *pval);
void   sp5kAdcEnable(UINT32 en); /* added by Edward 20071119 */

/* sp5k_button_xxx.c */
UINT32 sp5kButtonInit(UINT32 gpioBtnTot, UINT32 gpioMtxBtnTot, UINT32 adcBtnTot);
UINT32 sp5kButtonGpioInit(UINT32 gpioBtnTot);
UINT32 sp5kButtonAdcInit(UINT32 adcBtnTot);
UINT32 sp5kButtonMatrixInit(UINT32 mtxPinTot);
UINT32 sp5kButtonGpioCfgSet(UINT32 btnId, UINT32 grpId, UINT32 pinId, UINT32 polar, UINT32 debnc, UINT32 repeat);
UINT32 sp5kButtonAdcCfgSet(UINT32 btnId, UINT32 chnlId, UINT32 loBnd, UINT32 hiBnd, UINT32 debnc, UINT32 repeat);
UINT32 sp5kButtonEnable(UINT32 btnType, UINT32 btnId, UINT32 en);
UINT32 sp5kButtonMatrixCfgSet(UINT32 btnId, UINT32 polar, UINT32 debnc, UINT32 repeat);
UINT32 sp5kButtonMatrixPinSet(UINT32 cfgId, UINT32 grpId, UINT32 pinId, UINT32 dir);

/* sp5k_cntr.c */
UINT32 sp5kCntrCfgSet(UINT32 cntrId, UINT32 selector, UINT32 param1, UINT32 param2);
UINT32 sp5kCntrControl(UINT32 cntrId, UINT32 ctrl, ...);
UINT32 sp5kCntrRead(UINT32 cntrId, UINT32 *pval);
void sp5kUsbEyeChartConfig(UINT8 sif2Data, UINT8 currentStrength);

/* gpio.c */
#define sp5kGpioCfgSet  gpioCfgSet  /*(UINT32 grpId, UINT32 pinMsk, UINT32 dir);*//*dir:0=in, 1=out*/
#define sp5kGpioRead    gpioRead    /*(UINT32 grpId, UINT32 pinMsk, UINT32 *pval);*/
#define sp5kGpioWrite   gpioWrite   /*(UINT32 grpId, UINT32 pinMsk, UINT32 val);*/
#define sp5kGpioPullSet gpioPullSet /*(UINT32 grpId, UINT32 pinMsk, UINT32 enable);*/
#define sp5kGpioDirSet  gpioDirSet  /*(UINT32 grpId, UINT32 pinMsk, UINT32 dir);*//*dir:0=in, 1=out*/
#define sp5kGpioFuncSet gpioFuncSet /*(UINT32 grpId, UINT32 pinMsk, UINT32 val);*//*val:0=func, 1=io*/
#define sp5kGpioDrvSet  gpioDrvSet  /*(UINT32 grpId, UINT32 pinMsk, UINT32 hdw, UINT32 ldw);*//*hdw,ldw:0~3*/
UINT32  sp5kGpioIsrReg(UINT32 intId, UINT32 intLvl, void *isr);
UINT32  sp5kGpioIsrUnreg(UINT32 intId, UINT32 intLvl);
UINT32  sp5kGpioIntEnable(UINT32 intId, UINT32 en);

/* sp5k_pwm.c */
UINT32 sp5kPwmCfgSet(UINT32 chnlId, UINT32 freqDevider, UINT32 duty);
/**
 @brief Extenstion configure PWM output setting.
 For K33/V33, 1 tick for PWM tickBase is (1/48MHz) = 20.83 ns.
 With this API, you can control PWM frequence (Hz) ONLY by (lowTicks+highTicks).
 Min=0.
 Max=4095 for chnlId==0, Max=2047 for the others.
 @param[in] id ID of PWM.
 @param[in] lowTicks count of LOW (Min=0. Max=4095 for chnlId==0, Max=2047 for the others)
 @param[in] highTicks count of HIGH (Min=0. Max=4095 for chnlId==0, Max=2047 for the others)
 @retval FAIL Fail
 @retval SUCCESS Success
 */
UINT32 sp5kPwmCfgSetExt(UINT32 chnlId, UINT32 lowTicks, UINT32 highTicks);
UINT32 sp5kPwmTrig(UINT32 startMsk, UINT32 stopMsk);

/* sp5k_msg.c */
UINT32 sp5kHostMsgSend(UINT32 msgid, UINT32 param);
UINT32 sp5kHostMsgReceive(UINT32 *pmsgid, UINT32 *pparam);
UINT32 sp5kHostMsgFilterCallbackSet(fpSp5kMsgCallback_t pfunc);

/* sp5k_rtc_direct.c */
/**
 * @brief to allow getting RTC related data before ros_initialize().
 * It should be only used in usbPowerOnGet().
 *
 * @param [out] pcode RTC reliable code.
 * @param [out] pinfo RTC extra information.
 * @param [out] ptime RTC date time.
 *
 * @return SUCCESS or FAIL.
 * @note caller should check return value to be SUCCESS and reliable code
 * to ensure the RTC contents are valid.
 */
UINT32 rtcDataDirectGet(UINT32 *pcode, UINT8 *pinfo, tmx_t *ptime);

/* sp5k_rtc.c */
UINT32 sp5kRtcReliableGet(UINT32 *pcode);
UINT32 sp5kRtcReliableSet(UINT32 reliableCode);
UINT32 sp5kRtcDateTimeGet(UINT32 option, tmx_t *ptime);
UINT32 sp5kRtcDateTimeSet(UINT32 option, tmx_t *ptime);
UINT32 sp5kRtcExtraGet(UINT8 *pinfo, UINT32 start, UINT32 size);
UINT32 sp5kRtcExtraSet(const UINT8 *pinfo, UINT32 start, UINT32 size);
UINT32 sp5kRtcCurrentSet(UINT32 current);
void   sp5kCustomTimeCheckCallbackSet(fpTimeCallback_t pfunc);
void   sp5kRtcSwEmulationSet(UINT32 en);

/* sp5k_timer.c */
UINT32 sp5kTimerCfgSet(UINT32 id, UINT32 ms);
UINT32 sp5kTimerCfgGet(UINT32 id, UINT32 *pms, UINT32 *penable);
UINT32 sp5kTimerEnable(UINT32 id, UINT32 enable);
UINT32 sp5kTimeDelay(UINT32 unit, UINT32 count);
UINT32 sp5kTimerIsrReg(UINT32 dontCare, UINT32 ms, void *pfunc);
UINT32 sp5kTimerIsrOneShotReg(UINT32 dontCare, UINT32 ms, void *pfunc);
UINT32 sp5kTimerUsOneShotReg(UINT32 dontCare, UINT32 us, void *pfunc);
UINT32 sp5kMsTimeGet(void);

/* sp5k_us_timer.c */
UINT32 sp5kTimerUsIsrReg(UINT32 tmrId, UINT32 repeat, UINT32 us, void *pfunc);

/* sp5k_version.c */
UINT32 sp5kSpcaVersionGet(void);

/* sp5k_ioscan.c */
UINT32 sp5kScanRegister(sp5kIoScan_t *pcfg);

/* sp5k_sys_cfg_xxx.c */
UINT32 sp5kSystemCfgSet(UINT32 paramId, ...);
UINT32 sp5kSystemCfgGet(UINT32 paramId, ...);

/*splitting from sp5kSystemCfgS/Get()*/
UINT32 sp5kDiskCfgGet(UINT32 paramId, ...);
UINT32 sp5kDiskCfgSet(UINT32 paramId, ...);
UINT32 sp5kDiskSdHwPinCfgSet(UINT32 sdio);
UINT32 sp5kDiskSdHwPinCfgGet(UINT32 *sdio);
UINT32 sp5kFsFat32On2GbSdSet(UINT32 en);

/* sp5k_log.c */
UINT32 sp5kLogControl( UINT32 paramId, ... );

/**
 * @brief Get power on detection.
 *
 * @param [in]  msk    Bitmap mask, combination of sp5kPwrDetectSrc_e.
 * @param [out] *pval  Bitmap 0: not, 1: detected.
 *
 * @return SUCCESS or FAIL.
 */
UINT32 sp5kPwrDetectGet(UINT32 msk, UINT32 *pval);

void sp5kCmdExec(char *cmd);

#endif  /* _SP5K_GLOBAL_API_H_ */

