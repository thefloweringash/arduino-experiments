#ifndef GENERICTIMER_H
#define GENERICTIMER_H

template <typename T>
class GenericTimer {
public:
  static typename T::PrescalerControl_t prescaler_control;
  static typename T::Counter_t counter;
  static typename T::Interrupts_t interrupts;

  template <int p>
  static inline void init_prescaler() {
    prescaler_control |= T::template prescaler_bits<p>::val;
  }

  template <int p>
  static inline void set_prescaler() {
    prescaler_control = (prescaler_control & ~T::prescaler_bitmask)
      | T::template prescaler_bits<p>::val;
  }

  static inline void enable_overflow_interrupt() {
    interrupts |= T::overflow;
  }

  static inline void disable_overflow_interrupt() {
    interrupts &= ~T::overflow;
  }
};

#endif
