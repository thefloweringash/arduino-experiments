#ifndef CPP_PRELUDE_H
#define CPP_PRELUDE_H 1

#if !defined(_AVR_IOXXX_H_)
#  error No _AVR_IOXXX_H_ file included
#endif

template <typename T, unsigned int addr>
class Mem {
private:
  static const T check_can_assign_numeric = 0;

public:
  typedef T type;

  // C++ number handling is awesome
  operator volatile T&() { return *(volatile T*) addr; }
  Mem<T, addr>& operator=(T x) { *(volatile T*) addr = x; return *this; }

  volatile T* operator&() { return (volatile T*)addr; }

  // instance versions could be written as *this |= _BV(bit), but
  // having the static can allow derivative templates to write
  // Mem<X,Y>::set_bit as well Mem<X,Y> mem; mem.set_bit

  template <unsigned char bit>
  static inline void set_bit() { *(volatile T*) addr |= _BV(bit); }

  template <unsigned char bit>
  static inline void clear_bit() { *(volatile T*) addr &= ~_BV(bit); }

  template <unsigned char bit>
  static inline type get_bit() { return *(volatile T*)addr & _BV(bit); }

  static inline void set(type x) { *(volatile T*)addr = x; }
  static inline type get() { return *(volatile T*)addr; }

};

// Curried aliases

// Mem8 = Mem uint8_t
template <unsigned int addr>
using Mem8 = Mem<uint8_t, addr>;

// Mem16 = Mem uint16_t
template <unsigned int addr>
using Mem16 = Mem<uint16_t, addr>;

// Mem32 = Mem uint32_t
template <unsigned int addr>
using Mem32 = Mem<uint32_t, addr>;

// IO8 = Mem uint8_t . (+__SFR_OFFSET)
template <unsigned int addr>
using IO8 = Mem8<addr + __SFR_OFFSET>;

// IO16 = Mem uint16_t . (+__SFR_OFFSET)
template <unsigned int addr>
using IO16 = Mem16<addr + __SFR_OFFSET>;

// Extern promises there's an instance, but we require no instance as
// all methods are inline

#if defined(_AVR_IOM328P_H_)
#  include "cpp-prelude/iom328p.h"
#elif defined(_AVR_IOTNX5_H_)
#  include "cpp-prelude/iotnx5.h"
#elif defined (__AVR_ATmega16A__)
#  include "cpp-prelude/iom16a.h"
#else
#  error No prelude file for target processor
#endif

#endif
