#pragma once

#include <string>
#include "V2MPAsm/Exception.h"
#include "Exceptions/PublicExceptionIDs.h"
#include "Utils/ParsingUtils.h"

namespace V2MPAsm
{
	class PublicException
	{
	public:
		explicit PublicException(
			PublicWarningID warning,
			const std::string& file = std::string(),
			size_t line = LINE_NUMBER_BASE,
			size_t column = COLUMN_NUMBER_BASE,
			const std::string& message = std::string()
		);

		explicit PublicException(
			PublicErrorID error,
			const std::string& file = std::string(),
			size_t line = LINE_NUMBER_BASE,
			size_t column = COLUMN_NUMBER_BASE,
			const std::string& message = std::string()
		);

		V2MPAsm_Exception_Type GetType() const;
		const char* GetIDString() const;

		// Format:
		// file_path:line:column warning/error: desc [-Werror=exception-id]
		std::string ToString() const;

	private:
		static std::string ExceptionTypeString(V2MPAsm_Exception_Type type);

		V2MPAsm_Exception_Type m_Type = V2MPASM_EXCEPTION_WARNING;
		std::string m_ExceptionStringID;
		std::string m_ExceptionDesc;
		std::string m_File;
		size_t m_Line = LINE_NUMBER_BASE;
		size_t m_Column = COLUMN_NUMBER_BASE;
		std::string m_Message;
	};
}
