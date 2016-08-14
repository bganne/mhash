#ifndef CRC32C_SSE42_H_
#define CRC32C_SSE42_H_

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus */

#include <stddef.h>
#include <stdint.h>

static inline uint32_t crc32c_sse42(uint32_t seed, const void *data, size_t len)
{
	uint32_t crc = seed;
	for (size_t i=0; i<len/8; i++) {
		const uint64_t *u64 = data;
		crc = __builtin_ia32_crc32di(crc, u64[i]);
	}
	const uint8_t *u8 = &((uint8_t *)data)[len-7];
	switch (len % 8) {
		case 7: crc = __builtin_ia32_crc32qi(crc, u8[0]);
		case 6: crc = __builtin_ia32_crc32qi(crc, u8[1]);
		case 5: crc = __builtin_ia32_crc32qi(crc, u8[2]);
		case 4: crc = __builtin_ia32_crc32qi(crc, u8[3]);
		case 3: crc = __builtin_ia32_crc32qi(crc, u8[4]);
		case 2: crc = __builtin_ia32_crc32qi(crc, u8[5]);
		case 1: crc = __builtin_ia32_crc32qi(crc, u8[6]);
	}
	return crc;
}

#ifdef __cplusplus
}
#endif	/* __cplusplus */

#endif	/* CRC32C_SSE42_H_ */
