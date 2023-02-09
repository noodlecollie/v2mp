#include "ProgramModel/CodeWordValidation.h"
#include "Exceptions/AssemblerException.h"
#include "Exceptions/PublicExceptionIDs.h"
#include "ProgramModel/CodeWord.h"
#include "ProgramModel/ValidationUtils.h"
#include "ProgramModel/Validators/CodeWordValidatorFactory.h"
#include "ProgramModel/Validators/BasicCodeWordValidator.h"

namespace V2MPAsm
{
	std::vector<ValidationFailure> ValidateCodeWord(const std::shared_ptr<CodeWord>& codeWord, bool validateLabelRefs)
	{
		if ( !codeWord )
		{
			return {
				ValidationFailure(
					PublicErrorID::INTERNAL,
					0,
					"Code word was null."
				)
			};
		}

		std::unique_ptr<BasicCodeWordValidator> validator;

		try
		{
			validator = CreateValidator(codeWord);
		}
		catch ( ... )
		{
			return { ValidationFailure(PublicErrorID::INTERNAL, 0, "Could not produce validator for code word") };
		}

		validator->SetValidateLabelRefs(validateLabelRefs);
		validator->Validate();

		// This is a copy - do we want a move instead?
		return validator->GetValidationFailures();
	}
}
