#ifndef CPP_PRELUDE_EEPROM_H
#define CPP_PRELUDE_EEPROM_H

#include <avr/eeprom.h>

template <typename T>
struct EEMemIO {};

template <>
struct EEMemIO<uint8_t> {
	inline static uint8_t read(uint8_t *x) {
		return eeprom_read_byte(x);
	}
	inline static void write(uint8_t *x, uint8_t value) {
		eeprom_write_byte(x, value);
	}
	inline static void update(uint8_t *x, uint8_t value) {
		eeprom_update_byte(x, value);
	}
};

template <>
struct EEMemIO<uint16_t> {
	inline static uint16_t read(uint16_t *x) {
		return eeprom_read_word(x);
	}
	inline static void write(uint16_t *x, uint16_t value) {
		eeprom_write_word(x, value);
	}
	inline static void update(uint16_t *x, uint16_t value) {
		eeprom_update_word(x, value);
	}
};

template <typename T>
class AutoEEMem {
	T mStorage;

	AutoEEMem<T>& operator=(AutoEEMem<T>& other);

public:
	inline operator T() {
		return EEMemIO<T>::read(&mStorage);
	}
	inline AutoEEMem<T>& operator=(T v) {
		EEMemIO<T>::update(&mStorage, v);
		return *this;
	}
};

#endif
