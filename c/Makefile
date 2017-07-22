.PHONY: all
all: test bench

.PHONY: test
test: clean test.bin
	./test.bin

.PHONY: bench
bench: clean bench.bin
	./bench.bin

test.bin:
	$(CC) -o test.bin -O0 test.c

bench.bin:
	$(CC) -o bench.bin -O2 -lstdc++ -lbenchmark bench.cpp

.PHONY: clean
clean:
	rm -fr test.bin bench.bin
