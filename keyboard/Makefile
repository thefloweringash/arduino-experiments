# Excuse this hack
VPATH=. ../common

ARDUINO_DIR = $(HOME)/Applications/Arduino.app/Contents/Resources/Java

TARGET = keyboard
MCU = atmega328p
F_CPU = 16000000
ARDUINO_PORT = /dev/cu.usb*

NO_CORE = 1

# Excuse this hack too
EXTRA_CORE_SRCS = HardwareSerial.o  Print.o
LOCAL_SRCS := keyboard.cc common/parity.c common/timing.c
VPATH := ..

include ../Arduino.mk
