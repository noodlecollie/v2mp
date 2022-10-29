#pragma once

#include <string>
#include <cstring>

namespace V2MPAsm
{
	size_t CopyStringToBuffer(const std::string& str, char* buffer, size_t length) noexcept;
	std::string ToLowercase(const std::string& str);
	std::string ToUppercase(const std::string& str);
}
