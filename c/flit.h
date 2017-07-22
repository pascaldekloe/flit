// FLIT64 Implementation
// This is free and unencumbered software released into the public domain.

#include <stddef.h>
#include <stdint.h>

#ifndef __amd64__
#error "first reference implementation AMD64 only"
#endif

// Decodes buf into v and returns the serial octet size.
size_t flit64dec(uint64_t* v, const void* buf) {
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

// Encodes v into buf and returns the serial octet size.
size_t flit64enc(void* buf, uint64_t v) {
	int lzc = 64;
	if (v) lzc = __builtin_clzll(v);
	if (lzc > 56) {
		*(uint8_t*)buf = (uint8_t)v << 1 | 1;
		return 1;
	}
	if (lzc < 8) {
		uint8_t* p = (uint8_t*)buf;
		*p++ = 0;
		*(uint64_t*)p = v;
		return 9;
	}

	// extra bytes = (bits - 1) / 7 = (63 - lzc) / 7
	size_t e = ((63 - lzc) * 2454267027) >> 34;

	v <<= 1;
	v |= 1;
	v <<= e;
	*(uint64_t*)buf = v;

	return e + 1;
}
