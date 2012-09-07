#include <stdint.h>
#include <string.h>

struct sha1_state {
	uint32_t h[5];

	sha1_state()
	{
		h[0] = 0x67452301;
		h[1] = 0xEFCDAB89;
		h[2] = 0x98BADCFE;
		h[3] = 0x10325476;
		h[4] = 0xC3D2E1F0;
	}

	sha1_state(uint32_t h0,
	           uint32_t h1,
	           uint32_t h2,
	           uint32_t h3,
	           uint32_t h4)
	{
		h[0] = h0;
		h[1] = h1;
		h[2] = h2;
		h[3] = h3;
		h[4] = h4;
	}

	uint8_t get_byte(uint8_t byte) const {
		uint32_t base = h[byte / 4];
		switch (byte % 4) {
		case 0: return (base >> 24) & 0xff;
		case 1: return (base >> 16) & 0xff;
		case 2: return (base >> 8) & 0xff;
		case 3: return (base >> 0) & 0xff;
		}
		return -1; // unreachable
	}
};


struct sha1_block {
	uint32_t data[16];

	sha1_block() {
		reset();
	}

	// todo: deinline
	void apply_xor(uint8_t xorConstant);

	inline void reset() {
		memset(data, 0, sizeof(data));
	}

	inline void init_with_sha1state(const sha1_state& st) {
		memcpy(data, st.h, sizeof(st.h));
	}

	void init_with_bytes(const uint8_t *bytes, uint8_t bytesLen);
	void init_with_eemem_bytes(const uint8_t *bytes, uint8_t bytesLen);

	inline void init_with_uint32(const uint32_t *words, uint8_t nWords) {
		memcpy(data, words, nWords * sizeof(*words));
	}

	void apply_padding(uint32_t totalLen, uint8_t myLen);

	inline uint8_t get_byte_idx(uint8_t i) const __attribute__((always_inline)) {
		return (i & ~0x3) + 3 - (i%4);
	}
};

void sha1_update(sha1_state& state, sha1_block& block);

void hmac_sha1(const uint8_t *secret, uint8_t secretLen,
               const uint8_t *data, uint8_t dataLen,
               sha1_state& state);

uint32_t dynamic_truncate(const sha1_state& st);
