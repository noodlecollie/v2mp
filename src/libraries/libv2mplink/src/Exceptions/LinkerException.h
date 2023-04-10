#pragma once

#include <exception>
#include <stdexcept>
#include "Exceptions/PublicException.h"
#include "Exceptions/PublicExceptionIDs.h"

namespace V2MPLink
{
	class LinkerException : public std::exception
	{
	public:
		LinkerException(
			PublicErrorID errorID,
			const std::string& file = std::string(),
			const std::string& message = std::string()
		);

		LinkerException(
			PublicWarningID warningID,
			const std::string& file = std::string(),
			const std::string& message = std::string()
		);

		PublicException& GetPublicException() noexcept;
		const PublicException& GetPublicException() const noexcept;

		const char* what() const noexcept override;

	private:
		PublicException m_Exception;
	};
}
