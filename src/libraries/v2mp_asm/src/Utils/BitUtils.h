#pragma once

#include <cstdint>
#include <cstddef>

namespace V2MPAsm
{
	static constexpr int32_t MaxUnsignedValue(size_t numBits)
	{
		// (2 ^ n) - 1;
		return (1 << numBits) - 1;
	}

	static constexpr int32_t MinSignedValue(size_t numBits)
	{
		// -(2 ^ (n-1))
		return numBits > 1 ? (static_cast<int32_t>(1 << (numBits - 1)) * -1) : -1;
	}

	static constexpr int32_t MaxSignedValue(size_t numBits)
	{
		// (2 ^ (n-1)) - 1
		return numBits > 1 ? (1 << (numBits - 1)) : 0;
	}

	static constexpr uint32_t BitMask(size_t numBits)
	{
		return static_cast<uint32_t>(~0) >> ((sizeof(uint32_t) * 8) - numBits);
	}
}
