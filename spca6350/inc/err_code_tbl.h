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
#ifndef __ERR_CODE_TBL_H__
#define __ERR_CODE_TBL_H__

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
/*
 * error code bitmap format, total 32 bits.
 *
 *  3      2 2      1 1
 *  1      4 3      6 5              0
 *  -------- -------- ----------------
 * |MMMMMMMM|SSSSSSSS|NNNNNNNNNNNNNNNN|
 *  -------- -------- ----------------
 *
 * MMMMMMMM field (bit 31~24, 8 bits): main group.
 * SSSSSSSS field (bit 23~16, 8 bits): sub group.
 * NNNNNNNNNNNNNNNN field (bit 15~0, 16 bits): error code number.
 *
 * note: MMMMMMMM = 00000000 and SSSSSSSS = 00000000 are reserved
 * for NO_ERR and error code group filter.
 */

/*
 * main groups.
 */
#define ERR_COMMON    (0x80 << 24)  /* common used by all modules */
#define ERR_OS        (0x81 << 24)  /* operating system */
#define ERR_FS        (0x82 << 24)  /* file system */
#define ERR_HAL       (0x83 << 24)  /* HAL layer */
#define ERR_SYSGLOB   (0x84 << 24)  /* system global */
#define ERR_USB       (0x85 << 24)  /* USB related */
#define ERR_DEV       (0x86 << 24)  /* device driver */
#define ERR_SNAP      (0x87 << 24)  /* snap image */
#define ERR_REC       (0x88 << 24)  /* record audio / video */
#define ERR_PLAY      (0x89 << 24)  /* play audio/video/media */
#define ERR_UI        (0x8A << 24)  /* UI control */
#define ERR_DISK      (0x8B << 24)  /* diskette */
#define ERR_DZOOM     (0x8C << 24)  /* digital zoom */
#define ERR_UTIL      (0x8D << 24)  /* utility */
#define ERR_MODESW    (0x8E << 24)  /* mode switch */
#define ERR_PREVIEW   (0x8F << 24)  /* preview */
#define ERR_RSVBLK    (0x90 << 24)  /* reserved block */
#define ERR_AUD_MIX   (0x91 << 24)  /* audio mix play */
#define ERR_SLIDE     (0x92 << 24)  /* slide show */
#define ERR_GFX       (0x93 << 24)  /* graphics */
#define ERR_SP5K      (0xE0 << 24)  /* sp5k api */
#define ERR_CUSTOM0   (0xF0 << 24)  /* customer definition 0 */
#define ERR_CUSTOM1   (0xF1 << 24)  /* customer definition 1 */
#define ERR_CUSTOM2   (0xF2 << 24)  /* customer definition 2 */
#define ERR_CUSTOM3   (0xF3 << 24)  /* customer definition 3 */

/*
 * common used by all modules.
 */
#define NO_ERR            (0x00000000)
#define ERR_COMMON_FILE   (ERR_COMMON | (0x01 << 16))
#define ERR_COMMON_MEM    (ERR_COMMON | (0x02 << 16))
#define ERR_COMMON_PARAM  (ERR_COMMON | (0x03 << 16))
#define ERR_COMMON_API    (ERR_COMMON | (0x04 << 16))

/*
 * common: file.
 */
#define ERR_FILE_OPEN_FAIL       (ERR_COMMON_FILE | 0x0000)
#define ERR_FILE_CLOSE_FAIL      (ERR_COMMON_FILE | 0x0001)
#define ERR_FILE_READ_FAIL       (ERR_COMMON_FILE | 0x0002)
#define ERR_FILE_WRITE_FAIL      (ERR_COMMON_FILE | 0x0003)
#define ERR_FILE_SEEK_FAIL       (ERR_COMMON_FILE | 0x0004)
#define ERR_FILE_FORMAT_UNKNOWN  (ERR_COMMON_FILE | 0x0005)

/*
 * common: memory.
 */
#define ERR_MEM_ALLOC_FAIL   (ERR_COMMON_MEM | 0x0000)
#define ERR_MEM_FREE_FAIL    (ERR_COMMON_MEM | 0x0001)
#define ERR_MEM_RESIZE_FAIL  (ERR_COMMON_MEM | 0x0002)
#define ERR_MEM_NO_ROOM      (ERR_COMMON_MEM | 0x0003)

/*
 * common: parameter.
 */
#define ERR_PARAM_INVALID  (ERR_COMMON_PARAM | 0x0000)
#define ERR_PARAM_UNSUPPORT  (ERR_COMMON_PARAM | 0x0001)

/*
 * common: API.
 */
#define ERR_API_UNSUPPORT  (ERR_COMMON_API | 0x0000)

/*
 * OS sub groups.
 */
#define ERR_OS_XX00  (ERR_OS | (0x00 << 16))
#define ERR_OS_XX01  (ERR_OS | (0x01 << 16))
#define ERR_OS_XX02  (ERR_OS | (0x02 << 16))

/*
 * FS sub groups.
 */
#define ERR_POSIX  (ERR_FS | (0x00 << 16))

/*
 * system global sub groups.
 */
#define ERR_SYSG_INVALID_ID           (ERR_SYSGLOB | (0x00 << 16))
#define ERR_SYSG_MODE_INVALID_ID      (ERR_SYSGLOB | 0x0000)
#define ERR_SYSG_MODE_TRANSIT_FAIL    (ERR_SYSGLOB | 0x0001)
#define ERR_SYSG_MODE_MSG_INIT_FAIL   (ERR_SYSGLOB | 0x0002)
#define ERR_SYSG_MODE_MSG_QUE_DELETE  (ERR_SYSGLOB | 0x0003)

/*
 * HAL sub groups.
 */
#define ERR_HAL_GLOB   (ERR_HAL | (0x00 << 16))
#define ERR_HAL_CPU    (ERR_HAL | (0x01 << 16))
#define ERR_HAL_DRAM   (ERR_HAL | (0x02 << 16))
#define ERR_HAL_DMA    (ERR_HAL | (0x03 << 16))
#define ERR_HAL_UART   (ERR_HAL | (0x04 << 16))
#define ERR_HAL_TMR    (ERR_HAL | (0x05 << 16))
#define ERR_HAL_GPRM   (ERR_HAL | (0x06 << 16))
#define ERR_HAL_PROB   (ERR_HAL | (0x07 << 16))
#define ERR_HAL_RTC    (ERR_HAL | (0x08 << 16))
#define ERR_HAL_GPIO   (ERR_HAL | (0x09 << 16))
#define ERR_HAL_CDSP   (ERR_HAL | (0x10 << 16))
#define ERR_HAL_MJPEG  (ERR_HAL | (0x18 << 16))
#define ERR_HAL_USB    (ERR_HAL | (0x20 << 16))
#define ERR_HAL_OTG    (ERR_HAL | (0x21 << 16))
#define ERR_HAL_SIF    (ERR_HAL | (0x30 << 16))
#define ERR_HAL_CMOS   (ERR_HAL | (0x31 << 16))
#define ERR_HAL_CCD    (ERR_HAL | (0x32 << 16))
#define ERR_HAL_AUDIO  (ERR_HAL | (0x40 << 16))
#define ERR_HAL_AC97   (ERR_HAL | (0x41 << 16))
#define ERR_HAL_I2S    (ERR_HAL | (0x42 << 16))
#define ERR_HAL_FLASH  (ERR_HAL | (0x50 << 16))
#define ERR_HAL_NAND   (ERR_HAL | (0x51 << 16))
#define ERR_HAL_SPI    (ERR_HAL | (0x53 << 16))
#define ERR_HAL_SD     (ERR_HAL | (0x54 << 16))
#define ERR_HAL_MS     (ERR_HAL | (0x55 << 16))
#define ERR_HAL_EMMC   (ERR_HAL | (0x56 << 16))
#define ERR_HAL_DISP   (ERR_HAL | (0x60 << 16))

/*
 * diskette.
 */
#define ERR_NAND   (ERR_DISK | (0x00 << 16))
#define ERR_SD     (ERR_DISK | (0x01 << 16))
#define ERR_MMC    (ERR_DISK | (0x02 << 16))
#define ERR_CF     (ERR_DISK | (0x03 << 16))
#define ERR_MS     (ERR_DISK | (0x04 << 16))
#define ERR_HD     (ERR_DISK | (0x05 << 16))
#define ERR_DRIVE  (ERR_DISK | (0x06 << 16))

/*
 * utility.
 */
#define ERR_BP    (ERR_UTIL | (0x00 << 16))  /* bad pixel */
#define ERR_DPOF  (ERR_UTIL | (0x08 << 16))  /* DPOF */
#define ERR_DCF   (ERR_UTIL | (0x09 << 16))  /* DCF */
#define ERR_EXIF  (ERR_UTIL | (0x0a << 16))  /* EXIF */
#define ERR_ISP   (ERR_UTIL | (0x0b << 16))  /* ISP */

/*
 * device driver.
 */
#define ERR_DEV_GLOB  (ERR_DEV | (0x00 << 16))
#define ERR_DEV_HNDL_INVALID  (ERR_DEV_GLOB | 0x0000)
#define ERR_DEV_OP_FUNC_NULL  (ERR_DEV_GLOB | 0x0001)
#define ERR_DEV_CB_FUNC_NULL  (ERR_DEV_GLOB | 0x0002)
#define ERR_DEV_ALLOC_FAIL    (ERR_DEV_GLOB | 0x0003)
#define ERR_DEV_OPEN_FAIL     (ERR_DEV_GLOB | 0x0004)
#define ERR_DEV_CLOSE_FAIL    (ERR_DEV_GLOB | 0x0005)
#define ERR_DEV_INIT_FAIL     (ERR_DEV_GLOB | 0x0006)

/*
 * HAL AUDIO.
 */
#define ERR_HAL_AUDIO_AC97_INIT_FAIL    (ERR_HAL_AUDIO | 0x0000)
#define ERR_HAL_AUDIO_AC97_WRITE_FAIL   (ERR_HAL_AUDIO | 0x0001)
#define ERR_HAL_AUDIO_AC97_READ_FAIL    (ERR_HAL_AUDIO | 0x0002)

/*
 * HAL SD.
 */
#define ERR_HAL_FLASH_MODE_ERR     (ERR_HAL_FLASH | 0x0000)
#define ERR_HAL_FLASH_CONFIG_ERR0  (ERR_HAL_FLASH | 0x0001)
#define ERR_HAL_FLASH_CONFIG_ERR1  (ERR_HAL_FLASH | 0x0002)
#define ERR_HAL_FLASH_SELECT_ERR   (ERR_HAL_FLASH | 0x0003)

#define ERR_HAL_NAND_PORT_RDY_ERR  (ERR_HAL_NAND | 0x0000)
#define ERR_HAL_NAND_ADDR_SEND_ERR (ERR_HAL_NAND | 0x0001)
#define ERR_HAL_NAND_RDY_CHK_ERR   (ERR_HAL_NAND | 0x0002)

#define ERR_HAL_EMMC_CMD_STS_TIMEOUT (ERR_HAL_EMMC | 0x0000)

#define ERR_HAL_SD_NO_RSP          (ERR_HAL_SD | 0x0000)
#define ERR_HAL_SD_RSP_TIMEOUT     (ERR_HAL_SD | 0x0001)
#define ERR_HAL_SD_CRC7_ERR        (ERR_HAL_SD | 0x0002)
#define ERR_HAL_SD_IDLE_TIMEOUT    (ERR_HAL_SD | 0x0003)
#define ERR_HAL_SD_CMD_RSP_ERR     (ERR_HAL_SD | 0x0004)
#define ERR_HAL_SD_CMD_UNMATCH     (ERR_HAL_SD | 0x0005)
#define ERR_HAL_SD_CMD_R2_R3_ERR   (ERR_HAL_SD | 0x0006)
#define ERR_HAL_SD_DUMMY_TIMEOUT   (ERR_HAL_SD | 0x0007)
#define ERR_HAL_SD_CARD_CRC_ERR    (ERR_HAL_SD | 0x0008)

#define ERR_HAL_MS_RWADDR_ERR0     (ERR_HAL_MS | 0x0001)
#define ERR_HAL_MS_RWADDR_TIMEOUT  (ERR_HAL_MS | 0x0002)
#define ERR_HAL_MS_REGREAD_ERR0    (ERR_HAL_MS | 0x0003)
#define ERR_HAL_MS_REGREAD_ERR1    (ERR_HAL_MS | 0x0004)
#define ERR_HAL_MS_REGREAD_TIMEOUT (ERR_HAL_MS | 0x0005)
#define ERR_HAL_MS_BURREAD_ERR0    (ERR_HAL_MS | 0x0006)
#define ERR_HAL_MS_BURREAD_ERR1    (ERR_HAL_MS | 0x0007)
#define ERR_HAL_MS_BURREAD_ERR2    (ERR_HAL_MS | 0x0008)
#define ERR_HAL_MS_REGWR_ERR0      (ERR_HAL_MS | 0x0009)
#define ERR_HAL_MS_REGWR_ERR1      (ERR_HAL_MS | 0x000a)
#define ERR_HAL_MS_CMDSET_ERR0     (ERR_HAL_MS | 0x000b)
#define ERR_HAL_MS_CMDSET_ERR1     (ERR_HAL_MS | 0x000c)
#define ERR_HAL_MS_INT_ERR0        (ERR_HAL_MS | 0x000d)
#define ERR_HAL_MS_INT_ERR1        (ERR_HAL_MS | 0x000e)
#define ERR_HAL_MS_ERASE_ERR0      (ERR_HAL_MS | 0x000f)
#define ERR_HAL_MS_ERASE_ERR1      (ERR_HAL_MS | 0x0010)
#define ERR_HAL_MS_ERASE_ERR2      (ERR_HAL_MS | 0x0011)
#define ERR_HAL_MS_ERASE_ERR3      (ERR_HAL_MS | 0x0012)
#define ERR_HAL_MS_ERASE_ERR4      (ERR_HAL_MS | 0x0013)
#define ERR_HAL_MS_ERASE_ERR5      (ERR_HAL_MS | 0x0014)
#define ERR_HAL_MS_ERASE_ERR6      (ERR_HAL_MS | 0x0015)
#define ERR_HAL_MS_ERASE_ERR7      (ERR_HAL_MS | 0x0016)
#define ERR_HAL_MS_ERASE_ERR8      (ERR_HAL_MS | 0x0017)
#define ERR_HAL_MS_ERASE_ERR9      (ERR_HAL_MS | 0x0018)
#define ERR_HAL_MS_ERASE_ERR10     (ERR_HAL_MS | 0x0019)
#define ERR_HAL_MS_ERASE_ERR11     (ERR_HAL_MS | 0x001a)
#define ERR_HAL_MS_ERASE_ERR12     (ERR_HAL_MS | 0x001b)
#define ERR_HAL_MS_DMAWR_ERR0      (ERR_HAL_MS | 0x001c)
#define ERR_HAL_MS_DMAWR_ERR1      (ERR_HAL_MS | 0x001d)
#define ERR_HAL_MS_DMAWR_ERR2      (ERR_HAL_MS | 0x001e)
#define ERR_HAL_MS_DMAWR_ERR3      (ERR_HAL_MS | 0x001f)
#define ERR_HAL_MS_DMAWR_ERR4      (ERR_HAL_MS | 0x0020)
#define ERR_HAL_MS_DMAWR_ERR5      (ERR_HAL_MS | 0x0021)
#define ERR_HAL_MS_DMAWR_ERR6      (ERR_HAL_MS | 0x0022)
#define ERR_HAL_MS_DMAWR_ERR7      (ERR_HAL_MS | 0x0023)
#define ERR_HAL_MS_DMAWR_ERR8      (ERR_HAL_MS | 0x0024)
#define ERR_HAL_MS_DMAWR_ERR9      (ERR_HAL_MS | 0x0025)
#define ERR_HAL_MS_DMAWR_ERR10     (ERR_HAL_MS | 0x0026)
#define ERR_HAL_MS_DMAWR_ERR11     (ERR_HAL_MS | 0x0027)
#define ERR_HAL_MS_DMAWR_ERR12     (ERR_HAL_MS | 0x0028)
#define ERR_HAL_MS_DMAWR_ERR13     (ERR_HAL_MS | 0x0029)
#define ERR_HAL_MS_DMAWR_ERR14     (ERR_HAL_MS | 0x002a)
#define ERR_HAL_MS_DMAWR_ERR15     (ERR_HAL_MS | 0x002b)
#define ERR_HAL_MS_DMAWR_ERR16     (ERR_HAL_MS | 0x002c)
#define ERR_HAL_MS_DMAWR_ERR17     (ERR_HAL_MS | 0x002d)
#define ERR_HAL_MS_DMAWR_ERR18     (ERR_HAL_MS | 0x002e)
#define ERR_HAL_MS_DMAREAD_ERR0    (ERR_HAL_MS | 0x002f)
#define ERR_HAL_MS_DMAREAD_ERR1    (ERR_HAL_MS | 0x0030)
#define ERR_HAL_MS_DMAREAD_ERR2    (ERR_HAL_MS | 0x0031)
#define ERR_HAL_MS_DMAREAD_ERR3    (ERR_HAL_MS | 0x0032)
#define ERR_HAL_MS_DMAREAD_ERR4    (ERR_HAL_MS | 0x0033)
#define ERR_HAL_MS_DMAREAD_ERR5    (ERR_HAL_MS | 0x0034)
#define ERR_HAL_MS_DMAREAD_ERR6    (ERR_HAL_MS | 0x0035)
#define ERR_HAL_MS_DMAREAD_ERR7    (ERR_HAL_MS | 0x0036)
#define ERR_HAL_MS_DMAREAD_ERR8    (ERR_HAL_MS | 0x0037)
#define ERR_HAL_MS_DMAREAD_ERR9    (ERR_HAL_MS | 0x0038)
#define ERR_HAL_MS_DMAREAD_ERR10   (ERR_HAL_MS | 0x0039)
#define ERR_HAL_MS_DMAREAD_ERR11   (ERR_HAL_MS | 0x003a)
#define ERR_HAL_MS_DMAREAD_ERR12   (ERR_HAL_MS | 0x003b)
#define ERR_HAL_MS_DMAREAD_ERR13   (ERR_HAL_MS | 0x003c)
#define ERR_HAL_MS_DMAREAD_ERR14   (ERR_HAL_MS | 0x003d)
#define ERR_HAL_MS_DMAREAD_ERR15   (ERR_HAL_MS | 0x003e)
#define ERR_HAL_MS_DMAREAD_ERR16   (ERR_HAL_MS | 0x003f)
#define ERR_HAL_MS_DMAREAD_ERR17   (ERR_HAL_MS | 0x0040)
#define ERR_HAL_MS_RESET_ERR       (ERR_HAL_MS | 0x0041)
#define ERR_HAL_MS_ATRB_ERR0       (ERR_HAL_MS | 0x0042)
#define ERR_HAL_MS_ATRB_ERR1       (ERR_HAL_MS | 0x0043)
#define ERR_HAL_MS_ATRB_ERR2       (ERR_HAL_MS | 0x0044)
#define ERR_HAL_MS_ATRB_ERR3       (ERR_HAL_MS | 0x0045)
#define ERR_HAL_MS_ATRB_ERR4       (ERR_HAL_MS | 0x0046)
#define ERR_HAL_MS_ATRB_ERR5       (ERR_HAL_MS | 0x0047)
#define ERR_HAL_MS_ATRB_ERR6       (ERR_HAL_MS | 0x0048)
#define ERR_HAL_MS_ATRB_ERR7       (ERR_HAL_MS | 0x0049)
#define ERR_HAL_MS_BUS_ERR0        (ERR_HAL_MS | 0x004a)
#define ERR_HAL_MS_BUS_ERR1        (ERR_HAL_MS | 0x004b)
#define ERR_HAL_MS_PRO_FMT_ERR0    (ERR_HAL_MS | 0x004c)
#define ERR_HAL_MS_PRO_FMT_ERR1    (ERR_HAL_MS | 0x004d)
#define ERR_HAL_MS_PRO_FMT_ERR2    (ERR_HAL_MS | 0x004e)
#define ERR_HAL_MS_PRO_FMT_ERR3    (ERR_HAL_MS | 0x004f)
#define ERR_HAL_MS_PRO_FMT_ERR4    (ERR_HAL_MS | 0x0050)
#define ERR_HAL_MS_PRO_FMT_ERR5    (ERR_HAL_MS | 0x0051)
#define ERR_HAL_MS_PRO_FMT_ERR6    (ERR_HAL_MS | 0x0052)

/*
 * HAL disp.
 */
#define ERR_HAL_DISP_VVALID_TIMEOUT  (ERR_HAL_DISP | 0x0000)
#define ERR_HAL_DISP_VSYNC_TIMEOUT	 (ERR_HAL_DISP | 0x0001)

/*
 * HAL gprm.
 */
#define ERR_HAL_GPRM_TIMEOUT		(ERR_HAL_GPRM | 0x0000)
#define ERR_HAL_GPRM_ALLOC_FAIL		(ERR_HAL_GPRM | 0x0001)
#define ERR_HAL_GPRM_MEM			(ERR_HAL_GPRM | 0x0002)
#define ERR_HAL_GPRM_INVALID_PARAM	(ERR_HAL_GPRM | 0x0003)

/*
 * HAL jpeg.
 */
#define ERR_HAL_JPEG_ILLEGAL_PARAM  (ERR_HAL_MJPEG | 0x0000)
#define ERR_HAL_JPEG_UNKNOWN_JOB    (ERR_HAL_MJPEG | 0x0001)
#define ERR_HAL_JPEG_UNKNOWN_IRQ    (ERR_HAL_MJPEG | 0x0002)
#define ERR_HAL_JPEG_TIMEOUT        (ERR_HAL_MJPEG | 0x0003)
#define ERR_HAL_JPEG_REENTERY       (ERR_HAL_MJPEG | 0x0004)

/*
 * diskette: SD.
 */
#define ERR_SD_IDENTIFY_TIMEOUT      (ERR_SD | 0x0000)
#define ERR_SD_DETECT_FAIL           (ERR_SD | 0x0001)
#define ERR_SD_LOCKED                (ERR_SD | 0x0002)
#define ERR_SD_BUS_PATTERN_FAIL      (ERR_SD | 0x0003)
#define ERR_SD_IDENTIFY_CMD41        (ERR_SD | 0x0004)
#define ERR_SD_IDENTIFY_CMD1         (ERR_SD | 0x0005)
#define ERR_SD_IDENTIFY_CMD2         (ERR_SD | 0x0006)
#define ERR_SD_IDENTIFY_CMD3         (ERR_SD | 0x0007)
#define ERR_SD_IDENTIFY_CMD9         (ERR_SD | 0x0008)
#define ERR_SD_DETECT_CMD16          (ERR_SD | 0x0009)
#define ERR_SD_STATE_TIMEOUT         (ERR_SD | 0x000a)
#define ERR_SD_READ_ERR0             (ERR_SD | 0x000b)
#define ERR_SD_READ_ERR1             (ERR_SD | 0x000c)
#define ERR_SD_READ_ERR2             (ERR_SD | 0x000d)
#define ERR_SD_READ_ERR3             (ERR_SD | 0x000e)
#define ERR_SD_READ_ERR4             (ERR_SD | 0x000f)
#define ERR_SD_READ_CRC16            (ERR_SD | 0x0010)
#define ERR_SD_READ_ERR5             (ERR_SD | 0x0011)
#define ERR_SD_WRITE_ERR0            (ERR_SD | 0x0012)
#define ERR_SD_WRITE_ERR1            (ERR_SD | 0x0013)
#define ERR_SD_WRITE_ERR2            (ERR_SD | 0x0014)
#define ERR_SD_WRITE_ERR3            (ERR_SD | 0x0015)
#define ERR_SD_WRITE_ERR4            (ERR_SD | 0x0016)
#define ERR_SD_WRITE_ERR5            (ERR_SD | 0x0017)
#define ERR_SD_WRITE_ERR6            (ERR_SD | 0x0018)
#define ERR_SD_WRITE_ERR7            (ERR_SD | 0x0019)
#define ERR_SD_WRITE_ERR8            (ERR_SD | 0x001a)
#define ERR_SD_TRANSTATE_TIMEOUT     (ERR_SD | 0x001b)
#define ERR_SD_RDYBIT_TIMEOUT        (ERR_SD | 0x001c)
#define ERR_SD_SWRITE_ERR0           (ERR_SD | 0x001d)
#define ERR_SD_SWRITE_ERR1           (ERR_SD | 0x001e)
#define ERR_SD_SWRITE_ERR2           (ERR_SD | 0x001f)
#define ERR_SD_SWRITE_ERR3           (ERR_SD | 0x0020)
#define ERR_SD_SWRITE_ERR4           (ERR_SD | 0x0021)
#define ERR_SD_SWRITE_ERR5           (ERR_SD | 0x0022)
#define ERR_SD_SWRITE_ERR6           (ERR_SD | 0x0023)
#define ERR_SD_SREAD_ERR0            (ERR_SD | 0x0024)
#define ERR_SD_SREAD_ERR1            (ERR_SD | 0x0025)
#define ERR_SD_SREAD_ERR2            (ERR_SD | 0x0026)
#define ERR_SD_SREAD_ERR3            (ERR_SD | 0x0027)
#define ERR_SD_SREAD_ERR4            (ERR_SD | 0x0028)
#define ERR_SD_STATUS_ERR0           (ERR_SD | 0x0029)
#define ERR_SD_STATUS_ERR1           (ERR_SD | 0x0029)
#define ERR_SD_BUSWIDTH_ERR0         (ERR_SD | 0x002a)
#define ERR_SD_BUSWIDTH_ERR1         (ERR_SD | 0x002b)
#define ERR_SD_DET_VFS_FAIL          (ERR_SD | 0x002c)
#define ERR_SD_BUF_INIT_ERR          (ERR_SD | 0x002d)
#define ERR_SDDET_BUF_INIT_ERR       (ERR_SD | 0x002e)
#define ERR_SD_STATE_CHK_ERR0        (ERR_SD | 0x002f)
#define ERR_SD_STATE_CHK_ERR1        (ERR_SD | 0x0030)
#define ERR_SD_MOUNT_ERR0            (ERR_SD | 0x0031)

/*
 * diskette: CF.
 */
#define ERR_CF_IDE_READ_ERR0         (ERR_CF | 0x0000)
#define ERR_CF_IDE_READ_ERR1         (ERR_CF | 0x0001)
#define ERR_CF_IDE_READ_ERR2         (ERR_CF | 0x0002)
#define ERR_CF_IDE_READ_ERR3         (ERR_CF | 0x0003)
#define ERR_CF_IDE_READ_ERR4         (ERR_CF | 0x0004)
#define ERR_CF_IDE_READ_ERR5         (ERR_CF | 0x0005)
#define ERR_CF_IDE_READ_ERR6         (ERR_CF | 0x0006)
#define ERR_CF_IDE_READ_ERR7         (ERR_CF | 0x0007)
#define ERR_CF_IDE_READ_ERR8         (ERR_CF | 0x0008)
#define ERR_CF_IDE_READ_ERR9         (ERR_CF | 0x0009)
#define ERR_CF_MEM_READ_ERR0         (ERR_CF | 0x000a)
#define ERR_CF_MEM_READ_ERR1         (ERR_CF | 0x000b)
#define ERR_CF_MEM_READ_ERR2         (ERR_CF | 0x000c)
#define ERR_CF_MEM_READ_ERR3         (ERR_CF | 0x000d)
#define ERR_CF_MEM_READ_ERR4         (ERR_CF | 0x000e)
#define ERR_CF_MEM_READ_ERR5         (ERR_CF | 0x000f)
#define ERR_CF_MEM_READ_ERR6         (ERR_CF | 0x0010)
#define ERR_CF_MEM_READ_ERR7         (ERR_CF | 0x0011)
#define ERR_CF_IDE_WRITE_ERR0        (ERR_CF | 0x0012)
#define ERR_CF_IDE_WRITE_ERR1        (ERR_CF | 0x0013)
#define ERR_CF_IDE_WRITE_ERR2        (ERR_CF | 0x0014)
#define ERR_CF_IDE_WRITE_ERR3        (ERR_CF | 0x0015)
#define ERR_CF_IDE_WRITE_ERR4        (ERR_CF | 0x0016)
#define ERR_CF_IDE_WRITE_ERR5        (ERR_CF | 0x0017)
#define ERR_CF_IDE_WRITE_ERR6        (ERR_CF | 0x0018)
#define ERR_CF_MEM_WRITE_ERR0        (ERR_CF | 0x0019)
#define ERR_CF_MEM_WRITE_ERR1        (ERR_CF | 0x001a)
#define ERR_CF_MEM_WRITE_ERR2        (ERR_CF | 0x001b)
#define ERR_CF_MEM_WRITE_ERR3        (ERR_CF | 0x001c)
#define ERR_CF_MEM_WRITE_ERR4        (ERR_CF | 0x001d)
#define ERR_CF_MEM_WRITE_ERR5        (ERR_CF | 0x001e)
#define ERR_CF_MEM_WRITE_ERR6        (ERR_CF | 0x001f)
#define ERR_CF_MEM_WRITE_ERR7        (ERR_CF | 0x0020)
#define ERR_CF_MEM_DET_BUF_ERR       (ERR_CF | 0x0021)
#define ERR_CF_MEM_DET_FAIL          (ERR_CF | 0x0022)
#define ERR_CF_MEM_DET_VFS_FAIL      (ERR_CF | 0x0023)
#define ERR_CF_IDE_DET_BUF_ERR       (ERR_CF | 0x0024)
#define ERR_CF_IDE_DET_FAIL          (ERR_CF | 0x0025)
#define ERR_CF_IDE_DET_VFS_FAIL      (ERR_CF | 0x0026)
#define ERR_CF_POLL_TIMEOUT          (ERR_CF | 0x0027)
#define ERR_CF_IDE_CMDSEND_HWRDY     (ERR_CF | 0x0028)
#define ERR_CF_IDE_CMDSEND_BUSY      (ERR_CF | 0x0029)
#define ERR_CF_IDE_CMDSEND_RDY       (ERR_CF | 0x002a)
#define ERR_CF_MEM_CMDSEND_HWRDY     (ERR_CF | 0x002b)
#define ERR_CF_MEM_CMDSEND_BUSY      (ERR_CF | 0x002c)
#define ERR_CF_MEM_CMDSEND_RDY       (ERR_CF | 0x002d)
#define ERR_CF_IDE_CMDREAD_HWRDY     (ERR_CF | 0x002e)
#define ERR_CF_IDE_CMDREAD_BUSY      (ERR_CF | 0x002f)
#define ERR_CF_IDE_CMDREAD_RDY       (ERR_CF | 0x0030)
#define ERR_CF_IDE_CMDREAD_ERR       (ERR_CF | 0x0031)
#define ERR_CF_MEM_CMDREAD_HWRDY     (ERR_CF | 0x0032)
#define ERR_CF_MEM_CMDREAD_BUSY      (ERR_CF | 0x0033)
#define ERR_CF_MEM_CMDREAD_RDY       (ERR_CF | 0x0034)
#define ERR_CF_MEM_CMDREAD_ERR       (ERR_CF | 0x0035)
#define ERR_CF_IDE_RDYCHK_HWRDY      (ERR_CF | 0x0036)
#define ERR_CF_IDE_RDYCHK_BUSY       (ERR_CF | 0x0037)
#define ERR_CF_IDE_RDYCHK_RDY        (ERR_CF | 0x0038)
#define ERR_CF_MEM_RDYCHK_HWRDY      (ERR_CF | 0x0039)
#define ERR_CF_MEM_RDYCHK_BUSY       (ERR_CF | 0x003a)
#define ERR_CF_MEM_RDYCHK_RDY        (ERR_CF | 0x003b)
#define ERR_CF_BUF_READ_ERR0         (ERR_CF | 0x003c)
#define ERR_CF_IDE_BUF_READ_ERR0     (ERR_CF | 0x003d)
#define ERR_CF_IDE_BUF_READ_ERR1     (ERR_CF | 0x003e)
#define ERR_CF_IDE_BUF_READ_ERR2     (ERR_CF | 0x003f)
#define ERR_CF_IDE_BUF_READ_ERR3     (ERR_CF | 0x0040)
#define ERR_CF_IDE_BUF_READ_ERR4     (ERR_CF | 0x0041)
#define ERR_CF_IDE_BUF_READ_ERR5     (ERR_CF | 0x0042)
#define ERR_CF_IDE_BUF_READ_ERR6     (ERR_CF | 0x0043)
#define ERR_CF_IDE_BUF_READ_ERR7     (ERR_CF | 0x0044)
#define ERR_CF_IDE_BUF_READ_ERR8     (ERR_CF | 0x0045)
#define ERR_CF_IDE_BUF_READ_ERR9     (ERR_CF | 0x0046)
#define ERR_CF_IDE_BUF_READ_ERR10    (ERR_CF | 0x0047)
#define ERR_CF_IDE_BUF_READ_ERR11    (ERR_CF | 0x0048)
#define ERR_CF_MEM_BUF_READ_ERR0     (ERR_CF | 0x0049)
#define ERR_CF_MEM_BUF_READ_ERR1     (ERR_CF | 0x004a)
#define ERR_CF_MEM_BUF_READ_ERR2     (ERR_CF | 0x004b)
#define ERR_CF_MEM_BUF_READ_ERR3     (ERR_CF | 0x004c)
#define ERR_CF_MEM_BUF_READ_ERR4     (ERR_CF | 0x004d)
#define ERR_CF_MEM_BUF_READ_ERR5     (ERR_CF | 0x004e)
#define ERR_CF_MEM_BUF_READ_ERR6     (ERR_CF | 0x004f)
#define ERR_CF_MEM_BUF_READ_ERR7     (ERR_CF | 0x0050)
#define ERR_CF_BUF_ENABLE_BUF_ERR    (ERR_CF | 0x0051)
#define ERR_CF_IDE_DET_UNKNOW_DEVICE (ERR_CF | 0x0052)
#define ERR_CF_MEM_DET_UNKNOW_DEVICE (ERR_CF | 0x0053)
#define ERR_CF_RSV_READ_ERR          (ERR_CF | 0x0054)
#define ERR_CF_RSV_WRITE_ERR         (ERR_CF | 0x0055)
#define ERR_CF_RSV_DET_ERR           (ERR_CF | 0x0056)
#define ERR_CF_RSV_DET_BUF_ERR       (ERR_CF | 0x0057)
#define ERR_CF_VFS_MOUNT_ERR0        (ERR_CF | 0x0058)
#define ERR_CF_BUF_INIT_ERR          (ERR_CF | 0x0059)

/*
 * diskette: NAND / SMC / XD.
 */
#define ERR_NAND_ID_DET_FAIL         (ERR_NAND | 0x0000)
#define ERR_NAND_DET_SML_VLDBLK_ERR  (ERR_NAND | 0x0001)
#define ERR_NAND_DET_BIG_VLDBLK_ERR  (ERR_NAND | 0x0002)
#define ERR_NAND_DET_SML_VFS_FAIL    (ERR_NAND | 0x0003)
#define ERR_NAND_DET_BIG_VFS_FAIL    (ERR_NAND | 0x0004)
#define ERR_NAND_READ_ONLY           (ERR_NAND | 0x0005)
#define ERR_NAND_SML_READ_ERR        (ERR_NAND | 0x0006)
#define ERR_NAND_SML_WRITE_ERR       (ERR_NAND | 0x0007)
#define ERR_NAND_FMT_ERR             (ERR_NAND | 0x0008)
#define ERR_NAND_RSV_CHKSUM_ERR      (ERR_NAND | 0x0009)
#define ERR_NAND_RSV_MAP_ERR0        (ERR_NAND | 0x000a)
#define ERR_NAND_RSV_MAP_ERR1        (ERR_NAND | 0x000b)
#define ERR_NAND_RSV_READ_ERR0       (ERR_NAND | 0x000c)
#define ERR_NAND_RSV_READ_ERR1       (ERR_NAND | 0x000d)
#define ERR_NAND_RSV_READ_ERR2       (ERR_NAND | 0x000e)
#define ERR_NAND_RSV_READ_ERR3       (ERR_NAND | 0x000f)
#define ERR_NAND_RSV_READ_ERR4       (ERR_NAND | 0x0010)
#define ERR_NAND_RSV_WRITE_ERR0      (ERR_NAND | 0x0011)
#define ERR_NAND_RSV_WRITE_ERR1      (ERR_NAND | 0x0012)
#define ERR_NAND_RSV_WRITE_ERR2      (ERR_NAND | 0x0013)
#define ERR_NAND_RSV_WRITE_ERR3      (ERR_NAND | 0x0014)
#define ERR_NAND_RSV_WRITE_ERR4      (ERR_NAND | 0x0015)
#define ERR_NAND_RSV_WRITE_ERR5      (ERR_NAND | 0x0016)
#define ERR_NAND_RSV_WRITE_ERR6      (ERR_NAND | 0x0017)
#define ERR_NAND_RSV_WRITE_ERR7      (ERR_NAND | 0x0018)
#define ERR_NAND_RSV_WRITE_ERR8      (ERR_NAND | 0x0019)
#define ERR_NAND_RSV_WRITE_ERR9      (ERR_NAND | 0x001a)
#define ERR_NAND_RSV_WRITE_ERR10     (ERR_NAND | 0x001b)
#define ERR_NAND_RSV_WRITE_ERR11     (ERR_NAND | 0x001c)
#define ERR_NAND_RSV_WRITE_ERR12     (ERR_NAND | 0x001d)
#define ERR_NAND_RSV_WRITE_ERR13     (ERR_NAND | 0x001e)
#define ERR_NAND_RSV_HEADWR_ERR0     (ERR_NAND | 0x001f)
#define ERR_NAND_RSV_HEADWR_ERR1     (ERR_NAND | 0x0020)
#define ERR_NAND_RSV_HEADWR_ERR2     (ERR_NAND | 0x0021)
#define ERR_NAND_RSV_HEADWR_ERR3     (ERR_NAND | 0x0022)
#define ERR_NAND_RSV_HEADWR_ERR4     (ERR_NAND | 0x0023)
#define ERR_NAND_RSV_HEADWR_ERR5     (ERR_NAND | 0x0024)
#define ERR_NAND_RSV_HEADWR_ERR6     (ERR_NAND | 0x0025)
#define ERR_NAND_RSV_HEADWR_ERR7     (ERR_NAND | 0x0026)
#define ERR_NAND_ERASE_ERR0          (ERR_NAND | 0x0027)
#define ERR_NAND_ERASE_ERR1          (ERR_NAND | 0x0028)
#define ERR_NAND_ERASE_ERR2          (ERR_NAND | 0x0029)
#define ERR_NAND_ERASE_ERR3          (ERR_NAND | 0x002a)
#define ERR_NAND_ERASE_ERR4          (ERR_NAND | 0x002b)
#define ERR_NAND_ERASE_ERR5          (ERR_NAND | 0x002c)
#define ERR_NAND_RSV_HEADRD_ERR      (ERR_NAND | 0x002d)
#define ERR_NAND_FWPG_ERR0           (ERR_NAND | 0x002e)
#define ERR_NAND_FWPG_ERR1           (ERR_NAND | 0x002f)
#define ERR_NAND_FWPG_ERR2           (ERR_NAND | 0x0030)
#define ERR_NAND_FWPG_ERR3           (ERR_NAND | 0x0031)
#define ERR_NAND_FWPG_ERR4           (ERR_NAND | 0x0032)
#define ERR_NAND_FWPG_ERR5           (ERR_NAND | 0x0033)
#define ERR_NAND_FWPG_ERR6           (ERR_NAND | 0x0034)
#define ERR_NAND_FWPG_ERR7           (ERR_NAND | 0x0035)
#define ERR_NAND_RSV_EPTPG_ERR0      (ERR_NAND | 0x0036)
#define ERR_NAND_RSV_EPTPG_ERR1      (ERR_NAND | 0x0037)
#define ERR_NAND_RSV_ERASE_ERR0      (ERR_NAND | 0x0038)
#define ERR_NAND_RSV_ERASE_ERR1      (ERR_NAND | 0x0039)
#define ERR_NAND_DEVA_READ_ERR0      (ERR_NAND | 0x003a)
#define ERR_NAND_DEVA_WRITE_ERR0     (ERR_NAND | 0x003b)
#define ERR_NAND_DEVB_READ_ERR0      (ERR_NAND | 0x003c)
#define ERR_NAND_DEVB_WRITE_ERR0     (ERR_NAND | 0x003d)
#define ERR_NAND_RSVA_DET_ERR0       (ERR_NAND | 0x003e)
#define ERR_NAND_RSVB_DET_ERR0       (ERR_NAND | 0x003f)
#define ERR_NAND_ADV_READ_ECC_ERR    (ERR_NAND | 0x0040)
#define ERR_NAND_ADV_WRITE_STS_ERR   (ERR_NAND | 0x0041)
#define ERR_NAND_ADV_READ_ERR0       (ERR_NAND | 0x0042)
#define ERR_NAND_ADV_READ_ERR1       (ERR_NAND | 0x0043)
#define ERR_NAND_ADV_READ_ERR2       (ERR_NAND | 0x0044)
#define ERR_NAND_ADV_READ_ERR3       (ERR_NAND | 0x0045)
#define ERR_NAND_ADV_READ_ERR4       (ERR_NAND | 0x0046)
#define ERR_NAND_ADV_WRITE_ERR0      (ERR_NAND | 0x0047)
#define ERR_NAND_ADV_WRITE_ERR1      (ERR_NAND | 0x0048)
#define ERR_NAND_ADV_ERASE_STS_ERR   (ERR_NAND | 0x0049)
#define ERR_NAND_ADV_FMT_ERR         (ERR_NAND | 0x004a)
#define ERR_NAND_ADV_CPY_ERR0        (ERR_NAND | 0x004b)
#define ERR_NAND_ADV_CPY_ERR1        (ERR_NAND | 0x004c)
#define ERR_NAND_CPY_ERR0            (ERR_NAND | 0x004d)
#define ERR_NAND_CPY_ERR1            (ERR_NAND | 0x004e)
#define ERR_NAND_CPY_ERR2            (ERR_NAND | 0x004f)
#define ERR_NAND_ADV_MAK_ERR         (ERR_NAND | 0x0050)
#define ERR_NAND_BUF_FAIL            (ERR_NAND | 0x0051)
#define ERR_NAND_MLC_TYPE_UNKNOWN    (ERR_NAND | 0x0052)
#define ERR_NAND_MLC_READ_ECC        (ERR_NAND | 0x0053)
#define ERR_NAND_MLC_WRITE_ERR       (ERR_NAND | 0x0054)
#define ERR_NAND_MLC_REDU_BUF_ERR    (ERR_NAND | 0x0055)
#define ERR_NAND_MLC_ERASE_ERR       (ERR_NAND | 0x0056)
#define ERR_NAND_MLC_MAK_ERR0        (ERR_NAND | 0x0057)
#define ERR_NAND_MLC_MAK_ERR1        (ERR_NAND | 0x0058)
#define ERR_NAND_MLC_MAK_ERR2        (ERR_NAND | 0x0059)
#define ERR_NAND_MLC_ECC_BUF_ERR     (ERR_NAND | 0x005a)
#define ERR_NAND_MLC_ECC_ERR0        (ERR_NAND | 0x005b)
#define ERR_NAND_MLC_ECC_UNCORRECT   (ERR_NAND | 0x005c)
#define ERR_NAND_SMC_DET_ERR0        (ERR_NAND | 0x005d)
#define ERR_NAND_XD_DET_ERR0         (ERR_NAND | 0x005e)
#define ERR_NAND_XD_DET_ERR1         (ERR_NAND | 0x005f)
#define ERR_NAND_SMC_DET_VFS_ERR0    (ERR_NAND | 0x005f)
#define ERR_NAND_SMC_DET_VFS_ERR1    (ERR_NAND | 0x005f)
#define ERR_NAND_LTP_ERR0            (ERR_NAND | 0x0060)
#define ERR_NAND_FREE_BLK_ERR0       (ERR_NAND | 0x0061)
#define ERR_NAND_VLD_BLK_ERR0        (ERR_NAND | 0x0062)
#define ERR_NAND_STS_CHK_ERR0        (ERR_NAND | 0x0063)
#define ERR_NAND_TBL_MAK_ERR0        (ERR_NAND | 0x0064)
#define ERR_NAND_TBL_WRITE_ERR0      (ERR_NAND | 0x0065)
#define ERR_NAND_ECC_UNCORRECT       (ERR_NAND | 0x0066)
#define ERR_NAND_TYPE_UNKNOWN        (ERR_NAND | 0x0067)
#define ERR_NAND_SPEC_TYPE_ERR0      (ERR_NAND | 0x0068)
#define ERR_NAND_SPEC_TYPE_ERR1      (ERR_NAND | 0x0069)
#define ERR_NAND_SPEC_SORT_ERR0      (ERR_NAND | 0x006a)
#define ERR_NAND_TBL_REFILL_ERR0     (ERR_NAND | 0x006b)
#define ERR_NAND_TBL_REFILL_ERR1     (ERR_NAND | 0x006c)
#define ERR_NAND_BLK_ADDR_ERR0       (ERR_NAND | 0x006d)
#define ERR_NAND_BLK_ADDR_ERR1       (ERR_NAND | 0x006e)
#define ERR_NAND_BLK_ADDR_ERR2       (ERR_NAND | 0x006f)
#define ERR_NAND_BLK_ADDR_ERR3       (ERR_NAND | 0x0070)
#define ERR_NAND_BLK_ADDR_ERR4       (ERR_NAND | 0x0071)
#define ERR_NAND_BLK_ADDR_ERR5       (ERR_NAND | 0x0072)
#define ERR_NAND_BLK_ADDR_ERR6       (ERR_NAND | 0x0073)
#define ERR_SMC_BUF_FAIL             (ERR_NAND | 0x0074)
#define ERR_SMC_FMT_ERR0             (ERR_NAND | 0x0075)
#define ERR_SMC_FMT_ERR1             (ERR_NAND | 0x0076)
#define ERR_SMC_FMT_ERR2             (ERR_NAND | 0x0077)
#define ERR_SMC_FMT_ERR3             (ERR_NAND | 0x0078)
#define ERR_SMC_DMA_READ_ECC         (ERR_NAND | 0x0079)
#define ERR_SMC_BLK_ERASE_ERR        (ERR_NAND | 0x007a)
#define ERR_SMC_DMA_WRITE_ERR        (ERR_NAND | 0x007b)
#define ERR_SMC_READ_ERR0            (ERR_NAND | 0x007c)
#define ERR_SMC_READ_ERR1            (ERR_NAND | 0x007d)
#define ERR_SMC_READ_ERR2            (ERR_NAND | 0x007e)
#define ERR_SMC_WRITE_ERR0           (ERR_NAND | 0x007f)
#define ERR_SMC_WRITE_ERR1           (ERR_NAND | 0x0080)
#define ERR_SMC_WRITE_ERR2           (ERR_NAND | 0x0081)
#define ERR_SMC_WRITE_ERR3           (ERR_NAND | 0x0082)
#define ERR_SMC_WRITE_ERR4           (ERR_NAND | 0x0083)
#define ERR_SMC_WRITE_ERR5           (ERR_NAND | 0x0084)
#define ERR_SMC_WRITE_ERR6           (ERR_NAND | 0x0085)
#define ERR_SMC_WRITE_ERR7           (ERR_NAND | 0x0086)
#define ERR_SMC_WRITE_ERR8           (ERR_NAND | 0x0087)
#define ERR_SMC_WRITE_ERR9           (ERR_NAND | 0x0088)
#define ERR_SMC_WRITE_ERR10          (ERR_NAND | 0x0089)
#define ERR_SMC_WRITE_ERR11          (ERR_NAND | 0x008a)
#define ERR_SMC_WRITE_ERR12          (ERR_NAND | 0x008b)
#define ERR_SMC_DEF_MARK_ERR         (ERR_NAND | 0x008c)
#define ERR_SMC_ECC_ERR              (ERR_NAND | 0x008d)
#define ERR_XD_NO_CARD               (ERR_NAND | 0x008e)
#define ERR_NAND_RSV_PARSE_ERR0      (ERR_NAND | 0x008f)
#define ERR_NAND_RSV_PARSE_ERR1      (ERR_NAND | 0x0090)
#define ERR_NAND_RSVA_DET_SIZE_NULL  (ERR_NAND | 0x0091)
#define ERR_NAND_RSVB_DET_SIZE_NULL  (ERR_NAND | 0x0092)
#define ERR_NAND_MOUNT_ERR0          (ERR_NAND | 0x0093)
#define ERR_NAND_RSV_HDR_ALLO_ERR0   (ERR_NAND | 0x0093)
#define ERR_NAND_FW_READ_ERR0        (ERR_NAND | 0x0094)
#define ERR_NAND_RSV_HDR_MODIFY_ERR0 (ERR_NAND | 0x0095)
#define ERR_NAND_RSV_HDR_MODIFY_ERR1 (ERR_NAND | 0x0096)
#define ERR_NAND_RSV_HDR_MODIFY_ERR2 (ERR_NAND | 0x0097)
#define ERR_NAND_RSV_HDR_MODIFY_ERR3 (ERR_NAND | 0x0098)
#define ERR_NAND_RSV_HDR_MODIFY_ERR4 (ERR_NAND | 0x0099)
#define ERR_NAND_RSV_HDR_MODIFY_ERR5 (ERR_NAND | 0x009a)
#define ERR_NAND_RSV_HDR_MODIFY_ERR6 (ERR_NAND | 0x009b)

/*
 * diskette: MS / MSPro.
 */
#define ERR_MS_BOOT_RES_BUF_ERR      (ERR_MS | 0x0001)
#define ERR_MS_BOOT_RES_ERR0         (ERR_MS | 0x0002)
#define ERR_MS_BOOT_RES_ERR1         (ERR_MS | 0x0003)
#define ERR_MS_BOOT_RES_ERR2         (ERR_MS | 0x0004)
#define ERR_MS_BOOT_RES_ERR3         (ERR_MS | 0x0005)
#define ERR_MS_BOOT_RES_ERR4         (ERR_MS | 0x0006)
#define ERR_MS_BOOT_RES_ERR5         (ERR_MS | 0x0007)
#define ERR_MS_BOOT_RES_ERR6         (ERR_MS | 0x0008)
#define ERR_MS_BUF_ALLOC_FAIL        (ERR_MS | 0x0009)
#define ERR_MS_IDENTIFY_TYPE_FAIL    (ERR_MS | 0x000a)
#define ERR_MS_IDENTIFY_NO_TYPE      (ERR_MS | 0x000b)
#define ERR_MS_DET_NO_CARD           (ERR_MS | 0x000c)
#define ERR_MS_DET_STS_READ_FAIL     (ERR_MS | 0x000d)
#define ERR_MS_DET_FUNC_FAIL         (ERR_MS | 0x000e)
#define ERR_MS_DET_VFS_FAIL          (ERR_MS | 0x000f)
#define ERR_MS_WRITE_PROTECT         (ERR_MS | 0x0010)
#define ERR_MS_WRITE_ERR0            (ERR_MS | 0x0011)
#define ERR_MS_WRITE_ERR1            (ERR_MS | 0x0012)
#define ERR_MS_WRITE_ERR2            (ERR_MS | 0x0013)
#define ERR_MS_WRITE_ERR3            (ERR_MS | 0x0014)
#define ERR_MS_WRITE_ERR4            (ERR_MS | 0x0015)
#define ERR_MS_WRITE_ERR5            (ERR_MS | 0x0016)
#define ERR_MS_WRITE_ERR6            (ERR_MS | 0x0017)
#define ERR_MS_WRITE_ERR7            (ERR_MS | 0x0018)
#define ERR_MS_WRITE_ERR8            (ERR_MS | 0x0019)
#define ERR_MS_WRITE_ERR9            (ERR_MS | 0x0020)
#define ERR_MS_WRITE_ERR10           (ERR_MS | 0x0021)
#define ERR_MS_WRITE_ERR11           (ERR_MS | 0x0022)
#define ERR_MS_WRITE_ERR12           (ERR_MS | 0x0023)
#define ERR_MS_WRITE_ERR13           (ERR_MS | 0x0024)
#define ERR_MS_WRITE_ERR14           (ERR_MS | 0x0025)
#define ERR_MS_WRITE_ERR15           (ERR_MS | 0x0026)
#define ERR_MS_WRITE_ERR16           (ERR_MS | 0x0027)
#define ERR_MSPRO_WRITE_ERR          (ERR_MS | 0x0028)
#define ERR_MS_READ_ERR0             (ERR_MS | 0x0029)
#define ERR_MS_READ_ERR1             (ERR_MS | 0x002a)
#define ERR_MS_READ_ERR2             (ERR_MS | 0x002b)
#define ERR_MS_READ_ERR3             (ERR_MS | 0x002c)
#define ERR_MS_READ_ERR4             (ERR_MS | 0x002d)
#define ERR_MS_READ_ERR5             (ERR_MS | 0x002e)
#define ERR_MSPRO_READ_ERR           (ERR_MS | 0x002f)
#define ERR_MS_BOOT_DET_NO_TYPE      (ERR_MS | 0x0030)
#define ERR_MS_BOOT_DET_ERR0         (ERR_MS | 0x0031)
#define ERR_MS_BOOT_DET_ERR1         (ERR_MS | 0x0032)
#define ERR_MS_BOOT_DET_ERR2         (ERR_MS | 0x0033)
#define ERR_MS_BOOT_DET_ERR3         (ERR_MS | 0x0034)
#define ERR_MS_TBL_MAK_ERR0          (ERR_MS | 0x0035)
#define ERR_MS_TBL_MAK_ERR1          (ERR_MS | 0x0036)
#define ERR_MS_TBL_MAK_ERR2          (ERR_MS | 0x0037)
#define ERR_MS_TBL_MAK_ERR3          (ERR_MS | 0x0038)
#define ERR_MS_TBL_MAK_ERR4          (ERR_MS | 0x0039)
#define ERR_MS_TBL_MAK_ERR5          (ERR_MS | 0x003a)
#define ERR_MS_TBL_MAK_ERR6          (ERR_MS | 0x003b)
#define ERR_MS_TBL_MAK_ERR7          (ERR_MS | 0x003c)
#define ERR_MS_TBL_MAK_ERR8          (ERR_MS | 0x003d)
#define ERR_MS_FREE_BLK_ERR0         (ERR_MS | 0x003e)
#define ERR_MS_BAD_MAK_ERR0          (ERR_MS | 0x003f)
#define ERR_MS_BAD_MAK_ERR1          (ERR_MS | 0x0040)
#define ERR_MS_BAD_MAK_ERR2          (ERR_MS | 0x0041)
#define ERR_MS_BAD_MAK_ERR3          (ERR_MS | 0x0042)
#define ERR_MS_BAD_MAK_ERR4          (ERR_MS | 0x0043)
#define ERR_MS_BAD_MAK_ERR5          (ERR_MS | 0x0044)
#define ERR_MS_BAD_MAK_ERR6          (ERR_MS | 0x0045)
#define ERR_MSPRO_BUS_ERR            (ERR_MS | 0x0046)
#define ERR_MSPRO_ATRB0_ERR          (ERR_MS | 0x0047)
#define ERR_MSPRO_ATRB1_ERR          (ERR_MS | 0x0048)
#define ERR_MSPRO_INFO_ERR           (ERR_MS | 0x0049)
#define ERR_MSPRO_TYPE_ERR           (ERR_MS | 0x004a)
#define ERR_MSPRO_INFO_SIG_ERR       (ERR_MS | 0x004b)
#define ERR_MSPRO_INFO_ERR0          (ERR_MS | 0x004c)
#define ERR_MSPRO_INFO_ERR1          (ERR_MS | 0x004d)
#define ERR_MSPRO_INFO_ERR2          (ERR_MS | 0x004e)
#define ERR_MSPRO_INFO_ERR3          (ERR_MS | 0x004f)
#define ERR_MS_FMT_WRITE_PROTECT     (ERR_MS | 0x0050)
#define ERR_MS_PGREAD_ERR0           (ERR_MS | 0x0051)
#define ERR_MS_PGREAD_ERR1           (ERR_MS | 0x0052)
#define ERR_MS_PGREAD_ERR2           (ERR_MS | 0x0053)
#define ERR_MS_PGWRITE_ERR0          (ERR_MS | 0x0054)
#define ERR_MS_PGWRITE_ERR1          (ERR_MS | 0x0055)
#define ERR_MS_PGWRITE_ERR2          (ERR_MS | 0x0056)
#define ERR_MS_PGWRITE_ERR3          (ERR_MS | 0x0057)
#define ERR_MS_EXT_READ_ERR0         (ERR_MS | 0x0058)
#define ERR_MS_EXT_READ_ERR1         (ERR_MS | 0x0059)
#define ERR_MS_EXT_READ_ERR2         (ERR_MS | 0x005a)
#define ERR_MS_EXT_READ_ERR3         (ERR_MS | 0x005b)
#define ERR_MS_EXT_READ_ERR4         (ERR_MS | 0x005c)
#define ERR_MS_EXT_READ_ERR5         (ERR_MS | 0x005d)
#define ERR_MS_EXT_READ_ERR6         (ERR_MS | 0x005e)
#define ERR_MS_EXT_READ_ERR7         (ERR_MS | 0x005f)
#define ERR_MS_EXT_READ_ERR8         (ERR_MS | 0x0060)
#define ERR_MS_EXT_READ_ERR9         (ERR_MS | 0x0061)
#define ERR_MS_EXT_WRITE_ERR0        (ERR_MS | 0x0062)
#define ERR_MS_EXT_WRITE_ERR1        (ERR_MS | 0x0063)
#define ERR_MS_EXT_WRITE_ERR2        (ERR_MS | 0x0064)
#define ERR_MS_EXT_WRITE_ERR3        (ERR_MS | 0x0065)
#define ERR_MS_EXT_WRITE_ERR4        (ERR_MS | 0x0066)
#define ERR_MS_EXT_WRITE_ERR5        (ERR_MS | 0x0067)
#define ERR_MS_EXT_WRITE_ERR6        (ERR_MS | 0x0068)
#define ERR_MS_EXT_WRITE_ERR7        (ERR_MS | 0x0069)
#define ERR_MS_EXT_WRITE_ERR8        (ERR_MS | 0x006a)
#define ERR_MS_READ_ECCCHK_ERR0      (ERR_MS | 0x006b)
#define ERR_MS_READ_ECCCHK_ERR1      (ERR_MS | 0x006c)
#define ERR_MS_READ_ECCCHK_ERR2      (ERR_MS | 0x006d)
#define ERR_MS_READ_ECCCHK_ERR3      (ERR_MS | 0x006e)
#define ERR_MS_READ_ECCCHK_ERR4      (ERR_MS | 0x006f)
#define ERR_MS_READ_ECCCHK_ERR5      (ERR_MS | 0x0070)
#define ERR_MS_READ_ECCCHK_ERR6      (ERR_MS | 0x0071)
#define ERR_MS_READ_ECCCHK_ERR7      (ERR_MS | 0x0072)
#define ERR_MS_READ_ECCCHK_ERR8      (ERR_MS | 0x0073)
#define ERR_MS_POLL_FAIL             (ERR_MS | 0x0074)
#define ERR_MS_WRITE_ERR17           (ERR_MS | 0x0075)
#define ERR_MS_MOUNT_ERR0            (ERR_MS | 0x0076)

/*
 * diskette: Logical Drive.
 */
#define ERR_DISK_DRIVE_GET              (ERR_DRIVE | 0x0000)
#define ERR_DISK_MOUNT_DRIVE_GET        (ERR_DRIVE | 0x0001)
#define ERR_DISK_MOUNT_NOT_EXIST        (ERR_DRIVE | 0x0002)
#define ERR_DISK_MOUNT_NULL_PTR         (ERR_DRIVE | 0x0003)
#define ERR_DISK_MOUNT_FAIL             (ERR_DRIVE | 0x0004)
#define ERR_DISK_DETECT_PIN_CFG_FAIL    (ERR_DRIVE | 0x0005)
#define ERR_DISK_POWER_PIN_CFG_FAIL     (ERR_DRIVE | 0x0006)
#define ERR_DISK_ENABLE_PIN_CFG_FAIL    (ERR_DRIVE | 0x0007)
#define ERR_DISK_WP_PIN_CFG_FAIL        (ERR_DRIVE | 0x0008)
#define ERR_DISK_PWR_RECYCLE_SET_FAIL   (ERR_DRIVE | 0x0009)
#define ERR_RAM_DET_VFS_FAIL            (ERR_DRIVE | 0x000a)
#define ERR_DISK_DETECT_ERR0            (ERR_DRIVE | 0x000b)
#define ERR_DISK_MOUNT_TASK_FAIL        (ERR_DRIVE | 0x000c)
#define ERR_DISK_MOUNT_TASK_DEL_FAIL    (ERR_DRIVE | 0x000d)
#define ERR_DISK_MOUNT_TASK_ERR0        (ERR_DRIVE | 0x000e)
#define ERR_DISK_MOUNT_TASK_CREATE_FAIL (ERR_DRIVE | 0x000e)
#define ERR_DISK_RAM_BUFFER_FAIL0       (ERR_DRIVE | 0x000f)
#define ERR_DISK_MOUNT_DIR_FAIL         (ERR_DRIVE | 0x0010)
#define ERR_DISK_RAM_BUFFER_FAIL1       (ERR_DRIVE | 0x0011)
#define ERR_DISK_INFO_FAIL_TO_GET       (ERR_DRIVE | 0x0012)
#define ERR_DISK_INFO_ERR_PARAM         (ERR_DRIVE | 0x0013)

/*
 * utility: bad pixel.
 */
#define ERR_BP_FIND_NO_CBFUNC        (ERR_BP | 0x0000)
#define ERR_BP_FIND_CFG_LOAD_ERR     (ERR_BP | 0x0001)
#define ERR_BP_FIND_RAWDATA_ERR0     (ERR_BP | 0x0002)
#define ERR_BP_FIND_RAWDATA_ERR1     (ERR_BP | 0x0003)
#define ERR_BP_FIND_LIST_ERR         (ERR_BP | 0x0004)
#define ERR_BP_CFG_NO_CBFUNC         (ERR_BP | 0x0005)
#define ERR_BP_CFG_HEADER_ERR0       (ERR_BP | 0x0006)
#define ERR_BP_CFG_HEADER_ERR1       (ERR_BP | 0x0007)
#define ERR_BP_CFG_HEADER_ERR2       (ERR_BP | 0x0008)
#define ERR_BP_CFG_HEADER_ERR3       (ERR_BP | 0x0009)
#define ERR_BP_RAWDATA_NO_CBFUNC     (ERR_BP | 0x000a)
#define ERR_BP_EXTRACT_NO_CBFUNC     (ERR_BP | 0x000b)
#define ERR_BP_EXTRACT_WIDTH_ERR     (ERR_BP | 0x000c)
#define ERR_BP_EXTRACT_HIGHT_ERR     (ERR_BP | 0x000d)
#define ERR_BP_EXTRACT_ERR0          (ERR_BP | 0x000e)
#define ERR_BP_EXTRACT_ERR1          (ERR_BP | 0x000f)
#define ERR_BP_EXTRACT_BADPIX_OVER   (ERR_BP | 0x0010)
#define ERR_BP_EXTRACT_BUF_ERR       (ERR_BP | 0x0011)
#define ERR_BP_LIST_NO_CBFUNC        (ERR_BP | 0x0012)
#define ERR_BP_LIST_BUF_ERR0         (ERR_BP | 0x0013)
#define ERR_BP_LIST_BUF_ERR1         (ERR_BP | 0x0014)
#define ERR_BP_SEARCH_OPT_ERR        (ERR_BP | 0x0015)
#define ERR_BP_BMP_FILE_OPEN_ERR     (ERR_BP | 0x0016)
#define ERR_BP_BMP_FILE_WR_ERR0      (ERR_BP | 0x0017)
#define ERR_BP_BMP_FILE_WR_ERR1      (ERR_BP | 0x0018)
#define ERR_BP_BMP_FILE_WR_ERR2      (ERR_BP | 0x0019)
#define ERR_BP_BMP_FILE_WR_ERR3      (ERR_BP | 0x001a)
#define ERR_BP_BMP_FILE_WR_ERR4      (ERR_BP | 0x001b)
#define ERR_BP_BMP_BUF_ERR           (ERR_BP | 0x001c)
#define ERR_BP_RAW2FILE_ERR0         (ERR_BP | 0x001d)
#define ERR_BP_RAW2FILE_ERR1         (ERR_BP | 0x001e)
#define ERR_BP_RAW2FILE_ERR2         (ERR_BP | 0x001f)
#define ERR_BP_RAW2FILE_ERR3         (ERR_BP | 0x0020)
#define ERR_BP_RAW2FILE_ERR4         (ERR_BP | 0x0021)
#define ERR_BP_RAWREAD_OVER          (ERR_BP | 0x0022)
#define ERR_BP_RAWWRITE_OVER         (ERR_BP | 0x0023)
#define ERR_BP_REORDER_ERR0          (ERR_BP | 0x0024)
#define ERR_BP_REORDER_ERR1          (ERR_BP | 0x0025)
#define ERR_BP_REORDER_ERR2          (ERR_BP | 0x0026)
#define ERR_BP_REORDER_ERR3          (ERR_BP | 0x0027)
#define ERR_BP_REORDER_ERR4          (ERR_BP | 0x0028)
#define ERR_BP_REORDER_ERR5          (ERR_BP | 0x0029)
#define ERR_BP_PRETBL_BUF_ERR        (ERR_BP | 0x002a)
#define ERR_BP_EXTRA_ERR1            (ERR_BP | 0x002b)
#define ERR_BP_EXTRA_ERR2            (ERR_BP | 0x002c)
#define ERR_BP_EXTRA_ERR3            (ERR_BP | 0x002d)
#define ERR_BP_EXTRA_ERR4            (ERR_BP | 0x002e)
#define ERR_BP_EXTRA_ERR5            (ERR_BP | 0x002f)

/*
 * DPOF.
 */
#define ERR_DPOF_UNKNOWN_CREATOR     (ERR_DPOF | 0x0001) /* Unrecognised print type */
#define ERR_DPOF_UNSUPPORT_SYNTAX    (ERR_DPOF | 0x0002) /* Unsupport syntax */
#define ERR_DPOF_UNSUPPORT_FORMAT    (ERR_DPOF | 0x0003) /* JFIF and exif only */
#define ERR_DPOF_ILL_PARAM           (ERR_DPOF | 0x0004) /* Illeage paramater */
#define ERR_DPOF_JOB_DUPLICATE       (ERR_DPOF | 0x0010) /* Print job is duplicate */
#define ERR_DPOF_JOB_NOTFOUND        (ERR_DPOF | 0x0011) /* Print jon not found */
#define ERR_DPOF_JOB_FULL            (ERR_DPOF | 0x0012) /* Job number has reached 999 */
#define ERR_DPOF_NODISK              (ERR_DPOF | 0x0020) /* No storage found */
#define ERR_DPOF_DISK_FULL           (ERR_DPOF | 0x0021) /* No disk space */
#define ERR_DPOF_NOMEM               (ERR_DPOF | 0x0022) /* Out of memory */

/*
 * EXIF.
 */
#define ERR_EXIF_UNSUPPROT_FILE    (ERR_EXIF | 0x0001) /* Unsupport file format */
#define ERR_EXIF_UNRECOGNIZE_APP1  (ERR_EXIF | 0x0010) /* Unrecognised APP1 */
#define ERR_EXIF_UNRECOGNIZE_APPN  (ERR_EXIF | 0x0011) /* Unrecognised APPn */
#define ERR_EXIF_UNRECOGNIZE_DQT   (ERR_EXIF | 0x0012) /* Unrecognised DQT */
#define ERR_EXIF_UNRECOGNIZE_DHT   (ERR_EXIF | 0x0013) /* Unrecognised DHT */
#define ERR_EXIF_MISSING_SOI       (ERR_EXIF | 0x0014) /* SOI missing */
#define ERR_EXIF_MISSING_EOI       (ERR_EXIF | 0x0015) /* EOI missing  */
#define ERR_EXIF_UNRECOGNIZE_SOF   (ERR_EXIF | 0x0016) /* Unrecognised SOF */
#define ERR_EXIF_UNRECOGNIZE_SOS   (ERR_EXIF | 0x0017) /* Unrecognised SOS */
#define ERR_EXIF_UNKNOWN_ERR       (ERR_EXIF | 0x0030) /* Unknown error */
#define ERR_EXIF_ILL_PARAM         (ERR_EXIF | 0x0031) /* Illegal Param */
#define ERR_EXIF_NOMEM             (ERR_EXIF | 0x0032) /* Out of memory */

/*
 * FS: Posix.
 */
#define ERR_POSIX_NOERR        (ERR_POSIX | 0x0000)  /* No error */
#define ERR_POSIX_NOENT        (ERR_POSIX | 0x0001)  /* No such entity */
#define ERR_POSIX_IO           (ERR_POSIX | 0x0002)  /* I/O error */
#define ERR_POSIX_BADF         (ERR_POSIX | 0x0003)  /* Bad file handle */
#define ERR_POSIX_NOMEM        (ERR_POSIX | 0x0004)  /* Out of memory */
#define ERR_POSIX_BUSY         (ERR_POSIX | 0x0005)  /* Resource busy */
#define ERR_POSIX_NODEV        (ERR_POSIX | 0x0006)  /* No such device */
#define ERR_POSIX_NOTDIR       (ERR_POSIX | 0x0007)  /* Not a directory */
#define ERR_POSIX_ISDIR        (ERR_POSIX | 0x0008)  /* Is a directory */
#define ERR_POSIX_INVAL        (ERR_POSIX | 0x0009)  /* Invalid argument */
#define ERR_POSIX_MFILE        (ERR_POSIX | 0x000A)  /* Too many open files */
#define ERR_POSIX_NOSPC        (ERR_POSIX | 0x000B)  /* No space left on device */
#define ERR_POSIX_SPIPE        (ERR_POSIX | 0x000C)  /* Illegal seek */
#define ERR_POSIX_ROFS         (ERR_POSIX | 0x000D)  /* Read-only file system */
#define ERR_POSIX_NAMETOOLONG  (ERR_POSIX | 0x000E)  /* File name too long */
#define ERR_POSIX_NOTEMPTY     (ERR_POSIX | 0x000F)  /* Directory not empty */
#define ERR_POSIX_NOTSUP       (ERR_POSIX | 0x0010)  /* Not supported error */
#define ERR_POSIX_EOF          (ERR_POSIX | 0x0011)  /* End of file reached */
/* Additional errors used by networking */
#define ERR_POSIX_NXIO         (ERR_POSIX | 0x0012)  /* Device not configured */
#define ERR_POSIX_ACCES        (ERR_POSIX | 0x0013)  /* Permission denied */
#define ERR_POSIX_EXIST        (ERR_POSIX | 0x0014)  /* File exists */
#define ERR_POSIX_NOTTY        (ERR_POSIX | 0x0015)  /* Inappropriate ioctl for device */

/*
 * mode switch.
 */
#define ERR_MODESW_INVALID_MSG      (ERR_MODESW | 0x0000)  /* invalid message control id. */
#define ERR_MODESW_INVALID_MODE     (ERR_MODESW | 0x0001)  /* invalid mode. */
#define ERR_MODESW_DISP_CHANGING    (ERR_MODESW | 0x0002)  /* disp dev is changing. */
#define ERR_MODESW_SENSOR_CHANGING  (ERR_MODESW | 0x0003)  /* sensor dev is changing. */
#define ERR_MODESW_MODE_CHANGING    (ERR_MODESW | 0x0004)  /* mode is changing. */

/*
 * preview.
 */
#define ERR_PREVIEW_INVALID_MSG    (ERR_PREVIEW | 0x0000)  /* invalid message control id. */
#define ERR_PREVIEW_INVALID_PARAM  (ERR_PREVIEW | 0x0001)  /* invalid parameter. */

/*
 * snap image.
 */
#define ERR_SNAP_GENERAL_FAIL       (ERR_SNAP | 0x0000)  /* Failed for general error */
#define ERR_SNAP_SNAPTASK_FAIL      (ERR_SNAP | 0x0001)  /* Failed for snapTask */
#define ERR_SNAP_SSERVERTASK_FAIL   (ERR_SNAP | 0x0002)  /* Failed for sServerTask */
#define ERR_SNAP_CDSPTASK_FAIL      (ERR_SNAP | 0x0003)  /* Failed for cdspTask */
#define ERR_SNAP_JPGCOMPTASK_FAIL   (ERR_SNAP | 0x0004)  /* Failed for jpgCompTask */
#define ERR_SNAP_STORETASK_FAIL     (ERR_SNAP | 0x0005)  /* Failed for storeTask */
#define ERR_SNAP_MEMORY_NOT_ENOUGH  (ERR_SNAP | 0x0006)  /* Memory is not enough */
#define ERR_SNAP_TASK_OPEN          (ERR_SNAP | 0x0007)  /* It is wrong for thread creatd */
#define ERR_SNAP_TASK_CLOSE         (ERR_SNAP | 0x0008)  /* It is wrong for thread removed */

/*
 * record.
 */
#define ERR_REC_PIC (ERR_REC | (0x00 << 16))    /* picture record - not used */
#define ERR_REC_AUD (ERR_REC | (0x01 << 16))    /* audio record error */
#define ERR_REC_VID (ERR_REC | (0x02 << 16))    /* video record error */

/*
 * audio record.
 */
#define ERR_REC_AUD_UNKNOWN_DEVICE  (ERR_REC_AUD | 0x0000)
#define ERR_REC_AUD_FILE_OPEN_FAIL  (ERR_REC_AUD | 0x0001)
#define ERR_REC_AUD_MEM_ALLOC_FAIL  (ERR_REC_AUD | 0x0002)
#define ERR_REC_AUD_NO_START_ABORT  (ERR_REC_AUD | 0x0003)
#define ERR_REC_AUD_ENCODE_ERR      (ERR_REC_AUD | 0x0004)
#define ERR_REC_AUD_KERNEL_FAIL     (ERR_REC_AUD | 0x0005)
#define ERR_REC_AUD_WRONG_STATE     (ERR_REC_AUD | 0x0006)


/*
 * play.
 */
#define ERR_PLAY_PIC  (ERR_PLAY | (0x00 << 16))   /* picture play error */
#define ERR_PLAY_AUD  (ERR_PLAY | (0x01 << 16))   /* audio play error */
#define ERR_PLAY_VID  (ERR_PLAY | (0x02 << 16))   /* video play error */

/*
 * still playback.
 */
#define ERR_PB_REENTRY_ERR      (ERR_PLAY_PIC | 0x0001)
#define ERR_PB_MEM_ALLOC_FAIL   (ERR_PLAY_PIC | 0x0002)
#define ERR_PB_MEM_NOT_ENOUGH   (ERR_PLAY_PIC | 0x0003)
#define ERR_PB_DECODE_FAIL      (ERR_PLAY_PIC | 0x0004)
#define ERR_PB_THUMB_NOT_FOUND  (ERR_PLAY_PIC | 0x0005)
#define ERR_PB_INVALID_PARAM    (ERR_PLAY_PIC | 0x0006)
#define ERR_PB_IMGEDIT_FAIL     (ERR_PLAY_PIC | 0x0007)
#define ERR_PB_JOB_ABORT        (ERR_PLAY_PIC | 0x0008)
#define ERR_PB_DISK_ERR         (ERR_PLAY_PIC | 0x0009)
#define ERR_PB_DISK_FULL        (ERR_PLAY_PIC | 0x000a)
#define ERR_PB_FILE_NOT_FOUND   (ERR_PLAY_PIC | 0x000b)
#define ERR_PB_UNKNOWN_ERROR    (ERR_PLAY_PIC | 0x000c)
#define ERR_PB_TIMER_ERR        (ERR_PLAY_PIC | 0x000d)

/*
 * audio play.
 */
#define ERR_PLAY_AUD_UNKNOWN_DEVICE  (ERR_PLAY_AUD | 0x0000)
#define ERR_PLAY_AUD_FILE_OPEN_FAIL  (ERR_PLAY_AUD | 0x0001)
#define ERR_PLAY_AUD_MEM_ALLOC_FAIL  (ERR_PLAY_AUD | 0x0002)
#define ERR_PLAY_AUD_REENTER_START   (ERR_PLAY_AUD | 0x0003)
#define ERR_PLAY_AUD_DECODE_ERR      (ERR_PLAY_AUD | 0x0004)
#define ERR_PLAY_AUD_KERNEL_FAIL     (ERR_PLAY_AUD | 0x0005)
#define ERR_PLAY_AUD_WRONG_STATE     (ERR_PLAY_AUD | 0x0006)

#define ERR_AUD_MIX_FILE_OPEN_FAIL   (ERR_AUD_MIX | 0x0000)
#define ERR_AUD_MIX_MEM_ALLOC_FAIL   (ERR_AUD_MIX | 0x0001)
#define ERR_AUD_MIX_AUD_NOT_PLAYING  (ERR_AUD_MIX | 0x0002)

/*
 * reserved block.
 */
#define ERR_RSVBLK_UNRECOGNIZE_ADF    (ERR_RSVBLK | 0x0001)  /* Unrecognized ADF file */
#define ERR_RSVBLK_UNRECOGNIZE_UDF    (ERR_RSVBLK | 0x0002)  /* Unrecognized UDF file */
#define ERR_RSVBLK_INVALID_AUDF_DATA  (ERR_RSVBLK | 0x0003)  /* Invalid ADF/UDF data */
#define ERR_RSVBLK_INVALID_PARAM   	  (ERR_RSVBLK | 0x0004)  /* Invalid parameter*/
#define ERR_RSVBLK_DISK_ERR		      (ERR_RSVBLK | 0x0005)  /* Disk IO error */
#define ERR_RSVBLK_DISK_FULL		  (ERR_RSVBLK | 0x0006)  /* Disk full */

#define ERR_ISP_BOOT_OPEN_FILE_FAIL  (ERR_ISP | 0x0001)  /*  */
#define ERR_ISP_BOOT_FILE_IS_ZERO    (ERR_ISP | 0x0002)  /*  */
#define ERR_ISP_BOOT_BUF_ERR0        (ERR_ISP | 0x0003)  /*  */
#define ERR_ISP_BOOT_BUF_ERR1        (ERR_ISP | 0x0004)  /*  */
#define ERR_ISP_OPEN_FILE_FAIL0      (ERR_ISP | 0x0005)  /*  */
#define ERR_ISP_OPEN_FILE_FAIL1      (ERR_ISP | 0x0006)  /*  */
#define ERR_ISP_FILE_IS_ZERO         (ERR_ISP | 0x0006)  /*  */
#define ERR_ISP_BUF_ERR0             (ERR_ISP | 0x0007)  /*  */
#define ERR_ISP_BUF_ERR1             (ERR_ISP | 0x0008)  /*  */
#define ERR_ISP_BUF_ERR2             (ERR_ISP | 0x0009)  /*  */
#define ERR_ISP_SEEK_FILE_FAIL       (ERR_ISP | 0x000a)

/*
 * slide show.
 */
#define ERR_SLIDE_NOT_INIT   (ERR_SLIDE | 0x0001)
#define ERR_SLIDE_BUSY       (ERR_SLIDE | 0x0002)
#define ERR_SLIDE_NOT_PLAY   (ERR_SLIDE | 0x0003)
#define ERR_SLIDE_NOT_PAUSE  (ERR_SLIDE | 0x0004)

/*
 * graphics.
 */
#define ERR_GFX_NOT_INIT        (ERR_GFX | 0x0001)
#define ERR_GFX_ICON_NOT_FOUND  (ERR_GFX | 0x0002)

#endif  /* __ERR_CODE_TBL_H__ */

