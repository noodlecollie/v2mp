#pragma once

#include <vector>
#include <variant>
#include <string>
#include "Exceptions/PublicExceptionIDs.h"
#include "Exceptions/AssemblerException.h"

namespace V2MPAsm
{
	class CodeWord;
	class ProgramModel;

	class ValidationFailure
	{
	public:
		ValidationFailure(
			PublicWarningID warning,
			size_t argIndex,
			const std::string& message
		);

		ValidationFailure(
			PublicErrorID error,
			size_t argIndex,
			const std::string& message
		);

		bool IsWarning() const;
		bool IsError() const;

		PublicWarningID GetWarningID() const;
		PublicErrorID GetErrorID() const;

		size_t GetArgIndex() const;
		const std::string& GetMessage() const;

	private:
		std::variant<PublicWarningID, PublicErrorID> m_ID;
		size_t m_ArgIndex = 0;
		std::string m_Message;
	};

	std::vector<ValidationFailure> ValidateCodeWord(CodeWord& codeWord, bool validateLabelRefs = false);

	AssemblerException ToAssemblerException(
		const ValidationFailure& failure,
		const std::string& filePath,
		const CodeWord& codeWord
	);
}
