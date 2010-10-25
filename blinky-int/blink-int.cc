#include <avr/io.h>
#include <avr/interrupt.h>

#include "cpp-prelude.h"
#include "cpp-prelude/digitalpin.h"

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

