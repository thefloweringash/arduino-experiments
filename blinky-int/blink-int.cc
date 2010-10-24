#include <avr/io.h>
#include <avr/interrupt.h>

#include "cpp-prelude.h"

// Simple digital output

template <typename io_reg_t,
	  typename ddr_reg_t,
	  unsigned char bit>
class DigitalPin {

public:
  inline void setDDR()   { ddr_reg_t::template set_bit<bit>(); }
  inline void clearDDR() { ddr_reg_t::template clear_bit<bit>(); }

  inline void set()      { io_reg_t::template set_bit<bit>(); }
  inline void clear()    { io_reg_t::template clear_bit<bit>(); }

  inline void set(bool value) {
    if (value) {
      set();
    }
    else {
      clear();
    }
  }
};

// Program

extern DigitalPin<PORTB_t, DDRB_t, PORTB5> led;

bool ledval = true;

int main() {
  cli();
  led.setDDR();

  TCCR1B = _BV(CS12) | _BV(CS10) | _BV(WGM12);
  OCR1A = 15625; // 1 second, 1024 prescaler
  TIMSK1 |= _BV(OCIE1A);

  sei();
  for(;;);
}

ISR(TIMER1_COMPA_vect) {
  ledval = !ledval;
  led.set(ledval);
}

