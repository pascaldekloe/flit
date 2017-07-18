# Specification

## Abstract

Fixed-Length Integer Trim (FLIT) is a byte oriented
[universal code](https://en.wikipedia.org/wiki/Universal_code_%28data_compression%29).
FLIT64 outperforms VLQ, a.k.a. varint, in terms of speed and size.

* Fixed serial size range by design
* Size known at fist byte
* CPU friendly: count zeros & bit shift, no loops



## FLIT64

The 64-bit unsigned version encodes an integer in 1 to 9 octets.

The first octet advertises the number of octets following with the trailing
zero count. Any remaining bits hold the least significant data bits and the
following octets, if any at all, hold the rest in little-endian order.

| Total Size | First Bits | Range                                                       |
|:-----------|:-----------|:------------------------------------------------------------|
| 1 octet    | xxxx xxx1  |  7-bit (0, 127)                                             |
| 2 octets   | xxxx xx10  | 14-bit (128, 16'383)                                        |
| 3 octets   | xxxx x100  | 21-bit (16'384, 2'097'151)                                  |
| 4 octets   | xxxx 1000  | 28-bit (2'097'152, 268'435'455)                             |
| 5 octets   | xxx1 0000  | 35-bit (268'435'456, 34'359'738'367)                        |
| 6 octets   | xx10 0000  | 42-bit (34'359'738'368, 4'398'046'511'103)                  |
| 7 octets   | x100 0000  | 49-bit (4'398'046'511'104, 562'949'953'421'311)             |
| 8 octets   | 1000 0000  | 56-bit (562'949'953'421'312, 72'057'594'037'927'935)        |
| 9 octets   | 0000 0000  | 64-bit (72'057'594'037'927'936, 18'446'744'073'709'551'615) |

Encoding *should* pick the smallest range capable to hold the value.


### Decoding Example

1. The first byte read is `1010 0110`.
2. One tailing zero means we need to read one more byte, which is `0000 1111`.
3. The two bytes in little-endian order make `0000 1111 1010 0110`.
4. Drop the two size bits with a bit shift to get `0000 0011 1110 1001` or decimal 1001.



## Benchmark

A full FLIT64 encode + decode cycle in C takes 3.88ns on a Mac Pro (Late 2013).

The implementation in Go
[![(GoDoc)](https://godoc.org/github.com/pascaldekloe/flit?status.svg)](https://godoc.org/github.com/pascaldekloe/flit)
needs a bit more time because of the safety checks. The "raw" benchmarks are as fast
as its gets with fixed-width encoding.


```
BenchmarkPutUint64-12       	300000000	         4.44 ns/op	1800.63 MB/s
BenchmarkPutUint64Raw-12    	2000000000	         1.96 ns/op	4089.20 MB/s
BenchmarkPutUint64VQL-12    	200000000	         7.61 ns/op	1050.87 MB/s
BenchmarkUint64-12          	300000000	         4.09 ns/op	1954.54 MB/s
BenchmarkUint64Raw-12       	1000000000	         2.08 ns/op	3853.11 MB/s
BenchmarkUint64VQL-12       	100000000	        11.1 ns/op	 722.76 MB/s
```
