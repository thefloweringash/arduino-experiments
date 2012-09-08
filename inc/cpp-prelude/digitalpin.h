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

template <typename i_reg_t,
          typename o_reg_t,
          typename ddr_reg_t,
          unsigned char bit>
class DigitalIOPin : public DigitalPin<o_reg_t, ddr_reg_t, bit>  {

public:
	static inline bool get() { return !!i_reg_t::template get_bit<bit>(); }
};

#endif
