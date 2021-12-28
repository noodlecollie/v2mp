#ifndef V2MP_UTIL_ENDIANNESS_H
#define V2MP_UTIL_ENDIANNESS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef V2MP_HOST_IS_BIG_ENDIAN
	#define V2MP_LE16(val16Bit) \
		( \
			(((val16Bit) & 0xF0) >> 8) | \
			(((val16Bit) & 0x0F) << 8) \
		)

	#define V2MP_LE32(val32Bit) \
		( \
			((val32Bit) & 0xF000) >> 24) | \
			((val32Bit) & 0x0F00) >> 8 ) | \
			((val32Bit) & 0x00F0) << 8 ) | \
			((val32Bit) & 0x000F) << 24) \
		)
#else
	#define V2MP_LE16(val16Bit) (val16Bit)
	#define V2MP_LE32(val32Bit) (val32Bit)
#endif // V2MP_HOST_IS_BIG_ENDIAN

#ifdef __cplusplus
} // extern "C"
#endif

#endif // V2MP_UTIL_ENDIANNESS_H
