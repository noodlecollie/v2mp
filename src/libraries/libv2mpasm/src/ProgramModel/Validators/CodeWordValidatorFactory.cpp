#include <stdexcept>
#include "ProgramModel/CodeWord.h"
#include "ProgramModel/Validators/CodeWordValidatorFactory.h"
#include "ProgramModel/Validators/BasicCodeWordValidator.h"
#include "ProgramModel/Validators/SimpleRegAndValueCodeWordValidator.h"
#include "ProgramModel/Validators/MulDivCodeWordValidator.h"
#include "ProgramModel/Validators/AsgnCodeWordValidator.h"
#include "ProgramModel/Validators/BitwCodeWordValidator.h"
#include "ProgramModel/Validators/CbxCodeWordValidator.h"
#include "ProgramModel/Validators/LdstCodeWordValidator.h"
#include "ProgramModel/Validators/StkCodeWordValidator.h"

namespace V2MPAsm
{
	std::unique_ptr<BasicCodeWordValidator> CreateValidator(const std::shared_ptr<CodeWord>& codeWord)
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
				return std::make_unique<BasicCodeWordValidator>(codeWord);
			}

			case InstructionType::ADD:
			case InstructionType::SUB:
			case InstructionType::SHFT:
			{
				return std::make_unique<SimpleRegAndValueCodeWordValidator>(codeWord);
			}

			case InstructionType::MUL:
			case InstructionType::DIV:
			{
				return std::make_unique<MulDivCodeWordValidator>(codeWord);
			}

			case InstructionType::ASGN:
			{
				return std::make_unique<AsgnCodeWordValidator>(codeWord);
			}

			case InstructionType::BITW:
			{
				return std::make_unique<BitwCodeWordValidator>(codeWord);
			}

			case InstructionType::CBX:
			{
				return std::make_unique<CbxCodeWordValidator>(codeWord);
			}

			case InstructionType::LDST:
			{
				return std::make_unique<LdstCodeWordValidator>(codeWord);
			}

			case InstructionType::STK:
			{
				return std::make_unique<StkCodeWordValidator>(codeWord);
			}

			default:
			{
				throw std::invalid_argument("Unsupported codeword type when producing validator");
			}
		}
	}
}
