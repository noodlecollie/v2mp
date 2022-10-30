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

	size_t CodeWord::GetArgumentCount() const
	{
		return m_Arguments.size();
	}

	uint16_t CodeWord::GetAddress() const
	{
		return m_Address;
	}

	void CodeWord::SetAddress(uint16_t address)
	{
		m_Address = address;
	}

	void CodeWord::AddArgument(int32_t value)
	{
		m_Arguments.emplace_back(value);
	}

	void CodeWord::AddArgument(LabelReference::ReferenceType refType, const std::string& labelName)
	{
		m_Arguments.emplace_back(refType, labelName);
	}
}
