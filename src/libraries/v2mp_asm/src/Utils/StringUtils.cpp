#include <string>
#include <cstring>
#include <algorithm>
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

	std::string ToLowercase(const std::string& str)
	{
		std::string out(str);
		std::transform(out.begin(), out.end(), out.begin(), [](char ch){ return static_cast<char>(::tolower(ch));});
		return out;
	}

	std::string ToUppercase(const std::string& str)
	{
		std::string out(str);
		std::transform(out.begin(), out.end(), out.begin(), [](char ch){ return static_cast<char>(::toupper(ch));});
		return out;
	}
}
