#include "ProgramModel/CodeWord.h"

namespace V2MPAsm
{
	CodeWord::CodeWord(InstructionType instructionType) :
		m_InstructionType(instructionType)
	{
	}

	CodeWord::CodeWord(const CodeWord& other)
	{
		*this = other;
	}

	CodeWord::CodeWord(CodeWord&& other)
	{
		*this = std::move(other);
	}

	CodeWord& CodeWord::operator =(const CodeWord& other)
	{
		m_InstructionType = other.m_InstructionType;

		return *this;
	}

	CodeWord& CodeWord::operator =(CodeWord&& other)
	{
		m_InstructionType = other.m_InstructionType;
		other.m_InstructionType = InstructionType::NOP;

		return *this;
	}

	InstructionType CodeWord::GetInstructionType() const
	{
		return m_InstructionType;
	}
}
