#!/bin/bash
write_key() { avrdude -c usbasp -p attiny85 -U eeprom:w:eeprom-with-secret:r; }
read_eeprom() { avrdude -c usbasp -p attiny85 -U eeprom:r:eeprom-dump:i; }

