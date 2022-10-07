#include <sstream>
#include "Exceptions/PublicException.h"
#include "Exceptions/PublicExceptionIDs.h"

namespace V2MPAsm
{
	static constexpr const char* const PLACEHOLDER_FILE_NAME = "<unknown file>";

	PublicException::PublicException(PublicWarningID warning, const std::string& file, size_t line, size_t column) :
		m_Type(V2MPASM_EXCEPTION_WARNING),
		m_ExceptionStringID(GetPublicWarningStringID(warning)),
		m_ExceptionDesc(GetPublicWarningDescription(warning)),
		m_File((!file.empty()) ? file : std::string(PLACEHOLDER_FILE_NAME)),
		m_Line(line),
		m_Column(column)
	{
	}

	PublicException::PublicException(PublicErrorID error, const std::string& file, size_t line, size_t column) :
		m_Type(V2MPASM_EXCEPTION_ERROR),
		m_ExceptionStringID(GetPublicErrorStringID(error)),
		m_ExceptionDesc(GetPublicErrorDescription(error)),
		m_File((!file.empty()) ? file : std::string(PLACEHOLDER_FILE_NAME)),
		m_Line(line),
		m_Column(column)
	{
	}

	V2MPAsm_Exception_Type PublicException::GetType() const
	{
		return m_Type;
	}

	std::string PublicException::ToString() const
	{
		std::stringstream stream;

		stream
			<< m_File << ":" << m_Line << ":" << m_Column
			<< ExceptionTypeString(m_Type) << ": "
			<< m_ExceptionDesc
			<< " [-Werror=" << m_ExceptionStringID << "]";

		return stream.str();
	}

	std::string PublicException::ExceptionTypeString(V2MPAsm_Exception_Type type)
	{
		switch ( type )
		{
			case V2MPASM_EXCEPTION_WARNING:
			{
				return "warning";
			}

			case V2MPASM_EXCEPTION_ERROR:
			{
				return "error";
			}

			default:
			{
				return "unknown-exception-type";
			}
		}
	}
}
