#ifndef PS2KEYBOARD_H
#define PS2KEYBOARD_H

#include "timing.h"
#include "parity.h"

template <typename data_pin_t, typename clock_pin_t>
class PS2Keyboard {
private:
  data_pin_t data_pin;
  clock_pin_t clock_pin;

  volatile uint8_t scancode_index; // 0-10
  uint8_t scancode_buffer; // 0-255

  bool full;
  bool dropping_byte;

  volatile bool _sending;
  uint8_t send_buffer;

public:
  inline void interrupt() {
    uint8_t i = scancode_index++;

    if (_sending) {
      // sending

      if (i == 0) {
	// start bit already set
      }
      else if (i == 9) {
	// parity
	data_pin.set(!ParityTable256[send_buffer]);
      }
      else if (i == 10) {
	data_pin.float_high();
	// stop
      }
      else if (i == 11) {
	// ack
	scancode_index = 0;
	_sending = false;
      }
      else {
	// i = 1..8
	// data
	data_pin.set(send_buffer & _BV(i - 1));
      }
    }
    else {
      // receiving
      if (dropping_byte) {
	if (i == 10) {
	  // stop bit
	  dropping_byte = false;
	  scancode_index = 0;
	}
      }
      else {
	if (i == 0) {
	  // start bit
	  if (full) {
	    dropping_byte = true;
	  }
	  else {
	    scancode_buffer = 0;
	  }
	}
	else if (i == 9) {
	  // parity bit
	  if (!check_odd_parity(scancode_buffer, data_pin.get())) {
	    dropping_byte = true;
	  }
	}
	else if (i == 10) {
	  // stop bit
	  scancode_index = 0;
	  full = true;
	}
	else {
	  // i = 1..8
	  // data bit
	  if (data_pin.get()) {
	    scancode_buffer |= _BV(i - 1);
	  }
	}
      }
    }
  }

  inline void begin() {
    scancode_index = 0;
    scancode_buffer = 0;
    full = false;
    dropping_byte = false;

    data_pin.float_high();
    clock_pin.float_high();
  }

  inline bool available() {
    return full;
  }

  void send(uint8_t b) {
    send_buffer = b;
    while (scancode_index != 0 || !clock_pin.get());
    clock_pin.pull_low(); // take clock
    _sending = true;
    delayMicroseconds(60);
    data_pin.pull_low(); // set start bit
    delayMicroseconds(40);
    clock_pin.float_high(); // give clock back
  }

  inline bool sending() {
    return _sending;
  }

  inline uint8_t read() {
    uint8_t x = scancode_buffer;
    full = false;
    scancode_buffer = 0;
    return x;
  }
};

#endif
