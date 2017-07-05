# Specification

## Abstract

Fixed-Length Integer Trim (FLIT) is a variable-length quantity algorithm.
FLIT64 outperforms varint encoding in terms of speed and size.

* Fixed data size range by design
* Size known at fist byte (minimizes memory access)
* CPU friendly (bit shift, count zeros)



## FLIT64

The 64-bit unsigned version encodes an integer in 1 to 9 octets.

The first octet advertises the number of octets following with the trailing
zero count. Any remaining bits hold the least significant data bits and the
following octets, if any at all, hold the rest in little-endian order.

| Size      | First Bits       | Range                                                       |
|:----------|:-----------------|:------------------------------------------------------------|
| 1 octet   | x x x x x x x 1  |  7-bit (0, 127)                                             |
| 2 octets  | x x x x x x 1 0  | 14-bit (128, 16'383)                                        |
| 3 octets  | x x x x x 1 0 0  | 23-bit (16'384, 2'097'151)                                  |
| 4 octets  | x x x x 1 0 0 0  | 28-bit (2'097'152, 268'435'455)                             |
| 5 octets  | x x x 1 0 0 0 0  | 35-bit (268'435'456, 34'359'738'367)                        |
| 6 octets  | x x 1 0 0 0 0 0  | 42-bit (34'359'738'368, 4'398'046'511'103)                  |
| 7 octets  | x 1 0 0 0 0 0 0  | 49-bit (4'398'046'511'104, 562'949'953'421'311)             |
| 8 octets  | 1 0 0 0 0 0 0 0  | 56-bit (562'949'953'421'312, 72'057'594'037'927'935)        |
| 9 octets  | 0 0 0 0 0 0 0 0  | 64-bit (72'057'594'037'927'936, 18'446'744'073'709'551'615) |

Encoding *should* pick the smallest range capable to hold the value.


### Decoding Example

1. The first byte read is `1 0 1 0 0 1 1 0`.
2. One tailing zero means we need to read one more byte, which is `0 0 0 0 1 1 1 1`.
3. Put the bytes in little-endian order to get `0 0 0 0 1 1 1 1  1 0 1 0 0 1 1 0`.
4. Right shift 2 to drop the size bits makes `0 0 0 0 0 0 1 1  1 1 1 0 1 0 0 1`.

... which is decimal value 1001.



## Benchmark

A full FLIT64 encode + decode cycle in C takes less than 8ns on an "Intel i5-2520M" mobile CPU from quarter 1 of year 2011.

The Go
[![(GoDoc)](https://godoc.org/github.com/pascaldekloe/flit?status.svg)](https://godoc.org/github.com/pascaldekloe/flit)
imlementation needs a bit more because of the safety checks.

```
BenchmarkPutUint64-4   	200000000	         6.28 ns/op	1273.75 MB/s
BenchmarkUint64-4      	200000000	         7.64 ns/op	1047.53 MB/s
```
