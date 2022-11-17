#pragma once

#include <string>
#include <cstring>
#include <optional>

namespace V2MPAsm
{
	size_t CopyStringToBuffer(const std::string& str, char* buffer, size_t length) noexcept;
	std::string ToLowercase(const std::string& str);
	std::string ToUppercase(const std::string& str);
	int32_t GetNumericalBase(const char numberStr, std::optional<size_t>* indexWhereValueBegins = nullptr);
	int32_t ParseInteger(const std::string& numberStr);
}
