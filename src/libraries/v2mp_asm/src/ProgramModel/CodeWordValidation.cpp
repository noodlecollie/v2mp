#include "ProgramModel/CodeWordValidation.h"
#include "Exceptions/PublicExceptionIDs.h"
#include "ProgramModel/CodeWord.h"
#include "ProgramModel/InstructionMeta.h"

namespace V2MPAsm
{
	ValidationFailure::ValidationFailure(
		PublicWarningID warning,
		size_t argIndex,
		const std::string& message
	) :
		m_ID(warning),
		m_ArgIndex(argIndex),
		m_Message(message)
	{
	}

	ValidationFailure::ValidationFailure(
		PublicErrorID error,
		size_t argIndex,
		const std::string& message
	) :
		m_ID(error),
		m_ArgIndex(argIndex),
		m_Message(message)
	{
	}

	bool ValidationFailure::IsWarning() const
	{
		return std::holds_alternative<PublicWarningID>(m_ID);
	}

	bool ValidationFailure::IsError() const
	{
		return std::holds_alternative<PublicErrorID>(m_ID);
	}

	PublicWarningID ValidationFailure::GetWarningID() const
	{
		return IsWarning() ? std::get<PublicWarningID>(m_ID) : PublicWarningID::INTERNAL;
	}

	PublicErrorID ValidationFailure::GetErrorID() const
	{
		return IsError() ? std::get<PublicErrorID>(m_ID) : PublicErrorID::INTERNAL;
	}

	size_t ValidationFailure::GetArgIndex() const
	{
		return m_ArgIndex;
	}

	const std::string& ValidationFailure::GetMessage() const
	{
		return m_Message;
	}

	static ValidationFailure TooManyArgumentsFailure(size_t expected, size_t actual, size_t argIndex = 0)
	{
		return ValidationFailure(
					PublicWarningID::TOO_MANY_ARGUMENTS,
					argIndex,
					"Expected " + std::to_string(expected) +
					" arguments to instruction but got " + std::to_string(actual) + " instructions"
		);
	}

	static std::vector<ValidationFailure> ValidateZeroArgCodeWord(const CodeWord& codeWord)
	{
		if ( codeWord.GetArgumentCount() > 0 )
		{
			return { TooManyArgumentsFailure(0, codeWord.GetArgumentCount()) };
		}

		return {};
	}

	std::vector<ValidationFailure> ValidateCodeWord(const CodeWord& codeWord)
	{
		switch ( codeWord.GetInstructionType() )
		{
			case InstructionType::NOP:
			case InstructionType::SIG:
			{
				return ValidateZeroArgCodeWord(codeWord);
			}

			default:
			{
				return {
					ValidationFailure(
						PublicErrorID::INTERNAL,
						0,
						"Unexpected instruction type encountered during code word validation."
					)
				};
			}
		}
	}
}
