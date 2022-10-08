#pragma once

#include <string>
#include "V2MPAsm/Exception.h"
#include "Exceptions/PublicExceptionIDs.h"

namespace V2MPAsm
{
	class PublicException
	{
	public:
		explicit PublicException(
			PublicWarningID warning,
			const std::string& file = std::string(),
			size_t line = 1,
			size_t column = 0,
			const std::string& message = std::string()
		);

		explicit PublicException(
			PublicErrorID error,
			const std::string& file = std::string(),
			size_t line = 1,
			size_t column = 0,
			const std::string& message = std::string()
		);

		V2MPAsm_Exception_Type GetType() const;

		// Format:
		// file_path:line:column warning/error: desc [-Werror=exception-id]
		std::string ToString() const;

	private:
		static std::string ExceptionTypeString(V2MPAsm_Exception_Type type);

		V2MPAsm_Exception_Type m_Type = V2MPASM_EXCEPTION_WARNING;
		std::string m_ExceptionStringID;
		std::string m_ExceptionDesc;
		std::string m_File;
		size_t m_Line = 1;
		size_t m_Column = 0;
		std::string m_Message;
	};
}
