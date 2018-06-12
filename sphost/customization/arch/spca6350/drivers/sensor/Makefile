#/**************************************************************************
# *
# *         Copyright (c) 2002-2012 by iCatch Technology Inc.
# *
# *  This software is copyrighted by and is the property of iCatch
# *  Technology Inc. All rights are reserved by iCatch Technology Inc.
# *  This software may only be used in accordance with the corresponding
# *  license agreement. Any unauthorized use, duplication, distribution,
# *  or disclosure of this software is expressly forbidden.
# *
# *  This Copyright notice MUST not be removed or modified without prior
# *  written consent of iCatch Technology Inc.
# *
# *  iCatch Technology Inc. reserves the right to modify this software
# *  without notice.
# *
# *  iCatch Technology Inc.
# *  19-1, Innovation First Road, Science-Based Industrial Park,
# *  Hsin-Chu, Taiwan.
# *
# *  Author: Anmin Deng
# *
# **************************************************************************/
#init empty simple flavor variables before any include
CFLAGS :=
ASFLAGS :=


HOST_ROOT := ../../../../..
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

GOAL := libsensor_custom.a

CFLAGS += -Wall
CFLAGS += -Os
ifneq ($(ICAT_ON_LINUX), YES)
CFLAGS += -G 8
endif
CFLAGS += -ansi
CFLAGS += -I$(DEST_DIR)/include
CFLAGS += -I$(PLATFORM_ROOT)/inc
CFLAGS += -fno-common

#---------------------------------------------------------------------------
# Intermediate objects
#---------------------------------------------------------------------------
SEN_OBJS :=

# CISPLUS CMOS Image Sensor


# OmniVision CMOS Image Sensor

# Aptina CMOS Image Sensor
ifeq ($(DUAL_SENSOR), YES)
ifeq ($(PRJ_NAME),19DVR)
SEN_OBJS += model/sensor_tg_ar0230.o
SEN_OBJS += model/sensor_afe_ar0230.o
SEN_OBJS += model/sensor_tg_ar0230_tp2825.o
endif
else
ifeq ($(SENSOR_AR0230), YES)
SEN_OBJS += model/sensor_tg_ar0230.o
SEN_OBJS += model/sensor_afe_ar0230.o
else ifeq ($(SENSOR_OV4689), YES)
SEN_OBJS += model/sensor_tg_ov4689.o
SEN_OBJS += model/sensor_afe_ov4689.o
else ifeq ($(SENSOR_FAKE), YES)
SEN_OBJS += model/sensor_tg_fake.o
SEN_OBJS += model/sensor_afe_fake.o
endif
endif

SEN_OBJS += sensor_config.o


OBJS := $(SEN_OBJS)

#---------------------------------------------------------------------------
# Referring to generic rules
#---------------------------------------------------------------------------
$(eval $(call GEN_GOAL_RULES))
