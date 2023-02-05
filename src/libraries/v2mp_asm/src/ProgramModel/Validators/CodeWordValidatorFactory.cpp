#include <stdexcept>
#include "ProgramModel/CodeWord.h"
#include "ProgramModel/Validators/CodeWordValidatorFactory.h"
#include "ProgramModel/Validators/ZeroArgCodeWordValidator.h"
#include "ProgramModel/Validators/AddSubCodeWordValidator.h"

namespace V2MPAsm
{
	std::unique_ptr<BaseCodeWordValidator> CreateValidator(const std::shared_ptr<CodeWord>& codeWord)
	{
		if ( !codeWord )
		{
			throw std::invalid_argument("Expected non-null codeword when creating validator");
		}

		switch ( codeWord->GetInstructionType() )
		{
			case InstructionType::NOP:
			case InstructionType::SIG:
			{
				return std::make_unique<ZeroArgCodeWordValidator>(codeWord);
			}

			case InstructionType::ADD:
			case InstructionType::SUB:
			{
				return std::make_unique<AddSubCodeWordValidator>(codeWord);
			}

			default:
			{
				throw std::runtime_error("Unsupported codeword type when producing validator");
			}
		}
	}
}
