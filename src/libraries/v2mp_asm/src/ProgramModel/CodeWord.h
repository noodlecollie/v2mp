#pragma once

#include <cstdint>
#include <vector>
#include "ProgramModel/InstructionMeta.h"

namespace V2MPAsm
{
	class CodeWord
	{
	public:
		explicit CodeWord(InstructionType instructionType = InstructionType::NOP);
		CodeWord(const CodeWord& other);
		CodeWord(CodeWord&& other);

		CodeWord& operator =(const CodeWord& other);
		CodeWord& operator =(CodeWord&& other);

		InstructionType GetInstructionType() const;

		std::vector<uint16_t>& GetArguments();
		const std::vector<uint16_t>& GetArguments() const;

	private:
		InstructionType m_InstructionType = InstructionType::NOP;
		std::vector<uint16_t> m_Arguments;
	};
}
