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

#ifndef _APP_STRINGID_H_
#define _APP_STRINGID_H_

#include "app_menu_tab.h"

#define STRTK_NULL                     0x0000
#define STRTK_PB_ROTATE                STRTK_ROTATE
#define STRTK_PB_SAVE                  STRTK_SAVE
#define STRTK_PB_BACK                  STRTK_BACK
#define STRTK_PB_PROTECT               STRTK_PROTECT
#define STRTK_PB_DELETE                STRTK_DELETE
#define STRTK_UNPROTECT_THIS_FILE      STRTK_UNPROTECT_THIS_FILE_
#define STRTK_UNPROTECT_ALL_FILES      STRTK_UNPROTECT_ALL_FILES_
#define STRTK_PROTECT_THIS_FILE        STRTK_PROTECT_THIS_FILE_
#define STRTK_PROTECT_ALL_FILES        STRTK_PROTECT_THIS_FILE_
#define STRTK_UNPTC_PTC_FILE           STRTK_PROTECT_UNPROTECT_FILE_
#define STRTK_DELETE_THIS_FILE         STRTK_DELETE_THIS_FILE_
#define STRTK_DETECTING_BLINKING       STRTK_DETECTING_BLINKING___
#define STRTK_DB_SAVE                  STRTK_SAVE
#define STRTK_DB_CANCEL                STRTK_CANCEL
#define STRTK_CROP_THIS_PHOTO          STRTK_CROP_THIS_PHOTO_
#define STRTK_PLS_SEL_FILETYPE         STRTK_PLEASE_SELECT_FILETYPE_
#define STRTK_REDEYE_SAVE              STRTK_REDEYESAVE
#define STRTK_OVER_WRITE               STRTK_OVERWRITE
#define STRTK_CALIB_INFOR              STRTK_CALIB_INFO
#define STRTK_ISO_CLBT                 STRTK_ISO_CLBT_LV11_
#define STRTK_WB_BACK                  STRTK_BACK
#define STRTK_CEARTE_LV10_CLBT         STRTK_CREAT_LV10_CALIBRATION
#define STRTK_CEARTE_LV10_STD          STRTK_CREAT_LV10_STANDARD
#define STRTK_FLASH_BACK               STRTK_BACK
#define STRTK_PREANALYSES              STRTK_PRE_ANALYSES
#define STRTK_HPOS_SCAN                STRTK_H_POS_SCAN
#define STRTK_VIEW_DPS                 STRTK_VIEW_DPC
#define STRTK_BT_ALKALINE              STRTK_ALKALINE
#define STRTK_BT_NIMH                  STRTK_NIMH
#define STRTK_BT_LITHIUM               STRTK_LITHIUM
#define STRTK_BT_OXYRIDE               STRTK_OXYRIDE
#define STRTK_CAM_CONFIG               STRTK__CAM_CONFIG
#define STRTK_FW_UPD                   STRTK_FW_UPDATE
#define STRTK_FACTORY_DEF              STRTK_FACTORY_DEFAULT
#define STRTK_RAWBATCH_ENA             STRTK_RAWBATCH_ENABLE
#define STRTK_TEST_BENCH               STRTK__TEST_BENCH
#define STRTK_SENSOR                   STRTK__SENSOR
#define STRTK_SENSOR_BACK              STRTK_BACK
#define STRTK_PV_LV416                 STRTK_PV_LV_4_16
#define STRTK_CAP_LV416                STRTK_CAP_LV_4_16
#define STRTK_PFLASH                   STRTK__FLASH
#define STRTK_PFLASH_BACK              STRTK_BACK
#define STRTK_P2                       STRTK_P2___
#define STRTK_PAF                      STRTK__AF
#define STRTK_NOISE_CORING_VAL_T       STRTK_NOISE_CORING_VALUE_TEST
#define STRTK_NOISE_CORING_THESH_T     STRTK_NOISE_CORING_THESHOLD_TEST
#define STRTK_NOISE_THESH_T            STRTK_NOISE_THESHOID_TEST
#define STRTK_AF_NOISE_BACK            STRTK_BACK
#define STRTK_USBISP                   STRTK_USB_ISP
#define STRTK_STD_ALONE_LSC            STRTK_STAND_ALONE_LSC
#define STRTK_DZ                       STRTK_DIGITAL_ZOOM
#define STRTK_DZ_ENTER                 STRTK_ENTER
#define STRTK_DZ_BACK                  STRTK_BACK
#define STRTK_DEL_ALL_FILES            STRTK_DELETE_ALL_FILES_
#define STRTK_DEL_FILES                STRTK_DELETE_FILES_
#define STRTK_FORMATTING               STRTK_FORMATTING___
#define STRTK_HAS_NO_SIZE_TO_RESIZE    STRTK_HAS_NO_SIZE_TO_RESIZE_
#define STRTK_FILE_LOCKED              STRTK_FILE_LOCKED_
#define STRTK_PROT_FILE                STRTK_PROTECT_FILE_
#define STRTK_UNPROT_FILE              STRTK_UNPROTECT_FILE_
#define STRTK_PROT_ALL                 STRTK_PROTECT_ALL_
#define STRTK_UNPROT_ALL               STRTK_UNPROTECT_ALL_
#define STRTK_NO_CARD                  STRTK_NO_CARD_
#define STRTK_REMOVE_CARD              STRTK_REMOVE_CARD_
#define STRTK_WAITING                  STRTK_WAITING___
#define STRTK_CARD_LOCKED              STRTK_CARD_LOCKED_
#define STRTK_F_ERROR                  STRTK_FILE_ERROR_
#define STRTK_PLS_PLUGOUT_CARD         STRTK_PLEASE_PLUG_OUT_CARD
#define STRTK_MEM_FULL                 STRTK_MEMORY_FULL
#define STRTK_FORMAT_FAIL              STRTK_FORMAT_FAIL_
#define STRTK_LOW_BATT                 STRTK_LOW_BATTERY
#define STRTK_NOT_SUPPORT              STRTK_NOT_SUPPORT_
#define STRTK_CNCL                     STRTK_CANCEL
#define STRTK_SEL_UP_TO_24             STRTK_SELECT_UP_TO_24
#define STRTK_PLS_SEL_PIC              STRTK_PLEASE_SELECT_A_PICTURE
#define STRTK_PRINT_ERR                STRTK_PRINT_ERROR
#define STRTK_PRINTER_ERR              STRTK_PRINTER_ERROR
#define STRTK_CTP                      STRTK_CROP_THIS_PHOTO_
#define STRTK_PIC_BRDG                 STRTK_PICTBRIDGE
#define STRTK_SET_THIS_PHO             STRTK_SET_THIS_PHOTO_
#define STRTK_SET_ALL_PHO              STRTK_SET_ALL_PHOTO_
#define STRTK_REVIEW_THIS_FILE         STRTK_REVIEW_THIS_FILE_
#define STRTK_SAVE_THIS_FILE           STRTK_SAVE_THIS_FILE_
#define STRTK_DEL                      STRTK_DELETE
#define STRTK_SEL                      STRTK_SELECT
#define STRTK_COPY_THIS_FILE           STRTK_COPY_THIS_FILE_
#define STRTK_COPY_ALL_FILES           STRTK_COPY_ALL_FILES_
#define STRTK_ROTATE_THIS_PHOTO        STRTK_ROTATE_THIS_PHOTO_
#define STRTK_DEL_VOICE_MEMO           STRTK_DELETE_VOICE_MEMO_
#define STRTK_RDY_TO_REC               STRTK_READY_TO_RECORD
#define STRTK_RDY_TO_PLAY              STRTK_READY_TO_PLAY
#define STRTK_RECORDING_MEMO           STRTK_RECORDING_MEMO___
#define STRTK_PLAYING_AUDIO            STRTK_PLAYING_AUDIO___
#define STRTK_TRIM_THIS_PHOTO          STRTK_TRIM_THIS_PHOTO_
#define STRTK_COPY_ER                  STRTK_COPY_ERROR
#define STRTK_CAPTURE_NOT_EN_SIZE      STRTK_CAPTURE_NOT_ENOUGH_SIZE
#define STRTK_BUSY_PLS_WAIT            STRTK_BUSY___PLEASE_WAIT
#define STRTK_NOT_AVAILABLE_THIS_FILE  STRTK_NOT_AVAILABLE_FOR_THIS_FILE
#define STRTK_IMG_SAVED                STRTK_IMAGE_SAVED
#define STRTK_PAPER_ERR                STRTK_PAPER_ERROR
#define STRTK_LAOD_ERR                 STRTK_LOAD_ERROR
#define STRTK_EJECT_ERR                STRTK_EJECT_ERROR
#define STRTK_LEVER_ERR                STRTK_LEVER_ERROR
#define STRTK_INFO_ERR                 STRTK_INFO_ERROR
#define STRTK_USB_ERR                  STRTK_USB_ERROR
#define STRTK_PRINTING                 STRTK_PRINTING___
#define STRTK_CANCELING                STRTK_CANCELLING
#define STRTK_TRIM_VIDEO_TO_SHORT      STRTK_TRIM_VIDEO_IS_TOO_SHORT
#define STRTK_PB_CDFS_TYPE             STRTK_PB_TYPE
#define STRTK_PB_CDFS_VIDEO            STRTK_VIDEO
#define STRTK_PB_CDFS_IMAGE            STRTK_IMAGE
#define STRTK_SPV_BURST_H              STRTK_BURST_LMT
#define STRTK_SPV_BURST_L              STRTK_BURST_SUST
#define STRTK_SPV_MFNR                 STRTK_MFNR
#define STRTK_SPV_PRESHOT              STRTK_PRESHOT
#define STRTK_SPV_ZEROSHOT             STRTK_ZEROSHOT
#define STRTK_STU_AR_SEC1_2            STRTK_ZEROSHOT
#define STRTK_STU_MEM                  STRTK_MEMORY
#define STRTK_STU_MEM_INTERNAL         STRTK_INTERNAL
#define STRTK_STU_MEM_CARD             STRTK_CARD
#define STRTK_STU_MEM_USB              STRTK_USB
#define STRTK_SPV_FACE_BEAUTY          STRTK_FACE_BEAUTY
#define STRTK_SPV_SUPER_RES            STRTK_SUPER_RESOLUTION
#define STRTK_STU_USB_DONGLE           STRTK_USB_DONGLE
#define STRTK_STU_USB_SDIO             STRTK_SDIO
#define STRTK_STU_WIFI_DEV             STRTK_WIFI_DEVICE
#define STRTK_SPV_MONO                 STRTK_MONO
#define STRTK_SPV_TOYCAM               STRTK_TOYCAM
#define STRTK_SPV_LIGHTTONE            STRTK_LIGHTTONE
#define STRTK_DUAL_FHD_720             STRTK_FHD_720P
#define STRTK_DUAL_FHD_VGA             STRTK_FHD_VGA
#define STRTK_DUAL_FHD_FHD             STRTK_FHD_FHD
#define STRTK_STU_PC_TIME_LAPSE        STRTK_TIME_LAPSE
#define STRTK_INVERTMODE               STRTK_INVERT_MODE
#define STRTK_DISK_SPEED_TOO_SLOW      STRTK_CARD_SPEED_SLOW
#define STRTK_PB_PROTECT               STRTK_PROTECT
#define STRTK_PB_PROTECT_ALL           STRTK_PROTECT_ALL
#define STRTK_PB_UNPROTECT_ALL         STRTK_UNPROTECT_ALL
#define STRTK_STU_STARTUP              STRTK_START_UP

#endif /* _APP_STRINGID_H_ */

