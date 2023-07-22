#include "Exceptions/LinkerException.h"

namespace V2MPLink
{
	LinkerException::LinkerException(
		PublicErrorID errorID,
		const std::string& file,
		const std::string& message
	) :
		std::exception(),
		m_Exception(errorID, file, message)
	{
	}

	LinkerException::LinkerException(
		PublicWarningID warningID,
		const std::string& file,
		const std::string& message
	) :
		std::exception(),
		m_Exception(warningID, file, message)
	{
	}

	PublicException& LinkerException::GetPublicException() noexcept
	{
		return m_Exception;
	}

	const PublicException& LinkerException::GetPublicException() const noexcept
	{
		return m_Exception;
	}

	const char* LinkerException::what() const noexcept
	{
		return "An exception occurred during linker processing";
	}
}
