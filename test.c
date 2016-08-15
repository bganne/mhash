#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "mhash.h"
#include "PMurHash.h"

#define TESTS	16384

static uint64_t tests[TESTS];

typedef uint32_t (*hash_fn)(uint32_t seed, const void *data, size_t len);

/*
 * see http://www.cs.cornell.edu/Courses/cs3110/2014fa/lectures/13/lec13.html
 */
static double clustering(void)
{
	double clust = 0;
	for (int i=0; i<TESTS; i++) {
		clust += (tests[i] * tests[i]) / (double)TESTS;
	}
	return (clust - 1) * TESTS / (TESTS-1);
}

static void test_clustering(hash_fn hfn, const char *name)
{
	srand(0);
	memset(tests, 0, sizeof(tests));
	for (int i=0; i<TESTS; i++) {
		uint32_t v = rand() * (UINT32_MAX/RAND_MAX);
		uint32_t h = hfn(0, &v, sizeof(v));
		tests[h%TESTS]++;
	}
	printf("%s: rand input clustering = %g\n", name,
			clustering());
	memset(tests, 0, sizeof(tests));
	for (int i=0; i<TESTS; i++) {
		uint32_t h = hfn(0, &i, sizeof(i));
		tests[h%TESTS]++;
	}
	printf("%s: linear input clustering = %g\n", name,
			clustering());


}

#define TEST_FN(fn)	do { \
	memset(tests, 0, sizeof(tests)); \
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
	} \
	test_clustering((hash_fn)fn, # fn); \
} while(0)


int main(void)
{
	TEST_FN(PMurHash32);
	TEST_FN(mhash);
	TEST_FN(platform_hash);
	return 0;
}
