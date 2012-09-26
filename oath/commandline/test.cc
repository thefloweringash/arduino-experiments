#include <string.h>

#include "parse.h"

static int test_append_bits();

int main() {
	int fails = 0;
	fails += test_append_bits();
	if (fails) {
		printf("Test failures: %i\n", fails);
		return -1;
	}
	else {
		printf("All tests passed\n");
	}
}

static int test_append_bits() {
	int fails = 0;
	{
		uint8_t bytes[4] = {0};
		uint8_t bytes_expected[] = { 0x0b, 0xcd, 0xaf, 0xe0 };
		size_t bitOff = 4;
		append_bits(bytes, &bitOff, 0xbcdafe, 6 * 4);
		if (memcmp(bytes, bytes_expected, sizeof(bytes_expected) != 0)) {
			fprintf(stderr, "test_append_bits: test 1 append failed\n");
			fails++;
		}
		if (bitOff != 28) {
			fprintf(stderr, "test_append_bits: test 1 bitOff failed\n");
			fails++;
		}
	}
	{
		uint8_t bytes[5] = {0};
		uint8_t bytes_expected[] = { 0x00, 0xca, 0xfe, 0xba, 0xbe };
		size_t bitOff = 8;
		append_bits(bytes, &bitOff, 0xcafebabe, 8 * 4);
		if (memcmp(bytes, bytes_expected, sizeof(bytes_expected) != 0)) {
			fprintf(stderr, "test_append_bits: test 2 append failed\n");
			fails++;
		}
		if (bitOff != 40) {
			fprintf(stderr, "test_append_bits: test 2 bitOff failed\n");
			fails++;
		}
	}
	{
		uint8_t bytes[4] = {0};
		uint8_t bytes_expected[] = { 0b01110101, 0b11010101, 0b11110000, 0 };
		size_t bitOff = 1;
		append_bits(bytes, &bitOff, 0xebabf, 20);
		if (memcmp(bytes, bytes_expected, sizeof(bytes_expected) != 0)) {
			fprintf(stderr, "test_append_bits: test 3 append failed\n");
			fails++;
		}
		if (bitOff != 21) {
			fprintf(stderr, "test_append_bits: test 3 bitOff failed\n");
			fails++;
		}
	}
	{
		uint8_t bytes[4] = {0};
		uint8_t bytes_expected[] = { 0b01110101, 0b11010101, 0b11110000, 0 };
		size_t bitOff = 1;
		append_bits(bytes, &bitOff, 0xe, 4);
		append_bits(bytes, &bitOff, 0xb, 4);
		append_bits(bytes, &bitOff, 0xa, 4);
		append_bits(bytes, &bitOff, 0xb, 4);
		append_bits(bytes, &bitOff, 0xf, 4);
		if (memcmp(bytes, bytes_expected, sizeof(bytes_expected) != 0)) {
			fprintf(stderr, "test_append_bits: test 4 append failed\n");
			fails++;
		}
		if (bitOff != 21) {
			fprintf(stderr, "test_append_bits: test 4 bitOff failed\n");
			fails++;
		}
	}
	{
		uint8_t bytes[4] = {0};
		uint8_t bytes_expected[4] = { 0b01111101, 0b11110111, 0b11011111, 0  };
		size_t bitOff = 0;
		append_bits(bytes, &bitOff, 0b01111, 5);
		append_bits(bytes, &bitOff, 0b10111, 5);
		append_bits(bytes, &bitOff, 0b11011, 5);
		append_bits(bytes, &bitOff, 0b11101, 5);
		append_bits(bytes, &bitOff, 0b11110, 5);
		if (memcmp(bytes, bytes_expected, sizeof(bytes_expected) != 0)) {
			fprintf(stderr, "test_append_bits: test 5 append failed\n");
			fails++;
		}
		if (bitOff != 25) {
			fprintf(stderr, "test_append_bits: test 5 bitOff failed\n");
			fails++;
		}
	}
	return fails;
}
