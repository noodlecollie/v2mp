#pragma once

#include <cstddef>

namespace V2MPAsm
{
	template<typename T, size_t SIZE>
	constexpr size_t ArraySize(const T(&)[SIZE])
	{
		return SIZE;
	}
}
