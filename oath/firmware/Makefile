ARDUINO_DIR=$(HOME)/Applications/Arduino.app/Contents/Resources/Java
# ARDUINO_VARIANT_PATH := $(ARDUINO_DIR)/hardware/attiny/variants/tiny8

LOCAL_SRCS := main.cc sha1.cc calibration.c \
	usbdrv/usbdrv.c usbdrv/usbdrvasm.S


NO_CORE=1
# EXTRA_CORE_SRCS := wiring.c wiring_digital.c

TARGET = oath
MCU := atmega16a
F_CPU = 12000000

FUSE_L  := 0xdf
FUSE_H  := 0xc1

AVRDUDE_OPTS := -c usbasp -p atmega16

CFLAGS := -Iusbdrv
CXXFLAGS := -Iusbdrv

CPP_PRELUDE_DIR := ../..
include $(CPP_PRELUDE_DIR)/Arduino.mk

