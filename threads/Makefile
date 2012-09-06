ARDUINO_DIR=$(HOME)/Applications/Arduino.app/Contents/Resources/Java
ARDUINO_VARIANT_PATH := $(ARDUINO_DIR)/hardware/attiny/variants/tiny8

NO_CORE=1
EXTRA_CORE_SRCS := wiring.c wiring_digital.c

TARGET = threads
MCU := attiny85
F_CPU = 16500000
ARDUINO_PORT := /dev/tty.usbmodem*

FUSE_L  := 0xe1
FUSE_H  := 0xdd

AVRDUDE_OPTS := -c usbasp -p $(MCU)

CPP_PRELUDE_DIR := ..
include $(CPP_PRELUDE_DIR)/Arduino.mk
