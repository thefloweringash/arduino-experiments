ARDUINO_DIR=$(HOME)/Applications/Arduino.app/Contents/Resources/Java
# ARDUINO_VARIANT_PATH := $(ARDUINO_DIR)/hardware/attiny/variants/tiny8

LOCAL_SRCS := main.cc sha1.cc calibration.c \
	usbdrv/usbdrv.c usbdrv/usbdrvasm.S


NO_CORE=1
# EXTRA_CORE_SRCS := wiring.c wiring_digital.c

TARGET = oath
MCU := attiny85
F_CPU = 16500000

FUSE_L  := 0xe1
FUSE_H  := 0xd5

AVRDUDE_OPTS := -c usbasp -p $(MCU)

CFLAGS := -Iusbdrv
CXXFLAGS := -Iusbdrv

CPP_PRELUDE_DIR := ../..
include $(CPP_PRELUDE_DIR)/Arduino.mk

