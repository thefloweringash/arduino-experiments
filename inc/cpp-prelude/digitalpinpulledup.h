#ifndef DIGITALPINPULLEDUP_H
#define DIGITALPINPULLEDUP_H

template <typename i_reg_t,
	  typename ddr_reg_t,
	  unsigned char bit>
class DigitalPinPulledUp {
public:
  static inline bool get() { return i_reg_t::template get_bit<bit>() != 0;  }

  static inline void pull_low()   { ddr_reg_t::template set_bit<bit>(); }
  static inline void float_high() { ddr_reg_t::template clear_bit<bit>(); }

  static inline void set(bool value) {
    if (value) {
      float_high();
    }
    else {
      pull_low();
    }
  }
};

#endif
