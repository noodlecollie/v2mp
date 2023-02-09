#pragma once

#include <memory>
#include <vector>
#include "ProgramModel/ValidationFailure.h"
#include "ProgramModel/InstructionMeta.h"

namespace V2MPAsm
{
	class CodeWord;

	class BasicCodeWordValidator
	{
	public:
		enum class ValidationResult
		{
			VALID,
			VALID_WITH_WARNINGS,
			INVALID
		};

		BasicCodeWordValidator(const std::shared_ptr<CodeWord>& codeWord);

		virtual ~BasicCodeWordValidator();

		CodeWord& GetCodeWord();
		const CodeWord& GetCodeWord() const;

		bool GetValidateLabelRefs() const;
		void SetValidateLabelRefs(bool validate);

		// Returns true if valid, or false if invalid.
		// If invalid, list of validation failures should
		// be checked.
		ValidationResult Validate();

		const std::vector<ValidationFailure>& GetValidationFailures() const;

	protected:
		enum class SignednessOverride
		{
			NO_OVERRIDE,
			FORCE_SIGNED,
			FORCE_UNSIGNED
		};

		void AddFailure(const ValidationFailure& failure);
		void AddFailure(ValidationFailure&& failure);

		bool ValidateCommonArgConstraints();

		bool ValidateRegIdentifier(size_t argIndex, uint32_t regIDMask = REG_ID_MASK);
		bool ValidateReservedArgIsZero(size_t argIndex);
		bool ValidateNumberForArg(size_t argIndex, SignednessOverride signednessOverride = SignednessOverride::NO_OVERRIDE);

		virtual void ValidateSpecific();

	private:
		std::shared_ptr<CodeWord> m_CodeWord;
		std::vector<ValidationFailure> m_ValidationFailures;
		bool m_ValidateLabelRefs = false;
	};
}
