// Package flit implements the FLIT64 algorithm.
// This is free and unencumbered software released into the public domain.
package flit

import (
	"encoding/binary"
	"math/bits"
)

// Uint64 decodes an integer from buf and returns that value and the number of
// bytes read. If the is buffer smaller than 9 bytes, Uint64 may panic.
func Uint64(buf []byte) (x uint64, n int) {
	x = binary.LittleEndian.Uint64(buf)

	a := bits.TrailingZeros64(x)
	if a > 7 {
		x = x>>8 | uint64(buf[8])<<56
		return x, 9
	}
	size := a + 1

	b := uint(size)
	trim := (8 - b) * 8
	x <<= trim
	x >>= trim
	x >>= b

	return x, size
}

// PutUint64 encodes an integer into buf and returns the number of bytes written.
// If the buffer is smaller than 9 bytes, PutUint64 may panic.
func PutUint64(buf []byte, x uint64) (n int) {
	lz := bits.LeadingZeros64(x)
	if lz > 56 {
		buf[0] = uint8(x)<<1 | 1
		return 1
	}
	if lz < 8 {
		buf[0] = 0
		binary.LittleEndian.PutUint64(buf[1:], x)
		return 9
	}

	// count extra bytes
	e := (63 - lz) / 7

	x <<= 1
	x |= 1
	x <<= uint(e)

	binary.LittleEndian.PutUint64(buf, x)
	return e + 1
}
