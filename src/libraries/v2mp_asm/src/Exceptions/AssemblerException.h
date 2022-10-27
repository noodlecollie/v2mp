#pragma once

#include <exception>
#include <stdexcept>
#include "Exceptions/PublicException.h"
#include "Exceptions/PublicExceptionIDs.h"

namespace V2MPAsm
{
	class AssemblerException : public std::exception
	{
	public:
		AssemblerException(
			PublicErrorID errorID,
			const std::string& file = std::string(),
			size_t line = 1,
			size_t column = 1,
			const std::string& message = std::string()
		);

		AssemblerException(
			PublicWarningID warningID,
			const std::string& file = std::string(),
			size_t line = 1,
			size_t column = 1,
			const std::string& message = std::string()
		);

		PublicException& GetPublicException() noexcept;
		const PublicException& GetPublicException() const noexcept;

		const char* what() const noexcept override;

	private:
		PublicException m_Exception;
	};
}
