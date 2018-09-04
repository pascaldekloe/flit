// Package flit implements the FLIT64 algorithm.
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

// Int64 decodes an integer from buf and returns that value and the serial size.
// If the is buffer smaller than 9 bytes, Int64 may panic.
func Int64(buf []byte) (v int64, n int) {
	u, n := Uint64(buf)
	return int64(u>>1) ^ -int64(u&1), n
}

// PutUint64 encodes an integer into buf and returns the serial size.
// If the buffer is smaller than 9 bytes, PutUint64 may panic.
func PutUint64(buf []byte, v uint64) (n int) {
	if v >= uint64(1)<<56 {
		buf[0] = 0
		binary.LittleEndian.PutUint64(buf[1:], v)
		return 9
	}

	bitCount := bits.Len64(v)
	e := (bitCount + (bitCount >> 3)) >> 3

	v = v<<1 | 1
	v <<= uint(e)
	binary.LittleEndian.PutUint64(buf, v)

	return e + 1
}

// PutInt64 encodes an integer into buf and returns the serial size.
// If the buffer is smaller than 9 bytes, PutInt64 may panic.
func PutInt64(buf []byte, v int64) (n int) {
	return PutUint64(buf, uint64(v<<1)^uint64(v>>63))
}
