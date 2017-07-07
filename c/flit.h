// FLIT64 Implementation
// This is free and unencumbered software released into the public domain.

#include <stddef.h>
#include <stdint.h>

#ifndef __amd64__
#error "first reference implementation AMD64 only"
#endif

// Decodes buf into v and returns the serial octet size.
int flit64dec(uint64_t* v, void* buf) {
	uint64_t* p = (uint64_t*)buf;
	uint64_t x = *p;

	int tzc = 8;
	if (x) tzc = __builtin_ctzll(x);
	if (tzc > 7) {
		uint8_t c = *(uint8_t*)(++p);
		*v = x >> 8 | (uint64_t)c << 56;
		return 9;
	}

	// const here seems to ensure that 'size' is not aliased by '*v'
	const int size = tzc + 1;

	static const uint64_t mask[7] = {
		0xff,
		0xffff,
		0xffffff,
		0xffffffff,
		0xffffffffff,
		0xffffffffffff,
		0xffffffffffffff
	};
	*v = (x >> size) & mask[tzc];
	// A shorter but slower alternative:
	// *v = (x >> size) & ((1ull << (size * 8)) - 1);

	return size;
}

// Encodes v into buf and returns the serial octet size.
int flit64enc(void* buf, uint64_t v) {
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

	// count extra bytes
	int e = (63 - lzc) / 7;

	v <<= 1;
	v |= 1;
	v <<= e;
	*(uint64_t*)buf = v;

	return e + 1;
}
