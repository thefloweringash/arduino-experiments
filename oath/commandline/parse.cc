#include <ctype.h>
#include "parse.h"

static inline int hexValue(int c) {
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	else if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	else if (c >= '0' && c <= '9')
		return c - '0';

	return -1;
}

template <typename T, typename U>
static inline T min(const T x, const U y) {
	if (x < y)
		return x;
	else
		return y;
}

int parse_hex_key(FILE *file, uint8_t *secret, const size_t secretLen) {
	uint8_t *p = &secret[0];
	uint8_t *end = &secret[secretLen];

	while (p < end) {
		int c1 = hexValue(fgetc(file));
		int c2 = hexValue(fgetc(file));
		if (c1 == -1 || c2 == -1) {
			break;
		}
		*p++ = (c1 << 4) | c2;
	}
	return p - &secret[0];
}

void append_bits(uint8_t *arr, size_t *bitOff,
				 uint32_t bits, uint8_t nBits)
{
	while (nBits) {
		size_t byte = *bitOff / 8;
		int sizeInByte = 8 - (*bitOff % 8);
		int bitsToTake = min(nBits, sizeInByte);
		uint32_t nBitMask = (1 << bitsToTake) - 1;
		uint8_t takenBits = (bits >> (nBits - bitsToTake)) & nBitMask;
		arr[byte] |= takenBits << (sizeInByte - bitsToTake);
		*bitOff += bitsToTake;
		nBits -= bitsToTake;
	}
}

static inline int base32_value(int c) {
	if (c >= 'A' && c <= 'Z')
		return c - 'A';
	else if (c >= 'a' && c <= 'z')
		return c - 'a';
	else if (c >= '2' && c <= '7')
		return c - '2' + 26;

	return -1;
}

int parse_base32_key(FILE *file, uint8_t *secret, const size_t secretLen) {
	uint8_t *end = &secret[secretLen];

	size_t bitOff = 0;
	while (bitOff + 5 < secretLen * 8) {
		int ch = fgetc(file);
		int chValue = base32_value(ch);

		if (ch == ' ')
			continue;
		else if (ch == EOF || chValue == -1)
			break;

		append_bits(secret, &bitOff, chValue, 5);
	}
	return bitOff;
}
