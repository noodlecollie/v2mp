#include <sstream>
#include "Exceptions/PublicException.h"
#include "Exceptions/PublicExceptionIDs.h"

namespace V2MPLink
{
	static constexpr const char* const PLACEHOLDER_FILE_NAME = "<unknown file>";

	PublicException::PublicException(
		PublicWarningID warning,
		const std::string& file,
		const std::string& message
	) :
		m_Type(V2MPLINK_EXCEPTION_WARNING),
		m_ExceptionStringID(GetPublicWarningStringID(warning)),
		m_ExceptionDesc(GetPublicWarningDescription(warning)),
		m_File((!file.empty()) ? file : std::string(PLACEHOLDER_FILE_NAME)),
		m_Message(message)
	{
	}

	PublicException::PublicException(
		PublicErrorID error,
		const std::string& file,
		const std::string& message
	) :
		m_Type(V2MPLINK_EXCEPTION_ERROR),
		m_ExceptionStringID(GetPublicErrorStringID(error)),
		m_ExceptionDesc(GetPublicErrorDescription(error)),
		m_File((!file.empty()) ? file : std::string(PLACEHOLDER_FILE_NAME)),
		m_Message(message)
	{
	}

	V2MPLink_Exception_Type PublicException::GetType() const
	{
		return m_Type;
	}

	const char* PublicException::GetIDString() const
	{
		return m_ExceptionStringID.c_str();
	}

	// file_path:line:column warning/error: desc [-Werror=exception-id]
	std::string PublicException::ToString() const
	{
		std::stringstream stream;

		stream
			<< m_File << " " << ExceptionTypeString(m_Type) << ": ";

		if ( !m_Message.empty() )
		{
			stream << m_Message;
		}
		else
		{
			stream << m_ExceptionDesc;
		}

		stream
			<< " [-Werror=" << m_ExceptionStringID << "]";

		return stream.str();
	}

	std::string PublicException::ExceptionTypeString(V2MPLink_Exception_Type type)
	{
		switch ( type )
		{
			case V2MPLINK_EXCEPTION_WARNING:
			{
				return "warning";
			}

			case V2MPLINK_EXCEPTION_ERROR:
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
