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
# *  Author: Anmin Deng
# *
# **************************************************************************/
#init empty simple flavor variables before any include
CFLAGS :=
ASFLAGS :=
CFG_DEFS :=
CFG_HEADERS :=
LDFLAGS :=
LDFLAGS_BSS :=
RST_OBJS :=
INI_OBJS :=
INT_OBJS := #EXT_OBJS of basefw
GEN_OBJS :=
CUSTOM_SUBLIBS :=
CUSTOM_SUBDIRS :=
CUSTOM_LIBS :=
PRJ_LIB :=
SUBLIBS :=
SUBDIRS :=
LIBS :=
BIN :=
CDSP_BIN :=
DAT8 :=
DAT16 :=
DAT32 :=
PRJ_BIN :=
PRJ_DAT8 :=
PRJ_DAT16 :=
PRJ_DAT32 :=
SHARED_BIN :=
SHARED_DAT8 :=
SHARED_DAT16 :=
SHARED_DAT32 :=
SHARED2_BIN :=
SHARED2_DAT8 :=
SHARED2_DAT16 :=
SHARED2_DAT32 :=
SHARED3_BIN :=
SHARED3_DAT8 :=
SHARED3_DAT16 :=
SHARED3_DAT32 :=
LDSCRIPT_CPPFLAGS :=

FRM_PARTIAL_DIRS :=
FRM_PARTIAL_EXCLUDE :=

# include & environment
HOST_MAKE := YES
HOST_ROOT := .
SRC_PATH        := customization
HOST_FW_PATH    := $(SRC_PATH)/hostfw
TOOL_PATH       := ../tool
LD_PATH         := linker_script
RES_PATH        := resource

include $(HOST_ROOT)/host.def
ifndef PLATFORM_PATH
PLATFORM_PATH   := $(PLATFORM)
endif
PLATFORM_ROOT   := ../$(PLATFORM_PATH)

APP_NDK_PATH := $(HOST_FW_PATH)/service/network
ifeq ($(NDK2), YES)
APP_OSS_LIB_PATH := $(THIRD_LIB_PATH)/oss2
APP_OSS_INC_PATH := $(THIRD_INC_PATH)/oss2
APP_NET_LIB_PATH := $(THIRD_LIB_PATH)/net2
APP_NET_INC_PATH := $(THIRD_INC_PATH)/net2
else
APP_OSS_LIB_PATH := $(THIRD_LIB_PATH)/oss
APP_OSS_INC_PATH := $(THIRD_INC_PATH)/oss
APP_NET_LIB_PATH := $(THIRD_LIB_PATH)/net
APP_NET_INC_PATH := $(THIRD_INC_PATH)/net
endif
APP_RTMP_LIB_PATH := $(THIRD_LIB_PATH)/rtmp

# output
DEST_DIR        := _$(PRJ)
OBJS_PATH       := $(DEST_DIR)/_$(PLATFORM_PATH)
DOWN_PATH       := $(DEST_DIR)/download
RO_DOWN_PATH    := $(DOWN_PATH)/A/RO_RES
USER_DOWN_PATH  := $(DOWN_PATH)/B/USER_RES
SYS_RES_PATH    := $(PLATFORM_ROOT)/resource


# PLATFORM = 5210 or spca5310 which is defined in tool_chain.def
include $(PLATFORM_ROOT)/tool_chain.def

# dbg lib suffix, unmark it to enable debug library (PROC INFO,
# MEMORY CALL STACK INFO)
#DBG_SUFFIX = .dbg

# project
LIB_PATH      := $(PLATFORM_ROOT)/lib$(pathext)
ARCH_PATH     := $(SRC_PATH)/arch/$(PLATFORM)
ifneq ($(PLATFORM), spca5210)
DISP_DRV_PATH := $(SRC_PATH)/drivers/disp
else
DISP_DRV_PATH := $(SRC_PATH)/drivers/disp.5210
endif
ifneq ($(PLATFORM), spca5310)
SEN_DRV_PATH  := $(ARCH_PATH)/drivers/sensor
else
SEN_DRV_PATH  := $(SRC_PATH)/drivers/sensor
endif

GYRO_DRV_PATH := $(SRC_PATH)/drivers/gyro
GSEN_DRV_PATH := $(SRC_PATH)/drivers/gsensor

# 3rd party
#RELEASE_TO_3RDPARTY := 1

ifeq ($(FPGA), YES)
ISP_BIN := $(PLATFORM)isp_fpga.bin
else
ISP_BIN := $(PLATFORM)isp.bin
endif

ISP_BIN_RES := $(SYS_RES_PATH)/$(ISP_BIN)
ifeq (,$(wildcard $(ISP_BIN_RES)))
 ISP_BIN_RES := $(RES_PATH)/ISP/$(ISP_BIN)
endif

FRM_INI := frm_user.ini

# define FRM_DRAM_PRM in project def to use project-specific prm
ifdef FRM_DRAM_PRM
 FRM_DRAM_PRM_SRC := $(RES_PATH)/$(PRJ_NAME)/$(FRM_DRAM_PRM)
else
 ifneq ($(PLATFORM),spca5210)
  FRM_DRAM_PRM := drampara-ddr2-5310-189MHz.prm
  ifneq (,$(findstring DCPU_CLK=2,$(CFLAGS)))
   FRM_DRAM_PRM := drampara-ddr2-5310-162MHz.prm
  endif
 else
  FRM_DRAM_PRM := drampara-ddr2-5210-162MHz.prm
 endif
 FRM_DRAM_PRM_SRC := $(RES_PATH)/ISP/$(FRM_DRAM_PRM)
endif

#define FRM_LED_GRP & FRM_LED_PIN in project def to use project-specific LED
ifndef FRM_LED_GRP
 FRM_LED_GRP := 0 # 0:TGL 1:TGH 2:GEN ..., check FRM for details
endif
ifndef FRM_LED_PIN
 FRM_LED_PIN := 1
endif

#command
ifneq (file,$(origin SORT))
ifeq (,$(wildcard $(PREFIX)gcc))
CC      := $(PREFIX)/gcc
CPP     := $(PREFIX)/cpp
AR      := $(PREFIX)/ar
OBJDUMP := $(PREFIX)/objdump
OBJCOPY := $(PREFIX)/objcopy
SIZE    := $(PREFIX)/size
NM      := $(PREFIX)/nm
CXX     := $(PREFIX)/g++
else
CC      := $(PREFIX)gcc
CPP     := $(PREFIX)cpp
AR      := $(PREFIX)ar
OBJDUMP := $(PREFIX)objdump
OBJCOPY := $(PREFIX)objcopy
SIZE    := $(PREFIX)size
NM      := $(PREFIX)nm
CXX     := $(PREFIX)g++
endif
ECHO    := echo
CP      := /bin/cp -f
MV      := /bin/mv -f
MKDIR   := /bin/mkdir -p
RM      := /bin/rm -f
RMR     := /bin/rm -rf
CAT     := /bin/cat
TOUCH   := /bin/touch
SED     := /bin/sed
GREP    := /bin/grep
DIFF    := /usr/bin/diff
SORT    := /usr/bin/sort
FIND    := /usr/bin/find
CSPLIT  := /usr/bin/csplit
endif
ifneq (file,$(origin PATCH))
PATCH   := /usr/bin/patch
endif
ifneq (file,$(origin TR))
TR      := /usr/bin/tr
WC      := /usr/bin/wc
CUT     := /usr/bin/cut
endif

#---------------------------------------------------------------------------
# Default definition:
#---------------------------------------------------------------------------
ifeq (,$(wildcard $(DEST_DIR)/multiboot.def))
-include $(DEST_DIR)/multiboot.def
include $(HOST_ROOT)/multiboot.def.tmpl
else
include $(DEST_DIR)/multiboot.def
endif

CFG_DEFS += multiboot.def

#---------------------------------------------------------------------------
# Variable definition:
#   1. Cross-compling tools
#   2. Linker script file
#   3. C complier options
#   4. Assembly compiler options
#   5. Linker options
#---------------------------------------------------------------------------
CFLAGS += -Wall
CFLAGS += -mgp32
CFLAGS += -ansi
CFLAGS += -I$(DEST_DIR)/include
CFLAGS += -I$(PLATFORM_ROOT)/inc
CFLAGS += -I$(PLATFORM_ROOT)/inc/api
CFLAGS += -I$ include/thirdparty/iq
CFLAGS += -I$(APP_OSS_INC_PATH)
ifeq ($(findstring YES,$(WIFION) $(ETHERDRV)), YES)
CFLAGS += -I$(APP_OSS_INC_PATH)/linuxbase
CFLAGS += -I$(APP_NET_INC_PATH)
endif
CFLAGS += -G 8
CFLAGS += -Os

ifeq ($(DDR3), YES)
CFLAGS += -DDDR3
else ifeq ($(DDR2), YES)
CFLAGS += -DDDR2
else ifeq ($(MDDR), YES)
CFLAGS += -DMDDR
else ifeq ($(DDR), YES)
CFLAGS += -DDDR
CFLAGS += -DDDR_64MB
else
# default
CFLAGS += -DDDR3
endif

LDSCRIPT_CPPFLAGS += -DLIB_PATH=$(LIB_PATH)

ifeq ($(nand), 1)
TARGET := 1
LDSCRIPT := $(LD_PATH)/$(PLATFORM)dev.ld
CFLAGS += -DRAM_PROGRAM=1
ifeq ($(paramfind), 1)
LDSCRIPT_CPPFLAGS += -DRAM_PARAM_FIND_IMEM=1
endif
endif

ifeq ($(spi), 1)
TARGET := 1
LDSCRIPT := $(LD_PATH)/$(PLATFORM)dev.ld
CFLAGS += -DRAM_PROGRAM=1
endif

ifeq ($(ram), 1)
TARGET := 1
LDSCRIPT := $(LD_PATH)/$(PLATFORM)ram.ld
CFLAGS += -DRAM_BOOT=1
endif

# build to rom
ifeq ($(rom),1)
TARGET := 1
ifeq ($(BEV),1)
LDSCRIPT := $(LD_PATH)/$(PLATFORM)rom_bev.ld
else
LDSCRIPT := $(LD_PATH)/$(PLATFORM)rom.ld
endif
endif

# default build to nand
ifndef TARGET
TARGET := 0
nand := 1
LDSCRIPT := $(LD_PATH)/$(PLATFORM)dev.ld
CFLAGS += -DRAM_PROGRAM=1
endif

ifeq ($(ram4g),1)
LDSCRIPT_CPPFLAGS += -DRAM_4GBIT_SUPPORT=1
endif

ifeq ($(isrimem),1)
CFLAGS += -DISR_ON_IMEM=1
LDSCRIPT_CPPFLAGS += -DISR_ON_IMEM=1
endif

ifdef DBG_SUFFIX
CFLAGS += -DLIB_DEBUG_MODE=1
endif

LDFLAGS += -static
LDFLAGS += -nostdlib
LDFLAGS += -G 512

LDFLAGS_BSS += -R .sbss
LDFLAGS_BSS += -R .bss

ifeq ($(findstring YES,$(WIFION) $(ETHERDRV) $(RTMP)), YES)
CFLAGS += -DCXX_SUPPORT=1
endif

#---------------------------------------------------------------------------
# Intermediate objects
#---------------------------------------------------------------------------
ifeq ($(PLATFORM_VER),B)
MASKROM_ELF := $(SYS_RES_PATH)/$(PLATFORM)maskrom_b.elf
else
MASKROM_ELF := $(SYS_RES_PATH)/$(PLATFORM)maskrom.elf
ifeq ($(TOOLCHAIN), ICATCHTEK)
# MIDE tool chain not supporting ASSERT in linker script
LDSCRIPT_CPPFLAGS += -DASSERT_IMAGE_SIZE_8MB=1
endif
LDSCRIPT_CPPFLAGS += -DMIPS24K_ERRATA_E156_J_EXC_INV_EPC_HACK
endif

ifeq ($(TOOLCHAIN), ICATCHTEK)
# MIDE tool chain not supporting ASSERT in linker script
LDSCRIPT_CPPFLAGS += -DASSERT_BKDOOR_SIZE_16KB=1
# MIDE tool chain not supporting .rel.dyn section in linker script
LDSCRIPT_CPPFLAGS += -DREL_DYN_SECTION_SUPPORT=1
endif

ifneq ($(PLATFORM), spca5210)
RST_OBJS += $(LIB_PATH)/reset.o
endif

ifeq ($(MULTI_BOOT),YES)

LDSCRIPT_CPPFLAGS += -DMULTI_BOOT=1

STAGE_OBJS :=
STAGE_OBJS += $(ARCH_PATH)/crt0.o
STAGE_OBJS += $(SRC_PATH)/init.o
STAGE_OBJS += $(SRC_PATH)/disp_spi.nostage$(MULTI_BOOT_STAGE_DISP).o
STAGE_OBJS += $(SRC_PATH)/app_init.nostage$(MULTI_BOOT_STAGE_DISK_RSV).o
STAGE_OBJS += $(ARCH_PATH)/dev_init.o
STAGE_OBJS += $(SRC_PATH)/multiboot_custom.o
STAGE_OBJS += $(SRC_PATH)/sensor_custom.nostage$(MULTI_BOOT_STAGE_SENSOR).o
STAGE_OBJS += $(SRC_PATH)/command.nostage$(MULTI_BOOT_STAGE_CMD).o
STAGE_OBJS += $(SRC_PATH)/sp5k_job.nostage$(MULTI_BOOT_STAGE_HOST).o
STAGE_OBJS += $(SRC_PATH)/drivers/if/if_v35.o

BUILD_STAGE_OBJS := $(addprefix $(OBJS_PATH)/, $(STAGE_OBJS))

BUILD_NOSTAGE_OBJS += $(shell $(ECHO) '$(BUILD_STAGE_OBJS)' | $(TR) ' ' '\n' \
			| $(SED) -n -e '/^.*\.nostage.*\.o$$/p')


ifeq ($(nand),1)
LDSCRIPT_CPPFLAGS += -DBOOT_DEV=nand
## INT_OBJS += $(LIB_PATH)/bkdoor_nand.o  # moved to linker script
INT_OBJS += $(LIB_PATH)/multiboot_nand_ini.o
endif

ifeq ($(spi),1)
LDSCRIPT_CPPFLAGS += -DBOOT_DEV=spi
## INT_OBJS += $(LIB_PATH)/bkdoor_spi.o  # moved to linker script
INT_OBJS += $(LIB_PATH)/multiboot_spi_ini.o
endif

GEN_OBJS += $(SRC_PATH)/multiboot_custom.o

endif #ifeq ($(MULTI_BOOT),YES)

INI_OBJS += $(ARCH_PATH)/crt0.o

ifneq (,$(or $(nand),$(spi)))
INI_OBJS += $(SRC_PATH)/boot_param.o
endif

INI_OBJS += $(SRC_PATH)/init.o

GEN_OBJS += $(SRC_PATH)/heap_init.o
GEN_OBJS += $(SRC_PATH)/disp_spi.o
GEN_OBJS += $(SRC_PATH)/app_init.o
GEN_OBJS += $(ARCH_PATH)/dev_init.o
GEN_OBJS += $(SRC_PATH)/fastboot_custom.o
GEN_OBJS += $(SRC_PATH)/sensor_custom.o
GEN_OBJS += $(SRC_PATH)/command.o
GEN_OBJS += $(SRC_PATH)/sp5k_job.o
GEN_OBJS += $(SRC_PATH)/drivers/if/if_v35.o


ifeq ($(findstring YES,$(WIFION) $(ETHERDRV)), YES)

# Default settings
WIFI_MODULE ?= rtk
LIVEST      ?= 0
export LIVEST WIFI_MODULE WIFI

#CUSTOM_SUBLIBS +=  $(APP_NDK_PATH)/libWiFi.a

ifeq ($(NDK2),YES)

#Bluetooth protocol stack.
ifeq ($(BTON),YES)
	CUSTOM_LIBS += $(APP_NET_LIB_PATH)/libbluez_v35.a
endif

ifeq ($(SSLEN),YES)
	CUSTOM_LIBS += $(APP_NET_LIB_PATH)/libopenssl_apps_v35.a
	CUSTOM_LIBS += $(APP_NET_LIB_PATH)/libssl_v35.a
	CUSTOM_LIBS += $(APP_NET_LIB_PATH)/libcrypto_v35.a
	CUSTOM_LIBS += $(APP_NET_LIB_PATH)/libcrypto_1_v35.a
	CUSTOM_LIBS += $(APP_NET_LIB_PATH)/libcrypto_v35.a
	CUSTOM_LIBS += $(APP_NET_LIB_PATH)/libaes_v35.a
endif

CUSTOM_LIBS += $(APP_NET_LIB_PATH)/liblivest_v35.a
CUSTOM_LIBS += $(APP_NET_LIB_PATH)/libsocutils_v35.a

ifeq ($(WIFION), YES)
CUSTOM_LIBS += $(APP_NET_LIB_PATH)/libwext_v35.a
endif

# mac80211
ifeq ($(MAC80211),YES)
	# v2.6 is for softmac and fullmac wifi drivers
	CUSTOM_LIBS += $(APP_NET_LIB_PATH)/libwpas-2.6_v35.a
	CUSTOM_LIBS += $(APP_NET_LIB_PATH)/libhapd-2.6_v35.a
	CUSTOM_LIBS += $(APP_NET_LIB_PATH)/libwpas_hapd_cmn-2.6_v35.a
else
	# v0.8 is for realtek wifi only
	CUSTOM_LIBS += $(APP_NET_LIB_PATH)/libwpashapd-0.8_v35.a
endif # MAC80211

ifeq ($(SMB_CLIENT),YES)
CUSTOM_LIBS += $(wildcard $(APP_NET_LIB_PATH)/libsmbclient_v35.a)
endif

# wifi module
ifeq ($(WIFION), YES)
ifeq ($(WIFI_NAME),MrvlSD8801)
	CUSTOM_LIBS += $(APP_NET_LIB_PATH)/libMrvlSd8801_v35.a
	CUSTOM_LIBS += $(APP_NET_LIB_PATH)/libMrvlUaputl_v35.a
	CUSTOM_LIBS += $(APP_NET_LIB_PATH)/libMrvlMfgBrdg_v35.a
else ifneq (,$(findstring $(WIFI_NAME),BCM43438 BCM43455))
	CUSTOM_LIBS += $(APP_NET_LIB_PATH)/libdhdutils_v35.a
	CUSTOM_LIBS += $(APP_NET_LIB_PATH)/libbcmdhd.1.363.59.144.x.cn_v35.a
else
	#realtek wifi
	ifeq ($(WIFI_NAME),RTL8723BS)
		CUSTOM_LIBS += $(APP_NET_LIB_PATH)/librtl8723bs_v5174_24878_v35.a
	else ifeq ($(WIFI_NAME),RTL8192ES)
		#CUSTOM_LIBS += $(APP_NET_LIB_PATH)/librtl8192es_v441_v35.a
		CUSTOM_LIBS += $(APP_NET_LIB_PATH)/librtl8192es_v5202_v35.a
	else ifeq ($(WIFI_NAME),RTL8189ES)
		CUSTOM_LIBS += $(APP_NET_LIB_PATH)/librtl818x_v43184_v35.a
	else ifeq ($(WIFI_NAME),RTL8192EU)
		CUSTOM_LIBS += $(APP_NET_LIB_PATH)/rtl8192cu_v402_v35.a
	else
		#default is RTL8189ES
		CUSTOM_LIBS += $(APP_NET_LIB_PATH)/librtl818x_v43181_v35.a
	endif

	CUSTOM_LIBS += $(APP_NET_LIB_PATH)/librtkdrvbase_v35.a
endif
endif

ifeq ($(USBNET),YES)
CUSTOM_LIBS += $(APP_NET_LIB_PATH)/libusbnet_v35.a
endif

CUSTOM_LIBS += $(APP_NET_LIB_PATH)/libnl114_v35.a
CUSTOM_LIBS += $(APP_NET_LIB_PATH)/libmac80211_v35.a
CUSTOM_LIBS += $(APP_NET_LIB_PATH)/liblwip_v35.a
CUSTOM_LIBS += $(APP_NET_LIB_PATH)/libndk_v35.a

ifeq ($(ETHERDRV),YES)
CUSTOM_LIBS += $(APP_NET_LIB_PATH)/libether_v35.a
CUSTOM_LIBS += $(APP_NET_LIB_PATH)/librtkdrvbase_v35.a
CFLAGS += -DICAT_ETHERNET=1
endif

else # NDK1

CUSTOM_LIBS += $(APP_NET_LIB_PATH)/liblivest_v35.a

ifeq ($(USBWIFI),YES)
CUSTOM_LIBS += $(APP_NET_LIB_PATH)/libndk_lwip_usb_v35.a
else ifeq ($(SDIODRV),YES)
CUSTOM_LIBS += $(APP_NET_LIB_PATH)/libndk_lwip_sdio_v35.a
else
CUSTOM_LIBS += $(APP_NET_LIB_PATH)/libndk_lwip_usb_v35.a
endif

endif # NDK2

endif # WIFION >>>>>> WIFI

ifeq ($(wildcard $(APP_OSS_LIB_PATH)/linuxbase.a),)
CUSTOM_LIBS += $(APP_OSS_LIB_PATH)/liboss_sdio.a
CUSTOM_LIBS += $(APP_OSS_LIB_PATH)/liboss_usbhost.a
CUSTOM_LIBS += $(APP_OSS_LIB_PATH)/liboss_linuxusbhost.a
CUSTOM_LIBS += $(APP_OSS_LIB_PATH)/liboss_linuxcrypto.a
ifeq ($(USBNET),YES)
CUSTOM_LIBS += $(APP_OSS_LIB_PATH)/liboss_linuxtty.a
endif
CUSTOM_LIBS += $(APP_OSS_LIB_PATH)/liboss_linuxcompat.a
CUSTOM_LIBS += $(APP_OSS_LIB_PATH)/liboss_linuxbase.a
endif

CUSTOM_LIBS += $(APP_OSS_LIB_PATH)/liboss_pthread.a
ifeq ($(NDK2),YES)
CUSTOM_LIBS += $(APP_NET_LIB_PATH)/libjansson_2.7_v35.a
endif

OBJS := $(INI_OBJS) $(GEN_OBJS)

BUILD_INI_OBJS := $(addprefix $(OBJS_PATH)/, $(INI_OBJS))
BUILD_GEN_OBJS := $(addprefix $(OBJS_PATH)/, $(GEN_OBJS))
BUILD_OBJS := $(addprefix $(OBJS_PATH)/, $(OBJS))

BUILD_LD_OBJS := $(RST_OBJS) $(BUILD_INI_OBJS) $(INT_OBJS) $(BUILD_GEN_OBJS)

BUILD_GOAL := $(DEST_DIR)/$(PLATFORM)

BUILD_LDSCRIPT := $(DEST_DIR)/$(LDSCRIPT)
BUILD_LDSCRIPT_DEF := $(BUILD_LDSCRIPT).cppflags.def

ifneq (,$(or $(nand),$(spi)))
BUILD_MASKROM_SYM := $(BUILD_LDSCRIPT).maskrom.sym
else
BUILD_MASKROM_SYM :=
endif


#[NOTES] subdirs and sublibs =========
#SUBLIBS are archives which get built from opened source trees
#CUSTOM_LIBS and PRJ_LIB are custom provided binaries (archives)
#CUSTOM_SUBLIBS are archives which get built from custom provided source trees
#SUBDIRS are opened source subtrees need to get built
#CUSTOM_SUBDIRS are custom provided source subtrees need to get built

#SUBLIBS += <path>/<archive> is equivalent to
#SUBDIRS += <path> and
#LIBS += $(OBJS_PATH)/<path>/<archive>

#[CUSTOM_]SUBLIBS += <path>/<archive> is equivalent to
#[CUSTOM_]SUBDIRS += <path> and
#[CUSTOM_]LIBS += $(OBJS_PATH)/<path>/<archive>


#ifeq ($(NDK2),YES)
#CUSTOM_SUBLIBS +=  $(HOST_FW_PATH)/service/rcfw/librcfw.a
#endif

CUSTOM_SUBLIBS += $(HOST_FW_PATH)/libhostfw.a
ifeq ($(findstring YES,$(WIFION) $(ETHERDRV)), YES)
ifneq ($(wildcard $(APP_NDK_PATH)/Makefile),)
CUSTOM_SUBLIBS +=  $(APP_NDK_PATH)/libWiFi.a
endif
endif

SUBLIBS += $(DISP_DRV_PATH)/libdisp_custom.a
SUBLIBS += $(SEN_DRV_PATH)/libsensor_custom.a

ifeq ($(GSENSOR), 1)
SUBLIBS += $(GSEN_DRV_PATH)/libgsensor_custom.a
endif

ifneq (,$(filter $(ICAT_RSC) $(ICAT_EIS), 1))
ifneq ($(GYRO_NAME), NONE)
SUBLIBS += $(GYRO_DRV_PATH)/libgyro.a
endif
endif

SUBDIRS += $(CUSTOM_SUBDIRS)

CUSTOM_LIBS += $(PRJ_LIB)

BUILD_SUBLIBS := $(addprefix $(OBJS_PATH)/,$(SUBLIBS))
BUILD_CUSTOM_SUBLIBS := $(addprefix $(OBJS_PATH)/,$(CUSTOM_SUBLIBS))

HOST_LIBS := $(BUILD_CUSTOM_SUBLIBS) $(CUSTOM_LIBS) $(BUILD_SUBLIBS)
HOST_LIBS += $(OBJS_PATH)/$(HOST_FW_PATH)/libhostfw.a ##dirty circular linkage

#---------------------------------------------------------------------------
# Libraries not provided by GCC
#---------------------------------------------------------------------------
# DON'T MODIFY LINES BELOW --------------
LIBS += $(LIB_PATH)/version.a
LIBS += $(LIB_PATH)/libboot.a
# DON'T MODIFY LINES ABOVE --------------
LIBS += $(LIB_PATH)/libcxx.a
LIBS += $(LIB_PATH)/libboot.a

# Clean include from here!!!
ifeq ($(RTMP),YES)
	ifneq ($(wildcard $(APP_RTMP_LIB_PATH)/librtmp.a),)
	LIBS += $(APP_RTMP_LIB_PATH)/libfwcmd.a
	LIBS += $(APP_RTMP_LIB_PATH)/libappcom.a
	LIBS += $(APP_RTMP_LIB_PATH)/libpluginavfilter.a
	LIBS += $(APP_RTMP_LIB_PATH)/libpluginrtsp.a
	LIBS += $(APP_RTMP_LIB_PATH)/libpluginrtmp.a
	#LIBS += $(APP_RTMP_LIB_PATH)/libpluginaac.a
	LIBS += $(APP_RTMP_LIB_PATH)/libpluginv35.a
	LIBS += $(APP_RTMP_LIB_PATH)/libappv35.a
	LIBS += $(APP_RTMP_LIB_PATH)/libpluginbase.a
	LIBS += $(APP_RTMP_LIB_PATH)/libfwcore.a
	LIBS += $(APP_RTMP_LIB_PATH)/libfwbase.a
	LIBS += $(APP_RTMP_LIB_PATH)/librtmp.a
	LIBS += $(APP_RTMP_LIB_PATH)/libfwporting.a
	endif
	CFLAGS += -DAPP_RTMP=1
endif
LIBS += $(wildcard $(LIB_PATH)/apimon.a)
LIBS += $(LIB_PATH)/libcmd.a
LIBS += $(LIB_PATH)/libvcmd.a
LIBS += $(LIB_PATH)/sp5k_global.a
LIBS += $(LIB_PATH)/lib5kmodesw.a
LIBS += $(LIB_PATH)/lib5kftrack.a
LIBS += $(LIB_PATH)/lib5kptp.a
LIBS += $(LIB_PATH)/lib5kusb.a
LIBS += $(LIB_PATH)/lib5kusbhost.a
LIBS += $(LIB_PATH)/lib5kts.a
LIBS += $(LIB_PATH)/lib5klmi.a
LIBS += $(LIB_PATH)/lib5ksensor.a
LIBS += $(LIB_PATH)/lib5kcapture.a
LIBS += $(LIB_PATH)/lib5kcalib.a
LIBS += $(LIB_PATH)/lib5k3a.a
LIBS += $(LIB_PATH)/lib5kpb.a
LIBS += $(LIB_PATH)/lib5kmedia.a
LIBS += $(LIB_PATH)/lib5kmoe.a
LIBS += $(LIB_PATH)/lib5kdisp.a
LIBS += $(LIB_PATH)/lib5kcec.a
LIBS += $(LIB_PATH)/lib5kpip.a
LIBS += $(LIB_PATH)/lib5kmview.a
LIBS += $(LIB_PATH)/lib5krsvblk.a

ifeq ($(TEST_ADAS), YES)
	CFLAGS += -DTEST_ADAS=1
	LIBS += $(LIB_PATH)/lib5kadas.a
	ifeq ($(TEST_ADAS_DEBUG), YES)
		DBG_SUFFIX = .dbg
		LIBS += $(LIB_PATH)/libadas$(DBG_SUFFIX).a
	else
		ifeq ($(TEST_ADAS_DEMO), YES)
			DBG_SUFFIX = .dbg
			LIBS += $(LIB_PATH)/libadas$(DBG_SUFFIX).a
		else
			LIBS += $(LIB_PATH)/libadas.a
		endif
	endif
else
CFLAGS += -DTEST_ADAS=0
endif

ifeq ($(TEST_ADAS_DEMO), YES)
	CFLAGS += -DTEST_ADAS_DEMO=1
else
	CFLAGS += -DTEST_ADAS_DEMO=0
endif

ifeq ($(TEST_ADAS_LDWS), YES)
	LIBS += $(LIB_PATH)/libadas.a
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
	LIBS += $(LIB_PATH)/libadas.a
	CFLAGS += -DTEST_ADAS_LPD=1
else
	CFLAGS += -DTEST_ADAS_LPD=0
endif

ifeq ($(TEST_ADAS_ROD), YES)
	LIBS += $(LIB_PATH)/libadas.a
	CFLAGS += -DTEST_ADAS_ROD=1
else
	CFLAGS += -DTEST_ADAS_ROD=0
endif

ifeq ($(DIQ_FRAMEWORK),YES)
LIBS += $(LIB_PATH)/lib5kdq.a
endif

# EIS/RSC
ifeq ($(ICAT_EIS), 1)
LIBS += $(wildcard $(LIB_PATH)/lib5keisrscalgo.a)
LIBS += $(wildcard $(LIB_PATH)/lib5keis.a)
LIBS += $(wildcard $(LIB_PATH)/lib5krsc.a)
endif

LIBS += $(LIB_PATH)/lib5kgfx.a
LIBS += $(LIB_PATH)/lib5kutil.a
LIBS += $(LIB_PATH)/lib5kdisk.a
LIBS += $(LIB_PATH)/lib5kaud.a
LIBS += $(LIB_PATH)/lib5kdcf.a
LIBS += $(LIB_PATH)/lib5kdpof.a
LIBS += $(LIB_PATH)/lib5kexif.a
LIBS += $(LIB_PATH)/lib5kcdsp.a
LIBS += $(LIB_PATH)/lib5kos.a

# libusb and USB related application is higher caller in middleware layer
LIBS += $(LIB_PATH)/libptp$(DBG_SUFFIX).a
LIBS += $(LIB_PATH)/libusb.a
LIBS += $(LIB_PATH)/libusbhost.a


LIBS += $(LIB_PATH)/libftrack.a

ifneq ($(PLATFORM),5210)
LIBS += $(LIB_PATH)/libldc.a
LIBS += $(wildcard $(LIB_PATH)/libsmear.a)
endif
LIBS += $(LIB_PATH)/modesw.a
LIBS += $(LIB_PATH)/libldc.a  ##dirty circular linkage by modesw_preview.o::modePreviewEnter(), ASSERT(ldcPvModeInitQuery()!=0);
LIBS += $(LIB_PATH)/libbanding.a
LIBS += $(LIB_PATH)/libsnap$(DBG_SUFFIX).a
ifeq ($(PLATFORM),5210)
LIBS += $(LIB_PATH)/libxdn.a
else
LIBS += $(wildcard $(LIB_PATH)/libxfilter.a)
endif

ifeq ($(EIS_DEBUG),YES)
LIBS += $(LIB_PATH)/libav.dbg.a
else
LIBS += $(LIB_PATH)/libav.a
endif
LIBS += $(LIB_PATH)/libpvraw.a
LIBS += $(LIB_PATH)/libdist.a  ##dirty circular linkage by modesw,libav
LIBS += $(LIB_PATH)/libpb.a
LIBS += $(LIB_PATH)/libeffect.a
LIBS += $(LIB_PATH)/libbp.a
LIBS += $(LIB_PATH)/libgfx$(DBG_SUFFIX).a
LIBS += $(LIB_PATH)/libpip.a
LIBS += $(LIB_PATH)/libmview.a
LIBS += $(LIB_PATH)/fnt.a
LIBS += $(LIB_PATH)/str.a
LIBS += $(LIB_PATH)/lib3a.a
LIBS += $(LIB_PATH)/libcdsp.a
ifeq ($(DIQ_FRAMEWORK),YES)
LIBS += $(LIB_PATH)/libdq.a
endif
LIBS += $(LIB_PATH)/libuhd.a
LIBS += $(LIB_PATH)/libsensor.a
LIBS += $(LIB_PATH)/lib3a.a  ##dirty circular linkage
LIBS += $(LIB_PATH)/libime.a
LIBS += $(LIB_PATH)/librsvblk.a
LIBS += $(LIB_PATH)/libresource.a
LIBS += $(LIB_PATH)/libdisp.a
LIBS += $(LIB_PATH)/libh264.a
LIBS += $(LIB_PATH)/libdcf.a
LIBS += $(LIB_PATH)/libdpof.a
LIBS += $(LIB_PATH)/libdisk.a
LIBS += $(LIB_PATH)/fs.a

USE_POSIX_VFS := YES
ifeq ($(USE_POSIX_VFS), YES)
LIBS += $(LIB_PATH)/libvfsposix.a
else
LIBS += $(LIB_PATH)/libvfs.a
endif

ifeq ($(findstring YES,$(WIFION) $(ETHERDRV) $(RTMP)), YES)
LIBS +=  $(APP_NET_LIB_PATH)/libstdc++.a # C++
endif

LIBS += $(LIB_PATH)/libaud.a      # Audio device H/W driver
LIBS += $(LIB_PATH)/global.a
LIBS += $(LIB_PATH)/libdrv$(DBG_SUFFIX).a
LIBS += $(LIB_PATH)/libutil$(DBG_SUFFIX).a
LIBS += $(LIB_PATH)/hal.a
LIBS += $(LIB_PATH)/libmsg.a
#LIBS += $(LIB_PATH)/libmp3.a
LIBS += $(LIB_PATH)/libts.a
LIBS += $(LIB_PATH)/libos$(DBG_SUFFIX).a
LIBS += $(LIB_PATH)/libcmd.a  # FIXME! cmd used in prog dead linkage workaround
LIBS += $(LIB_PATH)/libdisk.a
LIBS += $(LIB_PATH)/libdrv$(DBG_SUFFIX).a  # FIXME! timer used in os, hal linkage workaround
LIBS += $(LIB_PATH)/hal.a  # FIXME! timer using hal linkage workaround
LIBS += $(LIB_PATH)/libutil$(DBG_SUFFIX).a  # FIXME! codeSum() circular linkage workaround
LIBS += $(LIB_PATH)/libexif.a
LIBS += $(LIB_PATH)/libc_ex.a
LIBS += $(LIB_PATH)/debug_mod.a
#LIBS += $(LIB_PATH)/liblua.a
LIBS += $(LIB_PATH)/libutil$(DBG_SUFFIX).a  # FIXME! printf() circular linkage workaround
LIBS += $(LIB_PATH)/hal.a  # FIXME! code sentry using hal linkage workaround
LIBS += $(wildcard $(SYS_RES_PATH)/lib/libchip.a)
LIBS += $(LIB_PATH)/libos$(DBG_SUFFIX).a  # FIXME! bitwidth circular linkage workaround
LIBS += $(wildcard $(LIB_PATH)/librsc.a)
LIBS += $(wildcard $(LIB_PATH)/libeis.a)
LIBS += $(wildcard $(LIB_PATH)/libfiveaxis.a)

LIBS += $(LIB_PATH)/lib5kfbeauty.a

PRE_LIBS := $(CUSTOM_LIBS) $(LIBS)

ifneq ($(RTMP),YES)
LD_LIBS := $(HOST_LIBS) $(LIBS)
else
LD_LIBS := $(HOST_LIBS) $(LIBS) $(LIBFW_LIBS)
endif

CFG_HEADERS += ./include/customization/app_init.h
CFG_HEADERS += ./include/customization/dev_init.h
CFG_HEADERS += ./include/customization/macro_expand.h
CFG_HEADERS += ./include/customization/project_def.h

BUILD_CFG_FILES := $(addprefix $(DEST_DIR)/,$(CFG_DEFS) $(CFG_HEADERS))

# FIC supporting
ifneq (,$(or $(nand),$(spi)))
DOWN_FIC := $(DOWN_PATH)/FIC.BIN
endif

# Project IO header
ifneq (,$(HW_VER))
IO_HEADER := $(HOST_ROOT)/$(DEST_DIR)/include/app_Gpio_$(HW_NAME)_$(HW_VER).h
else
IO_HEADER := $(HOST_ROOT)/$(DEST_DIR)/include/app_Gpio_$(HW_NAME).h
endif

WINE_PRG :=
CHECK_WINE_PRG := $(shell which wine 2>&1 | grep -c 'no wine')

ifeq ($(CHECK_WINE_PRG), 1)
 CHECK_WINE_PRG := NO
 ifeq ($(OS), Windows_NT)
  WINE_PRG :=
  ##force execution on Windows
  CHECK_WINE_PRG := YES
 endif
else
 WINE_PRG := wine
 CHECK_WINE_PRG := YES
endif
#---------------------------------------------------------------------------
# Explicit rules
#---------------------------------------------------------------------------
.PHONY: all
all: $(PLATFORM)

.PHONY: $(PLATFORM) resource FRM
$(PLATFORM): resource $(DOWN_PATH)/$(PRJ_NAME).bin FRM lastprj.def
	@$(ECHO) 'Building $(PLATFORM) : $(PRJ_NAME) $(HW_NAME) $(HW_VER) done'
ifeq ($(ICAT_FW2_SUPPORT), 1)
# paul add cp fw to fw1
	$(CP) $(DOWN_PATH)/$(PRJ_NAME).bin $(DOWN_PATH)/$(PRJ_NAME)FW1.bin
endif
ifeq ($(RES), NO)
ifeq ($(ISP), YES)
	@$(TOOL_PATH_EXE)/FRM-F5
endif
endif

ifeq ($(FPGA), YES)
 FPGA_MSG := $(ECHO) 'FPGA := YES' ;
else
 FPGA_MSG :=
endif
.PHONY: lastprj.def.bak
lastprj.def.bak:
	@$(RM) $@
	@($(ECHO) 'PRJ := $(PRJ)' ; \
	$(ECHO) 'PLATFORM := $(PLATFORM)' ; \
	$(ECHO) 'WIFION := $(WIFION)' ; \
	$(ECHO) 'USBWIFI := $(USBWIFI)' ; \
	$(ECHO) 'SDIODRV := $(SDIODRV)' ; \
	$(ECHO) 'NDK2 := $(NDK2)' ; \
	$(ECHO) 'WIFI_NAME := $(WIFI_NAME)' ; \
	$(ECHO) 'BTON := $(BTON)' ; \
	$(FPGA_MSG) \
	$(ECHO) 'PREFIX := $(PREFIX)') > $@

lastprj.def: lastprj.def.bak
	@if ! $(DIFF) -q -N $@ $< ; then \
		$(CP) $< $@ ; \
	fi

$(IO_HEADER): io
.PHONY: io
io: $(BUILD_CFG_FILES)
	@$(MAKE) -C $(SRC_PATH)/drivers/io -f MakefileIO PRJ=$(PRJ)

.PHONY: $(SUBDIRS)
$(SUBDIRS): %: $(BUILD_CFG_FILES) $(IO_HEADER)
	@$(ECHO) 'Making $@'
	@$(MAKE) -C $@ PRJ=$(PRJ)

.PHONY: $(SUBLIBS) $(CUSTOM_SUBLIBS)
$(SUBLIBS) $(CUSTOM_SUBLIBS): %: $(BUILD_CFG_FILES) $(IO_HEADER)
	@$(ECHO) 'Making $@'
	@$(MAKE) -C $(dir $@) PRJ=$(PRJ)

$(BUILD_SUBLIBS) $(BUILD_CUSTOM_SUBLIBS): $(OBJS_PATH)/%: %
	$(MSG_GOAL_OK) #NOTE! empty rule results in .elf not always rebuilt


ifneq (,$(or $(nand),$(spi)))
$(DEST_DIR)/sumpatch.dat: $(BUILD_GOAL).elf.sym.bak
	@$(RM) $@ $@.bak
ifneq ($(TOOLCHAIN), ICATCHTEK)
	@(bkdoorsz=`$(TOOL_PATH)/sym_addr_find.sh $< _bkdoor_block_limit \
	| $(CUT) -c 6-10` \
	&& $(ECHO) "**** backdoor image size $$((0x$$bkdoorsz)) bytes *******")
endif
	@(ofs=`$(TOOL_PATH)/sym_addr_find.sh $< signature | $(CUT) -c 6-10` \
	&& $(ECHO) "0x$$ofs 60") > $@.bak
	@$(MV) $@.bak $@

$(BUILD_GOAL).elf.sym.bak: $(BUILD_GOAL).elf
	@$(RM) $@
	@$(OBJDUMP) -t $< > $@
else
$(DEST_DIR)/sumpatch.dat:
endif

# removing all the stage builds if stage list or objs / libs order changed
# also removing all the possible stage objs
ifeq ($(MULTI_BOOT),YES)
$(BUILD_GOAL).stagelist: $(BUILD_GOAL).stagelist.bak $(BUILD_GOAL).multiboot.bak
	$(MULTI_BOOT_ERR)
	@$(ECHO) '****building multi boot stage list: $(STAGE_LIST) ***'
else
$(BUILD_GOAL).stagelist: $(BUILD_GOAL).stagelist.bak
endif
	@if ! $(DIFF) -q -N $@ $< ; then \
		$(RM) $(BUILD_GOAL)*.elf $(BUILD_GOAL)*.map $(BUILD_GOAL)*.bin \
			$(DOWN_PATH)/$(PRJ_NAME).bin \
			$(DOWN_PATH)/$(PRJ_NAME).stage*.bin \
			$(BUILD_OBJS:.o=.stage*.d) $(BUILD_OBJS:.o=.stage*.o) \
			$(BUILD_LDSCRIPT:.ld=*.ld) && \
		$(CP) $< $@ ; \
	fi

.PHONY: $(BUILD_GOAL).stagelist.bak
$(BUILD_GOAL).stagelist.bak:
	@$(MKDIR) $(dir $@)
	@$(RM) $@
	@$(ECHO) '$(STAGE_LIST)' > $@
	@$(ECHO) '$(strip $(BUILD_STAGE_OBJS) $(BUILD_LD_OBJS) $(LD_LIBS))' \
		| $(TR) ' ' '\n' >> $@

# FIC using multiload info too
$(DOWN_PATH)/$(PRJ_NAME).bin: \
$(BUILD_GOAL).bin $(DEST_DIR)/multiload.dat $(DOWN_FIC)
#$(BUILD_GOAL).bin # $(DEST_DIR)/sumpatch.dat $(DEST_DIR)/multiload.dat $(DOWN_FIC)
	@$(MKDIR) $(dir $@)
ifneq (,$(or $(nand),$(spi)))
	@$(TOOL_PATH)/multiboot_load_patch.sh $< \
			$(DEST_DIR)/multiload.dat $(DOWN_FIC) > $@.bak
#	@$(TOOL_PATH_EXE)/sumpatch $@.bak < $(DEST_DIR)/sumpatch.dat
	@$(MV) $@.bak $@
else
	@$(CP) $< $@
endif
	@$(WC) -c $@
	$(MSG_GOAL_OK)

$(BUILD_GOAL).bin: $(BUILD_GOAL).elf
	@$(OBJCOPY) --output-target=binary -S -g -x -X \
		$(LDFLAGS_BSS) -R .reginfo $< $@
	$(MSG_GOAL_OK)


ifneq ($(MULTI_BOOT),YES)

$(BUILD_GOAL).elf: $(BUILD_LDSCRIPT:.ld=.fic.ld) \
$(BUILD_LD_OBJS) $(LD_LIBS) $(SUBDIRS)
	$(MSG_SPLIT_LINE)

ifeq ($(OTG), YES)
#	@$(ECHO) $(OBJS_PATH)/$(FM_DRV_PATH)/$(THIRD_LIB_PATH)
	$(CP) $(OBJS_PATH)/$(FM_DRV_PATH)/libfm_custom.a $(THIRD_LIB_PATH)
endif
# To generate cross-reference, use:
# -Wl,-Map,$(@:.elf=.map) -Wl,--warn-common -Wl,--cref
	@$(CC) $(LDFLAGS) -o $@ $(BUILD_LD_OBJS) $(LD_LIBS) -lm -lc -lgcc \
		-T $< -Wl,-Map,$(@:.elf=.map) -Wl,--warn-common
	$(MSG_GOAL_OK)

$(BUILD_LDSCRIPT:.ld=.fic.ld): $(BUILD_LDSCRIPT) $(DOWN_FIC)
	@$(CP) $< $@.bak
	@$(TOOL_PATH)/ldscript_fic_patch.sh -cachelinesize=32 $@.bak $(DOWN_FIC)
	@$(MV) $@.bak $@

$(BUILD_LDSCRIPT): $(LDSCRIPT) $(BUILD_LDSCRIPT_DEF) \
$(DOWN_FIC) $(BUILD_MASKROM_SYM) $(BUILD_GOAL).stagelist
	@$(RM) $@ $@.bak
 ifneq (,$(or $(nand),$(spi)))
	@$(SED) -ne 's,^\(.\+\)\t\(.\+\)$$,_maskrom_\2 = \1 ;,p' \
			$(BUILD_MASKROM_SYM) > $@.bak
 endif #ifneq (,$(or $(nand),$(spi)))
	@$(CPP) -Wall -undef -C -P $(LDSCRIPT_CPPFLAGS) $< >> $@.bak
	@$(MV) $@.bak $@

else #ifneq ($(MULTI_BOOT),YES)

BUILD_STAGE_OUT_OBJS := $(filter-out $(shell $(ECHO) '$(BUILD_STAGE_OBJS)' \
	| $(TR) ' ' '\n' | $(SED) -e 's/\.nostage.*\.o$$/.o/'),$(BUILD_OBJS))

LAST_STAGE := $(lastword $(STAGE_LIST))

# backup archives
BUILD_BAK_LIBS := $(filter-out $(LIB_PATH)/%,$(sort $(LD_LIBS)))

$(BUILD_GOAL).multiboot.bak: $(BUILD_BAK_LIBS)
	@for lib in $^ ; do \
		if [ ! -e "$@" ] || [ "$@" -ot "$${lib}" ] ; then \
			$(TOOL_PATH)/multiboot_archive_bak.pl \
					$${lib} $(PREFIX) ; \
			if [ $$? != 0 ] ; then \
				exit 1 ; \
			fi \
		fi \
	done
	@$(RM) $@
	@$(TOUCH) $@


$(BUILD_GOAL).elf: $(BUILD_GOAL).stage$(LAST_STAGE).elf
	@$(RM) $@ $(@:.elf=.map)
	@$(CP) $(<:.elf=.map) $(@:.elf=.map)
	@$(CP) $< $@

$(BUILD_LDSCRIPT:.ld=.fic.ld): \
$(BUILD_LDSCRIPT:.ld=.stage$(LAST_STAGE).ld) $(DOWN_FIC)
	@$(CP) $< $@.bak
	@$(TOOL_PATH)/ldscript_fic_patch.sh -cachelinesize=32 $@.bak $(DOWN_FIC)
	@$(MV) $@.bak $@
	$(MSG_GOAL_OK)

#---------------------------------------------------------------------------
# Implicit rules
#---------------------------------------------------------------------------
define STAGE_OBJ_RULES

 BUILD_STAGE$(1)_ALL_OBJS := $$(call stage-obj,$(1),\
		$$(BUILD_LD_OBJS),$$(BUILD_STAGE_OBJS))

 BUILD_STAGE$(1)_OBJS := $$(call stage-obj,$(1),\
		$$(BUILD_STAGE_OBJS),$$(BUILD_STAGE_OBJS))

 $$(OBJS_PATH)/%.stage$(1).d: CFLAGS += -DMULTI_BOOT_STAGE=$(1)
 $$(OBJS_PATH)/%.stage$(1).o: CFLAGS += -DMULTI_BOOT_STAGE=$(1)

 $$(OBJS_PATH)/%.stage$(1).d: %.c $$(BUILD_CFG_FILES) $$(IO_HEADER) \
 $$(BUILD_GOAL).stagelist
	@$$(MKDIR) $$(dir $$@)
	@$$(CC) $$(CFLAGS) -MM -MT '$$(@:.d=.o) $$@' -o $$@ $$<

 $$(OBJS_PATH)/%.stage$(1).d: %.S $$(BUILD_CFG_FILES) $$(IO_HEADER) \
 $$(BUILD_GOAL).stagelist
	@$$(MKDIR) $$(dir $$@)
	@$$(CC) $$(ASFLAGS) $$(CFLAGS) -MM -MT '$$(@:.d=.o) $$@' -o $$@ $$<

 $$(OBJS_PATH)/%.stage$(1).o: %.c $$(OBJS_PATH)/%.stage$(1).d $$(IO_HEADER)
	$$(MSG_SPLIT_LINE)
	$$(MSG_COMPILING)
	@$$(CC) -c $$(CFLAGS) -o $$@ $$<

 $$(OBJS_PATH)/%.stage$(1).o: %.S $$(OBJS_PATH)/%.stage$(1).d $$(IO_HEADER)
	$$(MSG_SPLIT_LINE)
	$$(MSG_COMPILING)
	@$$(CC) -c $$(ASFLAGS) $$(CFLAGS) -o $$@ $$<

endef

$(foreach stage,$(STAGE_LIST),$(eval $(call STAGE_OBJ_RULES,$(stage))))


ifeq (,$(and $(filter YES,$(MULTI_BOOT_TOOL_REMAP)),\
	$(filter YES,$(HAS_MULTI_BOOT_TOOL_REMAP))))

BASE_STAGE := $(firstword $(STAGE_LIST))

define STAGE_GOAL_RULES

 STAGE$(1)_PRE_LIBS := $$(call stage-archive,$(1),$$(PRE_LIBS))

 STAGE$(1)_LIBS := $$(call stage-archive,$(1),$$(LD_LIBS))

 ifeq ($(1),$$(LAST_STAGE))
  ifeq ($(OBJMAP),NO)
   $$(BUILD_GOAL).stage$(1).elf: $$(BUILD_LDSCRIPT:.ld=.fic.ld) \
   $$(BUILD_STAGE$(1)_ALL_OBJS) $$(STAGE$(1)_PRE_LIBS) \
   $$(BUILD_SUBLIBS) $$(BUILD_CUSTOM_SUBLIBS) $$(SUBDIRS)
  else  #ifeq ($(OBJMAP),NO)
   $$(BUILD_GOAL).stage$(1).elf: $$(BUILD_LDSCRIPT:.ld=.fic.ld) \
   $$(BUILD_STAGE$(1)_ALL_OBJS)
  endif #ifeq ($(OBJMAP),NO)
 else
  $$(BUILD_GOAL).stage$(1).elf: $$(BUILD_LDSCRIPT:.ld=.stage$(1).ld) \
  $$(BUILD_STAGE$(1)_ALL_OBJS)
 endif
	$$(MSG_SPLIT_LINE)
# To generate cross-reference, use:
# -Wl,-Map,$$(@:.elf=.map) -Wl,--warn-common -Wl,--cref
	@$$(CC) -o $$@ $$(LDFLAGS) -T $$< \
		-Wl,-Map,$$(@:.elf=.map) -Wl,--warn-common
	@$$(ECHO) 'STAGE $(1) ===== multi boot stage has binary size ===='
	@$$(SIZE) $$@
	@$$(ECHO) 'STAGE $(1) ==========================================='
	$$(MSG_GOAL_OK)

endef

define STAGE_LDSCRIPT_RULES

  PRE_STAGE$(1) := $$(call pre-stage,$(1),$$(STAGE_LIST))
  BUILD_PRE_GOAL$(1) := $$(BUILD_GOAL).stage$$(PRE_STAGE$(1))

  $$(BUILD_LDSCRIPT:.ld=.stage$(1).ld): \
  $$(BUILD_LDSCRIPT:.ld=.stage$$(PRE_STAGE$(1)).ld) \
  $$(BUILD_PRE_GOAL$(1)).map.input $$(BUILD_GOAL).stagelist
	@$$(RM) $$@ $$@.cmd*
	@$$(ECHO) '$$(strip $$(BUILD_STAGE$(1)_ALL_OBJS))' \
		| $$(TR) ' ' '\n' > $$@.cmdobj
	@$$(ECHO) '$$(strip $$(STAGE$(1)_LIBS) -lm -lc -lgcc)' \
		| $$(TR) ' ' '\n' > $$@.cmdlib
	@$$(TOOL_PATH_EXE)/multiboot_ldscript -cachelinesize=32 \
		$$(PRE_STAGE$(1)) $(1) $$@ \
		$$< $$(BUILD_PRE_GOAL$(1)).map.input $$(PREFIX)
	$$(MSG_GOAL_OK)

endef

ifeq ($(OBJMAP),NO)

$(eval $(call STAGE_GOAL_RULES,$(LAST_STAGE)))

else #ifeq ($(OBJMAP),NO)

$(foreach stage, $(STAGE_LIST), $(eval $(call STAGE_GOAL_RULES,$(stage))))

$(foreach stage, $(wordlist 2, $(words $(STAGE_LIST)), $(STAGE_LIST)), \
		$(eval $(call STAGE_LDSCRIPT_RULES,$(stage))))


$(BUILD_LDSCRIPT:.ld=.stage$(BASE_STAGE).ld): \
$(LDSCRIPT) $(BUILD_LDSCRIPT_DEF) $(LD_LIBS) $(SUBDIRS) $(BUILD_MASKROM_SYM) \
$(BUILD_GOAL).stagelist
	@$(RM) $@ $@.bak $@.*.bak $@.cmd*
	@$(ECHO) '$(strip $(BUILD_STAGE$(BASE_STAGE)_ALL_OBJS))' \
		| $(SED) -e 's/ /\n/g' > $@.cmdobj
	@$(ECHO) '$(strip $(STAGE$(BASE_STAGE)_LIBS) -lm -lc -lgcc)' \
		| $(SED) -e 's/ /\n/g' > $@.cmdlib
	@$(SED) -ne 's,^\(.\+\)\t\(.\+\)$$,_maskrom_\2 = \1 ;,p' \
			$(BUILD_MASKROM_SYM) > $@.bak
	@$(CPP) -Wall -undef -C -P $(LDSCRIPT_CPPFLAGS) $< >> $@.bak
	@$(TOOL_PATH_EXE)/multiboot_ldscript -cachelinesize=32 \
			0 $(BASE_STAGE) $@ $@.bak $(words $(STAGE_LIST))
	$(MSG_GOAL_OK)

$(BUILD_GOAL).stage%.map.input: $(BUILD_GOAL).stage%.elf
# keeping backups of stage binaries
	@$(MKDIR) $(DOWN_PATH)/
	@$(RM) $(DOWN_PATH)/$(PRJ_NAME).stage$*.bin
	@$(OBJCOPY) --output-target=binary -S -g -x -X $(LDFLAGS_BSS) \
		-R .reginfo $< $(DOWN_PATH)/$(PRJ_NAME).stage$*.bin
	@$(WC) -c $(DOWN_PATH)/$(PRJ_NAME).stage$*.bin
	@$(ECHO) '**** back up $(DOWN_PATH)/$(PRJ_NAME).stage$*.bin **********'
	@$(RM) $@
	@$(TOOL_PATH_EXE)/multiboot_map $(<:.elf=.map) $@

endif #ifeq ($(OBJMAP),NO)

else  #$(MULTI_BOOT_TOOL_REMAP) == YES && $(HAS_MULTI_BOOT_TOOL_REMAP) == YES


PRE_STAGES_LIST := $(filter-out $(LAST_STAGE),$(STAGE_LIST))

BUILD_PRE_STAGES_OBJS := $(foreach stage,\
		$(PRE_STAGES_LIST),$(BUILD_STAGE$(stage)_OBJS))

ifeq ($(OBJMAP),NO)
$(BUILD_GOAL).stage$(LAST_STAGE).elf: $(BUILD_LDSCRIPT:.ld=.fic.ld) \
$(BUILD_STAGE$(LAST_STAGE)_ALL_OBJS) $(LD_LIBS) $(SUBDIRS)
else  #ifeq ($(OBJMAP),NO)
$(BUILD_GOAL).stage$(LAST_STAGE).elf: $(BUILD_LDSCRIPT:.ld=.fic.ld)
endif #ifeq ($(OBJMAP),NO)
	$(MSG_SPLIT_LINE)
# To generate cross-reference, use:
# -Wl,-Map,$(@:.elf=.map) -Wl,--warn-common -Wl,--cref
	@$(CC) $(LDFLAGS) -o $@ -T $< \
		-Wl,-Map,$(@:.elf=.map) -Wl,--warn-common
	$(MSG_GOAL_OK)


ifeq ($(OBJMAP),NO)
else  #ifeq ($(OBJMAP),NO)

$(BUILD_LDSCRIPT:.ld=.stage$(LAST_STAGE).ld): $(BUILD_LDSCRIPT:.ld=.stage0.ld) \
$(BUILD_GOAL).stage0.elf $(BUILD_PRE_STAGES_OBJS) $(BUILD_GOAL).stagelist
# checking if all the previous staged objs valid
	@for stage in $(PRE_STAGES_LIST) ; do \
		$(TOOL_PATH)/multiboot_lscript_input.pl -q -k $$stage \
		'$(BUILD_STAGE_OBJS)' '$(BUILD_STAGE_OBJS)'; \
		if [ $$? != 0 ] ; then \
			exit 1 ; \
		fi \
	done
	@$(TOOL_PATH_EXE)/multiboot_lscript_remap.pl -cachelinesize=32 \
		-vinfo,bcz -k'$(BUILD_NOSTAGE_OBJS)' \
		$@ $< $(BUILD_GOAL).stage0.map '$(STAGE_LIST)' $(PREFIX)
	$(MSG_GOAL_OK)

$(BUILD_GOAL).stage0.elf: $(BUILD_LDSCRIPT:.ld=.stage0.ld) \
$(BUILD_STAGE$(LAST_STAGE)_ALL_OBJS) $(LD_LIBS) $(SUBDIRS)
	$(MSG_SPLIT_LINE)
	@$(TOOL_PATH)/multiboot_lscript_input.pl -k $(LAST_STAGE) \
		'$(BUILD_LD_OBJS) $(LD_LIBS)' '$(BUILD_STAGE_OBJS)' > $@.depend
# To generate cross-reference, use:
# -Wl,-Map,$(@:.elf=.map) -Wl,--warn-common -Wl,--cref
	@$(CC) $(LDFLAGS) -o $@ `$(CAT) $@.depend` -lm -lc -lgcc -T $< \
		-Wl,-Map,$(@:.elf=.map) -Wl,--warn-common
	$(MSG_GOAL_OK)

$(BUILD_LDSCRIPT:.ld=.stage0.ld): $(LDSCRIPT) \
$(BUILD_LDSCRIPT_DEF) $(BUILD_MASKROM_SYM) $(BUILD_GOAL).stagelist
	@$(RM) $@ $@.mload.bak $@.bak
	@$(SED) -ne 's,^\(.\+\)\t\(.\+\)$$,_maskrom_\2 = \1 ;,p' \
			$(BUILD_MASKROM_SYM) > $@.bak
	@$(GREP) -L -m 1 '\b_bkdoor_block_limit\b' $< 1>&2
	@$(SED) -e 's/^\(.*\b_bkdoor_block_limit\b.*\)$$/\t\t_multiboot_load_info = . ;\n\t\t. += 16*(2+$(words $(STAGE_LIST))) ;\n\1/' $< > $@.mload.bak
	@$(CPP) -Wall -undef -C -P $(LDSCRIPT_CPPFLAGS) $@.mload.bak >> $@.bak
	@$(MV) $@.bak $@
	$(MSG_GOAL_OK)

endif #ifeq ($(OBJMAP),NO)

endif #$(MULTI_BOOT_TOOL_REMAP) == YES && $(HAS_MULTI_BOOT_TOOL_REMAP) == YES

endif #ifneq ($(MULTI_BOOT),YES)

$(BUILD_LDSCRIPT_DEF): $(BUILD_LDSCRIPT_DEF).bak
	@if ! $(DIFF) -q -N $@ $< ; then \
		$(CP) $< $@ ; \
	fi

.PHONY: $(BUILD_LDSCRIPT_DEF).bak
$(BUILD_LDSCRIPT_DEF).bak:
	@$(MKDIR) $(dir $@)
	@$(RM) $@
	@$(ECHO) '$(strip $(LDSCRIPT_CPPFLAGS))' | $(TR) ' ' '\n' > $@

ifneq (,$(or $(nand),$(spi)))

BUILD_MASKROM_ALL_SYM := $(BUILD_MASKROM_SYM:.sym=.all.sym)
BUILD_MASKROM_UNDEF_SYM := $(LIB_PATH)/_maskrom.undef.sym

$(BUILD_MASKROM_SYM): $(BUILD_MASKROM_ALL_SYM) $(BUILD_MASKROM_UNDEF_SYM)
	@$(RM) $@ $@.bak
	@{ for key in `$(CAT) $(BUILD_MASKROM_UNDEF_SYM)` ; do \
		addr=`$(TOOL_PATH)/sym_addr_find.sh $< $${key}` ; \
		if [ $$? != 0 ] || [ -z "$${addr}" ] ; then \
			addr=0x00000000 ; \
		fi ; \
		$(ECHO) "$${addr}	$${key}" ; \
	done } > $@.bak
	@$(MV) $@.bak $@

$(BUILD_MASKROM_ALL_SYM): $(BUILD_MASKROM_ALL_SYM).bak
	@if ! $(DIFF) -q -N $@ $< ; then \
		$(CP) $< $@ ; \
	fi

.PHONY: $(BUILD_MASKROM_ALL_SYM).bak
$(BUILD_MASKROM_ALL_SYM).bak: $(MASKROM_ELF)
	@$(MKDIR) $(dir $@)
	@$(RM) $@
	@$(OBJDUMP) -t $< > $@

endif #ifneq (,$(or $(nand),$(spi)))

# FIC for non-multiboot needs multiload info too
$(DEST_DIR)/multiload.dat: $(BUILD_GOAL).elf
	@$(TOOL_PATH)/multiboot_load_info.sh -v -b \
		$< "$(STAGE_LIST)" $(PREFIX) $(MULTIBOOT_FLASH_SIZE_ALIGN512) > $@
	$(MSG_GOAL_OK)


ifneq ($(RES),NO)
ifneq (,$(or $(nand),$(spi)))
$(DOWN_FIC): resource
	$(MSG_GOAL_OK) #NOTE! empty rule results in .elf not always rebuilt
endif #ifneq (, $(or $(nand), $(spi)))

resource:

	#@$(MAKE) -C $(HOST_FW_PATH) -f MakefileMenu
  ifeq ($(DIQ_FRAMEWORK), YES)
	@$(ECHO) === diq YES, wine $(CHECK_WINE_PRG) ===
   ifeq ($(CHECK_WINE_PRG), YES)
	@$(MAKE) -f MakefileResDIQ diq_cal CHECK_WINE_PRG=$(CHECK_WINE_PRG) WINE_PRG=$(WINE_PRG)
   endif
  endif
	@$(MAKE) -f MakefileRes $(MAKECMDGOALS) PRJ=$(PRJ) CHECK_WINE_PRG=$(CHECK_WINE_PRG) WINE_PRG=$(WINE_PRG)

endif #ifneq ($(RES),NO)


# ====== FRM ======
FRM_RSV_SRCS :=# $(TOOL_PATH)/FRM/ResImageMaker/config_template.xml
FRM_RSV_DOWN :=# $(DOWN_PATH)/ResImageMaker/config_template.xml

FRM_BIN_DIRS := \
	$(TOOL_PATH)/FRM/Profiles \
	$(TOOL_PATH)/FRM/ResImageMaker

FRM_BIN_SRCS := \
	$(filter-out $(FRM_RSV_SRCS),\
	$(wildcard $(FRM_BIN_DIRS:=/*)) \
	$(filter-out $(FRM_BIN_DIRS),$(wildcard $(TOOL_PATH)/FRM/*)))

FRM_BIN_DOWN := $(patsubst $(TOOL_PATH)/FRM/%,$(DOWN_PATH)/%,$(FRM_BIN_SRCS))
$(FRM_BIN_DOWN): $(DOWN_PATH)/%: $(TOOL_PATH)/FRM/%

FRM_ISP_DOWN := $(DOWN_PATH)/$(ISP_BIN)
$(FRM_ISP_DOWN): $(ISP_BIN_RES)

FRM_SET_SRCS := \
	$(TOOL_PATH)/SetPara.exe \
	$(TOOL_PATH)/SetReg.exe \
	$(TOOL_PATH)/SetReg.ini \

# keep the above empty line

FRM_SET_DOWN := $(patsubst $(TOOL_PATH)/%,$(DOWN_PATH)/%,$(FRM_SET_SRCS))
$(FRM_SET_DOWN): $(DOWN_PATH)/%: $(TOOL_PATH)/%

FRM_DOWN := $(FRM_BIN_DOWN) $(FRM_ISP_DOWN) $(FRM_SET_DOWN)

FRM: $(FRM_DOWN) $(DOWN_PATH)/$(FRM_INI) $(DOWN_PATH)/$(FRM_DRAM_PRM) \
$(FRM_RSV_DOWN)

$(FRM_DOWN): %:
	@$(MKDIR) $(dir $@)
	@$(CP) $< $@

ifeq ($(PLATFORM), spca6350)
FRM_DRAM_PRM_OP := 2
else ifeq ($(PLATFORM), spca6330)
FRM_DRAM_PRM_OP := 1
else
FRM_DRAM_PRM_OP := 0
endif

$(DOWN_PATH)/$(FRM_DRAM_PRM): $(FRM_DRAM_PRM_SRC:.prm=.txt)
	@$(MKDIR) $(dir $@)
	@$(RM) $@
	@$(TOOL_PATH_EXE)/drampara $< $@ $(FRM_DRAM_PRM_OP)

#$(FRM_RSV_DOWN): $(FRM_RSV_SRCS)
#	@$(MKDIR) $(dir $@)
#	@$(RM) $@
#ifdef RSV_A_SIZE_KB
#	@$(SED) -e "/FirstDisk/s/maxSizePlaceHolder=\"\"/maxSize=\"$(RSV_A_SIZE_KB)\"/" \
#		-e "/SecondDisk/s/maxSizePlaceHolder=\"\"/maxSize=\"$(RSV_B_SIZE_KB)\"/" \
#		$< > $@
#else
#	@./newrsv.sh $(RSV_A_SIZE) $(RSV_B_SIZE) $@
#endif

# default FRM partial paths list
FRM_PARTIAL_DIRS += A B
FRM_PARTIAL_EXCLUDE += \
	BADPIX \
	COLD.BIN \
	DEFPIX1 \
	DEFTPXL
ifeq ($(PRJ_NAME),$(filter $(PRJ_NAME),17DVR 19DVR))
ifneq ($(ICAT_FW2_SUPPORT), 0)
$(DOWN_PATH)/$(FRM_INI): $(RES_PATH)/$(PRJ_NAME)/ISP/dualboot/$(FRM_INI) resource
else
$(DOWN_PATH)/$(FRM_INI): $(RES_PATH)/$(PRJ_NAME)/ISP/boot/$(FRM_INI) resource
endif
else
$(DOWN_PATH)/$(FRM_INI): $(RES_PATH)/$(PRJ_NAME)/ISP/$(FRM_INI) resource
endif

	@$(MKDIR) $(dir $@)
	@$(RM) $@
	@$(SED) -e "/ADefSize/s/=.*/=$(RSV_A_SIZE)/" -e "/BDefSize/s/=.*/=$(RSV_B_SIZE)/" \
		-e "/FIRMWARE.BIN/s/FIRMWARE/$(PRJ_NAME)/" \
		-e "/SpcaPlatformName/s/SpcaPlatformName/$(PLATFORM_CAP)/" \
		-e "/ISPName/s/ISP.BIN/$(ISP_BIN)/" \
		-e "/SettingFilename/s/drampara.prm/$(FRM_DRAM_PRM)/" \
		-e "/Custom_CurGpio/s/XXX/$(FRM_LED_GRP)/" \
		-e "/Custom_CurMethod/s/XXX/$(FRM_LED_PIN)/" \
			$< > $@.bak
	@$(SED) -i '/\[PARTIAL_FILELIST\]/,$$ d' $@.bak
	@( $(ECHO) '[PARTIAL_FILELIST]' && \
	cd $(DOWN_PATH) && \
	$(FIND) $(FRM_PARTIAL_DIRS) -type d -or \
	$(foreach f,$(FRM_PARTIAL_EXCLUDE),-not -name '*$(f)*') -print | \
	$(TR) '/' '\\' ) >> $@.bak
ifeq ($(PRJ_NAME),$(filter $(PRJ_NAME),17DVR 19DVR))
	$(CP) $(RES_PATH)/$(PRJ_NAME)/RAWC.BIN $(DOWN_PATH)/
endif
	@$(MV) $@.bak $@


#---------------------------------------------------------------------------
# Implicit rules
#---------------------------------------------------------------------------
$(OBJS_PATH)/%.d: %.c $(BUILD_CFG_FILES) $(IO_HEADER)
	@$(MKDIR) $(dir $@)
	@$(CC) $(CFLAGS) -MM -MT '$(@:.d=.o) $@' -o $@ $<

$(OBJS_PATH)/%.d: %.S $(BUILD_CFG_FILES) $(IO_HEADER)
	@$(MKDIR) $(dir $@)
	@$(CC) $(ASFLAGS) $(CFLAGS) -MM -MT '$(@:.d=.o) $@' -o $@ $<

$(OBJS_PATH)/%.o: %.c $(OBJS_PATH)/%.d $(IO_HEADER)
	$(MSG_SPLIT_LINE)
	$(MSG_COMPILING)
	@$(CC) -c $(CFLAGS) -o $@ $<

$(OBJS_PATH)/%.o: %.S $(OBJS_PATH)/%.d $(IO_HEADER)
	$(MSG_SPLIT_LINE)
	$(MSG_COMPILING)
	@$(CC) -c $(ASFLAGS) $(CFLAGS) -o $@ $<


# project dependent config and header files
.PHONY: config
config: $(BUILD_CFG_FILES)
# TODO: manual config scripts

.PHONY: $(BUILD_CFG_FILES:=.patch)
$(BUILD_CFG_FILES:=.patch): $(DEST_DIR)/%.patch:
	@$(RM) $@
# diff exit code: 0=the same; 1=different; 2,others=trouble
	@if [ -f "$(@:.patch=.base)" ]; then \
	  $(DIFF) -duN $(@:.patch=.base) $(HOST_ROOT)/$*.tmpl > $@ ; \
	  _err=$$? ; \
	  if [ $$_err != 0 ] && [ $$_err != 1 ] ; then \
	    $(ECHO) 'diff:$@:error:troubles' ; \
	    $(RM) $@ ; \
	    $(TOUCH) $@ ; \
	    if [ $$? != 0 ] ; then \
	      exit 1 ; \
	    fi ; \
	  fi ; \
	else \
	  $(ECHO) "$@: './config.sh', 'make PRJ=xxx' not run, use default"; \
	  $(MKDIR) $(dir $@) && \
	  $(CP) -u $(HOST_ROOT)/$*.tmpl $(@:.patch=.base) && \
	  $(TOUCH) $@ ; \
	  if [ $$? != 0 ] ; then \
	    exit 1 ; \
	  fi ; \
	fi

$(BUILD_CFG_FILES): $(DEST_DIR)/%: $(DEST_DIR)/%.patch
	@if [ -f "$@" ]; then \
	  if [ -s "$<" ] ; then \
	    $(ECHO) '$@: new version '$*.tmpl' differs, merging..' ; \
	    if $(PATCH) --verbose -u -i $< -r $@.rej -o $@.merge $@ ; then \
	      $(CP) $(HOST_ROOT)/$*.tmpl $@.base && \
	      $(MV) $@.merge $@ ; \
	    else \
	      $(ECHO) '$@:error:$(HOST_ROOT)/$*.tmpl conflicts, use theirs' ; \
	      $(ECHO) '$@:error:backup mine in $@.{bak,base.bak,merge,rej}' ; \
	      $(MV) $@ $@.bak && \
	      $(MV) $@.base $@.base.bak && \
	      $(CP) $(HOST_ROOT)/$*.tmpl $@.base && \
	      $(CP) $(HOST_ROOT)/$*.tmpl $@ ; \
	    fi \
	  fi \
	else \
	  $(ECHO) "$@: './config.sh', 'make PRJ=xxx' not run, use default" ; \
	  $(MKDIR) $(dir $@) && \
	  $(CP) $(HOST_ROOT)/$*.tmpl $@.base && \
	  $(CP) $(HOST_ROOT)/$*.tmpl $@ ; \
	fi ; \
	if [ $$? != 0 ] ; then \
	  exit 1 ; \
	fi

# cancelling implicit rules to build source files
$(LIB_PATH)/%: ;
$(SYS_RES_PATH)/%: ;
$(RES_PATH)/%: ;
$(TOOL_PATH)/%: ;
$(TOOL_PATH_EXE)/%: ;

.PHONY: dump
dump: $(BUILD_GOAL).dump

$(BUILD_GOAL).dump: $(BUILD_GOAL).elf
	$(OBJDUMP) -x -d -a -h -z -s $< > $@

.PHONY: clean
clean: clean_resource
	$(MSG_SPLIT_LINE)
	@$(MAKE) -C $(SRC_PATH)/drivers/io -f MakefileIO $@ PRJ=$(PRJ)
	@for subdir in $(dir $(SUBLIBS) $(CUSTOM_SUBLIBS)) $(SUBDIRS); do \
		$(ECHO) "Making $@ in $$subdir"; \
		$(MAKE) -C $$subdir $@ PRJ=$(PRJ); \
	done;
	@$(RM) $(BUILD_OBJS:.o=.d) $(BUILD_OBJS) \
		$(BUILD_OBJS:.o=.stage*.d) $(BUILD_OBJS:.o=.stage*.o)
	@$(RM) $(BUILD_GOAL).* \
		$(BUILD_GOAL).stagelist $(BUILD_GOAL).stagelist.bak \
		$(BUILD_GOAL).multiboot.bak
	@$(RM) $(DOWN_PATH)/$(PRJ_NAME).bin* \
ifeq ($(ICAT_FW2_SUPPORT), 1)
#paul add remove FW1
		$(RM) $(DOWN_PATH)/$(PRJ_NAME)FW1.bin* \
endif
		$(DOWN_PATH)/$(PRJ_NAME).stage*.bin*
ifneq (,$(strip $(BUILD_BAK_LIBS)))
	@$(RMR) $(foreach lib,$(BUILD_BAK_LIBS),\
		$(dir $(lib))_multiboot.bak/$(notdir $(lib:.a=)))
endif
	@$(RM) $(DEST_DIR)/sumpatch.dat \
		$(DEST_DIR)/multiload.dat
	@$(RMR) $(dir $(BUILD_LDSCRIPT))
	@$(RMR) $(OBJS_PATH)
	@$(RM) $(DOWN_PATH)/$(ISP_BIN) \
		$(DOWN_PATH)/$(FRM_INI) \
		$(DOWN_PATH)/*.prm

.PHONY: clean_resource
clean_resource:
	@$(MAKE) -f MakefileResDIQ clean
	@$(MAKE) -f MakefileRes $(MAKECMDGOALS) PRJ=$(PRJ)


ifeq (,$(filter $(BUILD_SUBLIBS) $(BUILD_CUSTOM_SUBLIBS) $(SUBLIBS) \
 $(CUSTOM_SUBLIBS) $(SUBDIRS) lastprj.def \
 clean clean_resource resource RES_% config io $(IO_HEADER),$(MAKECMDGOALS)))

ifneq ($(MULTI_BOOT),YES)

-include $(BUILD_OBJS:.o=.d)

else  #ifneq ($(MULTI_BOOT),YES)

-include $(BUILD_STAGE_OUT_OBJS:.o=.d)

define STAGE_DEP_RULES
-include $$(BUILD_STAGE$(1)_OBJS:.o=.d)
endef

ifeq ($(OBJMAP),NO)
$(eval $(call STAGE_DEP_RULES,$(LAST_STAGE)))
else
$(foreach stage, $(STAGE_LIST), $(eval $(call STAGE_DEP_RULES,$(stage))))
endif

endif #ifneq ($(MULTI_BOOT),YES)
endif


show:
	@echo LIBS=$(LD_LIBS)
