#pragma once

#include <string>
#include "LibV2MPLink/Exception.h"
#include "Exceptions/PublicExceptionIDs.h"

namespace V2MPLink
{
	class PublicException
	{
	public:
		explicit PublicException(
			PublicWarningID warning,
			const std::string& file = std::string(),
			const std::string& message = std::string()
		);

		explicit PublicException(
			PublicErrorID error,
			const std::string& file = std::string(),
			const std::string& message = std::string()
		);

		V2MPLink_Exception_Type GetType() const;
		const char* GetIDString() const;

		// Format:
		// file_path: warning/error: desc [-Werror=exception-id]
		std::string ToString() const;

	private:
		static std::string ExceptionTypeString(V2MPLink_Exception_Type type);

		V2MPLink_Exception_Type m_Type = V2MPLINK_EXCEPTION_WARNING;
		std::string m_ExceptionStringID;
		std::string m_ExceptionDesc;
		std::string m_File;
		std::string m_Message;
	};
}
