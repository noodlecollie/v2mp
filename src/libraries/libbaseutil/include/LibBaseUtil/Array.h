#ifndef BASEUTIL_ARRAY_H
#define BASEUTIL_ARRAY_H

#include <stddef.h>

#ifdef __cplusplus
namespace BaseUtil
{
	template<typename T, size_t SIZE>
	static constexpr size_t ArraySize(const T(&)[SIZE])
	{
		return SIZE;
	}
}
#endif // __cplusplus

#endif // BASEUTIL_ARRAY_H
