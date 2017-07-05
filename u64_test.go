// Package flit implements the FLIT algorithm.
// This is free and unencumbered software released into the public domain.
package flit

import (
	"encoding/hex"
	"testing"
)

var tests = []struct {
	hex string
	val uint64
}{
	{"01", 0},
	{"ff", 127},
	{"0202", 128},
	{"feff", 16383},
	{"040002", 16384},
	{"fcffff", 2097151},
	{"08000002", 2097152},
	{"f8ffffff", 268435455},
	{"1000000002", 268435456},
	{"f0ffffffff", 34359738367},
	{"200000000002", 34359738368},
	{"e0ffffffffff", 4398046511103},
	{"40000000000002", 4398046511104},
	{"c0ffffffffffff", 562949953421311},
	{"8000000000000002", 562949953421312},
	{"80ffffffffffffff", 72057594037927935},
	{"000000000000000001", 72057594037927936},
	{"00ffffffffffffffff", 18446744073709551615},
}

func TestPutUint64(t *testing.T) {
	for _, gold := range tests {
		buf := [...]byte{1, 2, 3, 4, 5, 6, 7, 8, 9}
		n := PutUint64(buf[:], gold.val)
		got := hex.EncodeToString(buf[:n])
		if got != gold.hex {
			t.Errorf("%d: got 0x%s, want 0x%s", gold.val, got, gold.hex)
			continue
		}
	}
}

func TestUint64(t *testing.T) {
	for _, gold := range tests {
		buf := [...]byte{1, 2, 3, 4, 5, 6, 7, 8, 9}
		n, err := hex.Decode(buf[:], []byte(gold.hex))
		if err != nil {
			t.Fatal(err)
		}

		got, read := Uint64(buf[:])
		if got != gold.val || read != n {
			t.Errorf("got %d, want %d; read %d bytes, want %d", got, gold.val, read, n)
		}
	}
}

// benchmark data
var serials [18][]byte
var values [18]uint64

func init() {
	for i, gold := range tests {
		buf := make([]byte, 9)
		if _, err := hex.Decode(buf, []byte(gold.hex)); err != nil {
			panic(err)
		}
		serials[i] = buf
		values[i] = gold.val
	}
}

// prevents compiler optimization
var ckx uint64
var ckn int

func BenchmarkPutUint64(b *testing.B) {
	b.SetBytes(8)
	buf := make([]byte, 9)

	for i := 0; i < b.N; i++ {
		ckn += PutUint64(buf, values[i%len(values)])
	}
}

func BenchmarkUint64(b *testing.B) {
	b.SetBytes(8)

	for i := 0; i < b.N; i++ {
		x, n := Uint64(serials[i%len(serials)])
		ckx += x
		ckn += n
	}
}
