#ifndef IOM328P_TIMERS_H
#define IOM328P_TIMERS_H

#include "cpp-prelude.h"
#include "cpp-prelude/generictimer.h"

namespace Timer0_Private {
  template <int p> struct prescaler_bits;

  template <> struct prescaler_bits<1> {
    static const int val = _BV(CS00);
  };

  template <> struct prescaler_bits<8> {
    static const int val = _BV(CS01);
  };

  template <> struct prescaler_bits<64> {
    static const int val = _BV(CS01) | _BV(CS00);
  };

  template <> struct prescaler_bits<256> {
    static const int val = _BV(CS02);
  };

  template <> struct prescaler_bits<1024> {
    static const int val = _BV(CS02) | _BV(CS00);
  };
};

class Timer0 {
public:
  static const uint8_t prescaler_bitmask =
    _BV(CS00) | _BV(CS01) | _BV(CS02);

  static const uint8_t overflow = _BV(TOIE0);

  typedef TCCR0B_t PrescalerControl_t;
  typedef TCNT0_t Counter_t;
  typedef TIMSK0_t Interrupts_t;

  template <int p>
  struct prescaler_bits {
    static const int val = Timer0_Private::prescaler_bits<p>::val;
  };
};


namespace Timer1_Private {
  template <int p> struct prescaler_bits;

  template <> struct prescaler_bits<1> {
    static const int val = _BV(CS10);
  };

  template <> struct prescaler_bits<8> {
    static const int val = _BV(CS11);
  };

  template <> struct prescaler_bits<64> {
    static const int val = _BV(CS11) | _BV(CS10);
  };

  template <> struct prescaler_bits<256> {
    static const int val = _BV(CS12);
  };

  template <> struct prescaler_bits<1024> {
    static const int val = _BV(CS12) | _BV(CS10);
  };
};

class Timer1 {
public:
  static const uint8_t prescaler_bitmask =
    _BV(CS10) | _BV(CS11) | _BV(CS12);

  static const uint8_t overflow = _BV(TOIE1);

  typedef TCCR1B_t PrescalerControl_t;
  typedef TCNT1_t Counter_t;
  typedef TIMSK1_t Interrupts_t;

  template <int p>
  struct prescaler_bits {
    static const int val = Timer1_Private::prescaler_bits<p>::val;
  };
};

namespace Timer2_Private {
  template <int p> struct prescaler_bits;

  template <> struct prescaler_bits<1> {
    static const int val = _BV(CS20);
  };

  template <> struct prescaler_bits<8> {
    static const int val = _BV(CS21);
  };

  template <> struct prescaler_bits<32> {
    static const int val = _BV(CS21) | _BV(CS20);
  };

  template <> struct prescaler_bits<64> {
    static const int val = _BV(CS22);
  };

  template <> struct prescaler_bits<128> {
    static const int val = _BV(CS22) | _BV(CS20);
  };

  template <> struct prescaler_bits<256> {
    static const int val = _BV(CS22) | _BV(CS21);
  };

  template <> struct prescaler_bits<1024> {
    static const int val = _BV(CS22) | _BV(CS21) | _BV(CS20);
  };
};

class Timer2 {
public:
  static const uint8_t prescaler_bitmask =
    _BV(CS20) | _BV(CS21) | _BV(CS22);

  static const uint8_t overflow = _BV(TOIE2);

  typedef TCCR2B_t PrescalerControl_t;
  typedef TCNT2_t Counter_t;
  typedef TIMSK2_t Interrupts_t;

  template <int p>
  struct prescaler_bits {
    static const int val = Timer2_Private::prescaler_bits<p>::val;
  };
};

extern GenericTimer<Timer0> timer0;
extern GenericTimer<Timer1> timer1;
extern GenericTimer<Timer2> timer2;

#endif
