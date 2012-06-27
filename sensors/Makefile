# Excuse this hack
VPATH=. ../common

ARDUINO_DIR = $(HOME)/Applications/Arduino.app/Contents/Resources/Java

TARGET = sensors
MCU = atmega328p
F_CPU = 16000000
ARDUINO_PORT = /dev/cu.usb*

NO_CORE = 1

# Excuse this hack too
EXTRA_CORE_SRCS = \
	HardwareSerial.cpp  Print.cpp \
	wiring_analog.cpp

VPATH := ..
LOCAL_SRCS := sensors.cc common/timing.c

include ../Arduino.mk
