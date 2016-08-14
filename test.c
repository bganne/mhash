#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include "mhash.h"
#include "PMurHash.h"

#define TESTS	10000

static uint64_t tests[TESTS];

#define TEST_FN(fn) \
	for (int sz=1; sz<=8; sz++) { \
		struct timespec start, stop; \
		clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start); \
		for (int i=0; i<TESTS; i++) { \
			tests[i] = fn(tests[i], &tests[i], sz); \
		} \
		clock_gettime(CLOCK_THREAD_CPUTIME_ID, &stop); \
		uint64_t ns = stop.tv_sec * 1000000000 + stop.tv_nsec \
			- start.tv_sec * 1000000000 - start.tv_nsec; \
		printf("%s: %i B data: %i hash in %lu ns (%g GB/s, %g cycle/byte @2GHz)\n", \
				# fn, sz, TESTS, ns, (double)TESTS*sizeof(tests[0])/ns, \
				2.*ns/(TESTS*sizeof(tests[0]))); \
	}

int main(void)
{
	TEST_FN(platform_hash);
	TEST_FN(mhash);
	TEST_FN(PMurHash32);
	return 0;
}
