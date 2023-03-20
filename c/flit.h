// FLIT64 Implementation

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Decodes buf into v and returns the serial octet size.
size_t flit64_dec(uint64_t* v, const void* buf) {
	uint64_t x = *(uint64_t*)buf;

	int tzc = 8;
	if (x) tzc = __builtin_ctzll(x);
	if (tzc > 7) {
		uint8_t* cp = (uint8_t*)buf;
		*v = *(uint64_t*)++cp;
		return 9;
	}

	static const uint64_t mask[8] = {
		0xff,
		0xffff,
		0xffffff,
		0xffffffff,
		0xffffffffff,
		0xffffffffffff,
		0xffffffffffffff,
		0xffffffffffffffff,
	};
	x &= mask[tzc];

	// const here seems to ensure that 'size' is not aliased by '*v'
	const size_t size = tzc + 1;

	*v = x >> size;

	return size;
}

// Decodes buf into v and returns the serial octet size.
size_t flit64s_dec(int64_t* v, const void* buf) {
	uint64_t u;
	size_t n = flit64_dec(&u, buf);
	*v = (u >> 1) ^ (~(u & 1) + 1);
	return n;
}

// Encodes v into buf and returns the serial octet size.
size_t flit64_enc(void* buf, uint64_t v) {
	if (v < 128) {
		*(uint8_t*)buf = (uint8_t)v << 1 | 1;
		return 1;
	}
	if (v >= (uint64_t)1 << 56) {
		uint8_t* p = (uint8_t*)buf;
		*p++ = 0;
		*(uint64_t*)p = v;
		return 9;
	}

	int lzc = __builtin_clzll(v);
	// extra bytes = (bits - 1) / 7 = (63 - lzc) / 7
	size_t e = ((63 - lzc) * 2454267027) >> 34;

	v <<= 1;
	v |= 1;
	v <<= e;
	*(uint64_t*)buf = v;

	return e + 1;
}

// Encodes v into buf and returns the serial octet size.
size_t flit64s_enc(void* buf, int64_t v) {
	return flit64_enc(buf, (v << 1) ^ (v >> 63));
}

#ifdef __cplusplus
} // extern "C"
#endif
