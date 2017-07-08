#include "flit.h"

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>

int main(int argc, char* argv[]) {
	uint8_t buf[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
	struct timespec start, end;

	clock_gettime(CLOCK_MONOTONIC, &start);

	uint64_t n = 1000000000;
	n += start.tv_nsec % 1001; // prevents compiler optimization

	for (uint64_t i = 0; i < n; ++i) {
		int enced = flit64enc(buf, i);
		uint64_t j;
		int deced = flit64dec(&j, buf);
		if (i != j) printf("got %" PRIu64 ", want %" PRIu64 "\n", j, i);
		int diff = deced - enced;
		if (diff) printf("size diff %d for %" PRIu64 "\n", diff, i);
	}

	clock_gettime(CLOCK_MONOTONIC, &end);

	time_t a = start.tv_sec * 1000000000 + start.tv_nsec;
	time_t b = end.tv_sec * 1000000000 + end.tv_nsec;
	printf("took %.2fns for %" PRIu64 "M\n", ((double) b - (double) a) / (double) n, n / 1000000);
}
