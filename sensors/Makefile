# Excuse this hack
VPATH=. ../common

ARDUINO_DIR = $(HOME)/Applications/Arduino.app/Contents/Resources/Java

TARGET = sensors
MCU = atmega328p
F_CPU = 16000000
ARDUINO_PORT = /dev/cu.usb*

NO_CORE = 1

# Excuse this hack too
CORE_OBJS = 	$(OBJDIR)/HardwareSerial.o \
		$(OBJDIR)/Print.o \
		$(OBJDIR)/wiring_analog.o \
		$(OBJDIR)/timing.o

include ../Arduino.mk
