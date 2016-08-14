#ifndef MHASH_H_
#define MHASH_H_

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus */

#include <stddef.h>
#include <stdint.h>

#ifdef __SSE4_2__
#include "crc32c_sse42.h"
#else	/* __SSE4_2__ */
#include "PMurHash.h"
#endif	/* __SSE4_2__ */

static inline uint32_t platform_hash(uint32_t seed, const void *data, size_t len)
{
#ifdef __SSE4_2__
	return crc32c_sse42(seed, data, len);
#else	/* __SSE4_2__ */
	return PMurHash32(seed, data, len);
#endif	/* __SSE4_2__ */
}

static inline uint32_t mhash(uint32_t seed, const void *data, size_t len)
{
	const struct mhash_ {
		union {
			uint8_t  _1;
			uint16_t _2;
			uint32_t _3:24;
			uint32_t _4;
			uint64_t _5:40;
			uint64_t _6:48;
			uint64_t _7:56;
			uint64_t _8;
		};
	} __attribute__((packed)) *data_ = (const struct mhash_ *)data;
	uint64_t val;
	switch (len) {
#define MHASH_GET(l) \
	case l: \
		val = data_->_ ## l; \
		break
		MHASH_GET(1);
		MHASH_GET(2);
		MHASH_GET(3);
		MHASH_GET(4);
		MHASH_GET(5);
		MHASH_GET(6);
		MHASH_GET(7);
		MHASH_GET(8);
		default: return platform_hash(seed, data, len);
	}

	return seed ^ ((val * 7319936632422683419ULL) >> 32);
}

#ifdef __cplusplus
}
#endif	/* __cplusplus */

#endif	/* MHASH_H_ */
