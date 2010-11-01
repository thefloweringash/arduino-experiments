#include <avr/io.h>
#include <avr/interrupt.h>

#include "cpp-prelude.h"
#include "cpp-prelude/digitalpin.h"
#include "cpp-prelude/iom328p/timers.h"

// Program


int main() {
  cli();

  __asm__ ("/* START timer0.init_prescaler<8>() */");
  timer0.init_prescaler<8>();
  __asm__ ("/* END timer0.init_prescaler<8>() */");

  __asm__ ("/* START timer0.set_prescaler<8>() */");
  timer0.set_prescaler<8>();
  __asm__ ("/* END timer0.set_prescaler<8>() */");

  __asm__ ("/* START timer0.counter = 0 */");
  timer0.counter = 0;
  __asm__ ("/* END timer0.counter = 0 */");

  __asm__ ("/* START TCCR1B = (TCCR1B & ~7) |  3 */");
  TCCR1B = (TCCR1B & ~7) |  3;
  __asm__ ("/* END TCCR1B = (TCCR1B & ~7) |  3 */");

  __asm__ ("/* START timer1.set_prescaler<64>() */");
  timer1.set_prescaler<64>();
  __asm__ ("/* END timer1.set_prescaler<64>() */");

  __asm__ ("/* START timer2.set_prescaler<32>() */");
  timer2.set_prescaler<32>();
  __asm__ ("/* END timer2.set_prescaler<32>() */");

  __asm__ ("/* START timer1.enable_overflow_interrupt() */");
  timer1.enable_overflow_interrupt();
  __asm__ ("/* END timer1.enable_overflow_interrupt() */");

}
