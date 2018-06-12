#/**************************************************************************
# *                                                                        *
# *       Copyright (c) 2005-2006 by Sunplus Technology Co., Ltd.          *
# *                                                                        *
# *  This software is copyrighted by and is the property of Sunplus        *
# *  Technology Co., Ltd. All rights are reserved by Sunplus Technology    *
# *  Co., Ltd. This software may only be used in accordance with the       *
# *  corresponding license agreement. Any unauthorized use, duplication,   *
# *  distribution, or disclosure of this software is expressly forbidden.  *
# *                                                                        *
# *  This Copyright notice MUST not be removed or modified without prior   *
# *  written consent of Sunplus Technology Co., Ltd.                       *
# *                                                                        *
# *  Sunplus Technology Co., Ltd. reserves the right to modify this        *
# *  software without notice.                                              *
# *                                                                        *
# *  Sunplus Technology Co., Ltd.                                          *
# *  19, Innovation First Road, Science-Based Industrial Park,             *
# *  Hsin-Chu, Taiwan, R.O.C.                                              *
# *                                                                        *
# *  Author: Anmin Deng                                                    *
# *                                                                        *
# **************************************************************************/
#init empty simple flavor variables before any include
CFLAGS :=
ASFLAGS :=


HOST_ROOT := ../../..
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

#---------------------------------------------------------------------------
# Variable definition:
#   1. Cross-compling tools
#   2. Linker script file
#   3. C complier options
#   4. Assembly compiler options
#   5. Linker options
#---------------------------------------------------------------------------
ifeq (cc,$(CC))
CC   := $(PREFIX)/gcc
AR   := $(PREFIX)/ar
NM   := $(PREFIX)/nm
ECHO := echo
endif

GOAL := libgsensor_custom.a

CFLAGS += -Wall
CFLAGS += -Os
CFLAGS += -G 8
CFLAGS += -ansi
CFLAGS += -msoft-float
CFLAGS += -I$(HOST_ROOT)/include
CFLAGS += -I$(PLATFORM_ROOT)/inc
CFLAGS += -EL
CFLAGS += -fno-common

CFLAGS += -I$(PLATFORM_ROOT)/inc/api
CFLAGS += -I./include
CFLAGS += -I$(DEST_DIR)/include
CFLAGS += -I./device
CFLAGS += -I./device/lens
CFLAGS += -I../../include
CFLAGS += -I../../include/thirdparty/iq
CFLAGS += -I$(APP_NET_INC_PATH)
CFLAGS += -I$(APP_OSS_INC_PATH)

#---------------------------------------------------------------------------
# Intermediate objects
#---------------------------------------------------------------------------
OBJS :=

ifeq ($(GSENSOR_MXC6225_XU), 1)
OBJS += app_gSensor_mxc6225xu.o
endif
ifeq ($(GSENSOR_LIS35DE), 1)
OBJS += app_gSensor_lis35de.o
endif
ifeq ($(GSENSOR_EWTS9P), 1)
OBJS += app_gSensor_ewts9p.o
endif
ifeq ($(GSENSOR_BMA220), 1)
OBJS += app_gSensor_bma220.o
endif
ifeq ($(GSENSOR_LIS35DH), 1)
OBJS += app_gSensor_lis35dh.o
endif
ifeq ($(GSENSOR_BMA250E), 1)
OBJS += app_gSensor_bma250e.o
endif
ifeq ($(GSENSOR_NONE), 1)
OBJS += app_gSensor_default.o
endif

ifneq ($(GSENSOR_NONE), 1)
OBJS += app_gSensor.o
endif

#---------------------------------------------------------------------------
# Referring to generic rules
#---------------------------------------------------------------------------
$(eval $(call GEN_GOAL_RULES))
