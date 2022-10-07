#include <string>
#include <cstring>
#include "Utils/StringUtils.h"

namespace V2MPAsm
{
	size_t CopyStringToBuffer(const std::string& str, char* buffer, size_t length) noexcept
	{
		const size_t strLength = str.length();

		if ( buffer && length > 0 )
		{
			const size_t numChars = std::min<size_t>(length - 1, strLength);

			if ( numChars > 0 )
			{
				std::memcpy(buffer, str.data(), numChars);
			}

			buffer[numChars] = '\0';
		}

		return strLength;
	}
}
