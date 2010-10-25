#ifndef DIGITALPIN_H
#define DIGITALPIN_H

template <typename o_reg_t,
	  typename ddr_reg_t,
	  unsigned char bit>
class DigitalPin {

public:
  static inline void setDDR()   { ddr_reg_t::template set_bit<bit>(); }
  static inline void clearDDR() { ddr_reg_t::template clear_bit<bit>(); }

  static inline void set()      { o_reg_t::template set_bit<bit>(); }
  static inline void clear()    { o_reg_t::template clear_bit<bit>(); }

  static inline void set(bool value) {
    if (value) {
      set();
    }
    else {
      clear();
    }
  }
};

#endif
