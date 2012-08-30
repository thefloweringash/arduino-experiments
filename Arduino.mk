########################################################################
#
# Arduino command line tools Makefile
# System part (i.e. project independent)
#
# Copyright (C) 2010 Martin Oldfield <m@mjo.tc>, based on work that is
# Copyright Nicholas Zambetti, David A. Mellis & Hernando Barragan
# 
# This file is free software; you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation; either version 2.1 of the
# License, or (at your option) any later version.
#
# Adapted from Arduino 0011 Makefile by M J Oldfield
#
# Original Arduino adaptation by mellis, eighthave, oli.keller
#
# Version 0.1  17.ii.2009  M J Oldfield
#
#         0.2  22.ii.2009  M J Oldfield
#                          - fixes so that the Makefile actually works!
#                          - support for uploading via ISP
#                          - orthogonal choices of using the Arduino for
#                            tools, libraries and uploading
#
#         0.3  21.v.2010   M J Oldfield
#                          - added proper license statement
#                          - added code from Philip Hands to reset
#                            Arduino prior to upload
#
#         0.4  25.v.2010   M J Oldfield
#                          - tweaked reset target on Philip Hands' advice
#
#         ...              Hacked to bits by lorne@cons.org.nz
########################################################################
#
# STANDARD ARDUINO WORKFLOW
#
# Given a normal sketch directory, all you need to do is to create
# a small Makefile which defines a few things, and then includes this one.
#
# For example:
#
#       ARDUINO_DIR  := /Applications/arduino-0013
#
#       TARGET       := CLItest
#       ARDUINO_LIBS := LiquidCrystal
#
#       MCU          := atmega168
#       F_CPU        := 16000000
#       ARDUINO_PORT := /dev/cu.usb*
#
#       include /usr/local/share/Arduino.mk
#
# Hopefully these will be self-explanatory but in case they're not:
#
#    ARDUINO_DIR  - Where the Arduino software has been unpacked
#    TARGET       - The basename used for the final files. Canonically
#                   this would match the .pde file, but it's not needed
#                   here: you could always set it to xx if you wanted!
#    ARDUINO_LIBS - A list of any libraries used by the sketch (we assume
#                   these are in $(ARDUINO_DIR)/hardware/libraries
#    MCU,F_CPU    - The target processor description
#    ARDUINO_PORT - The port where the Arduino can be found (only needed
#                   when uploading
#
# Once this file has been created the typical workflow is just
#
#   $ make upload
#
# All of the object files are created in the build-cli subdirectory
# All sources should be in the current directory and can include:
#  - at most one .pde file which will be treated as C++ after the standard
#    Arduino header and footer have been affixed.
#  - any number of .c, .cpp, .s and .h files
#
#
# Besides make upload you can also
#   make            - no upload
#   make clean      - remove all our dependencies
#   make reset      - reset the Arduino by tickling DTR on the serial port
#   make raw_upload - upload without first resetting
#
########################################################################
#
# ARDUINO WITH ISP
#
# You need to specify some details of your ISP programmer and might
# also need to specify the fuse values:
#
#     ISP_PROG	   := -c stk500v2
#     ISP_PORT     := /dev/ttyACM0
#     
#     ISP_LOCK_FUSE_PRE  := 0x3f
#     ISP_LOCK_FUSE_POST := 0xcf
#     ISP_HIGH_FUSE      := 0xdf
#     ISP_LOW_FUSE       := 0xff
#     ISP_EXT_FUSE       := 0x01
#
# I think the fuses here are fine for uploading to the ATmega168
# without bootloader.
# 
# To actually do this upload use the ispload target:
#
#    make ispload
#
#
########################################################################
# Some paths
#

ifneq ($(ARDUINO_DIR),)

ARDUINO_LIB_PATH  := $(ARDUINO_DIR)/libraries
ARDUINO_CORE_PATH := $(ARDUINO_DIR)/hardware/arduino/cores/arduino

ifeq ($(ARDUINO_VARIANT_PATH),)
ARDUINO_VARIANT_PATH := $(ARDUINO_DIR)/hardware/arduino/variants/standard
endif

endif

ifeq ($(CPP_PRELUDE_DIR),)
CPP_PRELUDE_DIR := ..
endif

# Everything gets built in here
OBJDIR  	  := obj

########################################################################
# Local sources
#
ifeq ($(LOCAL_SRCS),)
LOCAL_SRCS    := $(wildcard *.c) $(wildcard *.cpp) $(wildcard *.cc) $(wildcard *.pde) $(wildcard *.S)
endif
LOCAL_OBJ_FILES := $(addsuffix .o,$(basename $(LOCAL_SRCS)))
LOCAL_OBJS      := $(addprefix $(OBJDIR)/,$(LOCAL_OBJ_FILES))


# core sources
ifeq ($(strip $(NO_CORE)),)
ifdef ARDUINO_CORE_PATH
CORE_SRCS     := $(wildcard $(ARDUINO_CORE_PATH)/*.c) $(wildcard $(ARDUINO_CORE_PATH)/*.cpp)
endif
else
CORE_SRCS     := $(addprefix $(ARDUINO_CORE_PATH)/,$(EXTRA_CORE_SRCS))
endif
CORE_OBJ_FILES  := $(addsuffix .o,$(basename $(CORE_SRCS)))
CORE_OBJS       := $(patsubst $(ARDUINO_DIR)/%,  \
			$(OBJDIR)/%,$(CORE_OBJ_FILES))

# The name of the main targets
TARGET_HEX := $(OBJDIR)/$(TARGET).hex
TARGET_ELF := $(OBJDIR)/$(TARGET).elf
TARGET_MAP := $(OBJDIR)/$(TARGET).map
TARGETS    := $(OBJDIR)/$(TARGET).*

# Names of executables
CC      := avr-gcc
CXX     := avr-g++
OBJCOPY := avr-objcopy
OBJDUMP := avr-objdump
AR      := avr-ar
SIZE    := avr-size
NM      := avr-nm
REMOVE  := rm -f
MV      := mv -f
CAT     := cat
ECHO    := echo

# General arguments
SYS_LIBS      := $(addprefix $(ARDUINO_LIB_PATH)/,$(ARDUINO_LIBS))
SYS_INCLUDES  := $(addprefix -I,$(SYS_LIBS))
SYS_SRCS      := \
	$(wildcard $(addsuffix /*.c,$(SYS_LIBS))) \
	$(wildcard $(addsuffix /*.cpp,$(SYS_LIBS)))
SYS_OBJ_FILES := $(addsuffix .o,$(basename $(SYS_SRCS)))
SYS_OBJS      :=  $(patsubst $(ARDUINO_DIR)/%,  \
			$(OBJDIR)/%,$(SYS_OBJ_FILES))
# all the objects!
OBJS            := $(LOCAL_OBJS) $(CORE_OBJS) $(SYS_OBJS)

# Dependency files
DEPS            := $(OBJS:.o=.d)

ALL_OBJDIR_SENTINELS := $(addsuffix /.made,$(dir $(OBJS)))

INTERNAL_FLAGS      := -mmcu=$(MCU) -DF_CPU=$(F_CPU) \
                        -I. -I$(ARDUINO_CORE_PATH) \
                        -I $(ARDUINO_VARIANT_PATH) \
                        $(SYS_INCLUDES) -g -Os -w -Wall \
                        -ffunction-sections -fdata-sections \
                        -I$(CPP_PRELUDE_DIR)/inc \
                        -D__PROG_TYPES_COMPAT__ \
                        -DARDUINO=100
INTERNAL_CFLAGS        := -std=gnu99
INTERNAL_CXXFLAGS      := -fno-exceptions -std=c++11
INTERNAL_ASFLAGS       := -mmcu=$(MCU) -I. -x assembler-with-cpp
INTERNAL_LDFLAGS       := -mmcu=$(MCU) -lm -Wl,--gc-sections -Os

# Expand and pick the first port
ARD_PORT      := $(firstword $(wildcard $(ARDUINO_PORT)))

VPATH := $(VPATH) $(ARDUINO_DIR)

########################################################################
# Rules for making stuff
#

$(OBJDIR)/%.o: %.c
	$(CC) -MMD -c $(INTERNAL_FLAGS) $(INTERNAL_CFLAGS) $(CFLAGS) $< -o $@

$(OBJDIR)/%.o: %.cc
	$(CXX) -MMD -c $(INTERNAL_FLAGS) $(INTERNAL_CXXFLAGS) $(CXXFLAGS) $< -o $@

$(OBJDIR)/%.o: %.cpp
	$(CXX) -MMD -c $(INTERNAL_FLAGS) $(INTERNAL_CXXFLAGS) $(CXXFLAGS) $< -o $@

$(OBJDIR)/%.o: %.S
	$(CC) -MMD -c $(INTERNAL_FLAGS) $(INTERNAL_ASFLAGS) $(ASFLAGS) $< -o $@

$(OBJDIR)/%.o: %.s
	$(CC) -MMD -c $(INTERNAL_FLAGS) $(INTERNAL_LDFLAGS) $(ASFLAGS) $< -o $@


# We won't support building with .pde/.ino files, too hard.

# various object conversions
$(OBJDIR)/%.hex: $(OBJDIR)/%.elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@
	avr-size $@

$(OBJDIR)/%.eep: $(OBJDIR)/%.elf
	-$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
		--change-section-lma .eeprom=0 -O ihex $< $@

$(OBJDIR)/%.lss: $(OBJDIR)/%.elf
	$(OBJDUMP) -h -S $< > $@

$(OBJDIR)/%.sym: $(OBJDIR)/%.elf
	$(NM) -n $< > $@

########################################################################
#
# Avrdude
#
AVRDUDE := avrdude

AVRDUDE_COM_OPTS := -q -V -p $(MCU)
ifdef AVRDUDE_CONF
AVRDUDE_COM_OPTS += -C $(AVRDUDE_CONF)
endif

ifndef AVRDUDE_ARD_PROGRAMMER
AVRDUDE_ARD_PROGRAMMER := arduino
endif

ifndef AVRDUDE_ARD_BAUDRATE
AVRDUDE_ARD_BAUDRATE   := 115200
endif

AVRDUDE_ARD_OPTS := -c $(AVRDUDE_ARD_PROGRAMMER) -b $(AVRDUDE_ARD_BAUDRATE) -P $(ARD_PORT)

ifndef ISP_LOCK_FUSE_PRE
ISP_LOCK_FUSE_PRE  := 0x3f
endif

ifndef ISP_LOCK_FUSE_POST
ISP_LOCK_FUSE_POST := 0xcf
endif

ifndef ISP_HIGH_FUSE
ISP_HIGH_FUSE      := 0xdf
endif

ifndef ISP_LOW_FUSE
ISP_LOW_FUSE       := 0xff
endif

ifndef ISP_EXT_FUSE
ISP_EXT_FUSE       := 0x01
endif

ifndef ISP_PROG
ISP_PROG	   := -c stk500v2
endif

AVRDUDE_ISP_OPTS := -P $(ISP_PORT) $(ISP_PROG)


########################################################################
#
# Explicit targets start here
#

all: 		$(TARGET_HEX)

$(OBJS): | $(ALL_OBJDIR_SENTINELS)

%/.made:
	mkdir -p $$(dirname $@)
	touch $@

$(TARGET_ELF): 	$(OBJS)
		$(CC) -Wl,-Map -Wl,$(TARGET_MAP) $(INTERNAL_LDFLAGS) $(LDFLAGS) -o $@ $(OBJS)
		avr-size $@

# upload, raw_upload, reset and ispload are inherited from earlier
# revisions and preserved in case someone finds them useful.

upload:		reset raw_upload

raw_upload:	$(TARGET_HEX)
		$(AVRDUDE) $(AVRDUDE_COM_OPTS) $(AVRDUDE_ARD_OPTS) \
			-U flash:w:$(TARGET_HEX):i

# stty on MacOS likes -F, but on Debian it likes -f redirecting
# stdin/out appears to work but generates a spurious error on MacOS at
# least. Perhaps it would be better to just do it in perl ?
reset:		
		for STTYF in 'stty --file' 'stty -f' 'stty <' ; \
		  do $$STTYF /dev/tty >/dev/null 2>/dev/null && break ; \
		done ;\
		$$STTYF $(ARD_PORT)  hupcl ;\
		(sleep 0.1 || sleep 1)     ;\
		$$STTYF $(ARD_PORT) -hupcl 

ispload:	$(TARGET_HEX)
		$(AVRDUDE) $(AVRDUDE_COM_OPTS) $(AVRDUDE_ISP_OPTS) -e \
			-U lock:w:$(ISP_LOCK_FUSE_PRE):m \
			-U hfuse:w:$(ISP_HIGH_FUSE):m \
			-U lfuse:w:$(ISP_LOW_FUSE):m \
			-U efuse:w:$(ISP_EXT_FUSE):m
		$(AVRDUDE) $(AVRDUDE_COM_OPTS) $(AVRDUDE_ISP_OPTS) -D \
			-U flash:w:$(TARGET_HEX):i
		$(AVRDUDE) $(AVRDUDE_COM_OPTS) $(AVRDUDE_ISP_OPTS) \
			-U lock:w:$(ISP_LOCK_FUSE_POST):m


# flash, fuse are simple options that should be familiar to vusb
# users.

fuse:
	@if [ -z "$(FUSE_H)" ] || [ -z "$(FUSE_L)" ]; then \
		[ -z "$(FUSE_H)" ] && echo "FUSE_H not set"; \
		[ -z "$(FUSE_L)" ] && echo "FUSE_L not set"; \
		exit 1; \
	fi
	$(AVRDUDE) $(AVRDUDE_OPTS) \
		-U hfuse:w:$(FUSE_H):m \
		-U lfuse:w:$(FUSE_L):m

flash: $(TARGET_HEX)
	$(AVRDUDE) $(AVRDUDE_OPTS) -U flash:w:$(TARGET_HEX):i

clean:
		[ -d $(OBJDIR) ] && rm -r $(OBJDIR) || true

.PHONY:	all clean depends upload raw_upload reset

-include $(DEPS)
