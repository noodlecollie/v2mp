#pragma once

#include <memory>
#include <vector>
#include "ProgramModel/ValidationFailure.h"

namespace V2MPAsm
{
	class CodeWord;

	class BaseCodeWordValidator
	{
	public:
		virtual ~BaseCodeWordValidator();

		CodeWord& GetCodeWord();
		const CodeWord& GetCodeWord() const;

		bool GetValidateLabelRefs() const;
		void SetValidateLabelRefs(bool validate);

		// Returns true if valid, or false if invalid.
		// If invalid, list of validation failures should
		// be checked.
		bool Validate();

		const std::vector<ValidationFailure>& GetValidationFailures() const;

	protected:
		BaseCodeWordValidator(const std::shared_ptr<CodeWord>& codeWord);
		void AddFailure(const ValidationFailure& failure);
		void AddFailure(ValidationFailure&& failure);

		bool ValidateRegIdentifier(size_t argIndex);
		bool ValidateReservedArgIsZero(size_t argIndex);
		bool ValidateNumberForArg(size_t argIndex);

		virtual void RunValidation() = 0;

	private:
		std::shared_ptr<CodeWord> m_CodeWord;
		std::vector<ValidationFailure> m_ValidationFailures;
		bool m_ValidateLabelRefs = false;
	};
}
