#pragma once

#include "ProgramModel/InstructionMeta.h"

namespace V2MPAsm
{
	class CodeWord
	{
	private:
		InstructionType m_InstructionType = InstructionType::NOP;
	};
}
