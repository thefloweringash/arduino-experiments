#ifndef PARSE_H
#define PARSE_H

#include <stdio.h>
#include <stdint.h>

void append_bits(uint8_t *arr, size_t *bitOff,
                 uint32_t bits, uint8_t nBits);

int parse_hex_key(FILE *file, uint8_t *secret, const size_t secretLen);

int parse_base32_key(FILE *file, uint8_t *secret, const size_t secretLen);

#endif
