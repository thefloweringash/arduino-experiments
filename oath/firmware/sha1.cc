#include <avr/eeprom.h>

#include <stdint.h>

#include <sha1.h>

#define INLINE __attribute__((always_inline))

uint32_t rotateL(uint32_t x, uint8_t off) INLINE;
uint32_t rotateL(uint32_t x, uint8_t off)
{
	return (x << off) | (x >> (32 - off));
}

void sha1_block::apply_xor(uint8_t xorConstant) {
	uint8_t *data8 = reinterpret_cast<uint8_t*>(&data[0]);
	for (int i = 0; i < sizeof(data); i++) {
		data8[i] ^= xorConstant;
	}
}

void sha1_block::init_with_bytes(const uint8_t *bytes, uint8_t bytesLen) {
	uint8_t *data8 = reinterpret_cast<uint8_t*>(&data[0]);
	for (uint8_t i = 0; i < bytesLen; i++) {
		data8[get_byte_idx(i)] = bytes[i];
	}
}

void sha1_block::init_with_eemem_bytes(const uint8_t *bytes, uint8_t bytesLen) {
	uint8_t *data8 = reinterpret_cast<uint8_t*>(&data[0]);
	for (uint8_t i = 0; i < bytesLen; i++) {
		data8[get_byte_idx(i)] = eeprom_read_byte(&bytes[i]);
	}
}

void sha1_block::apply_padding(uint32_t totalLen, uint8_t myLen) {
	totalLen = (totalLen + myLen) << 3;

	uint8_t *data8 = reinterpret_cast<uint8_t*>(&data[0]);
	data8[get_byte_idx(myLen)] = 0x80;
	memcpy(data8 + sizeof(data) - sizeof(totalLen),
	       &totalLen, sizeof(totalLen));
}


void sha1_update(sha1_state& state, sha1_block& block) {

	uint32_t a = state.h[0];
	uint32_t b = state.h[1];
	uint32_t c = state.h[2];
	uint32_t d = state.h[3];
	uint32_t e = state.h[4];

	for (uint8_t t = 0; t < 80; t++) {
		uint32_t w_t;
		if (t >= 16) {
			block.data[t & 0xf] = rotateL(
			    block.data[(t + 13) & 0xf] ^
			    block.data[(t +  8) & 0xf] ^
			    block.data[(t +  2) & 0xf] ^
			    block.data[(t     ) & 0xf],
			    1);
		}
		w_t = block.data[t & 0xf];
		uint32_t f_t;
		uint32_t k_t;
		if (t < 20) {
			f_t = (b & c) | ((~b) & d);
			k_t = 0x5A827999;
		}
		else if (t < 40) {
			f_t = b ^ c ^ d;
			k_t = 0x6ED9EBA1;
		}
		else if (t < 60) {
			f_t = (b & c) | (b & d) | (c & d);
			k_t = 0x8F1BBCDC;
		}
		else if (t < 80) {
			f_t = b ^ c ^ d;
			k_t = 0xCA62C1D6;
		}

		uint32_t temp = rotateL(a, 5) + f_t + e + w_t + k_t;
		e = d;
		d = c;
		c = rotateL(b, 30);
		b = a;
		a = temp;
	}

	state.h[0] += a;
	state.h[1] += b;
	state.h[2] += c;
	state.h[3] += d;
	state.h[4] += e;
}

void hmac_sha1(const uint8_t *secret, uint8_t secretLen,
               const uint8_t *data, uint8_t dataLen,
               sha1_state& state)
{
	sha1_state inner;
	sha1_block b;

	// do inner hash
	{
		b.init_with_bytes(secret, secretLen);
		b.apply_xor(0x36);
		sha1_update(inner, b);

		b.reset();
		b.init_with_bytes(data, dataLen);
		b.apply_padding(64, dataLen);
		sha1_update(inner, b);
	}

	// do outer hash
	{
		b.reset();
		b.init_with_bytes(secret, secretLen);
		b.apply_xor(0x5c);
		sha1_update(state, b);

		b.reset();
		b.init_with_sha1state(inner);
		b.apply_padding(64, 20);
		sha1_update(state, b);
	}
}

uint32_t dynamic_truncate(const sha1_state& st) {
	uint8_t offset = st.get_byte(19) & 0xf;
	uint32_t truncated =
	    (((uint32_t) (st.get_byte(offset+0) & 0x7f)) << 24) |
	    (((uint32_t)  st.get_byte(offset+1)) << 16) |
	    (((uint32_t)  st.get_byte(offset+2)) << 8) |
	    (((uint32_t)  st.get_byte(offset+3)) << 0);
	return truncated;
}
