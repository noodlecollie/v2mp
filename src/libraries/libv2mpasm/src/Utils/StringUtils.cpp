#include <limits>
#include <optional>
#include <stdexcept>
#include <string>
#include <cstring>
#include <algorithm>
#include <cctype>
#include "Utils/StringUtils.h"

namespace V2MPAsm
{
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

	int32_t GetNumericalBase(const std::string& numberStr, std::optional<size_t>* indexWhereValueBegins)
	{
		int32_t base = 0;
		int32_t sign = 1;
		size_t firstDigitIndex = 0;
		size_t internalIndexWhereValueBegins = 0;

		if ( !numberStr.empty() )
		{
			if ( numberStr[0] == '+' )
			{
				firstDigitIndex = 1;
			}
			else if ( numberStr[0] == '-' )
			{
				firstDigitIndex = 1;
				sign = -1;
			}

			if ( numberStr.size() > firstDigitIndex )
			{
				const char firstDigit = numberStr[firstDigitIndex];

				if ( firstDigit == '0' )
				{
					if ( numberStr.size() > firstDigitIndex + 1 )
					{
						const char baseChar = numberStr[firstDigitIndex + 1];

						if ( baseChar == 'X' || baseChar == 'x' )
						{
							base = 16;
							internalIndexWhereValueBegins = firstDigitIndex + 2;
						}
						else if ( baseChar == 'B' || baseChar == 'b' )
						{
							base = 2;
							internalIndexWhereValueBegins = firstDigitIndex + 2;
						}
						else if ( std::isdigit(baseChar) )
						{
							base = 10;
							internalIndexWhereValueBegins = firstDigitIndex;
						}
					}
					else
					{
						base = 10;
						internalIndexWhereValueBegins = firstDigitIndex;
					}
				}
				else if ( std::isdigit(firstDigit) )
				{
					base = 10;
					internalIndexWhereValueBegins = firstDigitIndex;
				}
			}
		}

		if ( base > 0 && indexWhereValueBegins )
		{
			*indexWhereValueBegins = internalIndexWhereValueBegins;
		}

		return sign * base;
	}

	int32_t ParseInteger(const std::string& numberStr)
	{
		std::optional<size_t> valueBegins;
		int32_t base = GetNumericalBase(numberStr, &valueBegins);
		int32_t sign = 1;

		if ( base < 0 )
		{
			sign = -1;
			base *= -1;
		}

		if ( base == 0 )
		{
			throw std::invalid_argument("Could not determine numerical base of number");
		}

		if ( !valueBegins.has_value() )
		{
			throw std::invalid_argument("Could not determine beginning of number value");
		}

		return static_cast<int32_t>(std::stol(numberStr.substr(valueBegins.value()), nullptr, base)) * sign;
	}
}
