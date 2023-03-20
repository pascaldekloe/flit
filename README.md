# Specification

## Abstract

Fixed-Length Integer Trim (FLIT) is a byte oriented
[universal code](https://en.wikipedia.org/wiki/Universal_code_%28data_compression%29).
FLIT64 outperforms VLQ, a.k.a. varint, in terms of speed and size.

* Fixed serial size range by design
* Size known at fist byte
* CPU friendly: count zeros & bit shift, no loops

This is free and unencumbered software released into the
[public domain](http://creativecommons.org/publicdomain/zero/1.0).

[![C](https://github.com/pascaldekloe/flit/actions/workflows/c.yml/badge.svg)](https://github.com/pascaldekloe/flit/actions/workflows/c.yml)
[![Go](https://github.com/pascaldekloe/flit/actions/workflows/go.yml/badge.svg)](https://github.com/pascaldekloe/flit/actions/workflows/go.yml)


## FLIT64

The 64-bit unsigned version encodes an integer in 1 to 9 octets.

The first octet advertises the number of octets following with the trailing
zero count. Any remaining bits hold the least significant data bits and the
following octets, if any at all, hold the rest in little-endian order.

| Total Size | First Bits  | Range                               |
|:-----------|:------------|:------------------------------------|
| 1 octet    | `xxxx xxx1` | 7-bit (128)                         |
| 2 octets   | `xxxx xx10` | 14-bit (16'384)                     |
| 3 octets   | `xxxx x100` | 21-bit (2'097'152)                  |
| 4 octets   | `xxxx 1000` | 28-bit (268'435'456)                |
| 5 octets   | `xxx1 0000` | 35-bit (34'359'738'368)             |
| 6 octets   | `xx10 0000` | 42-bit (4'398'046'511'104)          |
| 7 octets   | `x100 0000` | 49-bit (562'949'953'421'312)        |
| 8 octets   | `1000 0000` | 56-bit (72'057'594'037'927'936)     |
| 9 octets   | `0000 0000` | 64-bit (18'446'744'073'709'551'616) |

Encoding *should* pick the smallest range capable to hold the value.


### Decoding Example

1. The first byte read is `1010 0110`.
2. One tailing zero means we need to read one more byte, which is `0000 1111`.
3. The two bytes in little-endian order make `0000 1111 1010 0110`.
4. Drop the two size bits with a bit shift to get `0000 0011 1110 1001` or decimal 1001.


## FLIT64S

For signed integers, the algorithm is preceded with *ZigZag encoding*. See the
[Protocol Buffers Encoding](https://developers.google.com/protocol-buffers/docs/encoding#signed-integers)
documentation for detailed information.

## Benchmark

Encoding and decoding in C is faster than memcpy on an Apple M1.

```
--------------------------------------------------------------------------
Benchmark                Time             CPU   Iterations UserCounters...
--------------------------------------------------------------------------
BM_flit64_enc/1      0.547 ns        0.547 ns   1000000000 bytes_per_second=13.6251G/s items_per_second=1.82873G/s
BM_flit64_enc/5      0.860 ns        0.860 ns    811782442 bytes_per_second=8.66524G/s items_per_second=1.16303G/s
BM_flit64_enc/9      0.625 ns        0.625 ns   1000000000 bytes_per_second=11.9131G/s items_per_second=1.59894G/s
BM_flit64_dec/1      0.742 ns        0.742 ns    939862243 bytes_per_second=10.0367G/s items_per_second=1.34711G/s
BM_flit64_dec/5      0.742 ns        0.742 ns    936479906 bytes_per_second=10.0357G/s items_per_second=1.34697G/s
BM_flit64_dec/9      0.705 ns        0.705 ns    990701558 bytes_per_second=10.5737G/s items_per_second=1.41918G/s
BM_memcpy64           2.04 ns         2.04 ns    343174264 bytes_per_second=3.65752G/s items_per_second=490.905M/s
```

The speed is similar to native endian encoding in Go on Apple M1. Dito for Intel Xeon.

```
name            time/op
PutUint64-8       2.08ns ± 0%
PutUint64Raw-8    2.08ns ± 0%
PutUint64VQL-8    3.77ns ± 0%
Uint64-8          2.80ns ± 1%
Uint64Raw-8       2.10ns ± 1%
Uint64VQL-8       5.73ns ± 1%

name            speed
PutUint64-8     3.85GB/s ± 0%
PutUint64Raw-8  3.84GB/s ± 0%
PutUint64VQL-8  2.12GB/s ± 0%
Uint64-8        2.86GB/s ± 1%
Uint64Raw-8     3.80GB/s ± 1%
Uint64VQL-8     1.40GB/s ± 1%
```


## Credits

* Aleksey Demakov @ademakov
* Roman Gershman @romange
