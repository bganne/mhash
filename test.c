#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "mhash.h"
#include "PMurHash.h"

#define TESTS	16384

static uint64_t tests[TESTS];
static char scratch[128];

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

__attribute__((always_inline))
static inline void test_clustering(hash_fn hfn, const char *name)
{
	srand(0);
	memset(tests, 0, sizeof(tests));
	for (int i=0; i<TESTS; i++) {
		uint64_t v = rand() * (UINT64_MAX/RAND_MAX);
		uint32_t h = hfn(0, &v, sizeof(v));
		tests[h%TESTS]++;
	}
	printf("%s: rand input clustering = %g\n", name,
			clustering());
	memset(tests, 0, sizeof(tests));
	for (int i=0; i<TESTS; i++) {
		__uint128_t v = i;
		v <<= 2*8-14;
		uint32_t h = hfn(0, &v, 6);
		tests[h%TESTS]++;
	}
	printf("%s: linear input clustering = %g\n", name,
			clustering());
}

static void stop(const char *name, int sz, const struct timespec *start)
{
	struct timespec stop;
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &stop);
	uint64_t ns = stop.tv_sec * 1000000000 + stop.tv_nsec
		- start->tv_sec * 1000000000 - start->tv_nsec;
	printf("%s: %i B data: %i hash in %lu ns (%g GB/s, %g cycle/byte @2GHz)\n",
			name, sz, TESTS, ns, (double)TESTS*sizeof(tests[0])/ns,
			2.*ns/(TESTS*sizeof(tests[0])));
}

__attribute__((always_inline))
static inline void test_sz(hash_fn hfn, const char *name, int sz)
{
	struct timespec start;
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
	for (int i=0; i<TESTS; i++) {
		tests[i] = hfn(tests[i], scratch, sz);
	}
	stop(name, sz, &start);
}

#define TEST_FN(fn)	do { \
	memset(tests, 0, sizeof(tests)); \
	test_sz((hash_fn)fn, # fn, 1); \
	test_sz((hash_fn)fn, # fn, 2); \
	test_sz((hash_fn)fn, # fn, 3); \
	test_sz((hash_fn)fn, # fn, 4); \
	test_sz((hash_fn)fn, # fn, 5); \
	test_sz((hash_fn)fn, # fn, 6); \
	test_sz((hash_fn)fn, # fn, 7); \
	test_sz((hash_fn)fn, # fn, 8); \
	test_sz((hash_fn)fn, # fn, 9); \
	test_sz((hash_fn)fn, # fn, 10); \
	test_sz((hash_fn)fn, # fn, 11); \
	test_sz((hash_fn)fn, # fn, 12); \
	test_sz((hash_fn)fn, # fn, 13); \
	test_sz((hash_fn)fn, # fn, 14); \
	test_sz((hash_fn)fn, # fn, 15); \
	test_sz((hash_fn)fn, # fn, 16); \
	test_sz((hash_fn)fn, # fn, 17); \
	test_clustering((hash_fn)fn, # fn); \
} while(0)


int main(void)
{
	TEST_FN(PMurHash32);
	TEST_FN(mhash);
	extern uint32_t mhash_extern__(uint32_t seed, const void *data, size_t len);
	TEST_FN(mhash_extern__);
	TEST_FN(mhash_platform);
	return 0;
}

#define MHASH_DECL(sz) \
uint32_t mhash ## sz(uint32_t seed, const void *data) \
{ \
	return mhash(seed, data, sz); \
}

MHASH_DECL(1);
MHASH_DECL(2);
MHASH_DECL(3);
MHASH_DECL(4);
MHASH_DECL(5);
MHASH_DECL(6);
MHASH_DECL(7);
MHASH_DECL(8);
MHASH_DECL(9);
MHASH_DECL(10);
MHASH_DECL(11);
MHASH_DECL(12);
MHASH_DECL(13);
MHASH_DECL(14);
MHASH_DECL(15);
MHASH_DECL(16);
