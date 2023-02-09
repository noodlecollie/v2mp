#pragma once

#include <string>
#include <cstring>
#include <optional>
#include <sstream>

namespace V2MPAsm
{
	size_t CopyStringToBuffer(const std::string& str, char* buffer, size_t length) noexcept;
	std::string ToLowercase(const std::string& str);
	std::string ToUppercase(const std::string& str);

	// If the value returned is negative, it means the number was negative.
	int32_t GetNumericalBase(const char numberStr, std::optional<size_t>* indexWhereValueBegins = nullptr);

	int32_t ParseInteger(const std::string& numberStr);

	// Formats number as "A (0xB)", where A is decimal representation, and B is hex representation.
	template<typename T>
	std::string DecAndHexString(const T& value)
	{
		std::stringstream stream;
		stream << value << " (0x" << std::hex << value << std::dec << ")";
		return stream.str();
	}
}
