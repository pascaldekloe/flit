#include "flit.h"

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

const uint64_t tests[] = {0, 127, 128, 16383, 16384, 2097151, 2097152, 268435455, 268435456, 34359738367, 34359738368, 4398046511103, 4398046511104, 562949953421311, 562949953421312, 72057594037927935, 72057594037927936, 18446744073709551615ull};

int main(int argc, char* argv[]) {
	for (int i = 0; i < sizeof(tests) / 8; ++i) {
		uint8_t buf[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
		uint64_t val = tests[i], got = 0;

		int enced = flit64_enc(buf, val);
		int deced = flit64_dec(&got, buf);

		if (got != val || enced != deced)
			printf("got %" PRIu64 ", want %" PRIu64
				"; encoded %d octets and decoded %d octets"
				"; buffer: %02x%02x %02x%02x %02x%02x %02x%02x %02x\n",
				got, val, enced, deced, 
				buf[0], buf[1], buf[2], buf[3],
				buf[4], buf[5], buf[6], buf[7],
				buf[8]);
	}
}
