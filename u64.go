// Package flit implements the FLIT64 algorithm.
// This is free and unencumbered software released into the public domain.
package flit

import (
	"encoding/binary"
	"math/bits"
)

var readMasks = [...]uint64{
	0xff,
	0xffff,
	0xffffff,
	0xffffffff,
	0xffffffffff,
	0xffffffffffff,
	0xffffffffffffff,
	0xffffffffffffffff,
}

// Uint64 decodes an integer from buf and returns that value and the serial size.
// If the is buffer smaller than 9 bytes, Uint64 may panic.
func Uint64(buf []byte) (v uint64, n int) {
	v = binary.LittleEndian.Uint64(buf)

	tz := bits.TrailingZeros64(v)
	if tz > 7 {
		v = binary.LittleEndian.Uint64(buf[1:])
		return v, 9
	}

	v &= readMasks[tz]

	size := tz + 1
	v >>= uint(size)
	return v, size
}

// PutUint64 encodes an integer into buf and returns the serial size.
// If the buffer is smaller than 9 bytes, PutUint64 may panic.
func PutUint64(buf []byte, v uint64) (n int) {
	lz := bits.LeadingZeros64(v)
	if lz > 56 {
		buf[0] = uint8(v)<<1 | 1
		return 1
	}
	if lz < 8 {
		buf[0] = 0
		binary.LittleEndian.PutUint64(buf[1:], v)
		return 9
	}

	// extra bytes = (bits - 1) / 7 = (63 - lz) / 7
	e := ((63 - lz) * 2454267027) >> 34

	v <<= 1
	v |= 1
	v <<= uint(e)
	binary.LittleEndian.PutUint64(buf, v)

	return e + 1
}
