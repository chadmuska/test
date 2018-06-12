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
/* Created at May,02,2018 08:31:55 */
#ifndef _MENU_TAB_17DVR_H_
#define _MENU_TAB_17DVR_H_

#include "app_menu_event_17dvr.h"

#define	MENU_MARK_STILL	0x01000000
#define	MENU_MARK_VIDEO	0x02000000
#define	MENU_MARK_PLAYBACK	0x03000000
#define	MENU_MARK_SETUP	0x04000000
#define	MENU_MARK_SOUND	0x04010000
#define	MENU_MARK_LIGHTFREQ	0x04030000
#define	MENU_MARK_SlEEPTIME	0x04040000
#define	MENU_MARK_POWERONREC	0x04050000
#define	MENU_MARK_DATETIME	0x04070000
#define	MENU_MARK_LANGUAGE	0x04080000
#define	MENU_MARK_FORMAT	0x040b0000
#define	MENU_MARK_SCENEMODE	0x05000000
#define	MENU_MARK_SCENEMODEITEM	0x05010000
#define	MENU_MARK_FORMAT_YES	0x06010101
#define	MENU_MARK_FORMAT_NO	0x06010102
#define	MENU_MARK_DELONE_YES	0x06010201
#define	MENU_MARK_DELONE_NO	0x06010202
#define	MENU_MARK_BLINK_SAVE	0x06010301
#define	MENU_MARK_BLINK_CANCEL	0x06010302
#define	MENU_MARK_JPG_CROP_YES	0x06010401
#define	MENU_MARK_JPG_CROP_NO	0x06010402
#define	MENU_MARK_USBMODESET	0x06010501
#define	MENU_MARK_FILETYPE_EVENT	0x06010601
#define	MENU_MARK_REDEYE_WRITE	0x07010000
#define	MENU_MARK_REDEYE_SAVE	0x07020000
#define	MENU_MARK_REDEYE_CANCEL	0x07030000
#define	MENU_MARK_CALIBRATION	0x08000000
#define	MENU_MARK_WBCLBT	0x08060000
#define	MENU_MARK_FLASH	0x08080000
#define	MENU_MARK_CAMCONFIG	0x080c0000
#define	MENU_MARK_TESTBENCH	0x080d0000

#define ID_STR_YES		0x0194
#define ID_STR_NO		0x0195
#define ID_STR_CARD_ERROR		0x0196
#define ID_STR_FORMAT		0x0197
#define ID_STR_UNPROTECT_THIS_FILE_		0x0198
#define ID_STR_UNPROTECT_ALL_FILES_		0x0199
#define ID_STR_PROTECT_THIS_FILE_		0x019a
#define ID_STR_PROTECT_ALL_FILES_		0x019b
#define ID_STR_PROTECT_UNPROTECT_FILE_		0x019c
#define ID_STR_DELETE_THIS_FILE_		0x019d
#define ID_STR_DETECTING_BLINKING___		0x019e
#define ID_STR_SAVE		0x019f
#define ID_STR_CROP_THIS_PHOTO_		0x01a0
#define ID_STR_PLEASE_SELECT_FILETYPE_		0x01a1
#define ID_STR_REDEYESAVE		0x01a2
#define ID_STR_OVERWRITE		0x01a3
#define ID_STR_SAVE_AS		0x01a4
#define ID_STR_CALIBRATION		0x01a5
#define ID_STR_CALIB_INFO		0x01a6
#define ID_STR_OB_CLBT		0x01a7
#define ID_STR_M_SHUTTER		0x01a8
#define ID_STR_ISO_CLBT_LV11_		0x01a9
#define ID_STR_DIFF_ISO_OB_CLBT		0x01aa
#define ID_STR_WB_CLBT		0x01ab
#define ID_STR_BACK		0x01ac
#define ID_STR_CALIB_AWB		0x01ad
#define ID_STR_CREAT_CTT		0x01ae
#define ID_STR_CREAT_LV10_CALIBRATION		0x01af
#define ID_STR_CREAT_LV10_STANDARD		0x01b0
#define ID_STR_FOCUS		0x01b1
#define ID_STR_FLASH		0x01b2
#define ID_STR_PRE_ANALYSES		0x01b3
#define ID_STR_H_POS_SCAN		0x01b4
#define ID_STR_DURATION_SCAN		0x01b5
#define ID_STR_VIEW_DPC		0x01b6
#define ID_STR_CAPTURE_DPC		0x01b7
#define ID_STR_BATTERY		0x01b8
#define ID_STR_BATTERY_TYPE		0x01b9
#define ID_STR_ALKALINE		0x01ba
#define ID_STR_NIMH		0x01bb
#define ID_STR_LITHIUM		0x01bc
#define ID_STR_OXYRIDE		0x01bd
#define ID_STR__CAM_CONFIG		0x01be
#define ID_STR_FW_UPDATE		0x01bf
#define ID_STR_FACTORY_DEFAULT		0x01c0
#define ID_STR_PV_SAVERAW		0x01c1
#define ID_STR_CAP_SAVERAW		0x01c2
#define ID_STR_RAWBATCH_ENABLE		0x01c3
#define ID_STR_PREFH_STAMP		0x01c4
#define ID_STR_AF_FULL_SCAN		0x01c5
#define ID_STR__TEST_BENCH		0x01c6
#define ID_STR_TEST_ROBOT		0x01c7
#define ID_STR__SENSOR		0x01c8
#define ID_STR_IRIS_DIFF_LV12		0x01c9
#define ID_STR_MINI_GAIN_LV12		0x01ca
#define ID_STR_PV_LV_4_16		0x01cb
#define ID_STR_CAP_LV_4_16		0x01cc
#define ID_STR_P2D		0x01cd
#define ID_STR_CIPA		0x01ce
#define ID_STR_LCD_TEST		0x01cf
#define ID_STR_KEY_TEST		0x01d0
#define ID_STR_AE_METER		0x01d1
#define ID_STR__FLASH		0x01d2
#define ID_STR_CURVE_A0		0x01d3
#define ID_STR_CURVE_A1		0x01d4
#define ID_STR_CURVE_B0		0x01d5
#define ID_STR_CURVE_B1		0x01d6
#define ID_STR_P2___		0x01d7
#define ID_STR__AF		0x01d8
#define ID_STR_NOISE_CORING_VALUE_TEST		0x01d9
#define ID_STR_NOISE_CORING_THESHOLD_TEST		0x01da
#define ID_STR_NOISE_THESHOID_TEST		0x01db
#define ID_STR_BATTERY_CURVE		0x01dc
#define ID_STR_AE_DEBUG		0x01dd
#define ID_STR_AWB_DEBUG		0x01de
#define ID_STR_USB_ISP		0x01df
#define ID_STR_STAND_ALONE_LSC		0x01e0
#define ID_STR_PV_OB_CAL		0x01e1
#define ID_STR_VIDEO_REC_LOG		0x01e2
#define ID_STR_DIGITAL_ZOOM		0x01e3
#define ID_STR_ENTER		0x01e4
#define ID_STR_DELETE_ALL_FILES_		0x01e5
#define ID_STR_DELETE_FILES_		0x01e6
#define ID_STR_FORMATTING___		0x01e7
#define ID_STR_HAS_NO_SIZE_TO_RESIZE_		0x01e8
#define ID_STR_FILE_LOCKED_		0x01e9
#define ID_STR_PROTECT_FILE_		0x01ea
#define ID_STR_UNPROTECT_FILE_		0x01eb
#define ID_STR_PROTECT_ALL_		0x01ec
#define ID_STR_UNPROTECT_ALL_		0x01ed
#define ID_STR_NO_FILE		0x01ee
#define ID_STR_NO_CARD_		0x01ef
#define ID_STR_REMOVE_CARD_		0x01f0
#define ID_STR_WAITING___		0x01f1
#define ID_STR_CARD_LOCKED_		0x01f2
#define ID_STR_PAUSE		0x01f3
#define ID_STR_REC		0x01f4
#define ID_STR_FILE_ERROR_		0x01f5
#define ID_STR_PLEASE_PLUG_OUT_CARD		0x01f6
#define ID_STR_MEMORY_FULL		0x01f7
#define ID_STR_WAITING_FOR_PRINTER		0x01f8
#define ID_STR_NO_JPEG		0x01f9
#define ID_STR_FORMAT_FAIL_		0x01fa
#define ID_STR_LOW_BATTERY		0x01fb
#define ID_STR_OUT_OF_BATTERY		0x01fc
#define ID_STR_NOT_SUPPORT_		0x01fd
#define ID_STR_CANCEL		0x01fe
#define ID_STR_SELECT_UP_TO_24		0x01ff
#define ID_STR_PLEASE_SELECT_A_PICTURE		0x0200
#define ID_STR_PRINT_ERROR		0x0201
#define ID_STR_PRINTER_ERROR		0x0202
#define ID_STR_NOT_FIND_PRINTER		0x0203
#define ID_STR_PICTBRIDGE		0x0204
#define ID_STR_SET_THIS_PHOTO_		0x0205
#define ID_STR_SET_ALL_PHOTO_		0x0206
#define ID_STR_REVIEW_THIS_FILE_		0x0207
#define ID_STR_SAVE_THIS_FILE_		0x0208
#define ID_STR_DELETE		0x0209
#define ID_STR_ALL		0x020a
#define ID_STR_SELECT		0x020b
#define ID_STR_COPY_TO_CARD		0x020c
#define ID_STR_COPY_THIS_FILE_		0x020d
#define ID_STR_COPY_ALL_FILES_		0x020e
#define ID_STR_ROTATE		0x020f
#define ID_STR_ROTATE_THIS_PHOTO_		0x0210
#define ID_STR_SET		0x0211
#define ID_STR_STOP		0x0212
#define ID_STR_DELETE_VOICE_MEMO_		0x0213
#define ID_STR_READY_TO_RECORD		0x0214
#define ID_STR_READY_TO_PLAY		0x0215
#define ID_STR_RECORDING_MEMO___		0x0216
#define ID_STR_PLAYING_AUDIO___		0x0217
#define ID_STR_TRIM_THIS_PHOTO_		0x0218
#define ID_STR_COPY_ERROR		0x0219
#define ID_STR_CAPTURE_NOT_ENOUGH_SIZE		0x021a
#define ID_STR_CAPTURE_DONE		0x021b
#define ID_STR_BUSY___PLEASE_WAIT		0x021c
#define ID_STR_NOT_AVAILABLE_FOR_THIS_FILE		0x021d
#define ID_STR_IMAGE_SAVED		0x021e
#define ID_STR_PAPER_SIZE		0x021f
#define ID_STR_PAPER_ERROR		0x0220
#define ID_STR_LOAD_ERROR		0x0221
#define ID_STR_EJECT_ERROR		0x0222
#define ID_STR_PAPER_JAM		0x0223
#define ID_STR_INK_EMPTY		0x0224
#define ID_STR_INK_LOW		0x0225
#define ID_STR_WASTE_INK		0x0226
#define ID_STR_FATAL_ERROR		0x0227
#define ID_STR_SERVICE_CALL		0x0228
#define ID_STR_PRINTER_BUSY		0x0229
#define ID_STR_LEVER_ERROR		0x022a
#define ID_STR_COVER_OPEN		0x022b
#define ID_STR_INK_COVER_OPEN		0x022c
#define ID_STR_NO_INK_CARTRIDGE		0x022d
#define ID_STR_INFO_ERROR		0x022e
#define ID_STR_USB_ERROR		0x022f
#define ID_STR_CONNECTING		0x0230
#define ID_STR_PRINTING___		0x0231
#define ID_STR_CANCELLING		0x0232
#define ID_STR_NO_PAPER		0x0233
#define ID_STR_PLAY		0x0234
#define ID_STR_DISPLAY_FILE		0x0235
#define ID_STR_COMPLETED		0x0236
#define ID_STR_NOT_SUPPORT_C2C		0x0237
#define ID_STR_FIND_START		0x0238
#define ID_STR_FIND_END		0x0239
#define ID_STR_AND_PRESS_OK		0x023a
#define ID_STR_WRITING_VIDEO		0x023b
#define ID_STR_WRITING_FILE_FAIL		0x023c
#define ID_STR_TRIM_VIDEO_IS_TOO_SHORT		0x023d
#define ID_STR_PB_TYPE		0x023e
#define ID_STR_VIDEO		0x023f
#define ID_STR_IMAGE		0x0240
#define ID_STR_BURST_LMT_3		0x0241
#define ID_STR_BURST_LMT_5		0x0242
#define ID_STR_MFNR		0x0243
#define ID_STR_PRESHOT		0x0244
#define ID_STR_ZEROSHOT		0x0245
#define ID_STR_0_5SEC		0x0246
#define ID_STR_MEMORY		0x0247
#define ID_STR_INTERNAL		0x0248
#define ID_STR_CARD		0x0249
#define ID_STR_USB		0x024a
#define ID_STR_FACE_BEAUTY		0x024b
#define ID_STR_SUPER_RESOLUTION		0x024c
#define ID_STR_USB_DONGLE		0x024d
#define ID_STR_SDIO		0x024e
#define ID_STR_WIFI_DEVICE		0x024f
#define ID_STR_INSUFFICIENT_DISK_SPACE		0x0250
#define ID_STR_MONO		0x0251
#define ID_STR_TOYCAM		0x0252
#define ID_STR_LIGHTTONE		0x0253
#define ID_STR_WAITING_FOR_CAPTURE		0x0254
#define ID_STR_WAITING_FOR_CARD		0x0255
#define ID_STR_FHD_720P		0x0256
#define ID_STR_FHD_VGA		0x0257
#define ID_STR_FHD_FHD		0x0258
#define ID_STR_TIME_LAPSE		0x0259
#define ID_STR_INVERT_MODE		0x025a
#define ID_STR_DISCONNECTING		0x025b
#define ID_STR_CARD_SPEED_SLOW		0x025c
#define ID_STR_POWER_ON_RECORD		0x025d
#define ID_STR_JPG		0x025e
#define ID_STR_EVENT		0x025f
#define ID_STR_PROTECT		0x0260
#define ID_STR_PROTECT_ALL		0x0261
#define ID_STR_UNPROTECT_ALL		0x0262
#define ID_STR_START_UP		0x0263
#define STRTK_YES		ID_STR_YES
#define STRTK_NO		ID_STR_NO
#define STRTK_CARD_ERROR		ID_STR_CARD_ERROR
#define STRTK_FORMAT		ID_STR_FORMAT
#define STRTK_UNPROTECT_THIS_FILE_		ID_STR_UNPROTECT_THIS_FILE_
#define STRTK_UNPROTECT_ALL_FILES_		ID_STR_UNPROTECT_ALL_FILES_
#define STRTK_PROTECT_THIS_FILE_		ID_STR_PROTECT_THIS_FILE_
#define STRTK_PROTECT_ALL_FILES_		ID_STR_PROTECT_ALL_FILES_
#define STRTK_PROTECT_UNPROTECT_FILE_		ID_STR_PROTECT_UNPROTECT_FILE_
#define STRTK_DELETE_THIS_FILE_		ID_STR_DELETE_THIS_FILE_
#define STRTK_DETECTING_BLINKING___		ID_STR_DETECTING_BLINKING___
#define STRTK_SAVE		ID_STR_SAVE
#define STRTK_CROP_THIS_PHOTO_		ID_STR_CROP_THIS_PHOTO_
#define STRTK_PLEASE_SELECT_FILETYPE_		ID_STR_PLEASE_SELECT_FILETYPE_
#define STRTK_REDEYESAVE		ID_STR_REDEYESAVE
#define STRTK_OVERWRITE		ID_STR_OVERWRITE
#define STRTK_SAVE_AS		ID_STR_SAVE_AS
#define STRTK_CALIBRATION		ID_STR_CALIBRATION
#define STRTK_CALIB_INFO		ID_STR_CALIB_INFO
#define STRTK_OB_CLBT		ID_STR_OB_CLBT
#define STRTK_M_SHUTTER		ID_STR_M_SHUTTER
#define STRTK_ISO_CLBT_LV11_		ID_STR_ISO_CLBT_LV11_
#define STRTK_DIFF_ISO_OB_CLBT		ID_STR_DIFF_ISO_OB_CLBT
#define STRTK_WB_CLBT		ID_STR_WB_CLBT
#define STRTK_BACK		ID_STR_BACK
#define STRTK_CALIB_AWB		ID_STR_CALIB_AWB
#define STRTK_CREAT_CTT		ID_STR_CREAT_CTT
#define STRTK_CREAT_LV10_CALIBRATION		ID_STR_CREAT_LV10_CALIBRATION
#define STRTK_CREAT_LV10_STANDARD		ID_STR_CREAT_LV10_STANDARD
#define STRTK_FOCUS		ID_STR_FOCUS
#define STRTK_FLASH		ID_STR_FLASH
#define STRTK_PRE_ANALYSES		ID_STR_PRE_ANALYSES
#define STRTK_H_POS_SCAN		ID_STR_H_POS_SCAN
#define STRTK_DURATION_SCAN		ID_STR_DURATION_SCAN
#define STRTK_VIEW_DPC		ID_STR_VIEW_DPC
#define STRTK_CAPTURE_DPC		ID_STR_CAPTURE_DPC
#define STRTK_BATTERY		ID_STR_BATTERY
#define STRTK_BATTERY_TYPE		ID_STR_BATTERY_TYPE
#define STRTK_ALKALINE		ID_STR_ALKALINE
#define STRTK_NIMH		ID_STR_NIMH
#define STRTK_LITHIUM		ID_STR_LITHIUM
#define STRTK_OXYRIDE		ID_STR_OXYRIDE
#define STRTK__CAM_CONFIG		ID_STR__CAM_CONFIG
#define STRTK_FW_UPDATE		ID_STR_FW_UPDATE
#define STRTK_FACTORY_DEFAULT		ID_STR_FACTORY_DEFAULT
#define STRTK_PV_SAVERAW		ID_STR_PV_SAVERAW
#define STRTK_CAP_SAVERAW		ID_STR_CAP_SAVERAW
#define STRTK_RAWBATCH_ENABLE		ID_STR_RAWBATCH_ENABLE
#define STRTK_PREFH_STAMP		ID_STR_PREFH_STAMP
#define STRTK_AF_FULL_SCAN		ID_STR_AF_FULL_SCAN
#define STRTK__TEST_BENCH		ID_STR__TEST_BENCH
#define STRTK_TEST_ROBOT		ID_STR_TEST_ROBOT
#define STRTK__SENSOR		ID_STR__SENSOR
#define STRTK_IRIS_DIFF_LV12		ID_STR_IRIS_DIFF_LV12
#define STRTK_MINI_GAIN_LV12		ID_STR_MINI_GAIN_LV12
#define STRTK_PV_LV_4_16		ID_STR_PV_LV_4_16
#define STRTK_CAP_LV_4_16		ID_STR_CAP_LV_4_16
#define STRTK_P2D		ID_STR_P2D
#define STRTK_CIPA		ID_STR_CIPA
#define STRTK_LCD_TEST		ID_STR_LCD_TEST
#define STRTK_KEY_TEST		ID_STR_KEY_TEST
#define STRTK_AE_METER		ID_STR_AE_METER
#define STRTK__FLASH		ID_STR__FLASH
#define STRTK_CURVE_A0		ID_STR_CURVE_A0
#define STRTK_CURVE_A1		ID_STR_CURVE_A1
#define STRTK_CURVE_B0		ID_STR_CURVE_B0
#define STRTK_CURVE_B1		ID_STR_CURVE_B1
#define STRTK_P2___		ID_STR_P2___
#define STRTK__AF		ID_STR__AF
#define STRTK_NOISE_CORING_VALUE_TEST		ID_STR_NOISE_CORING_VALUE_TEST
#define STRTK_NOISE_CORING_THESHOLD_TEST		ID_STR_NOISE_CORING_THESHOLD_TEST
#define STRTK_NOISE_THESHOID_TEST		ID_STR_NOISE_THESHOID_TEST
#define STRTK_BATTERY_CURVE		ID_STR_BATTERY_CURVE
#define STRTK_AE_DEBUG		ID_STR_AE_DEBUG
#define STRTK_AWB_DEBUG		ID_STR_AWB_DEBUG
#define STRTK_USB_ISP		ID_STR_USB_ISP
#define STRTK_STAND_ALONE_LSC		ID_STR_STAND_ALONE_LSC
#define STRTK_PV_OB_CAL		ID_STR_PV_OB_CAL
#define STRTK_VIDEO_REC_LOG		ID_STR_VIDEO_REC_LOG
#define STRTK_DIGITAL_ZOOM		ID_STR_DIGITAL_ZOOM
#define STRTK_ENTER		ID_STR_ENTER
#define STRTK_DELETE_ALL_FILES_		ID_STR_DELETE_ALL_FILES_
#define STRTK_DELETE_FILES_		ID_STR_DELETE_FILES_
#define STRTK_FORMATTING___		ID_STR_FORMATTING___
#define STRTK_HAS_NO_SIZE_TO_RESIZE_		ID_STR_HAS_NO_SIZE_TO_RESIZE_
#define STRTK_FILE_LOCKED_		ID_STR_FILE_LOCKED_
#define STRTK_PROTECT_FILE_		ID_STR_PROTECT_FILE_
#define STRTK_UNPROTECT_FILE_		ID_STR_UNPROTECT_FILE_
#define STRTK_PROTECT_ALL_		ID_STR_PROTECT_ALL_
#define STRTK_UNPROTECT_ALL_		ID_STR_UNPROTECT_ALL_
#define STRTK_NO_FILE		ID_STR_NO_FILE
#define STRTK_NO_CARD_		ID_STR_NO_CARD_
#define STRTK_REMOVE_CARD_		ID_STR_REMOVE_CARD_
#define STRTK_WAITING___		ID_STR_WAITING___
#define STRTK_CARD_LOCKED_		ID_STR_CARD_LOCKED_
#define STRTK_PAUSE		ID_STR_PAUSE
#define STRTK_REC		ID_STR_REC
#define STRTK_FILE_ERROR_		ID_STR_FILE_ERROR_
#define STRTK_PLEASE_PLUG_OUT_CARD		ID_STR_PLEASE_PLUG_OUT_CARD
#define STRTK_MEMORY_FULL		ID_STR_MEMORY_FULL
#define STRTK_WAITING_FOR_PRINTER		ID_STR_WAITING_FOR_PRINTER
#define STRTK_NO_JPEG		ID_STR_NO_JPEG
#define STRTK_FORMAT_FAIL_		ID_STR_FORMAT_FAIL_
#define STRTK_LOW_BATTERY		ID_STR_LOW_BATTERY
#define STRTK_OUT_OF_BATTERY		ID_STR_OUT_OF_BATTERY
#define STRTK_NOT_SUPPORT_		ID_STR_NOT_SUPPORT_
#define STRTK_CANCEL		ID_STR_CANCEL
#define STRTK_SELECT_UP_TO_24		ID_STR_SELECT_UP_TO_24
#define STRTK_PLEASE_SELECT_A_PICTURE		ID_STR_PLEASE_SELECT_A_PICTURE
#define STRTK_PRINT_ERROR		ID_STR_PRINT_ERROR
#define STRTK_PRINTER_ERROR		ID_STR_PRINTER_ERROR
#define STRTK_NOT_FIND_PRINTER		ID_STR_NOT_FIND_PRINTER
#define STRTK_PICTBRIDGE		ID_STR_PICTBRIDGE
#define STRTK_SET_THIS_PHOTO_		ID_STR_SET_THIS_PHOTO_
#define STRTK_SET_ALL_PHOTO_		ID_STR_SET_ALL_PHOTO_
#define STRTK_REVIEW_THIS_FILE_		ID_STR_REVIEW_THIS_FILE_
#define STRTK_SAVE_THIS_FILE_		ID_STR_SAVE_THIS_FILE_
#define STRTK_DELETE		ID_STR_DELETE
#define STRTK_ALL		ID_STR_ALL
#define STRTK_SELECT		ID_STR_SELECT
#define STRTK_COPY_TO_CARD		ID_STR_COPY_TO_CARD
#define STRTK_COPY_THIS_FILE_		ID_STR_COPY_THIS_FILE_
#define STRTK_COPY_ALL_FILES_		ID_STR_COPY_ALL_FILES_
#define STRTK_ROTATE		ID_STR_ROTATE
#define STRTK_ROTATE_THIS_PHOTO_		ID_STR_ROTATE_THIS_PHOTO_
#define STRTK_SET		ID_STR_SET
#define STRTK_STOP		ID_STR_STOP
#define STRTK_DELETE_VOICE_MEMO_		ID_STR_DELETE_VOICE_MEMO_
#define STRTK_READY_TO_RECORD		ID_STR_READY_TO_RECORD
#define STRTK_READY_TO_PLAY		ID_STR_READY_TO_PLAY
#define STRTK_RECORDING_MEMO___		ID_STR_RECORDING_MEMO___
#define STRTK_PLAYING_AUDIO___		ID_STR_PLAYING_AUDIO___
#define STRTK_TRIM_THIS_PHOTO_		ID_STR_TRIM_THIS_PHOTO_
#define STRTK_COPY_ERROR		ID_STR_COPY_ERROR
#define STRTK_CAPTURE_NOT_ENOUGH_SIZE		ID_STR_CAPTURE_NOT_ENOUGH_SIZE
#define STRTK_CAPTURE_DONE		ID_STR_CAPTURE_DONE
#define STRTK_BUSY___PLEASE_WAIT		ID_STR_BUSY___PLEASE_WAIT
#define STRTK_NOT_AVAILABLE_FOR_THIS_FILE		ID_STR_NOT_AVAILABLE_FOR_THIS_FILE
#define STRTK_IMAGE_SAVED		ID_STR_IMAGE_SAVED
#define STRTK_PAPER_SIZE		ID_STR_PAPER_SIZE
#define STRTK_PAPER_ERROR		ID_STR_PAPER_ERROR
#define STRTK_LOAD_ERROR		ID_STR_LOAD_ERROR
#define STRTK_EJECT_ERROR		ID_STR_EJECT_ERROR
#define STRTK_PAPER_JAM		ID_STR_PAPER_JAM
#define STRTK_INK_EMPTY		ID_STR_INK_EMPTY
#define STRTK_INK_LOW		ID_STR_INK_LOW
#define STRTK_WASTE_INK		ID_STR_WASTE_INK
#define STRTK_FATAL_ERROR		ID_STR_FATAL_ERROR
#define STRTK_SERVICE_CALL		ID_STR_SERVICE_CALL
#define STRTK_PRINTER_BUSY		ID_STR_PRINTER_BUSY
#define STRTK_LEVER_ERROR		ID_STR_LEVER_ERROR
#define STRTK_COVER_OPEN		ID_STR_COVER_OPEN
#define STRTK_INK_COVER_OPEN		ID_STR_INK_COVER_OPEN
#define STRTK_NO_INK_CARTRIDGE		ID_STR_NO_INK_CARTRIDGE
#define STRTK_INFO_ERROR		ID_STR_INFO_ERROR
#define STRTK_USB_ERROR		ID_STR_USB_ERROR
#define STRTK_CONNECTING		ID_STR_CONNECTING
#define STRTK_PRINTING___		ID_STR_PRINTING___
#define STRTK_CANCELLING		ID_STR_CANCELLING
#define STRTK_NO_PAPER		ID_STR_NO_PAPER
#define STRTK_PLAY		ID_STR_PLAY
#define STRTK_DISPLAY_FILE		ID_STR_DISPLAY_FILE
#define STRTK_COMPLETED		ID_STR_COMPLETED
#define STRTK_NOT_SUPPORT_C2C		ID_STR_NOT_SUPPORT_C2C
#define STRTK_FIND_START		ID_STR_FIND_START
#define STRTK_FIND_END		ID_STR_FIND_END
#define STRTK_AND_PRESS_OK		ID_STR_AND_PRESS_OK
#define STRTK_WRITING_VIDEO		ID_STR_WRITING_VIDEO
#define STRTK_WRITING_FILE_FAIL		ID_STR_WRITING_FILE_FAIL
#define STRTK_TRIM_VIDEO_IS_TOO_SHORT		ID_STR_TRIM_VIDEO_IS_TOO_SHORT
#define STRTK_PB_TYPE		ID_STR_PB_TYPE
#define STRTK_VIDEO		ID_STR_VIDEO
#define STRTK_IMAGE		ID_STR_IMAGE
#define STRTK_BURST_LMT_3		ID_STR_BURST_LMT_3
#define STRTK_BURST_LMT_5		ID_STR_BURST_LMT_5
#define STRTK_MFNR		ID_STR_MFNR
#define STRTK_PRESHOT		ID_STR_PRESHOT
#define STRTK_ZEROSHOT		ID_STR_ZEROSHOT
#define STRTK_0_5SEC		ID_STR_0_5SEC
#define STRTK_MEMORY		ID_STR_MEMORY
#define STRTK_INTERNAL		ID_STR_INTERNAL
#define STRTK_CARD		ID_STR_CARD
#define STRTK_USB		ID_STR_USB
#define STRTK_FACE_BEAUTY		ID_STR_FACE_BEAUTY
#define STRTK_SUPER_RESOLUTION		ID_STR_SUPER_RESOLUTION
#define STRTK_USB_DONGLE		ID_STR_USB_DONGLE
#define STRTK_SDIO		ID_STR_SDIO
#define STRTK_WIFI_DEVICE		ID_STR_WIFI_DEVICE
#define STRTK_INSUFFICIENT_DISK_SPACE		ID_STR_INSUFFICIENT_DISK_SPACE
#define STRTK_MONO		ID_STR_MONO
#define STRTK_TOYCAM		ID_STR_TOYCAM
#define STRTK_LIGHTTONE		ID_STR_LIGHTTONE
#define STRTK_WAITING_FOR_CAPTURE		ID_STR_WAITING_FOR_CAPTURE
#define STRTK_WAITING_FOR_CARD		ID_STR_WAITING_FOR_CARD
#define STRTK_FHD_720P		ID_STR_FHD_720P
#define STRTK_FHD_VGA		ID_STR_FHD_VGA
#define STRTK_FHD_FHD		ID_STR_FHD_FHD
#define STRTK_TIME_LAPSE		ID_STR_TIME_LAPSE
#define STRTK_INVERT_MODE		ID_STR_INVERT_MODE
#define STRTK_DISCONNECTING		ID_STR_DISCONNECTING
#define STRTK_CARD_SPEED_SLOW		ID_STR_CARD_SPEED_SLOW
#define STRTK_POWER_ON_RECORD		ID_STR_POWER_ON_RECORD
#define STRTK_JPG		ID_STR_JPG
#define STRTK_EVENT		ID_STR_EVENT
#define STRTK_PROTECT		ID_STR_PROTECT
#define STRTK_PROTECT_ALL		ID_STR_PROTECT_ALL
#define STRTK_UNPROTECT_ALL		ID_STR_UNPROTECT_ALL
#define STRTK_START_UP		ID_STR_START_UP

extern UINT16 CustomIconPtr[];
extern UINT32 CustomMenuRootStart[];
extern UINT32 CustomMenuIndex[];
extern UINT8 CustomMenuNodeItem[];
extern UINT8 CustomMenuSubItem[];
extern menufunc CustomMenuFunc[];
extern menuexe CustomMenuExe[];
extern UINT16 CustomMenuParaTab[];

#endif /* _MENU_TAB_H_ */
