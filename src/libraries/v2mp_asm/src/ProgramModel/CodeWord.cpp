#include <stdexcept>
#include "ProgramModel/CodeWord.h"
#include "Utils/ParsingUtils.h"

namespace V2MPAsm
{
	CodeWord::CodeWord(
		InstructionType instructionType,
		size_t line,
		size_t column
	) :
		m_Line(line),
		m_Column(column),
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

	size_t CodeWord::GetLine() const
	{
		return m_Line;
	}

	size_t CodeWord::GetColumn() const
	{
		return m_Column;
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

	void CodeWord::AddArgument(size_t columnIndex, int32_t value)
	{
		m_Arguments.emplace_back(columnIndex, value);
	}

	void CodeWord::AddArgument(size_t columnIndex, LabelReference::ReferenceType refType, const std::string& labelName)
	{
		m_Arguments.emplace_back(columnIndex, refType, labelName);
	}

	CodeWordArg* CodeWord::GetArgument(size_t argIndex)
	{
		return argIndex < m_Arguments.size() ? &m_Arguments[argIndex] : nullptr;
	}

	const CodeWordArg* CodeWord::GetArgument(size_t argIndex) const
	{
		return argIndex < m_Arguments.size() ? &m_Arguments[argIndex] : nullptr;
	}

	CodeWordArg& CodeWord::GetArgumentRef(size_t argIndex)
	{
		if ( argIndex >= m_Arguments.size() )
		{
			throw std::invalid_argument("Code word argument index was out of range");
		}

		return m_Arguments[argIndex];
	}

	const CodeWordArg& CodeWord::GetArgumentRef(size_t argIndex) const
	{
		if ( argIndex >= m_Arguments.size() )
		{
			throw std::invalid_argument("Code word argument index was out of range");
		}

		return m_Arguments[argIndex];
	}

	size_t CodeWord::GetArgumentColumn(size_t argIndex) const
	{
		return argIndex < m_Arguments.size() ? m_Arguments[argIndex].GetColumn() : COLUMN_NUMBER_BASE;
	}
}
