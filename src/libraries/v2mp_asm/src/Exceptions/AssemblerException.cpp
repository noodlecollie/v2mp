#include "Exceptions/AssemblerException.h"

namespace V2MPAsm
{
	AssemblerException::AssemblerException(
		PublicErrorID errorID,
		const std::string& file,
		size_t line,
		size_t column
	) :
		std::exception(),
		m_Exception(errorID, file, line, column)
	{
	}

	AssemblerException::AssemblerException(
		PublicWarningID warningID,
		const std::string& file,
		size_t line,
		size_t column
	) :
		std::exception(),
		m_Exception(warningID, file, line, column)
	{
	}

	PublicException& AssemblerException::GetPublicException() noexcept
	{
		return m_Exception;
	}

	const PublicException& AssemblerException::GetPublicException() const noexcept
	{
		return m_Exception;
	}

	const char* AssemblerException::what() const noexcept
	{
		return "An exception occurred during assembler processing";
	}
}
