#ifndef PARITY_H
#define PARITY_H

extern const bool ParityTable256[256];

inline bool check_odd_parity(uint8_t byte, bool parity_bit) {
  return ParityTable256[byte] == !parity_bit;
}

#endif
