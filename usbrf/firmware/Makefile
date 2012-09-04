ARDUINO_DIR := $(HOME)/Applications/Arduino.app/Contents/Resources/Java
ARDUINO_VARIANT_PATH := $(ARDUINO_DIR)/hardware/attiny/variants/tiny8
ARDUINO_LIBS := RemoteReceiver RemoteTransmitter

ARDUINO_LIB_PATH := libs

LOCAL_SRCS := main.cc calibration.c \
	usbdrv/usbdrv.c usbdrv/usbdrvasm.S usbdrv/oddebug.c \
	wiring.c

EXTRA_CORE_SRCS :=  wiring_digital.c

TARGET = usbrf
MCU = attiny85
F_CPU = 16500000
NO_CORE=1

CFLAGS   := -Iusbdrv -DREMOTE_RECEIVER_USE_ATTACH_INTERRUPT=0
CXXFLAGS := -Iusbdrv -DREMOTE_RECEIVER_USE_ATTACH_INTERRUPT=0

FUSE_L  := 0xe1
FUSE_H  := 0xdd

AVRDUDE_OPTS := -c usbasp -p $(MCU)

CPP_PRELUDE_DIR := ../..
include $(CPP_PRELUDE_DIR)/Arduino.mk
