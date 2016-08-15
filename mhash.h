#ifndef MHASH_H_
#define MHASH_H_

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus */

#include <stddef.h>
#include <stdint.h>

#ifndef __SSE4_2__
#include "PMurHash.h"
#endif	/* __SSE4_2__ */

/*
 * A platform-specific hash to hash bigger data, eg. use CRC32-C HW
 * SSE4.2 support if available on x86. Other platforms can use other
 * HW support (eg. Kalray MPPA also supports HW CRC32).
 * Default to MurMur3A to maximize portability.
 */
static inline uint32_t mhash_platform(uint32_t seed, const void *data, size_t len)
{
#ifdef __SSE4_2__
	extern uint32_t mhash_crc32c_sse42(uint32_t seed, const void *data, size_t len);
	return mhash_crc32c_sse42(seed, data, len);
#else	/* __SSE4_2__ */
	return PMurHash32(seed, data, len);
#endif	/* __SSE4_2__ */
}

#define MHASH_MK128(hi_, lo_) ({ \
	union { \
		struct { uint64_t lo, hi; }; \
		__uint128_t u128; \
	} v = {{.lo=lo_, .hi=hi_}}; \
	v.u128; })

#define MHASH_GET(l) \
	case l: \
		val = data_->_ ## l; \
		break

/*
 * mhash aims to be a fast, good, general-purpose hashing function
 * It is basically a multiplicative hashing function for small values,
 * and default to a platform-specific hash for bigger values (see above).
 * It is especially efficient when hashing small, known-at-compiled-time values,
 * such as IP addresses, pointers, etc.
 * It uses Phi as a multiplicative constant, at various precision level.
 */
static inline uint32_t mhash_inline__(uint32_t seed, const void *data, size_t len)
{
	/* Used to load data efficiently even if not aligned */
	const struct mhash {
		union {
			uint8_t		_1;
			uint16_t	_2;
			uint32_t	_3:24;
			uint32_t	_4;
			uint64_t	_5:40;
			uint64_t	_6:48;
			uint64_t	_7:56;
			uint64_t	_8;
#ifdef __SIZEOF_INT128__
			__uint128_t _9:72;
			__uint128_t _10:80;
			__uint128_t _11:88;
			__uint128_t _12:96;
			__uint128_t _13:104;
			__uint128_t _14:112;
			__uint128_t _15:120;
			__uint128_t _16;
#endif	/* __SIZEOF_INT128__ */
		};
	} __attribute__((packed)) *data_ = (const struct mhash *)data;
	/*
	 * Multiplicative hashing tends to spread entropy to higher bits.
	 * Depending the size of the data, we want to use a big enough
	 * data type (and constant, and division) so the result is "good".
	 * 32-bits ops are cheaper than 64-bits ops which are cheaper than
	 * 128-bits ops (when supported), depending of the size of the data,
	 * we specialize to get the best performance vs quality ratio
	 */
	if (len <= 3) {
		uint32_t val = 0;
		switch (len) {
			MHASH_GET(1);
			MHASH_GET(2);
			MHASH_GET(3);
		}
		/* the magic used is 2^32 * Phi */
		return seed ^ ((val * 0x9E3779B9) >> 4);
	} else if (len <= 6) {
		uint64_t val;
		switch (len) {
			MHASH_GET(4);
			MHASH_GET(5);
			MHASH_GET(6);
		}
		/* the magic used is 2^64 * Phi */
		return seed ^ ((val * 0x9E3779B97F4A7C16) >> 32);
	}
#ifdef __SIZEOF_INT128__
	else if (len <= 13) {
		__uint128_t val;
		switch (len) {
			MHASH_GET(7);
			MHASH_GET(8);
			MHASH_GET(9);
			MHASH_GET(10);
			MHASH_GET(11);
			MHASH_GET(12);
			MHASH_GET(13);
		}
		/* the magic used is 2^128 * Phi */
		return seed ^ ((val * MHASH_MK128(0x9E3779B97F4A7C15, 0xF39CC0605CEDC834)) >> 96);
	}
#endif	/* __SIZEOF_INT128__ */
	return mhash_platform(seed, data, len);
}

/*
 * To avoid to clutter the resulting binary with big chunks
 * of duplicated code, inline mhash() only if we know the length at
 * compile-time. This allows the compiler to prune most of the code.
 * Otherwise just call the generic function.
 */
static inline uint32_t mhash(uint32_t seed, const void *data, size_t len)
{
	if (__builtin_constant_p(len)) return mhash_inline__(seed, data, len);
	extern uint32_t mhash_extern__(uint32_t seed, const void *data, size_t len);
	return mhash_extern__(seed, data, len);
}

#ifdef __cplusplus
}
#endif	/* __cplusplus */

#endif	/* MHASH_H_ */
