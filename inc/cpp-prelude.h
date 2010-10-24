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
  // C++ number handling is awesome
  operator volatile T&() { return *(volatile T*) addr; }
  Mem<T, addr>& operator=(T x) { *(volatile T*) addr = x; return *this; }

  // instance versions could be written as *this |= _BV(bit), but
  // having the static can allow derivative templates to write
  // Mem<X,Y>::set_bit as well Mem<X,Y> mem; mem.set_bit

  template <unsigned char bit>
  static inline void set_bit() { *(volatile T*) addr |= _BV(bit); }

  template <unsigned char bit>
  static inline void clear_bit() { *(volatile T*) addr &= ~_BV(bit); }
};

// Curried aliases

// Mem8 = Mem uint8_t
template <unsigned int addr>
class Mem8 : public Mem<uint8_t, addr> {
  using Mem<uint8_t, addr>::operator=;
};

// Mem16 = Mem uint8_t
template <unsigned int addr>
class Mem16 : public Mem<uint16_t, addr> {
  using Mem<uint16_t, addr>::operator=;
};

// Mem32 = Mem uint8_t
template <unsigned int addr>
class Mem32 : public Mem<uint32_t, addr> {
  using Mem<uint32_t, addr>::operator=;
};

// IO8 = Mem uint8_t . (+__SFR_OFFSET)
template <unsigned int addr>
class IO8 : public Mem<uint8_t, addr+__SFR_OFFSET> {
  using Mem<uint8_t, addr+__SFR_OFFSET>::operator=;
};

// IO16 = Mem uint16_t . (+__SFR_OFFSET)
template <unsigned int addr>
class IO16 : public Mem<uint16_t, addr+__SFR_OFFSET> {
  using Mem<uint16_t, addr+__SFR_OFFSET>::operator=;
};

// Extern promises there's an instance, but we require no instance as
// all methods are inline

#if defined(_AVR_IOM328P_H_)
#  include "cpp-prelude/iom328p.h"
#else
#  error No prelude file for target processor
#endif

#endif
