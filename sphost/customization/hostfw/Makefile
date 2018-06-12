#/**************************************************************************
# *
# *       Copyright (c) 2002-2018 by iCatch Technology, Inc.
# *
# *  This software is copyrighted by and is the property of iCatch
# *  Technology, Inc.. All rights are reserved by iCatch Technology, Inc..
# *  This software may only be used in accordance with the corresponding
# *  license agreement. Any unauthorized use, duplication, distribution,
# *  or disclosure of this software is expressly forbidden.
# *
# *  This Copyright notice MUST not be removed or modified without prior
# *  written consent of iCatch Technology, Inc..
# *
# *  iCatch Technology, Inc. reserves the right to modify this software
# *  without notice.
# *
# *  iCatch Technology, Inc.
# *  19-1, Innovation First Road, Science-Based Industrial Park,
# *  Hsin-Chu, Taiwan.
# *
# **************************************************************************/
#init empty simple flavor variables before any include
CFLAGS :=
ASFLAGS :=


HOST_ROOT := ../..
include $(HOST_ROOT)/host.def
ifndef PLATFORM_PATH
PLATFORM_PATH := $(PLATFORM)
endif
PLATFORM_ROOT := $(HOST_ROOT)/../$(PLATFORM_PATH)
TOOL_PATH := $(HOST_ROOT)/../tool

# output
SRC_DIR   := .
DEST_DIR  := $(HOST_ROOT)/_$(PRJ)
OBJS_PATH := $(DEST_DIR)/_$(PLATFORM_PATH)

# Include the definition of tool chain
include $(PLATFORM_ROOT)/tool_chain.def
include $(DEST_DIR)/project/$(PRJ).def

#---------------------------------------------------------------------------
# Variable definition:
#   1. Cross-compling tools
#   2. Linker script file
#   3. C complier options
#   4. Assembly compiler options
#   5. Linker options
#---------------------------------------------------------------------------
GOAL := libhostfw.a

CFLAGS += -Wall
CFLAGS += -mgp32
#CFLAGS += -ansi
CFLAGS += -G 0

ifeq ($(usboot), 1)
CFLAGS += -DOPT_USB_BOOT=1
else
CFLAGS += -DOPT_USB_BOOT=0
endif

ifeq ($(NDK2), YES)
APP_OSS_INC_PATH := $(HOST_ROOT)/include/thirdparty/oss2
APP_NET_INC_PATH := $(HOST_ROOT)/include/thirdparty/net2
else
APP_OSS_INC_PATH := $(HOST_ROOT)/include/thirdparty/oss
APP_NET_INC_PATH := $(HOST_ROOT)/include/thirdparty/net
endif

CFLAGS += -I$(PLATFORM_ROOT)/inc
CFLAGS += -I$(PLATFORM_ROOT)/inc/api
CFLAGS += -I$(HOST_ROOT)/include
CFLAGS += -I./include
CFLAGS += -I$(DEST_DIR)/include
CFLAGS += -I./device
#CFLAGS += -I./device/lens
CFLAGS += -I../../include
CFLAGS += -I../../include/thirdparty/iq
CFLAGS += -I$(APP_NET_INC_PATH)
CFLAGS += -I$(APP_OSS_INC_PATH)
ifeq ($(UVC_MODE), YES)
CFLAGS += -I$(APP_OSS_INC_PATH)/linuxcompat
CFLAGS += -I$(APP_OSS_INC_PATH)/linuxbase
CFLAGS += -I$(APP_OSS_INC_PATH)/linuxusbhost
CFLAGS += -I$(APP_OSS_INC_PATH)/linuxusbhost/linux
endif
ifeq ($(SSLEN), YES)
CFLAGS += -I$(APP_OSS_INC_PATH)/linuxcompat
CFLAGS += -I$(APP_OSS_INC_PATH)/linuxbase
CFLAGS += -I$(APP_OSS_INC_PATH)/linuxusbhost
CFLAGS += -I$(APP_OSS_INC_PATH)/linuxusbhost/linux
endif

CFLAGS += -G 8
CFLAGS += -DAPP_BUILD=1

CFLAGS += -I./include

# Isaac add include try to fixed about the g sensor compiler error
CFLAGS += -I$(HOST_ROOT)/customization/drivers/gsensor

CGDBFLAGS = $(CFLAGS)
CGDBFLAGS += -g

CFLAGS += -gstabs+
CFLAGS += -Os

ifeq ($(IMAGE_EFFECT), YES)
CFLAGS += -DIMG_EFFECT=1
endif

ifeq ($(APP_VIDEO_EIS), YES)
CFLAGS += -DAPP_VIDEO_EIS=1
endif

ifeq ($(LIVEST),1)
CFLAGS += -DICAT_STREAMING=1
CFLAGS 	+= -I${PLATFORM_ROOT}/resource/stream_lib/include
endif

ifeq ($(RTMP), YES)
CFLAGS += -DAPP_RTMP=1
endif

ifeq ($(USBNET),YES)
CFLAGS += -DUSB_NET_SUPPORT
endif

ifeq ($(nand), 1)
CFLAGS += -DSP5K_DISK_NAND=1
endif

ifeq ($(ETHERDRV), YES)
CFLAGS += -DICAT_ETHERNET=1
endif
ifeq ($(spi), 1)
CFLAGS += -DSP5K_DISK_SPI=1
endif

#---------------------------------------------------------------------------
# Intermediate objects
#---------------------------------------------------------------------------
OBJS :=
OBJS += app_ver.o
ifeq ($(PRJ_NAME),$(filter $(PRJ_NAME),17DVR 19DVR))
OBJS += app_infohandler.o
OBJS += app_infohandler_func.o
OBJS += app_infohandler_feedback.o
OBJS += jet_utility.o
OBJS += jet_cdfs.o
endif
OBJS += sys/app_main.o
OBJS += sys/app_power_on.o
OBJS += sys/app_power_off.o
OBJS += sys/app_batcharging.o
OBJS += sys/app_fast_boot.o
OBJS += sys/app_power_hibernate.o
OBJS += sys/app_uart2.o
ifneq ($(JET_FEATURE),)
OBJS += sys/app_i2ctool.o
OBJS += sys/app_cmd_gsensor.o
OBJS += sys/app_jet_power_off.o
OBJS += sys/app_jet_sd_cert.o
OBJS += service/misc/app_jet_cmd.o
endif
OBJS += sys/state_ctrl/app_state_ctrl.o
OBJS += sys/state_ctrl/app_state_null.o
OBJS += sys/state_ctrl/app_state_preproc.o
OBJS += sys/state_ctrl/app_state_postproc.o
ifeq ($(PRJ_NAME),DUAL2)
OBJS += sys/i2c/app_i2c_lib.o
OBJS += sys/i2c/app_i2c_util.o
OBJS += sys/i2c/app_i2c_lib_icatch.o
else ifeq ($(PRJ_NAME),SBC)
ifeq ($(SENSOR_NAME), $(filter $(SENSOR_NAME), FAKE))
OBJS += sys/i2c/app_i2c_lib.o
OBJS += sys/i2c/app_i2c_util.o
OBJS += sys/i2c/app_i2c_lib_icatch.o
endif
endif
OBJS += device/app_disk.o
OBJS += device/app_timer.o
OBJS += device/app_timer_isr.o
OBJS += device/app_key.o
OBJS += device/app_key_name.o
OBJS += device/app_tvlcd.o
OBJS += device/app_sound.o
OBJS += device/app_sensor.o
OBJS += device/app_battery.o
OBJS += device/app_dev_plug.o
OBJS += device/app_buzzer.o
OBJS += device/app_uvc_request.o
#OBJS += device/lens/app_lens_sample.o	         # device_lens section
#OBJS += device/lens/app_lens_mshut.o
#OBJS += device/VCM/DrvDW9800W/VCM_DrvDW9800W.o
OBJS += device/app_strobe.o
OBJS += device/app_rtc.o
OBJS += device/app_led.o
ifeq ($(GPS_OPEN),1)
OBJS += device/app_gps.o
endif
OBJS += service/misc/app_misc.o
OBJS += service/misc/app_msg_ext.o
OBJS += service/misc/app_sio_filter.o
OBJS += service/misc/app_custom_cmd.o
OBJS += service/misc/app_param_manager.o
OBJS += service/misc/app_view_param.o
OBJS += service/misc/app_background_timer.o
OBJS += service/misc/app_user_setting.o
OBJS += service/misc/app_errorosd.o
OBJS += service/misc/app_error_msg.o
OBJS += service/misc/app_jpg_draw.o
OBJS += service/misc/app_card_isp.o
OBJS += service/misc/app_simple_menu.o
OBJS += service/misc/app_yuv_illustrator.o
OBJS += service/misc/app_modeSet.o
OBJS += service/misc/tg_cmd.o
OBJS += service/misc/app_battery_ctrl.o
OBJS += service/misc/app_battery_charge.o
OBJS += service/misc/app_spca7002_utility.o
OBJS += service/misc/app_sd_log.o
OBJS += service/misc/app_watch_dog.o
OBJS += service/misc/app_dead_process.o
OBJS += service/misc/app_facedetect_ex.o
OBJS += service/misc/app_yuvscale.o
OBJS += service/osd/app_osd_init.o
OBJS += service/osd/app_osd_lib.o
OBJS += service/osd/app_osd_api.o
OBJS += service/exif/app_exif.o
OBJS += service/dcf/app_dcf.o
OBJS += service/cdfs/app_cdfs_api.o
OBJS += service/dpof/app_dpof.o
#OBJS += service/calib/app_calib.o
#OBJS += service/calib/app_camcfg_main.o
#OBJS += service/calib/app_calib_api.o
#OBJS += service/calib/app_calib_zoom.o
#OBJS += service/calib/app_calib_awb.o
OBJS += service/script/app_script.o
OBJS += service/script/app_script_lib.o
#OBJS += service/testbed/app_tb_main.o
#OBJS += service/testbed/app_tb_util.o
#OBJS += service/testbed/app_tb_unit_test.o
#OBJS += service/testbed/app_tb_batcurve.o
#OBJS += service/testbed/app_tb_cipa.o
#OBJS += service/syscfg/app_is_cfg.o
OBJS += ui/still/app_zoom_api.o
OBJS += ui/still/app_still_view.o
OBJS += ui/still/app_still_callback.o
OBJS += ui/still/app_still_capture.o
OBJS += ui/still/app_still_burst.o
OBJS += ui/still/app_still_setting.o
OBJS += ui/still/app_view_osd.o
OBJS += ui/still/app_still_background.o
OBJS += ui/still/app_still_busy.o
OBJS += ui/still/app_still_aaa.o
OBJS += ui/still/app_still_flow.o
OBJS += ui/still/app_still_util.o
ifeq ($(UVC_MODE), YES)
OBJS += ui/video/app_video_uvc_setting.o
OBJS += ui/video/app_video_uvc.o
endif
OBJS += ui/video/app_video_view.o
OBJS += ui/video/app_video_record.o
OBJS += ui/video/app_video_setting.o
OBJS += ui/video/app_video_osd.o
OBJS += ui/video/app_video_stamp.o
OBJS += ui/audio/app_audio_record.o
OBJS += ui/play/app_playback_main.o
OBJS += ui/play/app_playback_prot.o
OBJS += ui/play/app_playback_del.o
OBJS += ui/play/app_playback_video.o
OBJS += ui/play/app_playback_lib.o
OBJS += ui/play/app_playback_osd.o
OBJS += ui/menu/app_ui_para.o
OBJS += ui/menu/app_menu_state.o

ifeq ($(PRJ_NAME), EVB)
OBJS += ui/menu/app_menu_tab_evb.o
else
ifeq ($(PRJ_NAME), SBC)
OBJS += ui/menu/app_menu_tab_sbc.o
else
ifeq ($(PRJ_NAME), DUAL2)
OBJS += ui/menu/app_menu_tab_dual2.o
else
ifeq ($(PRJ_NAME), 17DVR)
OBJS += ui/menu/app_menu_tab_17dvr.o
else
ifeq ($(PRJ_NAME), 19DVR)
OBJS += ui/menu/app_menu_tab_19dvr.o
else
ifeq ($(PRJ_NAME), JETDVR)
OBJS += ui/menu/app_menu_tab_jetdvr.o
else
ifeq ($(PRJ_NAME), PDVR)
OBJS += ui/menu/app_menu_tab_pdvr.o
else
error "define new project?"
endif #($(PRJ_NAME), DUAL2)
endif #($(PRJ_NAME), SBC)
endif #($(PRJ_NAME), EVB)
endif #($(PRJ_NAME), 17DVR)
endif #($(PRJ_NAME), JETDVR)
endif #($(PRJ_NAME), PDVR)
endif #($(PRJ_NAME), 19DVR)

OBJS += ui/menu/app_menu_core.o
OBJS += ui/menu/app_menu_disp.o
OBJS += ui/menu/app_menu_disp1.o
OBJS += ui/menu/app_menu_event_cvr.o
OBJS += ui/menu/app_menu_active_cvr.o
OBJS += ui/menu/app_menu_frameosd.o
OBJS += ui/setup/app_setup_lib.o
OBJS += ui/usb/app_usb_create.o
OBJS += ui/usb/app_usb_msdc.o
OBJS += ui/usb/app_usb_pccam.o
OBJS += ui/hot_plug/app_tv_plug.o
OBJS += ui/hot_plug/app_HDMI_plug.o
OBJS += ui/hot_plug/app_cec_capability_ctrl.o
OBJS += ui/hot_plug/app_card_plug.o
OBJS += ui/util/app_callback_setting.o

ifeq ($(TEST_ADAS), YES)
	CFLAGS += -DTEST_ADAS=1
	OBJS += service/adas/app_test_adas.o
	OBJS += service/adas/app_test_adas_ldws.o
	OBJS += service/adas/app_test_adas_fcws.o
	OBJS += service/adas/app_test_adas_lpd.o
	OBJS += service/adas/app_test_adas_rod.o
else
	CFLAGS += -DTEST_ADAS=0
endif

ifeq ($(TEST_ADAS_PRODUCT_OSD), YES)
	CFLAGS += -DTEST_ADAS_PRODUCT_OSD=1
else
	CFLAGS += -DTEST_ADAS_PRODUCT_OSD=0
endif

ifeq ($(TEST_ADAS_DEBUG_DISPALY_OSD), YES)
	CFLAGS += -DTEST_ADAS_DEBUG_DISPALY_OSD=1
else
	CFLAGS += -DTEST_ADAS_DEBUG_DISPALY_OSD=0
endif

ifeq ($(TEST_ADAS_DEBUG_REC_RESULT_VIDEO), YES)
	CFLAGS += -DTEST_ADAS_DEBUG_REC_RESULT_VIDEO=1
else
	CFLAGS += -DTEST_ADAS_DEBUG_REC_RESULT_VIDEO=0
endif

ifeq ($(TEST_ADAS_DEMO), YES)
	CFLAGS += -DTEST_ADAS_DEMO=1
	ifeq ($(TEST_ADAS_DEMO_OSD), YES)
		CFLAGS += -DTEST_ADAS_DEMO_OSD=1
	else
		CFLAGS += -DTEST_ADAS_DEMO_OSD=0
	endif

	ifeq ($(TEST_ADAS_DEMO_REC_RESULT_VIDEO), YES)
		CFLAGS += -DTEST_ADAS_DEMO_REC_RESULT_VIDEO=1
	else
		CFLAGS += -DTEST_ADAS_DEMO_REC_RESULT_VIDEO=0
	endif
else
	CFLAGS += -DTEST_ADAS_DEMO=0
	CFLAGS += -DTEST_ADAS_DEMO_REC_RESULT_VIDEO=0
	CFLAGS += -DTEST_ADAS_DEMO_OSD=0
endif

ifeq ($(TEST_ADAS_LDWS), YES)
	CFLAGS += -DTEST_ADAS_LDWS=1
else
	CFLAGS += -DTEST_ADAS_LDWS=0
endif

ifeq ($(TEST_ADAS_FCWS), YES)
	CFLAGS += -DTEST_ADAS_FCWS=1
else
	CFLAGS += -DTEST_ADAS_FCWS=0
endif

ifeq ($(TEST_ADAS_LPD), YES)
	CFLAGS += -DTEST_ADAS_LPD=1
else
	CFLAGS += -DTEST_ADAS_LPD=0
endif

ifeq ($(TEST_ADAS_ROD), YES)
	CFLAGS += -DTEST_ADAS_ROD=1
else
	CFLAGS += -DTEST_ADAS_ROD=0
endif

ifeq ($(ALGORITHM_DUAL_AE), 1)
OBJS += aaa/app_ae_dual.o
else
OBJS += aaa/app_ae.o
endif
OBJS += aaa/app_awb.o
OBJS += aaa/app_still_awb.o
#OBJS += aaa/app_still_flash.o
OBJS += service/script/app_script.o
OBJS += service/script/app_script_lib.o
ifeq ($(SSLEN), YES)
OBJS += service/pcs/app_utility_min_ini.o
OBJS += service/pcs/app_http_parser.o
OBJS += service/pcs/app_http_utility.o
OBJS += service/pcs/app_ssl_utility.o
OBJS += service/pcs/app_sockel_utility.o

OBJS += service/pcs/app_pcs_if.o
OBJS += service/pcs/app_dropbox_http.o
endif
#---------------------------------------------------------------------------
# Referring to generic rules
#---------------------------------------------------------------------------
$(eval $(call GEN_GOAL_RULES))
